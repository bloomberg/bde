// bslstl_unorderedmultimap.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMULTIMAP
#define INCLUDED_BSLSTL_UNORDEREDMULTIMAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant unordered_multimap class.
//
//@CLASSES:
//   bsl::unordered_multimap : hashed-map container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component defines a single class template,
// 'unordered_multimap', implementing the standard container holding a
// collection of (possibly repeated) keys, each mapped to an associated value
// (with minimal guarantees on ordering).
//
// An instantiation of 'unordered_multimap' is an allocator-aware,
// value-semantic type whose salient attributes are its size (number of keys)
// and the set of key-value pairs the 'unordered_multimap' contains, without
// regard to their order.  If 'unordered_multimap' is instantiated with a key
// type or mapped value-type that is not itself value-semantic, then it will
// not retain all of its value-semantic qualities.  In particular, if the key
// or value type cannot be tested for equality, then an 'unordered_multimap'
// containing that type cannot be tested for equality.  It is even possible to
// instantiate 'unordered_multimap' with type that do not have an accessible
// copy-constructor, in which case the 'unordered_multimap' will not be
// copyable.  Note that the equality operator for each key-value pair is used
// to determine when two 'unordered_multimap' objects have the same value, and
// not the instance of the 'EQUAL' template parameter supplied at construction.
//
// An 'unordered_multimap' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_multimap' implemented here adheres to the C++11 standard, except
// that it does not have interfaces that take rvalue references,
// 'initializer_list', 'emplace', or operations taking a variadic number of
// template parameters.  Note that excluded C++11 features are those that
// require (or are greatly simplified by) C++11 compiler support.
//
///Requirements on 'KEY' and 'VALUE'
///---------------------------------
// An 'unordered_multimap' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY' and 'VALUE' template
// parameters are fully value-semantic.  It is possible to instantiate an
// 'unoredered_multimap' with 'KEY' and 'VALUE' parameter arguments that do not
// provide a full set of value-semantic operations, but then some methods of
// the container may not be instantiable.  The following terminology, adopted
// from the C++11 standard, is used in the function documentation of
// 'unordered_multimap' to describe a function's requirements for the 'KEY' and
// 'VALUE' template parameters.  These terms are also defined in section
// [utility.arg.requirements] of the C++11 standard.  Note that, in the context
// of an 'unordered_multimap' instantiation, the requirements apply
// specifically to the 'unordered_multimap's entry type, 'value_type', which is
// an alias for std::pair<const KEY, VALUE>'.
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
// this container will allocate memory.  The 'unordered_multimap' template
// supports allocators meeting the requirements of the C++11 standard
// [allocator.requirements], and in addition it supports scoped-allocators
// derived from the 'bslma::Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use the template's default
// 'ALLOCATOR' type: The default type for the 'ALLOCATOR' template parameter,
// 'bsl::allocator', provides a C++11 standard-compatible adapter for a
// 'bslma::Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If the parameterized 'ALLOCATOR' type of an 'unordered_multimap'
// instantiation is 'bsl::allocator', then objects of that set type will
// conform to the standard behavior of a 'bslma'-allocator-enabled type.  Such
// a type accepts an optional 'bslma::Allocator' argument at construction.  If
// the address of a 'bslma::Allocator' object is explicitly supplied at
// construction, it will be used to supply memory for the 'unordered_multimap'
// throughout its lifetime; otherwise, the 'unordered_multimap' will use the
// default allocator installed at the time of the 'unordered_multimap's
// construction (see 'bslma_default').  In addition to directly allocating
// memory from the indicated 'bslma::Allocator', an 'unordered_multimap'
// supplies that allocator's address to the constructors of contained objects
// of the parameterized 'KEY' types with the
// 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
//-----------------------------------------------------------------------------
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'unsupported_multimap':
//..
//  Legend
//  ------
//  'K'             - parameterized 'KEY' type of the unordered multi map
//  'V'             - parameterized 'VALUE' type of the unordered multi map
//  'a', 'b'        - two distinct objects of type 'unordered_multimap<K, V>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'w'             - number of buckets of 'a'
//  'value_type'    - unordered_multimap<K, V>::value_type
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
//  | unordered_multimap<K, V> a;    (dflt construction) | O[1]               |
//  | unordered_multimap<K, V> a(al);                    |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(b); (copy construction) | Average: O[n]      |
//  | unordered_multimap<K, V> a(b, al);                 | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(w);                     | O[n]               |
//  | unordered_multimap<K, V> a(w, hf);                 |                    |
//  | unordered_multimap<K, V> a(w, hf, eq);             |                    |
//  | unordered_multimap<K, V> a(w, hf, eq, al);         |                    |
//  +----------------------------------------------------+--------------------+
//  | unordered_multimap<K, V> a(i1, i2);                | Average: O[N]      |
//  | unordered_multimap<K, V> a(i1, i2, n)              | Worst:  O[N^2]     |
//  | unordered_multimap<K, V> a(i1, i2, n, hf);         | where N =          |
//  | unordered_multimap<K, V> a(i1, i2, n, hf, eq);     |  distance(i1, i2)] |
//  | unordered_multimap<K, V> a(i1, i2, n, hf, eq, al); |                    |
//  +----------------------------------------------------+--------------------+
//  | a.~unordered_multimap<K, V>(); (destruction)       | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | a = b;          (assignment)                       | Average: O[n]      |
//  |                                                    | Worst:   O[n^2]    |
//  +----------------------------------------------------+--------------------+
//  | a.begin(), a.end(), a.cbegin(), a.cend(),          | O[1]               |
//  +----------------------------------------------------+--------------------+
//  | a == b, a != b                                     | Best:  O[n]        |
//  |                                                    | Worst: O[n^2]      |
//  +----------------------------------------------------+--------------------+
//  | a.swap(b), swap(a, b                               | O[1] if 'a' and    |
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
//  | a.allocator()                                      | O[1]               |
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
//  | a.max_load_factor(z)                               | Average: O[1]      |
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



#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#error "<bslstl_unorderedmultimap.h> header can't be included directly in \
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
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION
#include <bslstl_unorderedmapkeyconfiguration.h>
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

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace bsl {

template <class KEY,
          class VALUE,                  // Not to be confused with 'value_type'
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY>,
          class ALLOCATOR = bsl::allocator<bsl::pair<const KEY, VALUE> > >
class unordered_multimap
{
    // an unordered sequence of unique keys (of the parameterized type, 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    typedef bsl::allocator_traits<ALLOCATOR>              AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

  public:
    // PUBLIC TYPES
    typedef KEY                         key_type;
    typedef VALUE                       mapped_type;
    typedef bsl::pair<const KEY, VALUE> value_type;
    typedef HASH                        hasher;
    typedef EQUAL                       key_equal;
    typedef ALLOCATOR                   allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

  private:
    // PRIVATE TYPES
    typedef ::BloombergLP::bslalg::BidirectionalLink   HashTableLink;

    typedef ::BloombergLP::bslstl::UnorderedMapKeyConfiguration<value_type>
                                                                    ListPolicy;
    typedef ::BloombergLP::bslstl::HashTable<ListPolicy,
                                             HASH,
                                             EQUAL,
                                             ALLOCATOR> Impl;
  public:
    // PUBLIC TYPES
    typedef ::BloombergLP::bslstl::HashTableIterator<value_type,
                                                     difference_type> iterator;
    typedef ::BloombergLP::bslstl::HashTableIterator<const value_type,
                                                     difference_type>
                                                                const_iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<value_type,
                                                           difference_type>
                                                                local_iterator;
    typedef ::BloombergLP::bslstl::HashTableBucketIterator<const value_type,
                                                           difference_type>
                                                          const_local_iterator;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(
                         unordered_multimap,
                         ::BloombergLP::bslmf::IsBitwiseMoveable,
                         ::BloombergLP::bslmf::IsBitwiseMoveable<Impl>::value);

  private:
    // DATA
    Impl d_impl;

    // FRIEND
    template <class KEY2,
              class VALUE2,
              class HASH2,
              class EQUAL2,
              class ALLOCATOR2>
    friend
    bool operator==(const unordered_multimap<KEY2,
                                             VALUE2,
                                             HASH2,
                                             EQUAL2,
                                             ALLOCATOR2>&,
                    const unordered_multimap<KEY2,
                                             VALUE2,
                                             HASH2,
                                             EQUAL2,
                                             ALLOCATOR2>&);

  public:
    // CREATORS
    explicit unordered_multimap(
                           size_type             initialNumBuckets = 0,
                           const hasher&         hash = hasher(),
                           const key_equal&      keyEqual = key_equal(),
                           const allocator_type& allocator = allocator_type());
        // Construct an empty unordered multi map.  Optionally specify an
        // 'initialNumBuckets' indicating the initial size of the array of
        // buckets of this container.  If 'initialNumBuckets' is not supplied,
        // an implementation defined value is used.  Optionally specify a
        // 'hasher' used to generate the hash values associated to the
        // key-value pairs contained in this object.  If 'hash' is not
        // supplied, a default-constructed object of type 'hasher' is used.
        // Optionally specify a key-equality functor 'keyEqual' used to verify
        // that two key values are the same.  If 'keyEqual' is not supplied, a
        // default-constructed object of type 'key_equal' is used.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.

    explicit unordered_multimap(const allocator_type& allocator);
        // Construct an empty unordered multi map that uses the specified
        // 'allocator' to supply memory.  Use a default-constructed object of
        // type 'hasher' to generate hash values for the key-value pairs
        // contained in this object.  Also, use a default-constructed object of
        // type 'key_equal' to verify that two key values are the same.  If the
        // 'allocator_type' is 'bsl::allocator' (the default), then 'allocator'
        // shall be convertible to 'bslma::Allocator *'.

    unordered_multimap(const unordered_multimap& original);
    unordered_multimap(const unordered_multimap& original,
                        const allocator_type&    allocator);
        // Construct an unordered multi map having the same value as that of
        // the specified 'original'.  Use a default-constructed object of type
        // 'hasher' to generate hash values for the key-value pairs contained
        // in this object.  Also, use a default-constructed object of type
        // 'key_equal' to verify that two key values are the same.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // supplied, a default-constructed object of type 'allocator_type' is
        // used.  If the 'allocator_type' is 'bsl::allocator' (the default),
        // then 'allocator' shall be convertible to 'bslma::Allocator *'.

    template <class INPUT_ITERATOR>
    unordered_multimap(INPUT_ITERATOR        first,
                       INPUT_ITERATOR        last,
                       size_type             initialNumBuckets = 0,
                       const hasher&         hash = hasher(),
                       const key_equal&      keyEqual = key_equal(),
                       const allocator_type& allocator = allocator_type());
        // Construct an empty unordered multi map and insert each 'value_type'
        // object in the sequence starting at the specified 'first' element,
        // and ending immediately before the specified 'last' element.
        // Optionally specify an 'initialNumBuckets' indicating the initial
        // size of the array of buckets of this container.  If
        // 'initialNumBuckets' is not supplied, an implementation defined value
        // is used.  Optionally specify a 'hasher' used to generate hash values
        // for the key-value pairs contained in this object.  If 'hash' is not
        // supplied, a default-constructed object of type 'hasher' is used.
        // Optionally specify a key-equality functor 'keyEqual' used to verify
        // that two key values are the same.  If 'keyEqual' is not supplied, a
        // default-constructed object of type 'key_equal' is used.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // type 'allocator_type' is used.  If the 'allocator_type' is
        // 'bsl::allocator' (the default), then 'allocator' shall be
        // convertible to 'bslma::Allocator *'.  If the 'allocator_type' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed default allocator will be used to supply memory.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type'.  The behavior is undefined unless 'first' and 'last'
        // refer to a sequence of valid values where 'first' is at a position
        // at or before 'last'.  This method requires that the (template
        // parameter) types 'KEY' and 'VALUE' both be "copy-constructible" (see
        // {Requirements on 'KEY' and 'VALUE'}).

    ~unordered_multimap();
        // Destroy this object.

    // MANIPULATORS
    unordered_multimap& operator=(const unordered_multimap& rhs);
        // Assign to this object the value, hasher, and key-equality functor of
        // the specified 'rhs' object, propagate to this object the allocator
        // of 'rhs' if the 'ALLOCATOR' type has trait
        // 'propagate_on_container_copy_assignment', and return a reference
        // providing modifiable access to this object.  This method requires
        // that the (template parameter types) 'KEY' and 'VALUE' both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).

    iterator begin();
        // Return an iterator providing modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this multi-map, or the 'end' iterator if
        // this multi-map is empty.

    iterator end();
        // Return an iterator providing modifiable access to the past-the-end
        // element in the sequence of 'value_type' objects maintained by this
        // unordered multi map.

    local_iterator begin(size_type index);
        // Return a local iterator providing modifiable access to the first
        // 'value_type' object in the sequence of 'value_type' objects
        // of the bucket having the specified 'index', in the array of buckets
        // maintained by this multi-map, or the 'end(index)'
        // otherwise.

    local_iterator end(size_type index);
        // Return a local iterator providing modifiable access to the
        // past-the-end element in the sequence of 'value_type' objects of the
        // bucket having the specified 'index's , in the array of buckets
        // maintained by this multi-map.

    template <class SOURCE_TYPE>
    iterator insert(const SOURCE_TYPE& value);
        // Insert the specified 'value' into this multi-map, and return an
        // iterator to the newly inserted element.  This method requires that
        // the (template parameter) types 'KEY' and 'VALUE' types both be
        // "copy-constructible" (see {Requirements on 'KEY' and 'VALUE'}).
        // Note that this one template stands in for two 'insert' functions in
        // the C++11 standard.

    template <class SOURCE_TYPE>
    iterator insert(const_iterator hint, const SOURCE_TYPE& value);
        // Insert the specified 'value' into this multi-map (in
        // constant time if the specified 'hint' is a valid element in the
        // bucket to which 'value' belongs).  Return an iterator referring to
        // the newly inserted 'value_type' object in this multi-map whose key
        // is the same as that of 'value'.  If 'hint' is not a position in the
        // bucket of the key of 'value', this operation will have worst case
        // O[N] and average case constant time complexity, where 'N' is the
        // size of this multi-map.  The behavior is
        // undefined unless 'hint' is a valid iterator into this unordered
        // multi map.  This method requires that the (template parameter) types
        // 'KEY' and 'VALUE' both be "copy-constructible" (see {Requirements on
        // 'KEY' and 'VALUE'}).  Note that this one template stands in for two
        // 'insert' functions in the C++11 standard.

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this multi-map the value of each 'value_type'
        // object in the range starting at the specified 'first' iterator and
        // ending immediately before the specified 'last' iterator.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the C++11 standard
        // [24.2.3] providing access to values of a type convertible to
        // 'value_type'.  This method requires that the (template parameter)
        // types 'KEY' and 'VALUE' both be "copy-constructible" (see
        // {Requirements on 'KEY' and 'VALUE'}).

    iterator erase(const_iterator position);
        // Remove from this multi-map the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this container.  The behavior
        // is undefined unless 'position' refers to a 'value_type' object in
        // this multi-map.

    size_type erase(const key_type& key);
        // Remove from this container all objects whose keys match the
        // specified 'key', and return the number of objects deleted.  If there
        // is no object matching 'key', return 0 with no other effect.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this container the objects starting at the specified
        // 'first' position up to, but not including the specified 'last'
        // position, and return 'last'.  The behavior is undefined unless
        // 'first' and 'last' either refer to elements in this container or are
        // the 'end' iterator, and the 'first' position is at or before the
        // 'last' position in the ordered sequence provided by this container.

    void clear();
        // Remove all entries from this container.  Note that the container is
        // empty after this call, but allocated memory may be retained for
        // future use.

    iterator find(const key_type& key);
        // Return an iterator providing modifiable access to the first
        // 'value_type' object in the sequence of all the 'value_type' objects
        // of this container matching the specified 'key', if they exist, and
        // the past-the-end ('end') iterator otherwise.

    pair<iterator, iterator> equal_range(const key_type& key);
        // Return a pair of iterators providing modifiable access to the
        // sequence of 'value_type' objects in this multi-map
        // matching the specified 'key', where the the first iterator is
        // positioned at the start of the sequence, and the second is
        // positioned one past the end of the sequence.  If this unordered
        // multi map contains no 'value_type' objects matching 'key', then the
        // two returned iterators will have the same value.

    void max_load_factor(float newLoadFactor);
        // Set the maximum load factor of this container to the specified
        // 'newLoadFactor'.  This operation will not do an immediate rehash of
        // the container, if that is wanted, it is recommended that this call
        // be followed by a call to 'reserve'.

    void rehash(size_type numBuckets);
        // Change the size of the array of buckets maintained by this container
        // to the specified 'numBuckets', and redistribute all the contained
        // elements into the new sequence of buckets, according to their hash
        // values.  Note that this operation has no effect if rehashing the
        // elements into 'numBuckets' would cause this multi-map to
        // exceed its 'max_load_factor'.

    void reserve(size_type numElements);
        // Increase the number of buckets of this multi-map to a
        // quantity such that the ratio between the specified 'numElements' and
        // this quantity does not exceed 'max_load_factor'.

    void swap(unordered_multimap& other);
        // Exchange the value of this object as well as its hasher and
        // key-equality functor with those of the specified 'other' object.
        // Additionally if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true' then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee and
        // guarantees O[1] complexity.  The behavior is undefined is unless
        // either this object was created with the same allocator as 'other' or
        // 'propagate_on_container_swap' is 'true'.


    // ACCESSORS
    allocator_type get_allocator() const;
        // Return (a copy of) the allocator used for memory allocation by this
        // multi-map.

    const_iterator begin() const;
    const_iterator cbegin() const;
        // Return an iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects)
        // maintained by this multi-map, or the 'end' iterator if
        // this multi-map is empty.

    const_iterator end() const;
    const_iterator cend() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects)
        // maintained by this multi-map.

    const_local_iterator begin(size_type index) const;
    const_local_iterator cbegin(size_type index) const;
        // Return a local iterator providing non-modifiable access to the first
        // 'value_type' object (in the sequence of 'value_type' objects) of the
        // bucket having the specified 'index' in the array of buckets
        // maintained by this container, or the 'end(index)' otherwise.

    const_local_iterator end(size_type index) const;
    const_local_iterator cend(size_type index) const;
        // Return a local iterator providing non-modifiable access to the
        // past-the-end element (in the sequence of 'value_type' objects) of
        // the bucket having the specified 'index' in the array of buckets
        // maintained by this container.

    size_type bucket(const key_type& key) const;
        // Return the index of the bucket, in the array of buckets of this
        // container, where values matching the specified 'key' would be
        // inserted.

    size_type bucket_count() const;
        // Return the number of buckets in the array of buckets maintained by
        // this container.

    size_type max_bucket_count() const;
        // Return a theoretical upper bound on the largest number of buckets
        // that this container could possibly manage.  Note that there is no
        // guarantee that the container can successfully grow to the returned
        // size, or even close to that size without running out of resources.

    size_type bucket_size(size_type index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this
        // container.

    size_type count(const key_type& key) const;
        // Return the number of 'value_type' objects within this container
        // matching the specified 'key'.

    bool empty() const;
        // Return 'true' if this container contains no elements, and 'false'
        // otherwise.

    pair<const_iterator, const_iterator> equal_range(
                                                    const key_type& key) const;
        // Return a pair of iterators providing non-modifiable access to the
        // sequence of 'value_type' objects in this container matching the
        // specified 'key', where the the first iterator is positioned at the
        // start of the sequence and the second iterator is positioned one past
        // the end of the sequence.  If this container contains no 'value_type'
        // objects matching 'key' then the two returned iterators will have the
        // same value.

    const_iterator find(const key_type& key) const;
        // Return an iterator providing non-modifiable access to the
        // first 'value_type' object in the sequence of all the 'value_type'
        // objects of this container matching the specified 'key', if they
        // exist, and the past-the-end ('end') iterator otherwise.  If multiple
        // elements match 'key', they are guaranteed to be adjacent to each
        // other, and this function will return the first
        // in the sequence.

    hasher hash_function() const;
        // Return (a copy of) the hash unary functor used by this container to
        // generate a hash value (of type 'size_t') for a 'key_type' object.

    key_equal key_eq() const;
        // Return (a copy of) the key-equality binary functor that returns
        // 'true' if the value of two 'key_type' objects is the same, and
        // 'false' otherwise.

    float load_factor() const;
        // Return the current ratio between the 'size' of this container and
        // the number of buckets.  The 'load_factor' is a measure of how full
        // the container is, and a higher load factor leads to an increased
        // number of collisions, thus resulting in a loss performance.

    float max_load_factor() const;
        // Return the maximum load factor allowed for this container.  If
        // an insert operation would cause 'load_factor' to exceed
        // the 'max_load_factor', that same insert operation will increase the
        // number of buckets and rehash the elements of the container into
        // those buckets the (see rehash).  Note that it is possible for the
        // load factor of this container to exceed 'max_load_factor',
        // especially after 'max_load_factor(newLoadFactor)' is called.

    size_type max_size() const;
        // Return a theoretical upper bound on the largest number of elements
        // that this container could possibly hold.  Note that there is no
        // guarantee that the container can successfully grow to the returned
        // size, or even close to that size without running out of resources.

    size_type size() const;
        // Return the number of elements in this container.
};

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'unordered_multimap' objects have the
    // same value if they have the same number of key-value pairs, and for each
    // key-value pair that is contained in 'lhs' there is a pair value-key
    // contained in 'rhs' having the same value, and vice-versa.  This method
    // requires that the (template parameter) types 'KEY' and 'VALUE' both be
    // "equality-comparable" (see {Requirements
    // on 'KEY' and 'VALUE'}).

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
            const unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'unordered_multimap' objects do
    // not have the same value if they do not have the same number of key-value
    // pairs, or that for some key-value pair that is contained in 'lhs' there
    // is not a key-value pair in 'rhs' having the same value, and vice-versa.
    // This method requires that the (template parameter) types 'KEY' and
    // 'VALUE' both be "equality-comparable" (see {Requirements on 'KEY' and
    // 'VALUE'}).

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void swap(unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& x,
          unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& y);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  Additionally if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true' then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees O[1] complexity.  The
    // behavior is undefined is unless either this object was created with the
    // same allocator as 'other' or 'propagate_on_container_swap' is 'true'.

}  // close namespace bsl

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct HasStlIterators<bsl::unordered_multimap<KEY,
                                               VALUE,
                                               HASH,
                                               EQUAL,
                                               ALLOCATOR> >
: bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<bsl::unordered_multimap<KEY,
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

                        //-------------------------
                        // class unordered_multimap
                        //-------------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                       size_type             initialNumBuckets,
                                       const hasher&         hash,
                                       const key_equal&      keyEqual,
                                       const allocator_type& allocator)
: d_impl(hash, keyEqual, initialNumBuckets, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
unordered_multimap(INPUT_ITERATOR        first,
                   INPUT_ITERATOR        last,
                   size_type             initialNumBuckets,
                   const hasher&         hash,
                   const key_equal&      keyEqual,
                   const allocator_type& allocator)
: d_impl(hash, keyEqual, initialNumBuckets, allocator)
{
    this->insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL,
          class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                            const unordered_multimap& original)
: d_impl(original.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                     original.get_allocator()))
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                               const allocator_type& allocator)
: d_impl(HASH(), EQUAL(), 0, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL,
          class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::unordered_multimap(
                                           const unordered_multimap& original,
                                           const allocator_type&     allocator)
: d_impl(original.d_impl, allocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::~unordered_multimap()
{
    // All memory management is handled by the base 'd_impl' member.
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL,
          class ALLOCATOR>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>&
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator=(
                                                 const unordered_multimap& rhs)
{
    unordered_multimap(rhs, this->get_allocator()).swap(*this);
    return *this;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(size_type index)
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::clear()
{
    d_impl.removeAll();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(
                                                           const key_type& key)
{
    return iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                       const_iterator position)
{
    BSLS_ASSERT(position != this->end());

    return iterator(d_impl.remove(position.node()));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                           const key_type& key)
{   // As an alternative implementation, the table could return an extracted
    // "slice" list from the underlying table, and now need merely:
    //   iterate each node, destroying the associated value
    //   reclaim each node (potentially returning to a node-pool)

    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;
    if (HashTableLink *target = d_impl.find(key)) {
        target = d_impl.remove(target);
        size_type result = 1;
        while (target && this->key_eq()(
              key,
              ListPolicy::extractKey(
                                    static_cast<BNode *>(target)->value()))) {
            target = d_impl.remove(target);
            ++result;
        }
        return result;                                                // RETURN
    }
    else {
        return 0;                                                     // RETURN
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::erase(
                                                         const_iterator first,
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const SOURCE_TYPE& value)
{
    return iterator(d_impl.insert(value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                       const_iterator    hint,
                                                       const SOURCE_TYPE& value)
{
    return iterator(d_impl.insert(value, hint.node()));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
template <class INPUT_ITERATOR>
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                          INPUT_ITERATOR first,
                                                          INPUT_ITERATOR last)
{
    if (size_t maxInsertions =
            ::BloombergLP::bslstl::IteratorUtil::insertDistance(first, last)) {
        this->reserve(this->size() + maxInsertions);
    }

    while (first != last) {
        d_impl.insert(*first++);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor(
                                                           float newLoadFactor)
{
    d_impl.setMaxLoadFactor(newLoadFactor);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::rehash(
                                                          size_type numBuckets)
{
    return d_impl.rehash(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::reserve(
                                                         size_type numElements)
{
    return d_impl.rehashForNumElements(numElements);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
void unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::swap(
                                                     unordered_multimap& other)
{
    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin()
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end()
{
    return iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end() const
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin() const
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend() const
{
    return const_iterator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
                                                           const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::begin(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::end(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cbegin(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(&d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::
                                                           const_local_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::cend(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return const_local_iterator(0, &d_impl.bucketAtIndex(index));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket(
                                                     const key_type& key) const
{
    return d_impl.bucketIndexForKey(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_count() const
{
    return d_impl.numBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::bucket_size(
                                                         size_type index) const
{
    BSLS_ASSERT_SAFE(index < this->bucket_count());

    return d_impl.countElementsInBucket(index);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>:: size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::count(
                                                     const key_type& key) const
{
    typedef ::BloombergLP::bslalg::BidirectionalNode<value_type> BNode;

    size_type result = 0;
    for (HashTableLink *cursor = d_impl.find(key);
         cursor;
         ++result, cursor = cursor->nextLink())
    {
        BNode *cursorNode = static_cast<BNode *>(cursor);
        if (!this->key_eq()(key,
                            ListPolicy::extractKey(cursorNode->value()))) {

            break;
        }
    }
    return  result;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bool unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::empty() const
{
    return 0 == d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
ALLOCATOR
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::get_allocator() const
{
    return d_impl.allocator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_size() const
{
    return d_impl.maxSize();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hasher
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_equal
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::key_eq() const
{
    return d_impl.comparator();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::const_iterator
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::find(
                                                     const key_type& key) const
{
    return const_iterator(d_impl.find(key));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<
     typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator,
     typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                           const key_type& key)
{
    typedef bsl::pair<iterator, iterator> ResultType;

    HashTableLink *first;
    HashTableLink *last;
    d_impl.findRange(&first, &last, key);
    return ResultType(iterator(first), iterator(last));
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_impl.size();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
bsl::pair<typename unordered_multimap<KEY,
                                      VALUE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::const_iterator,
          typename unordered_multimap<KEY,
                                      VALUE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::const_iterator>
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                     const key_type& key) const
{
    typedef bsl::pair<const_iterator, const_iterator> ResultType;

    HashTableLink *first;
    HashTableLink *last;
    d_impl.findRange(&first, &last, key);
    return ResultType(const_iterator(first), const_iterator(last));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::load_factor()
                                                                          const
{
    return d_impl.loadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
typename unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::size_type
unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>:: max_bucket_count()
                                                                          const
{
    return d_impl.maxNumOfBuckets();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
float unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::max_load_factor()
                                                                          const
{
    return d_impl.maxLoadFactor();
}

}  // close namespace bsl

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator==(
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
bool bsl::operator!=(
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& lhs,
        const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& rhs)
{
    return !(lhs == rhs);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
inline
void bsl::swap(bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& x,
               bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>& y)
{
    x.swap(y);
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
