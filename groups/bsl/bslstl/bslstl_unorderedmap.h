// bslstl_unorderedmap.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMAP
#define INCLUDED_BSLSTL_UNORDEREDMAP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE:Provide an STL-compliant unordered_map class.
//
//@CLASSES:
//   bsl::unordered_map : STL-compatible unordered map containerr
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@SEE_ALSO: bsl+stdhdrs
//
//@DESCRIPTION: This component defines a single class template 'unordered_map',
// implementing the standard container holding a collection of unique keys with
// no guarantees on ordering, each mapped to an associated value.
//
// An instantiation of 'unordered_map' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of keys) and the set of
// values the 'unordered_map' contains, without regard to their order.  If
// 'unordered_map' is instantiated with a key type or associated value type
// that is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.  In particular, if the key or value type cannot be
// tested for equality, then an 'unordered_map' containing that type cannot be
// tested for equality.  It is even possible to instantiate 'unordered_map'
// with type that do not have an accessible copy-constructor, in which case the
// 'unordered_map' will not be copyable.  Note that the equality operator for
// each element is used to determine when two 'unordered_map' objects have the
// same value, and not the equality comparator supplied at construction.
//
// An 'unordered_map' meets the requirements of an unordered associative
// container with forward iterators in the C++11 standard [unord].  The
// 'unordered_map' implemented here adheres to the C++11 standard, except that
// it does not have interfaces that take rvalue references, 'initializer_list',
// 'emplace', or operations taking a variadic number of template parameters.
// Note that excluded C++11 features are those that require (or are greatly
// simplified by) C++11 compiler support.
//
///Requirements on 'KEY'
///---------------------
// An 'unordered_map' instantiation is a fully "Value-Semantic Type" (see
// {'bsldoc_glossary'}) only if the supplied 'KEY_TYPE' and 'MAPPED_TYPE'
// template parameters are fully value-semantic.  It is possible to instantiate
// an 'unoredered_map' with 'KEY_TYPE' and 'MAPPED_TYPE' parameter arguments
// that do not provide a full set of value-semantic operations, but
// then some methods of the container may not be instantiable.  The following
// terminology, adopted from the C++11 standard, is used in the function
// documentation of 'unordered_map' to describe a function's requirements for
// the 'KEY_TYPE' and 'MAPPED_TYPE' template parameters.  These terms are also
// defined in section [utility.arg.requirements] of the C++11 standard.  Note
// that, in the context of an 'unordered_map' instantiation, the requirements
// apply specifically to the 'unordered_map's entry type, 'value_type', which
// is an alias for std::pair<const KEY_TYPE, VALUE_TYPE>'.
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
// The type supplied as a set's 'ALLOCATOR' template parameter determines how
// that set will allocate memory.  The 'unordered_map' template supports
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
// If the parameterized 'ALLOCATOR' type of an 'unordered_map' instantiation
// is 'bsl::allocator', then objects of that set type will conform to the
// standard behavior of a 'bslma'-allocator-enabled type.  Such a set accepts
// an optional 'bslma::Allocator' argument at construction.  If the address of
// a 'bslma::Allocator' object is explicitly supplied at construction, it will
// be used to supply memory for the 'unordered_map' throughout its lifetime;
// otherwise, the 'unordered_map' will use the default allocator installed at
// the time of the 'unordered_map's construction (see 'bslma_default').  In
// addition to directly allocating memory from the indicated
// 'bslma::Allocator', an 'unordered_map' supplies that allocator's address to
// the constructors of contained objects of the parameterized 'KEY_TYPE' types
// with the 'bslalg::TypeTraitUsesBslmaAllocator' trait.
//
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLALG_UNORDEREDMAPKEYPOLICY
#include <bslstl_unorderedmapkeypolicy.h>
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

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h> // required to implement 'at'
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

// Reflect the std interface, although we do not yet require the keyword
#define BSLSTL_NOEXCEPT

namespace BloombergLP
{
namespace bslalg { class BidirectionalLink; }
}

namespace bsl {

namespace BSTL = ::BloombergLP::bslstl;

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH  = bsl::hash<KEY_TYPE>,
          class EQUAL = bsl::equal_to<KEY_TYPE>,
          class ALLOC = bsl::allocator<bsl::pair<const KEY_TYPE, MAPPED_TYPE> > >
class unordered_map
{
    // This class template implements a value-semantic container type holding
    // an unordered sequence of unique keys (of the parameterized type, 'KEY').
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic except for the 'at' method)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see {'bsldoc_glossary'}.

    typedef bsl::allocator_traits<ALLOC>                       AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

  public:
    // PUBLIC TYPES
    typedef KEY_TYPE                                   key_type;
    typedef MAPPED_TYPE                                mapped_type;
    typedef bsl::pair<const KEY_TYPE, MAPPED_TYPE>     value_type;
    typedef HASH                                       hasher;
    typedef EQUAL                                      key_equal;
    typedef ALLOC                                      allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

  private:
    typedef ::BloombergLP::bslalg::BidirectionalLink        HashTableLink;
    
    typedef BSTL::UnorderedMapKeyPolicy<value_type>         ListPolicy;
    typedef BSTL::HashTable<ListPolicy, HASH, EQUAL, ALLOC> Impl;

  public:
    typedef BSTL::HashTableIterator<value_type, difference_type>
                                                                      iterator;
    typedef BSTL::HashTableIterator<const value_type, difference_type>
                                                                const_iterator;
    typedef BSTL::HashTableBucketIterator<value_type, difference_type>
                                                                local_iterator;
    typedef BSTL::HashTableBucketIterator<const value_type, difference_type>
                                                          const_local_iterator;

  private:
    enum { DEFAULT_BUCKET_COUNT = 127 };  // 127 is a prime number

    // DATA
    Impl d_impl;

  public:
    // CREATORS
    explicit unordered_map(size_type n = DEFAULT_BUCKET_COUNT,
                           const hasher& hf = hasher(),
                           const key_equal& eql = key_equal(),
                           const allocator_type& a = allocator_type());

    template <class InputIterator>
    unordered_map(InputIterator f, InputIterator l,
                  size_type n = DEFAULT_BUCKET_COUNT,
                  const hasher& hf = hasher(),
                  const key_equal& eql = key_equal(),
                  const allocator_type& a = allocator_type());

    unordered_map(const unordered_map&);
    explicit unordered_map(const allocator_type&);
    unordered_map(const unordered_map&, const allocator_type&);

    ~unordered_map();
    unordered_map& operator=(const unordered_map&);

    allocator_type get_allocator() const BSLSTL_NOEXCEPT;

    // size and capacity
    bool empty() const BSLSTL_NOEXCEPT;
    size_type size() const BSLSTL_NOEXCEPT;
    size_type max_size() const BSLSTL_NOEXCEPT;

    // iterators
    iterator begin() BSLSTL_NOEXCEPT;
    const_iterator begin() const BSLSTL_NOEXCEPT;
    iterator end() BSLSTL_NOEXCEPT;
    const_iterator end() const BSLSTL_NOEXCEPT;
    const_iterator cbegin() const BSLSTL_NOEXCEPT;
    const_iterator cend() const BSLSTL_NOEXCEPT;

    // modifiers
    pair<iterator, bool> insert(const value_type& obj);
    iterator insert(const_iterator hint, const value_type& obj);
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last);


    iterator erase(const_iterator position);
    size_type erase(const key_type& k);
    iterator erase(const_iterator first, const_iterator last);
    void clear() BSLSTL_NOEXCEPT;
    void swap(unordered_map&);

    // observers
    hasher hash_function() const;
    key_equal key_eq() const;

    // lookup
    iterator find(const key_type& k);
    const_iterator find(const key_type& k) const;
    size_type count(const key_type& k) const;
    pair<iterator, iterator> equal_range(const key_type& k);
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

    mapped_type& operator[](const key_type& k);
    mapped_type& at(const key_type& k);
    const mapped_type& at(const key_type& k) const;

    // bucket interface
    size_type      bucket_count() const BSLSTL_NOEXCEPT;
    size_type      max_bucket_count() const BSLSTL_NOEXCEPT;
    size_type      bucket_size(size_type n) const;
    size_type      bucket(const key_type& k) const;
    local_iterator       begin(size_type n);
    const_local_iterator begin(size_type n) const;
    local_iterator       end(size_type n);
    const_local_iterator end(size_type n) const;
    const_local_iterator cbegin(size_type n) const;
    const_local_iterator cend(size_type n) const;

    // hash policy
    float load_factor() const BSLSTL_NOEXCEPT;
    float max_load_factor() const BSLSTL_NOEXCEPT;
    void  max_load_factor(float z);
    void  rehash(size_type n);
    void  reserve(size_type n);

#if BDE_BUILD_TARGET_CXX11
    template <class... Args>
    pair<iterator, bool> emplace(Args&&... args);
    template <class... Args>
    iterator emplace_hint(const_iterator position, Args&&... args);
#else
    // could supply overloads for a limitted implementation
#endif

#if BDE_BUILD_TARGET_CXX11
    unordered_map(unordered_map&&);
    unordered_map(unordered_map&&, const Allocator&);
    unordered_map(initializer_list<value_type>,
                  size_type = DEFAULT_BUCKET_COUNT,
                  const hasher& hf = hasher(),
                  const key_equal&  d_ eql = key_equal(),
                  const allocator_type& a = allocator_type());

    unordered_map& operator=(unordered_map&&);
    unordered_map& operator=(initializer_list<value_type>);

    template <class P>
    pair<iterator, bool> insert(P&& obj);
    template <class P>
    iterator insert(const_iterator hint, P&& obj);
    void insert(initializer_list<value_type>);

    mapped_type& operator[](key_type&& k);
#endif


    friend // must be defined inline, as no syntax to declare out-of-line
    bool operator==(const unordered_map& lhs, const unordered_map& rhs) {
        return lhs.d_impl == rhs.d_impl;
    }

};

// FREE FUNCTIONS
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
void swap(unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& x,
          unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& y);

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
bool operator!=(
          const unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& lhs,
          const unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& rhs);


// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================


                        //--------------------
                        // class unordered_map
                        //--------------------

// CREATORS
// construct/destroy/copy
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
unordered_map(size_type n,
              const hasher& hf,
              const key_equal& eql,
              const allocator_type& a)
: d_impl(hf, eql, n, a)
{
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
template <class InputIterator>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
unordered_map(InputIterator first,
              InputIterator last,
              size_type n,
              const hasher& hf,
              const key_equal& eql,
              const allocator_type& a)
: d_impl(hf, eql, n, a)
{
    this->insert(first, last);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
unordered_map(const unordered_map& other)
: d_impl(other.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                        other.get_allocator()))
{
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
unordered_map(const allocator_type& a)
: d_impl(HASH(), EQUAL(), DEFAULT_BUCKET_COUNT, a)
{
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
unordered_map(const unordered_map& other, const allocator_type& a)
: d_impl(other.d_impl, a)
{
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::~unordered_map()
{
    // All memory management is handled by the base 'd_impl' member.
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>&
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::operator=(
                                                    const unordered_map& other)
{
    unordered_map(other, this->get_allocator()).swap(*this);
    return *this;
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
ALLOC
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::get_allocator() const
                                                                BSLSTL_NOEXCEPT
{
    return d_impl.allocator();
}

    // size and capacity
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
bool
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::empty() const
                                                                BSLSTL_NOEXCEPT
{
    return d_impl.isEmpty();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size() const
                                                                BSLSTL_NOEXCEPT
{
    return d_impl.size();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::max_size() const
                                                                BSLSTL_NOEXCEPT
{
    return d_impl.maxSize();
}

    // iterators
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::begin()
                                                                BSLSTL_NOEXCEPT
{
    return iterator(d_impl.elementListRoot());
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::begin() const
                                                                BSLSTL_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}


template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::end() BSLSTL_NOEXCEPT
{
    return iterator();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::end() const
                                                                BSLSTL_NOEXCEPT
{
    return const_iterator();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::cbegin() const
                                                                BSLSTL_NOEXCEPT
{
    return const_iterator(d_impl.elementListRoot());
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::cend() const
                                                                BSLSTL_NOEXCEPT
{
    return const_iterator();
}

    // modifiers
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
bsl::pair<
   typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator,
   bool>
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::insert(
                                                         const value_type& obj)
{
    typedef bsl::pair<iterator, bool> ResultType;

    bool isInsertedFlag = false;
    
    HashTableLink *result = d_impl.insertIfMissing(&isInsertedFlag, obj);

    return ResultType(iterator(result), isInsertedFlag);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
insert(const_iterator hint, const value_type& obj)
{   // There is no realistic use-case for the 'hint' in an unordered_map of
    // unique values.  We could quickly test for a duplicate key, and have a
    // fast return path for when the method fails, but in the typical use case
    // where a new element is inserted, we are adding an extra key-check for no
    // benefit.  In order to insert an element into a bucket, we need to walk
    // the whole bucket looking for duplicates, and the hint is no help in
    // finding the start of a bucket.
    return this->insert(obj);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
template <class InputIterator>
inline
void
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
insert(InputIterator first, InputIterator last)
{
    if (size_t maxInsertions = BSTL::HashTable_IterUtil::insertDistance(first,
                                                                       last)) {
        this->reserve(this->size() + maxInsertions);
    }

    // This loop could be clean up with fewer temporaries, we are discarding a
    // 'pair' returned from each call to 'insert'.
    while (first != last) {
        this->insert(*first++);
    }
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::erase(
                                                       const_iterator position)
{
    BSLS_ASSERT(position != this->end());
    return iterator(d_impl.remove(position.node()));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::erase(
                                                             const key_type& k)
{
    if (HashTableLink *target = d_impl.find(k)) {
        d_impl.remove(target);
        return 1;
    }
    else {
        return 0;
    }
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
erase(const_iterator first, const_iterator last)
{   // bad answer here, need to turn 'first' into a non-const iterator

    // 7 Most of the library's algorithmic templates that operate on data
    // structures have interfaces that use ranges.  A range is a pair of
    // iterators that designate the beginning and end of the computation. A
    // range [i,i) is an empty range; in general, a range [i,j) refers to the
    // elements in the data structure starting with the element pointed to by i
    // and up to but not including the element pointed to by j. Range [i,j) is
    // valid if and only if j is reachable from i. The result of the
    // application of functions in the library to invalid ranges is undefined.
#if defined BDE_BUILD_TARGET_SAFE2
    // Check that 'first' and 'last' are valid iterators referring to this
    // container.
    if (first == last) {
        iterator it = this->begin();
        while(it != first) {
            BSLS_ASSERT(it != this->end());
        }
        while(it != last) {
            BSLS_ASSERT(it != this->end());
        }
    }
#endif

    // more efficient to:
    // 1. unlink a set of nodes
    // 2. destroy their values
    // 3. reclaim their memory
    // merge steps 2/3 to avoid multiple list walks?
    // tricky issue of fixing up bucket indices as well

    while (first != last) {
        first = this->erase(first);
    }

    return iterator(first.node()); // convert from const_iterator
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::clear()
                                                                BSLSTL_NOEXCEPT
{
    d_impl.removeAll();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::swap(
                                                          unordered_map& other)
{
    // assert that allocators are compatible
    // TBD
    d_impl.swap(other.d_impl);
}

    // observers
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::hasher
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::key_equal
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::key_eq() const
{
    return d_impl.comparator();
}


// lookup
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::find(
                                                             const key_type& k)
{
    return iterator(d_impl.find(k));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::find(
                                                       const key_type& k) const
{
    return const_iterator(d_impl.find(k));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::count(
                                                       const key_type& k) const
{
    // bool conversion to size_type gives the correct result
    // Simple implementation could be optimized with a list-walk
    // Might have list-walk search for value as LinkUtil algorithm?
    return d_impl.find(k) != 0;
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
bsl::pair<typename unordered_map<KEY_TYPE,
                                 MAPPED_TYPE,
                                 HASH,
                                 EQUAL,
                                 ALLOC>::iterator,
          typename unordered_map<KEY_TYPE,
                                 MAPPED_TYPE,
                                 HASH,
                                 EQUAL,
                                 ALLOC>::iterator>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::equal_range(
                                                             const key_type& k)
{
    typedef bsl::pair<iterator, iterator> ResultType;

    HashTableLink *first = d_impl.find(k);
    return first
         ? ResultType(iterator(first), iterator(first->nextLink()))
         : ResultType(iterator(0),     iterator(0));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
bsl::pair<typename unordered_map<KEY_TYPE,
                                 MAPPED_TYPE,
                                 HASH,
                                 EQUAL,
                                 ALLOC>::const_iterator,
          typename unordered_map<KEY_TYPE,
                                 MAPPED_TYPE,
                                 HASH,
                                 EQUAL,
                                 ALLOC>::const_iterator>
inline
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::equal_range(
                                                       const key_type& k) const
{
    typedef bsl::pair<const_iterator, const_iterator> ResultType;

    HashTableLink *first = d_impl.find(k);
    return first
         ? ResultType(const_iterator(first), const_iterator(first->nextLink()))
         : ResultType(const_iterator(0),     const_iterator(0));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::mapped_type&
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
operator[](const key_type& k)
{   // paying search cost twice when inserting
    // do not think this is used in the benchmarks that show double-cost though
    HashTableLink *node = d_impl.find(k);
    if (!node) {
        node = d_impl.doEmplace(value_type(k, mapped_type()));
    }

    BSLS_ASSERT(k == ListPolicy::extractValue(node).first);
    return ListPolicy::extractValue(node).second;
}


template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::mapped_type&
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::at(const key_type& k)
{
    HashTableLink *target = d_impl.find(k);
    
    if (!target) {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("Boo!");
    }
   
    BSLS_ASSERT(k == ListPolicy::extractValue(target).first);
    return ListPolicy::extractValue(target).second;
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
const typename
         unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::mapped_type&
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::at(const key_type& k)
                                                                          const
{
    typedef typename ::BloombergLP::bslalg::BidirectionalNode<value_type>
                                                                         BNode;
    if (HashTableLink *target = d_impl.find(k)) {
        BSLS_ASSERT(k == ListPolicy::extractKey(target));
        return static_cast<BNode *>(target)->value().second;
    }
    else {
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("Boo!");
    }
}

    // bucket interface
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::bucket_count()
                                                          const BSLSTL_NOEXCEPT
{
    return d_impl.numBuckets();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::max_bucket_count()
                                                          const BSLSTL_NOEXCEPT
{
    return d_impl.maxNumOfBuckets();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
bucket_size(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return d_impl.numElementsInBucket(n);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
bucket(const key_type& k) const
{
    return d_impl.bucketIndexForKey(k);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::local_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::begin(size_type n)
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return local_iterator(&d_impl.bucketAtIndex(n));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
 unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::begin(size_type n)
                                                                          const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return const_local_iterator(&d_impl.bucketAtIndex(n));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
       unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::local_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::end(size_type n)
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return local_iterator(0, &d_impl.bucketAtIndex(n));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::end(size_type n)
                                                                          const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return const_local_iterator(0, &d_impl.bucketAtIndex(n));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
 unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::cbegin(size_type n)
                                                                          const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return const_local_iterator(&d_impl.bucketAtIndex(n));
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
 unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::cend(size_type n)
                                                                          const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return const_local_iterator(0, &d_impl.bucketAtIndex(n));
}

    // hash policy
template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
float
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
load_factor() const BSLSTL_NOEXCEPT
{
    return d_impl.loadFactor();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
float
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
max_load_factor() const BSLSTL_NOEXCEPT
{
    return d_impl.maxLoadFactor();
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::
max_load_factor(float z)
{
    d_impl.maxLoadFactor(z);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::rehash(size_type n)
{
    return d_impl.rehash(n);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>::reserve(size_type n)
{
    return d_impl.reserve(n);
}

}  // close namespace bsl

//----------------------------------------------------------------------------
//                  free functions and opterators
//----------------------------------------------------------------------------

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
bsl::swap(bsl::unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& x,
          bsl::unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& y)
{
    x.swap(y);
}

template <class KEY_TYPE,
          class MAPPED_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
bool bsl::operator!=(
      const bsl::unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& lhs,
      const bsl::unordered_map<KEY_TYPE, MAPPED_TYPE, HASH, EQUAL, ALLOC>& rhs)
{
    return !(lhs == rhs);
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
