// bslstl_unorderedmultiset.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMULTISET
#define INCLUDED_BSLSTL_UNORDEREDMULTISET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'unordered_multiset' container.
//
//@CLASSES:
//   bsl::unordered_multiset : STL-compliant 'unordered_multiset' container
//
//@CANONICAL_HEADER: bsl_unordered_set.h
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::unordered_multiset', implementing the standard container holding a
// collection of possibly duplicate keys with no guarantees on ordering (unless
// keys have the same value).
//
// An instantiation of 'unordered_multiset' is an allocator-aware,
// value-semantic type whose salient attributes are its size (number of keys)
// and the set of keys the 'unordered_multiset' contains, without regard to
// their order.  If 'unordered_multiset' is instantiated with a key type that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.  It is possible to instantiate
// 'unordered_multiset' with a key type that does not have an accessible
// copy-constructor, in which case the 'unordered_multiset' will not be
// copyable.  Note that the equality operator for each element is used to
// determine when two 'unordered_multiset' objects have the same value, and not
// the equality comparator supplied at construction.
//
// An 'unordered_multiset' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_multiset' implemented here adheres to the C++11 standard, except
// that it may rehash when setting the 'max_load_factor' in order to preserve
// the property that the value is always respected (which is a potentially
// throwing operation).
//
///Requirements on 'KEY'
///---------------------
// An 'unordered_multiset' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' template parameter is fully
// value-semantic.  It is possible to instantiate an 'unordered_multiset' with
// a 'KEY' parameter argument that does not provide a full set of
// value-semantic operations, but then some methods of the container may not be
// instantiable.  The following terminology, adopted from the C++11 standard,
// is used in the function documentation of 'unordered_multiset' to describe a
// function's requirements for the 'KEY' template parameter.  These terms are
// also defined in section [utility.arg.requirements] of the C++11 standard.
// Note that, in the context of an 'unordered_multiset' instantiation, the
// requirements apply specifically to the 'unordered_multiset's element type,
// 'value_type', which is an alias for 'KEY'.
//
// Legend
// ------
// 'X'    - denotes an allocator-aware container type ('unordered_multiset')
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
// The type supplied as an unordered multiset's 'ALLOCATOR' template parameter
// determines how that unordered multiset will allocate memory.  The
// 'unordered_multiset' template supports allocators meeting the requirements
// of the C++11 standard [allocator.requirements], and in addition it supports
// scoped-allocators derived from the 'bslma::Allocator' memory allocation
// protocol.  Clients intending to use 'bslma'-style allocators should use the
// template's default 'ALLOCATOR' type.  The default type for the 'ALLOCATOR'
// template parameter, 'bsl::allocator', provides a C++11 standard-compatible
// adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'unordered_multiset'
// instantiation is 'bsl::allocator', then objects of that unordered multiset
// type will conform to the standard behavior of a 'bslma'-allocator-enabled
// type.  Such an unordered multiset accepts an optional 'bslma::Allocator'
// argument at construction.  If the address of a 'bslma::Allocator' object is
// explicitly supplied at construction, it will be used to supply memory for
// the 'unordered_multiset' throughout its lifetime; otherwise, the
// 'unordered_multiset' will use the default allocator installed at the time of
// the 'unordered_multiset's construction (see 'bslma_default').  In addition
// to directly allocating memory from the indicated 'bslma::Allocator', an
// 'unordered_multiset' supplies that allocator's address to the constructors
// of contained objects of the (template parameter) type 'KEY' with the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unordered_multiset':
//..
//  Legend
//  ------
//  'K'             - (template parameter) type 'KEY' of the unordered multiset
//  'a', 'b'        - two distinct objects of type 'unordered_multiset<K>'
//  'rv'            - modifiable rvalue of type 'unordered_multiset<K>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - unordered_multiset<K>::value_type
//  'hf'            - hash function for objects of type 'K'
//  'eq'            - equality comparator for objects of type 'K'
//  'al'            - STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'li'            - object of type 'initializer_list<K>'
//  'k'             - object of type 'K'
//  'rk'            - modifiable rvalue of type 'K'
//  'v'             - object of type 'value_type'
//  'p1', 'p2'      - two 'const_iterator's belonging to 'a'
//  distance(i1,i2) - number of elements in the range '[i1 .. i2)'
//  distance(p1,p2) - number of elements in the range '[p1 .. p2)'
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | unordered_multiset<K> a;     (default construction)| O[1]               |
//  | unordered_multiset<K> a(al);                       |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multiset<K> a(b);  (copy construction)   | Average: O[n]      |
//  | unordered_multiset<K> a(b, al);                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multiset<K> a(rv); (move construction)   | O[1] if 'a' and    |
//  | unordered_multiset<K> a(rv, al);                   | 'rv' use the same  |
//  |                                                    | allocator;         |
//  |                                                    | otherwise,         |
//  |                                                    | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multiset<K> a(li);                       | Average: O[N]      |
//  | unordered_multiset<K> a(li, al);                   | Worst:   O[N^2]    |
//  | unordered_multiset<K> a(li, w, al);                | where N =          |
//  | unordered_multiset<K> a(li, w, hf, al);            |         'li.size()'|
//  | unordered_multiset<K> a(li, w, hf, eq, al);        |                    |
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
//  | a.~unordered_multiset<K>();  (destruction)         | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;                       (copy assignment)     | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a = rv;                      (move assignment)     | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator;         |
//  |                                                    | otherwise,         |
//  |                                                    | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a = li;                                            | Average: O[N]      |
//  |                                                    | Worst:   O[N^2]    |
//  |                                                    | where N =          |
//  |                                                    |         'li.size()'|
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1]               |
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
//  | a.insert(li);                                      | Average: O[N]      |
//  |                                                    | Worst:   O[n * N]  |
//  |                                                    | where N =          |
//  |                                                    |         'li.size()'|
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
//  | a.contains(k)                                      | Average: O[1]      |
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
// element in the unordered multiset, unless it also erases that element, such
// as any 'erase' overload, 'clear', or the destructor (that erases all
// elements).  Pointers and references are stable through a rehash.
//
// Iterators to elements in the container are invalidated by any rehash, so
// iterators may be invalidated by an 'insert' or 'emplace' call if it triggers
// a rehash (but not otherwise).  Iterators to specific elements are also
// invalidated when that element is erased.  Note that the 'end' iterator is
// not an iterator referring to any element in the container, so may be
// invalidated by any non-'const' method.
//
///Unordered Multiset Configuration
///---------------------------------
// The unordered multiset has interfaces that can provide insight into and
// control of its inner workings.  The syntax and semantics of these interfaces
// for 'bslstl_unorderedmultiset' are identical to those of
// 'bslstl_unorderedmap'.  See the discussion in
// {'bslstl_unorderedmap'|Unordered Map Configuration} and the illustrative
// material in {'bslstl_unorderedmap'|Example 2}.
//
///Practical Requirements on 'HASH'
///--------------------------------
// An important factor in the performance of an unordered multiset (and any of
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
// One uses a multiset (ordered or unordered) when there may be more than one
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
// nothing is lost by using an unordered multiset instead of an ordered
// multiset:
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
// Next, we define the type of the unordered multiset and a convenience
// aliases:
//..
//  typedef bsl::unordered_multiset<CustomerDatum,
//                                  CustomerDatumHash,
//                                  CustomerDatumEqual> DataByProfile;
//  typedef DataByProfile::const_iterator               DataByProfileConstItr;
//..
// Now, create a helper function to calculate the average financials for a
// category of customer profiles within the unordered multiset.
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
// Then, we create an unordered multiset and insert each item of 'data'.
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

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_hashtable.h>
#include <bslstl_hashtablebucketiterator.h>
#include <bslstl_hashtableiterator.h>
#include <bslstl_iteratorutil.h>
#include <bslstl_pair.h>  // result type of 'equal_range' method
#include <bslstl_unorderedsetkeyconfiguration.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionalnode.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslma_allocatortraits.h>
#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>        // Can probably escape with a fwd-decl,
                                       // but not very user friendly
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <cstddef>  // for 'std::size_t'

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>  // 'std::is_nothrow_move_assignable'
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_unorderedmultiset.h
# define COMPILING_BSLSTL_UNORDEREDMULTISET_H
# include <bslstl_unorderedmultiset_cpp03.h>
# undef COMPILING_BSLSTL_UNORDEREDMULTISET_H
#else

namespace bsl {

                        // ========================
                        // class unordered_multiset
                        // ========================

template <class KEY,
          class HASH      = bsl::hash<KEY>,
          class EQUAL     = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<KEY> >
class unordered_multiset
{
    // This class template implements a value-semantic container type holding
    // an unordered multiset of values (of template parameter type 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for BDEX serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

  private:

    // PRIVATE TYPE
    typedef bsl::allocator_traits<ALLOCATOR>                 AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef KEY                                              ValueType;
        // This typedef is an alias for the type of values maintained by this
        // unordered multiset.

    typedef ::BloombergLP::bslstl::UnorderedSetKeyConfiguration<ValueType>
                                                             ListConfiguration;
        // This typedef is an alias for the policy used internally by this
        // container to extract the 'KEY' value from the values maintained by
        // this unordered multiset.

    typedef ::BloombergLP::bslstl::HashTable<ListConfiguration,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR>      HashTable;
        // This typedef is an alias for the template instantiation of the
        // underlying 'bslstl::HashTable' used to implement this unordered
        // multiset.

    typedef ::BloombergLP::bslalg::BidirectionalLink         HashTableLink;
        // This typedef is an alias for the type of links maintained by the
        // linked list of elements held by the underlying 'bslstl::HashTable'.

    typedef BloombergLP::bslmf::MovableRefUtil               MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    // FRIENDS
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
                    unordered_multiset,
                    ::BloombergLP::bslmf::IsBitwiseMoveable,
                    ::BloombergLP::bslmf::IsBitwiseMoveable<HashTable>::value);

  private:
    // DATA
    HashTable d_impl;

  public:
    // CREATORS
    unordered_multiset();
    explicit unordered_multiset(size_type        initialNumBuckets,
                                const HASH&      hashFunction = HASH(),
                                const EQUAL&     keyEqual = EQUAL(),
                                const ALLOCATOR& basicAllocator = ALLOCATOR());
    unordered_multiset(size_type        initialNumBuckets,
                       const HASH&      hashFunction,
                       const ALLOCATOR& basicAllocator);
    unordered_multiset(size_type        initialNumBuckets,
                       const ALLOCATOR& basicAllocator);
    explicit unordered_multiset(const ALLOCATOR& basicAllocator);
        // Create an empty unordered multiset.  Optionally specify an
        // 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // a single bucket is used.  Optionally specify a 'hashFunction' used
        // to generate the hash values for the keys contained in this unordered
        // multiset.  If 'hashFunction' is not supplied, a default-constructed
        // object of the (template parameter) type 'HASH' is used.  Optionally
        // specify a key-equality functor 'keyEqual' used to verify that two
        // keys are equivalent.  If 'keyEqual' is not supplied, a
        // default-constructed object of the (template parameter) type 'EQUAL'
        // is used.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not supplied, a default-constructed
        // object of the (template parameter) type 'ALLOCATOR' is used.  If the
        // type 'ALLOCATOR' is 'bsl::allocator' (the default), then
        // 'basicAllocator', if supplied, shall be convertible to
        // 'bslma::Allocator *'.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used.

    unordered_multiset(const unordered_multiset& original);
        // Create an unordered multiset having the same value as the specified
        // 'original' object.  Use a copy of 'original.hash_function()' to
        // generate hash values for the keys contained in this unordered
        // multiset.  Use a copy of 'original.key_eq()' to verify that two keys
        // are equivalent.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'KEY' be 'copy-insertable' into this unordered multiset (see
        // {Requirements on 'KEY'}).

    unordered_multiset(
                  BloombergLP::bslmf::MovableRef<unordered_multiset> original);
        // Create an unordered multiset having the same value as the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new unordered multiset.  Use a copy of
        // 'original.hash_function()' to generate hash values for the keys
        // contained in this unordered multiset.  Use a copy of
        // 'original.key_eq()' to verify that two keys are equivalent.  The
        // allocator associated with 'original' is propagated for use in the
        // newly-created unordered multiset.  'original' is left in a valid but
        // unspecified state.

    unordered_multiset(
                const unordered_multiset&                      original,
                const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create an unordered multiset having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  Use a copy of 'original.hash_function()' to generate hash
        // values for the keys contained in this unordered multiset.  Use a
        // copy of 'original.key_eq()' to verify that two keys are equivalent.
        // This method requires that the (template parameter) type 'KEY' be
        // 'copy-insertable' into this unordered multiset (see {Requirements on
        // 'KEY'}).  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    unordered_multiset(
            BloombergLP::bslmf::MovableRef<unordered_multiset> original,
            const typename type_identity<ALLOCATOR>::type&     basicAllocator);
        // Create an unordered multiset having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  The contents of 'original' are moved (in constant time) to
        // the new unordered multiset if 'basicAllocator ==
        // original.get_allocator()', and are move-inserted (in linear time)
        // using 'basicAllocator' otherwise.  'original' is left in a valid but
        // unspecified state.  Use a copy of 'original.hash_function()' to
        // generate hash values for the keys contained in this unordered
        // multiset.  Use a copy of 'original.key_eq()' to verify that two keys
        // are equivalent.  This method requires that the (template parameter)
        // type 'KEY' be 'move-insertable' into this unordered multiset (see
        // {Requirements on 'KEY'}).  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the (template parameter) type
        // 'ALLOCATOR' is 'bsl::allocator' (the default).

    template <class INPUT_ITERATOR>
    unordered_multiset(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       size_type        initialNumBuckets = 0,
                       const HASH&      hashFunction = HASH(),
                       const EQUAL&     keyEqual = EQUAL(),
                       const ALLOCATOR& basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    unordered_multiset(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       size_type        initialNumBuckets,
                       const HASH&      hashFunction,
                       const ALLOCATOR& basicAllocator);
    template <class INPUT_ITERATOR>
    unordered_multiset(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       size_type        initialNumBuckets,
                       const ALLOCATOR& basicAllocator);
    template <class INPUT_ITERATOR>
    unordered_multiset(INPUT_ITERATOR   first,
                       INPUT_ITERATOR   last,
                       const ALLOCATOR& basicAllocator);
        // Create an unordered multiset, and insert each 'value_type' object in
        // the sequence starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // an 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // a single bucket is used.  Optionally specify a 'hashFunction' used
        // to generate hash values for the keys contained in this unordered
        // multiset.  If 'hashFunction' is not supplied, a default-constructed
        // object of (template parameter) type 'HASH' is used.  Optionally
        // specify a key-equality functor 'keyEqual' used to verify that two
        // keys are equivalent.  If 'keyEqual' is not supplied, a
        // default-constructed object of (template parameter) type 'EQUAL' is
        // used.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.  The (template parameter) type 'INPUT_ITERATOR' shall
        // meet the requirements of an input iterator defined in the C++11
        // standard [24.2.3] providing access to values of a type convertible
        // to 'value_type', and 'value_type' must be 'emplace-constructible'
        // from '*i' into this unordered multiset, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'KEY'}).  The behavior is undefined unless 'first'
        // and 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that a 'bslma::Allocator *' can
        // be supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<
                         std::is_invocable_v<EQUAL, const KEY &, const KEY &>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
# endif
    unordered_multiset(
                      std::initializer_list<KEY> values,
                      size_type                  initialNumBuckets = 0,
                      const HASH&                hashFunction = HASH(),
                      const EQUAL&               keyEqual = EQUAL(),
                      const ALLOCATOR&           basicAllocator = ALLOCATOR());
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator<ALLOCATOR>::value>
    >
# endif
    unordered_multiset(std::initializer_list<KEY> values,
                       size_type                  initialNumBuckets,
                       const HASH&                hashFunction,
                       const ALLOCATOR&           basicAllocator);
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <class = bsl::enable_if_t<bsl::IsStdAllocator<ALLOCATOR>::value>>
# endif
    unordered_multiset(std::initializer_list<KEY> values,
                       size_type                  initialNumBuckets,
                       const ALLOCATOR&           basicAllocator);
# ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
    template <class = bsl::enable_if_t<bsl::IsStdAllocator<ALLOCATOR>::value>>
# endif
    unordered_multiset(std::initializer_list<KEY> values,
                       const ALLOCATOR&           basicAllocator);
        // Create an unordered multiset and insert each 'value_type' object in
        // the specified 'values' initializer list.  Optionally specify an
        // 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // a single bucket is used.  Optionally specify a 'hashFunction' used
        // to generate the hash values for the keys contained in this unordered
        // multiset.  If 'hashFunction' is not supplied, a default-constructed
        // object of the (template parameter) type 'HASH' is used.  Optionally
        // specify a key-equality functor 'keyEqual' used to verify that two
        // keys are equivalent.  If 'keyEqual' is not supplied, a
        // default-constructed object of the (template parameter) type 'EQUAL'
        // is used.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not supplied, a default-constructed
        // object of the (template parameter) type 'ALLOCATOR' is used.  If the
        // type 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.  This method requires that the (template parameter)
        // type 'KEY' be 'copy-insertable' into this unordered multiset (see
        // {Requirements on 'KEY'}).  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).
#endif

    ~unordered_multiset();
        // Destroy this object.

    // MANIPULATORS
    unordered_multiset& operator=(const unordered_multiset& rhs);
        // Assign to this object the value, hash function, and equality
        // comparator of the specified 'rhs' object, propagate to this object
        // the allocator of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  This
        // method requires that the (template parameter) type 'KEY' be both
        // 'copy-assignable' and 'copy-insertable" into this unordered multiset
        // (see {Requirements on 'KEY'}).

    unordered_multiset&
    operator=(BloombergLP::bslmf::MovableRef<unordered_multiset> rhs)
                            BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                AllocatorTraits::is_always_equal::value
                             && std::is_nothrow_move_assignable<HASH>::value
                             && std::is_nothrow_move_assignable<EQUAL>::value);
        // Assign to this object the value, hash function, and equality
        // comparator of the specified 'rhs' object, propagate to this object
        // the allocator of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_move_assignment', and return a reference
        // providing modifiable access to this object.  The contents of 'rhs'
        // are moved (in constant time) to this unordered multiset if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this unordered
        // multiset are either destroyed or move-assigned to and each
        // additional element in 'rhs' is move-inserted into this unordered
        // multiset.  'rhs' is left in a valid but unspecified state, and if an
        // exception is thrown, '*this' is left in a valid but unspecified
        // state.  This method requires that the (template parameter) type
        // 'KEY' be both 'move-assignable' and 'move-insertable' into this
        // unordered multiset (see {Requirements on 'KEY'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    unordered_multiset& operator=(std::initializer_list<KEY> values);
        // Assign to this object the value resulting from first clearing this
        // unordered multiset and then inserting each 'value_type' object in
        // the specified 'values' initializer list, and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter) type 'KEY' be 'copy-insertable' into
        // this unordered multiset (see {Requirements on 'KEY'}).
#endif

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this unordered multiset, or the 'end' iterator if this
        // unordered multiset is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // unordered multiset.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects of the
        // bucket having the specified 'index', in the array of buckets
        // maintained by this unordered multiset, or the 'end(index)'
        // otherwise.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index', in the array of buckets
        // maintained by this unordered multiset.

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all entries from this unordered multiset.  Note that the
        // container is empty after this call, but allocated memory may be
        // retained for future use.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      pair<iterator, iterator> >::type
    equal_range(const LOOKUP_KEY& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered multiset
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multiset contains no 'value_type' objects equivalent to the 'key',
        // then the two returned iterators will have the same value.  The
        // behavior is undefined unless 'key' is equivalent to the elements of
        // at most one equivalent-key group in this unordered multiset.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            typedef bsl::pair<iterator, iterator> ResultType;
            HashTableLink *first;
            HashTableLink *last;
            d_impl.findRange(&first, &last, key);
            return ResultType(iterator(first), iterator(last));
        }

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this unordered multiset
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multiset contains no 'value_type' objects equivalent to the 'key',
        // then the two returned iterators will have the same value.

    size_type erase(const key_type& key);
        // Remove from this unordered multiset all 'value_type' objects that
        // are equivalent to the specified 'key', if they exist, and return the
        // number of object erased; otherwise, if there are no 'value_type'
        // objects equivalent to 'key', return 0 with no other effect.  This
        // method invalidates only iterators and references to the removed
        // element and previously saved values of the 'end()' iterator, and
        // preserves the relative order of the elements not removed.

    iterator erase(const_iterator position);
        // Remove from this unordered multiset the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this unordered multiset.
        // This method invalidates only iterators and references to the removed
        // element and previously saved values of the 'end()' iterator, and
        // preserves the relative order of the elements not removed.  The
        // behavior is undefined unless 'position' refers to a 'value_type'
        // object in this unordered multiset.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from unordered multiset the 'value_type' objects starting at
        // the specified 'first' position up to, but not including the
        // specified 'last' position, and return 'last'.  This method
        // invalidates only iterators and references to the removed element and
        // previously saved values of the 'end()' iterator, and preserves the
        // relative order of the elements not removed.  The behavior is
        // undefined unless 'first' and 'last' either refer to elements in this
        // unordered multiset or are the 'end' iterator, and the 'first'
        // position is at or before the 'last' position in the sequence
        // provided by this container.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      iterator>::type
    find(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of all the value elements of
        // this unordered multiset equivalent to the specified 'key', if such
        // entries exist, and the past-the-end ('end') iterator otherwise.  The
        // behavior is undefined unless 'key' is equivalent to the elements of
        // at most one equivalent-key group in this unordered multiset.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            return iterator(d_impl.find(key));
        }

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of all the value elements of
        // this unordered multiset equivalent to the specified 'key', if such
        // entries exist, and the past-the-end ('end') iterator otherwise.

    iterator insert(const value_type& value);
        // Insert the specified 'value' into this unordered multiset.  If one
        // or more keys equivalent to 'value' already exist in this unordered
        // multiset, this method is guaranteed to insert 'value' in a position
        // contiguous to one of those equivalent keys.  Return an iterator
        // referring to the newly inserted 'value_type' object that is
        // equivalent to 'value.  Note that this method requires that the
        // (template parameter) type 'KEY' be 'copy-insertable' into this
        // unordered multiset (see {Requirements on 'KEY'}).

    iterator insert(BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this unordered multiset.  If one
        // or more keys equivalent to 'value' already exist in this unordered
        // multiset, this method is guaranteed to insert 'value' in a position
        // contiguous to one of those equivalent keys.  Return an iterator
        // referring to the newly inserted 'value_type' object that is
        // equivalent to 'value'.  This method requires that the (template
        // parameter) type 'KEY' be 'move-insertable' into this unordered
        // multiset (see {Requirements on 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this unordered multiset (in
        // constant time if the specified 'hint' refers to an element in this
        // container equivalent to 'value').  If one or more keys equivalent to
        // 'value' already exist in this unordered multiset, this method is
        // guaranteed to insert 'value' in a position contiguous to one of
        // those equivalent keys.  Return an iterator referring to the newly
        // inserted 'value_type' object that is equivalent to 'value'.  If
        // 'hint' does not refer to an element in this container equivalent to
        // 'value', this operation has worst case 'O[N]' and average case
        // constant-time complexity, where 'N' is the size of this unordered
        // multiset.  This method requires that the (template parameter) type
        // 'KEY' be 'copy-insertable' into this unordered multiset (see
        // {Requirements on 'KEY'}).  The behavior is undefined unless 'hint'
        // is an iterator in the range '[begin() .. end()]' (both endpoints
        // included).

    iterator insert(const_iterator                             hint,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this unordered multiset (in
        // constant time if the specified 'hint' refers to an element in this
        // container equivalent to 'value').  If one or more keys equivalent to
        // 'value' already exist in this unordered multiset, this method is
        // guaranteed to insert 'value' in a position contiguous to one of
        // those equivalent keys.  Return an iterator referring to the newly
        // inserted 'value_type' object that is equivalent to 'value'.  If
        // 'hint' does not refer to an element in this container equivalent to
        // 'value', this operation has worst case 'O[N]' and average case
        // constant-time complexity, where 'N' is the size of this unordered
        // multiset.  This method requires that the (template parameter) type
        // 'KEY' be 'move-insertable' into this unordered multiset (see
        // {Requirements on 'KEY'}).  The behavior is undefined unless 'hint'
        // is an iterator in the range '[begin() .. end()]' (both endpoints
        // included).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this unordered multiset the value of each 'value_type'
        // object in the range starting at the specified 'first' iterator and
        // ending immediately before the specified 'last' iterator.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type', and 'value_type' must be 'emplace-constructible' from
        // '*i' into this unordered multiset, where 'i' is a dereferenceable
        // iterator in the range '[first .. last)' (see {Requirements on
        // 'KEY'}).  The behavior is undefined unless 'first' and 'last' refer
        // to a sequence of valid values where 'first' is at a position at or
        // before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<KEY> values);
        // Insert into this unordered multiset the value of each 'value_type'
        // object in the specified 'values' initializer list.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'copy-insertable' into this unordered multiset (see {Requirements on
        // 'KEY'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    iterator emplace(Args&&... args);
        // Insert into this unordered multiset a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type'.  Return an iterator referring to the
        // newly created and inserted object in this unordered multiset whose
        // value is equivalent to that of an object constructed from 'args'.
        // This method requires that the (template parameter) type 'KEY' be
        // 'emplace-constructible' into this unordered multiset from 'args'
        // (see {Requirements on 'KEY'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);
        // Insert into this unordered multiset a newly created 'value_type'
        // object, constructed by forwarding 'get_allocator()' (if required)
        // and the specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type' (in constant time if the specified
        // 'hint' refers to an element in this container equivalent to the
        // newly created 'value_type' object).  Return an iterator referring to
        // the newly created and inserted object in this unordered multiset
        // whose value is equivalent to that of an object constructed from
        // 'args'.  If 'hint' does not refer to an element in this container
        // equivalent to the newly created 'value_type' object, this operation
        // has worst case 'O[N]' and average case constant-time complexity,
        // where 'N' is the size of this unordered multiset.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'emplace-constructible' into this unordered multiset from 'args'
        // (see {Requirements on 'KEY'}).  The behavior is undefined unless
        // 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).

#endif

    void max_load_factor(float newLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to at least the specified 'numBuckets', and redistribute all the
        // contained elements into the new sequence of buckets, according to
        // their hash values.  Note that this operation has no effect if
        // rehashing the elements into 'numBuckets' would cause this unordered
        // multiset to exceed its 'max_load_factor'.

    void reserve(size_type numElements);
        // Increase the number of buckets of this unordered multiset to a
        // quantity such that the ratio between the specified 'numElements' and
        // this quantity does not exceed 'max_load_factor'.  Note that this
        // guarantees that, after the reserve, elements can be inserted to grow
        // the container to 'size() == numElements' without rehashing.  Also
        // note that memory allocations may still occur when growing the
        // container to 'size() == numElements'.  Also note that this operation
        // has no effect if 'numElements <= size()'.

    void swap(unordered_multiset& other)
                                 BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                      AllocatorTraits::is_always_equal::value
                                  &&  bsl::is_nothrow_swappable<HASH>::value
                                  &&  bsl::is_nothrow_swappable<EQUAL>::value);
        // Exchange the value, hasher, key-equality functor, and
        // 'max_load_factor' of this object with those of the specified 'other'
        // object; also exchange the allocator of this object with that of
        // 'other' if the (template parameter) type 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait, and do not modify either
        // allocator otherwise.  This method provides the no-throw
        // exception-safety guarantee if and only if both the (template
        // parameter) types 'HASH' and 'EQUAL' provide no-throw swap
        // operations; if an exception is thrown, both objects are left in
        // valid but unspecified states.  This operation guarantees 'O[1]'
        // complexity.  The behavior is undefined unless either this object was
        // created with the same allocator as 'other' or 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait.

    // ACCESSORS
    ALLOCATOR get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // unordered multiset.

    const_iterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // maintained by this unordered multiset, or the 'end' iterator if this
        // unordered multiset is empty.

    const_iterator  end() const BSLS_KEYWORD_NOEXCEPT;
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects
        // maintained by this unordered multiset.

    bool contains(const key_type &key) const;
        // Return 'true' if this unordered multiset contains an element whose
        // key is equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value &&
            BloombergLP::bslmf::IsTransparentPredicate<EQUAL,
                                                       LOOKUP_KEY>::value,
        bool>::type
    contains(const LOOKUP_KEY& key) const
        // Return 'true' if this unordered multiset contains an element whose
        // key is equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error
    {
        return find(key) != end();
    }

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this unordered multiset contains no elements, and
        // 'false' otherwise.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this unordered multiset.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this unordered multiset could possibly hold.  Note that there
        // is no guarantee that the unordered multiset can successfully grow to
        // the returned size, or even close to that size without running out of
        // resources.

    EQUAL key_eq() const;
        // Return (a copy of) the key-equality binary functor that returns
        // 'true' if the value of two 'key_type' objects are equivalent, and
        // 'false' otherwise.

    HASH hash_function() const;
        // Return (a copy of) the hash unary functor used by this unordered
        // multiset to generate a hash value (of type 'size_t') for a
        // 'key_type' object.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      const_iterator>::type
    find(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of all the value elements of
        // this unordered multiset equivalent to the specified 'key', if such
        // entries exist, and the past-the-end ('end') iterator otherwise.  The
        // behavior is undefined unless 'key' is equivalent to the elements of
        // at most one equivalent-key group in this unordered multiset.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            return const_iterator(d_impl.find(key));
        }

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the sequence of all the value elements of
        // this unordered multiset equivalent to the specified 'key', if such
        // entries exist, and the past-the-end ('end') iterator otherwise.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      size_type>::type
    count(const LOOKUP_KEY& key) const
        // Return the number of 'value_type' objects within this unordered
        // multiset that are equivalent to the specified 'key'.  The behavior
        // is undefined unless 'key' is equivalent to the elements of at most
        // one equivalent-key group in this unordered multiset.
        //
        // Note: implemented inline due to Sun CC compilation error.
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

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this unordered
        // multiset that are equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename enable_if<
           BloombergLP::bslmf::IsTransparentPredicate<HASH, LOOKUP_KEY>::value
        && BloombergLP::bslmf::IsTransparentPredicate<EQUAL,LOOKUP_KEY>::value,
                      pair<const_iterator, const_iterator> >::type
    equal_range(const LOOKUP_KEY& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this unordered multiset
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multiset contains no 'value_type' objects equivalent to the 'key',
        // then the two returned iterators will have the same value.  The
        // behavior is undefined unless 'key' is equivalent to the elements of
        // at most one equivalent-key group in this unordered multiset.
        //
        // Note: implemented inline due to Sun CC compilation error.
        {
            typedef bsl::pair<const_iterator, const_iterator> ResultType;
            HashTableLink *first;
            HashTableLink *last;
            d_impl.findRange(&first, &last, key);
            return ResultType(const_iterator(first), const_iterator(last));
        }


    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this unordered multiset
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multiset contains no 'value_type' objects equivalent to the 'key',
        // then the two returned iterators will have the same value.

    const_local_iterator begin(size_type index) const;
    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered multiset, or the 'end(index)'
        // otherwise.  The behavior is undefined unless 'index <
        // bucket_count()'.

    const_local_iterator end(size_type index) const;
    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this unordered multiset.  The behavior is undefined
        // unless 'index < bucket_count()'.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where a value equivalent to the specified 'key' would be
        // inserted.

    size_type bucket_count() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of buckets in the array of buckets maintained by
        // this unordered multiset.

    size_type max_bucket_count() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the unordered multiset can successfully grow to the
        // returned size, or even close to that size without running out of
        // resources.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.  The behavior is undefined unless 'index <
        // bucket_count()'.


    float load_factor() const BSLS_KEYWORD_NOEXCEPT;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The 'load_factor' is a measure of how full
        // the container is, and a higher load factor leads to an increased
        // number of collisions, thus resulting in a loss performance.

    float max_load_factor() const BSLS_KEYWORD_NOEXCEPT;
        // Return the maximum load factor allowed for this container.  If an
        // insert operation would cause 'load_factor' to exceed the
        // 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets the (see rehash).

};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class HASH = bsl::hash<KEY>,
    class EQUAL = bsl::equal_to<KEY>,
    class ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY&>>,
    class = bsl::enable_if_t<
                           std::is_invocable_v<EQUAL, const KEY&, const KEY&>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(INPUT_ITERATOR,
                   INPUT_ITERATOR,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type = 0,
                   HASH      = HASH(),
                   EQUAL     = EQUAL(),
                   ALLOCATOR = ALLOCATOR())
-> unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.  Deduce
    // the template parameters 'HASH', 'EQUAL' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.  This deduction guide does not
    // participate unless: (1) the supplied 'HASH' is invocable with a 'KEY',
    // (2) the supplied 'EQUAL' is invocable with two 'KEY's, and (3) the
    // supplied allocator meets the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class HASH,
    class EQUAL,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(
    INPUT_ITERATOR,
    INPUT_ITERATOR,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    HASH,
    EQUAL,
    ALLOC *)
-> unordered_multiset<KEY, HASH, EQUAL>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.  Deduce
    // the template parameters 'HASH' and 'EQUAL' from the other parameters
    // passed to the constructor.  This deduction guide does not participate
    // unless the supplied allocator is convertible to 'bsl::allocator<KEY>'.

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class HASH,
    class ALLOCATOR,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(INPUT_ITERATOR,
                   INPUT_ITERATOR,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   HASH,
                   ALLOCATOR)
-> unordered_multiset<KEY, HASH, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.  Deduce
    // the template parameters 'HASH' and 'ALLOCATOR' from the other parameters
    // passed to the constructor.  This deduction guide does not participate
    // unless the supplied 'HASH' is invocable with a 'KEY', and the supplied
    // allocator meets the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class HASH,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(
    INPUT_ITERATOR,
    INPUT_ITERATOR,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    HASH,
    ALLOC *)
-> unordered_multiset<KEY, HASH>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.  Deduce
    // the template parameter 'HASH' from the other parameters passed to the
    // constructor.  This deduction guide does not participate unless the
    // supplied allocator is convertible to 'bsl::allocator<KEY>'.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(INPUT_ITERATOR,
                   INPUT_ITERATOR,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   ALLOCATOR)
-> unordered_multiset<KEY, bsl::hash<KEY>, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.   This
    // deduction guide does not participate unless the supplied allocator meets
    // the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(
    INPUT_ITERATOR,
    INPUT_ITERATOR,
    typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
    ALLOC *)
-> unordered_multiset<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<KEY>'.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR)
-> unordered_multiset<KEY, bsl::hash<KEY>, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.   This
    // deduction guide does not participate unless the supplied allocator meets
    // the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> unordered_multiset<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'unordered_multiset'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<KEY>'.

template <
    class KEY,
    class HASH = bsl::hash<KEY>,
    class EQUAL = bsl::equal_to<KEY>,
    class ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY&>>,
    class = bsl::enable_if_t<
                           std::is_invocable_v<EQUAL, const KEY&, const KEY&>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(std::initializer_list<KEY>,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type = 0,
                   HASH      = HASH(),
                   EQUAL     = EQUAL(),
                   ALLOCATOR = ALLOCATOR())
-> unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // Deduce the template parameters 'HASH', EQUAL and 'ALLOCATOR' from the
    // other parameters passed to the constructor.  This deduction guide does
    // not participate unless: (1) the supplied 'HASH' is invocable with a
    // 'KEY', (2) the supplied 'EQUAL' is invocable with two 'KEY's, and (3)
    // the supplied allocator meets the requirements of a standard allocator.

template <
    class KEY,
    class HASH,
    class EQUAL,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(
                  std::initializer_list<KEY>,
                  typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
                  HASH,
                  EQUAL,
                  ALLOC *)
-> unordered_multiset<KEY, HASH, EQUAL>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // Deduce the template parameters 'HASH' and 'EQUAL' from the other
    // parameters passed to the constructor.  This deduction guide does not
    // participate unless the supplied allocator is convertible to
    // 'bsl::allocator<KEY>'.

template <
    class KEY,
    class HASH,
    class ALLOCATOR,
    class = bsl::enable_if_t<std::is_invocable_v<HASH, const KEY &>>,
    class = bsl::enable_if_t< bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(std::initializer_list<KEY>,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   HASH,
                   ALLOCATOR)
-> unordered_multiset<KEY, HASH, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // Deduce the template parameters 'HASH' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.  This deduction guide does not
    // participate unless the supplied 'HASH' is invocable with a 'KEY', and
    // the supplied allocator meets the requirements of a standard allocator.


template <
    class KEY,
    class HASH,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(
                  std::initializer_list<KEY>,
                  typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
                  HASH,
                  ALLOC *)
-> unordered_multiset<KEY, HASH>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // Deduce the template parameter 'HASH' from the other parameters passed to
    // the constructor.  This deduction guide does not participate unless the
    // supplied allocator is convertible to 'bsl::allocator<KEY>'.

template <
    class KEY,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(std::initializer_list<KEY>,
                   typename bsl::allocator_traits<ALLOCATOR>::size_type,
                   ALLOCATOR)
-> unordered_multiset<KEY, bsl::hash<KEY>, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // This deduction guide does not participate unless the supplied allocator
    // meets the requirements of a standard allocator.

template <
    class KEY,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(
                  std::initializer_list<KEY>,
                  typename bsl::allocator_traits<DEFAULT_ALLOCATOR>::size_type,
                  ALLOC *)
-> unordered_multiset<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // This deduction guide does not participate unless the supplied allocator
    // is convertible to 'bsl::allocator<KEY>'.

template <
    class KEY,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
unordered_multiset(std::initializer_list<KEY>, ALLOCATOR)
-> unordered_multiset<KEY, bsl::hash<KEY>, bsl::equal_to<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // This deduction guide does not participate unless the supplied allocator
    // meets the requirements of a standard allocator.

template <
    class KEY,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
unordered_multiset(std::initializer_list<KEY>, ALLOC *)
-> unordered_multiset<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'unordered_multiset'.
    // This deduction guide does not participate unless the supplied allocator
    // is convertible to 'bsl::allocator<KEY>'.
#endif

// FREE OPERATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_multiset' objects have the
    // same value if they have the same number of value elements, and for each
    // value-element that is contained in 'lhs' there is a value-element
    // contained in 'rhs' having the same value, and vice-versa.  Note that
    // this method requires that the (template parameter) type 'KEY' be
    // 'equality-comparable' (see {Requirements on 'KEY'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_multiset' objects do
    // not have the same value if they do not have the same number of
    // value elements, or that for some value-element contained in 'lhs' there
    // is not a value-element in 'rhs' having the same value, and vice-versa.
    // Note that this method requires that the (template parameter) type 'KEY'
    // and be 'equality-comparable' (see {Requirements on 'KEY'}).
#endif

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR, class PREDICATE>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
erase_if(unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& ms,
         PREDICATE                                        predicate);
    // Erase all the elements in the specified unordered_multiset 'ms' that
    // satisfy the specified predicate 'predicate'.  Return the number of
    // elements erased.

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& a,
          unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& b)
                                BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                    BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
    // Exchange the value, hasher, key-equality functor, and 'max_load_factor'
    // of the specified 'a' object with those of the specified 'b' object; also
    // exchange the allocator of 'a' with that of 'b' if the (template
    // parameter) type 'ALLOCATOR' has the 'propagate_on_container_swap' trait,
    // and do not modify either allocator otherwise.  This function provides
    // the no-throw exception-safety guarantee if and only if both the
    // (template parameter) types 'HASH' and 'EQUAL' provide no-throw swap
    // operations; if an exception is thrown, both objects are left in valid
    // but unspecified states.  This operation guarantees 'O[1]' complexity.
    // The behavior is undefined unless either 'a' was created with the same
    // allocator as 'b' or 'ALLOCATOR' has the 'propagate_on_container_swap'
    // trait.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //-------------------------
                        // class unordered_multiset
                        //-------------------------

// CREATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset()
: d_impl(HASH(), EQUAL(), 0, 1.0f, ALLOCATOR())
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, keyEqual, initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                            size_type        initialNumBuckets,
                                            const HASH&      hashFunction,
                                            const ALLOCATOR& basicAllocator)
: d_impl(hashFunction, EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                            size_type        initialNumBuckets,
                                            const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), initialNumBuckets, 1.0f, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                               const ALLOCATOR& basicAllocator)
: d_impl(basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                            const unordered_multiset& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                   BloombergLP::bslmf::MovableRef<unordered_multiset> original)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl))
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                 const unordered_multiset&                      original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
             BloombergLP::bslmf::MovableRef<unordered_multiset> original,
             const typename type_identity<ALLOCATOR>::type&     basicAllocator)
: d_impl(MoveUtil::move(MoveUtil::access(original).d_impl), basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
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
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
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
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
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
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                               INPUT_ITERATOR   first,
                                               INPUT_ITERATOR   last,
                                               const ALLOCATOR& basicAllocator)
: d_impl(HASH(), EQUAL(), 0, 1.0f, basicAllocator)
{
    this->insert(first, last);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class, class, class>
#endif
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                  std::initializer_list<KEY> values,
                                  size_type                  initialNumBuckets,
                                  const hasher&              hashFunction,
                                  const key_equal&           keyEqual,
                                  const ALLOCATOR&           basicAllocator)
: unordered_multiset(values.begin(),
                     values.end(),
                     initialNumBuckets,
                     hashFunction,
                     keyEqual,
                     basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class, class>
#endif
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                  std::initializer_list<KEY> values,
                                  size_type                  initialNumBuckets,
                                  const HASH&                hashFunction,
                                  const ALLOCATOR&           basicAllocator)
: unordered_multiset(values.begin(),
                     values.end(),
                     initialNumBuckets,
                     hashFunction,
                     EQUAL(),
                     basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class>
#endif
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                  std::initializer_list<KEY> values,
                                  size_type                  initialNumBuckets,
                                  const ALLOCATOR&           basicAllocator)
: unordered_multiset(values.begin(),
                     values.end(),
                     initialNumBuckets,
                     HASH(),
                     EQUAL(),
                     basicAllocator)
{
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
template <class>
#endif
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::unordered_multiset(
                                     std::initializer_list<KEY> values,
                                     const ALLOCATOR&           basicAllocator)
: unordered_multiset(values.begin(),
                     values.end(),
                     0,
                     HASH(),
                     EQUAL(),
                     basicAllocator)
{
}
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::~unordered_multiset()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::operator=(
                                                 const unordered_multiset& rhs)
{
    // Note that we have delegated responsibility for correct handling of
    // allocator propagation to the 'HashTable' implementation.

    d_impl = rhs.d_impl;

    return *this;
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::operator=(
                        BloombergLP::bslmf::MovableRef<unordered_multiset> rhs)
                             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                 AllocatorTraits::is_always_equal::value
                              && std::is_nothrow_move_assignable<HASH>::value
                              && std::is_nothrow_move_assignable<EQUAL>::value)
{
    // Note that we have delegated responsibility for correct handling of
    // allocator propagation to the 'HashTable' implementation.

    unordered_multiset& lvalue = rhs;

    d_impl = MoveUtil::move(lvalue.d_impl);

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>&
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::operator=(
                                             std::initializer_list<KEY> values)
{
    unordered_multiset tmp(values, d_impl.allocator());

    d_impl.swap(tmp.d_impl);

    return *this;
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::emplace(Args&&... arguments)
{
    return iterator(d_impl.emplace(
                           BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...));

}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class... Args>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::emplace_hint(
                                                      const_iterator hint,
                                                      Args&&...      arguments)
{
    return iterator(d_impl.emplaceWithHint(hint.node(),
                           BSLS_COMPILERFEATURES_FORWARD(Args, arguments)...));
}
#endif

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::end() BSLS_KEYWORD_NOEXCEPT
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
inline
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
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
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::erase(const key_type& key)
{
    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

    HashTableLink *target = d_impl.find(key);
    if (target) {
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
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    return iterator(d_impl.insert(MoveUtil::move(value)));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const_iterator    hint,
                                                       const value_type& value)
{
    return iterator(d_impl.insert(value, hint.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                              const_iterator                             hint,
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    return iterator(d_impl.insert(MoveUtil::move(value), hint.node()));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                        INPUT_ITERATOR last)
{
    difference_type maxInsertions =
              ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);
    if (maxInsertions) {
        this->reserve(this->size() + maxInsertions);
    }

    while (first != last) {
        d_impl.insert(*first);
        ++first;
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                                             std::initializer_list<KEY> values)
{
    insert(values.begin(), values.end());
}
#endif

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
inline
void
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::swap(
                                                     unordered_multiset& other)
                                   BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                       AllocatorTraits::is_always_equal::value
                                   &&  bsl::is_nothrow_swappable<HASH>::value
                                   &&  bsl::is_nothrow_swappable<EQUAL>::value)
{
    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.allocator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::begin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::end() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::cbegin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::cend() const
                                                          BSLS_KEYWORD_NOEXCEPT
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
                                                          BSLS_KEYWORD_NOEXCEPT
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
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::find(
                                                     const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::contains(
                                                     const key_type& key) const
{
    return find(key) != end();
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::empty() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.size();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::max_size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::hasher
unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
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
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.maxNumBuckets();
}


template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::load_factor() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.loadFactor();
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::max_load_factor() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return d_impl.maxLoadFactor();
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& lhs,
               const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}
#endif

// FREE FUNCTIONS
template <class KEY, class HASH, class EQUAL, class ALLOCATOR, class PREDICATE>
inline
typename bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>::size_type
bsl::erase_if(unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& ms,
              PREDICATE                                        predicate)
{
    return BloombergLP::bslstl::AlgorithmUtil::containerEraseIf(ms, predicate);
}

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
bsl::swap(bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& a,
          bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOCATOR>& b)
                                 BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)))
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

#endif // End C++11 code

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
