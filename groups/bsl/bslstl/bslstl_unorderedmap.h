// bslstl_unorderedmap.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMAP
#define INCLUDED_BSLSTL_UNORDEREDMAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE:Provide an STL-compliant 'unordered_map' class.
//
//@CLASSES:
//   bsl::unordered_map : STL-compliant 'unordered-map' class
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component defines a single class template 'unordered_map',
// implementing the standard container holding a collection of unique keys,
// each mapped to an associated value with no guarantees on ordering.
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
// the 'unordered_map' will not be copyable.  Note that the equality comparison
// operator for each key-value pair is used to determine when two
// 'unordered_map' objects have the same value, and not the instance of the
// 'EQUAL' template parameter supplied at construction.
//
// An 'unordered_map' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_map' implemented here adheres to the C++11 standard, except that
// it does not have interfaces that take rvalue references, 'initializer_list',
// 'emplace', or operations taking a variadic number of template parameters.
// Note that excluded C++11 features are those that require (or are greatly
// simplified by) C++11 compiler support.
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
// std::pair<const KEY, VALUE>'.
//
//: "default-constructible": The type provides an accessible default
//:                          constructor.
//:
//: "copy-constructible": The type provides an accessible copy constructor.
//:
//: "equality-comparable": The type provides an equality-comparison operator
//:     that defines an equivalence relationship and is both reflexive and
//:     transitive.
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
// supplied at construction, it will be used to supply memory for the
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
//  | a.resize(k)                                        | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----

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

template <
        class KEY,
        class VALUE,
        class HASH  = bsl::hash<KEY>,
        class EQUAL = bsl::equal_to<KEY>,
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
                   const hasher&         hash              = hasher(),
                   const key_equal&      keyEqual          = key_equal(),
                   const allocator_type& allocator         = allocator_type());
        // Create an empty unordered map having a 'max_load_factor' of 1.0.
        // Optionally specify an 'initialNumBuckets' indicating the minimum
        // initial size of the array of buckets of this unordered map.  If
        // 'initialNumBuckets' is not supplied, one empty bucket shall be used
        // and no memory allocated.  Optionally specify a 'hasher' used to
        // generate the hash values associated with the key-value pairs
        // contained in this unordered map.  If 'hash' is not supplied, a
        // default-constructed object of type 'hasher' is used.  Optionally
        // specify a key-equality functor 'keyEqual' used to determine whether
        // two keys have the same value.  If 'keyEqual' is not supplied, a
        // default-constructed object of type 'key_equal' is used.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'allocator_type' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.  Note
        // that more than 'initialNumBuckets' buckets may be created in order
        // to preserve the bucket allocation strategy of the hash-table (but
        // never fewer).

    explicit unordered_map(const allocator_type& allocator);
        // Create an empty unordered map, having a 'max_load_factor' of 1.0,
        // that uses the specified 'allocator' to supply memory.  Use a
        // default-constructed object of type 'hasher' to generate hash values
        // for the key-value pairs contained in this unordered map, and use a
        // default-constructed object of type 'key_equal' to determine whether
        // two keys have the same value.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.

    unordered_map(const unordered_map& original);
        // Create an unordered map having the same value, hasher, key-equality
        // comparator, and 'max_load_factor' as the specified 'original'.  Use
        // the allocator returned by 'bsl::allocator_traits<allocator_type>::
        // select_on_container_copy_construction(original.get_allocator())'
        // to supply memory.  If the 'allocator_type' is 'bsl::allocator'
        // (the default), the currently installed default allocator will be
        // used to supply memory.

    unordered_map(const unordered_map&  original,
                  const allocator_type& allocator);
        // Create an unordered map having the same value, hasher, key-equality
        // comparator, and 'max_load_factor' as the specified 'original', and
        // using the specified 'allocator' to supply memory.  If the
        // 'allocator_type' is 'bsl::allocator' (the default), then 'allocator'
        // shall be convertible to 'bslma::Allocator *'.

    template <class INPUT_ITERATOR>
    unordered_map(INPUT_ITERATOR        first,
                  INPUT_ITERATOR        last,
                  size_type             initialNumBuckets = 0,
                  const hasher&         hash              = hasher(),
                  const key_equal&      keyEqual          = key_equal(),
                  const allocator_type& allocator         = allocator_type());
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
        // supplied.  Optionally specify a 'hasher' used to generate hash
        // values associated with the key-value pairs contained in this
        // unordered map.  If 'hash' is not supplied, a default-constructed
        // object of type 'hasher' is used.  Optionally specify a key-equality
        // functor 'keyEqual' used to verify that two key values are the same.
        // If 'keyEqual' is not supplied, a default-constructed object of type
        // 'key_equal' is used.  Optionally specify an 'allocator' used to
        // supply memory.  If 'allocator' is not supplied, a
        // default-constructed object of the (template parameter) type
        // 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'allocator_type' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type'.  The behavior is undefined unless 'first' and 'last'
        // refer to a sequence of valid values where 'first' is at a position
        // at or before 'last'.  Note that more than 'initialNumBuckets'
        // buckets may be created in order to preserve the bucket allocation
        // strategy of the hash-table (but never fewer).

    ~unordered_map();
        // Destroy this object and each of its elements.

    // MANIPULATORS
    unordered_map& operator=(const unordered_map& rhs);
        // Assign to this object the value, hasher, key-equality functor, and
        // 'max_load_factor' of the specified 'rhs' object, propagate to this
        // object the allocator of 'rhs' if 'allocator_type' has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter) types 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    mapped_type& operator[](const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key' in this unordered map; if this
        // unordered map does not already contain a 'value_type' object with
        // 'key', first insert a new 'value_type' object having 'key' and a
        // default-constructed 'VALUE' object.  This method requires that the
        // (template parameter) type 'KEY' is "copy-constructible" and the
        // (template parameter) 'VALUE' is "default-constructible" (see
        // {Requirements on 'KEY' and 'VALUE'}).

    mapped_type& at(const key_type& key);
        // Return a reference providing modifiable access to the mapped-value
        // associated with the specified 'key', if such an entry exists;
        // otherwise throw a 'std::out_of_range' exception.  Note that this
        // method is not exception agnostic.

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
        // the bucket is empty.  The behavior is undefined unless
        // 'index < bucket_count()'.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this unordered map.  The behavior is undefined unless
        // 'index < bucket_count()'.

    void clear();
        // Remove all entries from this unordered map.  Note that this
        // unordered map will be empty after this call, but allocated memory
        // may be retained for future use.

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
        // already present.  This method requires that the (template parameter)
        // types 'KEY' and 'VALUE' both be "copy-constructible" (see
        // {Requirements on 'KEY' and 'VALUE'}).  Note that this one template
        // stands in for two 'insert' functions in the C++11 standard.

    template <class SOURCE_TYPE>
    iterator insert(const_iterator hint, const SOURCE_TYPE& value);
        // Insert the specified 'value' into this unordered map if the key (the
        // 'first' element) of a 'value_type' object constructed from 'value'
        // does not already exist in this unordered map.  Return an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this unordered map whose key is the same as that of the converted
        // 'value'.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be "copy-constructible" (see {Requirements on
        // 'KEY' and 'VALUE'}).  The behavior is undefined unless the specified
        // 'hint' is a valid iterator into this unordered map.  Note that
        // 'hint' is not used by this method template, and this one template
        // stands in for two 'insert' functions in the C++11 standard.

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Create a 'value_type' object for each iterator in the range starting
        // at the specified 'first' iterator and ending immediately before the
        // specified 'last' iterator, by converting from the object referred to
        // by each iterator.  Insert into this unordered map each such object
        // whose key is not already contained.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'value_type'.  This method requires that the
        // (template parameter) types 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

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
        // newMaxLoadFactor'.  Note that, however, the C++11 standard does not
        // allow this operation to rehash, as it requires a constant cost for
        // all (positive) values of 'newMaxLoadFactor'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this unordered
        // map to at least the specified 'numBuckets', and redistribute all the
        // contained elements into the new sequence of buckets, according to
        // their hash values.  After this call, 'load_factor' will be less than
        // or equal to 'max_load_factor'.  This operation has no effect if
        // rehashing the elements into 'numBuckets' would cause this map to
        // exceed its 'max_load_factor'.

    void reserve(size_type numElements);
        // Change the size of the array of buckets maintained by this unordered
        // container so that 'load_factor() < max_load_factor()', even if the
        // container grows to a size of the specified 'numElements', and
        // redistribute all the contained elements into the new sequence of
        // buckets, according to their hash values.  This operation has no
        // effect if 'numElements <= size()'.

    void swap(unordered_map& other);
        // Exchange the value of this object as well as its hasher,
        // key-equality functor, and 'max_load_factor' with those of the
        // specified 'other' object.  If 'allocator_type' has the trait
        // 'propagate_on_container_swap', exchange the allocator of this object
        // with that of the 'other' object, and do not modify either allocator
        // otherwise.  This method provides the no-throw exception-safety
        // guarantee and guarantees O[1] complexity, as long as the (template
        // parameter) 'HASH' and (template parameter) 'EQUAL' do not throw when
        // swapped.  The behavior is undefined unless either this object was
        // created with the same allocator as 'other' or 'allocator_type' has
        // the 'propagate_on_container_swap' trait.

    // ACCESSORS
    const mapped_type& at(const key_type& key) const;
        // Return a reference providing non-modifiable access to the
        // mapped-value associated with the specified 'key', if such an entry
        // exists; otherwise throw a 'std::out_of_range' exception.  Note that
        // this method is not exception agnostic.

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
        // past-the-end element in the sequence of 'value_type' objects of
        // the bucket having the specified 'index' in the array of buckets
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
    // contained in 'rhs' having the same value, and vice versa.  This method
    // requires that the (template parameter) types 'KEY' and 'VALUE' both be
    // "equality-comparable" (see {Requirements on 'KEY' and 'VALUE'}).

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
                const unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_map' objects do not
    // have the same value if they do not have the same number of key-value
    // pairs, or for some key-value pair that is contained in 'lhs' there is
    // not a key-value pair in 'rhs' having the same value or vice-versa.  This
    // method requires that the (template parameter) types 'KEY' and 'VALUE'
    // both be "equality-comparable" (see {Requirements on 'KEY' and 'VALUE'}).

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
    // O[1] complexity, as long as the (template parameter) type 'HASH' and the
    // (template parameter) type 'EQUAL' do not throw when swapped.  The
    // behavior is undefined unless 'a' and 'b' were created with the same
    // allocator, or 'ALLOCATOR' has the 'propagate_on_container_swap' trait.

}  // close namespace bsl

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

}  // close namespace BloombergLP

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
              const hasher&         hash,
              const key_equal&      keyEqual,
              const allocator_type& allocator)
: d_impl(hash, keyEqual, initialNumBuckets, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                       INPUT_ITERATOR        first,
                                       INPUT_ITERATOR        last,
                                       size_type             initialNumBuckets,
                                       const hasher&         hash,
                                       const key_equal&      keyEqual,
                                       const allocator_type& allocator)
: d_impl(hash, keyEqual, initialNumBuckets, allocator)
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
                                               const allocator_type& allocator)
: d_impl(HASH(), EQUAL(), 0, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_map(
                                               const unordered_map&  original,
                                               const allocator_type& allocator)
: d_impl(original.d_impl, allocator)
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
    std::size_t maxInsertions =
            ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last);
    if (maxInsertions) {
        this->reserve(this->size() + maxInsertions);
    }

    bool isInsertedFlag;  // not used
    while (first != last) {
        d_impl.insertIfMissing(&isInsertedFlag, *first++);
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
    return d_impl.rehash(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
unordered_map<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::reserve(
                                                         size_type numElements)
{
    return d_impl.rehashForNumElements(numElements);
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
    return d_impl.maxNumOfBuckets();
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


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
