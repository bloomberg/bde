// bslstl_multiset.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_MULTISET
#define INCLUDED_BSLSTL_MULTISET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant multiset class.
//
//@CLASSES:
//   bsl::multiset: STL-compatible multiset template
//
//@CANONICAL_HEADER: bsl_set.h
//
//@SEE_ALSO: bslstl_set, bslstl_multimap
//
//@DESCRIPTION: This component defines a single class template 'bsl::multiset',
// implementing the standard container holding an ordered sequence of possibly
// duplicate keys.
//
// An instantiation of 'multiset' is an allocator-aware, value-semantic type
// whose salient attributes are its size (number of keys) and the ordered
// sequence of keys the 'multiset' contains.  If 'multiset' is instantiated
// with a key type that is not itself value-semantic, then it will not retain
// all of its value-semantic qualities.  In particular, if the key type cannot
// be tested for equality, then a multiset containing that type cannot be
// tested for equality.  It is even possible to instantiate 'multiset' with a
// key type that does not have a copy-constructor, in which case the 'multiset'
// will not be copyable.
//
// A multiset meets the requirements of an associative container with
// bidirectional iterators in the C++ standard [23.2.4].  The 'multiset'
// implemented here adheres to the C++11 standard when compiled with a C++11
// compiler, and makes the best approximation when compiled with a C++03
// compiler.  In particular, for C++03 we emulate move semantics, but limit
// forwarding (in 'emplace') to 'const' lvalues, and make no effort to emulate
// 'noexcept' or initializer-lists.
//
///Requirements on 'KEY'
///---------------------
// A 'multiset' is a fully "Value-Semantic Type" (see {'bsldoc_glossary'}) only
// if the supplied 'KEY' template parameter is fully value-semantic.  It is
// possible to instantiate a 'multiset' with a 'KEY' parameter argument that
// does not provide a full set of value-semantic operations, but then some
// methods of the container may not be instantiable.  The following
// terminology, adopted from the C++11 standard, is used in the function
// documentation of 'multiset' to describe a function's requirements for the
// 'KEY' template parameter.  These terms are also defined in section
// [17.6.3.1] of the C++11 standard.  Note that, in the context of a 'multiset'
// instantiation, the requirements apply specifically to the multiset's entry
// type, 'value_type', which is an alias for 'KEY'.
//
//  Legend
//  ------
//  'X'    - denotes an allocator-aware container type (e.g., 'multiset')
//  'T'    - 'value_type' associated with 'X'
//  'A'    - type of the allocator used by 'X'
//  'm'    - lvalue of type 'A' (allocator)
//  'p',   - address ('T *') of uninitialized storage for a 'T' within an 'X'
//  'rv'   - rvalue of type (non-'const') 'T'
//  'v'    - rvalue or lvalue of type (possibly 'const') 'T'
//  'args' - 0 or more arguments
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
// The type supplied as a multiset's 'ALLOCATOR' template parameter determines
// how that multiset will allocate memory.  The 'multiset' template supports
// allocators meeting the requirements of the C++11 standard [17.6.3.5]; in
// addition, it supports scoped-allocators derived from the 'bslma::Allocator'
// memory allocation protocol.  Clients intending to use 'bslma'-style
// allocators should use the template's default 'ALLOCATOR' type: The default
// type for the 'ALLOCATOR' template parameter, 'bsl::allocator', provides a
// C++11 standard-compatible adapter for a 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the (template parameter) type 'ALLOCATOR' of a 'multiset' instantiation
// is 'bsl::allocator', then objects of that multiset type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a multiset
// accepts an optional 'bslma::Allocator' argument at construction.  If the
// address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it is used to supply memory for the multiset throughout its
// lifetime; otherwise, the multiset will use the default allocator installed
// at the time of the multiset's construction (see 'bslma_default').  In
// addition to directly allocating memory from the indicated
// 'bslma::Allocator', a multiset supplies that allocator's address to the
// constructors of contained objects of the (template parameter) type 'KEY'
// having the 'bslma::UsesBslmaAllocator' trait.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'multiset':
//..
//  Legend
//  ------
//  'K'             - (template parameter) type 'KEY' of the multiset
//  'a', 'b'        - two distinct objects of type 'multiset<K>'
//  'rv'            - modifiable rvalue of type 'multiset<K>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'c'             - comparator providing an ordering for objects of type 'K'
//  'al'            - STL-style memory allocator
//  'i1', 'i2'      - two iterators defining a sequence of 'value_type' objects
//  'li'            - object of type 'initializer_list<K>'
//  'k'             - object of type 'K'
//  'rk'            - modifiable rvalue of type 'K'
//  'p1', 'p2'      - two 'const_iterator's belonging to 'a'
//  distance(i1,i2) - number of elements in the range '[i1 .. i2)'
//  distance(p1,p2) - number of elements in the range '[p1 .. p2)'
//
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | multiset<K> a;               (default construction)| O[1]               |
//  | multiset<K> a(al);                                 |                    |
//  | multiset<K> a(c, al);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | multiset<K> a(b);            (copy construction)   | O[n]               |
//  | multiset<K> a(b, al);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | multiset<K> a(rv);           (move construction)   | O[1] if 'a' and    |
//  | multiset<K> a(rv, al);                             | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | multiset<K> a(i1, i2, al);   (range construction)  | O[N] if [i1, i2)   |
//  | multiset<K> a(i1, i2, c, al);                      | is sorted with     |
//  |                                                    | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where N |
//  |                                                    | is distance(i1,i2) |
//  +----------------------------------------------------+--------------------+
//  | multiset<K> a(li);                                 | O[N] if 'li' is    |
//  | multiset<K> a(li, al);                             | sorted with        |
//  | multiset<K> a(li, c);                              | 'a.value_comp()',  |
//  | multiset<K> a(li, c, al);                          | O[N * log(N)]      |
//  |                                                    | otherwise, where   |
//  |                                                    | N = 'li.size()'    |
//  +----------------------------------------------------+--------------------+
//  | a.~multiset<K>();            (destruction)         | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;                       (copy assignment)     | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = rv;                      (move assignment)     | O[1] if 'a' and    |
//  |                                                    | 'rv' use the same  |
//  |                                                    | allocator,         |
//  |                                                    | O[n] otherwise     |
//  +----------------------------------------------------+--------------------+
//  | a = li;                                            | O[N] if 'li' is    |
//  |                                                    | sorted with        |
//  |                                                    | 'a.value_comp()',  |
//  |                                                    | O[N * log(N)]      |
//  |                                                    | otherwise, where   |
//  |                                                    | N = 'li.size()'    |
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
//  | a.insert(k)                                        | O[log(n)]          |
//  | a.insert(rk)                                       |                    |
//  | a.emplace(Args&&...)                               |                    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(p1, k)                                    | amortized constant |
//  | a.insert(p1, rk)                                   | if the value is    |
//  | a.emplace_hint(p1, Args&&...)                      | inserted right     |
//  |                                                    | before p1,         |
//  |                                                    | O[log(n)]          |
//  |                                                    | otherwise          |
//  +----------------------------------------------------+--------------------+
//  | a.insert(i1, i2)                                   | O[N * log(n + N)]  |
//  |                                                    | where N is         |
//  |                                                    | distance(i1,i2)    |
//  +----------------------------------------------------+--------------------+
//  | a.insert(li)                                       | O[N * log(n + N)]  |
//  |                                                    | where N =          |
//  |                                                    |         'li.size()'|
//  +----------------------------------------------------+--------------------+
//  | a.erase(p1)                                        | amortized constant |
//  +----------------------------------------------------+--------------------+
//  | a.erase(k)                                         | O[log(n) +         |
//  |                                                    | a.count(k)]        |
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
//  | a.contains(k)                                      | O[log(n)]          |
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
///Example 1: Creating a Shopping Cart
///- - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsl::multiset' to define a class
// 'ShoppingCart', that characterizes a simple online shopping cart with the
// ability to add, remove, and view items in the shopping cart.
//
// Note that this example uses a type 'string' that is based on the standard
// type 'string' (see 'bslstl_string').  For the sake of brevity, the
// implementation of 'string' is not explored here.
//
// First, we define a comparison functor for 'string' objects:
//..
//  struct StringComparator {
//      // This 'struct' defines an ordering on 'string' values, allowing
//      // them to be included in sorted containers such as 'bsl::multiset'.
//
//      bool operator()(const string& lhs, const string& rhs) const
//          // Return 'true' if the value of the specified 'lhs' is less than
//          // (ordered before) the value of the specified 'rhs', and 'false'
//          // otherwise.
//      {
//          int cmp = std::strcmp(lhs.c_str(), rhs.c_str());
//          return cmp < 0;
//      }
//  };
//..
// Then, we define the public interface for 'ShoppingCart':
//..
//  class ShoppingCart {
//      // This class provides an ordered collection of (possibly duplicate)
//      // items in a shopping cart.  For simplicity of the usage example, each
//      // item in the shopping cart is represented by a 'string'.
//..
// Here, we create a type alias, 'StringSet', for a 'bsl::multiset' that will
// serve as the data member for a 'ShoppingCart'.  A 'StringSet' has keys of
// type 'string', and uses the default 'ALLOCATOR' template parameter to be
// compatible with 'bslma' style allocators:
//..
//      // PRIVATE TYPES
//      typedef bsl::multiset<string, StringComparator> StringSet;
//          // This 'typedef' is an alias for a multiset of 'string' objects,
//          // each representing an item in a shopping cart;
//
//      // DATA
//      StringSet d_items;  // multiset of items in the shopping cart
//
//      // FRIENDS
//      friend bool operator==(const ShoppingCart& lhs,
//                             const ShoppingCart& rhs);
//
//    public:
//      // PUBLIC TYPES
//      typedef StringSet::const_iterator ConstIterator;
//          // This 'typedef' provides an alias for the type of an iterator
//          // providing non-modifiable access to the items in a
//          // 'ShoppingCart'.
//
//      // CREATORS
//      ShoppingCart(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'Shopping' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      ShoppingCart(const ShoppingCart&  original,
//                   bslma::Allocator    *basicAllocator = 0);
//          // Create a 'ShoppingCart' object having the same value as the
//          // specified 'original' object.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      //! ~ShoppingCart() = default;
//          // Destroy this object.
//
//      // MANIPULATORS
//      ShoppingCart& operator=(const ShoppingCart& rhs);
//          // Assign to this object the value of the specified 'rhs' object,
//          // and return a reference providing modifiable access to this
//          // object.
//
//      void addItem(const string& name);
//          // Add an item with the specified 'name' to this shopping cart.
//          // The behavior is undefined unless 'name' is a non-empty strings.
//
//      size_t removeItems(const string& name);
//          // Remove from this shopping cart all items having the specified
//          // 'name', if they exist, and return the number of removed items;
//          // otherwise, return 0 with no other effects.  The behavior is
//          // undefined unless 'name' is a non-empty strings.
//
//      // ACCESSORS
//      size_t count(const string& name) const;
//          // Return the number of items in the shopping cart with the
//          // specified 'name'.  The behavior is undefined unless 'name' is a
//          // non-empty strings.
//
//      ConstIterator begin() const;
//          // Return an iterator providing non-modifiable access to the first
//          // item in the ordered sequence of item held in this shopping cart,
//          // or the past-the-end iterator if this shopping cart is empty.
//
//      ConstIterator end() const;
//          // Return an iterator providing non-modifiable access to the
//          // past-the-end item in the ordered sequence of items maintained by
//          // this shopping cart.
//
//      size_t numItems() const;
//          // Return the number of items contained in this shopping cart.
//  };
//..
// Then, we declare the free operators for 'ShoppingCart':
//..
//  inline
//  bool operator==(const ShoppingCart& lhs, const ShoppingCart& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
//      // value, and 'false' otherwise.  Two 'ShoppingCart' objects have the
//      // same value if they have the same number of items, and each
//      // corresponding item, in their respective ordered sequence of items,
//      // is the same.
//
//  inline
//  bool operator!=(const ShoppingCart& lhs, const ShoppingCart& rhs);
//      // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
//      // the same value, and 'false' otherwise.  Two 'ShoppingCart' objects
//      // do not have the same value if they either differ in their number of
//      // contained items, or if any of the corresponding items, in their
//      // respective ordered sequences of items, is not the same.
//..
// Now, we define the implementations methods of the 'ShoppingCart' class:
//..
//  // CREATORS
//  inline
//  ShoppingCart::ShoppingCart(bslma::Allocator *basicAllocator)
//  : d_items(basicAllocator)
//  {
//  }
//..
// Notice that, on construction, we pass the contained 'bsl::multiset' object
// the allocator supplied to 'ShoppingCart' at construction'.
//..
//  inline
//  ShoppingCart::ShoppingCart(const ShoppingCart&   original,
//                             bslma::Allocator     *basicAllocator)
//  : d_items(original.d_items, basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  ShoppingCart& ShoppingCart::operator=(const ShoppingCart& rhs)
//  {
//      d_items = rhs.d_items;
//      return *this;
//  }
//
//  inline
//  void ShoppingCart::addItem(const string& name)
//  {
//      BSLS_ASSERT(!name.empty());
//
//      d_items.insert(name);
//  }
//
//  inline
//  size_t ShoppingCart::removeItems(const string& name)
//  {
//      BSLS_ASSERT(!name.empty());
//
//      return d_items.erase(name);
//  }
//
//  // ACCESSORS
//  size_t ShoppingCart::count(const string& name) const
//  {
//      BSLS_ASSERT(!name.empty());
//
//      return d_items.count(name);
//  }
//
//  ShoppingCart::ConstIterator ShoppingCart::begin() const
//  {
//      return d_items.begin();
//  }
//
//  ShoppingCart::ConstIterator ShoppingCart::end() const
//  {
//      return d_items.end();
//  }
//
//  size_t ShoppingCart::numItems() const
//  {
//      return d_items.size();
//  }
//..
// Finally, we implement the free operators for 'ShoppingCart':
//..
//  inline
//  bool operator==(const ShoppingCart& lhs, const ShoppingCart& rhs)
//  {
//      return lhs.d_items == rhs.d_items;
//  }
//
//  inline
//  bool operator!=(const ShoppingCart& lhs, const ShoppingCart& rhs)
//  {
//      return !(lhs == rhs);
//  }
//..

#include <bslscm_version.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_iteratorutil.h>
#include <bslstl_pair.h>
#include <bslstl_setcomparator.h>
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

#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowswappable.h>
#include <bslmf_istransparentpredicate.h>
#include <bslmf_movableref.h>
#include <bslmf_typeidentity.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <functional>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>  // 'std::is_nothrow_move_assignable'
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslstl_multiset.h
# define COMPILING_BSLSTL_MULTISET_H
# include <bslstl_multiset_cpp03.h>
# undef COMPILING_BSLSTL_MULTISET_H
#else

namespace bsl {

                             // ==============
                             // class multiset
                             // ==============

template <class KEY,
          class COMPARATOR = std::less<KEY>,
          class ALLOCATOR  = bsl::allocator<KEY> >
class multiset {
    // This class template implements a value-semantic container type holding
    // an ordered sequence of possibly duplicate keys (of the template
    // parameter type, 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for BDEX serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    // PRIVATE TYPES
    typedef const KEY                                           ValueType;
        // This typedef is an alias for the type of key objects maintained by
        // this multiset.

    typedef BloombergLP::bslstl::SetComparator<KEY, COMPARATOR> Comparator;
        // This typedef is an alias for the comparator used internally by this
        // multiset.

    typedef BloombergLP::bslstl::TreeNode<KEY>                 Node;
        // This typedef is an alias for the type of nodes held by the tree (of
        // nodes) used to implement this multiset.

    typedef BloombergLP::bslstl::TreeNodePool<KEY, ALLOCATOR>  NodeFactory;
        // This typedef is an alias for the factory type used to create and
        // destroy 'Node' objects.

    typedef bsl::allocator_traits<ALLOCATOR>                   AllocatorTraits;
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
        explicit DataWrapper(const COMPARATOR& comparator,
                             const ALLOCATOR&  basicAllocator);
            // Create a data wrapper using a copy of the specified 'comparator'
            // to order keys and a copy of the specified 'basicAllocator' to
            // supply memory.

        DataWrapper(BloombergLP::bslmf::MovableRef<DataWrapper> original);
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
    typedef KEY                                        value_type;
    typedef COMPARATOR                                 key_compare;
    typedef COMPARATOR                                 value_compare;
    typedef ALLOCATOR                                  allocator_type;
    typedef value_type&                                reference;
    typedef const value_type&                          const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

    typedef BloombergLP::bslstl::TreeIterator<const value_type,
                                              Node,
                                              difference_type> iterator;
    typedef BloombergLP::bslstl::TreeIterator<const value_type,
                                              Node,
                                              difference_type> const_iterator;
    typedef bsl::reverse_iterator<iterator>            reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator>      const_reverse_iterator;

  private:
    // PRIVATE MANIPULATORS
    Comparator& comparator();
        // Return a reference providing modifiable access to the comparator for
        // this multiset.

    NodeFactory& nodeFactory();
        // Return a reference providing modifiable access to the node-allocator
        // for this multiset.

    void quickSwapExchangeAllocators(multiset& other);
        // Efficiently exchange the value, comparator, and allocator of this
        // object with the value, comparator, and allocator of the specified
        // 'other' object.  This method provides the no-throw exception-safety
        // guarantee, *unless* swapping the (user-supplied) comparator or
        // allocator objects can throw.

    void quickSwapRetainAllocators(multiset& other);
        // Efficiently exchange the value and comparator of this object with
        // the value and comparator of the specified 'other' object.  This
        // method provides the no-throw exception-safety guarantee, *unless*
        // swapping the (user-supplied) comparator objects can throw.  The
        // behavior is undefined unless this object was created with the same
        // allocator as 'other'.

    // PRIVATE ACCESSORS
    const Comparator& comparator() const;
        // Return a reference providing non-modifiable access to the comparator
        // for this multiset.

    const NodeFactory& nodeFactory() const;
        // Return a reference providing non-modifiable access to the
        // node-allocator for this multiset.

  public:
    // CREATORS
    multiset();
    explicit multiset(const COMPARATOR& comparator,
                      const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Create an empty multiset.  Optionally specify a 'comparator' used to
        // order keys contained in this object.  If 'comparator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'COMPARATOR' is used.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is 'bsl::allocator'
        // (the default), then 'basicAllocator', if supplied, shall be
        // convertible to 'bslma::Allocator *'.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.
    : d_compAndAlloc(comparator, basicAllocator)
    , d_tree()
    {
        // The implementation is placed here in the class definition to work
        // around an AIX compiler bug, where the constructor can fail to
        // compile because it is unable to find the definition of the default
        // argument.  This occurs when a templatized class wraps around the
        // container and the comparator is defined after the new class.
    }

    explicit multiset(const ALLOCATOR& basicAllocator);
        // Create an empty multiset that uses the specified 'basicAllocator' to
        // supply memory.  Use a default-constructed object of the (template
        // parameter) type 'COMPARATOR' to order the keys contained in this
        // multiset.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    multiset(const multiset& original);
        // Create a multiset having the same value as the specified 'original'
        // object.  Use a copy of 'original.key_comp()' to order the keys
        // contained in this multiset.  Use the allocator returned by
        // 'bsl::allocator_traits<ALLOCATOR>::
        // select_on_container_copy_construction(original.get_allocator())' to
        // allocate memory.  This method requires that the (template parameter)
        // type 'KEY' be 'copy-insertable' into this multiset (see
        // {Requirements on 'KEY'}).

    multiset(BloombergLP::bslmf::MovableRef<multiset> original);    // IMPLICIT
        // Create a multiset having the same value as that of the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new multiset.  Use a copy of 'original.key_comp()'
        // to order the keys contained in this multiset.  The allocator
        // associated with 'original' is propagated for use in the
        // newly-created multiset.  'original' is left in a valid but
        // unspecified state.

    multiset(const multiset&                                original,
             const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a multiset having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Use a copy of 'original.key_comp()' to order the keys contained in
        // this multiset.  This method requires that the (template parameter)
        // type 'KEY' be 'copy-insertable' into this multiset (see
        // {Requirements on 'KEY'}).  Note that a 'bslma::Allocator *' can be
        // supplied for 'basicAllocator' if the (template parameter) type
        // 'ALLOCATOR' is 'bsl::allocator' (the default).

    multiset(BloombergLP::bslmf::MovableRef<multiset>       original,
             const typename type_identity<ALLOCATOR>::type& basicAllocator);
        // Create a multiset having the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // The contents of 'original' are moved (in constant time) to the new
        // multiset if 'basicAllocator == original.get_allocator()', and are
        // move-inserted (in linear time) using 'basicAllocator' otherwise.
        // 'original' is left in a valid but unspecified state.  Use a copy of
        // 'original.key_comp()' to order the keys contained in this multiset.
        // This method requires that the (template parameter) type 'KEY' be
        // 'move-insertable' into this multiset (see {Requirements on 'KEY'}).
        // Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the (template parameter) type 'ALLOCATOR' is
        // 'bsl::allocator' (the default).

    template <class INPUT_ITERATOR>
    multiset(INPUT_ITERATOR    first,
             INPUT_ITERATOR    last,
             const COMPARATOR& comparator     = COMPARATOR(),
             const ALLOCATOR&  basicAllocator = ALLOCATOR());
    template <class INPUT_ITERATOR>
    multiset(INPUT_ITERATOR    first,
             INPUT_ITERATOR    last,
             const ALLOCATOR&  basicAllocator);
        // Create a multiset, and insert each 'value_type' object in the
        // sequence starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // a 'comparator' used to order keys contained in this object.  If
        // 'comparator' is not supplied, a default-constructed object of the
        // (template parameter) type 'COMPARATOR' is used.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  If the sequence 'first' to
        // 'last' is ordered according to 'comparator', then this operation has
        // 'O[N]' complexity, where 'N' is the number of elements between
        // 'first' and 'last', otherwise this operation has 'O[N * log(N)]'
        // complexity.  The (template parameter) type 'INPUT_ITERATOR' shall
        // meet the requirements of an input iterator defined in the C++11
        // standard [24.2.3] providing access to values of a type convertible
        // to 'value_type', and 'value_type' must be 'emplace-constructible'
        // from '*i' into this multiset, where 'i' is a dereferenceable
        // iterator in the range '[first .. last)' (see {Requirements on
        // 'KEY'}).  The behavior is undefined unless 'first' and 'last' refer
        // to a sequence of valid values where 'first' is at a position at or
        // before 'last'.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    multiset(std::initializer_list<KEY> values,
             const COMPARATOR&          comparator     = COMPARATOR(),
             const ALLOCATOR&           basicAllocator = ALLOCATOR());
    multiset(std::initializer_list<KEY> values,
             const ALLOCATOR&           basicAllocator);
        // Create a multiset and insert each 'value_type' object in the
        // specified 'values' initializer list.  Optionally specify a
        // 'comparator' used to order keys contained in this object.  If
        // 'comparator' is not supplied, a default-constructed object of the
        // (template parameter) type 'COMPARATOR' is used.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not supplied, a default-constructed object of the (template
        // parameter) type 'ALLOCATOR' is used.  If the type 'ALLOCATOR' is
        // 'bsl::allocator' and 'basicAllocator' is not supplied, the currently
        // installed default allocator is used.  If 'values' is ordered
        // according to 'comparator', then this operation has 'O[N]'
        // complexity, where 'N' is the number of elements in 'values';
        // otherwise this operation has 'O[N * log(N)]' complexity.  This
        // method requires that the (template parameter) type 'KEY' be
        // 'copy-insertable' into this multiset (see {Requirements on 'KEY'}).
        // Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the type 'ALLOCATOR' is 'bsl::allocator' (the
        // default).
#endif

    ~multiset();
        // Destroy this object.

    // MANIPULATORS
    multiset& operator=(const multiset& rhs);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_copy_assignment',
        // and return a reference providing modifiable access to this object.
        // If an exception is thrown, '*this' is left in a valid but
        // unspecified state.  This method requires that the (template
        // parameter) type 'KEY' be 'copy-assignable' and 'copy-insertable'
        // into this multiset (see {Requirements on 'KEY'}).

    multiset& operator=(BloombergLP::bslmf::MovableRef<multiset> rhs)
                       BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                           AllocatorTraits::is_always_equal::value
                        && std::is_nothrow_move_assignable<COMPARATOR>::value);
        // Assign to this object the value and comparator of the specified
        // 'rhs' object, propagate to this object the allocator of 'rhs' if the
        // 'ALLOCATOR' type has trait 'propagate_on_container_move_assignment',
        // and return a reference providing modifiable access to this object.
        // The contents of 'rhs' are moved (in constant time) to this multiset
        // if 'get_allocator() == rhs.get_allocator()' (after accounting for
        // the aforementioned trait); otherwise, all elements in this multiset
        // are either destroyed or move-assigned to and each additional element
        // in 'rhs' is move-inserted into this multiset.  'rhs' is left in a
        // valid but unspecified state, and if an exception is thrown, '*this'
        // is left in a valid but unspecified state.  This method requires that
        // the (template parameter) type 'KEY' be 'move-assignable' and
        // 'move-insertable' into this multiset (see {Requirements on 'KEY'}).

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    multiset& operator=(std::initializer_list<KEY> values);
        // Assign to this object the value resulting from first clearing this
        // multiset and then inserting each 'value_type' object in the
        // specified 'values' initializer list and return a reference providing
        // modifiable access to this object.  This method requires that the
        // (template parameter) type 'KEY' be 'copy-insertable' into this
        // multiset (see {Requirements on 'KEY'}).
#endif

    iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multiset, or the 'end' iterator if this multiset
        // is empty.

    iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // element in the ordered sequence of 'value_type' objects maintained
        // by this multiset.

    reverse_iterator rbegin() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the last
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multiset, or 'rend' if this multiset is empty.

    reverse_iterator rend() BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multiset.

    iterator insert(const value_type& value);
        // Insert the specified 'value' into this multiset.  If a range
        // containing elements equivalent to 'value' already exists, insert the
        // 'value' at the end of that range.  Return an iterator referring to
        // the newly inserted 'value_type' object.  This method requires that
        // the (template parameter) type 'KEY' be 'copy-insertable' into this
        // multiset (see {Requirements on 'KEY'}).

    iterator insert(BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this multiset.  If a range
        // containing elements equivalent to 'value' already exists in this
        // multiset, insert 'value' at the end of that range.  'value' is left
        // in a valid but unspecified state.  Return an iterator referring to
        // the newly inserted 'value_type' object in this multiset that is
        // equivalent to 'value'.  This method requires that the (template
        // parameter) type 'KEY' be 'move-insertable' into this multiset (see
        // {Requirements on 'KEY'}).

    iterator insert(const_iterator hint, const value_type& value);
        // Insert the specified 'value' into this multiset (in amortized
        // constant time if the specified 'hint' is a valid immediate successor
        // to 'value').  Return an iterator referring to the newly inserted
        // 'value_type' object in this multiset that is equivalent to 'value'.
        // If 'hint' is not a valid immediate successor to 'value', this
        // operation has 'O[log(N)]' complexity, where 'N' is the size of this
        // multiset.  This method requires that the (template parameter) type
        // 'KEY' be 'copy-insertable' into this multiset (see {Requirements on
        // 'KEY'}).  The behavior is undefined unless 'hint' is an iterator in
        // the range '[begin() .. end()]' (both endpoints included).

    iterator insert(const_iterator                             hint,
                    BloombergLP::bslmf::MovableRef<value_type> value);
        // Insert the specified 'value' into this multiset (in amortized
        // constant time if the specified 'hint' is a valid immediate successor
        // to 'value').  'value' is left in a valid but unspecified state.
        // Return an iterator referring to the newly inserted 'value_type'
        // object in this multiset that is equivalent to 'value'.  If 'hint' is
        // not a valid immediate successor to 'value', this operation has
        // 'O[log(N)]' complexity, where 'N' is the size of this multiset.
        // This method requires that the (template parameter) type 'KEY' be
        // 'move-insertable' into this multiset (see {Requirements on 'KEY'}).
        // The behavior is undefined unless 'hint' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this multiset the value of each 'value_type' object in
        // the range starting at the specified 'first' iterator and ending
        // immediately before the specified 'last' iterator.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type', and
        // 'value_type' must be 'emplace-constructible' from '*i' into this
        // multiset, where 'i' is a dereferenceable iterator in the range
        // '[first .. last)' (see {Requirements on 'KEY'}).  The behavior is
        // undefined unless 'first' and 'last' refer to a sequence of valid
        // values where 'first' is at a position at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<KEY> values);
        // Insert into this multiset the value of each 'value_type' object in
        // the specified 'values' initializer list.  This method requires that
        // the (template parameter) type 'KEY' be 'copy-insertable' into this
        // multiset (see {Requirements on 'KEY'}).
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args>
    iterator emplace(Args&&... args);
        // Insert into this multiset a newly-created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type'.  Return an iterator referring to the
        // newly created and inserted object in this multiset.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'emplace-constructible' from 'args' (see {Requirements on 'KEY'}).

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);
        // Insert into this multiset a newly-created 'value_type' object,
        // constructed by forwarding 'get_allocator()' (if required) and the
        // specified (variable number of) 'args' to the corresponding
        // constructor of 'value_type' (in amortized constant time if the
        // specified 'hint' is a valid immediate successor to the 'value_type'
        // object constructed from 'args').  Return an iterator referring to
        // the newly created and inserted object in this multiset.  If 'hint'
        // is not a valid immediate successor to the 'value_type' object
        // implied by 'args', this operation has 'O[log(N)]' complexity where
        // 'N' is the size of this multiset.  This method requires that the
        // (template parameter) type 'KEY' be 'emplace-constructible' from
        // 'args' (see {Requirements on 'KEY'}).  The behavior is undefined
        // unless 'hint' is an iterator in the range '[begin() .. end()]' (both
        // endpoints included).

#endif

    iterator erase(const_iterator position);
        // Remove from this multiset the 'value_type' object at the specified
        // 'position', and return an iterator referring to the element
        // immediately following the removed element, or to the past-the-end
        // position if the removed element was the last element in the sequence
        // of elements maintained by this multiset.   This method invalidates
        // only iterators and references to the removed element and previously
        // saved values of the 'end()' iterator.  The behavior is undefined
        // unless 'position' refers to a 'value_type' object in this multiset.

    size_type erase(const key_type& key);
        // Remove from this multiset all 'value_type' objects equivalent to the
        // specified 'key', if they exist, and return the number of erased
        // objects; otherwise, if there are no 'value_type' objects equivalent
        // to 'key', return 0 with no other effect.   This method invalidates
        // only iterators and references to the removed element and previously
        // saved values of the 'end()' iterator.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this multiset the 'value_type' objects starting at the
        // specified 'first' position up to, but not including the specified
        // 'last' position, and return 'last'.   This method invalidates only
        // iterators and references to the removed element and previously saved
        // values of the 'end()' iterator.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this multiset or are
        // the 'end' iterator, and the 'first' position is at or before the
        // 'last' position in the ordered sequence provided by this container.

    void swap(multiset& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                 AllocatorTraits::is_always_equal::value
                              && bsl::is_nothrow_swappable<COMPARATOR>::value);
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
        // Remove all entries from this multiset.  Note that the multiset is
        // empty after this call, but allocated memory may be retained for
        // future use.

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    iterator find(const key_type& key)
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in this multiset equivalent to the specified
        // 'key', if such an object exists, and the past-the-end ('end')
        // iterator otherwise.
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
        // 'value_type' object in this multiset equivalent to the specified
        // 'key', if such an object exists, and the past-the-end ('end')
        // iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    iterator lower_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multiset greater-than or
        // equal-to the specified 'key', and the past-the-end iterator if this
        // multiset does not contain a 'value_type' object greater-than or
        // equal-to 'key'.  Note that this function returns the *first*
        // position before which a 'value_type' object equivalent to 'key'
        // could be inserted into the ordered sequence maintained by this
        // multiset, while preserving its ordering.
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
        // ordered least) 'value_type' object in this multiset greater-than or
        // equal-to the specified 'key', and the past-the-end iterator if this
        // multiset does not contain a 'value_type' object greater-than or
        // equal-to 'key'.  Note that this function returns the *first*
        // position before which a 'value_type' object equivalent to 'key'
        // could be inserted into the ordered sequence maintained by this
        // multiset, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    iterator upper_bound(const key_type& key)
        // Return an iterator providing modifiable access to the first (i.e.,
        // ordered least) 'value_type' object in this multiset greater than the
        // specified 'key', and the past-the-end iterator if this multiset does
        // not contain a 'value_type' object greater-than 'key'.  Note that
        // this function returns the *last* position before which a
        // 'value_type' object equivalent to 'key' could be inserted into the
        // ordered sequence maintained by this multiset, while preserving its
        // ordering.
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
        // ordered least) 'value_type' object in this multiset greater than the
        // specified 'key', and the past-the-end iterator if this multiset does
        // not contain a 'value_type' object greater-than 'key'.  Note that
        // this function returns the *last* position before which a
        // 'value_type' object equivalent to 'key' could be inserted into the
        // ordered sequence maintained by this multiset, while preserving its
        // ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<iterator, iterator> equal_range(const key_type& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multiset equivalent to the
        // specified 'key', where the first iterator is positioned at the start
        // of the sequence and the second is positioned one past the end of the
        // sequence.  The first returned iterator will be 'lower_bound(key)',
        // the second returned iterator will be 'upper_bound(key)', and, if
        // this multiset contains no 'value_type' objects with an equivalent
        // key, then the two returned iterators will have the same value.
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

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<iterator, iterator> >::type
    equal_range(const LOOKUP_KEY& key)
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multiset equivalent to the
        // specified 'key', where the first iterator is positioned at the start
        // of the sequence and the second is positioned one past the end of the
        // sequence.  The first returned iterator will be 'lower_bound(key)',
        // the second returned iterator will be 'upper_bound(key)', and, if
        // this multiset contains no 'value_type' objects with an equivalent
        // key, then the two returned iterators will have the same value.
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
        // multiset.

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multiset, or the 'end' iterator if this multiset
        // is empty.

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this multiset.

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this multiset, or 'rend' if this multiset is
        // empty.

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multiset.

    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object in the ordered sequence of 'value_type' objects
        // maintained by this multiset, or the 'end' iterator if this multiset
        // is empty.

    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the ordered sequence of 'value_type' objects
        // maintained by this multiset.

    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // last 'value_type' object in the ordered sequence of 'value_type'
        // objects maintained by this multiset, or 'rend' if this multiset is
        // empty.

    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reverse iterator providing non-modifiable access to the
        // prior-to-the-beginning element in the ordered sequence of
        // 'value_type' objects maintained by this multiset.

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
        // Return 'true' if this multiset contains no elements, and 'false'
        // otherwise.

    size_type size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this multiset.

    size_type max_size() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this multiset could possibly hold.  Note that there is no
        // guarantee that the multiset can successfully grow to the returned
        // size, or even close to that size without running out of resources.

    key_compare key_comp() const;
        // Return the key-comparison functor (or function pointer) used by this
        // multiset; if a comparator was supplied at construction, return its
        // value, otherwise return a default constructed 'key_compare' object.
        // Note that this comparator compares objects of type 'KEY', which is
        // the type of the 'value_type' objects contained in this multiset.

    value_compare value_comp() const;
        // Return a functor for comparing two 'value_type' objects using
        // 'key_comp()'.  Note that since 'value_type' is an alias to 'KEY' for
        // 'multiset', this method returns the same functor as 'key_comp()'.

    // Turn off complaints about necessarily class-defined methods.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -CD01

    const_iterator find(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object that is equivalent to the specified 'key' in
        // ordered sequence maintained by this multiset, if such an object
        // exists, and the past-the-end ('end') iterator otherwise.
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
        // 'value_type' object that is equivalent to the specified 'key' in
        // ordered sequence maintained by this multiset, if such an object
        // exists, and the past-the-end ('end') iterator otherwise.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::find(
            d_tree, this->comparator(), key));
    }

    size_type count(const key_type& key) const
        // Return the number of 'value_type' objects within this multiset that
        // are equivalent to the specified 'key'.
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
        // Return the number of 'value_type' objects within this multiset that
        // are equivalent to the specified 'key'.
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
        // (i.e., ordered least) 'value_type' object in this multiset
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this multiset does not contain a 'value_type'
        // greater-than or equal-to 'key'.  Note that this function returns the
        // *first* position before which a 'value_type' object equivalent to
        // 'key' could be inserted into the ordered sequence maintained by this
        // multiset, while preserving its ordering.
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
        // (i.e., ordered least) 'value_type' object in this multiset
        // greater-than or equal-to the specified 'key', and the past-the-end
        // iterator if this multiset does not contain a 'value_type'
        // greater-than or equal-to 'key'.  Note that this function returns the
        // *first* position before which a 'value_type' object equivalent to
        // 'key' could be inserted into the ordered sequence maintained by this
        // multiset, while preserving its ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::lowerBound(
            d_tree, this->comparator(), key));
    }

    const_iterator upper_bound(const key_type& key) const
        // Return an iterator providing non-modifiable access to the first
        // (i.e., ordered least) 'value_type' object in this multiset greater
        // than the specified 'key', and the past-the-end iterator if this
        // multiset does not contain a 'value_type' object greater-than 'key'.
        // Note that this function returns the *last* position before which a
        // 'value_type' object equivalent to 'key' could be inserted into the
        // ordered sequence maintained by this multiset, while preserving its
        // ordering.
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
        // (i.e., ordered least) 'value_type' object in this multiset greater
        // than the specified 'key', and the past-the-end iterator if this
        // multiset does not contain a 'value_type' object greater-than 'key'.
        // Note that this function returns the *last* position before which a
        // 'value_type' object equivalent to 'key' could be inserted into the
        // ordered sequence maintained by this multiset, while preserving its
        // ordering.
        //
        // Note: implemented inline due to Sun CC compilation error.
    {
        return const_iterator(BloombergLP::bslalg::RbTreeUtil::upperBound(
            d_tree, this->comparator(), key));
    }

    pair<const_iterator, const_iterator> equal_range(const key_type& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this multiset that are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  The first returned
        // iterator will be 'lower_bound(key)'; the second returned iterator
        // will be 'upper_bound(key)'; and, if this multiset contains no
        // 'value_type' objects equivalent to 'key', then the two returned
        // iterators will have the same value.
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

    template <class LOOKUP_KEY>
    typename bsl::enable_if<
        BloombergLP::bslmf::IsTransparentPredicate<COMPARATOR,
                                                   LOOKUP_KEY>::value,
        pair<const_iterator, const_iterator> >::type
    equal_range(const LOOKUP_KEY& key) const
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this multiset that are
        // equivalent to the specified 'key', where the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  The first returned
        // iterator will be 'lower_bound(key)'; the second returned iterator
        // will be 'upper_bound(key)'; and, if this multiset contains no
        // 'value_type' objects equivalent to 'key', then the two returned
        // iterators will have the same value.
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
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multiset(INPUT_ITERATOR,
         INPUT_ITERATOR,
         COMPARATOR = COMPARATOR(),
         ALLOCATOR = ALLOCATOR())
-> multiset<KEY, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'multiset'.  Deduce the
    // template parameters 'COMPARATOR' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.  This guide does not participate
    // unless the supplied (or defaulted) 'ALLOCATOR' meets the requirements of
    // a standard allocator.

template <
    class INPUT_ITERATOR,
    class COMPARATOR,
    class ALLOC,
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multiset(INPUT_ITERATOR, INPUT_ITERATOR, COMPARATOR, ALLOC *)
-> multiset<KEY, COMPARATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'multiset'.  Deduce the
    // template parameter 'COMPARATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // specified 'ALLOC' is convertible to 'bsl::allocator<KEY>'.

template <
    class INPUT_ITERATOR,
    class ALLOCATOR,
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multiset(INPUT_ITERATOR, INPUT_ITERATOR, ALLOCATOR)
-> multiset<KEY, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'multiset'.  Deduce the
    // template parameter 'ALLOCATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // supplied allocator meets the requirements of a standard allocator.

template <
    class INPUT_ITERATOR,
    class ALLOC,
    class KEY =
         typename BloombergLP::bslstl::IteratorUtil::IterVal_t<INPUT_ITERATOR>,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multiset(INPUT_ITERATOR, INPUT_ITERATOR, ALLOC *)
-> multiset<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // iterators supplied to the constructor of 'multiset'.  This deduction
    // guide does not participate unless the specified 'ALLOC' is convertible
    // to 'bsl::allocator<KEY>'.

template <
    class KEY,
    class COMPARATOR = std::less<KEY>,
    class ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<!bsl::IsStdAllocator_v<COMPARATOR>>,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multiset(std::initializer_list<KEY>,
         COMPARATOR = COMPARATOR(),
         ALLOCATOR = ALLOCATOR())
-> multiset<KEY, COMPARATOR, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'multiset'.  Deduce the
    // template parameters 'COMPARATOR' and 'ALLOCATOR' from the other
    // parameters passed to the constructor.

template <
    class KEY,
    class COMPARATOR,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multiset(std::initializer_list<KEY>, COMPARATOR, ALLOC *)
-> multiset<KEY, COMPARATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'multiset'.  Deduce the
    // template parameter 'COMPARATOR' from the other parameter passed to the
    // constructor.  This deduction guide does not participate unless the
    // specified 'ALLOC' is convertible to 'bsl::allocator<KEY>'.

template <
    class KEY,
    class ALLOCATOR,
    class = bsl::enable_if_t<bsl::IsStdAllocator_v<ALLOCATOR>>
    >
multiset(std::initializer_list<KEY>, ALLOCATOR)
-> multiset<KEY, std::less<KEY>, ALLOCATOR>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'multiset'.  Deduce the
    // template parameter 'ALLOCATOR' from the other parameter passed to the
    // constructor.

template <
    class KEY,
    class ALLOC,
    class DEFAULT_ALLOCATOR = bsl::allocator<KEY>,
    class = bsl::enable_if_t<bsl::is_convertible_v<ALLOC *, DEFAULT_ALLOCATOR>>
    >
multiset(std::initializer_list<KEY>, ALLOC *)
-> multiset<KEY>;
    // Deduce the template parameter 'KEY' from the 'value_type' of the
    // initializer_list supplied to the constructor of 'multiset'.  This
    // deduction guide does not participate unless the specified 'ALLOC' is
    // convertible to 'bsl::allocator<KEY>'.

#endif

// FREE OPERATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator==(const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'multiset' objects 'lhs' and 'rhs'
    // have the same value if they have the same number of keys, and each
    // element in the ordered sequence of keys of 'lhs' has the same value as
    // the corresponding element in the ordered sequence of keys of 'rhs'.
    // This method requires that the (template parameter) type 'KEY' be
    // 'equality-comparable' (see {Requirements on 'KEY'}).

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator!=(const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'multiset' objects 'lhs' and
    // 'rhs' do not have the same value if they do not have the same number of
    // keys, or some element in the ordered sequence of keys of 'lhs' does not
    // have the same value as the corresponding element in the ordered sequence
    // of keys of 'rhs'.  This method requires that the (template parameter)
    // type 'KEY' be 'equality-comparable' (see {Requirements on 'KEY'}).
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class COMPARATOR, class ALLOCATOR>
BloombergLP::bslalg::SynthThreeWayUtil::Result<KEY>
operator<=>(const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
            const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Perform a lexicographic three-way comparison of the specified 'lhs' and
    // the specified 'rhs' multisets by using the comparison operators of 'KEY'
    // on each element; return the result of that comparison.

#else

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator< (const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multiset is
    // lexicographically less than that of the specified 'rhs' multiset, and
    // 'false' otherwise.  Given iterators 'i' and 'j' over the respective
    // sequences '[lhs.begin() .. lhs.end())' and '[rhs.begin() .. rhs.end())',
    // the value of multiset 'lhs' is lexicographically less than that of
    // multiset 'rhs' if 'true == *i < *j' for the first pair of corresponding
    // iterator positions where '*i < *j' and '*j < *i' are not both 'false'.
    // If no such corresponding iterator position exists, the value of 'lhs' is
    // lexicographically less than that of 'rhs' if 'lhs.size() < rhs.size()'.
    // This method requires that 'operator<', inducing a total order, be
    // defined for 'value_type'.

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator> (const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multiset is
    // lexicographically greater than that of the specified 'rhs' multiset, and
    // 'false' otherwise.  The value of multiset 'lhs' is lexicographically
    // greater than that of multiset 'rhs' if 'rhs' is lexicographically less
    // than 'lhs' (see 'operator<').  This method requires that 'operator<',
    // inducing a total order, be defined for 'value_type'.  Note that this
    // operator returns 'rhs < lhs'.

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator<=(const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multiset is
    // lexicographically less than or equal to that of the specified 'rhs'
    // multiset, and 'false' otherwise.  The value of multiset 'lhs' is
    // lexicographically less than or equal to that of multiset 'rhs' if 'rhs'
    // is not lexicographically less than 'lhs' (see 'operator<').  This method
    // requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(rhs < lhs)'.

template <class KEY, class COMPARATOR, class ALLOCATOR>
bool operator>=(const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the value of the specified 'lhs' multiset is
    // lexicographically greater than or equal to that of the specified 'rhs'
    // multiset, and 'false' otherwise.  The value of multiset 'lhs' is
    // lexicographically greater than or equal to that of multiset 'rhs' if
    // 'lhs' is not lexicographically less than 'rhs' (see 'operator<').  This
    // method requires that 'operator<', inducing a total order, be defined for
    // 'value_type'.  Note that this operator returns '!(lhs < rhs)'.

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY, class COMPARATOR, class ALLOCATOR, class PREDICATE>
typename multiset<KEY, COMPARATOR, ALLOCATOR>::size_type
erase_if(multiset<KEY, COMPARATOR, ALLOCATOR>& ms, PREDICATE predicate);
    // Erase all the elements in the specified multiset 'ms' that satisfy the
    // specified predicate 'predicate'.  Return the number of elements erased.

template <class KEY, class COMPARATOR, class ALLOCATOR>
void swap(multiset<KEY, COMPARATOR, ALLOCATOR>& a,
          multiset<KEY, COMPARATOR, ALLOCATOR>& b)
                                    BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                    BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
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
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // -----------------
                             // class DataWrapper
                             // -----------------

// CREATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                                              const COMPARATOR& comparator,
                                              const ALLOCATOR&  basicAllocator)
: ::bsl::multiset<KEY, COMPARATOR, ALLOCATOR>::Comparator(comparator)
, d_pool(basicAllocator)
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::DataWrapper(
                          BloombergLP::bslmf::MovableRef<DataWrapper> original)
: ::bsl::multiset<KEY, COMPARATOR, ALLOCATOR>::Comparator(
                                    MoveUtil::access(original).keyComparator())
, d_pool(MoveUtil::move(MoveUtil::access(original).d_pool))
{
}

// MANIPULATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
multiset<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory()
{
    return d_pool;
}

// ACCESSORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
const typename multiset<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
multiset<KEY, COMPARATOR, ALLOCATOR>::DataWrapper::nodeFactory() const
{
    return d_pool;
}
                             // --------------
                             // class multiset
                             // --------------

// PRIVATE MANIPULATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::Comparator&
multiset<KEY, COMPARATOR, ALLOCATOR>::comparator()
{
    return d_compAndAlloc;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
multiset<KEY, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_compAndAlloc.nodeFactory();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void multiset<KEY, COMPARATOR, ALLOCATOR>::quickSwapExchangeAllocators(
                                                               multiset& other)
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void multiset<KEY, COMPARATOR, ALLOCATOR>::quickSwapRetainAllocators(
                                                               multiset& other)
{
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &other.d_tree);
    nodeFactory().swapRetainAllocators(other.nodeFactory());

    // See 'quickSwapExchangeAllocators' (above).

    if (sizeof(NodeFactory) != sizeof(DataWrapper)) {
        comparator().swap(other.comparator());
    }
}

// PRIVATE ACCESSORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
const typename multiset<KEY, COMPARATOR, ALLOCATOR>::Comparator&
multiset<KEY, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_compAndAlloc;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
const typename multiset<KEY, COMPARATOR, ALLOCATOR>::NodeFactory&
multiset<KEY, COMPARATOR, ALLOCATOR>::nodeFactory() const
{
    return d_compAndAlloc.nodeFactory();
}

// CREATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset()
: d_compAndAlloc(COMPARATOR(), ALLOCATOR())
, d_tree()
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(const ALLOCATOR& basicAllocator)
: d_compAndAlloc(COMPARATOR(), basicAllocator)
, d_tree()
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(const multiset& original)
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(
                             BloombergLP::bslmf::MovableRef<multiset> original)
: d_compAndAlloc(MoveUtil::move(MoveUtil::access(original).d_compAndAlloc))
, d_tree()
{
    multiset& lvalue = original;
    BloombergLP::bslalg::RbTreeUtil::swap(&d_tree, &lvalue.d_tree);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(const multiset&  original,
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(
                 BloombergLP::bslmf::MovableRef<multiset>       original,
                 const typename type_identity<ALLOCATOR>::type& basicAllocator)
: d_compAndAlloc(MoveUtil::access(original).comparator().keyComparator(),
                 basicAllocator)
, d_tree()
{
    multiset& lvalue = original;

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

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(
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
        // 'insertAt' without 'findUniqueInsertLocation'.

        insert(*first);
        BloombergLP::bslalg::RbTreeNode *prevNode = d_tree.rootNode();
        while (++first != last) {
            // The values are not in order, so insert them normally.

            const value_type& value = *first;
            if (this->comparator()(value, *prevNode)) {
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(
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
        // 'insertAt' without 'findUniqueInsertLocation'.

        insert(*first);
        BloombergLP::bslalg::RbTreeNode *prevNode = d_tree.rootNode();
        while (++first != last) {
            // The values are not in order, so insert them normally.

            const value_type& value = *first;
            if (this->comparator()(value, *prevNode)) {
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
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(
                                     std::initializer_list<KEY> values,
                                     const COMPARATOR&          comparator,
                                     const ALLOCATOR&           basicAllocator)
: multiset(values.begin(), values.end(), comparator, basicAllocator)
{
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::multiset(
                                     std::initializer_list<KEY> values,
                                     const ALLOCATOR&           basicAllocator)
: multiset(values.begin(), values.end(), COMPARATOR(), basicAllocator)
{
}
#endif

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>::~multiset()
{
    clear();
}

// MANIPULATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>&
multiset<KEY, COMPARATOR, ALLOCATOR>::operator=(const multiset& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        if (AllocatorTraits::propagate_on_container_copy_assignment::value) {
            multiset other(rhs, rhs.nodeFactory().allocator());
            quickSwapExchangeAllocators(other);
        }
        else {
            multiset other(rhs, nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>&
multiset<KEY, COMPARATOR, ALLOCATOR>::operator=(
                                  BloombergLP::bslmf::MovableRef<multiset> rhs)
                        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                            AllocatorTraits::is_always_equal::value
                         && std::is_nothrow_move_assignable<COMPARATOR>::value)
{
    multiset& lvalue = rhs;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &lvalue)) {
        if (nodeFactory().allocator() == lvalue.nodeFactory().allocator()) {
            multiset other(MoveUtil::move(lvalue));
            quickSwapRetainAllocators(other);
        }
        else if (
              AllocatorTraits::propagate_on_container_move_assignment::value) {
            multiset other(MoveUtil::move(lvalue));
            quickSwapExchangeAllocators(other);
        }
        else {
            multiset other(MoveUtil::move(lvalue), nodeFactory().allocator());
            quickSwapRetainAllocators(other);
        }
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
multiset<KEY, COMPARATOR, ALLOCATOR>&
multiset<KEY, COMPARATOR, ALLOCATOR>::operator=(
                                             std::initializer_list<KEY> values)
{
    clear();
    insert(values.begin(), values.end());
    return *this;
}
#endif

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::begin() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.firstNode());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::end() BSLS_KEYWORD_NOEXCEPT
{
    return iterator(d_tree.sentinel());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::reverse_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::rbegin() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(end());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::reverse_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::rend() BSLS_KEYWORD_NOEXCEPT
{
    return reverse_iterator(begin());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::insert(const value_type& value)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value);

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(value);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::insert(
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    value_type& lvalue = value;
    bool        leftChild;

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            lvalue);

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(MoveUtil::move(lvalue));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::insert(const_iterator    hint,
                                             const value_type& value)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            value,
                                                            hintNode);

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(value);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::insert(
                              const_iterator                             hint,
                              BloombergLP::bslmf::MovableRef<value_type> value)
{
    value_type& lvalue = value;
    bool        leftChild;

    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                                            lvalue,
                                                            hintNode);

    BloombergLP::bslalg::RbTreeNode *node =
        nodeFactory().emplaceIntoNewNode(MoveUtil::move(lvalue));

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class INPUT_ITERATOR>
inline
void multiset<KEY, COMPARATOR, ALLOCATOR>::insert(INPUT_ITERATOR first,
                                                  INPUT_ITERATOR last)
{
    ///Implementation Notes
    ///--------------------
    // First, consume currently held free nodes.  If those nodes are
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void multiset<KEY, COMPARATOR, ALLOCATOR>::insert(
                                             std::initializer_list<KEY> values)
{
    insert(values.begin(), values.end());
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::emplace(Args&&... args)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
                                  BSLS_COMPILERFEATURES_FORWARD(Args,args)...);

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                     static_cast<const Node *>(node)->value());

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
template <class... Args>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::emplace_hint(const_iterator hint,
                                                   Args&&...      args)
{
    bool leftChild;

    BloombergLP::bslalg::RbTreeNode *node = nodeFactory().emplaceIntoNewNode(
                                  BSLS_COMPILERFEATURES_FORWARD(Args,args)...);

    BloombergLP::bslalg::RbTreeNode *hintNode =
                const_cast<BloombergLP::bslalg::RbTreeNode *>(hint.node());

    BloombergLP::bslalg::RbTreeNode *insertLocation =
        BloombergLP::bslalg::RbTreeUtil::findInsertLocation(&leftChild,
                                                            &d_tree,
                                                            this->comparator(),
                                      static_cast<const Node *>(node)->value(),
                                                            hintNode);

    BloombergLP::bslalg::RbTreeUtil::insertAt(&d_tree,
                                              insertLocation,
                                              leftChild,
                                              node);
    return iterator(node);
}
#endif

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::erase(const_iterator position)
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::size_type
multiset<KEY, COMPARATOR, ALLOCATOR>::erase(const key_type& key)
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::erase(const_iterator first,
                                            const_iterator last)
{
    while (first != last) {
        first = erase(first);
    }
    return iterator(last.node());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void multiset<KEY, COMPARATOR, ALLOCATOR>::swap(multiset& other)
                              BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                  AllocatorTraits::is_always_equal::value
                               && bsl::is_nothrow_swappable<COMPARATOR>::value)
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

            multiset toOtherCopy(MoveUtil::move(*this),
                                 other.nodeFactory().allocator());
            multiset toThisCopy(MoveUtil::move(other),
                                nodeFactory().allocator());

            other.quickSwapRetainAllocators(toOtherCopy);
            this->quickSwapRetainAllocators(toThisCopy);
        }
    }
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
void multiset<KEY, COMPARATOR, ALLOCATOR>::clear() BSLS_KEYWORD_NOEXCEPT
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
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::allocator_type
multiset<KEY, COMPARATOR, ALLOCATOR>::get_allocator() const
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return nodeFactory().allocator();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return cbegin();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return cend();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return crbegin();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return crend();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.firstNode());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_iterator(d_tree.sentinel());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::const_reverse_iterator
multiset<KEY, COMPARATOR, ALLOCATOR>::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool multiset<KEY, COMPARATOR, ALLOCATOR>::contains(const key_type& key) const
{
    return find(key) != end();
}

// capacity:
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool multiset<KEY, COMPARATOR, ALLOCATOR>::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_tree.numNodes();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::size_type
multiset<KEY, COMPARATOR, ALLOCATOR>::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_tree.numNodes();
}


template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::size_type
multiset<KEY, COMPARATOR, ALLOCATOR>::max_size() const BSLS_KEYWORD_NOEXCEPT
{
    return AllocatorTraits::max_size(get_allocator());
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::key_compare
multiset<KEY, COMPARATOR, ALLOCATOR>::key_comp() const
{
    return comparator().keyComparator();
}

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
typename multiset<KEY, COMPARATOR, ALLOCATOR>::value_compare
multiset<KEY, COMPARATOR, ALLOCATOR>::value_comp() const
{
    return value_compare(key_comp());
}

}  // close namespace bsl

// FREE OPERATORS
template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
bool bsl::operator==(const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return BloombergLP::bslalg::RangeCompare::equal(lhs.begin(),
                                                    lhs.end(),
                                                    lhs.size(),
                                                    rhs.begin(),
                                                    rhs.end(),
                                                    rhs.size());
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator!=(const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

template <class KEY, class COMPARATOR, class ALLOCATOR>
inline
BloombergLP::bslalg::SynthThreeWayUtil::Result<KEY>
bsl::operator<=>(const multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                 const multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return bsl::lexicographical_compare_three_way(
                              lhs.begin(),
                              lhs.end(),
                              rhs.begin(),
                              rhs.end(),
                              BloombergLP::bslalg::SynthThreeWayUtil::compare);
}

#else

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<(const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                    const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return 0 > BloombergLP::bslalg::RangeCompare::lexicographical(lhs.begin(),
                                                                  lhs.end(),
                                                                  lhs.size(),
                                                                  rhs.begin(),
                                                                  rhs.end(),
                                                                  rhs.size());
}

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>(const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                    const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return rhs < lhs;
}

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator<=(const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(rhs < lhs);
}


template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
bool bsl::operator>=(const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& lhs,
                     const bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& rhs)
{
    return !(lhs < rhs);
}

#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

// FREE FUNCTIONS
template <class KEY,  class COMPARATOR,  class ALLOCATOR, class PREDICATE>
inline
typename bsl::multiset<KEY, COMPARATOR, ALLOCATOR>::size_type
bsl::erase_if(multiset<KEY, COMPARATOR, ALLOCATOR>& ms, PREDICATE predicate)
{
    return BloombergLP::bslstl::AlgorithmUtil::containerEraseIf(ms, predicate);
}

template <class KEY,  class COMPARATOR,  class ALLOCATOR>
inline
void bsl::swap(bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& a,
               bsl::multiset<KEY, COMPARATOR, ALLOCATOR>& b)
                                 BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(
                                     BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)))
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

template <class KEY, class COMPARATOR, class ALLOCATOR>
struct HasStlIterators<bsl::multiset<KEY, COMPARATOR, ALLOCATOR> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class COMPARATOR, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::multiset<KEY, COMPARATOR, ALLOCATOR> >
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
