// bslstl_multimap.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_MULTIMAP
#define INCLUDED_BSLSTL_MULTIMAP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant multimap class.
//
//@CLASSES:
//   bsl::multimap: STL-compatible multimap template
//
//@CANONICAL_HEADER: bsl_map.h
//
//@SEE_ALSO: bslstl_map, bslstl_multiset
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::multimap', implementing the standard container holding an ordered
// sequence of key-value pairs (possibly having duplicate keys), and presenting
// a mapping from the keys (of a template parameter type, 'KEY') to their
// associated values (of another template parameter type, 'VALUE').
//
// An instantiation of 'multimap' is an allocator-aware, value-semantic type
// whose salient attributes are its size (number of key-value pairs) and the
// ordered sequence of key-value pairs the multimap contains.  If 'multimap' is
// instantiated with either a key type or mapped-value type that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// In particular, if either the key or value type cannot be tested for
// equality, then a 'multimap' containing that type cannot be tested for
// equality.  It is even possible to instantiate 'multimap' with a key or
// mapped-value type that does not have a copy-constructor, in which case the
// 'multimap' will not be copyable.
//
// A multimap meets the requirements of an associative container with
// bidirectional iterators in the C++ standard [23.2.4].  The 'multimap'
// implemented here adheres to the C++11 standard when compiled with a C++11
// compiler, and makes the best approximation when compiled with a C++03
// compiler.  In particular, for C++03 we emulate move semantics, but limit
// forwarding (in 'emplace') to 'const' lvalues, and make no effort to emulate
// 'noexcept' or initializer-lists.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// A 'multimap' is a fully "Value-Semantic Type" (see {'bsldoc_glossary'}) only
// if the supplied 'KEY' and 'VALUE' template parameters are themselves fully
// value-semantic.  It is possible to instantiate a 'multimap' with 'KEY' and
// 'VALUE' parameter arguments that do not provide a full set of value-semantic
// operations, but then some methods of the container may not be instantiable.
// The following terminology, adopted from the C++11 standard, is used in the
// function documentation of 'multimap' to describe a function's requirements
// for the 'KEY' and 'VALUE' template parameters.  These terms are also defined
// in section [17.6.3.1] of the C++11 standard.  Note that, in the context of a
// 'multimap' instantiation, the requirements apply specifically to the
// multimap's entry type, 'value_type', which is an alias for 'pair<const KEY,
// VALUE>'.
//
// Legend
// ------
// 'X'    - denotes an allocator-aware container type (e.g., 'multimap')
// 'T'    - 'value_type' associated with 'X'
// 'A'    - type of the allocator used by 'X'
// 'm'    - lvalue of type 'A' (allocator)
// 'p',   - address ('T *') of uninitialized storage for a 'T' within an 'X'
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
///Memory Allocation
///-----------------
// The type supplied as a multimap's 'ALLOCATOR' template parameter determines
// how that multimap will allocate memory.  The 'multimap' template supports
// allocators meeting the requirements of the C++11 standard [17.6.3.5].  In
// addition, it supports scoped-allocators derived from the 'bslma::Allocator'
// memory allocation protocol.  Clients intending to use 'bslma'-style
// allocators should use the template's default 'ALLOCATOR' type.  The default
// type for the 'ALLOCATOR' template parameter, 'bsl::allocator', provides a
// C++11 standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of a 'multimap' instantiation
// is 'bsl::allocator', then objects of that multimap type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a multimap
// accepts an optional 'bslma::Allocator' argument at construction.  If the
// address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it is used to supply memory for the multimap throughout its
// lifetime; otherwise, the multimap will use the default allocator installed
// at the time of the multimap's construction (see 'bslma_default').  In
// addition to directly allocating memory from the indicated
// 'bslma::Allocator', a multimap supplies that allocator's address to the
// constructors of contained objects of the (template parameter) types 'KEY'
// and 'VALUE', if respectively, the types define the
// 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'multimap':
//..
//  Legend
//  ------
//  'K'             - (template parameter) type 'KEY' of the 'multimap'
//  'V'             - (template parameter) type 'VALUE' of the 'multimap'
//  'a', 'b'        - two distinct objects of type 'multimap<K, V>'
//  'rv'            - modifiable rvalue of type 'multimap<K, V>'
//  'n', 'm'        - number of elements in 'a' and 'b', respectively
//  'value_type'    - 'pair<const K, V>'
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al'            - STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - object of type 'K'
//  'v'             - object of type 'V'
//  'vt'            - object of type 'value_type'
//  'rvt'           - modifiable rvalue of type 'value_type'
//  'p1', 'p2'      - two 'const_iterator's belonging to 'a'
//  distance(i1,i2) - number of elements in the range '[i1 .. i2)'
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | multimap<K, V> a;        (default construction)    | O[1]               |
//  | multimap<K, V> a(al);                              |                    |
//  | multimap<K, V> a(c, al);                           |                    |
//  +----------------------------------------------------+--------------------+
//  | multimap<K, V> a(rv);    (move construction)       | O[1] if 'a' and    |
//  | multimap<K, V> a(rv, al);                          | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | multimap<K, V> a(b);     (copy construction)       | O[n]               |
//  | multimap<K, V> a(b, al);                           |                    |
//  +----------------------------------------------------+--------------------+
//  | multimap<K, V> a(i1, i2);                          | O[N] if [i1, i2)   |
//  | multimap<K, V> a(i1, i2, al);                      | is sorted with     |
//  | multimap<K, V> a(i1, i2, c, al);                   | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where N |
//  |                                                    | is distance(i1,i2) |
//  +----------------------------------------------------+--------------------+
//  | a.~multimap<K, V>();     (destruction)             | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = rv;                  (move assignment)         | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | a = b;                   (copy assignment)         | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  | a.rbegin(), a.rend(), a.crbegin(), a.crend()       |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a < b, a <= b, a > b, a >= b                       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b)                              | O[1] if 'a' and    |
//  |                                                    | 'b' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O[n + m] otherwise |
//  +----------------------------------------------------+--------------------+
//  | a.size()                                           | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.max_size()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.empty()                                          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | get_allocator()                                    | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.insert(vt)                                       | O[log(n)]          |
//  | a.insert(rvt)                                      |                    |
//  | a.emplace(Args&&...)                               |                    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, vt)                                   | amortized constant |
//  | a.insert(p1, rvt)                                  | if the value is    |
//  | a.emplace(p1, Args&&...)                           | inserted right     |
//  |                                                    | before p1,         |
//  |                                                    | O[log(n)]          |
//  |                                                    | otherwise          |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | O[log(N) *         |
//  |                                                    |   distance(i1,i2)] |
//  |                                                    |                    |
//  |                                                    | where N is         |
//  |                                                    | n + distance(i1,i2)|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | amortized constant |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | O[log(n) +         |
//  |                                                    | a.count(k)]        |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O[log(n) +         |
//  |                                                    | distance(p1, p2)]  |
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1, p2)                                    | O[log(n) +         |
//  |                                                    | distance(p1, p2)]  |
//  +----------------------------------------------------+--------------------+
//  | a.clear()                                          | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.contains(k)                                      | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.key_comp()                                       | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.value_comp()                                     | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a.find(k)                                          | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.count(k)                                         | O[log(n) +         |
//  |                                                    | a.count(k)]        |
//  +----------------------------------------------------+--------------------+
//  | a.lower_bound(k)                                   | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.upper_bound(k)                                   | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.equal_range(k)                                   | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Phone Book
/// - - - - - - - - - - - - - - - -
// In this example, we will define a class 'PhoneBook', that provides a mapping
// of names to phone numbers.  The 'PhoneBook' class will be implemented using
// a 'bsl::multimap', and will supply manipulators, allowing a client to add or
// remove entries from the phone book, as well as accessors, allowing clients
// to efficiently lookup entries by name, and to iterate over the entries in
// the phone book in sorted order.
//
// Note that this example uses a type 'string' that is based on the standard
// type 'string' (see 'bslstl_string').  For the sake of brevity, the
// implementation of 'string' is not explored here.
//
// First, we define an alias for a pair of 'string' objects that we will use to
// represent names in the phone book:
//..
//  typedef bsl::pair<string, string> FirstAndLastName;
//      // This 'typedef' provides an alias for a pair of 'string' objects,
//      // whose 'first' and 'second' elements refer to the first and last
//      // names of a person, respectively.
//..
// Then, we define a comparison functor for 'FirstAndLastName' objects (note
// that this comparator is required because we intend for the last name to
// take precedence over the first name in the ordering of entries maintained
// by the phone book, which differs from the behavior supplied by 'operator<'
// for 'bsl::pair'):
//..
//  struct FirstAndLastNameLess {
//      // This 'struct' defines an ordering on 'FirstAndLastName' values,
//      // allowing them to be included in sorted containers such as
//      // 'bsl::multimap'.  Note that last name (the 'second' member of a
//      // 'FirstAndLastName' value) takes precedence over first name in the
//      // ordering defined by this functor.
//
//      bool operator()(const FirstAndLastName& lhs,
//                      const FirstAndLastName& rhs) const
//          // Return 'true' if the value of the specified 'lhs' is less than
//          // (ordered before) the value of the specified 'rhs', and 'false'
//          // otherwise.  The 'lhs' value is considered less than the 'rhs'
//          // value if the second value in the 'lhs' pair (the last name) is
//          // less than the second value in the 'rhs' pair or, if the second
//          // values are equal, if the first value in the 'lhs' pair (the
//          // first name) is less than the first value in the 'rhs' pair.
//      {
//          int cmp = std::strcmp(lhs.second.c_str(), rhs.second.c_str());
//          if (0 == cmp) {
//              cmp = std::strcmp(lhs.first.c_str(), rhs.first.c_str());
//          }
//          return cmp < 0;
//      }
//  };
//..
// Next, we define the public interface for 'PhoneBook':
//..
//  class PhoneBook {
//      // This class provides a mapping of a person's name to their phone
//      // number.  Names within a 'PhoneBook' are represented using a using
//      // 'FirstAndLastName' object, and phone numbers are represented using a
//      // 'bsls::Types::Uint64' value.
//..
// Here, we create a type alias, 'NameToNumberMap', for a 'bsl::multimap' that
// will serve as the data member for a 'PhoneBook'.  A 'NameToNumberMap' has
// keys of type 'FirstAndLastName', mapped-values of type
// 'bsls::Types::Uint64', and a comparator of type 'FirstAndLastNameLess'.  We
// use the default 'ALLOCATOR' template parameter as we intend to use
// 'PhoneBook' with 'bslma' style allocators:
//..
//      // PRIVATE TYPES
//      typedef bsl::multimap<FirstAndLastName,
//                            bsls::Types::Uint64,
//                            FirstAndLastNameLess> NameToNumberMap;
//          // This 'typedef' is an alias for a mapping between names and phone
//          // numbers.
//
//      // DATA
//      NameToNumberMap d_nameToNumber;  // mapping of names to phone numbers
//
//      // FRIENDS
//      friend bool operator==(const PhoneBook& lhs, const PhoneBook& rhs);
//
//    public:
//      // PUBLIC TYPES
//      typedef bsls::Types::Uint64 PhoneNumber;
//          // This 'typedef' provides an alias for the type of an unsigned
//          // integers used to represent phone-numbers in a 'PhoneBook'.
//
//      typedef NameToNumberMap::const_iterator ConstIterator;
//          // This 'typedef' provides an alias for the type of an iterator
//          // providing non-modifiable access to the entries in a 'PhoneBook'.
//
//      // CREATORS
//      PhoneBook(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'PhoneBook' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      PhoneBook(const PhoneBook&  original,
//                bslma::Allocator  *basicAllocator = 0);
//          // Create a 'PhoneBook' object having the same value as the
//          // specified 'original' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      //! ~PhoneBook() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      PhoneBook& operator=(const PhoneBook& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
//      void addEntry(const FirstAndLastName& name, PhoneNumber number);
//          // Add an entry to this phone book having the specified 'name' and
//          // 'number'.  The behavior is undefined unless 'name.first' and
//          // 'name.end' are non-empty strings.
//
//      int removeEntry(const FirstAndLastName& name, PhoneNumber number);
//          // Remove the entries from this phone book having the specified
//          // 'name' and 'number', if they exists, and return the number of
//          // removed entries; otherwise, return 0 with no other effects.
//
//      // ACCESSORS
//      bsl::pair<ConstIterator, ConstIterator> lookupByName(
//                                         const FirstAndLastName& name) const;
//          // Return a pair of iterators to the ordered sequence of entries
//          // held in this phone book having the specified 'name', where the
//          // first iterator is position at the start of the sequence, and the
//          // second is positioned one past the last entry in the sequence.
//          // If 'name' does not exist in this phone book, then the two
//          // returned iterators will have the same value.
//
//      ConstIterator begin() const;
//          // Return an iterator providing non-modifiable access to the first
//          // entry in the ordered sequence of entries held in this phone
//          // book, or the past-the-end iterator if this phone book is empty.
//
//      ConstIterator end() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end entry in the ordered sequence of entries maintained
//          // by this phone book.
//
//      size_t numEntries() const;
//          // Return the number of entries contained in this phone book.
//  };
//..
// Then, we declare the free operators for 'PhoneBook':
//..
//  inline
//  bool operator==(const PhoneBook& lhs, const PhoneBook& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'PhoneBook' objects have the
//      // same value if they have the same number of entries, and each
//      // corresponding entry, in their respective ordered sequence of
//      // entries, is the same.
//
//  inline
//  bool operator!=(const PhoneBook& lhs, const PhoneBook& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'PhoneBook' objects do
//      // not have the same value if they either differ in their number of
//      // contained entries, or if any of the corresponding entries, in their
//      // respective ordered sequences of entries, is not the same.
//..
// Now, we define the implementations methods of the 'PhoneBook' class:
//..
//  // CREATORS
//  inline
//  PhoneBook::PhoneBook(bslma::Allocator *basicAllocator)
//  : d_nameToNumber(FirstAndLastNameLess(), basicAllocator)
//  {
//  }
//..
// Notice that, on construction, we pass the contained 'bsl::multimap'
// ('d_nameToNumber'), a default constructed 'FirstAndLastNameLess' object that
// it will use to perform comparisons, and the allocator supplied to
// 'PhoneBook' at construction'.
//..
//  inline
//  PhoneBook::PhoneBook(const PhoneBook&   original,
//                       bslma::Allocator  *basicAllocator)
//  : d_nameToNumber(original.d_nameToNumber, basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  PhoneBook& PhoneBook::operator=(const PhoneBook& rhs)
//  {
//      d_nameToNumber = rhs.d_nameToNumber;
//      return *this;
//  }
//
//  inline
//  void PhoneBook::addEntry(const FirstAndLastName& name, PhoneNumber number)
//  {
//      BSLS_ASSERT(!name.first.empty());
//      BSLS_ASSERT(!name.second.empty());
//
//      d_nameToNumber.insert(NameToNumberMap::value_type(name, number));
//  }
//
//  inline
//  int PhoneBook::removeEntry(const FirstAndLastName& name,
//                             PhoneNumber             number)
//  {
//      bsl::pair<NameToNumberMap::iterator, NameToNumberMap::iterator> range =
//                                            d_nameToNumber.equal_range(name);
//
//      NameToNumberMap::iterator it = range.first;
//
//      int numRemovedEntries = 0;
//
//      while (it != range.second) {
//          if (it->second == number) {
//              it = d_nameToNumber.erase(it);
//              ++numRemovedEntries;
//          }
//          else {
//              ++it;
//          }
//      }
//
//      return numRemovedEntries;
//  }
//
//  // ACCESSORS
//  inline
//  bsl::pair<PhoneBook::ConstIterator, PhoneBook::ConstIterator>
//  PhoneBook::lookupByName(const FirstAndLastName&  name) const
//  {
//      return d_nameToNumber.equal_range(name);
//  }
//
//  inline
//  PhoneBook::ConstIterator PhoneBook::begin() const
//  {
//      return d_nameToNumber.begin();
//  }
//
//  inline
//  PhoneBook::ConstIterator PhoneBook::end() const
//  {
//      return d_nameToNumber.end();
//  }
//
//  inline
//  size_t PhoneBook::numEntries() const
//  {
//      return d_nameToNumber.size();
//  }
//..
// Finally, we implement the free operators for 'PhoneBook':
//..
//  inline
//  bool operator==(const PhoneBook& lhs, const PhoneBook& rhs)
//  {
//      return lhs.d_nameToNumber == rhs.d_nameToNumber;
//  }
//
//  inline
//  bool operator!=(const PhoneBook& lhs, const PhoneBook& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_iteratorutil.h>
#include <bslstl_mapcomparator.h>
#include <bslstl_pair.h>
#include <bslstl_stdexceptutil.h>
#include <bslstl_treeiterator.h>
#include <bslstl_treenode.h>
#include <bslstl_treenodepool.h>

#include <bslalg_rangecompare.h>
#include <bslalg_rbtreeanchor.h>
#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeutil.h>
#include <bslalg_swaputil.h>
#include <bslalg_synththreewayutil.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslma_isstdallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_istransparentpredicate.h>
#include <bslmf_movableref.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <functional>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>  // 'std::is_constructible'
    #ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    #error Rvalue references curiously absent despite native 'type_traits'.
    #endif
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_multimap.h
# define COMPILING_BSLSTL_MULTIMAP_H
# include <bslstl_multimap_cpp03.h>
# undef COMPILING_BSLSTL_MULTIMAP_H
#else

namespace bsl {

                             // ==============
                             // class multimap
                             // ==============

template <class KEY,
          class VALUE,
          class COMPARATOR = std::less<KEY>,
          class ALLOCATOR  = allocator<pair<const KEY, VALUE> > >
class multimap {
    // This class template implements a value-semantic container type holding
    // an ordered sequence of key-value pairs having possibly duplicate keys
    // that provide a mapping from keys (of the template parameter type, 'KEY')
    // to their associated values (of another template parameter type,
    // 'VALUE').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for BDEX serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    // PRIVATE TYPES
    typedef pair<const KEY, VALUE>                             ValueType;
        // This typedef is an alias for the type of key-value pair objects
        // maintained by this multimap.

    typedef BloombergLP::bslstl::MapComparator<KEY, VALUE, COMPARATOR>
                                                               Comparator;
        // This typedef is an alias for the comparator used internally by this
        // multimap.

    typedef BloombergLP::bslstl::TreeNode<ValueType>           Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this multimap.

    typedef BloombergLP::bslstl::TreeNodePool<ValueType, ALLOCATOR>
                                                               NodeFactory;
        // This typedef is an alias for the factory type used to create and
        // destroy 'Node' objects.

    typedef typename bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    typedef BloombergLP::bslmf::MovableRefUtil                 MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    class DataWrapper : public Comparator {
        // This class is a wrapper around the comparator and allocator data
        // members.  It takes advantage of the empty-base optimization (EBO) so
        // that if the comparator is stateless, it takes up no space.
        //
        // TBD: This class should eventually be replaced by the use of a
        // general EBO-enabled component that provides a 'pair'-like interface
        // or a 'tuple'.

        // DATA
        NodeFactory d_pool;  // pool of 'Node' objects

      private:
        // NOT IMPLEMENTED
        DataWrapper(const DataWrapper&);
        DataWrapper& operator=(const DataWrapper&);

      public:
        // CREATORS
        DataWrapper(const COMPARATOR& comparator,
                    const ALLOCATOR&  basicAllocator);
            // Create a data wrapper using a copy of the specified 'comparator'
            // to order key-value pairs and a copy of the specified
            // 'basicAllocator' to supply memory.

        DataWrapper(
              BloombergLP::bslmf::MovableRef<DataWrapper> original);// IMPLICIT
            // Create a data wrapper initialized to the contents of the 'pool'
            // associated with the specified 'original' data wrapper.  The
            // comparator and allocator associated with 'original' are
            // propagated to the new data wrapper.  'original' is left in a
            // valid but unspecified state.

        // MANIPULATORS
        NodeFactory& nodeFactory();
            // Return a reference providing modifiable access to the node
            // factory associated with this data wrapper.

        // ACCESSORS
        const NodeFactory& nodeFactory() const;
            // Return a reference providing non-modifiable access to the node
            // factory associated with this data wrapper.
    };

    // DATA
    DataWrapper                       d_compAndAlloc;
                                               // comparator and pool of 'Node'
                                               // objects

    BloombergLP::bslalg::RbTreeAnchor d_tree;  // balanced tree of 'Node'
                                               // objects

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef VALUE                                      mapped_type;
    typedef pair<const KEY, VALUE>                     value_type;
    typedef COMPARATOR                                 key_compare;
    typedef ALLOCATOR                                  allocator_type;
    typedef value_type&                                reference;
    typedef const value_type&                          const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

    typedef BloombergLP::bslstl::TreeIterator<value_type,
                                              Node,
                                              difference_type> iterator;

    typedef BloombergLP::bslstl::TreeIterator<const value_type,
                                              Node,
                                              difference_type> const_iterator;

    typedef bsl::reverse_iterator<iterator>            reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>      const_reverse_iterator;

    class value_compare {
        // This nested class defines a mechanism for comparing two objects of
        // 'value_type' by adapting an object of (template parameter) type
        // 'COMPARATOR', which compares two objects of (template parameter)
        // type 'KEY' .  Note that this class exactly matches its definition in
        // the C++11 standard [23.4.4.1]; otherwise, we would have implemented
        // it as a separate component-local class.

        // FRIENDS
        friend class multimap;

      protected:
        // PROTECTED DATA
        COMPARATOR comp;  // we would not have elected to make this data
                          // member 'protected'

        // PROTECTED CREATORS
        value_compare(COMPARATOR comparator);                       // IMPLICIT
            // Create a 'value_compare' object that uses the specified
            // 'comparator'.

      public:
        // PUBLIC TYPES
        typedef bool result_type;
            // This 'typedef' is an alias for the result type of a call to the
            // overload of 'operator()' (the comparison function) provided by a
            // 'multimap::value_compare' object.

        typedef value_type first_argument_type;
            // This 'typedef' is an alias for the type of the first parameter
            // of the overload of 'operator()' (the comparison function)
            // provided by a 'multimap::value_compare' object.

        typedef value_type second_argument_type;
            // This 'typedef' is an alias for the type of the second parameter
            // of the overload of 'operator()' (the comparison function)
            // provided by a 'multimap::value_compare' object.

        // CREATORS
        //! value_compare(const value_compare& original) = default;
            // Create a 'value_compare' object having the same value as the
            // specified 'original' object.

        //! ~value_compare() = default;
            // Destroy this object.

        // MANIPULATORS
        //! value_compare& operator=(const value_compare& rhs) = default;
            // Assign to this object the value of the specified 'rhs' object,
            // and return a reference providing modifiable access to this
            // object.

        // ACCESSORS
        bool operator()(const value_type& x, const value_type& y) const;
            // Return 'true' if the specified 'x' object is ordered before the
            // specified 'y' object, as determined by the comparator supplied
            // at construction, and 'false' otherwise.
    };

  private:
    // PRIVATE CLASS METHODS
    static Node *toNode(BloombergLP::bslalg::RbTreeNode *node);
        // Return an address providing modifiable access to the specified
        // 'node'.  The behavior is undefined unless 'node' is the address of a
        // 'Node' object.

    static const Node *toNode(const BloombergLP::bslalg::RbTreeNode *node);
        // Return an address providing non-modifiable access to the specified
        // 'node'.  The behavior is undefined unless 'node' is the address of a
        // 'Node' object.

    // PRIVATE MANIPULATORS
    Comparator& comparator();
        // Return a reference providing modifiable access to the comparator for
        // this multimap.

    NodeFactory& nodeFactory();
        // Return a reference providing modifiable access to the node allocator
        // for this multimap.

    void quickSwapExchangeAllocators(multimap& other);
        // Efficiently exchange the value, comparator, and allocator of this
        // object with the value, comparator, and allocator of the specified
        // 'other' object.  This method provides the no-throw exception-safety
        // guarantee, *unless* swapping the (user-supplied) comparator or
        // allocator objects can throw.

    void quickSwapRetainAllocators(multimap& other);
        // Efficiently exchange the value and comparator of this object with
        // the value and comparator of the specified 'other' object.  This
        // method provides the no-throw exception-safety guarantee, *unless*
        // swapping the (user-supplied) comparator objects can throw.  The
        // behavior is undefined unless this object was created with the same
        // allocator as 'other'.

    // PRIVATE ACCESSORS
    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this multimap.

    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the node
        // allocator for this multimap.

  public:
    // CREATORS
    multimap();
    explicit multimap(const COMPARATOR& comparator,
                      const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Create an empty multimap.  Optionally specify a 'comparator' used to
        // order key-value pairs contained in this object.  If 'comparator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'COMPARATOR' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.
    : d_compAndAlloc(comparator, basicAllocator)
    , d_tree()
    {
        // The implementation is placed here in the class definition to work
        // around an AIX compiler bug, where the constructor can fail to
        // compile because it is unable to find the definition of the default
        // argument.  This occurs when a parameterized class wraps around the
        // container and the comparator is defined after the new class.
    }

    explicit multimap(const ALLOCATOR& basicAllocator);
        // Create an empty multimap that uses the specified 'basicAllocator' to
        // supply memory.  Use a default-constructed object of the (template
        // parameter) type 'COMPARATOR' to order the key-value pairs contained
        // in this multimap.  Note that a 'bslma::Allocator *' can be supplied
        // for 'basicAllocator' if the (template parameter) 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    multimap(const multimap& original);
        // Create a multimap having the same value as the specified 'original'
        // object.  Use a copy of 'original.key_comp()' to order the key-value
        // pairs contained in this multimap.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // types 'KEY' and 'VALUE' both be 'copy-insertable' into this multimap
        // (see {Requirements on 'KEY' and 'VALUE'}).

    multimap(BloombergLP::bslmf::MovableRef<multimap> original);    // IMPLICIT
        // Create a multimap having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new multimap.  Use a copy of 'original.key_comp()' to order the
        // key-value pairs contained in this multimap.  The allocator
        // associated with 'original' is propagated for use in the
        // newly-created multimap.  'original' is left in a valid but
        // unspecified state.

    multimap(const multimap&                                original,
             const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a multimap having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Use a copy of 'original.key_comp()' to order the key-value pairs
        // contained in this multimap.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be 'copy-insertable' into
        // this multimap (see {Requirements on 'KEY' and 'VALUE'}).  Note that
        // a 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator' (the default).

    multimap(BloombergLP::bslmf::MovableRef<multimap>       original,
             const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a multimap having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // multimap if 'basicAllocator == original.get_allocator()', and are
        // move-inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  Use a copy of
        // 'original.key_comp()' to order the key-value pairs contained in this
        // multimap.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'move-insertable' into this multimap (see
        // {Requirements on 'KEY' and 'VALUE'}).  Note that a 'bslma::Allocator
        // *' can be supplied for 'basicAllocator' if the (template parameter)
        // 'ALLOCATOR' is 'bsl::allocator' (the default).

    template <class INPUT_ITERATOR>
    multimap(INPUT_ITERATOR    first,
             INPUT_ITERATOR    last,
             const COMPARATOR& comparator     = COMPARATOR(),
             const ALLOCATOR&  basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    multimap(INPUT_ITERATOR    first,
             INPUT_ITERATOR    last,
             const ALLOCATOR&  basicAllocator);
        // Create a multimap, and insert each 'value_type' object in the
        // sequence starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // a 'comparator' used to order key-value pairs contained in this
        // object.  If 'comparator' is not supplied, a default-constructed
        // object of the (template parameter) type 'COMPARATOR' is used.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' (the default), then
        // 'basicAllocator', if supplied, shall be convertible to
        // 'bslma::Allocator *'.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // and 'basicAllocator' is not supplied, the currently installed
        // default allocator is used.  If the sequence 'first' to 'last' is
        // ordered according to 'comparator', then this operation has 'O[N]'
        // complexity, where 'N' is the number of elements between 'first' and
        // 'last'; otherwise, this operation has 'O[N * log(N)]' complexity.
        // The (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type', and 'value_type' must be 'emplace-constructible' from
        // '*i' into this multimap, where 'i' is a dereferenceable iterator in
        // the range '[first .. last)' (see {Requirements on 'KEY' and
        // 'VALUE'}).  The behavior is undefined unless 'first' and 'last'
        // refer to a sequence of valid values where 'first' is at a position
        // at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    multimap(std::initializer_list<value_type> values,
             const COMPARATOR&                 comparator     = COMPARATOR(),
             const ALLOCATOR&                  basicAllocator = ALLOCATOR());
    multimap(std::initializer_list<value_type> values,
             const ALLOCATOR&                  basicAllocator);
        // Create a multimap and insert each 'value_type' object in the
        // specified 'values' initializer list.  Optionally specify a
        // 'comparator' used to order keys contained in this object.  If
        // 'comparator' is not supplied, a default-constructed object of the
        // (template parameter) type 'COMPARATOR' is used.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used.  If
        // 'values' is ordered according to 'comparator', then this operation
        // has 'O[N]' complexity, where 'N' is the number of elements in
        // 'values'; otherwise, this operation has 'O[N * log(N)]' complexity.
        // This method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'copy-insertable' into this multimap (see
        // {Requirements on 'KEY' and 'VALUE'}).
#endif

    ~multimap();
        // Destroy this object.

    // MANIPULATORS
    multimap& operator=(const multimap& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // If an exception is thrown, '*this' is left in a valid but
        // unspecified state.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be 'copy-assignable' and
        // 'copy-insertable' into this multimap (see {Requirements on 'KEY' and
        // 'VALUE'}).

    multimap& operator=(BloombergLP::bslmf::MovableRef<multimap> rhs)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           AllocatorTraits::is_always_equal::value &&
                           std::is_nothrow_move_assignable<COMPARATOR>::value);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_move_assignment',
        // and return a reference providing modifiable access to this object.
        // The contents of 'rhs' are moved (in constant time) to this multimap
        // if 'get_allocator() == rhs.get_allocator()' (after accounting for
        // the aforementioned trait); otherwise, all elements in this multimap
        // are either destroyed or move-assigned to and each additional element
        // in 'rhs' is move-inserted into this multimap.  'rhs' is left in a
        // valid but unspecified state, and if an exception is thrown, '*this'
        // is left in a valid but unspecified state.  This method requires that
        // the (template parameter) types 'KEY' and 'VALUE' both be
        // 'move-assignable' and 'move-insertable' into this multimap (see
        // {Requirements on 'KEY' and 'VALUE'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    multimap& operator=(std::initializer_list<value_type> values);
        // Assign to this object the value resulting from first clearing this
        // multimap and then inserting each 'value_type' object in the
        // specified 'values' initializer list, and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter) types 'KEY' and 'VALUE' both be
        // 'copy-insertable' into this multimap (see {Requirements on 'KEY' and
        // 'VALUE'}).
#endif

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or the 'end' iterator if this multimap
        // is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this multimap.

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or 'rend' if this multimap is empty.

    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multimap.

    iterator insert(const value_type& value);
        // Insert the specified 'value' into this multimap.  If a range
        // containing elements equivalent to 'value' already exists, insert the
        // 'value' at the end of that range.  Return an iterator referring to
        // the newly inserted 'value_type' object.  This method requires that
        // the (template parameter) types 'KEY' and 'VALUE' both be
        // 'copy-insertable' into this multimap (see {Requirements on 'KEY' and
        // 'VALUE'}).

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ALT_VALUE_TYPE>
    iterator
#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    template <class ALT_VALUE_TYPE>
    typename enable_if<is_convertible<ALT_VALUE_TYPE, value_type>::value,
                       iterator>::type
#else
    template <class ALT_VALUE_TYPE>
    typename enable_if<std::is_constructible<value_type,
                                             ALT_VALUE_TYPE&&>::value,
                       iterator>::type
#endif
    insert(BSLS_COMPILERFEATURES_FORWARD_REF(ALT_VALUE_TYPE) value)
        // Insert into this multimap a 'value_type' object created from the
        // specified 'value'.  If a range containing elements equivalent to
        // 'value_type' object already exists, insert the 'value_type' object
        // at the end of that range.  Return an iterator referring to the newly
        // inserted 'value_type' object.  This method requires that the
        // (template parameter) types 'KEY' and 'VALUE' both be
        // 'move-insertable' into this multimap (see {Requirements on 'KEY' and
        // 'VALUE'}), and the 'value_type' be constructible from the (template
        // parameter) 'ALT_VALUE_TYPE'.
    {
        // Note that some compilers fail when this method is defined
        // out-of-line.

        return emplace(BSLS_COMPILERFEATURES_FORWARD(ALT_VALUE_TYPE, value));
    }

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this multimap (in amortized
        // constant time if the specified 'hint' is a valid immediate successor
        // to the key of 'value').  Return an iterator referring to the newly
        // inserted 'value_type' object.  If 'hint' is not a valid immediate
        // successor to the key of 'value', this operation has 'O[log(N)]'
        // complexity, where 'N' is the size of this multimap.  This method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be 'copy-insertable' into this multimap (see {Requirements on 'KEY'
        // and 'VALUE'}).  The behavior is undefined unless 'hint' is an
        // iterator in the range '[begin() .. end()]' (both endpoints
        // included).

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ALT_VALUE_TYPE>
    iterator
#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    template <class ALT_VALUE_TYPE>
    typename enable_if<is_convertible<ALT_VALUE_TYPE, value_type>::value,
                       iterator>::type
#else
    template <class ALT_VALUE_TYPE>
    typename enable_if<std::is_constructible<value_type,
                                             ALT_VALUE_TYPE&&>::value,
                       iterator>::type
#endif
    insert(const_iterator                                    hint,
           BSLS_COMPILERFEATURES_FORWARD_REF(ALT_VALUE_TYPE) value)
        // Insert into this multimap a 'value_type' object created from the
        // specified 'value' (in amortized constant time if the specified
        // 'hint' is a valid immediate successor to the object created from
        // 'value').  Return an iterator referring to the newly inserted
        // 'value_type' object in this multimap.  If 'hint' is not a valid
        // immediate successor to the object created from 'value', this
        // operation has 'O[log(N)]' complexity, where 'N' is the size of this
        // multimap.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'move-insertable' into this multimap (see
        // {Requirements on 'KEY' and 'VALUE'}), and the 'value_type' be
        // constructible from the (template parameter) 'ALT_VALUE_TYPE'.  The
        // behavior is undefined unless 'hint' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).
    {
        // Note that some compilers fail when this method is defined
        // out-of-line.

        return emplace_hint(hint,
                        BSLS_COMPILERFEATURES_FORWARD(ALT_VALUE_TYPE, value));
    }

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this multimap the value of each 'value_type' object in
        // the range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type', and
        // 'value_type' must be 'emplace-constructible' from '*i' into this
        // multimap, where 'i' is a dereferenceable iterator in the range
        // '[first .. last)' (see {Requirements on 'KEY' and 'VALUE'}).  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    void insert(const_iterator first, const_iterator last);
        // This method is provided only on Sun to work around a bug in the Sun
        // Studio 12.3 compiler, which prevents us from disabling (at compile
        // time) the overload of 'insert' taking a 'const_iterator' and a
        // forwarding reference if the second argument is not convertible to
        // the value type associated with the map.  Without such a check, in
        // certain cases, the same compiler complains of ambiguity between
        // the 'insert' method taking two input iterators and the 'insert'
        // method taking a 'const_iterator' and a forwarding reference; such
        // an ambiguity is resolved by providing this method, which is
        // equivalent to the 'insert' method (above) taking two input iterators
        // of template parameter type.
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<value_type> values);
        // Insert into this multimap the value of each 'value_type' object in
        // the specified 'values' initializer list.  This method requires that
        // the (template parameter) types 'KEY' and 'VALUE' both be
        // 'copy-insertable' into this multimap (see {Requirements on 'KEY' and
        // 'VALUE'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=2
    template <class... Args>
    iterator emplace(Args&&... args);
        // Insert into this multimap a newly-created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type'.  Return an iterator referring to the
        // newly created and inserted object in this multimap.  This method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be 'emplace-constructible' from 'args' (see {Requirements on 'KEY'
        // and 'VALUE'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);
        // Insert into this multimap a newly-created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type' (in amortized constant time if the
        // specified 'hint' is a valid immediate successor to the 'value_type'
        // object constructed from 'args').  Return an iterator referring to
        // the newly created and inserted object in this multimap.  If 'hint'
        // is not a valid immediate successor to the 'value_type' object
        // implied by 'args', this operation has 'O[log(N)]' complexity where
        // 'N' is the size of this multimap.  This method requires that the
        // (template parameter) types 'KEY' and 'VALUE' both be
        // 'emplace-constructible' from 'args' (see {Requirements on 'KEY' and
        // 'VALUE'}).  The behavior is undefined unless 'hint' is an iterator
        // in the range '[begin() .. end()]' (both endpoints included).
#endif

    iterator erase(const_iterator position);
    iterator erase(iterator position);
        // Remove from this multimap the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this multimap.   This method invalidates
        // only iterators and references to the removed element and previously
        // saved values of the 'end()' iterator.  The behavior is undefined
        // unless 'position' refers to a 'value_type' object in this multimap.

    size_type erase(const key_type& key);
        // Remove from this multimap all 'value_type' objects whose keys are
        // equivalent to the specified 'key', if such entries exist, and return
        // the number of erased objects; otherwise, if there is no 'value_type'
        // objects having an equivalent key, return 0 with no other effect.
        // This method invalidates only iterators and references to the removed
        // element and previously saved values of the 'end()' iterator.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this multimap the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this multimap or are
        // the 'end' iterator, and the 'first' position is at or before the
        // 'last' position in the ordered sequence provided by this container.

    void swap(multimap& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                 AllocatorTraits::is_always_equal::value &&
                                 bsl::is_nothrow_swappable<COMPARATOR>::value);
        // Exchange the value and comparator of this object with those of the
        // specified 'other' object; also exchange the allocator of this object
        // with that of 'other' if the (template parameter) type 'ALLOCATOR'
        // has the 'propagate_on_container_swap' trait, and do not modify
        // either allocator otherwise.  This method provides the no-throw
        // exception-safety guarantee if and only if the (template parameter)
        // type 'COMPARATOR' provides a no-throw swap operation, and provides
        // the basic exception-safety guarantee otherwise; if an exception is
        // thrown, both objects are left in valid but unspecified states.  This
        // operation has 'O[1]' complexity if either this object was created
        // with the same allocator as 'other' or 'ALLOCATOR' has the
        // 'propagate_on_container_swap' trait; otherwise, it has 'O[n + m]'
        // complexity, where 'n' and 'm' are the number of elements in this
        // object and 'other', respectively.  Note that this method's support
        // for swapping objects created with different allocators when
        // 'ALLOCATOR' does not have the 'propagate_on_container_swap' trait is
        // a departure from the C++ Standard.

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all entries from this multimap.  Note that the multimap is
        // empty after this call, but allocated memory may be retained for
        // future use.

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    iterator find(const key_type& key)
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in this multimap whose key is equivalent to the
        // specified 'key', if such an entry exists, and the past-the-end
        // ('end') iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        iterator>::type
    find(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in this multimap whose key is equivalent to the
        // specified 'key', if such an entry exists, and the past-the-end
        // ('end') iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    iterator lower_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multimap whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this multimap does not contain a 'value_type' object
        // whose key is greater-than or equal-to 'key'.  Note that this
        // function returns the *first* position before which a 'value_type'
        // object having an equivalent key could be inserted into the ordered
        // sequence maintained by this multimap, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        iterator>::type
    lower_bound(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multimap whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this multimap does not contain a 'value_type' object
        // whose key is greater-than or equal-to 'key'.  Note that this
        // function returns the *first* position before which a 'value_type'
        // object having an equivalent key could be inserted into the ordered
        // sequence maintained by this multimap, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    iterator upper_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multimap whose key is
        // greater than the specified 'key', and the past-the-end iterator if
        // this multimap does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having an equivalent key
        // could be inserted into the ordered sequence maintained by this
        // multimap, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        iterator>::type
    upper_bound(const LOOKUP_KEY& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multimap whose key is
        // greater than the specified 'key', and the past-the-end iterator if
        // this multimap does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having an equivalent key
        // could be inserted into the ordered sequence maintained by this
        // multimap, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    bsl::pair<iterator,iterator> equal_range(const key_type& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multimap whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second is positioned
        // one past the end of the sequence.  The first returned iterator will
        // be 'lower_bound(key)', the second returned iterator will be
        // 'upper_bound(key)', and, if this multimap contains no 'value_type'
        // object with an equivalent key, then the two returned iterators will
        // have the same value.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        iterator startIt = lower_bound(key);
        iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            endIt = upper_bound(key);
        }
        return bsl::pair<iterator, iterator>(startIt, endIt);
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<iterator, iterator> >::type
    equal_range(const LOOKUP_KEY& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multimap whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second is positioned
        // one past the end of the sequence.  The first returned iterator will
        // be 'lower_bound(key)', the second returned iterator will be
        // 'upper_bound(key)', and, if this multimap contains no 'value_type'
        // object with an equivalent key, then the two returned iterators will
        // have the same value.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        iterator startIt = lower_bound(key);
        iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            endIt = upper_bound(key);
        }
        return pair<iterator, iterator>(startIt, endIt);
    }

    // BDE_VERIFY pragma: pop

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // multimap.

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or the 'end' iterator if this multimap
        // is empty.

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this multimap.

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this multimap, or 'rend' if this multimap is
        // empty.

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multimap.

    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or the 'cend' iterator if this multimap
        // is empty.

    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this multimap.

    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this multimap, or 'rend' if this multimap is
        // empty.

    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multimap.

    bool contains(const key_type &key) const;
        // Return 'true' if this multimap contains an element whose key is
        // equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        bool>::type
    contains(const LOOKUP_KEY& key) const
        // Return 'true' if this multimap contains an element whose key is
        // equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error
    {
        return find(key) != end();
    }

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this multimap contains no elements, and 'false'
        // otherwise.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this multimap.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this multimap could possibly hold.  Note that there is no
        // guarantee that the multimap can successfully grow to the returned
        // size, or even close to that size without running out of resources.

    key_compare key_comp() const;
        // Return the key-comparison functor (or function pointer) used by this
        // multimap; if a comparator was supplied at construction, return its
        // value, otherwise return a default constructed 'key_compare' object.
        // Note that this comparator compares objects of type 'KEY', which is
        // the key part of the 'value_type' objects contained in this multimap.

    value_compare value_comp() const;
        // Return a functor for comparing two 'value_type' objects by comparing
        // their respective keys using 'key_comp()'.   Note that this
        // comparator compares objects of type 'value_type' (i.e., 'pair<const
        // KEY, VALUE>').

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    const_iterator find(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object having the specified 'key' in the ordered
        // sequence maintained by this multimap, if such an object exists, and
        // the past-the-end ('end') iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        const_iterator>::type
    find(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object having the specified 'key' in the ordered
        // sequence maintained by this multimap, if such an object exists, and
        // the past-the-end ('end') iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    size_type count(const key_type& key) const
        // Return the number of 'value_type' objects within this multimap whose
        // keys are equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        int            count = 0;
        const_iterator it    = lower_bound(key);

        while (it != end() && !comparator()(key, *it.node())) {
            ++it;
            ++count;
        }
        return count;
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        size_type>::type
    count(const LOOKUP_KEY& key) const
        // Return the number of 'value_type' objects within this multimap whose
        // keys are equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        int            count = 0;
        const_iterator it    = lower_bound(key);

        while (it != end() && !comparator()(key, *it.node())) {
            ++it;
            ++count;
        }
        return count;
    }

    const_iterator lower_bound(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multimap whose key
        // is greater-than or equal-to the specified 'key', and the
        // past-the-end iterator if this multimap does not contain a
        // 'value_type' object whose key is greater-than or equal-to 'key'.
        // Note that this function returns the *first* position before which a
        // 'value_type' object having an equivalent key could be inserted into
        // the ordered sequence maintained by this multimap, while preserving
        // its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        const_iterator>::type
    lower_bound(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multimap whose key
        // is greater-than or equal-to the specified 'key', and the
        // past-the-end iterator if this multimap does not contain a
        // 'value_type' object whose key is greater-than or equal-to 'key'.
        // Note that this function returns the *first* position before which a
        // 'value_type' object having an equivalent key could be inserted into
        // the ordered sequence maintained by this multimap, while preserving
        // its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    const_iterator upper_bound(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multimap whose key
        // is greater than the specified 'key', and the past-the-end iterator
        // if this multimap does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having an equivalent key
        // could be inserted into the ordered sequence maintained by this
        // multimap, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        const_iterator>::type
    upper_bound(const LOOKUP_KEY& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multimap whose key
        // is greater than the specified 'key', and the past-the-end iterator
        // if this multimap does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having an equivalent key
        // could be inserted into the ordered sequence maintained by this
        // multimap, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this multimap whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second iterator is
        // positioned one past the end of the sequence.  The first returned
        // iterator will be 'lower_bound(key)', the second returned iterator
        // will be 'upper_bound(key)', and, if this multimap contains no
        // 'value_type' objects having keys equivalent to 'key', then the two
        // returned iterators will have the same value.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        const_iterator startIt = lower_bound(key);
        const_iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            endIt = upper_bound(key);
        }
        return bsl::pair<const_iterator, const_iterator>(startIt, endIt);
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<const_iterator, const_iterator> >::type
    equal_range(const LOOKUP_KEY& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this multimap whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second iterator is
        // positioned one past the end of the sequence.  The first returned
        // iterator will be 'lower_bound(key)', the second returned iterator
        // will be 'upper_bound(key)', and, if this multimap contains no
        // 'value_type' objects having keys equivalent to 'key', then the two
        // returned iterators will have the same value.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        const_iterator startIt = lower_bound(key);
        const_iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            endIt = upper_bound(key);
        }
        return pair<const_iterator, const_iterator>(startIt, endIt);
    }

    // BDE_VERIFY pragma: pop
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
// CLASS TEMPLATE DEDUCTION GUIDES

template <
    class INPUT_ITERATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<
             BloombergLP::bslstl::IteratorUtil::IterToAlloc_t<INPUT_ITERATOR>>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multimap(INPUT_ITERATOR,
    INPUT_ITERATOR,
    COMPARATOR = COMPARATOR(),
    ALLOCATOR = ALLOCATOR())
-> multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'multimap'.  Deduce the
    // template parameters 'COMPARATOR' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.  This deduction guide does not
    // participate unless the supplied allocator meets the requirements of a
    // standard allocator.

template <
    class INPUT_ITERATOR,
    class COMPARATOR,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multimap(INPUT_ITERATOR, INPUT_ITERATOR, COMPARATOR, ALLOC *)
-> multimap<KEY, VALUE, COMPARATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'multimap'.  Deduce the
    // template parameter 'COMPARATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multimap(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR)
-> multimap<KEY, VALUE, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'multimap'.  This
    // deduction guide does not participate unless the supplied allocator meets
    // the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multimap(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> multimap<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'multimap'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multimap(std::initializer_list<pair<const KEY, VALUE>>,
         COMPARATOR = COMPARATOR(),
         ALLOCATOR = ALLOCATOR())
-> multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'multimap'.
    // Deduce the template parameters 'COMPARATOR' and 'ALLOCATOR' from the
    // other parameters passed to the constructor.  This deduction guide does
    // not participate unless the supplied allocator meets the requirements of
    // a standard allocator.

template <
    class KEY,
    class VALUE,
    class COMPARATOR,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multimap(std::initializer_list<pair<const KEY, VALUE>>, COMPARATOR, ALLOC *)
-> multimap<KEY, VALUE, COMPARATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'multimap'.
    // Deduce the template parameter 'COMPARATOR' from the other parameters
    // passed to the constructor.  This deduction guide does not participate
    // unless the supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multimap(std::initializer_list<pair<const KEY, VALUE>>, ALLOCATOR)
-> multimap<KEY, VALUE, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'multimap'.
    // Deduce the template parameter 'ALLOCATOR' from the other parameter
    // passed to the constructor.  This deduction guide does not participate
    // unless the supplied allocator meets the requirements of a standard
    // allocator.

template <
    class KEY,
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multimap(std::initializer_list<pair<const KEY, VALUE>>, ALLOC *)
-> multimap<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'multimap'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.
#endif

// FREE OPERATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator==(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'multimap' objects 'lhs' and 'rhs'
    // have the same value if they have the same number of key-value pairs, and
    // each element in the ordered sequence of key-value pairs of 'lhs' has the
    // same value as the corresponding element in the ordered sequence of
    // key-value pairs of 'rhs'.  This method requires that the (template
    // parameter) types 'KEY' and 'VALUE' both be 'equality-comparable' (see
    // {Requirements on 'KEY' and 'VALUE'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator!=(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'multimap' objects 'lhs' and
    // 'rhs' do not have the same value if they do not have the same number of
    // key-value pairs, or some element in the ordered sequence of key-value
    // pairs of 'lhs' does not have the same value as the corresponding element
    // in the ordered sequence of key-value pairs of 'rhs'.  This method
    // requires that the (template parameter) types 'KEY' and 'VALUE' both be
    // 'equality-comparable' (see {Requirements on 'KEY' and 'VALUE'}).
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
BloombergLP::bslalg::SynthThreeWayUtil::Result<pair<const KEY, VALUE>>
operator<=>(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
            const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' maps by using the comparison operators of
    // 'bsl::pair<const KEY, VALUE>' on each element; return the result of that
    // comparison.

#else

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator<(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multimap is
    // lexicographically less than that of the specified 'rhs' multimap, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of multimap 'lhs' is lexicographically less than that of
    // multimap 'rhs' if 'true == *i < *j' for the first pair of corresponding
    // iterator positions where '*i < *j' and '*j < *i' are not both 'false'.
    // If no such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator>(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multimap is
    // lexicographically greater than that of the specified 'rhs' multimap, and
    // 'false' otherwise.  The value of multimap 'lhs' is lexicographically
    // greater than that of multimap 'rhs' if 'rhs' is lexicographically less
    // than 'lhs' (see 'operator<').  This method requires that 'operator<',
    // inducing a total order, be defined for 'value_type'.  Note that this
    // operator returns 'rhs < lhs'.

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator<=(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multimap is
    // lexicographically less than or equal to that of the specified 'rhs'
    // multimap, and 'false' otherwise.  The value of multimap 'lhs' is
    // lexicographically less than or equal to that of multimap 'rhs' if 'rhs'
    // is not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator>=(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multimap is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // multimap, and 'false' otherwise.  The value of multimap 'lhs' is
    // lexicographically greater than or equal to that of multimap 'rhs' if
    // 'lhs' is not lexicographically less than 'rhs' (see 'operator<').  This
    // method requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
void swap(multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
          multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& b)
             BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false);
    // Exchange the value and comparator of the specified 'a' object with those
    // of the specified 'b' object; also exchange the allocator of 'a' with
    // that of 'b' if the (template parameter) type 'ALLOCATOR' has the
    // 'propagate_on_container_swap' trait, and do not modify either allocator
    // otherwise.  This function provides the no-throw exception-safety
    // guarantee if and only if the (template parameter) type 'COMPARATOR'
    // provides a no-throw swap operation, and provides the basic
    // exception-safety guarantee otherwise; if an exception is thrown, both
    // objects are left in valid but unspecified states.  This operation has
    // 'O[1]' complexity if either 'a' was created with the same allocator as
    // 'b' or 'ALLOCATOR' has the 'propagate_on_container_swap' trait;
    // otherwise, it has 'O[n + m]' complexity, where 'n' and 'm' are the
    // number of elements in 'a' and 'b', respectively.  Note that this
    // function's support for swapping objects created with different
    // allocators when 'ALLOCATOR' does not have the
    // 'propagate_on_container_swap' trait is a departure from the C++
    // Standard.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class DataWrapper
                             // -----------------

// CREATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                                              const COMPARATOR& comparator,
                                              const ALLOCATOR&  basicAllocator)
: ::bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator(comparator)
, d_pool(basicAllocator)
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                          BloombergLP::bslmf::MovableRef<DataWrapper> original)
: ::bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator(
                                    MoveUtil::access(original).keyComparator())
, d_pool(MoveUtil::move(MoveUtil::access(original).d_pool))
{
}

// MANIPULATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory()
{
    return d_pool;
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory() const
{
    return d_pool;
}

                             // -----------------------------
                             // class multimap::value_compare
                             // -----------------------------

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::value_compare(
                                                         COMPARATOR comparator)
: comp(comparator)
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::operator()(
                                                     const value_type& x,
                                                     const value_type& y) const
{
    return comp(x.first, y.first);
}

                             // --------------
                             // class multimap
                             // --------------

// PRIVATE MANIPULATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::comparator()
{
    return d_compAndAlloc;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_compAndAlloc.nodeFactory();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::quickSwapExchangeAllocators(
                                                               multimap& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swapExchangeAllocators(other.nodeFactory());

    // 'DataWrapper' contains a 'NodeFactory' object and inherits from
    // 'Comparator'.  If the empty-base-class optimization has been applied to
    // 'Comparator', then we must not call 'swap' on it because
    // 'sizeof(Comparator) > 0' and, therefore, we will incorrectly swap bytes
    // of the 'NodeFactory' members!

    if (sizeof(NodeFactory) != sizeof(DataWrapper)) {
        comparator().swap(other.comparator());
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::quickSwapRetainAllocators(
                                                               multimap& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swapRetainAllocators(other.nodeFactory());

    // See 'quickSwapExchangeAllocators' (above).

    if (sizeof(NodeFactory) != sizeof(DataWrapper)) {
        comparator().swap(other.comparator());
    }
}

// PRIVATE ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_compAndAlloc;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory() const
{
    return d_compAndAlloc.nodeFactory();
}

// CREATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap()
: d_compAndAlloc(COMPARATOR(), ALLOCATOR())
, d_tree()
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                                               const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(const multimap& original)
: d_compAndAlloc(original.comparator().keyComparator(),
                 AllocatorTraits::select_on_container_copy_construction(
                                           original.nodeFactory().allocator()))
, d_tree()
{
    if (0 < original.size()) {
        nodeFactory().reserveNodes(original.size());
        BloombergLP::bslalg::RbTreeUtil::copyTree(&d_tree,
                                                  original.d_tree,
                                                  &nodeFactory());
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                             BloombergLP::bslmf::MovableRef<multimap> original)
: d_compAndAlloc(MoveUtil::move(MoveUtil::access(original).d_compAndAlloc))
, d_tree()
{
    multimap& lvalue = original;
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                 const multimap&                                original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_compAndAlloc(original.comparator().keyComparator(), basicAllocator)
, d_tree()
{
    if (0 < original.size()) {
        nodeFactory().reserveNodes(original.size());
        BloombergLP::bslalg::RbTreeUtil::copyTree(&d_tree,
                                                  original.d_tree,
                                                  &nodeFactory());
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                 BloombergLP::bslmf::MovableRef<multimap> original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_compAndAlloc(MoveUtil::access(original).comparator().keyComparator(),
                 basicAllocator)
, d_tree()
{
    multimap& lvalue = original;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
              nodeFactory().allocator() == lvalue.nodeFactory().allocator())) {
        d_compAndAlloc.nodeFactory().adopt(
                          MoveUtil::move(lvalue.d_compAndAlloc.nodeFactory()));
        BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
    }
    else {
        if (0 < lvalue.size()) {
            nodeFactory().reserveNodes(lvalue.size());
            BloombergLP::bslalg::RbTreeUtil::moveTree(&d_tree,
                                                      &lvalue.d_tree,
                                                      &nodeFactory(),
                                                      &lvalue.nodeFactory());
        }
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                                              INPUT_ITERATOR    first,
                                              INPUT_ITERATOR    last,
                                              const COMPARATOR& comparator,
                                              const ALLOCATOR&  basicAllocator)
: d_compAndAlloc(comparator, basicAllocator)
, d_tree()
{
    if (first != last) {

        size_type numElements =
                BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);

        if (0 < numElements) {
            nodeFactory().reserveNodes(numElements);
        }

        BloombergLP::bslalg::RbTreeUtilTreeProctor<NodeFactory> proctor(
                                                               &d_tree,
                                                               &nodeFactory());

        // The following loop guarantees amortized linear time to insert an
        // ordered sequence of values (as required by the standard).   If the
        // values are in sorted order, we are guaranteed the next node can be
        // inserted as the right child of the previous node, and can call
        // 'insertAt'.

        insert(*first);
        BloombergLP::bslalg::RbTreeNode *prevNode = d_tree.rootNode();
        while (++first != last) {
            // The values are not in order, so insert them normally.

            const value_type& value = *first;
            if (this->comparator()(value.first, *prevNode)) {
                insert(value);
                insert(++first, last);
                break;
            }
            BloombergLP::bslalg::RbTreeNode *node =
                nodeFactory().emplaceIntoNewNode(value);
            BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                                      prevNode,
                                                      false,
                                                      node);
            prevNode = node;
        }
        proctor.release();
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                                              INPUT_ITERATOR    first,
                                              INPUT_ITERATOR    last,
                                              const ALLOCATOR&  basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
    if (first != last) {

        size_type numElements =
                BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);

        if (0 < numElements) {
            nodeFactory().reserveNodes(numElements);
        }

        BloombergLP::bslalg::RbTreeUtilTreeProctor<NodeFactory> proctor(
                                                               &d_tree,
                                                               &nodeFactory());

        // The following loop guarantees amortized linear time to insert an
        // ordered sequence of values (as required by the standard).   If the
        // values are in sorted order, we are guaranteed the next node can be
        // inserted as the right child of the previous node, and can call
        // 'insertAt'.

        insert(*first);
        BloombergLP::bslalg::RbTreeNode *prevNode = d_tree.rootNode();
        while (++first != last) {
            // The values are not in order, so insert them normally.

            const value_type& value = *first;
            if (this->comparator()(value.first, *prevNode)) {
                insert(value);
                insert(++first, last);
                break;
            }
            BloombergLP::bslalg::RbTreeNode *node =
                nodeFactory().emplaceIntoNewNode(value);
            BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                                      prevNode,
                                                      false,
                                                      node);
            prevNode = node;
        }
        proctor.release();
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                              std::initializer_list<value_type> values,
                              const COMPARATOR&                 comparator,
                              const ALLOCATOR&                  basicAllocator)
: multimap(values.begin(), values.end(), comparator, basicAllocator)
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                              std::initializer_list<value_type> values,
                              const ALLOCATOR&                  basicAllocator)
: multimap(values.begin(), values.end(), COMPARATOR(), basicAllocator)
{
}
#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::~multimap()
{
    clear();
}

// MANIPULATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator=(const multimap& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        if (AllocatorTraits::propagate_on_container_copy_assignment::value) {
            multimap other(rhs, rhs.nodeFactory().allocator());
            quickSwapExchangeAllocators(other);
        }
        else {
            multimap other(rhs, nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator=(
                                  BloombergLP::bslmf::MovableRef<multimap> rhs)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                            AllocatorTraits::is_always_equal::value &&
                            std::is_nothrow_move_assignable<COMPARATOR>::value)
{
    multimap& lvalue = rhs;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &lvalue)) {
        if (nodeFactory().allocator() == lvalue.nodeFactory().allocator()) {
            multimap other(MoveUtil::move(lvalue));
            quickSwapRetainAllocators(other);
        }
        else if (
              AllocatorTraits::propagate_on_container_move_assignment::value) {
            multimap other(MoveUtil::move(lvalue));
            quickSwapExchangeAllocators(other);
        }
        else {
            multimap other(MoveUtil::move(lvalue), nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator=(
                                      std::initializer_list<value_type> values)
{
    clear();
    insert(values.begin(), values.end());
    return *this;
}
#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::end() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const value_type& value)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value.first);

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(value);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                         INPUT_ITERATOR last)
{
    ///Implementation Notes
    ///--------------------
    // First, consume currently held free nodes.  Tf those nodes are
    // insufficient *and* one can calculate the remaining number of elements,
    // then reserve exactly that many free nodes.  There is no more than one
    // call to 'reserveNodes' per invocation of this method, hence the use of
    // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.

    const bool canCalculateInsertDistance =
             is_convertible<typename
                            iterator_traits<INPUT_ITERATOR>::iterator_category,
                            forward_iterator_tag>::value;

    while (first != last) {
        if (canCalculateInsertDistance
        && BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                              !nodeFactory().hasFreeNodes())) {
            const size_type numElements =
                BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);

            nodeFactory().reserveNodes(numElements);
        }
        insert(*first);
        ++first;
    }
}

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const_iterator first,
                                                         const_iterator last)
{
    while (first != last) {
        insert(*first);
        ++first;
    }
}
#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const_iterator    hint,
                                                    const value_type& value)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value.first,
                                                            hintNode);

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(value);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(
                                      std::initializer_list<value_type> values)
{
    insert(values.begin(), values.end());
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::emplace(Args&&... args)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        BSLS_COMPILERFEATURES_FORWARD(Args, args)...);

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(
                               &leftChild,
                               &d_tree,
                               this->comparator(),
                               static_cast<const Node *>(node)->value().first);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::emplace_hint(const_iterator hint,
                                                          Args&&...      args)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        BSLS_COMPILERFEATURES_FORWARD(Args, args)...);

    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(
                                &leftChild,
                                &d_tree,
                                this->comparator(),
                                static_cast<const Node *>(node)->value().first,
                                hintNode);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position != end());

    BloombergLP::bslalg::RbTreeNode *node =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(position.node());
    BloombergLP::bslalg::RbTreeNode *result =
                                   BloombergLP::bslalg::RbTreeUtil::next(node);
    BloombergLP::bslalg::RbTreeUtil::remove(&d_tree, node);
    nodeFactory().deleteNode(node);
    return iterator(result);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(iterator position)
{
    return erase(const_iterator(position));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const key_type& key)
{
    size_type      count = 0;
    const_iterator first = find(key);

    if (first != end()) {
        const_iterator last = upper_bound(key);
        while (first != last) {
            first = erase(first);
            ++count;
        }
    }
    return count;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const_iterator first,
                                                   const_iterator last)
{
    while (first != last) {
        first = erase(first);
    }
    return iterator(last.node());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::swap(multimap& other)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                  AllocatorTraits::is_always_equal::value &&
                                  bsl::is_nothrow_swappable<COMPARATOR>::value)
{
    if (AllocatorTraits::propagate_on_container_swap::value) {
        quickSwapExchangeAllocators(other);
    }
    else {
        // C++11 behavior for member 'swap': undefined for unequal allocators.
        // BSLS_ASSERT(allocator() == other.allocator());

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
               nodeFactory().allocator() == other.nodeFactory().allocator())) {
            quickSwapRetainAllocators(other);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

            multimap toOtherCopy(MoveUtil::move(*this),
                                 other.nodeFactory().allocator());
            multimap toThisCopy( MoveUtil::move(other),
                                 nodeFactory().allocator());

            this->quickSwapRetainAllocators(toThisCopy);
            other.quickSwapRetainAllocators(toOtherCopy);
        }
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(d_tree.firstNode());

    if (d_tree.rootNode()) {
        BSLS_ASSERT_SAFE(0 < d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() != d_tree.sentinel());

        BloombergLP::bslalg::RbTreeUtil::deleteTree(&d_tree, &nodeFactory());
    }
#if defined(BSLS_ASSERT_SAFE_IS_USED)
    else {
        BSLS_ASSERT_SAFE(0 == d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() == d_tree.sentinel());
    }
#endif
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::allocator_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return nodeFactory().allocator();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return cbegin();
}


template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return cend();
}


template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return crbegin();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return crend();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::cbegin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::crbegin() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::crend() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::contains(
                                                     const key_type& key) const
{
    return find(key) != end();
}

// capacity:
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::empty() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::max_size() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::key_compare
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::key_comp() const
{
    return comparator().keyComparator();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_comp() const
{
    return value_compare(key_comp());
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator==(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator!=(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
BloombergLP::bslalg::SynthThreeWayUtil::Result<bsl::pair<const KEY, VALUE>>
bsl::operator<=>(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                 const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator<(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator>(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator<=(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}


template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator>=(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void bsl::swap(bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
               bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& b)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *ordered* containers:
//: o An ordered container defines STL iterators.
//: o An ordered container uses 'bslma' allocators if the (template parameter)
//:   type 'ALLOCATOR' is convertible from 'bslma::Allocator *'.

namespace BloombergLP {

namespace bslalg {

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
struct HasStlIterators<bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::true_type
{
};

}  // close namespace bslalg

namespace bslma {

template <class KEY,   class VALUE, class COMPARATOR, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma

}  // close enterprise namespace

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
