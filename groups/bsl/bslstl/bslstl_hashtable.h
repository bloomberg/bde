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

#ifndef INCLUDED_BSLSTL_ALLOCATORTRAITS
#include <bslstl_allocatortraits.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#include <bslstl_bidirectionalnodepool.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLALG_FUNCTORADAPTER
#include <bslalg_functoradapter.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#include <bslalg_hashtableimputil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>  // for swap
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CMATH
#include <cmath> // for 'ceil'
#define INCLUDED_CMATH
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

namespace bslstl {

                           // ===============
                           // class HashTable
                           // ===============

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
class HashTable_Parameters : private bslalg::FunctorAdapter<HASH>::Type
                           , private bslalg::FunctorAdapter<EQUAL>::Type
{
    // This class holds all the parameterized parts of a 'HashTable' class,
    // efficiently exploiting the empty base optimization without adding
    // unforeseen assocaitions to the 'HashTable' class itself due to the
    // structural inheritance.
  public:
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename KEY_POLICY::KeyType           KeyType;
    typedef typename KEY_POLICY::ValueType         ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;
    typedef typename AllocatorTraits::size_type    SizeType;

  private:
    // PRIVATE TYPES
    typedef typename AllocatorTraits::template
                         rebind_traits<NodeType>::allocator_type NodeAllocator;

    // These two aliases simplify naming the base classes in the constructor
    typedef typename bslalg::FunctorAdapter<HASH>::Type  HasherBaseType;
    typedef typename bslalg::FunctorAdapter<EQUAL>::Type ComparatorBaseType;

  public:
    typedef BidirectionalNodePool<ValueType, NodeAllocator>      NodeFactory;

  private:
    // DATA
    NodeFactory  d_nodeFactory;

  private:
    // NOT IMPLEMENTED
    HashTable_Parameters(const HashTable_Parameters&); // = delete;
    HashTable_Parameters& operator=(const HashTable_Parameters&); // = delete;

  public:
    // CREATORS
    HashTable_Parameters(const HASH&          hash,
                         const EQUAL&         compare,
                         const AllocatorType& allocator);

    HashTable_Parameters(const HashTable_Parameters& other, 
                         const AllocatorType&        allocator);

    // MANIPULATORS
    NodeFactory& nodeFactory();

    void swap(HashTable_Parameters& other);

    // OBSERVERS
    const HASH&        hasher()      const;
    const EQUAL&       comparator()  const;
    const NodeFactory& nodeFactory() const;
};

                           // ===============
                           // class HashTable
                           // ===============

// No need for any defaults below, this imp.-detail component is intended to be
// instantiated and used from the public interface of a std container that
// provides all the user-friendly defaults, and explicitly pass down what is
// needed.
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
class HashTable {
  private:
    // PRIVATE TYPES
    typedef HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>
                                                                ImplParameters;
  public:
    typedef typename ImplParameters::KeyType   KeyType;
    typedef typename ImplParameters::ValueType ValueType;
    typedef typename ImplParameters::NodeType  NodeType;
    typedef typename ImplParameters::SizeType  SizeType;
  private:
    // DATA
    ImplParameters          d_parameters;
    bslalg::HashTableAnchor d_anchor;
    SizeType                d_size;
    size_t                  d_capacity; // max number of elements before 
                                        // rehash is required (computed from
                                        // 'd_maxLoadFactor')
    
    double                  d_maxLoadFactor; // maximum load factor

  private:
    // TBD implement assignment
    HashTable& operator=(const HashTable&);

  private:
    // PRIVATE MANIPULATORS
    void removeAllImp();
        // Erase all the nodes in this table and deallocate their memory via
        // the node factory, without performing the necessary bookkeeping to
        // reflect such change.
    
    void removeAllAndDeallocate();
        // Erase all the nodes in this table, and deallocate their memory via
        // the supplied node factory. Destroy the arry of bucket of this table.

    // PRIVATE ACCESSORS
    native_std::size_t hashCodeForNode(bslalg::BidirectionalLink *node) const;

    bslalg::BidirectionalLink *find(const KeyType&     key, 
                                    native_std::size_t hashValue) const;
        // Find ...... The behavior is undefined unless 
        // 'this->hasher()(key) == hashValue)'
   
    bslalg::HashTableBucket *getBucketAddress(SizeType bucketIndex) const;
        // The behavior is undefined unles 'this->numBuckets()  >=
        // bucketIndex'.

  public:
    // CREATORS
    HashTable(const HASH&      hash,
              const EQUAL&     compare,
              SizeType         initialBucketCount,
              const ALLOCATOR& allocator = ALLOCATOR());
        // Behavior is undefined unless '0 < intialBucketCount'.

    HashTable(const HashTable& other,
              const ALLOCATOR& allocator = ALLOCATOR());
        // Copy the specified 'other' using the specified allocator 'a'.

    ~HashTable();

    // ACCESSORS
    ALLOCATOR allocator() const;
        // TBD..

    bool isEmpty() const;
        // TBD..

    SizeType size() const;
        // TBD..

    SizeType maxSize() const;
        // TBD..

    bslalg::BidirectionalLink *elementListRoot() const;

    // MANIPULATORS
    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertIfMissing
                                           (bool               *isInsertedFlag,
                                            const SOURCE_TYPE&  obj);
        // ... and load true into the specified 'isInsertedFlag' if the
        // insertion is performed, and false otherwise.    
    
    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertContiguous(const SOURCE_TYPE& obj);

    bslalg::BidirectionalLink *remove(bslalg::BidirectionalLink *node);

    bslalg::BidirectionalLink *findOrInsertDefault(const KeyType& key);
    
    void rehashForNumBuckets(SizeType newNumBuckets);
    void rehashForNumElements(SizeType n);
    
    void removeAll();

    void setMaxLoadFactor(float loadFactor);
    void swap(HashTable& other);


    // ACCESSORS
    SizeType bucketIndexForKey(const KeyType& key) const;
        // Return the index of the bucket that contains all the elements having
        // the specified 'key'.

    const bslalg::HashTableBucket& bucketAtIndex(SizeType index) const;
        // Return a reference to the 'n'th non-modifiable bucket in the
        // sequence of buckets.  The behavior is undefined unless 'n < TBD'.
    
    const EQUAL& comparator() const;
    
    bslalg::BidirectionalLink *find(const KeyType& key) const;
        // Return the first link of the contiguous list of links containing the
        // elements of this table having the same specified 'key'. 

    void findRange(bslalg::BidirectionalLink **first,
                   bslalg::BidirectionalLink **last,
                   const KeyType&              k) const;
    
    const HASH& hasher()     const;
   

    bslalg::BidirectionalLink *findEndOfRange(
                                       bslalg::BidirectionalLink *first) const;
        // Return the address of the first node in the total sequence of the
        // hash table following the specified 'first' node that holds a value
        // with a key that does not have the same value as the key stored in
        // the node pointed to by 'first'.

    SizeType maxNumBuckets() const;
        // Return the maximum number of buckets that can be contained in this
        // hash table.
    
    SizeType numBuckets() const;
        // Return the number of buckets contained in this hash table.
   
    SizeType countElementsInBucket(SizeType index) const;
        // Return the number elements contained in the bucket at the specified
        // index 'index'.

    float loadFactor() const;
    
    float maxLoadFactor() const;
    
};

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOCATOR>
void swap(HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>& x,
          HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>& y);

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOCATOR>
bool operator==(const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>& lhs,
                const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>& rhs);

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOCATOR>
bool operator!=(const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>& lhs,
                const HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>& rhs);

// Move this to its own component, and avoid 'iterator' dependency
// This is used only in higher level components anyway - does not belong.
// Generally useful for all container implementations, probably already exists
// somewhere in bslalg.
                    
                    // ========================
                    // class HashTable_IterUtil
                    // ========================

struct HashTable_IterUtil {

    // generic utility that needs a non-template hosted home
    template <class InputIterator>
    static native_std::size_t insertDistance(InputIterator first,
                                             InputIterator last);
        // Return 0 if InputIterator really is limitted to the standard
        // input-iterator category, otherwise return the distance from first
        // to last.
};

                    // ============================
                    // class HashTable_StaticBucket
                    // ============================

struct HashTable_StaticBucket {
  public:
    // CLASS DATA
    //static bslalg::HashTableBucket s_bucket;

    static bslalg::HashTableBucket *getDefaultBucketAddress();
    // TBD add public method to return the address and hide the variable in
    // the cpp
};

                    // ====================
                    // class HashTable_Util
                    // ====================

template<class ALLOCATOR>
struct HashTable_Util {
  private:
    // PRIVATE TYPES
    typedef typename ::bsl::allocator_traits<ALLOCATOR>::template
                         rebind_traits<bslalg::HashTableBucket>::allocator_type
                                                                ArrayAllocator;
    typedef ::bsl::allocator_traits<ArrayAllocator> ArrayAllocatorTraits;

  public:
    // PUBLIC TYPES
    typedef typename ArrayAllocatorTraits::size_type SizeType;

    // CLASS METHODS
    static void initAnchor(bslalg::HashTableAnchor *anchor,
                           SizeType                 size,
                           const ALLOCATOR&         allocator);
        // Create an array of the specified 'size' of the parameterized type
        // 'bslalg::HashTableBucket', using the default constructor of 'TYPE'
        // to initialize the individual elements in the array. (TBD does not
        // affect the listRoot)

    static void destroyBucketArray(bslalg::HashTableBucket *data,
                                   SizeType                 size,
                                   const ALLOCATOR&         allocator);
};

                    // =========================
                    // class HashTable_PrimeUtil
                    // =========================

struct HashTable_PrimeUtil {
    static size_t nextPrime(size_t n);
};

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // --------------------------
                    // class HashTable_Parameters
                    // --------------------------

// CREATORS
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable_Parameters(const HASH&          hash,
                     const EQUAL&         compare,
                     const AllocatorType& allocator)
: HasherBaseType(hash)
, ComparatorBaseType(compare)
, d_nodeFactory(allocator)
{
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable_Parameters(const HashTable_Parameters& other, 
                     const AllocatorType&        allocator)
: HasherBaseType(static_cast<const HasherBaseType&>(other))
, ComparatorBaseType(static_cast<const ComparatorBaseType&>(other))
, d_nodeFactory(allocator)
{
}

// MANIPULATORS
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::NodeFactory&
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::nodeFactory()
{
    return d_nodeFactory;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::
swap(HashTable_Parameters& other)
{
    using native_std::swap;    // otherwise it is hidden by this very definition!

    bslalg::SwapUtil::swap(
                     static_cast<HasherBaseType*>(this),
                     static_cast<HasherBaseType*>(BSLS_UTIL_ADDRESSOF(other)));

    bslalg::SwapUtil::swap(
                 static_cast<ComparatorBaseType*>(this),
                 static_cast<ComparatorBaseType*>(BSLS_UTIL_ADDRESSOF(other)));

    swap(d_nodeFactory, other.d_nodeFactory);
}


// OBSERVERS
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const HASH&
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::hasher() const
{
    return *this;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const EQUAL&
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::comparator() const
{
    return *this;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const typename HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::
                                                                   NodeFactory&
HashTable_Parameters<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::nodeFactory() const
{
    return d_nodeFactory;
}

                    // --------------------
                    // class HashTable_Util
                    // --------------------

template <class ALLOCATOR>
inline
void HashTable_Util<ALLOCATOR>::initAnchor(bslalg::HashTableAnchor *anchor,
                                           SizeType                size,
                                           const ALLOCATOR&        allocator)
{
    BSLS_ASSERT_SAFE(anchor);
    BSLS_ASSERT_SAFE(0 != size);

    using bslalg::HashTableBucket;

    ArrayAllocator reboundAllocator(allocator);

    HashTableBucket *data =
                        ArrayAllocatorTraits::allocate(reboundAllocator, size);

    native_std::fill_n(data, size, HashTableBucket());

    anchor->setBucketArrayAddressAndSize(data, size);
}

template <class ALLOCATOR>
inline
void HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                           bslalg::HashTableBucket  *data,
                                           SizeType                  size,
                                           const ALLOCATOR&          allocator)
{
    BSLS_ASSERT_SAFE(data);
    BSLS_ASSERT_SAFE(
    (1  < size && HashTable_StaticBucket::getDefaultBucketAddress() != data) ||
    (1 == size && HashTable_StaticBucket::getDefaultBucketAddress() == data));

    if (HashTable_StaticBucket::getDefaultBucketAddress() != data) {
        ArrayAllocator reboundAllocator(allocator);
        ArrayAllocatorTraits::deallocate(reboundAllocator, data, size);
    }
}

                    // ------------------------
                    // class HashTable_IterUtil
                    // ------------------------

template <class InputIterator>
native_std::size_t HashTable_IterUtil::insertDistance(InputIterator first,
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
        static native_std::size_t calc(InputIterator, // first
                                       InputIterator, // last
                                       native_std::input_iterator_tag)
        {
            return 0;
        }

        static native_std::size_t calc(InputIterator first,
                                       InputIterator last,
                                       native_std::forward_iterator_tag)
        {
            return native_std::distance(first, last);
        }
    };

    typedef typename
                  native_std::iterator_traits<InputIterator>::iterator_category
                                                                  IterCategory;
    return impl::calc(first, last, IterCategory());
#endif
}
                        //----------------
                        // class HashTable
                        //----------------

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
native_std::size_t
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::hashCodeForNode(
                                         bslalg::BidirectionalLink *node) const
{
    BSLS_ASSERT_SAFE(node);
    
    const typename KEY_POLICY::KeyType& key = 
                        bslalg::HashTableImpUtil::extractKey<KEY_POLICY>(node);
    return hasher()(key);
}

// CREATORS
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable(const HASH&      hash,
          const EQUAL&     compare,
          SizeType         initialBucketCount,
          const ALLOCATOR& allocator)
: d_parameters(hash, compare, allocator)
, d_anchor(HashTable_StaticBucket::getDefaultBucketAddress(), 1, 0)
, d_size()
, d_capacity(initialBucketCount)
, d_maxLoadFactor(1.0)
{
    if (0 != initialBucketCount) {
        HashTable_Util<ALLOCATOR>::initAnchor(&d_anchor,
                                              initialBucketCount,
                                              allocator);
    }
}

template <class FACTORY>
class HashTable_ListProctor {
  private:
    // DATA
    FACTORY                   *d_factory;
    bslalg::BidirectionalLink *d_root;

  public:
    HashTable_ListProctor(FACTORY                   *factory,
                          bslalg::BidirectionalLink *listRoot);

    ~HashTable_ListProctor();

    void dismiss();
};

template <class ALLOCATOR>
class HashTable_ArrayProctor {
    // This class probably already exists in 'bslalg'
  private:
    // DATA
    ALLOCATOR                      d_allocator;
    bslalg::HashTableBucket       *d_array;
    typename ALLOCATOR::size_type  d_size;

  public:
    HashTable_ArrayProctor(const ALLOCATOR&               allocator,
                          bslalg::HashTableBucket       *array,
                          typename ALLOCATOR::size_type  size);

    ~HashTable_ArrayProctor();

    void dismiss();
};


template <class FACTORY>
inline
HashTable_ListProctor<FACTORY>::HashTable_ListProctor(
                                           FACTORY                   *factory,
                                           bslalg::BidirectionalLink *listRoot)
: d_factory(factory)
, d_root(listRoot)
{
    BSLS_ASSERT_SAFE(factory);
}

template <class FACTORY>
HashTable_ListProctor<FACTORY>::~HashTable_ListProctor()
{
    while (d_root) {
        bslalg::BidirectionalLink *next = d_root->nextLink();
        d_factory->deleteNode(d_root);
        d_root = next;
    }
}

template <class FACTORY>
inline
void HashTable_ListProctor<FACTORY>::dismiss()
{
    d_root = 0;
}


template <class ALLOCATOR>
inline
HashTable_ArrayProctor<ALLOCATOR>::HashTable_ArrayProctor(
                                      const ALLOCATOR&               allocator,
                                      bslalg::HashTableBucket       *array,
                                      typename ALLOCATOR::size_type  size)
: d_allocator(allocator)
, d_array(array)
, d_size(size)
{
}

template <class ALLOCATOR>
HashTable_ArrayProctor<ALLOCATOR>::~HashTable_ArrayProctor()
{
    if (d_array) {
        HashTable_Util<ALLOCATOR>::destroyBucketArray(d_array,
                                                      d_size,
                                                      d_allocator);
    }
}

template <class ALLOCATOR>
inline
void HashTable_ArrayProctor<ALLOCATOR>::dismiss()
{
    d_array = 0;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable(const HashTable& other, const ALLOCATOR& allocator)
: d_parameters(other.d_parameters, allocator)
, d_anchor(HashTable_StaticBucket::getDefaultBucketAddress(), 1, 0)
, d_size(other.d_size)
, d_capacity(0)
, d_maxLoadFactor(other.d_maxLoadFactor)
{
    if (0 == d_size) {
        return;                                                       // RETURN
    }

    // Allocate an appropriate number of buckets
    SizeType numBuckets = HashTable_PrimeUtil::nextPrime(
                             native_std::ceil(d_size / this->d_maxLoadFactor));

    HashTable_Util<ALLOCATOR>::initAnchor(&d_anchor, numBuckets, allocator);
    d_capacity = numBuckets * this->d_maxLoadFactor;
        
    // create a proctor for d_anchor's allocated array
    HashTable_ArrayProctor<ALLOCATOR> arrayProctor(allocator,
                                                 d_anchor.bucketArrayAddress(),
                                                   d_anchor.bucketArraySize());

    const bslalg::BidirectionalLink *cursor = other.d_anchor.listRootAddress();

    bslalg::BidirectionalLink *newNode =
                                d_parameters.nodeFactory().createNode(*cursor);
    newNode->reset();
    d_anchor.setListRootAddress(newNode);

    // create a proctor for the root of the list
    HashTable_ListProctor<typename ImplParameters::NodeFactory>
                             listProctor(&d_parameters.nodeFactory(), newNode);

    // After this point, no operations can throw
    SizeType curBucketIndex = bslalg::HashTableImpUtil::computeBucketIndex(
                                                      hashCodeForNode(newNode),
                                                      numBuckets);

    bslalg::HashTableBucket *curBucket =
                                        this->getBucketAddress(curBucketIndex);

    curBucket->setFirstAndLast(newNode, newNode);
    SizeType prevBucketNumber = curBucketIndex;
    bslalg::BidirectionalLink *prevNode = newNode;

    while ((cursor = cursor->nextLink())) {
        newNode = d_parameters.nodeFactory().createNode(*cursor);

        // Thread the node into the list so the proctor can clean up if the
        // user-supplied hash functor throws.
        prevNode->setNextLink(newNode);
        newNode->setPreviousLink(prevNode);
        newNode->setNextLink(0); // could defer this to one set at the end
                                 // of the list copy, but want a 'safe'
                                 // terminator in case an exception causes us
                                 // to free the whole list

        curBucketIndex = bslalg::HashTableImpUtil::computeBucketIndex(
                                                      hashCodeForNode(newNode),
                                                      numBuckets);

        if (curBucketIndex != prevBucketNumber) {
            curBucket->setLast(prevNode);
            curBucket = this->getBucketAddress(curBucketIndex);
            curBucket->setFirstAndLast(newNode, newNode);
            prevBucketNumber = curBucketIndex;
        }
        prevNode = newNode;
    }

    curBucket->setLast(prevNode);
        
    // release the proctors
    listProctor.dismiss();
    arrayProctor.dismiss();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::~HashTable()
{
    // This is overly expensive, as we need only reclaim memory, and need not
    // update bucket indices and other sentries.  We might also want to reclaim
    // any leading sentinel node.
    // We can factor 'clear' into a two-part implementation later.
    // Note that the 'vector' member will reclaim its own memory, so nothing
    // additional beyond clear needed there.
    this->removeAllAndDeallocate();
}

// PRIVATE MANIPULATORS
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
void
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::removeAllImp()
{
    using bslalg::BidirectionalLink;
    using bslalg::HashTableBucket;

    // Doing too much book-keeping of hash table - look for a more efficient
    // dispose-as-we-walk, that simply resets table.Anchor.next = 0, and
    // assigns the buckets index all null pointers
    if (BidirectionalLink *root = d_anchor.listRootAddress()) {
        BidirectionalLink *next;
        do {
            next = root->nextLink();
            d_parameters.nodeFactory().deleteNode((NodeType *)root);
        }
        while((root = next));
    }
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::isEmpty() const
{
    return !d_size;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_size;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::maxSize() const
{
    return native_std::numeric_limits<SizeType>::max();
}
 
    // iterators
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::elementListRoot() const
{
    return d_anchor.listRootAddress();
}

// PRIVATE ACCESSORS
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::HashTableBucket *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::getBucketAddress(
                                                    SizeType bucketIndex) const
{
    BSLS_ASSERT_SAFE(bucketIndex < this->numBuckets());

    return d_anchor.bucketArrayAddress() + bucketIndex;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::find(
                                                 const KeyType& key, 
                                                 size_t         hashValue) const
{
    return bslalg::HashTableImpUtil::find<KEY_POLICY>(d_anchor,
                                                      this->comparator(),
                                                      key,
                                                      hashValue);
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::findOrInsertDefault(
                                                            const KeyType& key)
{
    size_t hashCode = this->hasher()(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);
    if (!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() + 1);
        }

        position = d_parameters.nodeFactory().createNode(
                            ValueType(key, typename ValueType::second_type()));
    
        
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
        ++d_size;
    }
    return position;
}

// MANIPULATORS 
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::insertIfMissing(
                                               bool               *isInserted,
                                               const SOURCE_TYPE&  obj)
{
    const KeyType& key = KEY_POLICY::extractKey(obj);
    size_t hashCode = this->hasher()(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);

    *isInserted = (!position);

    if(!position) {
        if (d_size >= d_capacity) {
            this->rehashForNumBuckets(numBuckets() + 1);
        }

        position = d_parameters.nodeFactory().createNode(obj);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
        ++d_size;
    }

    return position;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::insertContiguous(
                                                       const SOURCE_TYPE&  obj)
{
    const KeyType& key = KEY_POLICY::extractKey(obj);
    size_t hashCode = this->hasher()(key);
    bslalg::BidirectionalLink *position = this->find(key, hashCode);

    if (d_size >= d_capacity) {
        this->rehashForNumBuckets(numBuckets() + 1);
    }

    if(!position) {
        position = d_parameters.nodeFactory().createNode(obj);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                                        position,
                                                        hashCode);
    }
    else {
        bslalg::BidirectionalLink *newNode =
                                    d_parameters.nodeFactory().createNode(obj);
        bslalg::HashTableImpUtil::insertDuplicateAtPosition(&d_anchor,
                                                            newNode,
                                                            hashCode,
                                                            position);
        position = newNode;
    }

    ++d_size;

    return position;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::removeAllAndDeallocate()
{
    this->removeAllImp();
    HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                       d_anchor.bucketArrayAddress(),
                                       d_anchor.bucketArraySize(),
                                       this->allocator());
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::remove(
                                               bslalg::BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->previousLink()
                  || d_anchor.listRootAddress() == node);

    bslalg::HashTableImpUtil::removeNode(&d_anchor,
                                         node,
                                         hashCodeForNode(node));
    --d_size;

    bslalg::BidirectionalLink *result = node->nextLink();
    d_parameters.nodeFactory().deleteNode((NodeType *)node);

    return result;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
void
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::removeAll()
{
    this->removeAllImp();
    native_std::memset(d_anchor.bucketArrayAddress(),
                0,
                sizeof(bslalg::HashTableBucket) * d_anchor.bucketArraySize());

    d_anchor.setListRootAddress(0);
    d_size = 0;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
void
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::swap(HashTable& other)
{
    // assert that allocators are compatible
    // TBD: We do not yet support allocator propagation by allocator traits
    BSLS_ASSERT(this->allocator() == other.allocator());

    d_parameters.swap(other.d_parameters);

    using native_std::swap;  // otherwise it is hidden by this very definition!

    swap(d_anchor,        other.d_anchor);
    swap(d_size,          other.d_size);
    swap(d_capacity,      other.d_capacity);
    swap(d_maxLoadFactor, other.d_maxLoadFactor);
}

// observers
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const HASH& HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::hasher() const
{
    return d_parameters.hasher();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const EQUAL& HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::comparator() const
{
    return d_parameters.comparator();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
ALLOCATOR HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::allocator() const
{
    return d_parameters.nodeFactory().allocator();
}

// lookup
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::find(const KeyType& key) const
{
    return bslalg::HashTableImpUtil::find<KEY_POLICY>(d_anchor,
                                                      this->comparator(),
                                                      key,
                                                      this->hasher()(key));
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable< KEY_POLICY, HASH, EQUAL, ALLOCATOR>::findRange(
                                           bslalg::BidirectionalLink **first,
                                           bslalg::BidirectionalLink **last,
                                           const KeyType&              k) const
{
    BSLS_ASSERT_SAFE(first);
    BSLS_ASSERT_SAFE(last);

    *last = (*first = this->find(k))
          ? this->findEndOfRange(*first)
          : 0;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::findEndOfRange(
                                        bslalg::BidirectionalLink *first) const
{
    BSLS_ASSERT_SAFE(first);

    const KeyType& k = bslalg::HashTableImpUtil::extractKey<KEY_POLICY>(first);
    while ((first = first->nextLink()) &&
           this->comparator()(k,
                              bslalg::HashTableImpUtil::extractKey<KEY_POLICY>
                                                                      (first)))
    {
    }
    return first;
}

// bucket interface
template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::numBuckets() const
{
    return d_anchor.bucketArraySize();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::maxNumBuckets() const
{
    return this->max_size();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::countElementsInBucket(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());
    return bucketAtIndex(index).countElements();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::bucketIndexForKey(
                                                      const KeyType& key) const
{
    size_t hashCode = this->hasher()(key);
    return bslalg::HashTableImpUtil::computeBucketIndex(
                                                   hashCode,
                                                   d_anchor.bucketArraySize());
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const bslalg::HashTableBucket&
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::bucketAtIndex(SizeType index)
                                                                          const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return d_anchor.bucketArrayAddress()[index];
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
float HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::loadFactor() const
{
    return (double)size() / this->numBuckets();
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
float HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::maxLoadFactor() const
{
    return d_maxLoadFactor;
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::setMaxLoadFactor(
                                                              float loadFactor)
{
    d_maxLoadFactor = loadFactor;
    d_capacity = this->numBuckets() * loadFactor;

    if (d_capacity < this->size()) {
        this->rehashForNumElements(this->size());
    }
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::rehashForNumBuckets(
                                                        SizeType newNumBuckets)
{
    // compute a "good" number of buckets, e.g., pick a prime number
    // from a sorted array of exponentially increasing primes.
    newNumBuckets = HashTable_PrimeUtil::nextPrime(newNumBuckets);

    if (newNumBuckets > this->numBuckets()) {

        // Now that 'anchor' is not default constructible, we take a copy of
        // the anchor in the table.  Would it be better for 'initAnchor' to
        // be replaced with a 'createArrayOfEmptyBuckets' function, and we use
        // the result to construct the 'newAnchor'?
        bslalg::HashTableAnchor newAnchor = d_anchor;
        HashTable_Util<ALLOCATOR>::initAnchor(&newAnchor,
                                              newNumBuckets,
                                              this->allocator());

        if (d_anchor.listRootAddress()) {
            bslalg::HashTableImpUtil::rehash<KEY_POLICY>(
                                                    &newAnchor,
                                                    d_anchor.listRootAddress(),
                                                    hasher());
        }
        d_anchor.swap(newAnchor);
        d_capacity = newNumBuckets * this->maxLoadFactor();
        HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                                newAnchor.bucketArrayAddress(),
                                                newAnchor.bucketArraySize(),
                                                this->allocator());
    }
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>::rehashForNumElements(SizeType n)
{
    this->rehashForNumBuckets(native_std::ceil(n / this->maxLoadFactor()));
}

}  // close namespace BloombergLP::bslstl

//----------------------------------------------------------------------------
//                  free functions and opterators
//----------------------------------------------------------------------------

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
bslstl::swap(bslstl::HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>& x,
             bslstl::HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>& y)
{
    x.swap(y);
}

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
bool
bslstl::operator==(
            const bslstl::HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>& lhs,
            const bslstl::HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    // The template bloat of this function can be significantly reduced
    // What matters is that the two hash tables:
    // i/   are the same size
    // ii/  have lists that are permutations of each other according to the
    //      element's 'operator=='
    // This means that the implementation should be independant of all four
    // template parameters, but will depend on VALUE_TYPE deduced from the
    // KEY_POLICY.  Otherwise, after the initial size comparison, the rest
    // depends only on the anchors.
    typedef typename KEY_POLICY::KeyType   KeyType;
    typedef typename KEY_POLICY::ValueType ValueType;
    typedef typename ::bsl::allocator_traits<ALLOCATOR>::size_type SizeType;
    typedef bslalg::HashTableImpUtil ImpUtil;


    // First test - are the containers the same size?
    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }
    bslalg::BidirectionalLink *cursor = lhs.elementListRoot();
    if (!cursor) {
        // containers are the same size, and empty.
        return true;                                                  // RETURN
    }

    while (cursor) {
        bslalg::BidirectionalLink *rhsFirst =
                             rhs.find(ImpUtil::extractKey<KEY_POLICY>(cursor));
        if (!rhsFirst) {
            return false;  // no matching key                         // RETURN
        }

        bslalg::BidirectionalLink *endRange = lhs.findEndOfRange(cursor);
        bslalg::BidirectionalLink *rhsLast  = rhs.findEndOfRange(rhsFirst);

        // Check the key-groups have the same length - anrhs quick-fail test.
        bslalg::BidirectionalLink *endWalker = cursor->nextLink();
        bslalg::BidirectionalLink *rhsWalker = rhsFirst->nextLink();


        while (endWalker != endRange) {


            if (rhsWalker == rhsLast) {
                return false;   // different length subsequences      // RETURN
            }
            endWalker = endWalker->nextLink();
            rhsWalker = rhsWalker->nextLink();
        }

        if (rhsWalker != rhsLast) {
            return false;   // different length subsequences          // RETURN
        }

        // Efficiently compare identical prefixes: O(N) if sequences
        // have the same elements in the same order.  Note that comparison of
        // values in nodes is tested using 'operator==' and not the
        // key-equality comparator stored in the hash table.
        while (cursor != endRange &&
                 (ImpUtil::extractValue<KEY_POLICY>(cursor) ==
                  ImpUtil::extractValue<KEY_POLICY>(rhsFirst)))
        {
            cursor   = cursor->nextLink();
            rhsFirst = rhsFirst->nextLink();
        }

        if (cursor == endRange) {
            continue;
        }


        // Now comes the harder part of validating one subsequence is a
        // permutation of another, by counting elements that compare equal
        // using the equality operator, 'operator=='.
        // Note that lhs code could be simplified for hash-tables with unique
        // keys, as we can omit the counting-scan, and merely test for any
        // match within the rhs range.  Trade off the ease of a single
        // well-tested code path, vs. the importance of an efficient operator==
        // for hash containers.  lhs is currently the only place the table
        // would care about uniqueness, and risk different table types for
        // unique vs. multi-containers.  Note again that comparison of values
        // in nodes is tested using 'operator==' and not the key-equality
        // comparator stored in the hash tables.
        for (bslalg::BidirectionalLink *marker = cursor;
             marker != endRange;
             marker = marker->nextLink())
        {
            const ValueType& valueAtMarker =
                                    ImpUtil::extractValue<KEY_POLICY>(marker);

            if (cursor != marker) {  // skip on first pass only
                bslalg::BidirectionalLink *scanner = cursor;
                while (scanner != marker &&
                   ImpUtil::extractValue<KEY_POLICY>(scanner) == valueAtMarker)
                                                                              {
                    scanner = scanner->nextLink();
                }
                if (scanner != marker) {
                    // We've seen lhs one before.
                    continue;
                }
            }

            SizeType matches = 0;
            for (bslalg::BidirectionalLink *scanner = rhsFirst;
                 scanner != rhsLast;
                 scanner = scanner->nextLink()) {
                if (ImpUtil::extractValue<KEY_POLICY>(scanner) == 
                                                               valueAtMarker) {
                    ++matches;
                }
            }
            if (!matches) {
                return false;                                         // RETURN
            }

            // Remember, *scanner is by definition a good match
            for (bslalg::BidirectionalLink *scanner = marker->nextLink();
                 scanner != endRange;
                 scanner = scanner->nextLink()) {

                if (ImpUtil::extractValue<KEY_POLICY>(scanner) == 
                                                               valueAtMarker) {
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

template <class KEY_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
bslstl::operator!=(
            const bslstl::HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>& lhs,
            const bslstl::HashTable<KEY_POLICY, HASH, EQUAL, ALLOCATOR>& rhs)
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
