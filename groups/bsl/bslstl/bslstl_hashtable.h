// bslstl_hashtable.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLE
#define INCLUDED_BSLSTL_HASHTABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash-container with support for duplicate values 
//
//@CLASSES:
//   bslimp::HashTable : hashed-map container
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an implementation of the container,
// 'HashTable', specified by the C++11 standard.
//
// This implementation will use a single, bidirectional list, indexed by
// a dynamic array of buckets, each of which is a simple pointer to a node
// in the list.
//
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propogate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'equal_to'
// predicate.
//
// Currently not implemented:
//   copy assignment operator
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
//#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
//#error "<bslstl_XXX.h> header can't be included directly in
//BSL_OVERRIDES_STD mode"
//#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLISTNODE
#include <bslalg_bidirectionallistnode.h>
#endif

#ifndef INCLUDED_BSLALG_FUNCTORADAPTER
#include <bslalg_functoradapter.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEUTIL
#include <bslalg_hashtableutil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLSTL_HASHTABLENODEFACTORY
#include <bslstl_hashtablenodefactory.h>
#endif

#ifndef INCLUDED_BSLSTL_VECTOR
#include <bslstl_vector.h>  // to be replaced with a simpler facility
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>  // for swap
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CMATH
#include <cmath> // for 'ceil'
#define INCLUDED_CMATH
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>  // for tag dispatch on iterator traits
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_LIMITS
#include <limits>  // for 'numeric_limits<size_t>'
#define INCLUDED_LIMITS
#endif

namespace BloombergLP {
   
namespace bslalg { struct BidirectionalLink; }

namespace bslstl {

// No need for any defaults below, this imp.-detail component is intended to be
// instantiated and used from the public interface of a std container that
// provides all the user-friendly defaults, and explicitly pass down what is
// needed.
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
class HashTable : private bslalg::FunctorAdapter<HASH>::Type
                , private bslalg::FunctorAdapter<EQUAL>::Type
{
  public:
    typedef typename VALUE_POLICY::ValueType         ValueType;
    typedef typename VALUE_POLICY::KeyType           KeyType;
    typedef ALLOC                                    AllocatorType;
    typedef bslalg::BidirectionalListNode<ValueType> NodeType;


    typedef typename ALLOC::size_type            size_type; // should find via allocator_traits?
  private:
    // PRIVATE TYPES
    typedef typename ALLOC::template rebind<bslalg::HashTableBucket>::other
                                                                  VECTOR_ALLOC;
    typedef typename bsl::allocator_traits<ALLOC>::template
                                        rebind_traits<NodeType>::allocator_type
                                                                 NodeAllocator;

    typedef HashTableNodeFactory<NodeAllocator> NodeFactory;

    typedef typename bslalg::FunctorAdapter<HASH>::Type  HasherBaseType;
    typedef typename bslalg::FunctorAdapter<EQUAL>::Type ComparatorBaseType;

  private:
    // DATA
    //HashTablePolicy               d_hashPolicy;  // typically empty
    bsl::vector<bslalg::HashTableBucket, VECTOR_ALLOC>  d_buckets;
    bslalg::BidirectionalLink  *d_root;
    size_t                      d_size;
    NodeFactory                 d_nodeFactory;
    size_t                      d_loadLimit;  // Rehash if d_data.size() > d_loadLimit.
    double                      d_maxLoadFactor; // preserve rounding in d_loadLimit.

  private:
    // this type is not copyable/clonable
    // an efficient clone should be implemented before a production release.
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);

  private:
    // PRIVATE MANIPULATORS
    template<typename SOURCE_TYPE>
    bslalg::BidirectionalLink *allocateNodeHavingValue(const SOURCE_TYPE& obj);

    size_t hashCodeForNode(bslalg::BidirectionalLink *node) const;

  public:
    // CREATORS
    HashTable(const HASH&          hash,
              const EQUAL&         compare,
              size_type            initialBucketCount,
              const AllocatorType& a);
        // Behavior is undefined unless '0 < intialBucketCount'.

    HashTable(const HashTable& other, const AllocatorType& a);
        // Copy the specified 'other' using the specified allocator 'a'.

    ~HashTable();

    // allocator
    AllocatorType get_allocator() const;

    // size and capacity
    bool empty() const;
    size_type size() const;
    size_type max_size() const;

    // iterators
    bslalg::BidirectionalLink *begin() const;

    // modifiers
    void destroyListValues();

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *doEmplace(const SOURCE_TYPE& obj);
    bslalg::BidirectionalLink *insertValueBefore(
                                            const ValueType&           obj,
                                            bslalg::BidirectionalLink *before);

    bslalg::BidirectionalLink *eraseNode(bslalg::BidirectionalLink *node);

    void expandTable();

    void swap(HashTable&);

    // observers
    const HASH& hasher()     const;
    const EQUAL& comparator() const;

    // lookup
    bslalg::BidirectionalLink *find(const KeyType& k) const;

    void findKeyRange(bslalg::BidirectionalLink **first,
                      bslalg::BidirectionalLink **last,
                      const KeyType&              k) const;

    bslalg::BidirectionalLink *findEndOfRange(
                                       bslalg::BidirectionalLink *first) const;
        // Return the address of the first node in the total sequence of the
        // hash table following the specified 'first' node that holds a value
        // with a key that does not have the same value as the key stored in
        // the node pointed to by 'first'.

    // bucket interface
    size_type bucket_count() const;
    size_type max_bucket_count() const;
    size_type bucket_size(size_type n) const;

    template<class key_type> // hope to determine from existing policy when done
    size_type bucket(const key_type& k) const;

    const bslalg::HashTableBucket *begin(size_type n) const;

    // hash policy
    float load_factor() const;
    float max_load_factor() const;
    void max_load_factor(float z);
    void rehash(size_type n);
    void reserve(size_type n);

    bool hasSameValue(const HashTable& other) const;
};

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOC>
void swap(HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>& x,
          HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>& y);

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOC>
bool operator==(const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>& lhs,
                const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>& rhs);

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOC>
bool operator!=(const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>& lhs,
                const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>& rhs);

// Move this to its own component, and avoid 'iterator' dependency
// This is used only in higher level components anyway - does not belong.
// Generally useful for all container implementations, probably already exists
// somewhere in bslalg.
struct HashTable_IterUtil {

    // generic utility that needs a non-template hosted home
    template <class InputIterator>
    static std::size_t insertDistance(InputIterator first, InputIterator last);
        // Return 0 if InputIterator really is limitted to the standard
        // input-iterator category, otherwise return the distance from first
        // to last.
};

struct HashTable_GrowthUtil {
    static size_t nextPrime(size_t n);
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================
// Static utility that should move to a utility class without template
// dependencies, to avoid needless duplication.  This utility is widely
// useful across a number of std containers, are we sure it does not exist
// already?
template <class InputIterator>
std::size_t HashTable_IterUtil::insertDistance(InputIterator first,
                                               InputIterator last)
{
#if defined(BSLS_PLATFORM__CMP_SUN)
    // Need to work around Sun library broken treatment of iterator tag
    // dispatch.
    return 0;
#else
    struct impl {
        // This local class provides a utility to determine the maximum
        // number of elements to be inserted, and so minimize the number of
        // rehashes.
        // We note that this utility might be useful for other container
        // implementations.
        static size_t calc(InputIterator first,
                           InputIterator last,
                           std::input_iterator_tag)
        {
            return 0;
        }

        static size_t calc(InputIterator first,
                          InputIterator last,
                          std::forward_iterator_tag)
        {
            return std::distance(first, last);
        }
    };

    typedef typename std::iterator_traits<InputIterator>::iterator_category
                                                                  IterCategory;
    return impl::calc(first, last, IterCategory());
#endif
}
                        //--------------------------
                        // class HashTable
                        //--------------------------

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
size_t HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::hashCodeForNode(
                                         bslalg::BidirectionalLink *node) const
{
    BSLS_ASSERT_SAFE(node);
    return hasher()(VALUE_POLICY::extractKey(node));
}

// CREATORS
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::
HashTable(const HASH&          hash,
          const EQUAL&         compare,
          size_type            initialBucketCount,
          const AllocatorType& a)
: HasherBaseType(hash)
, ComparatorBaseType(compare)
, d_buckets(initialBucketCount, a)
, d_root()
, d_size()
, d_nodeFactory(a)
, d_loadLimit(initialBucketCount)
, d_maxLoadFactor(1.0)
{
    BSLS_ASSERT_SAFE(0 < initialBucketCount);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::
HashTable(const HashTable& other, const AllocatorType& a)
: HasherBaseType(static_cast<const HasherBaseType&>(other))
, ComparatorBaseType(static_cast<const ComparatorBaseType&>(other))
, d_buckets(other.d_buckets.size(), a)
, d_root()
, d_size(other.d_size)
, d_nodeFactory(a)
, d_loadLimit(other.d_loadLimit)
, d_maxLoadFactor(other.d_maxLoadFactor)
{
    BSLS_ASSERT_SAFE(0 < d_buckets.size());
    if (other.d_root) {
        try {
            bslalg::BidirectionalLink *lastNode   = 0;
            size_type  lastBucket = 0;
            for (const bslalg::BidirectionalLink *cursor = other.d_root;
                 cursor;
                 cursor = cursor->next())
            {
                bslalg::BidirectionalLink *next = d_nodeFactory.cloneNode(cursor);

                // After this point, no operations can throw
                size_type bucket =
                             bslalg::HashTableUtil::bucketNumberForHashCode(
                                                         hashCodeForNode(next),
                                                         d_buckets.size());
                // APPEND 'next' after 'lastNode', and update any buckets / end-markers
                if (!lastNode) {
                    d_buckets[bucket].fillNewBucket(next);
                    d_root            = next;
                    lastBucket        = bucket;
                }
                else {
                    lastNode->setNext(next);
                    next->setPrev(lastNode);

                    if (bucket != lastBucket) {
                        d_buckets[bucket].fillNewBucket(next);
                        d_buckets[lastBucket].setLast(lastNode);
                        lastBucket        = bucket;
                    }
                }
                lastNode = next;
            }
            d_buckets[lastBucket].setLast(lastNode);
        }
        catch(...) {
            // reclaim any allocated nodes, the vector will clean up
            // after itself.
            this->destroyListValues();
            throw;
        }
    }
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::~HashTable()
{
    // This is overly expensive, as we need only reclaim memory, and need not
    // update bucket indices and other sentries.  We might also want to reclaim
    // any leading sentinel node.
    // We can factor 'clear' into a two-part implementation later.
    // Note that the 'vector' member will reclaim its own memory, so nothing
    // additional beyond clear needed there.
    this->destroyListValues();
}

    // size and capacity
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bool
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::empty() const
{
    return !d_size;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size() const
{
    return d_size;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::max_size() const
{
    return native_std::numeric_limits<std::size_t>::max();
}

    // iterators
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::begin() const
{
    return d_root;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
template<typename SOURCE_TYPE>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::
allocateNodeHavingValue(const SOURCE_TYPE& obj)
{
    NodeType *result = d_nodeFactory.createEmptyNode();
    // try/catch should be replaced with a proctor, to most easily support
    // building without exceptions.
    try {
       d_nodeFactory.constructValueInNode(result, obj);
    }
    catch(...) {
        // reclaim new node
        d_nodeFactory.reclaimNode(result); 
        throw;
    }

    return result;
}

// template on SOURCE_TYPE to avoid binding an un-necessary temporary object.
// Typically an optimization for iterator-range inserts rather than inserting
// a single value.
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
template <class SOURCE_TYPE>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::doEmplace(const SOURCE_TYPE& obj)
{
    if (d_size >= d_loadLimit) {
        this->expandTable();
    }

    bslalg::BidirectionalLink *newNode = this->allocateNodeHavingValue(obj);
    bslalg::HashTableUtil::insertAtFrontOfBucket(newNode,
                                                 hashCodeForNode(newNode),
                                                 &d_root,
                                                 d_buckets.data(),
                                                 d_buckets.size());
    ++d_size;
    return newNode;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::
insertValueBefore(const ValueType& obj, bslalg::BidirectionalLink *before)
{
    BSLS_ASSERT(before);

    if (d_size >= d_loadLimit) {
        this->expandTable();
    }

    bslalg::BidirectionalLink *newNode = this->allocateNodeHavingValue(obj);
    bslalg::HashTableUtil::insertDuplicateAtPosition(newNode,
                                                     hashCodeForNode(newNode),
                                                     before,
                                                     &d_root,
                                                     d_buckets.data(),
                                                     d_buckets.size());
    ++d_size;
    return newNode;
}

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOC>
inline
void
HashTable<VALUE_TYPE, HASH, EQUAL, ALLOC>::expandTable()
{
//    this->rehash(2*this->bucket_count());
    this->rehash(this->bucket_count() + 1);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::eraseNode(
                                                   bslalg::BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->prev() || d_root == node);

    bslalg::HashTableUtil::removeNodeFromTable(node,
                                               hashCodeForNode(node),
                                               &d_root,
                                               d_buckets.data(),
                                               d_buckets.size());
    --d_size;

    bslalg::BidirectionalLink *result = node->next();
    d_nodeFactory.disposeNode((NodeType *)node);

    return result;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::destroyListValues()
{
    // Doing too much book-keeping of hash table - look for a more efficient
    // dispose-as-we-walk, that simply resets table.d_root.next = 0, and assigns
    // the buckets index all null pointers
    if (bslalg::BidirectionalLink *root = d_root) {
        bslalg::BidirectionalLink *next;
        do {
            next = root->next();
            d_nodeFactory.disposeNode((NodeType *)root);
        }
        while((root = next));
    }
    d_buckets.assign(d_buckets.size(), bslalg::HashTableBucket());
    d_size = 0;
    d_root = 0;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::swap(HashTable& other)
{
    // assert that allocators are compatible
    // We do not yet support allocator propagation by allocator traits
    BSLS_ASSERT(this->d_nodeFactory.allocator() ==
                other.d_nodeFactory.allocator());

    using std::swap;    // otherwise it is hidden by this very definition!
    //swap(d_hashPolicy, other.d_hashPolicy);
    // TBD: SWAP BASE CLASS HASH/EQUAL FUNCTORS
    //static_cast<HasherBaseType *>(this)->swap(other);
    //static_cast<ComparatorBaseType *>(this)->swap(other);

    bslalg::SwapUtil::swap(
                     static_cast<HasherBaseType*>(this),
                     static_cast<HasherBaseType*>(BSLS_UTIL_ADDRESSOF(other)));

    bslalg::SwapUtil::swap(
                 static_cast<ComparatorBaseType*>(this),
                 static_cast<ComparatorBaseType*>(BSLS_UTIL_ADDRESSOF(other)));

    d_buckets.swap(other.d_buckets);
    swap(d_root, other.d_root);
    swap(d_size, other.d_size);
//    swap(d_nodeFactory, other.d_nodeFactory);  // swap based on allocator propagation 
    swap(d_loadLimit, other.d_loadLimit);
    swap(d_maxLoadFactor, other.d_maxLoadFactor);
}

// observers
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
const HASH& HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::hasher() const
{
    return *this; 
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
const EQUAL& HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::comparator() const
{
    return *this;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::AllocatorType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::get_allocator() const
{
    return d_nodeFactory.allocator();
}

// lookup
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::find(const KeyType& k) const
{
    return bslalg::HashTableUtil::find<VALUE_POLICY>(this->comparator(),
                                                     d_buckets.data(),
                                                     d_buckets.size(),
                                                     k,
                                                     this->hasher()(k));
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::findKeyRange(
                                                 bslalg::BidirectionalLink **first,
                                                 bslalg::BidirectionalLink **last,
                                                 const KeyType& k) const
{
    BSLS_ASSERT_SAFE(first);
    BSLS_ASSERT_SAFE(last);

    *last = (*first = this->find(k))
          ? this->findEndOfRange(*first)
          : 0;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::findEndOfRange(
                                            bslalg::BidirectionalLink *first) const
{
    BSLS_ASSERT_SAFE(first);

    const KeyType& k = VALUE_POLICY::extractKey(first);
    while ((first = first->next()) &&
           this->comparator()(k, VALUE_POLICY::extractKey(first))) {
    }
    return first;
}

// bucket interface
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::bucket_count() const
{
    return d_buckets.size();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::max_bucket_count() const
{
    return this->max_size();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::bucket_size(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return d_buckets[n].size();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
template <class key_type> // hope to determine from existing policy when done
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::bucket(const key_type& k) const
{
    size_t hashCode = this->hasher()(k);
    return bslalg::HashTableUtil::bucketNumberForHashCode(hashCode,
                                                          d_buckets.size());
}


template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
const bslalg::HashTableBucket *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::begin(size_type n) const
{
    BSLS_ASSERT_SAFE(n < this->bucket_count());
    return &d_buckets[n];
}


    // hash policy
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
float HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::load_factor() const
{
    return (double)size() / this->bucket_count();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
float HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::max_load_factor() const
{
    return d_maxLoadFactor;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
void HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::max_load_factor(float z)
{
    d_maxLoadFactor = z;
    d_loadLimit = this->bucket_count() * z;
    if (d_loadLimit < this->size()) {
        this->reserve(this->size());
    }
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::rehash(size_type n)
{
    // compute a "good" number of buckets, e.g., pick a prime number
    // from a sorted array of exponentially increasing primes. 
    n = HashTable_GrowthUtil::nextPrime(n);
    if (n > this->bucket_count()) {
        d_loadLimit = n * this->max_load_factor();
        // Which allocator should we use for this array, given we expect to
        // 'swap' at the end?
        bsl::vector<bslalg::HashTableBucket, VECTOR_ALLOC>
                                      newBuckets(n, d_buckets.get_allocator());
        
        if (d_root) {
            d_root = bslalg::HashTableUtil::rehash<VALUE_POLICY>(
                                                             newBuckets.data(),
                                                             newBuckets.size(),
                                                             d_root,
                                                             hasher());
        }
        d_buckets.swap(newBuckets);
    }
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::reserve(size_type n)
{
    this->rehash(ceil(n / this->max_load_factor()));
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bool
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::hasSameValue(
                                                  const HashTable& other) const
{
    typedef typename
           bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>::size_type
                                                                      SizeType;

    typedef typename VALUE_POLICY::KeyType   KeyType;
    typedef typename VALUE_POLICY::ValueType ValueType;

    // First test - are the containers the same size?
    if (this->size() != other.size()) {
        return false;                                                 // RETURN
    }
    bslalg::BidirectionalLink *cursor = this->begin();
    if (!cursor) {
        // containers are the same size, and empty.
        return true;                                                  // RETURN
    }

    while (cursor) {
        bslalg::BidirectionalLink *rhsFirst =
                                  other.find(VALUE_POLICY::extractKey(cursor));
        if (!rhsFirst) {
            return false;  // no matching key                         // RETURN
        }

        bslalg::BidirectionalLink *endRange = this->findEndOfRange(cursor);
        bslalg::BidirectionalLink *rhsLast  = other.findEndOfRange(rhsFirst);

            
        // Check the key-groups have the same length - another quick-fail test.
        bslalg::BidirectionalLink *endWalker = cursor->next();
        bslalg::BidirectionalLink *rhsWalker = rhsFirst->next();
        while (endWalker != endRange) {
            if (rhsWalker == rhsLast) {
                return false;   // different length subsequences      // RETURN
            }
            endWalker = endWalker->next();
            rhsWalker = rhsWalker->next();
        }
        if (rhsWalker != rhsLast) {
            return false;   // different length subsequences          // RETURN
        }

        // Efficiently compare identical prefixes: O(N) if sequences
        // have the same elements in the same order.  Note that comparison of
        // values in nodes is tested using 'operator==' and not the
        // key-equality comparator stored in the hash table.
        while (cursor != endRange &&
                   VALUE_POLICY::extractValue(cursor) == 
                   VALUE_POLICY::extractValue(rhsFirst))
        {
            cursor   = cursor->next();
            rhsFirst = rhsFirst->next();
        }

        if (cursor == endRange) {
            continue;
        }


        // Now comes the harder part of validating one subsequence is a
        // permutation of another, by counting elements that compare equal
        // using the equality operator, 'operator=='.
        // Note that this code could be simplified for hash-tables with unique
        // keys, as we can omit the counting-scan, and merely test for any
        // match within the other range.  Trade off the ease of a single
        // well-tested code path, vs. the importance of an efficient operator==
        // for hash containers.  This is currently the only place the table
        // would care about uniqueness, and risk different table types for
        // unique vs. multi-containers.
        for (bslalg::BidirectionalLink *marker = cursor;
             marker != endRange;
             marker = marker->next())
        {
            const ValueType& valueAtMarker =
                                            VALUE_POLICY::extractValue(marker);

            if (cursor != marker) {  // skip on first pass only
                bslalg::BidirectionalLink *scanner = cursor;
                while (scanner != marker &&
                       VALUE_POLICY::extractValue(scanner) == valueAtMarker) {
                    scanner = scanner->next();
                }
                if (scanner != marker) {
                    // We've seen this one before.
                    continue;
                }
            }

            SizeType matches = 0;
            for (bslalg::BidirectionalLink *scanner = rhsFirst;
                 scanner != rhsLast;
                 scanner = scanner->next())
            {
                if (VALUE_POLICY::extractValue(scanner) == valueAtMarker) {
                    ++matches;
                }
            }
            if (!matches) {
                return false;                                         // RETURN
            }

            // Remember, *scanner is by definition a good match
            for (bslalg::BidirectionalLink *scanner = marker->next();
                 scanner != endRange;
                 scanner = scanner->next())
            {
                if (VALUE_POLICY::extractValue(scanner) == valueAtMarker) {
                    if (!--matches) {  // equal matches, but excluding initial
                        return false;                                 // RETURN
                    }
                }
            }
            if (1 != matches) {
                return false;                                         // RETURN
            }
        }

        cursor = endRange;
    }

    return true;
}


}  // close namespace BloombergLP::bslstl

//----------------------------------------------------------------------------
//                  free functions and opterators
//----------------------------------------------------------------------------

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
void
bslstl::swap(bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>& x,
             bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>& y)
{
    x.swap(y);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bool
bslstl::operator==(
                const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>& lhs,
                const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>& rhs)
{
    return lhs.hasSameValue(rhs);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOC>
inline
bool
bslstl::operator!=(
                const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>& lhs,
                const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOC>& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
