// bslstl_unorderedmultiset.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMULTISET
#define INCLUDED_BSLSTL_UNORDEREDMULTISET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash-container with support for duplicate values 
//
//@CLASSES:
//   bsl::unordered_multiset : hashed-map container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an implementation of the container,
// 'unordered_multiset', specified by the C++11 standard.
//
// This implementation will be the 'monkey-skin' approach that minimizes the
// size of the nodes, by using the preceding element in the list when storing
// references to nodes, e.g., in iterators or buckets.  The main container is
// a singly-linked list, indexed by a vector of buckets.  The overhead is:
//   o one pointer in each node
//   o one pointer per bucket (no. buckets determined by max_load_factor)
//   o one additional (empty) sentinel node
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propogate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'equal_to'
// predicate.
//
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_unorderedmultiset.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKTRANSLATORFORSETS
#include <bslalg_bidirectionallinktranslatorforsets.h>
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
#include <bslstl_pair.h>  // result type of 'equal_range' method
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>  // for 'std::iterator_traits'
#define INCLUDED_ITERATOR
#endif

namespace BloombergLP
{
namespace bslalg { struct BidirectionalLink; }
}

namespace bsl {

namespace BSTL = ::BloombergLP::bslstl;

template <class KEY_TYPE,
          class HASH  = bsl::hash<KEY_TYPE>,
          class EQUAL = bsl::equal_to<KEY_TYPE>,
          class ALLOC = bsl::allocator<KEY_TYPE> >
class unordered_multiset
{
    typedef bsl::allocator_traits<ALLOC>                       AllocatorTraits;
        // This typedef is an alias for the allocator traits type associated
        // with this container.

  public:
    // types
    typedef KEY_TYPE key_type;
    typedef KEY_TYPE value_type;
    typedef HASH     hasher;
    typedef EQUAL    key_equal;
    typedef ALLOC    allocator_type;

    typedef typename allocator_type::reference         reference;
    typedef typename allocator_type::const_reference   const_reference;

    typedef typename AllocatorTraits::size_type        size_type;
    typedef typename AllocatorTraits::difference_type  difference_type;
    typedef typename AllocatorTraits::pointer          pointer;
    typedef typename AllocatorTraits::const_pointer    const_pointer;

  private:
    typedef ::BloombergLP::bslalg::BidirectionalLink             HashTableLink;
    typedef ::BloombergLP::bslalg::BidirectionalLinkTranslatorForSets<
                                                                    value_type>
                                                                    ListPolicy;

    typedef BSTL::HashTable<ListPolicy, HASH, EQUAL, ALLOC> Impl;

  public:
    typedef BSTL::HashTableIterator<value_type, difference_type, ListPolicy>
                                                                      iterator;
    typedef iterator                                            const_iterator;
    typedef BSTL::HashTableBucketIterator<value_type, difference_type, ListPolicy>
                                                                local_iterator;
    typedef local_iterator                                const_local_iterator;

  private:
//    enum { DEFAULT_BUCKET_COUNT = 127 };  // 127 is a prime number
    enum { DEFAULT_BUCKET_COUNT = 13 };  // 13 is a prime number

    // DATA
    Impl d_impl;

  public:
    // CREATORS
    explicit unordered_multiset(size_type n = DEFAULT_BUCKET_COUNT,
                                const hasher& hf = hasher(),
                                const key_equal& eql = key_equal(),
                                const allocator_type& a = allocator_type());

    template <class InputIterator>
    unordered_multiset(InputIterator f, InputIterator l,
                       size_type n = DEFAULT_BUCKET_COUNT,
                       const hasher& hf = hasher(),
                       const key_equal& eql = key_equal(),
                       const allocator_type& a = allocator_type());

    unordered_multiset(const unordered_multiset&);

    explicit unordered_multiset(const allocator_type&);

    unordered_multiset(const unordered_multiset&, const allocator_type&);

    ~unordered_multiset();
    unordered_multiset& operator=(const unordered_multiset&);

    allocator_type get_allocator() const;

    // size and capacity
    bool empty() const;
    size_type size() const;
    size_type max_size() const;

    // iterators
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    // modifiers
    iterator insert(const value_type& obj);
    iterator insert(const_iterator hint, const value_type& obj);
    template <class InputIterator>
    void insert(InputIterator first, InputIterator last);

    iterator erase(const_iterator position);
    size_type erase(const key_type& k);
    iterator erase(const_iterator first, const_iterator last);

    void clear();

    void swap(unordered_multiset&);

    // observers
    hasher hash_function() const;
    key_equal key_eq() const;

    // lookup
    iterator find(const key_type& k);
    const_iterator find(const key_type& k) const;

    size_type count(const key_type& k) const;

    pair<iterator, iterator> equal_range(const key_type& k);
    pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

    // bucket interface
    size_type bucket_count() const;
    size_type max_bucket_count() const;
    size_type bucket_size(size_type n) const;
    size_type bucket(const key_type& k) const;

    local_iterator       begin(size_type n);
    const_local_iterator begin(size_type n) const;
    local_iterator       end(size_type n);
    const_local_iterator end(size_type n) const;
    const_local_iterator cbegin(size_type n) const;
    const_local_iterator cend(size_type n) const;

    // hash policy
    float load_factor() const;
    float max_load_factor() const;
    void max_load_factor(float z);
    void rehash(size_type n);
    void reserve(size_type n);


    friend // must be defined inline, as no syntax to declare out-of-line
    bool operator==(const unordered_multiset& lhs,
                    const unordered_multiset& rhs) {
        return lhs.d_impl.hasSameValue(rhs.d_impl);
    }
    
};

template <class KEY_TYPE, class HASH, class EQUAL, class ALLOC>
void swap(unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& x,
          unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& y);

template <class KEY_TYPE, class HASH, class EQUAL, class ALLOC>
bool operator!=(const unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& a,
                const unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& b);


// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //-------------------------
                        // class unordered_multiset
                        //-------------------------

// CREATORS
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
unordered_multiset(size_type        n,
                   const hasher&    hf,
                   const key_equal& eql,
                   const allocator_type& a)
: d_impl(hf, eql, n, a)
{
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
template <class InputIterator>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
unordered_multiset(InputIterator    first,
                   InputIterator    last,
                   size_type        n,
                   const hasher&    hf,
                   const key_equal& eql,
                   const allocator_type& a)
: d_impl(hf, eql, n, a)
{   // Could pick initial number of buckets based on length of [first,last).
    // Reduces number of rehashes, and no risk of unduly low load_factor on
    // a multi-container.
    this->insert(first, last);
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
unordered_multiset(const allocator_type& a)
: d_impl(HASH(), EQUAL(), DEFAULT_BUCKET_COUNT, a)
{
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
unordered_multiset(const unordered_multiset& other, const allocator_type& a)
: d_impl(other.d_impl, a)
{
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
unordered_multiset(const unordered_multiset& other)
: d_impl(other.d_impl,
         AllocatorTraits::select_on_container_copy_construction(
                                                        other.get_allocator()))
{
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::~unordered_multiset()
{
    // All memory management is handled by the base 'd_impl' member.
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>&
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::operator=(
                                               const unordered_multiset& other)
{
    // Actually, need to check propagate_on_copy_assign trait
    unordered_multiset(other, get_allocator()).swap(*this);
    return *this;

}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
ALLOC
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::get_allocator() const
{
    return d_impl.allocator();
}

    // size and capacity
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
bool
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::empty() const
{
    return d_impl.isEmpty();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size() const
{
    return d_impl.size();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::max_size() const
{
    return d_impl.maxSize();
}

    // iterators
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::begin()
{
    return iterator(d_impl.begin());
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::begin() const
{
    return const_iterator(d_impl.begin());
}


template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::end()
{
    return iterator();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::end() const
{
    return const_iterator();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::cbegin() const
{
    return const_iterator(d_impl.begin());
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::cend() const
{
    return const_iterator();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::insert(const value_type& obj)
{   
    return iterator(d_impl.insertContiguous(obj));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
insert(const_iterator hint, const value_type& obj)
{
    // To be useful, hint must point to an iterator with an object comparing
    // equal to 'obj'
    if (this->end() != hint && d_impl.comparator()(obj, *hint)) {
        return d_impl.insertValueBefore(obj, hint.node());
    }
    else {
        return this->insert(obj);
    }
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
template <class InputIterator>
void
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
insert(InputIterator first, InputIterator last)
{
    if (size_t maxInsertions = BSTL::HashTable_IterUtil::insertDistance(first,
                                                                       last)) {
        this->reserve(this->size() + maxInsertions);
    }

    while (first != last) {
        // Use the appropriate reference type to avoid making a temporary.
        // Be sure to dereference the iterator exactly once on each iteration.
        typename std::iterator_traits<InputIterator>::reference obj = *first;
        if (HashTableLink *existing = d_impl.find(obj)) {
            d_impl.insertValueBefore(obj, existing);
        }
        else {
            d_impl.doEmplace(obj);
        }
        ++first;
    }
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::erase(
                                                       const_iterator position)
{
    BSLS_ASSERT(position != this->end());
    return iterator(d_impl.eraseNode(position.node()));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::erase(const key_type& k)
{   // As an alternative implementation, the table could return an extracted
    // "slice" list from the underlying table, and now need merely:
    //   iterate each node, destroying the associated value
    //   reclaim each node (potentially returning to a node-pool)
    if (HashTableLink *target = d_impl.find(k)) {
        target = d_impl.eraseNode(target);
        size_type result = 1;
        while (target && this->key_eq()(k, ListPolicy::extractValue(target))) {
            target = d_impl.eraseNode(target);
            ++result;
        }
        return result;
    }
    else {
        return 0;
    }
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
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

    // implementation must handle the case that 'last' is 'end()', which will
    // be invalidated when the preceding element is erased.

    while (first != last) {
        first = this->erase(first);
    }

    return iterator(first.node()); // convert from const_iterator
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
void
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::clear()
{
    d_impl.clear();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
void
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
swap(unordered_multiset& other)
{
    // assert that allocators are compatible
    // TBD
    d_impl.swap(other.d_impl);
}

    // observers
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::hasher
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::hash_function() const
{
    return d_impl.hasher();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::key_equal
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::key_eq() const
{
    return d_impl.comparator();
}


// lookup
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::find(const key_type& k)
{
    return iterator(d_impl.find(k));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::find(const key_type& k) const
{
    return const_iterator(d_impl.find(k));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
count(const key_type& k) const
{
    size_type result = 0;
    for (HashTableLink *cursor = d_impl.find(k);
         cursor;
         ++result, cursor = cursor->next()) {
        if (!this->key_eq()(k, ListPolicy::extractValue(cursor))) {
            break;
        }
    }

    return result;
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
bsl::pair<typename unordered_multiset<KEY_TYPE,
                                     HASH,
                                     EQUAL,
                                     ALLOC>::iterator,
          typename unordered_multiset<KEY_TYPE,
                                     HASH,
                                     EQUAL,
                                     ALLOC>::iterator>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
equal_range(const key_type& k)
{
    HashTableLink *first;
    HashTableLink *last;
    d_impl.findKeyRange(&first, &last, k);
    return bsl::pair<iterator, iterator>(iterator(first), iterator(last));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
bsl::pair<typename unordered_multiset<KEY_TYPE,
                                     HASH,
                                     EQUAL,
                                     ALLOC>::const_iterator,
          typename unordered_multiset<KEY_TYPE,
                                     HASH,
                                     EQUAL,
                                     ALLOC>::const_iterator>
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
equal_range(const key_type& k) const
{
    HashTableLink *first;
    HashTableLink *last;
    d_impl.findKeyRange(&first, &last, k);
    return bsl::pair<const_iterator, const_iterator>(const_iterator(first),
                                                     const_iterator(last));
}

    // bucket interface
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::bucket_count() const
{
    return d_impl.numOfBuckets();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::max_bucket_count() const
{
    return d_impl.maxNumOfBuckets();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
bucket_size(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return d_impl.bucket_size(n);
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::size_type
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::
bucket(const key_type& k) const
{
    return d_impl.bucket(k);
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::local_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::begin(size_type n)
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return local_iterator(&d_impl.getBucket(n));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::begin(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return const_local_iterator(&d_impl.getBucket(n));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::local_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::end(size_type n)
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return local_iterator(0, &d_impl.getBucket(n));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
 unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::end(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return const_local_iterator(0, &d_impl.getBucket(n));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename
 unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::cbegin(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    //SP: invoke begin(n)? 
    return const_local_iterator(&d_impl.getBucket(n));
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
typename unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::const_local_iterator
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::cend(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    //SP: invoke end(n)? 
    return const_local_iterator(0, &d_impl.getBucket(n));
}

    // hash policy
template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
float unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::load_factor() const
{
    return d_impl.load_factor();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
float unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::max_load_factor() const
{
    return d_impl.max_load_factor();
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::max_load_factor(float z)
{
    d_impl.max_load_factor(z);
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::rehash(size_type n)
{
    return d_impl.rehash(n);
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>::reserve(size_type n)
{
    return d_impl.reserve(n);
}

}  // close namespace bsl

//----------------------------------------------------------------------------
//                  free functions and opterators
//----------------------------------------------------------------------------

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
void
bsl::swap(bsl::unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& x,
          bsl::unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& y)
{
    x.swap(y);
}

template <class KEY_TYPE,
          class HASH,
          class EQUAL,
          class ALLOC>
inline
bool
bsl::operator!=(
              const bsl::unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& lhs,
              const bsl::unordered_multiset<KEY_TYPE, HASH, EQUAL, ALLOC>& rhs)
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
