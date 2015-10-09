// bslstl_map.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_MAP
#define INCLUDED_BSLSTL_MAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant map class.
//
//@CLASSES:
//   bsl::map: STL-compliant map template
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
// if a either the key or value type cannot be tested for equality, then a
// 'map' containing that type cannot be tested for equality.  It is even
// possible to instantiate 'map' with a key or mapped-value type that does not
// have a copy-constructor, in which case the 'map' will not be copyable.
//
// A map meets the requirements of an associative container with bidirectional
// iterators in the C++ standard [23.2.4].  The 'map' implemented here adheres
// to the C++11 standard, except that it does not have interfaces that take
// rvalue references, 'initializer_lists', 'emplace', or operations taking a
// variadic number of template parameters.  Note that excluded C++11 features
// are those that require (or are greatly simplified by) C++11 compiler
// support.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// A 'map' is a fully "Value-Semantic Type" (see {'bsldoc_glossary'}) only if
// the supplied 'KEY' and 'VALUE' template parameters are themselves fully
// value-semantic.  It is possible to instantiate a 'map' with 'KEY' and
// 'VALUE' parameter arguments that do not provide a full set of value-semantic
// operations, but then some methods of the container may not be instantiable.
// The following terminology, adopted from the C++11 standard, is used in the
// function documentation of 'map' to describe a function's requirements for
// the 'KEY' and 'VALUE' template parameters.  These terms are also defined in
// section [17.6.3.1] of the C++11 standard.  Note that, in the context of a
// 'map' instantiation, the requirements apply specifically to the map's entry
// type, 'value_type', which is an alias for 'bsl::pair<KEY, VALUE>'.
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
// The type supplied as a map's 'ALLOCATOR' template parameter determines how
// that map will allocate memory.  The 'map' template supports allocators
// meeting the requirements of the C++11 standard [17.6.3.5].  In addition, it
// supports scoped-allocators derived from the 'bslma::Allocator' memory
// allocation protocol.  Clients intending to use 'bslma' style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator', provides a C++11
// standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of an 'map' instantiation' is
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
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'value_type'    - map<K, V>::value_type
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
//  | map<K, V> a;    (default construction)             | O[1]               |
//  | map<K, V> a(al);                                   |                    |
//  | map<K, V> a(c, al);                                |                    |
//  +----------------------------------------------------+--------------------+
//  | map<K, V> a(b); (copy construction)                | O[n]               |
//  | map<K, V> a(b, al);                                |                    |
//  +----------------------------------------------------+--------------------+
//  | map<K, V> a(i1, i2);                               | O[N] if [i1, i2)   |
//  | map<K, V> a(i1, i2, al);                           | is sorted with     |
//  | map<K, V> a(i1, i2, c, al);                        | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where N |
//  |                                                    | is distance(i1,i2) |
//  +----------------------------------------------------+--------------------+
//  | a.~map<K, V>(); (destruction)                      | O[n]               |
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
//  | a[k]                                               | O[log(n)]          |
//  +----------------------------------------------------+--------------------+
//  | a.at(k)                                            | O[log(n)]          |
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
// ascending price order.  'BuyOrderMap' uses the 'bsl::greater' comparator to
// store the sequence of buy orders in descending price order.  Also note that
// we use the default 'ALLOCATOR' template parameter for both aliases as we
// intend to provide memory with 'bslma' style allocators:
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
//      // This class does not support copy construction or copy assignment.
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
//      while (numShares && itr != d_sellOrders.upper_bound(price))
//      {
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
//      while (numShares && itr != d_buyOrders.upper_bound(price))
//      {
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

#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_map.h> instead of <bslstl_map.h> in \
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

                             // =========
                             // class map
                             // =========

template <class KEY,
          class VALUE,
          class COMPARATOR = std::less<KEY>,
          class ALLOCATOR  = allocator<bsl::pair<const KEY, VALUE> > >
class map {
    // This class template implements a value-semantic container type holding
    // an ordered sequence of key-value pairs having unique keys that provide a
    // mapping from keys (of the template parameter type, 'KEY') to their
    // associated values (of another template parameter type, 'VALUE').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    // PRIVATE TYPES
    typedef pair<const KEY, VALUE> ValueType;
        // This typedef is an alias for the type of key-value pair objects
        // maintained by this map.

    typedef BloombergLP::bslstl::MapComparator<KEY, VALUE, COMPARATOR>
                                                                    Comparator;
        // This typedef is an alias for the comparator used internally by this
        // map.

    typedef BloombergLP::bslstl::TreeNode<ValueType> Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this map.

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

    typedef BloombergLP::bslstl::TreeIterator<
                                   value_type, Node, difference_type> iterator;
    typedef BloombergLP::bslstl::TreeIterator<
                       const value_type, Node, difference_type> const_iterator;
    typedef bsl::reverse_iterator<iterator>            reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>      const_reverse_iterator;

    class value_compare {
        // This nested class defines a mechanism for comparing two objects of
        // 'value_type' using the (template parameter) type 'COMPARATOR'.  Note
        // that this class exactly matches its definition in the C++11 standard
        // [23.4.4.1]; otherwise we would have implemented it as a separate
        // component-local class.

        // FRIENDS
        friend class map;

      protected:
        COMPARATOR comp;  // we would not have elected to make this data
                          // member protected ourselves

        value_compare(COMPARATOR comparator);                       // IMPLICIT
            // Create a 'value_compare' object that will delegate to the
            // specified 'comparator' for comparisons.

      public:
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

        bool operator()(const value_type& x, const value_type& y) const;
            // Return 'true' if the specified 'x' object is ordered before the
            // specified 'y' object, as determined by the comparator supplied
            // at construction.
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

    void quickSwap(map& other);
        // Efficiently exchange the value and comparator of this object with
        // the value of the specified 'other' object.  This method provides the
        // no-throw exception-safety guarantee.  The behavior is undefined
        // unless this object was created with the same allocator as 'other'.

    // PRIVATE ACCESSORS
    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the node
        // allocator for this map.

    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this map.

  public:
    // CREATORS
    explicit map(const COMPARATOR& comparator     = COMPARATOR(),
                 const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Construct an empty map.  Optionally specify a 'comparator' used to
        // order key-value pairs contained in this object.  If 'comparator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'COMPARATOR' is used.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'ALLOCATOR' is used.  If the 'ALLOCATOR' is 'bsl::allocator'
        // (the default), then 'basicAllocator', if supplied, shall be
        // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used to supply memory.
    : d_compAndAlloc(comparator, basicAllocator)
    , d_tree()
    {
        // The implementation is placed here in the class definition to
        // workaround an AIX compiler bug, where the constructor can fail to
        // compile because it is unable to find the definition of the default
        // argument.  This occurs when a templatized class wraps around the
        // container and the comparator is defined after the new class.
    }

    explicit map(const ALLOCATOR& basicAllocator);
        // Construct an empty map that will use the specified 'basicAllocator'
        // to supply memory.  Use a default-constructed object of the (template
        // parameter) type 'COMPARATOR' to order the key-value pairs contained
        // in this map.  If the (template parameter) type 'ALLOCATOR' is of
        // 'bsl::allocator' (the default), then 'basicAllocator' shall be
        // convertible to 'bslma::Allocator *'.

    map(const map& original);
        // Construct a map having the same value as the specified 'original'.
        // Use a copy of 'original.key_comp()' to order the key-value pairs
        // contained in this map.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.allocator())' to
        // allocate memory.  If the (template parameter) type 'ALLOCATOR' is of
        // type 'bsl::allocator' (the default), the currently installed default
        // allocator is used to supply memory.  Note that this method requires
        // that the (template parameter) types 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    map(const map& original, const ALLOCATOR& basicAllocator);
        // Construct a map having the same value as that of the specified
        // 'original' that will use the specified 'basicAllocator' to supply
        // memory.  Use a copy of 'original.key_comp()' to order the key-value
        // pairs contained in this map.  If the (template parameter) type
        // 'ALLOCATOR' is 'bsl::allocator' (the default), then 'basicAllocator'
        // shall be convertible to 'bslma::Allocator *'.  Note that this method
        // requires that the (template parameter) types 'KEY' and 'VALUE' types
        // both be "copy-constructible" (see {Requirements on 'KEY' and
        // 'VALUE'}).

    template <class INPUT_ITERATOR>
    map(INPUT_ITERATOR    first,
        INPUT_ITERATOR    last,
        const COMPARATOR& comparator = COMPARATOR(),
        const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Construct a map, and insert each 'value_type' object in the sequence
        // starting at the specified 'first' element, and ending immediately
        // before the specified 'last' element, ignoring those pairs having a
        // key that appears earlier in the sequence.  Optionally specify a
        // 'comparator' used to order key-value pairs contained in this object.
        // If 'comparator' is not supplied, a default-constructed object of the
        // (template parameter) type 'COMPARATOR' is used.  Optionally specify
        // the 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' (the default), then 'basicAllocator', if supplied,
        // shall be convertible to 'bslma::Allocator *'.  If the type
        // 'ALLOCATOR' is 'bsl::allocator' and 'basicAllocator' is not
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

    ~map();
        // Destroy this object.

    // MANIPULATORS
    map& operator=(const map& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // Note that this method requires that the (template parameter types)
        // 'KEY' and 'VALUE' both be "copy-constructible" (see {Requirements on
        // 'KEY' and 'VALUE'}).

    VALUE& operator[](const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key'; if this 'map' does not already
        // contain a 'value_type' object with 'key', first insert a new
        // 'value_type' object having 'key' and a default-constructed 'VALUE'
        // object, and return a reference to the mapped value.  Note that this
        // method requires that the (template parameter) types 'KEY' and
        // 'VALUE' both be "default-constructible" (see {Requirements on 'KEY'
        // and 'VALUE'}).

    VALUE& at(const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key', if such an entry exists;
        // otherwise throw an 'std::out_of_range' exception.  Note that this
        // method is not exception-neutral.

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or the 'end' iterator if this map is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this map.

    reverse_iterator rbegin();
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or 'rend' if this map is empty.

    reverse_iterator rend();
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this map.

    bsl::pair<iterator, bool> insert(const value_type& value);
        // Insert the specified 'value' into this map if the key (the 'first'
        // element) of the 'value' does not already exist in this map;
        // otherwise, if a 'value_type' object having the same key as 'value'
        // already exists in this map, this method has no effect.  Return a
        // pair whose 'first' member is an iterator referring to the (possibly
        // newly inserted) 'value_type' object in this map whose key is the
        // same as that of 'value', and whose 'second' member is 'true' if a
        // new value was inserted, and 'false' if the value was already
        // present.  Note that this method requires that the (template
        // parameter) types 'KEY' and 'VALUE' types both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this map (in amortized constant
        // time if the specified 'hint' is a valid immediate successor to the
        // key of 'value'), if the key (the 'first' element) of the 'value'
        // does not already exist in this map; otherwise, if a 'value_type'
        // object having the same key as 'value' already exists in this map,
        // this method has no effect.  Return an iterator referring to the
        // (possibly newly inserted) 'value_type' object in this map whose key
        // is the same as that of 'value'.  If 'hint' is not a valid immediate
        // successor to the key of 'value', this operation has 'O[log(N)]'
        // complexity, where 'N' is the size of this map.  The behavior is
        // undefined unless 'hint' is a valid iterator into this map.  Note
        // that this method requires that the (template parameter) types 'KEY'
        // and 'VALUE' both be "copy-constructible" (see {Requirements on 'KEY'
        // and 'VALUE'}).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this map the value of each 'value_type' object in the
        // range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator, whose key is not
        // already contained in this map.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type'.  The behavior is undefined
        // unless 'first' and 'last' refer to a sequence of valid values where
        // 'first' is at a position at or before 'last'.  Note that this method
        // requires that the (template parameter) types 'KEY' and 'VALUE' both
        // be "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    iterator erase(const_iterator position);
        // Remove from this map the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this map.  The behavior is undefined
        // unless 'position' refers to a 'value_type' object in this map.

    size_type erase(const key_type& key);
        // Remote from this map the 'value_type' object having the specified
        // 'key', if it exists, and return 1; otherwise, if there is no
        // 'value_type' object having 'key', return 0 with no other effect.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this map the 'value_type' objects starting at the
        // specified 'first' position up to, but including the specified 'last'
        // position, and return 'last'.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this map or are the
        // 'end' iterator, and the 'first' position is at or before the 'last'
        // position in the ordered sequence provided by this container.

    void swap(map& other);
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
        // Remove all entries from this map.  Note that the map is empty after
        // this call, but allocated memory may be retained for future use.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the 'value_type'
        // object in this map having the specified 'key', if such an entry
        // exists, and the past-the-end ('end') iterator otherwise.

    iterator lower_bound(const key_type& key);
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this map whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this map does not contain a 'value_type' object whose
        // key is greater-than or equal-to 'key'.  Note that this function
        // returns the *first* position before which a 'value_type' object
        // having 'key' could be inserted into the ordered sequence maintained
        // by this map, while preserving its ordering.

    iterator upper_bound(const key_type& key);
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this map whose key is greater
        // than the specified 'key', and the past-the-end iterator if this map
        // does not contain a 'value_type' object whose key is greater-than
        // 'key'.  Note that this function returns the *last* position before
        // which a 'value_type' object having 'key' could be inserted into the
        // ordered sequence maintained by this map, while preserving its
        // ordering.

    bsl::pair<iterator,iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this map having the specified
        // 'key', where the the first iterator is positioned at the start of
        // the sequence, and the second is positioned one past the end of the
        // sequence.  The first returned iterator will be 'lower_bound(key)';
        // the second returned iterator will be 'upper_bound(key)'; and, if
        // this map contains no 'value_type' objects having 'key', then the two
        // returned iterators will have the same value.  Note that since a map
        // maintains unique keys, the range will contain at most one element.

    // ACCESSORS
    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // map.

    const_iterator begin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or the 'end' iterator if this map is empty.

    const_iterator end() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type'
        // objects maintained by this map.

    const_reverse_iterator rbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this map, or 'rend' if this map is empty.

    const_reverse_iterator rend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this map.

    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this map, or the 'cend' iterator if this map is empty.

    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this map.

    const_reverse_iterator crbegin() const;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this map, or 'crend' if this set is empty.

    const_reverse_iterator crend() const;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this map.

    bool empty() const;
        // Return 'true' if this map contains no elements, and 'false'
        // otherwise.

    size_type size() const;
        // Return the number of elements in this map.

    size_type max_size() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this map could possibly hold.  Note that there is no guarantee
        // that the map can successfully grow to the returned size, or even
        // close to that size without running out of resources.

    const VALUE& at(const key_type& key) const;
        // Return a reference providing non-modifiable access to the
        // mapped-value associated with the specified 'key', if such an entry
        // exists; otherwise throw an 'std::out_of_range' exception.  Note
        // that this method is not exception-neutral.

    key_compare key_comp() const;
        // Return the key-comparison functor (or function pointer) used by this
        // map; if a comparator was supplied at construction, return its value,
        // otherwise return a default constructed 'key_compare' object.  Note
        // that this comparator compares objects of type 'KEY', which is the
        // key part of the 'value_type' objects contained in this map.

    value_compare value_comp() const;
        // Return a functor for comparing two 'value_type' objects by comparing
        // their respective keys using 'key_comp()'.   Note that this
        // comparator compares objects of type 'value_type' (i.e.,
        // 'bsl::pair<KEY, VALUE>').

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // 'value_type' object in this map having the specified 'key', if such
        // an entry exists, and the past-the-end ('end') iterator otherwise.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this map having the
        // specified 'key'.  Note that since a map maintains unique keys, the
        // returned value will be either 0 or 1.

    const_iterator lower_bound(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this map whose key is
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this map does not contain a 'value_type' object whose
        // key is greater-than or equal-to 'key'.  Note that this function
        // returns the *first* position before which a 'value_type' object
        // having 'key' could be inserted into the ordered sequence maintained
        // by this map, while preserving its ordering.

    const_iterator upper_bound(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this map whose key is
        // greater than the specified 'key', and the past-the-end iterator if
        // this map does not contain a 'value_type' object whose key is
        // greater-than 'key'.  Note that this function returns the *last*
        // position before which a 'value_type' object having 'key' could be
        // inserted into the ordered sequence maintained by this map, while
        // preserving its ordering.

    bsl::pair<const_iterator,const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this map having the specified
        // 'key', where the the first iterator is positioned at the start of
        // the sequence and the second iterator is positioned one past the end
        // of the sequence.  The first returned iterator will be
        // 'lower_bound(key)', the second returned iterator will be
        // 'upper_bound(key)', and if this map contains no 'value_type' objects
        // having 'key', then the two returned iterators will have the same
        // value.  Note that since a map maintains unique keys, the range will
        // contain at most one element.

    // NOT IMPLEMENTED
        // The following methods are defined by the C++11 standard, but they
        // are not implemented as they require some level of C++11 compiler
        // support not currently available on all supported platforms.

    // map(map&&, const ALLOCATOR&);

    // map(initializer_list<value_type>,
    //     const COMPARATOR& = COMPARATOR(),
    //     const ALLOCATOR& = ALLOCATOR());

    // map<KEY, VALUE, COMPARATOR, ALLOCATOR>&
    // operator=(map<KEY, VALUE, COMPARATOR, ALLOCATOR>&& rhs);

    // map& operator=(initializer_list<value_type>);

    // T& operator[](key_type&& x);

    // template <class... Args> pair<iterator, bool> emplace(Args&&... args);

    // template <class... Args> iterator emplace_hint(const_iterator position,
    //                                                Args&&... args);

    // template <class P> pair<iterator, bool> insert(P&& value);

    // template <class P>
    // iterator insert(const_iterator position, P&&);

    // void insert(initializer_list<value_type>);
};

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
bool operator==(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'map' objects have the same value if
    // they have the same number of key-value pairs, and each key-value pair
    // that is contained in one of the objects is also contained in the other
    // object.  Note that this method requires that the (template parameter)
    // types 'KEY' and 'VALUE' both be "equality-comparable" (see {Requirements
    // on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator!=(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'map' objects do not have the
    // same value if they do not have the same number of key-value pairs, or
    // some key-value pair that is contained in one of the objects is not also
    // contained in the other object.  Note that this method requires that the
    // (template parameter) types 'KEY' and 'VALUE' both be
    // "equality-comparable" (see {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator<(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is less than the specified
    // 'rhs' value, and 'false' otherwise.  A map, 'lhs', has a value that is
    // less than that of 'rhs', if, for the first non-equal corresponding
    // key-value pairs in their respective sequences, the 'lhs' key-value pair
    // is less than the 'rhs' pair, or, if the keys of all of their
    // corresponding key-value pairs compare equal, 'lhs' has fewer key-value
    // pairs than 'rhs'.  Note that this method requires that the (template
    // parameter) types 'KEY' and 'VALUE' both be "less-than-comparable" (see
    // {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator>(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
               const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is greater than the specified
    // 'rhs' value, and 'false' otherwise.  A map, 'lhs', has a value that is
    // greater than that of 'rhs', if, for the first non-equal corresponding
    // key-value pairs in their respective sequences, the 'lhs' key-value pair
    // is greater than the 'rhs' pair, or, if the keys of all of their
    // corresponding key-value pairs compare equal, 'lhs' has more key-value
    // pairs than 'rhs'.  Note that this method requires that the (template
    // parameter) types 'KEY' and 'VALUE' both be "less-than-comparable" (see
    // {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator<=(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is less-than or equal-to the
    // specified 'rhs' value, and 'false' otherwise.  A map, 'lhs', has a value
    // that is less-than or equal-to that of 'rhs', if, for the first non-equal
    // corresponding key-value pairs in their respective sequences, the 'lhs'
    // key-value pair is less than the 'rhs' pair, or, if the keys of all of
    // their corresponding key-value pairs compare equal, 'lhs' has less-than
    // or equal number of key-value pairs as 'rhs'.  Note that this method
    // requires that the (template parameter) types 'KEY' and 'VALUE' both be
    // "less-than-comparable" (see {Requirements on 'KEY' and 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
bool operator>=(const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                const map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' value is greater-than or equal-to
    // the specified 'rhs' value, and 'false' otherwise.  A map, 'lhs', has a
    // value that is greater-than or equal-to that of 'rhs', if, for the first
    // non-equal corresponding key-value pairs in their respective sequences,
    // the 'lhs' key-value pair is greater than the 'rhs' pair, or, if the keys
    // of all of their corresponding key-value pairs compare equal, 'lhs' has
    // greater-than or equal number of key-value pairs as 'rhs'.  Note that
    // this method requires that the (template parameter) types 'KEY' and
    // 'VALUE' both be "less-than-comparable" (see {Requirements on 'KEY' and
    // 'VALUE'}).

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
void swap(map<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
          map<KEY, VALUE, COMPARATOR, ALLOCATOR>& b);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  Additionally, if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true', then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees 'O[1]' complexity.  The
    // behavior is undefined unless either this object was created with the
    // same allocator as 'other' or 'propagate_on_container_swap' is 'true'.


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

                             // ------------------------
                             // class map::value_compare
                             // ------------------------

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::value_compare(
                                                         COMPARATOR comparator)
: comp(comparator)
{
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool map<KEY, VALUE, COMPARATOR, ALLOCATOR>::value_compare::operator()(
                                const value_type& x, const value_type& y) const
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
    return d_compAndAlloc.d_pool;
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
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::quickSwap(map& other)
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
const typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::NodeFactory&
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::nodeFactory() const
{
    return d_compAndAlloc.d_pool;
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
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::map(const map&       original,
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
                                               nodeFactory().createNode(value);
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
            BloombergLP::bslalg::SwapUtil::swap(
                                             &nodeFactory().allocator(),
                                             &other.nodeFactory().allocator());
            quickSwap(other);
        }
        else {
            map other(rhs, nodeFactory().allocator());
            quickSwap(other);
        }
    }
    return *this;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
VALUE& map<KEY, VALUE, COMPARATOR, ALLOCATOR>::operator[](const key_type& key)
{
    BloombergLP::bslalg::RbTreeNode *node =
        BloombergLP::bslalg::RbTreeUtil::find(d_tree, this->comparator(), key);
    if (d_tree.sentinel() == node) {
        return insert(iterator(node), value_type(key, VALUE()))->second;
                                                                      // RETURN
    }
    return toNode(node)->value().second;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
VALUE& map<KEY, VALUE, COMPARATOR, ALLOCATOR>::at(const key_type& key)
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
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin()
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::end()
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin()
{
    return reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend()
{
    return reverse_iterator(begin());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bsl::pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator, bool>
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
        return bsl::pair<iterator, bool>(iterator(insertLocation), false);
                                                                      // RETURN
    }
    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return bsl::pair<iterator, bool>(iterator(node), true);
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                    INPUT_ITERATOR last)
{
    while (first != last) {
        insert(*first);
        ++first;
    }
}

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

    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().createNode(value);
    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              comparisonResult < 0,
                                              node);
    return iterator(node);
}

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
            map thisCopy(*this, other.nodeFactory().allocator());
            map otherCopy(other, nodeFactory().allocator());

            quickSwap(otherCopy);
            other.quickSwap(thisCopy);
        }
    }
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
void map<KEY, VALUE, COMPARATOR, ALLOCATOR>::clear()
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
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::find(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::find(d_tree,
                                                          this->comparator(),
                                                          key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::lower_bound(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::upper_bound(const key_type& key)
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bsl::pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator,
          typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::iterator>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::equal_range(const key_type& key)
{
    iterator startIt = lower_bound(key);
    iterator endIt   = startIt;
    if (endIt != end() && !comparator()(key, *endIt.node())) {
        ++endIt;
    }
    return bsl::pair<iterator, iterator>(startIt, endIt);
}

// ACCESSORS
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::allocator_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::get_allocator() const
{
    return nodeFactory().allocator();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::begin() const
{
    return cbegin();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::end() const
{
    return cend();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rbegin() const
{
    return crbegin();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::rend() const
{
    return crend();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::cend() const
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::crbegin() const
{
    return const_reverse_iterator(end());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_reverse_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::crend() const
{
    return const_reverse_iterator(begin());
}

// capacity:
template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bool map<KEY, VALUE, COMPARATOR, ALLOCATOR>::empty() const
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size() const
{
    return d_tree.numNodes();
}


template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::max_size() const
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
const VALUE& map<KEY, VALUE, COMPARATOR, ALLOCATOR>::at(
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

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::find(const key_type& key) const
{
    return const_iterator(
       BloombergLP::bslalg::RbTreeUtil::find(d_tree, this->comparator(), key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::size_type
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::count(const key_type& key) const
{
    return (find(key) != end()) ? 1 : 0;
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::lower_bound(const key_type& key) const
{
    return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::upper_bound(const key_type& key) const
{
    return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
                                                            d_tree,
                                                            this->comparator(),
                                                            key));
}

template <class KEY, class VALUE, class COMPARATOR, class ALLOCATOR>
inline
bsl::pair<typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator,
          typename map<KEY, VALUE, COMPARATOR, ALLOCATOR>::const_iterator>
map<KEY, VALUE, COMPARATOR, ALLOCATOR>::equal_range(const key_type& key) const
{
    const_iterator startIt = lower_bound(key);
    const_iterator endIt   = startIt;
    if (endIt != end() && !comparator()(key, *endIt.node())) {
        ++endIt;
    }
    return bsl::pair<const_iterator, const_iterator>(startIt, endIt);
}

}  // close namespace bsl

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

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator!=(const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

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

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
inline
void bsl::swap(bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& a,
               bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR>& b)
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

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
struct HasStlIterators<bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY,  class VALUE,  class COMPARATOR,  class ALLOCATOR>
struct UsesBslmaAllocator<bsl::map<KEY, VALUE, COMPARATOR, ALLOCATOR> >
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
