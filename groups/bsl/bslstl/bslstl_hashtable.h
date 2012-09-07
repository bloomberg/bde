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

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#include <bslstl_bidirectionalnodepool.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
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
#include <iostream>

#ifndef INCLUDED_ITERATOR
#include <iterator>  // for tag dispatch on iterator traits
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_LIMITS
#include <limits>  // for 'numeric_limits<size_t>'
#define INCLUDED_LIMITS
#endif

namespace BloombergLP {

namespace bslalg { class BidirectionalLink; }

namespace bslstl {

template<class VALUE_TYPE>
struct HashTable_NodeUtil;

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
class HashTable_Parameters : private bslalg::FunctorAdapter<HASH>::Type
                           , private bslalg::FunctorAdapter<EQUAL>::Type
{
    // This class holds all the parameterized parts of a 'HashTable' class,
    // efficiently exploiting the empty base optimization without assing
    // unforeseen assocaitions to the 'HashTable' class itself due to the
    // structural inheritance.

  public:
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename VALUE_POLICY::KeyType         KeyType;
    typedef typename VALUE_POLICY::ValueType       ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;
    typedef typename AllocatorTraits::size_type    SizeType;

  private:
    // PRIVATE TYPES
    typedef typename AllocatorTraits::template
                         rebind_traits<NodeType>::allocator_type NodeAllocator;

    typedef BidirectionalNodePool<ValueType, NodeAllocator>      NodeFactory;

    // These two aliases simplify naming the base classes in the constructor
    typedef typename bslalg::FunctorAdapter<HASH>::Type  HasherBaseType;
    typedef typename bslalg::FunctorAdapter<EQUAL>::Type ComparatorBaseType;

  private:
    // DATA
    NodeFactory  d_nodeFactory;

  public:
    // CREATORS
    HashTable_Parameters(const HASH&          hash,
                         const EQUAL&         compare,
                         const AllocatorType& allocator);

    // MANIPULATORS
    NodeFactory& nodeFactory();

    void swap(HashTable_Parameters& other);

    // ACCESSORS
    AllocatorType allocator() const;

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
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
// TBD look at bslstl_map, add the allocator to the EBO
class HashTable //: private bslalg::FunctorAdapter<HASH>::Type
                //, private bslalg::FunctorAdapter<EQUAL>::Type
{
  public:
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename VALUE_POLICY::KeyType         KeyType;
    typedef typename VALUE_POLICY::ValueType       ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;
    typedef typename AllocatorTraits::size_type    SizeType;
  private:
    // PRIVATE TYPES
    typedef typename AllocatorTraits::template
                         rebind_traits<NodeType>::allocator_type NodeAllocator;

    typedef BidirectionalNodePool<ValueType, NodeAllocator>      NodeFactory;

    typedef HashTable_NodeUtil<ValueType>                       LinkTranslator;

    typedef HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>
                                                                ImplParameters;
  private:
    // DATA
    ImplParameters          d_parameters;
    bslalg::HashTableAnchor d_anchor;
    SizeType                d_size;
    size_t                  d_capacity;  // Rehash if d_data.size() > d_capacity.
    double                  d_maxLoadFactor; // preserve rounding in d_capacity.

  private:
    // TBD implement assignment
    HashTable& operator=(const HashTable&);

  private:
    size_t hashCodeForNode(bslalg::BidirectionalLink *node) const;

    bslalg::HashTableBucket *getBucketAddress(SizeType bucketNumber);

    void removeAllAndDeallocate();

    void expandTable();

  public:
    // CREATORS
    HashTable(const HASH&          hash,
              const EQUAL&         compare,
              SizeType             initialBucketCount,
              const AllocatorType& allocator = AllocatorType());
        // Behavior is undefined unless '0 < intialBucketCount'.

    HashTable(const HashTable& other,
              const AllocatorType& a = AllocatorType());
        // Copy the specified 'other' using the specified allocator 'a'.

    ~HashTable();

    // ACCESSORS
    AllocatorType allocator() const;
        // TBD..

    bool isEmpty() const;
        // TBD..

    SizeType size() const;
        // TBD..

    SizeType maxSize() const;
        // TBD..

    bslalg::BidirectionalLink *elementListRoot() const;

    // MANIPULATORS
    void removeAll();

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertIfMissing(bool               *isFoundFlag,
                                               const SOURCE_TYPE&  obj);
    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertContiguous(const SOURCE_TYPE&  obj);

    bslalg::BidirectionalLink *remove(bslalg::BidirectionalLink *node);

    bslalg::BidirectionalLink *findOrInsertDefault(const KeyType& k);

    void setMaxLoadFactor(float loadFactor);
    void swap(HashTable& other);

    void rehash(SizeType n);
    void reserve(SizeType n);  // maybe??

    // OBSERVERS
    const HASH& hasher()     const;
    const EQUAL& comparator() const;

    // ACCESSORS
    bslalg::BidirectionalLink *find(const KeyType& k) const;

    void findRange(bslalg::BidirectionalLink **first,
                   bslalg::BidirectionalLink **last,
                   const KeyType&              k) const;

    bslalg::BidirectionalLink *findEndOfRange(
                                       bslalg::BidirectionalLink *first) const;
        // Return the address of the first node in the total sequence of the
        // hash table following the specified 'first' node that holds a value
        // with a key that does not have the same value as the key stored in
        // the node pointed to by 'first'.

    SizeType numBuckets() const;
    SizeType maxNumBuckets() const;
    SizeType numElementsInBucket(SizeType n) const;

    SizeType bucketIndexForKey(const KeyType& k) const;

    const bslalg::HashTableBucket& bucketAtIndex(SizeType n) const;
        // Return a reference to the 'n'th non-modifiable bucket in the
        // sequence of buckets.  The behavior is undefined unless 'n < TBD'.

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
struct HashTable_IterUtil {

    // generic utility that needs a non-template hosted home
    template <class InputIterator>
    static native_std::size_t insertDistance(InputIterator first,
                                             InputIterator last);
        // Return 0 if InputIterator really is limitted to the standard
        // input-iterator category, otherwise return the distance from first
        // to last.
};

                    // =====================
                    // class HashTable_Util
                    // =====================

struct HashTable_StaticBucket
{
  public:
    // CLASS DATA
    static bslalg::HashTableBucket s_bucket;
};

// TBD movve after class definition
template<class ALLOCATOR>
struct HashTable_Util {
    // This class provides a mechanism for managing an in-place array of
    // elements of a templatized type 'bslalg::HashTableBucket'.  The contained
    // elements are stored contiguously in memory and are constructed using the
    // default constructor of type 'bslalg::HashTableBucket'.  The templatized
    // type 'TYPE' must have a public default constructor, and a public
    // destructor, but an array does not attempt to use the copy constructor or
    // assignment operator of type 'bslalg::HashTableBucket'.  Therefore a
    // 'HashTable_Util' cannot be copied or assigned, and individual elements
    // cannot be appended or removed.
  private:
    // PRIVATE TYPES
    typedef typename ::bsl::allocator_traits<ALLOCATOR>::template
                         rebind_traits<bslalg::HashTableBucket>::allocator_type
                                                                ArrayAllocator;
    typedef ::bsl::allocator_traits<ArrayAllocator> AllocatorTraits;

  public:
    // PUBLIC TYPES
    typedef typename AllocatorTraits::size_type SizeType;

    // CLASS METHODS
    static void initAnchor(bslalg::HashTableAnchor *anchor,
                           SizeType                 size,
                           const ALLOCATOR&         allocator);
        // Create an array of the specified 'size' of the parameterized type
        // 'bslalg::HashTableBucket', using the default constructor of 'TYPE'
        // to initialize the individual elements in the array.

    static void destroyBucketArray(bslalg::HashTableBucket *data,
                                   SizeType                 size,
                                   const ALLOCATOR&         allocator);
};

struct HashTable_PrimeUtil {
    static size_t nextPrime(size_t n);
};

template<class VALUE_TYPE>
struct HashTable_NodeUtil {
    static VALUE_TYPE& extractValue(bslalg::BidirectionalLink *node);

    template<class KEY_POLICY>
    static const typename KEY_POLICY::KeyType& extractKey(
                                              bslalg::BidirectionalLink *node);
};

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------
                    // class HashTable_NodeUtil
                    // ------------------------

template <class VALUE_TYPE>
inline
VALUE_TYPE&
HashTable_NodeUtil<VALUE_TYPE>::extractValue(bslalg::BidirectionalLink *node)
{
    return static_cast<bslalg::BidirectionalNode<VALUE_TYPE> *>(node)->value();
}

template <class VALUE_TYPE>
template<class KEY_POLICY>
inline
const typename KEY_POLICY::KeyType&
HashTable_NodeUtil<VALUE_TYPE>::extractKey(bslalg::BidirectionalLink *node)
{
    return KEY_POLICY::extractKey(extractValue(node));
}


                    // --------------------------
                    // class HashTable_Parameters
                    // --------------------------

// CREATORS
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable_Parameters(const HASH&          hash,
                     const EQUAL&         compare,
                     const AllocatorType& allocator)
: HasherBaseType(hash)
, ComparatorBaseType(compare)
, d_nodeFactory(allocator)
{
}

// MANIPULATORS
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::
                                                                   NodeFactory&
HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::nodeFactory()
{
    return d_nodeFactory;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::
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
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const HASH&
HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::hasher() const
{
    return *this;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const EQUAL&
HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::comparator() const
{
    return *this;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const typename HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::
                                                                   NodeFactory&
HashTable_Parameters<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::nodeFactory() const
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

    HashTableBucket *data = static_cast<HashTableBucket *>(0);
    data = AllocatorTraits::allocate(reboundAllocator, size);

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
    BSLS_ASSERT_SAFE((&HashTable_StaticBucket::s_bucket != data) && 1 < size
                     || 1 == size);


    if (&HashTable_StaticBucket::s_bucket != data) {
        ArrayAllocator reboundAllocator(allocator);
        AllocatorTraits::deallocate(reboundAllocator, data, size);
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

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
native_std::size_t
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::hashCodeForNode(
                                         bslalg::BidirectionalLink *node) const
{
    BSLS_ASSERT_SAFE(node);

    return hasher()(LinkTranslator::template extractKey<VALUE_POLICY>(node));
}

// CREATORS
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable(const HASH&          hash,
          const EQUAL&         compare,
          SizeType             initialBucketCount,
          const AllocatorType& allocator)
: d_parameters(hash, compare, allocator)
, d_anchor(&HashTable_StaticBucket::s_bucket, 1, 0)
, d_size()
, d_capacity(initialBucketCount)
, d_maxLoadFactor(1.0)
{
    HashTable_Util<AllocatorType>::initAnchor(&d_anchor,
                                              initialBucketCount,
                                              d_parameters.nodeFactory().allocator());
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::
HashTable(const HashTable& other, const AllocatorType& a)
: d_parameters(other.d_parameters)
, d_anchor(&HashTable_StaticBucket::s_bucket, 1, 0)
, d_size(other.d_size)
, d_capacity(other.d_capacity)
, d_maxLoadFactor(other.d_maxLoadFactor)
{
    if(0 != other.d_size) {
        HashTable_Util<ALLOCATOR>::initAnchor(&d_anchor,
                                              other.d_anchor.bucketArraySize(),
                                              a);

        bslalg::BidirectionalLink *prevNode = 0;
        SizeType prevBucketNumber = 0;
        try {
            for (const bslalg::BidirectionalLink *cursor =
                    other.d_anchor.listRootAddress();
                    cursor;
                    cursor = cursor->nextLink())
            {
                bslalg::BidirectionalLink *curNode =
                                             d_parameters.nodeFactory().createNode(*cursor);

                // After this point, no operations can throw
                SizeType curBucketNumber =
                    bslalg::HashTableImpUtil::computeBucketIndex(
                                                   hashCodeForNode(curNode),
                                                   d_anchor.bucketArraySize());

                bslalg::HashTableBucket *curBucket = this->getBucketAddress(
                                                              curBucketNumber);

                bslalg::HashTableBucket *lastBucket = this->getBucketAddress(
                                                             prevBucketNumber);

                // APPEND 'next' after 'prevNode', and update any buckets or
                // end-markers.

                if (!prevNode) {
                    curBucket->setFirstAndLast(curNode, curNode);
                    d_anchor.setListRootAddress(curNode);
                    prevBucketNumber = curBucketNumber;
                    curNode->reset();
                }
                else {
                    prevNode->setNextLink(curNode);
                    curNode->setPreviousLink(prevNode);
                    curNode->setNextLink(0);

                    if (curBucketNumber != prevBucketNumber) {
                        curBucket->setFirstAndLast(curNode, curNode);
                        lastBucket->setLast(prevNode);
                        prevBucketNumber = curBucketNumber;
                    }
                }
                prevNode = curNode;
            }
            getBucketAddress(prevBucketNumber)->setLast(prevNode);
        }
        catch(...) {
            // reclaim any allocated nodes, the vector will clean up
            // after itself.
            this->removeAllAndDeallocate();

            throw;
        }
    }
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::~HashTable()
{
    // This is overly expensive, as we need only reclaim memory, and need not
    // update bucket indices and other sentries.  We might also want to reclaim
    // any leading sentinel node.
    // We can factor 'clear' into a two-part implementation later.
    // Note that the 'vector' member will reclaim its own memory, so nothing
    // additional beyond clear needed there.
    this->removeAllAndDeallocate();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::isEmpty() const
{
    return !d_size;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::size() const
{
    return d_size;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::maxSize() const
{
    return native_std::numeric_limits<SizeType>::max();
}

    // iterators
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::elementListRoot() const
{
    return d_anchor.listRootAddress();
}

// PRIVATE ACCESSORS
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::HashTableBucket *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::getBucketAddress(
                                                         SizeType bucketNumber)
{
    BSLS_ASSERT_SAFE(bucketNumber < this->numBuckets());

    return d_anchor.bucketArrayAddress() + bucketNumber;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::findOrInsertDefault(
                                                              const KeyType& k)
{
    bslalg::BidirectionalLink *position = this->find(k);
    if (!position) {
        if (d_size >= d_capacity) {
            this->expandTable();
        }

        position = d_parameters.nodeFactory().createDefaultNodeForKey(k);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(
                                                    &d_anchor,
                                                    position,
                                                    hashCodeForNode(position));
        ++d_size;
    }
    return position;
}

// template on SOURCE_TYPE to avoid binding an un-necessary temporary object.
// Typically an optimization for iterator-range inserts rather than inserting
// a single value.
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::insertIfMissing(
                                               bool               *isInserted,
                                               const SOURCE_TYPE&  obj)
{
    const KeyType& k = VALUE_POLICY::extractKey(obj);

    bslalg::BidirectionalLink *position = this->find(k);
    *isInserted = (!position);

    if(!position) {
        if (d_size >= d_capacity) {
            this->expandTable();
        }

        position = d_parameters.nodeFactory().createDefaultNode(obj);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(
                                                    &d_anchor,
                                                    position,
                                                    hashCodeForNode(position));
        ++d_size;
    }

    return position;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::insertContiguous(
                                                       const SOURCE_TYPE&  obj)
{
    const KeyType& k = VALUE_POLICY::extractKey(obj);

    bslalg::BidirectionalLink *position = this->find(k);

    if (d_size >= d_capacity) {
        this->expandTable();
    }

    if(!position) {
        position = d_parameters.nodeFactory().createNode(obj);
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&d_anchor,
                                             position,
                                             hashCodeForNode(position));
    }
    else {
        bslalg::BidirectionalLink *newNode =
                                    d_parameters.nodeFactory().createNode(obj);
        bslalg::HashTableImpUtil::insertDuplicateAtPosition(
                                                      &d_anchor,
                                                      newNode,
                                                      hashCodeForNode(newNode),
                                                      position);
        position = newNode;
    }

    ++d_size;

    return position;
}

template <class VALUE_TYPE, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable<VALUE_TYPE, HASH, EQUAL, ALLOCATOR>::expandTable()
{
    this->rehash(this->numBuckets() + 1);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::removeAllAndDeallocate()
{
    this->removeAll();
    HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                       d_anchor.bucketArrayAddress(),
                                       d_anchor.bucketArraySize(),
                                       d_parameters.nodeFactory().allocator());
    d_anchor.setBucketArrayAddressAndSize(&HashTable_StaticBucket::s_bucket,
                                          1);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::remove(
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

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::removeAll()
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

    // TBD REFACTOR

    // should we really be memsetting here? we should put default onstructed
    // buckets.
    native_std::memset(d_anchor.bucketArrayAddress(),
                0,
                sizeof(HashTableBucket) * d_anchor.bucketArraySize());

    d_anchor.setListRootAddress(0);
    d_size = 0;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::swap(HashTable& other)
{
    // assert that allocators are compatible
    // TBD: We do not yet support allocator propagation by allocator traits
    BSLS_ASSERT(this->d_parameters.nodeFactory().allocator() ==
                other.d_parameters.nodeFactory().allocator());

    d_parameters.swap(other.d_parameters);

    using native_std::swap;  // otherwise it is hidden by this very definition!

    swap(d_anchor,        other.d_anchor);
    swap(d_size,          other.d_size);
    swap(d_capacity,      other.d_capacity);
    swap(d_maxLoadFactor, other.d_maxLoadFactor);
}

// observers
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const HASH& HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::hasher() const
{
    return d_parameters.hasher();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const EQUAL& HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::comparator() const
{
    return d_parameters.comparator();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::AllocatorType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::allocator() const
{
    return d_parameters.nodeFactory().allocator();
}

// lookup
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::find(const KeyType& k) const
{
    return bslalg::HashTableImpUtil::find<VALUE_POLICY>(d_anchor,
                                                        this->comparator(),
                                                        k,
                                                        this->hasher()(k));
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::findRange(
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

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::findEndOfRange(
                                        bslalg::BidirectionalLink *first) const
{
    BSLS_ASSERT_SAFE(first);

    const KeyType& k = LinkTranslator::template
                                               extractKey<VALUE_POLICY>(first);
    while ((first = first->nextLink()) &&
           this->comparator()(k,
                              LinkTranslator::template extractKey<VALUE_POLICY>
                                                                      (first)))
    {
    }
    return first;
}

// bucket interface
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::numBuckets() const
{
    return d_anchor.bucketArraySize();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::maxNumBuckets() const
{
    return this->max_size();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::numElementsInBucket(
                                                              SizeType n) const
{
    BSLS_ASSERT_SAFE(n < this->numBuckets());
    return bucketAtIndex(n).countElements();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
typename HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::SizeType
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::bucketIndexForKey(
                                                        const KeyType& k) const
{
    size_t hashCode = this->hasher()(k);
    return bslalg::HashTableImpUtil::computeBucketIndex(
                                                   hashCode,
                                                   d_anchor.bucketArraySize());
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
const bslalg::HashTableBucket&
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::bucketAtIndex(
                                                              SizeType n) const
{
    BSLS_ASSERT_SAFE(n < this->numBuckets());

    return d_anchor.bucketArrayAddress()[n];
}


    // hash policy
template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
float HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::loadFactor() const
{
    return (double)size() / this->numBuckets();
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
float HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::maxLoadFactor() const
{
    return d_maxLoadFactor;
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::setMaxLoadFactor(
                                                              float loadFactor)
{
    d_maxLoadFactor = loadFactor;
    d_capacity = this->numBuckets() * loadFactor;

    if (d_capacity < this->size()) {
        this->reserve(this->size());
    }
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::rehash(SizeType n)
{
    // compute a "good" number of buckets, e.g., pick a prime number
    // from a sorted array of exponentially increasing primes.
    n = HashTable_PrimeUtil::nextPrime(n);

    if (n > this->numBuckets()) {
        d_capacity = n * this->maxLoadFactor();

        // Now that 'anchor' is not default constructible, we take a copy of
        // the anchor in the table.  Would it be better for 'initAnchor' to
        // be replaced with a 'createArrayOfEmptyBuckets' function, and we use
        // the result to construct the 'newAnchor'?
        bslalg::HashTableAnchor newAnchor = d_anchor;
        HashTable_Util<ALLOCATOR>::initAnchor(&newAnchor,
                                              n,
                                              d_parameters.nodeFactory().allocator());

        if (d_anchor.listRootAddress()) {
            bslalg::HashTableImpUtil::rehash<VALUE_POLICY>(
                                                    &newAnchor,
                                                    d_anchor.listRootAddress(),
                                                    hasher());
        }
        d_anchor.swap(newAnchor);
        HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                                newAnchor.bucketArrayAddress(),
                                                newAnchor.bucketArraySize(),
                                                d_parameters.nodeFactory().allocator());
    }
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>::reserve(SizeType n)
{
    this->rehash(native_std::ceil(n / this->maxLoadFactor()));
}

}  // close namespace BloombergLP::bslstl

//----------------------------------------------------------------------------
//                  free functions and opterators
//----------------------------------------------------------------------------

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
void
bslstl::swap(bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>& x,
             bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>& y)
{
    x.swap(y);
}

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
bool
bslstl::operator==(
            const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>& lhs,
            const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>& rhs)
{
    // The template bloat of this function can be significantly reduced
    // What matters is that the two hash tables:
    // i/   are the same size
    // ii/  have lists that are permutations of each other according to the
    //      element's 'operator=='
    // This means that the implementation should be independant of all four
    // template parameters, but will depend on VALUE_TYPE deduced from the
    // VALUE_POLICY.  Otherwise, after the initial size comparison, the rest
    // depends only on the anchors.
    typedef typename VALUE_POLICY::KeyType   KeyType;
    typedef typename VALUE_POLICY::ValueType ValueType;
    typedef typename ::bsl::allocator_traits<ALLOCATOR>::size_type SizeType;
    typedef HashTable_NodeUtil<ValueType>    LinkTranslator;

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
           rhs.find(LinkTranslator::template extractKey<VALUE_POLICY>(cursor));
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
                   LinkTranslator::extractValue(cursor) ==
                   LinkTranslator::extractValue(rhsFirst))
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
                                          LinkTranslator::extractValue(marker);

            if (cursor != marker) {  // skip on first pass only
                bslalg::BidirectionalLink *scanner = cursor;
                while (scanner != marker &&
                       LinkTranslator::extractValue(scanner) == valueAtMarker)
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
                if (LinkTranslator::extractValue(scanner) == valueAtMarker) {
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

                if (LinkTranslator::extractValue(scanner) == valueAtMarker) {
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

template <class VALUE_POLICY, class HASH, class EQUAL, class ALLOCATOR>
inline
bool
bslstl::operator!=(
            const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>& lhs,
            const bslstl::HashTable<VALUE_POLICY, HASH, EQUAL, ALLOCATOR>& rhs)
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
