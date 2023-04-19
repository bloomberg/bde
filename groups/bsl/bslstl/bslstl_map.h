// bslstl_map.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_MAP
#define INCLUDED_BSLSTL_MAP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant map class.
//
//@CLASSES:
//   bsl::map: STL-compliant map template
//
//@CANONICAL_HEADER: bsl_map.h
//
//@SEE_ALSO: bslstl_multimap, bslstl_set
//
//@DESCRIPTION: This component defines a single class template 'bsl::map',
// implementing the standard container holding an ordered sequence of key-value
// pairs (having unique keys), and presenting a mapping from the keys (of a
// template parameter type, 'KEY') to their associated values (of another
// template parameter type, 'VALUE').
//
// An instantiation of 'map' is an allocator-aware, value-semantic type whose
// salient attributes are its size (number of key-value pairs) and the ordered
// sequence of key-value pairs the map contains.  If 'map' is instantiated with
// either a key type or mapped-value type that is not itself value-semantic,
// then it will not retain all of its value-semantic qualities.  In particular,
// if either the key or value type cannot be tested for equality, then a 'map'
// containing that type cannot be tested for equality.  It is even possible to
// instantiate 'map' with a key or mapped-value type that does not have a copy
// constructor, in which case the 'map' will not be copyable.
//
// A map meets the requirements of an associative container with bidirectional
// iterators in the C++ standard [associative.reqmts].  The 'map' implemented
// here adheres to the C++11 standard when compiled with a C++11 compiler, and
// makes the best approximation when compiled with a C++03 compiler.  In
// particular, for C++03 we emulate move semantics, but limit forwarding (in
// 'emplace') to 'const' lvalues, and make no effort to emulate 'noexcept' or
// initializer-lists.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// A 'map' is a fully Value-Semantic Type (see {'bsldoc_glossary'}) only if the
// supplied 'KEY' and 'VALUE' template parameters are themselves fully
// value-semantic.  It is possible to instantiate a 'map' with 'KEY' and
// 'VALUE' parameter arguments that do not provide a full set of value-semantic
// operations, but then some methods of the container may not be instantiable.
// The following terminology, adopted from the C++11 standard, is used in the
// function documentation of 'map' to describe a function's requirements for
// the 'KEY' and 'VALUE' template parameters.  These terms are also defined in
// sections [utility.arg.requirements] and [container.requirements.general] of
// the C++11 standard.  Note that, in the context of a 'map' instantiation, the
// requirements apply specifically to the map's entry type, 'value_type', which
// is an alias for 'bsl::pair<const KEY, VALUE>'.
//
///Glossary
///--------
//..
// Legend
// ------
// 'X'    - denotes an allocator-aware container type (e.g., 'map')
// 'T'    - 'value_type' associated with 'X'
// 'A'    - type of the allocator used by 'X'
// 'm'    - lvalue of type 'A' (allocator)
// 'p',   - address ('T *') of uninitialized storage for a 'T' within an 'X'
// 'rv'   - rvalue of type (non-'const') 'T'
// 'v'    - rvalue or lvalue of type (possibly 'const') 'T'
// 'args' - 0 or more arguments
//..
// The following terms are used to more precisely specify the requirements on
// template parameter types in function-level documentation.
//
//: *default-insertable*: 'T' has a default constructor.  More precisely, 'T'
//:   is 'default-insertable' into 'X' means that the following expression is
//:   well-formed:
//:   'allocator_traits<A>::construct(m, p)'
//:
//: *move-insertable*: 'T' provides a constructor that takes an rvalue of type
//:   (non-'const') 'T'.  More precisely, 'T' is 'move-insertable' into 'X'
//:   means that the following expression is well-formed:
//:   'allocator_traits<A>::construct(m, p, rv)'
//:
//: *copy-insertable*: 'T' provides a constructor that takes an lvalue or
//:   rvalue of type (possibly 'const') 'T'.  More precisely, 'T' is
//:   'copy-insertable' into 'X' means that the following expression is
//:   well-formed:
//:   'allocator_traits<A>::construct(m, p, v)'
//:
//: *move-assignable*: 'T' provides an assignment operator that takes an rvalue
//:   of type (non-'const') 'T'.
//:
//: *copy-assignable*: 'T' provides an assignment operator that takes an lvalue
//:   or rvalue of type (possibly 'const') 'T'.
//:
//: *emplace-constructible*: 'T' is 'emplace-constructible' into 'X' from
//:   'args' means that the following expression is well-formed:
//:   'allocator_traits<A>::construct(m, p, args)'
//:
//: *erasable*: 'T' provides a destructor.  More precisely, 'T' is 'erasable'
//:   from 'X' means that the following expression is well-formed:
//:   'allocator_traits<A>::destroy(m, p)'
//:
//: *equality-comparable*: The type provides an equality-comparison operator
//:   that defines an equivalence relationship and is both reflexive and
//:   transitive.
//
///Key Comparison
///--------------
// The type supplied as a map's 'COMPARATOR' template parameter determines how
// that map will order elements.  In particular, the 'COMPARATOR' type must
// induce a strict weak ordering on objects of type 'KEY'.  The 'COMPARATOR'
// parameter defaults to 'std::less<KEY>' when not supplied in an instantiation
// of 'map'.  Note that the 'COMPARATOR' type must be copy-constructible.
//
// The C++11 standard does not require that the function-call operator provided
// by 'COMPARATOR' functors be 'const'-qualified.  However, there is a
// suggestion for C++17 that this is an oversight in the standard, and that
// 'const'-qualification will be required in the future.  Keep this in mind
// when opting to use an alternative to the default 'COMPARATOR'.
//
///Memory Allocation
///-----------------
// The type supplied as a map's 'ALLOCATOR' template parameter determines how
// that map will allocate memory.  The 'map' template supports allocators
// meeting the requirements of the C++11 standard [allocator.requirements].
// In addition, 'map' supports scoped-allocators derived from the
// 'bslma::Allocator' memory allocation protocol.  Clients intending to use
// 'bslma'-style allocators should use the template's default 'ALLOCATOR'
// type: The default type for the 'ALLOCATOR' template parameter,
// 'bsl::allocator', provides a C++11 standard-compatible adapter for a
// 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of a 'map' instantiation' is
// 'bsl::allocator', then objects of that map type will conform to the standard
// behavior of a 'bslma'-allocator-enabled type.  Such a map accepts an
// optional 'bslma::Allocator' argument at construction.  If the address of a
// 'bslma::Allocator' object is explicitly supplied at construction, it is used
// to supply memory for the map throughout its lifetime; otherwise, the map
// will use the default allocator installed at the time of the map's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', a map supplies that
// allocator's address to the constructors of contained objects of the
// (template parameter) type 'KEY' and 'VALUE', if respectively, the types
// define the 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'map':
//..
//  Legend
//  ------
//  'K'             - (template parameter) type 'KEY' of the map
//  'V'             - (template parameter) type 'VALUE' of the map
//  'a', 'b'        - two distinct objects of type 'map<K, V>'
//  'rv'            - modifiable rvalue of type 'map<K, V>'
//  'n', 'm'        - number of elements in 'a' and 'b', respectively
//  'value_type'    - 'pair<const K, V>'
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al             - STL-style memory allocator
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
//  | map<K, V> a;     (default construction)            | O[1]               |
//  | map<K, V> a(al);                                   |                    |
//  | map<K, V> a(c, al);                                |                    |
//  +----------------------------------------------------+--------------------+
//  | map<K, V> a(rv); (move construction)               | O[1] if 'a' and    |
//  | map<K, V> a(rv, al);                               | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | map<K, V> a(b);  (copy construction)               | O[n]               |
//  | map<K, V> a(b, al);                                |                    |
//  +----------------------------------------------------+--------------------+
//  | map<K, V> a(i1, i2);                               | O[N] if [i1 .. i2) |
//  | map<K, V> a(i1, i2, al);                           | is sorted with     |
//  | map<K, V> a(i1, i2, c, al);                        | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where N |
//  |                                                    | is distance(i1,i2) |
//  +----------------------------------------------------+--------------------+
//  | a.~map<K, V>();  (destruction)                     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = rv;          (move assignment)                 | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | a = b;           (copy assignment)                 | O[n]               |
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
//  | a[k]                                               | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.at(k)                                            | O[log(n)]          |
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
///Example 1: Creating a Trade Matching System
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsl::map' to define and implement a class,
// 'TradeMatcher', that provides a simple trade matching system for a single
// stock.  The manipulators of 'TradeMatcher' will allow clients to place buy
// orders and sell orders, and the accessors of 'TradeMatcher' will allow
// clients to retrieve active orders and past executions.
//
// First, we define the public interface for 'TradeMatcher':
//..
//  class TradeMatcher {
//      // This class provides a mechanism that characterizes a simple trade
//      // matching system for one stock.  An object of this class allows
//      // clients to place orders and view the active orders.
//..
// Here, we create two type aliases, 'SellOrdersMap' and 'BuyOrdersMap', for
// two 'bsl::map' instantiations that maps the price of an order (type
// 'double') to the quantity of the order (type 'int').  'SellOrdersMap' uses
// the default 'bsl::less' comparator to store the sequence of sell orders in
// ascending price order.  'BuyOrdersMap' uses the 'bsl::greater' comparator to
// store the sequence of buy orders in descending price order.  Also note that
// we use the default 'ALLOCATOR' template parameter for both aliases as we
// intend to provide memory with 'bslma'-style allocators:
//..
//      // PRIVATE TYPES
//      typedef bsl::map<double, int> SellOrdersMap;
//          // This 'typedef' is an alias for a mapping between the price and
//          // quantity of an order in ascending price order.
//
//      typedef bsl::map<double, int, std::greater<double> > BuyOrdersMap;
//          // This 'typedef' is an alias for a mapping between the price and
//          // quantity of an order in descending price order.
//
//
//      // DATA
//      SellOrdersMap   d_sellOrders;  // current sell orders
//      BuyOrdersMap    d_buyOrders;   // current buy orders
//
//    private:
//      // NOT IMPLEMENTED
//      TradeMatcher& operator=(const TradeMatcher&);
//      TradeMatcher(const TradeMatcher&);
//
//    public:
//      // PUBLIC TYPES
//      typedef SellOrdersMap::const_iterator SellOrdersConstIterator;
//          // This 'typedef' provides an alias for the type of an iterator
//          // providing non-modifiable access to sell orders in a
//          // 'TradeMatcher'.
//
//      typedef BuyOrdersMap::const_iterator BuyOrdersConstIterator;
//          // This 'typedef' provides an alias for the type of an iterator
//          // providing non-modifiable access to buy orders in a
//          // 'TradeMatcher'.
//
//      // CREATORS
//      TradeMatcher(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'TradeMatcher' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      //! ~TradeMatcher() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      void placeBuyOrder(double price, int numShares);
//          // Place an order to buy the specified 'numShares' at the specified
//          // 'price'.  The placed buy order will (possibly partially) execute
//          // when active sale orders exist in the system at or below 'price'.
//          // The behavior is undefined unless '0 < price' and '0 <
//          // numShares'.
//
//      void placeSellOrder(double price, int numShares);
//          // Place an order to sell the specified 'numShares' at the
//          // specified 'price'.  The placed sell order will (possibly
//          // partially) execute when active buy orders exist in the system at
//          // or above 'price'.  The behavior is undefined unless '0 < price'
//          // and '0 < numShares'.
//
//      // ACCESSORS
//      SellOrdersConstIterator beginSellOrders() const;
//          // Return an iterator providing non-modifiable access to the active
//          // sell order at the lowest price in the ordered sequence (from low
//          // price to high price) of sell orders maintained by this object.
//
//      SellOrdersConstIterator endSellOrders() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end sell order in the ordered sequence (from low price
//          // to high price) of sell orders maintained by this object.
//
//      BuyOrdersConstIterator beginBuyOrders() const;
//          // Return an iterator providing non-modifiable access to the active
//          // buy order at the highest price in the ordered sequence (from
//          // high price to low price) of buy orders maintained by this
//          // object.
//
//      BuyOrdersConstIterator endBuyOrders() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end buy order in the ordered sequence (from high price
//          // to low price) of buy orders maintained by this object.
//  };
//..
// Now, we define the implementations methods of the 'TradeMatcher' class:
//..
//  // CREATORS
//  TradeMatcher::TradeMatcher(bslma::Allocator *basicAllocator)
//  : d_sellOrders(basicAllocator)
//  , d_buyOrders(basicAllocator)
//  {
//  }
//..
// Notice that, on construction, we pass the contained 'bsl::map' objects the
// 'bsl::Allocator' supplied at construction'.
//..
//  // MANIPULATORS
//  void TradeMatcher::placeBuyOrder(double price, int numShares)
//  {
//      BSLS_ASSERT(0 < price);
//      BSLS_ASSERT(0 < numShares);
//
//      // Buy shares from sellers from the one with the lowest price up to but
//      // not including the first seller with a price greater than the
//      // specified 'price'.
//
//      SellOrdersMap::iterator itr = d_sellOrders.begin();
//
//      while (numShares && itr != d_sellOrders.upper_bound(price)) {
//          if (itr->second > numShares) {
//              itr->second -= numShares;
//              numShares = 0;
//              break;
//          }
//
//          itr = d_sellOrders.erase(itr);
//          numShares -= itr->second;
//      }
//
//      if (numShares > 0) {
//          d_buyOrders[price] += numShares;
//      }
//  }
//
//  void TradeMatcher::placeSellOrder(double price, int numShares)
//  {
//      BSLS_ASSERT(0 < price);
//      BSLS_ASSERT(0 < numShares);
//
//      // Sell shares to buyers from the one with the highest price up to but
//      // not including the first buyer with a price smaller than the
//      // specified 'price'.
//
//      BuyOrdersMap::iterator itr = d_buyOrders.begin();
//
//      while (numShares && itr != d_buyOrders.upper_bound(price)) {
//          if (itr->second > numShares) {
//              itr->second -= numShares;
//              numShares = 0;
//              break;
//          }
//
//          itr = d_buyOrders.erase(itr);
//          numShares -= itr->second;
//      }
//
//      if (numShares > 0) {
//          d_sellOrders[price] += numShares;
//      }
//  }
//
//  // ACCESSORS
//  TradeMatcher::SellOrdersConstIterator TradeMatcher::beginSellOrders() const
//  {
//      return d_sellOrders.begin();
//  }
//
//  TradeMatcher::SellOrdersConstIterator TradeMatcher::endSellOrders() const
//  {
//      return d_sellOrders.end();
//  }
//
//  TradeMatcher::BuyOrdersConstIterator TradeMatcher::beginBuyOrders() const
//  {
//      return d_buyOrders.begin();
//  }
//
//  TradeMatcher::BuyOrdersConstIterator TradeMatcher::endBuyOrders() const
//  {
//      return d_buyOrders.end();
//  }
//..

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
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
#include <bslalg_synththreewayutil.h>
#include <bslalg_typetraithasstliterators.h>

#include <bslma_constructionutil.h>
#include <bslma_isstdallocator.h>
#include <bslma_default.h>
#include <bslma_destructorguard.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_addlvaluereference.h>
#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_istransparentpredicate.h>
#include <bslmf_movableref.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <functional>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
#include <tuple>      // for forward_as_tuple (C++11)
#include <utility>    // for piecewise_construct (C++11)
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>
    #ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    #error Rvalue references curiously absent despite native 'type_traits'.
    #endif
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_map.h
# define COMPILING_BSLSTL_MAP_H
# include <bslstl_map_cpp03.h>
# undef COMPILING_BSLSTL_MAP_H
#else

namespace bsl {

                             // =========
                             // class map
                             // =========

template <class KEY,
          class VALUE,
          class COMPARATOR = std::less<KEY>,
          class ALLOCATOR  = allocator<pair<const KEY, VALUE> > >
class map {
    // This class template implements a value-semantic container type holding
    // an ordered sequence of key-value pairs having unique keys that provide a
    // mapping from keys (of the template parameter type, 'KEY') to their
    // associated values (of another template parameter type, 'VALUE').
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
        // maintained by this map.

    typedef BloombergLP::bslstl::MapComparator<KEY, VALUE, COMPARATOR>
                                                               Comparator;
        // This typedef is an alias for the comparator used internally by this
        // map.

    typedef BloombergLP::bslstl::TreeNode<ValueType>           Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this map.

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
        // This struct is wrapper around the comparator and allocator data
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

    typedef BloombergLP::bslstl::TreeIterator<
                                   value_type, Node, difference_type> iterator;
    typedef BloombergLP::bslstl::TreeIterator<
                       const value_type, Node, difference_type> const_iterator;
    typedef bsl::reverse_iterator<iterator>            reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>      const_reverse_iterator;

    class value_compare {
        // This nested class defines a mechanism for comparing two objects of
        // 'value_type' by adapting an object of (template parameter) type
        // 'COMPARATOR', which compares two objects of (template parameter)
        // type 'KEY' .  Note that this class exactly matches its definition in
        // the C++11 standard [map.overview]; otherwise, we would have
        // implemented it as a separate component-local class.

        // FRIENDS
        friend class map;

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
            // 'map::value_compare' object.

        typedef value_type first_argument_type;
            // This 'typedef' is an alias for the type of the first parameter
            // of the overload of 'operator()' (the comparison function)
            // provided by a 'map::value_compare' object.

        typedef value_type second_argument_type;
            // This 'typedef' is an alias for the type of the second parameter
            // of the overload of 'operator()' (the comparison function)
            // provided by a 'map::value_compare' object.

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
    NodeFactory& nodeFactory();
        // Return a reference providing modifiable access to the node allocator
        // for this map.

    Comparator& comparator();
        // Return a reference providing modifiable access to the comparator for
        // this map.

    void quickSwapExchangeAllocators(map& other);
        // Efficiently exchange the value, comparator, and allocator of this
        // object with the value, comparator, and allocator of the specified
        // 'other' object.  This method provides the no-throw exception-safety
        // guarantee, *unless* swapping the (user-supplied) comparator or
        // allocator objects can throw.

    void quickSwapRetainAllocators(map& other);
        // Efficiently exchange the value and comparator of this object with
        // the value and comparator of the specified 'other' object.  This
        // method provides the no-throw exception-safety guarantee, *unless*
        // swapping the (user-supplied) comparator objects can throw.  The
        // behavior is undefined unless this object was created with the same
        // allocator as 'other'.

    // PRIVATE ACCESSORS
    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the node
        // allocator for this map.

    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this map.

  public:
    // CREATORS
    map();
    explicit map(const COMPARATOR& comparator,
                 const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Create an empty map.  Optionally specify a 'comparator' used to
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

    explicit map(const ALLOCATOR& basicAllocator);
        // Create an empty map that uses the specified 'basicAllocator' to
        // supply memory.  Use a default-constructed object of the (template
        // parameter) type 'COMPARATOR' to order the key-value pairs contained
        // in this map.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    map(const map& original);
        // Create a map having the same value as the specified 'original'
        // object.  Use a copy of 'original.key_comp()' to order the key-value
        // pairs contained in this map.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  If the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), the currently installed default
        // allocator is used.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be 'copy-insertable' into
        // this map (see {Requirements on 'KEY' and 'VALUE'}).

    map(BloombergLP::bslmf::MovableRef<map> original);              // IMPLICIT
        // Create a map having the same value as the specified 'original'
        // object by moving (in constant time) the contents of 'original' to
        // the new map.  Use a copy of 'original.key_comp()' to order the
        // key-value pairs contained in this map.  The allocator associated
        // with 'original' is propagated for use in the newly-created map.
        // 'original' is left in a valid but unspecified state.

    map(const map&                                     original,
        const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a map having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Use a copy of 'original.key_comp()' to order the key-value pairs
        // contained in this map.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be 'copy-insertable' into
        // this map (see {Requirements on 'KEY' and 'VALUE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator' (the default).

    map(BloombergLP::bslmf::MovableRef<map>            original,
        const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a map having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // map if 'basicAllocator == original.get_allocator()', and are move-
        // inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  Use a copy of
        // 'original.key_comp()' to order the key-value pairs contained in this
        // map.  This method requires that the (template parameter) types 'KEY'
        // and 'VALUE' both be 'move-insertable' into this map (see
        // {Requirements on 'KEY' and 'VALUE'}).  Note that a
        // 'bslma::Allocator *' can be supplied for 'basicAllocator' if the
        // (template parameter) 'ALLOCATOR' is 'bsl::allocator' (the default).

    template <class INPUT_ITERATOR>
    map(INPUT_ITERATOR    first,
        INPUT_ITERATOR    last,
        const COMPARATOR& comparator     = COMPARATOR(),
        const ALLOCATOR&  basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    map(INPUT_ITERATOR    first,
        INPUT_ITERATOR    last,
        const ALLOCATOR&  basicAllocator);
        // Create a map, and insert each 'value_type' object in the sequence
        // starting at the specified 'first' element, and ending immediately
        // before the specified 'last' element, ignoring those objects having a
        // key equivalent to that which appears earlier in the sequence.
        // Optionally specify a 'comparator' used to order key-value pairs
        // contained in this object.  If 'comparator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'COMPARATOR' is used.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // (the default), then 'basicAllocator', if supplied, shall be
        // convertible to 'bslma::Allocator *'.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  If the sequence 'first' to
        // 'last' is ordered according to 'comparator', then this operation has
        // 'O[N]' complexity, where 'N' is the number of elements between
        // 'first' and 'last'; otherwise, this operation has 'O[N * log(N)]'
        // complexity.  The (template parameter) type 'INPUT_ITERATOR' shall
        // meet the requirements of an input iterator defined in the C++11
        // standard [input.iterators] providing access to values of a type
        // convertible to 'value_type', and 'value_type' must be
        // 'emplace-constructible' from '*i' into this map, where 'i' is a
        // dereferenceable iterator in the range '[first .. last)' (see
        // {Requirements on 'KEY' and 'VALUE'}).  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    map(std::initializer_list<value_type> values,
        const COMPARATOR&                 comparator     = COMPARATOR(),
        const ALLOCATOR&                  basicAllocator = ALLOCATOR());
    map(std::initializer_list<value_type> values,
        const ALLOCATOR&                  basicAllocator);
        // Create a map and insert each 'value_type' object in the specified
        // 'values' initializer list, ignoring those objects having a key
        // equivalent to that which appears earlier in the list.  Optionally
        // specify a 'comparator' used to order keys contained in this object.
        // If 'comparator' is not supplied, a default-constructed object of the
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
        // 'VALUE' both be 'copy-insertable' into this map (see {Requirements
        // on 'KEY' and 'VALUE'}).
#endif

    ~map();
        // Destroy this object.

    // MANIPULATORS
    map& operator=(const map& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // If an exception is thrown, '*this' is left in a valid but
        // unspecified state.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be 'copy-assignable' and
        // 'copy-insertable' into this map (see {Requirements on 'KEY' and
        // 'VALUE'}).

    map& operator=(BloombergLP::bslmf::MovableRef<map> rhs)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           AllocatorTraits::is_always_equal::value &&
                           std::is_nothrow_move_assignable<COMPARATOR>::value);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_move_assignment',
        // and return a reference providing modifiable access to this object.
        // The contents of 'rhs' are moved (in constant time) to this map if
        // 'get_allocator() == rhs.get_allocator()' (after accounting for the
        // aforementioned trait); otherwise, all elements in this map are
        // either destroyed or move-assigned to and each additional element in
        // 'rhs' is move-inserted into this map.  'rhs' is left in a valid but
        // unspecified state, and if an exception is thrown, '*this' is left
        // in a valid but unspecified state.  This method requires that the
        // (template parameter) types 'KEY' and 'VALUE' both be
        // 'move-assignable' and 'move-insertable' into this map (see
        // {Requirements on 'KEY' and 'VALUE'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    map& operator=(std::initializer_list<value_type> values);
        // Assign to this object the value resulting from first clearing this
        // map and then inserting each 'value_type' object in the specified
        // 'values' initializer list, ignoring those objects having a key
        // equivalent to that which appears earlier in the list; return a
        // reference providing modifiable access to this object.  This method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be 'copy-insertable' into this map (see {Requirements on 'KEY' and
        // 'VALUE'}).
#endif

    typename add_lvalue_reference<VALUE>::type operator[](const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key'; if this 'map' does not already
        // contain a 'value_type' object having an equivalent key, first insert
        // a new 'value_type' object having 'key' and a default-constructed
        // 'VALUE' object, and return a reference to the newly mapped (default)
        // value.  This method requires that the (template parameter) type
        // 'KEY' be 'copy-insertable' into this map and the (template
        // parameter) type 'VALUE' be 'default-insertable' into this map (see
        // {Requirements on 'KEY' and 'VALUE'}).

    typename add_lvalue_reference<VALUE>::type operator[](
                                 BloombergLP::bslmf::MovableRef<key_type> key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key'; if this 'map' does not already
        // contain a 'value_type' object having an equivalent key, first insert
        // a new 'value_type' object having the move-inserted 'key' and a
        // default-constructed 'VALUE' object, and return a reference to the
        // newly mapped (default) value.  This method requires that the
        // (template parameter) type 'KEY' be 'move-insertable' into this map
        // and the (template parameter) type 'VALUE' be 'default-insertable'
        // into this map (see {Requirements on 'KEY' and 'VALUE'}).

    typename add_lvalue_reference<VALUE>::type at(const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key', if such an entry exists;
        // otherwise, throw a 'std::out_of_range' exception.  Note that this
        // method may also throw a different kind of exception if the
        // (user-supplied) comparator throws.

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or the 'end' iterator if this map is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this map.

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or 'rend' if this map is empty.

    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this map.

    pair<iterator, bool> insert(const value_type& value);
        // Insert the specified 'value' into this map if a key (the 'first'
        // element) equivalent to that of 'value' does not already exist in
        // this map; otherwise, if a 'value_type' object whose key is
        // equivalent to that of 'value' already exists in this map, this
        // method has no effect.  Return a pair whose 'first' member is an
        // iterator referring to the (possibly newly inserted) 'value_type'
        // object in this map whose key is equivalent to that of 'value', and
        // whose 'second' member is 'true' if a new value was inserted, and
        // 'false' if the key was already present.  This method requires that
        // the (template parameter) types 'KEY' and 'VALUE' both be
        // 'copy-insertable' into this map (see {Requirements on 'KEY' and
        // 'VALUE'}).

    pair<iterator, bool> insert(
                             BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert into this map the specified 'value' if a key (the 'first'
        // element) equivalent to that of 'value' does not already exist in
        // this map; otherwise, this method has no effect.  Return a pair whose
        // 'first' member is an iterator referring to the (possibly newly
        // inserted) 'value_type' object in this map whose key is equivalent to
        // that of 'value', and whose 'second' member is 'true' if a new value
        // was inserted and 'false' if the key was already present.  This
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'move-insertable' into this map (see {Requirements
        // on 'KEY' and 'VALUE'}).

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ALT_VALUE_TYPE>
    pair<iterator, bool>
#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    template <class ALT_VALUE_TYPE>
    typename enable_if<is_convertible<ALT_VALUE_TYPE, value_type>::value,
                       pair<iterator, bool> >::type
#else
    template <class ALT_VALUE_TYPE>
    typename enable_if<std::is_constructible<value_type,
                                             ALT_VALUE_TYPE&&>::value,
                       pair<iterator, bool> >::type
#endif
    insert(BSLS_COMPILERFEATURES_FORWARD_REF(ALT_VALUE_TYPE) value)
        // Insert into this map a 'value_type' object created from the
        // specified 'value' if a key (the 'first' element) equivalent to that
        // of such an object does not already exist in this map; otherwise,
        // this method has no effect (other than possibly creating a temporary
        // 'value_type' object).  Return a pair whose 'first' member is an
        // iterator referring to the (possibly newly inserted) 'value_type'
        // object in this map whose key is equivalent to that of the object
        // created from 'value', and whose 'second' member is 'true' if a new
        // value was inserted and 'false' if the key was already present.  This
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'move-insertable' into this map (see {Requirements
        // on 'KEY' and 'VALUE'}), and the 'value_type' be constructible from
        // the (template parameter) 'ALT_VALUE_TYPE'.
    {
        // This function has to be implemented inline, in violation of BDE
        // convention, as the MSVC compiler cannot match the out-of-class
        // definition of the declaration in the class.

        return emplace(BSLS_COMPILERFEATURES_FORWARD(ALT_VALUE_TYPE, value));
    }

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this map (in amortized constant
        // time if the specified 'hint' is a valid immediate successor to the
        // key of 'value') if a key (the 'first' element) equivalent to that of
        // 'value' does not already exist in this map; otherwise, if a
        // 'value_type' object whose key is equivalent to that of 'value'
        // already exists in this map, this method has no effect.  Return an
        // iterator referring to the (possibly newly inserted) 'value_type'
        // object in this map whose key is equivalent to that of 'value'.  If
        // 'hint' is not a valid immediate successor to the key of 'value',
        // this operation has 'O[log(N)]' complexity, where 'N' is the size of
        // this map.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'copy-insertable' into this map (see
        // {Requirements on 'KEY' and 'VALUE'}).  The behavior is undefined
        // unless 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).

    iterator insert(const_iterator                             hint,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert into this map the specified 'value' (in amortized constant
        // time if the specified 'hint' is a valid immediate successor to
        // 'value') if a key (the 'first' element) equivalent to that of
        // 'value' does not already exist in this map; otherwise, this method
        // has no effect.  Return an iterator referring to the (possibly newly
        // inserted) 'value_type' object in this map whose key is equivalent to
        // that of 'value'.  If 'hint' is not a valid immediate successor to
        // 'value', this operation has 'O[log(N)]' complexity, where 'N' is the
        // size of this map.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be 'move-insertable' into
        // this map (see {Requirements on 'KEY' and 'VALUE'}).  The behavior is
        // undefined unless 'hint' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).

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
        // Insert into this map a 'value_type' object created from the
        // specified 'value' (in amortized constant time if the specified
        // 'hint' is a valid immediate successor to the object created from
        // 'value') if a key (the 'first' element) equivalent to such an object
        // does not already exist in this map; otherwise, this method has no
        // effect (other than possibly creating a temporary 'value_type'
        // object).  Return an iterator referring to the (possibly newly
        // inserted) 'value_type' object in this map whose key is equivalent to
        // that of the object created from 'value'.  If 'hint' is not a valid
        // immediate successor to the object created from 'value', this
        // operation has 'O[log(N)]' complexity, where 'N' is the size of this
        // map.  This method requires that the (template parameter) types 'KEY'
        // and 'VALUE' both be 'move-insertable' into this map (see
        // {Requirements on 'KEY' and 'VALUE'}), and the 'value_type' be
        // constructible from the (template parameter) 'ALT_VALUE_TYPE'.  The
        // behavior is undefined unless 'hint' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).
    {
        // This function has to be implemented inline, in violation of BDE
        // convention, as the MSVC compiler cannot match the out-of-class
        // definition of the declaration in the class.

        return emplace_hint(
                         hint,
                         BSLS_COMPILERFEATURES_FORWARD(ALT_VALUE_TYPE, value));
    }

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this map the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator, if a key
        // equivalent to that of the object is not already contained in this
        // map.  The (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [input.iterators] providing access to values of a type convertible
        // to 'value_type', and 'value_type' must be 'emplace-constructible'
        // from '*i' into this map, where 'i' is a dereferenceable iterator in
        // the range '[first .. last)' (see {Requirements on 'KEY' and
        // 'VALUE'}).  The behavior is undefined unless 'first' and 'last'
        // refer to a sequence of valid values where 'first' is at a position
        // at or before 'last'.

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
        // Insert into this map the value of each 'value_type' object in the
        // specified 'values' initializer list if a key equivalent to that of
        // the object is not already contained in this map.  This method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be 'copy-insertable' into this map (see {Requirements on 'KEY' and
        // 'VALUE'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class BDE_OTHER_TYPE>
    pair<iterator, bool> insert_or_assign(const KEY&       key,
                                          BDE_OTHER_TYPE&& obj);
        // If a key equivalent to the specified 'key' already exists in this
        // map, assign the specified 'obj' to the value associated with that
        // key, and return a pair containing an iterator referring to the
        // existing item and 'false'.  Otherwise, insert into this map a
        // newly-created 'value_type' object, constructed from
        // '(key, std::forward<BDE_OTHER_TYPE>(obj)...))', and return a pair
        // containing an iterator referring to the newly-created entry and
        // 'true'.

    template <class BDE_OTHER_TYPE>
    pair<iterator, bool> insert_or_assign(
                                      BloombergLP::bslmf::MovableRef<KEY> key,
                                      BDE_OTHER_TYPE&&                    obj);
        // If a key equivalent to the specified 'key' already exists in this
        // map, assign the specified 'obj' to the value associated with that
        // key, and return a pair containing an iterator referring to the
        // existing item and 'false'.  Otherwise, insert into this map a
        // newly-created 'value_type' object, constructed from
        // '(std::forward<KEY>(key), std::forward<BDE_OTHER_TYPE>(obj)...))',
        // and return a pair containing an iterator referring to the
        // newly-created entry and 'true'.

    template <class BDE_OTHER_TYPE>
    iterator insert_or_assign(const_iterator   hint,
                              const KEY&       key,
                              BDE_OTHER_TYPE&& obj);
        // If a key equivalent to the specified 'key' already exists in this
        // map, assign the specified 'obj' to the value associated with that
        // key, and return an iterator referring to the existing item.
        // Otherwise, insert into this map a newly-created 'value_type' object,
        // constructed from '(key, std::forward<BDE_OTHER_TYPE>(obj)...))', and
        // return an iterator referring to the newly-created entry.  Use the
        // specified 'hint' as a starting point for checking to see if the key
        // is already in the map.

    template <class BDE_OTHER_TYPE>
    iterator insert_or_assign(const_iterator                      hint,
                              BloombergLP::bslmf::MovableRef<KEY> key,
                              BDE_OTHER_TYPE&&                    obj);
        // If a key equivalent to the specified 'key' already exists in this
        // _map, assign the specified 'obj' to the value associated with that
        // key, and return an iterator referring to the existing item.
        // Otherwise, insert into this map a newly-created 'value_type' object
        // constructed from
        // '(std::forward<KEY>(key), std::forward<BDE_OTHER_TYPE>(obj)...))',
        // and return an iterator referring to the newly-created entry.  Use
        // the specified 'hint' as a starting point for checking to see if the
        // key already in the map.
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    pair<iterator, bool> emplace(Args&&... args);
        // Insert into this map a newly-created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type', if a key equivalent to such a value
        // does not already exist in this map; otherwise, this method has no
        // effect (other than possibly creating a temporary 'value_type'
        // object).  Return a pair whose 'first' member is an iterator
        // referring to the (possibly newly created and inserted) object in
        // this map whose key is equivalent to that of an object constructed
        // from 'args', and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' if an equivalent key was already present.
        // This method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be 'emplace-constructible' from 'args' (see
        // {Requirements on 'KEY' and 'VALUE'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);
        // Insert into this map a newly-created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type' (in amortized constant time if the
        // specified 'hint' is a valid immediate successor to the 'value_type'
        // object constructed from 'args'), if a key equivalent to such a value
        // does not already exist in this map; otherwise, this method has no
        // effect (other than possibly creating a temporary 'value_type'
        // object).  Return an iterator referring to the (possibly newly
        // created and inserted) object in this map whose key is equivalent to
        // that of an object constructed from 'args'.  If 'hint' is not a valid
        // immediate successor to the 'value_type' object implied by 'args',
        // this operation has 'O[log(N)]' complexity where 'N' is the size of
        // this map.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be 'emplace-constructible' from 'args' (see
        // {Requirements on 'KEY' and 'VALUE'}).  The behavior is undefined
        // unless 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).
#endif

    iterator erase(const_iterator position);
    iterator erase(iterator position);
        // Remove from this map the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this map.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.  The behavior is undefined unless
        // 'position' refers to a 'value_type' object in this map.

    size_type erase(const key_type& key);
        // Remove from this map the 'value_type' object whose key is equivalent
        // the specified 'key', if such an entry exists, and return 1;
        // otherwise, if there is no 'value_type' object having an equivalent
        // key, return 0 with no other effect.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this map the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this map or are the
        // 'end' iterator, and the 'first' position is at or before the 'last'
        // position in the ordered sequence provided by this container.

    void swap(map& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
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

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    pair<iterator, bool> try_emplace(const KEY& key, Args&&... args);
    template <class... Args>
    pair<iterator, bool> try_emplace(BloombergLP::bslmf::MovableRef<KEY> key,
                                     Args&&...                           args);
        // If a key equivalent to the specified 'key' already exists in this
        // map, return a pair containing an iterator referring to the existing
        // item and 'false'.  Otherwise, insert into this map a newly-created
        // entry constructed from 'key' and the specified 'args', and return a
        // pair containing an iterator referring to the newly-created entry and
        // 'true'.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' are 'emplace-constructible' from 'key' and 'args'
        // respectively.  For C++03, 'VALUE' must also be 'copy-constructible'.

    template<class... Args>
    iterator try_emplace(const_iterator hint, const KEY& key, Args&&... args);
    template <class... Args>
    iterator try_emplace(const_iterator                      hint,
                         BloombergLP::bslmf::MovableRef<KEY> key,
                         Args&&...                           args);
        // If a key equivalent to the specified 'key' already exists in this
        // map, return an iterator referring to the existing item.  Otherwise,
        // insert into this map a newly-created 'value_type' object,
        // constructed from from 'key' and the specified 'args', and return an
        // iterator referring to the newly-created entry.  Use the specified
        // 'hint' as a starting point for checking to see if the key already in
        // the map.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' are 'emplace-constructible' from 'key' and 'args'
        // respectively.  For C++03, 'VALUE' must also be 'copy-constructible'.
#endif

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all entries from this map.  Note that the map is empty after
        // this call, but allocated memory may be retained for future use.

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    iterator find(const key_type& key)
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this map whose key is equivalent to the specified 'key',
        // if such an entry exists, and the past-the-end ('end') iterator
        // otherwise.
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
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this map whose key is equivalent to the specified 'key',
        // if such an entry exists, and the past-the-end ('end') iterator
        // otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    iterator lower_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this map whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this map does not contain a 'value_type' object whose
        // key is greater-than or equal-to 'key'.  Note that this function
        // returns the *first* position before which a 'value_type' object
        // having an equivalent key could be inserted into the ordered sequence
        // maintained by this map, while preserving its ordering.
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
        // ordered least) 'value_type' object in this map whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this map does not contain a 'value_type' object whose
        // key is greater-than or equal-to 'key'.  Note that this function
        // returns the *first* position before which a 'value_type' object
        // having an equivalent key could be inserted into the ordered sequence
        // maintained by this map, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    iterator upper_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this map whose key is greater
        // than the specified 'key', and the past-the-end iterator if this map
        // does not contain a 'value_type' object whose key is greater-than
        // 'key'.  Note that this function returns the *last* position before
        // which a 'value_type' object having an equivalent key could be
        // inserted into the ordered sequence maintained by this map, while
        // preserving its ordering.
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
        // ordered least) 'value_type' object in this map whose key is greater
        // than the specified 'key', and the past-the-end iterator if this map
        // does not contain a 'value_type' object whose key is greater-than
        // 'key'.  Note that this function returns the *last* position before
        // which a 'value_type' object having an equivalent key could be
        // inserted into the ordered sequence maintained by this map, while
        // preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<iterator, iterator> equal_range(const key_type& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this map whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second is positioned
        // one past the end of the sequence.  The first returned iterator will
        // be 'lower_bound(key)', the second returned iterator will be
        // 'upper_bound(key)', and, if this map contains no 'value_type'
        // objects with an equivalent key, then the two returned iterators will
        // have the same value.  Note that since a map maintains unique keys,
        // the range will contain at most one element.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        iterator startIt = lower_bound(key);
        iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;
        }
        return pair<iterator, iterator>(startIt, endIt);
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<iterator, iterator> >::type
    equal_range(const LOOKUP_KEY& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this map whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second is positioned
        // one past the end of the sequence.  The first returned iterator will
        // be 'lower_bound(key)', the second returned iterator will be
        // 'upper_bound(key)', and, if this map contains no 'value_type'
        // objects with an equivalent key, then the two returned iterators will
        // have the same value.  Note that although a map maintains unique
        // keys, the range may contain more than one  element, because a
        // transparent comparator may have been supplied that provides a
        // different (but compatible) partitioning of keys for 'LOOKUP_KEY' as
        // the comparisons used to order the keys in the map.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        iterator startIt = lower_bound(key);
        iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;

            // Typically, even with a transparent comparator, we expect to find
            // either 0 or 1 matching keys. We test for those two common cases
            // before performing a logarithmic search via 'upper_bound' to
            // determine the end of the range.

            if (endIt != end() && !comparator()(key, *endIt.node())) {
                endIt = upper_bound(key);
            }
        }
        return pair<iterator, iterator>(startIt, endIt);
    }

    // BDE_VERIFY pragma: pop

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return (a copy of) the allocator used for memory allocation by this
        // map.

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or the 'end' iterator if this map is empty.

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type'
        // objects maintained by this map.

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this map, or 'rend' if this map is empty.

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this map.

    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or the 'cend' iterator if this map is empty.

    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this map.

    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this map, or 'crend' if this map is empty.

    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this map.

    bool contains(const key_type &key) const;
        // Return 'true' if this map contains an element whose key is
        // equivalent to the specified 'key'.

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        bool>::type
    contains(const LOOKUP_KEY& key) const
        // Return 'true' if this map contains an element whose key is
        // equivalent to the specified 'key'.
        //
        // Note: implemented inline due to Sun CC compilation error
    {
        return find(key) != end();
    }

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this map contains no elements, and 'false'
        // otherwise.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this map.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this map could possibly hold.  Note that there is no guarantee
        // that the map can successfully grow to the returned size, or even
        // close to that size without running out of resources.

    typename add_lvalue_reference<const VALUE>::type at(const key_type& key)
                                                                         const;
        // Return a reference providing non-modifiable access to the
        // mapped-value associated with a key that is equivalent to the
        // specified 'key', if such an entry exists; otherwise, throw a
        // 'std::out_of_range' exception.  Note that this method may also throw
        // a different kind of exception if the (user-supplied) comparator
        // throws.

    key_compare key_comp() const;
        // Return the key-comparison functor (or function pointer) used by this
        // map; if a comparator was supplied at construction, return its value;
        // otherwise, return a default constructed 'key_compare' object.  Note
        // that this comparator compares objects of type 'KEY', which is the
        // key part of the 'value_type' objects contained in this map.

    value_compare value_comp() const;
        // Return a functor for comparing two 'value_type' objects by comparing
        // their respective keys using 'key_comp()'.   Note that this
        // comparator compares objects of type 'value_type' (i.e.,
        // 'bsl::pair<const KEY, VALUE>').

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    const_iterator find(const key_type& key) const
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this map whose key is equivalent to the
        // specified 'key', if such an entry exists, and the past-the-end
        // ('end') iterator otherwise.
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
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this map whose key is equivalent to the
        // specified 'key', if such an entry exists, and the past-the-end
        // ('end') iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    size_type count(const key_type& key) const
        // Return the number of 'value_type' objects within this map whose keys
        // are equivalent to the specified 'key'.  Note that since a map
        // maintains unique keys, the returned value will be either 0 or 1.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return (find(key) != end()) ? 1 : 0;
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        size_type>::type
    count(const LOOKUP_KEY& key) const
        // Return the number of 'value_type' objects within this map whose keys
        // are equivalent to the specified 'key'.  Note that although a map
        // maintains unique keys, the returned value can be other than 0 or 1,
        // because a transparent comparator may have been supplied that
        // provides a different (but compatible) partitioning of keys for
        // 'LOOKUP_KEY' as the comparisons used to order the keys in the map.
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
        // (i.e., ordered least) 'value_type' object in this map whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this map does not contain a 'value_type' object whose
        // key is greater-than or equal-to 'key'.  Note that this function
        // returns the *first* position before which a 'value_type' object
        // having an equivalent key could be inserted into the ordered sequence
        // maintained by this map, while preserving its ordering.
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
        // (i.e., ordered least) 'value_type' object in this map whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this map does not contain a 'value_type' object whose
        // key is greater-than or equal-to 'key'.  Note that this function
        // returns the *first* position before which a 'value_type' object
        // having an equivalent key could be inserted into the ordered sequence
        // maintained by this map, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(
                BloombergLP::bslalg::RbTreeUtil::lowerBound(d_tree,
                                                            this->comparator(),
                                                            key));
    }

    const_iterator upper_bound(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this map whose key is
        // greater than the specified 'key', and the past-the-end iterator if
        // this map does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having an equivalent key
        // could be inserted into the ordered sequence maintained by this map,
        // while preserving its ordering.
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
        // (i.e., ordered least) 'value_type' object in this map whose key is
        // greater than the specified 'key', and the past-the-end iterator if
        // this map does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having an equivalent key
        // could be inserted into the ordered sequence maintained by this map,
        // while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this map whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second iterator is
        // positioned one past the end of the sequence.  The first returned
        // iterator will be 'lower_bound(key)', the second returned iterator
        // will be 'upper_bound(key)', and, if this map contains no
        // 'value_type' objects having keys equivalent to 'key', then the two
        // returned iterators will have the same value.  Note that since a map
        // maintains unique keys, the range will contain at most one element.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        const_iterator startIt = lower_bound(key);
        const_iterator endIt   = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;
        }
        return pair<const_iterator, const_iterator>(startIt, endIt);
    }

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<const_iterator, const_iterator> >::type
    equal_range(const LOOKUP_KEY& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this map whose keys are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second iterator is
        // positioned one past the end of the sequence.  The first returned
        // iterator will be 'lower_bound(key)', the second returned iterator
        // will be 'upper_bound(key)', and, if this map contains no
        // 'value_type' objects having keys equivalent to 'key', then the two
        // returned iterators will have the same value.  Note that although a
        // map maintains unique keys, the range may contain more than one
        // element,  because a transparent comparator may have been supplied
        // that provides a different (but compatible) partitioning of keys for
        // 'LOOKUP_KEY' as the comparisons used to order the keys in the map.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        const_iterator startIt = lower_bound(key);
        const_iterator   endIt = startIt;
        if (endIt != end() && !comparator()(key, *endIt.node())) {
            ++endIt;

            // Typically, even with a transparent comparator, we expect to find
            // either 0 or 1 matching keys. We test for those two common cases
            // before performing a logarithmic search via 'upper_bound' to
            // determine the end of the range.

            if (endIt != end() && !comparator()(key, *endIt.node())) {
                endIt = upper_bound(key);
            }
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
map(INPUT_ITERATOR,
    INPUT_ITERATOR,
    COMPARATOR = COMPARATOR(),
    ALLOCATOR = ALLOCATOR())
-> map<KEY, VALUE, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'map'.  Deduce the
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
map(INPUT_ITERATOR, INPUT_ITERATOR, COMPARATOR, ALLOC *)
-> map<KEY, VALUE, COMPARATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'map'.  Deduce the
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
map(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR)
-> map<KEY, VALUE, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'map'.  This deduction
    // guide does not participate unless the supplied allocator meets the
    // requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOC,
    class KEY = BloombergLP::bslstl::IteratorUtil::IterKey_t<INPUT_ITERATOR>,
    class VALUE =
               BloombergLP::bslstl::IteratorUtil::IterMapped_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
map(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> map<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the iterators supplied to the constructor of 'map'.  This deduction
    // guide does not participate unless the supplied allocator is convertible
    // to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
map(std::initializer_list<pair<const KEY, VALUE>>,
    COMPARATOR = COMPARATOR(),
    ALLOCATOR = ALLOCATOR())
-> map<KEY, VALUE, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'map'.  Deduce
    // the template parameters 'COMPARATOR' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.  This deduction guide does not
    // participate unless the supplied allocator meets the requirements of a
    // standard allocator.

template <
    class KEY,
    class VALUE,
    class COMPARATOR,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
map(std::initializer_list<pair<const KEY, VALUE>>, COMPARATOR, ALLOC *)
-> map<KEY, VALUE, COMPARATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'map'.  Deduce
    // the template parameter 'COMPARATOR' from the other parameters passed to
    // the constructor.  This deduction guide does not participate unless the
    // supplied allocator is convertible to
    // 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.

template <
    class KEY,
    class VALUE,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
map(std::initializer_list<pair<const KEY, VALUE>>, ALLOCATOR)
-> map<KEY, VALUE, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'map'.  Deduce
    // the template parameter 'ALLOCATOR' from the other parameter passed to
    // the constructor.  This deduction guide does not participate unless the
    // supplied allocator meets the requirements of a standard allocator.

template <
    class KEY,
    class VALUE,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE>>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
map(std::initializer_list<pair<const KEY, VALUE>>, ALLOC *)
-> map<KEY, VALUE>;
    // Deduce the template parameters 'KEY' and 'VALUE' from the 'value_type'
    // of the initializer_list supplied to the constructor of 'map'.  This
    // deduction guide does not participate unless the supplied allocator is
    // convertible to 'bsl::allocator<bsl::pair<const KEY, VALUE>>'.
#endif

// FREE OPERATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator==(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'map' objects 'lhs' and 'rhs' have
    // the same value if they have the same number of key-value pairs, and each
    // element in the ordered sequence of key-value pairs of 'lhs' has the same
    // value as the corresponding element in the ordered sequence of key-value
    // pairs of 'rhs'.  This method requires that the (template parameter)
    // types 'KEY' and 'VALUE' both be 'equality-comparable' (see {Requirements
    // on 'KEY' and 'VALUE'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator!=(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'map' objects 'lhs' and 'rhs' do
    // not have the same value if they do not have the same number of key-value
    // pairs, or some element in the ordered sequence of key-value pairs of
    // 'lhs' does not have the same value as the corresponding element in the
    // ordered sequence of key-value pairs of 'rhs'.  This method requires that
    // the (template parameter) types 'KEY' and 'VALUE' both be
    // 'equality-comparable' (see {Requirements on 'KEY' and 'VALUE'}).
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
BloombergLP::bslalg::SynthThreeWayUtil::Result<pair<const KEY, VALUE>>
operator<=>(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
            const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' maps by using the comparison operators of
    // 'bsl::pair<const KEY, VALUE>' on each element; return the result of that
    // comparison.

#else

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator<(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' map is
    // lexicographically less than that of the specified 'rhs' map, and 'false'
    // otherwise.  Given iterators 'i' and 'j' over the respective sequences
    // '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())', the value
    // of map 'lhs' is lexicographically less than that of map 'rhs' if
    // 'true == *i < *j' for the first pair of corresponding iterator positions
    // where '*i < *j' and '*j < *i' are not both 'false'.  If no such
    // corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator>(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' map is
    // lexicographically greater than that of the specified 'rhs' map, and
    // 'false' otherwise.  The value of map 'lhs' is lexicographically greater
    // than that of map 'rhs' if 'rhs' is lexicographically less than 'lhs'
    // (see 'operator<').  This method requires that 'operator<', inducing a
    // total order, be defined for 'value_type'.  Note that this operator
    // returns 'rhs < lhs'.

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator<=(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' map is
    // lexicographically less than or equal to that of the specified 'rhs' map,
    // and 'false' otherwise.  The value of map 'lhs' is lexicographically less
    // than or equal to that of map 'rhs' if 'rhs' is not lexicographically
    // less than 'lhs' (see 'operator<').  This method requires that
    // 'operator<', inducing a total order, be defined for 'value_type'.  Note
    // that this operator returns '!(rhs < lhs)'.

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator>=(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' map is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // map, and 'false' otherwise.  The value of map 'lhs' is lexicographically
    // greater than or equal to that of map 'rhs' if 'lhs' is not
    // lexicographically less than 'rhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY,
          class VALUE,
          class COMPARATOR,
          class ALLOCATOR,
          class PREDICATE>
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
erase_if(map<KEY, VALUE, COMPARATOR, ALLOCATOR>& m, PREDICATE predicate);
    // Erase all the elements in the specified map 'm' that satisfy the
    // specified predicate 'predicate'.  Return the number of elements erased.

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
void swap(map<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
          map<KEY, VALUE, COMPARATOR, ALLOCATOR>& b)
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
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                                              const COMPARATOR& comparator,
                                              const ALLOCATOR&  basicAllocator)
: ::bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator(comparator)
, d_pool(basicAllocator)
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                          BloombergLP::bslmf::MovableRef<DataWrapper> original)
: ::bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator(
                                    MoveUtil::access(original).keyComparator())
, d_pool(MoveUtil::move(MoveUtil::access(original).d_pool))
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory()
{
    return d_pool;
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory() const
{
    return d_pool;
}

                             // ------------------------
                             // class map::value_compare
                             // ------------------------

// CREATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::value_compare(
                                                         COMPARATOR comparator)
: comp(comparator)
{
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool map<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::operator()(
                                                     const value_type& x,
                                                     const value_type& y) const
{
    return comp(x.first, y.first);
}

                             // ---------
                             // class map
                             // ---------

// PRIVATE CLASS METHODS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::Node *
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::toNode(
                                         BloombergLP::bslalg::RbTreeNode *node)
{
    return static_cast<Node *>(node);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::Node *
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::toNode(
                                   const BloombergLP::bslalg::RbTreeNode *node)
{
    return static_cast<const Node *>(node);
}

// PRIVATE MANIPULATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_compAndAlloc.nodeFactory();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::comparator()
{
    return d_compAndAlloc;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::quickSwapExchangeAllocators(
                                                                    map& other)
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
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::quickSwapRetainAllocators(
                                                                    map& other)
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
const typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory() const
{
    return d_compAndAlloc.nodeFactory();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
const typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::Comparator&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_compAndAlloc;
}

// CREATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map()
: d_compAndAlloc(COMPARATOR(), ALLOCATOR())
, d_tree()
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(const map& original)
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
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(
                                  BloombergLP::bslmf::MovableRef<map> original)
: d_compAndAlloc(MoveUtil::move(MoveUtil::access(original).d_compAndAlloc))
, d_tree()
{
    map& lvalue = original;
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(const map&          original,
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
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(
                 BloombergLP::bslmf::MovableRef<map>            original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_compAndAlloc(MoveUtil::access(original).comparator().keyComparator(),
                 basicAllocator)
, d_tree()
{
    map& lvalue = original;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
              nodeFactory().allocator() == lvalue.nodeFactory().allocator())) {
        d_compAndAlloc.nodeFactory().adopt(
                          MoveUtil::move(lvalue.d_compAndAlloc.nodeFactory()));
        BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
    }
    else if (0 < lvalue.size()) {
        nodeFactory().reserveNodes(lvalue.size());
        BloombergLP::bslalg::RbTreeUtil::moveTree(&d_tree,
                                                  &lvalue.d_tree,
                                                  &nodeFactory(),
                                                  &lvalue.nodeFactory());
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(INPUT_ITERATOR    first,
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

            if (this->comparator()(*prevNode, value.first)) {
                BloombergLP::bslalg::RbTreeNode *node =
                                       nodeFactory().emplaceIntoNewNode(value);
                BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                                          prevNode,
                                                          false,
                                                          node);
                prevNode = node;
            }
        }
        proctor.release();
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(INPUT_ITERATOR   first,
                                            INPUT_ITERATOR   last,
                                            const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
    map other(first, last, COMPARATOR(), nodeFactory().allocator());
    quickSwapRetainAllocators(other);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(
                              std::initializer_list<value_type> values,
                              const COMPARATOR&                 comparator,
                              const ALLOCATOR&                  basicAllocator)
: map(values.begin(), values.end(), comparator, basicAllocator)
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(
                              std::initializer_list<value_type> values,
                              const ALLOCATOR&                  basicAllocator)
: map(values.begin(), values.end(), COMPARATOR(), basicAllocator)
{
}
#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::~map()
{
    clear();
}

// MANIPULATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator=(const map& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        if (AllocatorTraits::propagate_on_container_copy_assignment::value) {
            map other(rhs, rhs.nodeFactory().allocator());
            quickSwapExchangeAllocators(other);
        }
        else {
            map other(rhs, nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator=(
                                       BloombergLP::bslmf::MovableRef<map> rhs)
    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                            AllocatorTraits::is_always_equal::value &&
                            std::is_nothrow_move_assignable<COMPARATOR>::value)
{
    map& lvalue = rhs;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &lvalue)) {
        if (nodeFactory().allocator() == lvalue.nodeFactory().allocator()) {
            map other(MoveUtil::move(lvalue));
            quickSwapRetainAllocators(other);
        }
        else if (
              AllocatorTraits::propagate_on_container_move_assignment::value) {
            map other(MoveUtil::move(lvalue));
            quickSwapExchangeAllocators(other);
        }
        else {
            map other(MoveUtil::move(lvalue), nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator=(
                                      std::initializer_list<value_type> values)
{
    clear();
    insert(values.begin(), values.end());
    return *this;
}
#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename add_lvalue_reference<VALUE>::type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator[](const key_type& key)
{
    iterator iter = lower_bound(key);
    if (iter == end() || this->comparator()(key, *iter.node())) {
        BloombergLP::bsls::ObjectBuffer<VALUE> temp;  // for default 'VALUE'

        ALLOCATOR alloc = nodeFactory().allocator();

        AllocatorTraits::construct(alloc, temp.address());

        BloombergLP::bslma::DestructorGuard<VALUE> guard(temp.address());

        // Unfortunately, in C++03, there are user types where a MovableRef
        // will not safely degrade to a lvalue reference when a move
        // constructor is not available, so 'move' cannot be used directly on a
        // user supplied type.  See internal bug report 99039150.
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        iter = emplace_hint(iter, key, MoveUtil::move(temp.object()));
#else
        iter = emplace_hint(iter, key, temp.object());
#endif
    }
    return iter->second;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename add_lvalue_reference<VALUE>::type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator[](
                                  BloombergLP::bslmf::MovableRef<key_type> key)
{
    key_type& lvalue = key;

    iterator iter = lower_bound(lvalue);
    if (iter == end() || this->comparator()(lvalue, *iter.node())) {
        BloombergLP::bsls::ObjectBuffer<VALUE> temp;  // for default 'VALUE'

        ALLOCATOR alloc = nodeFactory().allocator();

        AllocatorTraits::construct(alloc, temp.address());

        BloombergLP::bslma::DestructorGuard<VALUE> guard(temp.address());

        // Unfortunately, in C++03, there are user types where a MovableRef
        // will not safely degrade to a lvalue reference when a move
        // constructor is not available, so 'move' cannot be used directly on a
        // user supplied type.  See internal bug report 99039150.
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        iter = emplace_hint(iter,
                            MoveUtil::move(lvalue),
                            MoveUtil::move(temp.object()));
#else
        iter = emplace_hint(iter,
                            lvalue,
                            temp.object());
#endif
    }
    return iter->second;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
typename add_lvalue_reference<VALUE>::type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::at(const key_type& key)
{
    BloombergLP::bslalg::RbTreeNode *node =
        BloombergLP::bslalg::RbTreeUtil::find(d_tree, this->comparator(), key);
    if (d_tree.sentinel() == node) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                 "map<...>::at(key_type): invalid key value");
    }
    return toNode(node)->value().second;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::end() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const value_type& value)
{
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value.first);
    if (!comparisonResult) {
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    value_type& lvalue = value;

    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            lvalue.first);
    if (!comparisonResult) {
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(MoveUtil::move(lvalue));
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                    INPUT_ITERATOR last)
{
    ///Implementation Notes
    ///--------------------
    // First, consume currently held free nodes.  If those nodes are
    // insufficient *and* one can calculate the remaining number of elements,
    // then reserve exactly that many free nodes.  There is no more than one
    // call to 'reserveNodes' per invocation of this method, hence the use of
    // 'BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY'.  When reserving nodes, we
    // assume the elements remaining to be inserted have unique keys that do
    // not duplicate any keys already in the container.  If there are any
    // duplicates, this container will have free nodes on return from this
    // method.

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
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const_iterator first,
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
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(const_iterator    hint,
                                               const value_type& value)
{
    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value.first,
                                                            hintNode);
    if (!comparisonResult) {
        return iterator(insertLocation);                              // RETURN
    }

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(
                              const_iterator                             hint,
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    value_type& lvalue = value;

    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                                            &comparisonResult,
                                                            &d_tree,
                                                            this->comparator(),
                                                            lvalue.first,
                                                            hintNode);
    if (!comparisonResult) {
        return iterator(insertLocation);                              // RETURN
    }

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(MoveUtil::move(lvalue));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(
                                      std::initializer_list<value_type> values)
{
    insert(values.begin(), values.end());
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class BDE_OTHER_TYPE>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert_or_assign(const key_type&  key,
                                                         BDE_OTHER_TYPE&& obj)
{
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               key);

    if (!comparisonResult) { // ASSIGN
        iterator(insertLocation)->second =
            BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj);
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

    // INSERT
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        key, BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);

    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class BDE_OTHER_TYPE>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert_or_assign(const_iterator   hint,
                                                         const key_type&  key,
                                                         BDE_OTHER_TYPE&& obj)
{
    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               key,
                               hintNode);

    if (!comparisonResult) { // ASSIGN
        iterator(insertLocation)->second =
            BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj);
        return iterator(insertLocation);                              // RETURN
    }

    // INSERT
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        key, BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);

    return iterator(node);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class BDE_OTHER_TYPE>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert_or_assign(
                                  BloombergLP::bslmf::MovableRef<key_type> key,
                                  BDE_OTHER_TYPE&&                         obj)
{
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               key);

    if (!comparisonResult) { // ASSIGN
        iterator(insertLocation)->second =
            BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj);
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

    // INSERT
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        BSLS_COMPILERFEATURES_FORWARD(key_type, key),
        BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);

    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class BDE_OTHER_TYPE>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert_or_assign(
                                 const_iterator                           hint,
                                 BloombergLP::bslmf::MovableRef<key_type> key,
                                 BDE_OTHER_TYPE&&                         obj)
{
    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               key,
                               hintNode);

    if (!comparisonResult) { // ASSIGN
        iterator(insertLocation)->second =
            BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj);
        return iterator(insertLocation);                              // RETURN
    }

    // INSERT
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
        BSLS_COMPILERFEATURES_FORWARD(key_type, key),
        BSLS_COMPILERFEATURES_FORWARD(BDE_OTHER_TYPE, obj));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);

    return iterator(node);
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::emplace(Args&&... args)
{
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
                                 BSLS_COMPILERFEATURES_FORWARD(Args, args)...);
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               static_cast<const Node *>(node)->value().first);
    if (!comparisonResult) {
        nodeFactory().deleteNode(node);
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::emplace_hint(const_iterator hint,
                                                     Args&&...      args)
{
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
                                 BSLS_COMPILERFEATURES_FORWARD(Args, args)...);
    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                                &comparisonResult,
                                &d_tree,
                                this->comparator(),
                                static_cast<const Node *>(node)->value().first,
                                hintNode);
    if (!comparisonResult) {
        nodeFactory().deleteNode(node);
        return iterator(insertLocation);                              // RETURN
    }

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const_iterator position)
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
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(iterator position)
{
    return erase(const_iterator(position));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const key_type& key)
{
    const_iterator it = find(key);
    if (it == end()) {
        return 0;                                                     // RETURN
    }
    erase(it);
    return 1;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::erase(const_iterator first,
                                              const_iterator last)
{
    while (first != last) {
        first = erase(first);
    }
    return iterator(last.node());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::swap(map& other)
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

            map toOtherCopy(MoveUtil::move(*this),
                            other.nodeFactory().allocator());
            map toThisCopy(MoveUtil::move(other), nodeFactory().allocator());

            this->quickSwapRetainAllocators(toThisCopy);
            other.quickSwapRetainAllocators(toOtherCopy);
        }
    }
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::try_emplace(const key_type& key,
                                                    Args&&...       args)
{
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               key);
    if (!comparisonResult) {
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          std::piecewise_construct,
          std::forward_as_tuple(key),
          std::forward_as_tuple(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#else
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          key,
          mapped_type(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#endif

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::try_emplace(const_iterator  hint,
                                                    const key_type& key,
                                                    Args&&...       args)
{
    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               key,
                               hintNode);
    if (!comparisonResult) {
        return iterator(insertLocation);                              // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          std::piecewise_construct,
          std::forward_as_tuple(key),
          std::forward_as_tuple(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#else
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          key,
          mapped_type(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#endif

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::try_emplace(
                                 BloombergLP::bslmf::MovableRef<key_type> key,
                                 Args&&...                                args)
{
    key_type& lvalue = key;

    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               lvalue);
    if (!comparisonResult) {
        return pair<iterator, bool>(iterator(insertLocation), false); // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          std::piecewise_construct,
          std::forward_as_tuple(BSLS_COMPILERFEATURES_FORWARD(key_type, key)),
          std::forward_as_tuple(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#else
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          BSLS_COMPILERFEATURES_FORWARD(key_type, key),
          mapped_type(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#endif

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);

    return pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::try_emplace(
                                 const_iterator                           hint,
                                 BloombergLP::bslmf::MovableRef<key_type> key,
                                 Args&&...                                args)
{
    key_type& lvalue = key;

    BloombergLP::bslalg::RbTreeNode *hintNode =
                    const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());
    int comparisonResult;
    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findUniqueInsertLocation(
                               &comparisonResult,
                               &d_tree,
                               this->comparator(),
                               lvalue,
                               hintNode);
    if (!comparisonResult) {
        return iterator(insertLocation);                              // RETURN
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          std::piecewise_construct,
          std::forward_as_tuple(BSLS_COMPILERFEATURES_FORWARD(key_type, key)),
          std::forward_as_tuple(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#else
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
          BSLS_COMPILERFEATURES_FORWARD(key_type, key),
          mapped_type(BSLS_COMPILERFEATURES_FORWARD(Args, args)...));
#endif

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}
#endif

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT_SAFE(d_tree.firstNode());

    if (d_tree.rootNode()) {
        BSLS_ASSERT_SAFE(                 0 <  d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() != d_tree.sentinel());

        BloombergLP::bslalg::RbTreeUtil::deleteTree(&d_tree, &nodeFactory());
    }
#if defined(BSLS_ASSERT_SAFE_IS_USED)
    else {
        BSLS_ASSERT_SAFE(                 0 == d_tree.numNodes());
        BSLS_ASSERT_SAFE(d_tree.firstNode() == d_tree.sentinel());
    }
#endif
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::allocator_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return nodeFactory().allocator();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return cbegin();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return cend();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return crbegin();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return crend();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool map<KEY, VALUE, COMPARATOR, ALLOCATOR>::contains(
                                                     const key_type& key) const
{
    return find(key) != end();
}

// capacity:
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool map<KEY, VALUE, COMPARATOR, ALLOCATOR>::empty() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
typename add_lvalue_reference<const VALUE>::type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::at(
                                                     const key_type& key) const
{
    const BloombergLP::bslalg::RbTreeNode *node =
                      BloombergLP::bslalg::RbTreeUtil::find(d_tree,
                                                            this->comparator(),
                                                            key);
    if (d_tree.sentinel() == node) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange(
                                 "map<...>::at(key_type): invalid key value");
    }
    return toNode(node)->value().second;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::key_compare
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::key_comp() const
{
    return comparator().keyComparator();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_comp() const
{
    return value_compare(key_comp());
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator==(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator!=(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
BloombergLP::bslalg::SynthThreeWayUtil::Result<bsl::pair<const KEY, VALUE>>
bsl::operator<=>(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                 const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                    const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
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
bool bsl::operator>(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                    const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<=(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>=(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY,
          class VALUE,
          class COMPARATOR,
          class ALLOCATOR,
          class PREDICATE>
inline
typename bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
bsl::erase_if(map<KEY, VALUE, COMPARATOR, ALLOCATOR>& m, PREDICATE predicate)
{
    return BloombergLP::bslstl::AlgorithmUtil::containerEraseIf(m, predicate);
}

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
void bsl::swap(bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
               bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& b)
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

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
struct HasStlIterators<bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::true_type
{
};

}  // close namespace bslalg

namespace bslma {

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
struct UsesBslmaAllocator<bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::is_convertible<Allocator *, ALLOCATOR>
{
};

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
