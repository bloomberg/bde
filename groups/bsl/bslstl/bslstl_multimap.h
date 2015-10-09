// bslstl_multimap.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_MULTIMAP
#define INCLUDED_BSLSTL_MULTIMAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant multimap class.
//
//@CLASSES:
//   bsl::multimap: STL-compatible multimap template
//
//@SEE_ALSO: bslstl_map, bslstl_multiset
//
//@DESCRIPTION: This component defines a single class template 'bsl::multimap',
// implementing the standard container holding an ordered sequence of key-value
// pairs (possibly having duplicate keys), and presenting a mapping from the
// keys (of a template parameter type, 'KEY') to their associated values (of
// another template parameter type, 'VALUE').
//
// An instantiation of 'multimap' is an allocator-aware, value-semantic type
// whose salient attributes are its size (number of key-value pairs) and the
// ordered sequence of key-value pairs the multimap contains.  If 'multimap' is
// instantiated with either a key type or mapped-value type that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// In particular, if a either the key or value type cannot be tested for
// equality, then a 'multimap' containing that type cannot be tested for
// equality.  It is even possible to instantiate 'multimap' with a key or
// mapped-value type that does not have a copy-constructor, in which case the
// 'multimap' will not be copyable.
//
// A 'multimap' meets the requirements of an associative container with
// bidirectional iterators in the C++11 standard [23.2.4].  The 'multimap'
// implemented here adheres to the C++11 standard, except that it does not have
// interfaces that take rvalue references, 'initializer_lists', 'emplace', or
// operations taking a variadic number of template parameters.  Note that
// excluded C++11 features are those that require (or are greatly simplified
// by) C++11 compiler support.
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
// multimap's entry type, 'value_type', which is an alias for 'bsl::pair<KEY,
// VALUE>'.
//
//: "default-constructible": The type provides a default constructor.
//:
//: "copy-constructible": The type provides a copy constructor.
//:
//: "equality-comparable": The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
//:
//: "less-than-comparable": The type provides a less-than operator, which
//:     defines a strict weak ordering relation on values of the type.
//
///Memory Allocation
///-----------------
// The type supplied as a multimap's 'ALLOCATOR' template parameter determines
// how that multimap will allocate memory.  The 'multimap' template supports
// allocators meeting the requirements of the C++11 standard [17.6.3.5], in
// addition it supports scoped-allocators derived from the 'bslma::Allocator'
// memory allocation protocol.  Clients intending to use 'bslma' style
// allocators should use the template's default 'ALLOCATOR' type: The default
// type for the 'ALLOCATOR' template parameter, 'bsl::allocator', provides a
// C++11 standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of an 'multimap' instantiation'
// is 'bsl::allocator', then objects of that multimap type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a multimap
// accepts an optional 'bslma::Allocator' argument at construction.  If the
// address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it is used to supply memory for the 'multimap' throughout its
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
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'value_type'    - 'multimap<K, V>::value_type'
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - an STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'k'             - an object of type 'K'
//  'v'             - an object of type 'V'
//  'p1', 'p2'      - two iterators belonging to 'a'
//  distance(i1,i2) - the number of elements in the range [i1, i2)
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | multimap<K, V> a;    (default construction)        | O[1]               |
//  | multimap<K, V> a(al);                              |                    |
//  | multimap<K, V> a(c, al);                           |                    |
//  +----------------------------------------------------+--------------------+
//  | multimap<K, V> a(b); (copy construction)           | O[n]               |
//  | multimap<K, V> a(b, al);                           |                    |
//  +----------------------------------------------------+--------------------+
//  | multimap<K, V> a(i1, i2);                          | O[N] if [i1, i2)   |
//  | multimap<K, V> a(i1, i2, al);                      | is sorted with     |
//  | multimap<K, V> a(i1, i2, c, al);                   | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where N |
//  |                                                    | is distance(i1,i2) |
//  +----------------------------------------------------+--------------------+
//  | a.~multimap<K, V>(); (destruction)                 | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  | a.rbegin(), a.rend(), a.crbegin(), a.crend()       |                    |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a < b, a <= b, a > b, a >= b                       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a,b)                               | O[1] if 'a' and    |
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
//  | a.insert(value_type(k, v))                         | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, value_type(k, v))                     | amortized constant |
//  |                                                    | if the value is    |
//  |                                                    | inserted right     |
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
// for 'pair'):
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
//      // number.  Names within a 'Phonebook' are represented using a using
//      // 'FirstAndLastName' object, and phone numbers are represented using a
//      // 'bsls::Types::Uint64' value.
//
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
//      int numEntries() const;
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
//
//      bsl::pair<NameToNumberMap::iterator, NameToNumberMap::iterator> range =
//                                            d_nameToNumber.equal_range(name);
//
//      NameToNumberMap::iterator itr = range.first;
//      int numRemovedEntries = 0;
//
//      while (itr != range.second) {
//          if (itr->second == number) {
//              itr = d_nameToNumber.erase(itr);
//              ++numRemovedEntries;
//          }
//          else {
//              ++itr;
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
//  int PhoneBook::numEntries() const
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

#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_map.h> instead of <bslstl_multimap.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_MAPCOMPARATOR
#include <bslstl_mapcomparator.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLSTL_TREEITERATOR
#include <bslstl_treeiterator.h>
#endif

#ifndef INCLUDED_BSLSTL_TREENODE
#include <bslstl_treenode.h>
#endif

#ifndef INCLUDED_BSLSTL_TREENODEPOOL
#include <bslstl_treenodepool.h>
#endif

#ifndef INCLUDED_BSLALG_RANGECOMPARE
#include <bslalg_rangecompare.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREEANCHOR
#include <bslalg_rbtreeanchor.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

#ifndef INCLUDED_BSLALG_RBTREEUTIL
#include <bslalg_rbtreeutil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

namespace bsl {

                             // ==============
                             // class multimap
                             // ==============

template <class KEY,
          class VALUE,
          class COMPARATOR  = std::less<KEY>,
          class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class multimap {
    // This class template implements a value-semantic container type holding
    // an ordered sequence of key-value pairs having possibly duplicate keys
    // that provide a mapping from keys (of the template parameter type, 'KEY')
    // to their associated values (of another template parameter type,
    // 'VALUE').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    // PRIVATE TYPES
    typedef bsl::pair<const KEY, VALUE> ValueType;
        // This typedef is an alias for the type of key-value pair objects
        // maintained by this multimap.

    typedef BloombergLP::bslstl::MapComparator<KEY, VALUE, COMPARATOR>
                                                                    Comparator;
        // This typedef is an alias for the comparator used internally by this
        // multimap.

    typedef BloombergLP::bslstl::TreeNode<ValueType> Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this multimap.

    typedef BloombergLP::bslstl::TreeNodePool<ValueType, ALLOCATOR>
                                                                   NodeFactory;
        // This typedef is an alias for the factory type used to create and
        // destroy 'Node' objects.

    typedef typename bsl::allocator_traits<ALLOCATOR> AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

    struct DataWrapper : public Comparator {
        // This struct is wrapper around the comparator and allocator data
        // members.  It takes advantage of the empty-base optimization (EBO) so
        // that if the allocator is stateless, it takes up no space.
        //
        // TBD: This struct should eventually be replaced by the use of a
        // general EBO-enabled component that provides a 'pair'-like interface
        // or a 'tuple'.

        NodeFactory d_pool;  // pool of 'Node' objects

        explicit DataWrapper(const COMPARATOR&  comparator,
                             const ALLOCATOR&   basicAllocator);
            // Create a 'DataWrapper' object with the specified 'comparator'
            // and 'basicAllocator'.
    };

    // DATA
    DataWrapper                       d_compAndAlloc;
                                               // comparator and pool of 'Node'
                                               // objects

    BloombergLP::bslalg::RbTreeAnchor d_tree;  // balanced tree of 'Node'
                                               // objects

  private:
    // PRIVATE MANIPULATORS
    NodeFactory& nodeFactory();
        // Return a reference providing modifiable access to the
        // node-allocator for this tree.

    Comparator& comparator();
        // Return a reference providing modifiable access to the comparator for
        // this tree.

    void quickSwap(multimap& other);
        // Efficiently exchange the value and comparator of this object with
        // the value of the specified 'other' object.  This method provides the
        // no-throw exception-safety guarantee.  The behavior is undefined
        // unless this object was created with the same allocator as 'other'.

    // PRIVATE ACCESSORS
    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the
        // node-allocator for this tree.

    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this tree.

  public:
    // PUBLIC TYPES
    typedef KEY                                        key_type;
    typedef VALUE                                      mapped_type;
    typedef bsl::pair<const KEY, VALUE>                value_type;
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
        // the (template parameter) type 'COMPARATOR'.  Note that this class
        // exactly matches its definition in the C++11 standard [23.4.5.1];
        // otherwise we would have implemented it as a separate component-local
        // class.

        // FRIENDS
        friend class multimap;
      protected:
        COMPARATOR comp;  // we would not have elected to make this data
                          // member protected ourselves

        value_compare(COMPARATOR c);                                // IMPLICIT
            // Create a 'value_compare' object that will delegate to the
            // specified 'comparator' for comparisons.

      public:
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

        bool operator()(const value_type& x, const value_type& y) const;
            // Return 'true' if the specified 'x' object is ordered before the
            // specified 'y' object, as determined by the comparator supplied
            // at construction.
    };

  public:
    // CREATORS
    explicit multimap(const COMPARATOR& comparator     = COMPARATOR(),
                      const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Construct an empty multimap.  Optionally specify a 'comparator' used
        // to order key-value pairs contained in this object.  If 'comparator'
        // is not supplied, a default-constructed object of the (template
        // parameter) type 'COMPARATOR' is used.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type ALLOCATOR' is used.  If the 'ALLOCATOR' argument is of type
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR'
        // argument is of type 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.
    : d_compAndAlloc(comparator, basicAllocator)
    , d_tree()
    {
        // The implementation is placed here in the class definition to
        // workaround an AIX compiler bug, where the constructor can fail to
        // compile because it is unable to find the definition of the default
        // argument.  This occurs when a templatized class wraps around the
        // container and the comparator is defined after the new class.
    }

    explicit multimap(const ALLOCATOR& basicAllocator);
        // Construct an empty multimap that will use the specified
        // 'basicAllocator' to supply memory.  Use a default-constructed object
        // of the (template parameter) type 'COMPARATOR' to order the key-value
        // pairs contained in this multimap.  If the template parameter
        // 'ALLOCATOR' argument is of type 'bsl::allocator' (the default), then
        // 'basicAllocator' shall be convertible to 'bslma::Allocator *'.

    multimap(const multimap& original);
        // Construct a multimap having the same value as the specified
        // 'original'.  Use a copy of 'original.key_comp()' to order the
        // key-value pairs contained in this multimap.  Use the allocator
        // returned by 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.allocator())' to
        // allocate memory.  If the (template parameter) type 'ALLOCATOR' is of
        // type 'bsl::allocator' (the default), the currently installed default
        // allocator is used to supply memory.  Note that this method requires
        // that the (template parameter) types 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    multimap(const multimap& original, const ALLOCATOR& basicAllocator);
        // Construct a multimap having the same value as that of the specified
        // 'original' that will use the specified 'basicAllocator' to supply
        // memory.  Use a copy of 'original.key_comp()' to order the key-value
        // pairs contained in this multimap.  If the template parameter
        // 'ALLOCATOR' argument is of type 'bsl::allocator' (the default), then
        // 'basicAllocator' shall be convertible to 'bslma::Allocator *'.  Note
        // that this method requires that the (template parameter) types 'KEY'
        // and 'VALUE' both be "copy-constructible" (see {Requirements on 'KEY'
        // and 'VALUE'}).

    template <class INPUT_ITERATOR>
    multimap(INPUT_ITERATOR    first,
             INPUT_ITERATOR    last,
             const COMPARATOR& comparator = COMPARATOR(),
             const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Construct a multimap, and insert each 'value_type' object in the
        // sequence starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element, ignoring those
        // pairs having a key that appears earlier in the sequence.  Optionally
        // specify a 'comparator' used to order key-value pairs contained in
        // this object.  If 'comparator' is not supplied, a default-constructed
        // object of the (template parameter) type 'COMPARATOR' is used.
        // Optionally specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, a default-constructed object of
        // the (template parameter) type 'ALLOCATOR' is used.  If the template
        // parameter 'ALLOCATOR' argument is of type 'bsl::allocator' (the
        // default), then 'basicAllocator', if supplied, shall be convertible
        // to 'bslma::Allocator *'.  If the template parameter 'ALLOCATOR'
        // argument is of type 'bsl::allocator' and 'basicAllocator' is not
        // supplied, the currently installed default allocator is used to
        // supply memory.  If the sequence 'first' and 'last' is ordered
        // according to the identified 'comparator', then this operation has
        // 'O[N]' complexity, where 'N' is the number of elements between
        // 'first' and 'last', otherwise this operation has 'O[N * log(N)]'
        // complexity.  The (template parameter) type 'INPUT_ITERATOR' shall
        // meet the requirements of an input iterator defined in the C++11
        // standard [24.2.3] providing access to values of a type convertible
        // to 'value_type'.  The behavior is undefined unless 'first' and
        // 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that this method requires that
        // the (template parameter) types 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    ~multimap();
        // Destroy this object;

    // MANIPULATORS
    multimap& operator=(const multimap& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // Note that this method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be "copy-constructible" (see {Requirements on
        // 'KEY' and 'VALUE'}).

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or the 'end' iterator if this multimap
        // is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this multimap.

    reverse_iterator rbegin();
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or 'rend' if this multimap is empty.

    reverse_iterator rend();
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multimap.

    iterator insert(const value_type& value);
        // Insert the specified 'value' into this multimap.  If a range
        // containing elements equivalent to 'value' already exist, insert
        // 'value' at the end of that range.  Return an iterator referring to
        // the newly inserted 'value_type' object.  Note that this method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this multimap as close as possible
        // to the position just prior to the specified 'hint' (in amortized
        // constant time if the 'hint' is a valid immediate successor to the
        // key of 'value').  If 'hint' is not a valid immediate successor to
        // the key of 'value', this operation has 'O[log(N)]' complexity, where
        // 'N' is the size of this multimap.  The behavior is undefined unless
        // 'hint' is a valid iterator into this multimap.  Note that this
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be "copy-constructible" (see {Requirements on 'KEY' and
        // 'VALUE'}).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this multimap the value of each 'value_type' object in
        // the range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type'.  The
        // behavior is undefined unless 'first' and 'last' refer to a sequence
        // of valid values where 'first' is at a position at or before 'last'.
        // Note that this method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be "copy-constructible" (see {Requirements on
        // 'KEY' and 'VALUE'}).

    iterator erase(const_iterator position);
        // Remove from this multimap the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this multimap.  The behavior is undefined
        // unless 'position' refers to a 'value_type' object in this multimap.

    size_type erase(const key_type& key);
        // Remote from this multimap all 'value_type' objects having the
        // specified 'key', if they exist, and return the number of erased
        // objects; otherwise, if there is no 'value_type' objects having
        // 'key', return 0 with no other effect.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this multimap the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this multimap or are
        // the 'end' iterator, and the 'first' position is at or before the
        // 'last' position in the ordered sequence provided by this container.

    void swap(multimap& other);
        // Exchange the value of this object as well as its comparator with
        // those of the specified 'other' object.  Additionally, if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true', then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees 'O[1]' complexity.  The behavior is undefined unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.

    void clear();
        // Remove all entries from this multimap.  Note that the multimap is
        // empty after this call, but allocated memory may be retained for
        // future use.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the first
        // 'value_type' object having the specified 'key' in ordered sequence
        // maintained by this multimap, if such an object exists; otherwise,
        // return the past-the-end ('end') iterator.

    iterator lower_bound(const key_type& key);
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multimap whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this multimap does not contain a 'value_type' object
        // whose key is greater-than or equal-to 'key'.  Note that this
        // function returns the *first* position before which a 'value_type'
        // object having 'key' could be inserted into the ordered sequence
        // maintained by this multimap, while preserving its ordering.

    iterator upper_bound(const key_type& key);
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multimap whose key is
        // greater than the specified 'key', and the past-the-end iterator if
        // this multimap does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having 'key' could be
        // inserted into the ordered sequence maintained by this multimap,
        // while preserving its ordering.

    bsl::pair<iterator,iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multimap having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this multimap contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value.

    // ACCESSORS
    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // multimap.

    const_iterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or the 'end' iterator if this multimap
        // is empty.

    const_iterator end() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this multimap.

    const_reverse_iterator rbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this multimap, or 'rend' if this multimap is
        // empty.

    const_reverse_iterator rend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multimap.

    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multimap, or the 'cend' iterator if this multimap
        // is empty.

    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this multimap.

    const_reverse_iterator crbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this multimap, or 'rend' if this multimap is
        // empty.

    const_reverse_iterator crend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multimap.

    bool empty() const;
        // Return 'true' if this multimap contains no elements, and 'false'
        // otherwise.

    size_type size() const;
        // Return the number of elements in this multimap.

    size_type max_size() const;
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
        // comparator compares objects of type 'value_type' (i.e.,
        // 'bsl::pair<KEY, VALUE>').

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object having the specified 'key' in ordered sequence
        // maintained by this multimap, if such an object exists; otherwise,
        // return the past-the-end ('end') iterator.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this multimap
        // having the specified 'key'.

    const_iterator lower_bound(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multimap whose key
        // is greater-than or equal-to the specified 'key', and the
        // past-the-end iterator if this multimap does not contain a
        // 'value_type' object whose key is greater-than or equal-to 'key'.
        // Note that this function returns the *first* position before which a
        // 'value_type' object having 'key' could be inserted into the ordered
        // sequence maintained by this multimap, while preserving its ordering.

    const_iterator upper_bound(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multimap whose key
        // is greater than the specified 'key', and the past-the-end iterator
        // if this multimap does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having 'key' could be
        // inserted into the ordered sequence maintained by this multimap,
        // while preserving its ordering.

    bsl::pair<const_iterator,const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this multimap having the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence, and the second is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)'; the second returned iterator will be
        // 'upper_bound(key)'; and, if this multimap contains no 'value_type'
        // objects having 'key', then the two returned iterators will have the
        // same value.

    // NOT IMPLEMENTED
        // The following methods are defined by the C++11 standard, but they
        // are not implemented as they require some level of C++11 compiler
        // support not currently available on all supported platforms.

    //  multimap(multimap<KEY,VALUE,COMPARATOR,ALLOCATOR>&& original);

    //  multimap(multimap&&, const ALLOCATOR&);

    //  multimap(initializer_list<value_type>,
    //           const COMPARATOR& = COMPARATOR(),
    //           const ALLOCATOR& = ALLOCATOR());

    //  multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>&
    //  operator=(multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>&& rhs);


    //  multimap& operator=(initializer_list<value_type>);

    //  template <class... Args> pair<iterator, bool> emplace(Args&&... args);

    //  template <class... Args> iterator emplace_hint(const_iterator position,
    //                                                 Args&&... args);

    //  template <class P> iterator insert(P&& value);

    //  template <class P>
    //  iterator insert(const_iterator position, P&&);

    //  void insert(initializer_list<value_type>);
};

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator==(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'multimap' objects have the same
    // value if they have the same number of key-value pairs, and each
    // key-value pair that is contained in one of the objects is also contained
    // in the other object.  Note that this method requires that the (template
    // parameter) types 'KEY' and 'VALUE' both be "equality-comparable" (see
    // {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator!=(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'multimap' objects do not have
    // the same value if they do not have the same number of key-value pairs,
    // or some key-value pair that is contained in one of the objects is not
    // also contained in the other object.  Note that this method requires that
    // the (template parameter) types 'KEY' and 'VALUE' types both be
    // "equality-comparable" (see {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator<(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is less than the specified
    // 'rhs' value, and 'false' otherwise.  A multimap, 'lhs', has a value that
    // is less than that of 'rhs', if, for the first non-equal corresponding
    // key-value pairs in their respective sequences, the 'lhs' key-value pair
    // is less than the 'rhs' pair, or, if the keys of all of their
    // corresponding key-value pairs compare equal, 'lhs' has fewer key-value
    // pairs than 'rhs'.  Note that this method requires that the (template
    // parameter) types 'KEY' and 'VALUE' types both be "less-than-comparable"
    // (see {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator>(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is greater than the specified
    // 'rhs' value, and 'false' otherwise.  A multimap, 'lhs', has a value that
    // is greater than that of 'rhs', if, for the first non-equal corresponding
    // key-value pairs in their respective sequences, the 'lhs' key-value pair
    // is greater than the 'rhs' pair, or, if the keys of all of their
    // corresponding key-value pairs compare equal, 'lhs' has more key-value
    // pairs than 'rhs'.  Note that this method requires that the (template
    // parameter) types 'KEY' and 'VALUE' both be "less-than-comparable" (see
    // {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator<=(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is less-than or equal-to the
    // specified 'rhs' value, and 'false' otherwise.  A multimap, 'lhs', has a
    // value that is less-than or equal-to that of 'rhs', if, for the first
    // non-equal corresponding key-value pairs in their respective sequences,
    // the 'lhs' key-value pair is less than the 'rhs' pair, or, if the keys of
    // all of their corresponding key-value pairs compare equal, 'lhs' has
    // less-than or equal number of key-value pairs as 'rhs'.  Note that this
    // method requires that the (template parameter) types 'KEY' and 'VALUE'
    // both be "less-than-comparable" (see {Requirements on 'KEY' and
    // 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator>=(const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is greater-than or equal-to
    // the specified 'rhs' value, and 'false' otherwise.  A multimap, 'lhs',
    // has a value that is greater-than or equal-to that of 'rhs', if, for the
    // first non-equal corresponding key-value pairs in their respective
    // sequences, the 'lhs' key-value pair is greater than the 'rhs' pair, or,
    // if the keys of all of their corresponding key-value pairs compare equal,
    // 'lhs' has greater-than or equal number of key-value pairs as 'rhs'.
    // Note that this method requires that the (template parameter) types 'KEY'
    // and 'VALUE' types both be "less-than-comparable" (see {Requirements on
    // 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
void swap(multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
          multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& b);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  Additionally, if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees 'O[1]' complexity.  The
    // behavior is undefined unless either this object was created with the
    // same allocator as 'other' or 'propagate_on_container_swap' is 'true'.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
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

                             // -----------------------------
                             // class multimap::value_compare
                             // -----------------------------

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::value_compare(
                                                                  COMPARATOR c)
: comp(c)
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
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_compAndAlloc.d_pool;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::comparator()
{
    return d_compAndAlloc;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::quickSwap(multimap& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swap(other.nodeFactory());

    // Work around to avoid the 1-byte swap problem on AIX for an empty class
    // under empty-base optimization.

    if (sizeof(NodeFactory) != sizeof(DataWrapper)) {
        comparator().swap(other.comparator());
    }
}

// PRIVATE ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory() const
{
    return d_compAndAlloc.d_pool;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator&
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_compAndAlloc;
}

// CREATORS
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
        BloombergLP::bslalg::RbTreeUtilTreeProctor<NodeFactory> proctor(
                                                               &d_tree,
                                                               &nodeFactory());

        // The following loop guarantees amortized linear time to insert an
        // ordered sequence of values (as required by the standard).   If the
        // values are in sorted order, we are guaranteed the next node can be
        // inserted as the right child of the previous node, and can call
        // 'insertAt' without 'findUniqueInsertLocation'.

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
            BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(
                                                                        value);
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
                                               const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::multimap(
                                               const multimap&  original,
                                               const ALLOCATOR& basicAllocator)
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
            BloombergLP::bslalg::SwapUtil::swap(
                                             &nodeFactory().allocator(),
                                             &other.nodeFactory().allocator());
            quickSwap(other);
        }
        else {
            multimap other(rhs, nodeFactory().allocator());
            quickSwap(other);
        }
    }
    return *this;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin()
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::end()
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend()
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
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(value);
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
    while (first != last) {
        insert(*first);
        ++first;
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const_iterator    hint,
                                                    const value_type& value)
{
    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    bool leftChild;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value.first,
                                                            hintNode);
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

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
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const key_type& key)
{
    size_type count = 0;
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
{
    if (AllocatorTraits::propagate_on_container_swap::value) {
        BloombergLP::bslalg::SwapUtil::swap(&nodeFactory().allocator(),
                                            &other.nodeFactory().allocator());
        quickSwap(other);
    }
    else {
        // C++11 behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        // backward compatible behavior: swap with copies
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
               nodeFactory().allocator() == other.nodeFactory().allocator())) {
            quickSwap(other);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            multimap thisCopy(*this, other.nodeFactory().allocator());
            multimap otherCopy(other, nodeFactory().allocator());

            quickSwap(otherCopy);
            other.quickSwap(thisCopy);
        }
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::clear()
{
    BSLS_ASSERT_SAFE(d_tree.firstNode());
    if (d_tree.rootNode()) {
        BSLS_ASSERT_SAFE(0 < d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() != d_tree.sentinel());

        BloombergLP::bslalg::RbTreeUtil::deleteTree(&d_tree, &nodeFactory());
    }
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    else {
        BSLS_ASSERT_SAFE(0 == d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() == d_tree.sentinel());
    }
#endif
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::find(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::find(d_tree,
                                                          this->comparator(),
                                                          key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::lower_bound(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::upper_bound(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bsl::pair<typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator,
          typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator>
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::equal_range(const key_type& key)
{
    iterator startIt = lower_bound(key);
    iterator endIt   = startIt;
    if (endIt != end() && !comparator()(key, *endIt.node())) {
        endIt = upper_bound(key);
    }
    return bsl::pair<iterator, iterator>(startIt, endIt);
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::allocator_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::get_allocator() const
{
    return nodeFactory().allocator();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin() const
{
    return cbegin();
}


template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::end() const
{
    return cend();
}


template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin() const
{
    return crbegin();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend() const
{
    return crend();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::cend() const
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::crend() const
{
    return const_reverse_iterator(begin());
}

// capacity:
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::empty() const
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size() const
{
    return d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::max_size() const
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

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::find(const key_type& key) const
{
    return const_iterator(
       BloombergLP::bslalg::RbTreeUtil::find(d_tree, this->comparator(), key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE,COMPARATOR, ALLOCATOR>::size_type
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::count(const key_type& key) const
{
    int cnt = 0;
    const_iterator it = lower_bound(key);
    while (it != end() && !comparator()(key, *it.node())) {
        ++it;
        ++cnt;
    }
    return cnt;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::lower_bound(
                                                     const key_type& key) const
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::upper_bound(
                                                     const key_type& key) const
{
    return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bsl::pair<typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator,
          typename multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator>
multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    const_iterator startIt = lower_bound(key);
    const_iterator endIt   = startIt;
    if (endIt != end() && !comparator()(key, *endIt.node())) {
        endIt = upper_bound(key);
    }
    return bsl::pair<const_iterator, const_iterator>(startIt, endIt);
}

}  // close namespace bsl

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

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator!=(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
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

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<=(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}


template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>=(
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                   const bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
void bsl::swap(bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
               bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR>& b)
{
    a.swap(b);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *ordered* containers:
//: o An ordered container defines STL iterators.
//: o An ordered container uses 'bslma' allocators if the parameterized
//:     'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
struct HasStlIterators<bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY,   class VALUE, class COMPARATOR, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::multimap<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
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
