// bslstl_hashtable.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLE
#define INCLUDED_BSLSTL_HASHTABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash-container with support for duplicate values.
//
//@CLASSES:
//   bslstl::HashTable : hashed-table container for user-supplied object types
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an implementation of a container,
// 'HashTable', specified by the C++11 standard.
//
// This implementation will use a single, bidirectional list, indexed by
// a dynamic array of buckets, each of which is a simple pointer to a node
// in the list.
//
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propagate, or, if some change
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
#error "<bslstl_hashtable.h> header can't be included directly in \
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

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALNODEPOOL
#include <bslstl_bidirectionalnodepool.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
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

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
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

                           // ==========================
                           // class HashTable_Parameters
                           // ==========================

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
class HashTable_Parameters : private bslalg::FunctorAdapter<HASHER>::Type
                           , private bslalg::FunctorAdapter<COMPARATOR>::Type
{
    // This class holds all the parameterized parts of a 'HashTable' class,
    // efficiently exploiting the empty base optimization without adding
    // unforeseen associations to the 'HashTable' class itself due to the
    // structural inheritance.

  public:
    typedef ALLOCATOR                              AllocatorType;
    typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
    typedef typename KEY_CONFIG::KeyType           KeyType;
    typedef typename KEY_CONFIG::ValueType         ValueType;
    typedef bslalg::BidirectionalNode<ValueType>   NodeType;
    typedef typename AllocatorTraits::size_type    SizeType;

  private:
    // PRIVATE TYPES
    typedef typename AllocatorTraits::template
                         rebind_traits<NodeType>::allocator_type NodeAllocator;

    // These two aliases simplify naming the base classes in the constructor
    typedef typename bslalg::FunctorAdapter<HASHER>::Type  HasherBaseType;
    typedef typename bslalg::FunctorAdapter<COMPARATOR>::Type ComparatorBaseType;

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
    HashTable_Parameters(const HASHER&          hash,
                         const COMPARATOR&         compare,
                         const AllocatorType& allocator);

    HashTable_Parameters(const HashTable_Parameters& other,
                         const AllocatorType&        allocator);

    // MANIPULATORS
    NodeFactory& nodeFactory();

    void swap(HashTable_Parameters& other);

    // ACCESSORS
    const HASHER&       hasher()      const;
    const COMPARATOR&   comparator()  const;
    const NodeFactory&  nodeFactory() const;
};

                           // ===============
                           // class HashTable
                           // ===============

// No need for any defaults below, this imp.-detail component is intended to be
// instantiated and used from the public interface of a std container that
// provides all the user-friendly defaults, and explicitly pass down what is
// needed.
template <class KEY_CONFIG,
          class HASHER,
          class COMPARATOR,
          class ALLOCATOR = ::bsl::allocator<typename KEY_CONFIG::ValueType> >
class HashTable {
  private:
    // PRIVATE TYPES
    typedef HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>
                                                                ImplParameters;
    typedef typename ImplParameters::AllocatorTraits AllocatorTraits;

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

    float                   d_maxLoadFactor; // maximum load factor

  private:
    // PRIVATE MANIPULATORS
    void copyDataStructure(const bslalg::BidirectionalLink *cursor);
        // Copy the sequence of elements from the list starting at the
        // specified 'cursor' and having 'size' elements.  Allocate a bucket
        // array sufficiently large to store 'size' elements while respecting
        // the 'maxLoadFactor', and index the copied list into that new array
        // of hash buckets.  This hash table then takes ownership of the list
        // and bucket array.  Note that this method is intended to be called
        // from copy constructors, which will have assigned some initial
        // values for the 'size' and other attributes that may not be
        // consistent with the class invariants until after this method is
        // called.

    void quickSwap(HashTable *other);
        // Efficiently exchange the value and functors of this object with
        // the value and functors of the specified 'other' object.  This method
        // provides the no-throw exception-safety guarantee.  The behavior is
        // undefined unless this object was created with the same allocator as
        // '*other'.

    void removeAllImp();
        // Erase all the nodes in this table and deallocate their memory via
        // the node factory, without performing the necessary bookkeeping to
        // reflect such change.  Note that this method is expected to be useful
        // when the anchor of this hash table is about to be over-written with
        // a new value, or when the hash table is going out of scope and the
        // extra bookkeeping is not necessary.

    void removeAllAndDeallocate();
        // Erase all the nodes in this table, and deallocate their memory via
        // the supplied node factory.  Destroy the array of buckets owned by
        // this table.  Note that a default constructed hash table does not
        // own its bucket array, so does not destroy it.

    // PRIVATE ACCESSORS
    native_std::size_t hashCodeForNode(bslalg::BidirectionalLink *node) const;
        // Return the hash code for the element stored in the specified 'node'
        // using a copy of the hash functor supplied at construction.  The
        // behavior is undefined unless 'node' points to a list node holding
        // a value of the same element type as this hash table.

    bslalg::BidirectionalLink *find(const KeyType&     key,
                                    native_std::size_t hashValue) const;
        // Return the first node in this hash table having a key that compares
        // equal to the specified 'key' when compared using this hash table's
        // comparator.  The behavior is undefined unless the specified
        // 'hashValue' is the hash code for the specified 'key' according to
        // the 'hasher' functor of this hash table.  Note that this function's
        // implementation relies on the supplied 'hashValue' rather than
        // recomputing it, eliminating some redundant computation for the
        // public methods.

    bslalg::HashTableBucket *getBucketAddress(SizeType bucketIndex) const;
        // Return the address of the bucket at the specified 'bucketIndex' in
        // bucket array of this hash table.  The behavior is undefined unless
        // 'bucketIndex < this->numBuckets()'.

  public:
    // CREATORS
    HashTable(const HASHER&      hash,
              const COMPARATOR&     compare,
              SizeType         initialBucketCount,
              const ALLOCATOR& allocator = ALLOCATOR());
        // Behavior is undefined unless '0 < initialBucketCount'.

    HashTable(const HashTable& original);
        // Copy the specified 'other' using the allocator specified by
        // 'bsl::allocator_traits<ALLOCATOR>::
        //  select_on_container_copy_construction(original.allocator())'.

    HashTable(const HashTable& original, const ALLOCATOR& allocator);
        // Copy the specified 'other' using the specified 'allocator'.

    ~HashTable();
        // Destroy this object.

    // MANIPULATORS
    HashTable& operator=(const HashTable& rhs);
        // Assign to this object the value and functors of the specified
        // 'rhs' object, and if the 'ALLOCATOR' type has the trait
        // 'propagate_on_container_copy_assignment' replace the allocator of
        // this object with the allocator of 'rhs'.  Return a reference
        // providing modifiable access to this object.  This method requires
        // that the parameterized 'HASH' and 'EQUAL' types be
        // "copy-constructible" (see {Requirements on 'KEY'}).  The behavior is
        // undefined unless this object's allocator and the allocator of 'rhs'
        // have the same value, or the 'ALLOCATOR' type has the trait
        // 'propagate_on_container_copy_assignment'.


    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertIfMissing
                                           (bool               *isInsertedFlag,
                                            const SOURCE_TYPE&  obj);
        // Return the address of an element in this hash table having a key
        // that compares equal to the key of the specified 'obj' using the
        // 'comparator' functor of this hash table.  If no such element exists,
        // insert a copy of 'obj' into this hash table and return the address
        // of that node in this table's list.  Load 'true' into the specified
        // 'isInsertedFlag' if insertion is performed, and 'false' if an
        // existing element having a matching key was found.

    bslalg::BidirectionalLink *remove(bslalg::BidirectionalLink *node);
        // Return the address of the node following the specified 'node in the
        // list owned by this hash table, after removing the 'node' from the
        // list and destroying the element stored in the 'node'.

    bslalg::BidirectionalLink *findOrInsertDefault(const KeyType& key);
        // Return the first link of the contiguous list of links containing the
        // elements of this table having the same specified 'key' according to
        // the 'comparator' functor of this hash table, or a null pointer value
        // if no such elements are stored in this hash table.  If there are no
        // such elements, insert an element having the specified 'key' and a
        // default value associated with that 'key', and return the address of
        // the new node.

    template <class SOURCE_TYPE>
    bslalg::BidirectionalLink *insertContiguous(const SOURCE_TYPE& obj);
        // Insert the specified 'obj' into this hash table.  If an element
        // is present in this hash table having a key that compares equal to
        // 'obj' using this hash table's 'comparator' functor, then 'obj'
        // will be inserted into this hash table's list immediately preceding
        // the first such element.

    void rehashForNumBuckets(SizeType newNumBuckets);
        // Allocate a new bucket array having at least the specified
        // 'newNumBuckets', and re-organize the list of elements owned by this
        // hash table so that the new buckets index them correctly.  If an
        // exception is thrown by either of the user supplied functors used to
        // configure this hash table, then it will be left in an unspecified
        // state; no memory will be leaked, but some elements may be destroyed
        // and erased from the container.  If an exception is thrown allocating
        // the new bucket array, then this hash table will be left unchanged.
        // Note that more buckets than requested may be allocated in order to
        // preserve the bucket allocation strategy of the hash table, but never
        // fewer.

    void rehashForNumElements(SizeType numElements);
        // Allocate a new bucket array having at least sufficient buckets to
        // support a hash table having 'size' elements without exceeding the
        // 'maxLoadFactor', and re-organize the list of elements owned by this
        // hash table so that the new buckets index them correctly.  If an
        // exception is thrown by either of the user supplied functors used to
        // configure this hash table, then it will be left in an unspecified
        // state; no memory will be leaked, but some elements may be destroyed
        // and erased from the container.  If an exception is thrown allocating
        // the new bucket array, then this hash table will be left unchanged.
        // Note that more buckets than requested may be allocated in order to
        // preserve the bucket allocation strategy of the hash table, but never
        // fewer.

    void removeAll();
        // Destroy all the elements in this container, reclaim their memory,
        // and make this hash table empty.

    void setMaxLoadFactor(float loadFactor);
        // Set the maximum load factor permitted by this hash table, where load
        // factor is the statistical mean number of elements per bucket.  This
        // hash table will rehash using a larger number of buckets if any
        // insert operation would cause it to exceed the 'maxLoadFactor'.

    void swap(HashTable& other);
        // Exchange the value of this object, its 'comparator' functor and its
        // 'hasher' function, with those of the specified 'other' object.
        // Additionally if
        // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
        // 'true' then exchange the allocator of this object with that of the
        // 'other' object, and do not modify either allocator otherwise.  This
        // method provides the no-throw exception-safety guarantee unless any
        // of the 'comparator' or 'hasher' functors throw when swapped, leaving
        // both objects in an safely destructible, but otherwise unusable,
        // state.  The operation guarantees O[1] complexity.  The behavior is
        // undefined unless either this object was created with the same
        // allocator as 'other' or 'propagate_on_container_swap' is 'true'.


    // ACCESSORS
    bslalg::BidirectionalLink *elementListRoot() const;
        // Return the address of the first element in this hash table, or a
        // null pointer value if this hash table is empty.

    bslalg::BidirectionalLink *find(const KeyType& key) const;
        // Return the first link of the contiguous list of links containing the
        // elements of this table having the same specified 'key' according to
        // the 'comparator' functor of this hash table, or a null pointer value
        // if no such elements are stored in this hash table.

    SizeType size() const;
        // Return the number of elements in this hash table.

    ALLOCATOR allocator() const;
        // Return a copy of the allocator used to construct this hash table.
        // Note that this is not the allocator used to allocate elements for
        // this hash table, which is instead a copy of that allocator rebound
        // to allocate the nodes used by the internal data structure of this
        // hash table.

    const COMPARATOR& comparator() const;
        // Return the 'comparator' functor of this hash table, used to
        // determine if two element keys have the same value.

    const HASHER& hasher()     const;
        // Return the 'hasher' functor of this hash table, used to compute
        // the hash code for the keys of elements to be stored or found in
        // this hash table.

    float maxLoadFactor() const;
        // Return the maximum load factor permitted by this hash table object,
        // where the load factor is the statistical mean number of elements per
        // bucket.  Note that this hash table will enforce the maximum load
        // factor by rehashing into a larger array of buckets if an insertion
        // would cause the maximum load factor to be exceeded.

    SizeType bucketIndexForKey(const KeyType& key) const;
        // Return the index of the bucket that would contain all the elements
        // having the specified 'key'.

    const bslalg::HashTableBucket& bucketAtIndex(SizeType index) const;
        // TBD...
        // Return a reference to the 'index'th non-modifiable bucket in the
        // sequence of buckets.  The behavior is undefined unless
        // 'index < numBuckets()'.

    SizeType countElementsInBucket(SizeType index) const;
        // Return the number elements contained in the bucket at the specified
        // index 'index'.  Note that this operation will be linear in the size
        // of the bucket.

    void findRange(bslalg::BidirectionalLink **first,
                   bslalg::BidirectionalLink **last,
                   const KeyType&              k) const;
        // Load into the specified 'first' and 'last' pointers the address of
        // the first and last link in the list of elements owned by this hash
        // table where the stored elements have a key that compares equal to
        // the specified 'k' using the 'comparator' of this hash table, and
        // pointer values otherwise.  Note
        // that the output values will form a closed range, where both 'first'
        // and 'last' point to links satisfying the predicate, rather than the
        // more familiar standard library notion of a semi-open range where
        // 'last' would point to the element following the range.  Also note
        // that the implicit requirements of this hash table are that all
        // elements having a key that would compare equal to 'k' will be stored
        // contiguously in the list owned by this hash table.

    bslalg::BidirectionalLink *findEndOfRange(
                                       bslalg::BidirectionalLink *first) const;
        // Return the address of the first node in the total sequence of the
        // hash table following the specified 'first' node that holds a value
        // with a key that does not have the same value as the key stored in
        // the node pointed to by 'first', or a null pointer value all the
        // following nodes hold a key that has the same value as the key stored
        // in the node pointed to by 'first'.  The behavior is undefined unless
        // 'first' points to a link in the list owned by this hash table.

    bool isEmpty() const;
        // Return 'true' if this hash table contains no elements, and 'false'
        // otherwise.
        // TBD: is this function really necessary?

    float loadFactor() const;
        // Return the current load factor for this table.  The load factor is
        // the statical mean number of elements per bucket.

    SizeType maxNumBuckets() const;
        // Return the maximum number of buckets that can be contained in this
        // hash table.

    SizeType maxSize() const;
        // Return the maximum number of elements that can be stored in this
        // hash table.

    SizeType numBuckets() const;
        // Return the number of buckets contained in this hash table.
};

template <class VALUE_TYPE, class HASHER, class COMPARATOR, class ALLOCATOR>
void swap(HashTable<VALUE_TYPE, HASHER, COMPARATOR, ALLOCATOR>& x,
          HashTable<VALUE_TYPE, HASHER, COMPARATOR, ALLOCATOR>& y);
    // Swap both the value and the comparator of the specified 'a' object with
    // the value and comparator of the specified 'b' object.  Additionally if
    // 'bslstl::AllocatorTraits<ALLOCATOR>::propagate_on_container_swap' is
    // 'true' then exchange the allocator of 'a' with that of 'b', and do not
    // modify either allocator otherwise.  This method provides the no-throw
    // exception-safety guarantee and guarantees O[1] complexity.  The
    // behavior is undefined is unless either this object was created with the
    // same allocator as 'other' or 'propagate_on_container_swap' is 'true'.

template <class VALUE_TYPE, class HASHER, class COMPARATOR, class ALLOCATOR>
bool operator==(
              const HashTable<VALUE_TYPE, HASHER, COMPARATOR, ALLOCATOR>& lhs,
              const HashTable<VALUE_TYPE, HASHER, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'HashTable' objects have the same
    // value if they have the same number of keys, and for each subset of keys
    // that compare equal with each other according to that hash table's
    // 'comparator' functor, a corresponding subset of keys exists in the other
    // hash table, having the same number of keys, and that for each such
    // subset the elements having the same key in one container form a
    // permutation of the corresponding subset of elements in the other
    // container.  This method requires that the 'ValueType' of the
    // parameterized 'KEY_POLICY' be "equality-comparable" (see {Requirements
    // on 'KEY_POLICY'}).  Note that 'operator==' is used to compare keys and
    // elements between hash tables, and *not* the 'EQUAL' functor stored in
    // the hash table that is used to compare keys between elements in the same
    // hash table.

template <class VALUE_TYPE, class HASHER, class COMPARATOR, class ALLOCATOR>
bool operator!=(
              const HashTable<VALUE_TYPE, HASHER, COMPARATOR, ALLOCATOR>& lhs,
              const HashTable<VALUE_TYPE, HASHER, COMPARATOR, ALLOCATOR>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'HashTable' objects do not have
    // the same value if they do not have the same number of elements, or if
    // for any key value found in one hash table, the subset of keys having the
    // same value as that key within that container holds a different number of
    // keys than the corresponding subset of keys in the other hash table, or
    // if any such corresponding pair of subsets are not permutations of each
    // other when the elements stored with those keys are compared using
    // 'operator=='.  This method requires that the 'ValueType' of the
    // parameterized 'KEY_POLICY' be "equality-comparable" (see {Requirements
    // on 'KEY_POLICY'}).  Note that 'operator==' is used to compare keys and
    // elements between hash tables, and *not* the 'EQUAL' functor stored in
    // the hash table that is used to compare keys between elements in the same
    // hash table.

                    // ========================
                    // class HashTable_IterUtil
                    // ========================

// Move this to its own component, and avoid 'iterator' dependency
// This is used only in higher level components anyway - does not belong.
// Generally useful for all container implementations, probably already exists
// somewhere in bslalg.

struct HashTable_IterUtil {
    // This utility struct provides a namespace for functions on iterators that
    // are useful when implementing a hash table.

    // generic utility that needs a non-template hosted home
    template <class InputIterator>
    static native_std::size_t insertDistance(InputIterator first,
                                             InputIterator last);
        // Return 0 if InputIterator really is limited to the standard
        // input-iterator category, otherwise return the distance from first
        // to last.
};

                    // ============================
                    // class HashTable_ArrayProctor
                    // ============================

template <class ALLOCATOR>
class HashTable_ArrayProctor {
    // This class probably already exists in 'bslalg'
  private:
    // DATA
    ALLOCATOR                      d_allocator;
    bslalg::HashTableBucket       *d_array;
    typename ALLOCATOR::size_type  d_size;

  private:
    // NOT IMPLEMENTED
    HashTable_ArrayProctor(const HashTable_ArrayProctor&);
    HashTable_ArrayProctor& operator == (const HashTable_ArrayProctor&);

  public:
    HashTable_ArrayProctor(const ALLOCATOR&               allocator,
                           const bslalg::HashTableAnchor& anchor);

    ~HashTable_ArrayProctor();

    void dismiss();
};

                    // ===========================
                    // class HashTable_ListProctor
                    // ===========================

template <class FACTORY>
class HashTable_ListProctor {
  private:
    // DATA
    FACTORY                   *d_factory;
    bslalg::BidirectionalLink *d_root;

  private:
    // NOT IMPLEMENTED
    HashTable_ListProctor(const HashTable_ListProctor&);
    HashTable_ListProctor& operator == (const HashTable_ListProctor&);

  public:
    HashTable_ListProctor(FACTORY                   *factory,
                          bslalg::BidirectionalLink *listRoot);

    ~HashTable_ListProctor();

    void dismiss();
};

                    // =========================
                    // class HashTable_PrimeUtil
                    // =========================

struct HashTable_PrimeUtil {
    static size_t nextPrime(size_t n);
};

                    // ============================
                    // class HashTable_StaticBucket
                    // ============================

struct HashTable_StaticBucket {
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

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

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
inline
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
                                      const bslalg::HashTableAnchor& anchor)
: d_allocator(allocator)
, d_array(anchor.bucketArrayAddress())
, d_size(anchor.bucketArraySize())
{
}

template <class ALLOCATOR>
inline
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

                    // --------------------------
                    // class HashTable_Parameters
                    // --------------------------

// CREATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable_Parameters(const HASHER&          hash,
                     const COMPARATOR&         compare,
                     const AllocatorType& allocator)
: HasherBaseType(hash)
, ComparatorBaseType(compare)
, d_nodeFactory(allocator)
{
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable_Parameters(const HashTable_Parameters& other,
                     const AllocatorType&        allocator)
: HasherBaseType(static_cast<const HasherBaseType&>(other))
, ComparatorBaseType(static_cast<const ComparatorBaseType&>(other))
, d_nodeFactory(allocator)
{
}

// MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::NodeFactory&
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::nodeFactory()
{
    return d_nodeFactory;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
swap(HashTable_Parameters& other)
{
    using native_std::swap;  // otherwise it is hidden by this very definition!

    bslalg::SwapUtil::swap(
                     static_cast<HasherBaseType*>(this),
                     static_cast<HasherBaseType*>(BSLS_UTIL_ADDRESSOF(other)));

    bslalg::SwapUtil::swap(
                 static_cast<ComparatorBaseType*>(this),
                 static_cast<ComparatorBaseType*>(BSLS_UTIL_ADDRESSOF(other)));

    swap(d_nodeFactory, other.d_nodeFactory);
}


// OBSERVERS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const HASHER&
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hasher() const
{
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const COMPARATOR&
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::comparator() const
{
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const typename HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::NodeFactory&
HashTable_Parameters<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>:: nodeFactory() const
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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
native_std::size_t
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hashCodeForNode(
                                         bslalg::BidirectionalLink *node) const
{
    BSLS_ASSERT_SAFE(node);

    const typename KEY_CONFIG::KeyType& key =
                        bslalg::HashTableImpUtil::extractKey<KEY_CONFIG>(node);
    return hasher()(key);
}

// CREATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HASHER&      hash,
          const COMPARATOR&     compare,
          SizeType         initialBucketCount,
          const ALLOCATOR& allocator)
: d_parameters(hash, compare, allocator)
, d_anchor(HashTable_StaticBucket::getDefaultBucketAddress(), 1, 0)
, d_size()
, d_capacity(initialBucketCount)
, d_maxLoadFactor(1.0)
{
    if (0 != initialBucketCount) {
        HashTable_Util<ALLOCATOR>::initAnchor(
                            &d_anchor,
                            HashTable_PrimeUtil::nextPrime(initialBucketCount),
                            allocator);
    }
}


template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HashTable& original)
: d_parameters(
  original.d_parameters,
  AllocatorTraits::select_on_container_copy_construction(original.allocator()))
, d_anchor(HashTable_StaticBucket::getDefaultBucketAddress(), 1, 0)
, d_size(original.d_size)
, d_capacity(0)
, d_maxLoadFactor(original.d_maxLoadFactor)
{
    if (d_size > 0) {
        this->copyDataStructure(original.d_anchor.listRootAddress());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
HashTable(const HashTable& original, const ALLOCATOR& allocator)
: d_parameters(original.d_parameters, allocator)
, d_anchor(HashTable_StaticBucket::getDefaultBucketAddress(), 1, 0)
, d_size(original.d_size)
, d_capacity(0)
, d_maxLoadFactor(original.d_maxLoadFactor)
{
    if (d_size > 0) {
        this->copyDataStructure(original.d_anchor.listRootAddress());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::~HashTable()
{
    this->removeAllAndDeallocate();
}

// PRIVATE MANIPULATORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::copyDataStructure(
                                       const bslalg::BidirectionalLink *cursor)
{
    // Allocate an appropriate number of buckets
    SizeType numBuckets = HashTable_PrimeUtil::nextPrime(
                                               static_cast<native_std::size_t>(
        native_std::ceil(static_cast<float>(d_size) / this->d_maxLoadFactor)));

    HashTable_Util<ALLOCATOR>::initAnchor(&d_anchor,
                                          numBuckets,
                                          this->allocator());
    // create a proctor for d_anchor's allocated array
    HashTable_ArrayProctor<ALLOCATOR> arrayProctor(this->allocator(),
                                                   d_anchor);

    d_capacity = static_cast<native_std::size_t>(
                       static_cast<float>(numBuckets) * this->d_maxLoadFactor);

    bslalg::BidirectionalLink *newNode =
                                d_parameters.nodeFactory().createNode(*cursor);

    newNode->reset();  // must terminate list before creating proctor
    HashTable_ListProctor<typename ImplParameters::NodeFactory>
                             listProctor(&d_parameters.nodeFactory(), newNode);

    d_anchor.setListRootAddress(newNode);

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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAllImp()
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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::quickSwap(
                                                              HashTable *other)
{
    BSLS_ASSERT_SAFE(other);
    BSLS_ASSERT_SAFE(this->allocator() == other->allocator());

    d_parameters.swap(other->d_parameters);

    using native_std::swap;  // otherwise it is hidden by this very definition!

    swap(d_anchor,        other->d_anchor);
    swap(d_size,          other->d_size);
    swap(d_capacity,      other->d_capacity);
    swap(d_maxLoadFactor, other->d_maxLoadFactor);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bool
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::isEmpty() const
{
    return !d_size;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::size() const
{
    return d_size;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxSize() const
{
    return native_std::numeric_limits<SizeType>::max();
}

    // iterators
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::elementListRoot() const
{
    return d_anchor.listRootAddress();
}

// PRIVATE ACCESSORS
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::HashTableBucket *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::getBucketAddress(
                                                    SizeType bucketIndex) const
{
    BSLS_ASSERT_SAFE(bucketIndex < this->numBuckets());

    return d_anchor.bucketArrayAddress() + bucketIndex;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::find(
                                                const KeyType& key,
                                                size_t         hashValue) const
{
    return bslalg::HashTableImpUtil::find<KEY_CONFIG>(d_anchor,
                                                      key,
                                                      this->comparator(),
                                                      hashValue);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findOrInsertDefault(
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
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::operator=(
                                                          const HashTable& rhs)
{
    typedef typename ImplParameters::AllocatorTraits AllocatorTraits;

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {

        if (AllocatorTraits::propagate_on_container_copy_assignment::VALUE) {
            HashTable other(rhs, rhs.allocator());
            bslalg::SwapUtil::swap(
                                &this->d_parameters.nodeFactory().allocator(),
                                &other.d_parameters.nodeFactory().allocator());
            quickSwap(&other);
        }
        else {
            HashTable other(rhs, this->allocator());
            quickSwap(&other);
        }
    }
    return *this;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertIfMissing(
                                               bool               *isInserted,
                                               const SOURCE_TYPE&  obj)
{
    const KeyType& key = KEY_CONFIG::extractKey(obj);
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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
template <class SOURCE_TYPE>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::insertContiguous(
                                                       const SOURCE_TYPE&  obj)
{
    const KeyType& key = KEY_CONFIG::extractKey(obj);
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
        bslalg::HashTableImpUtil::insertAtPosition(&d_anchor,
                                                   newNode,
                                                   hashCode,
                                                   position);
        position = newNode;
    }

    ++d_size;

    return position;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAllAndDeallocate()
{
    this->removeAllImp();
    HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                                 d_anchor.bucketArrayAddress(),
                                                 d_anchor.bucketArraySize(),
                                                 this->allocator());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::remove(
                                               bslalg::BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);
    BSLS_ASSERT_SAFE(node->previousLink()
                  || d_anchor.listRootAddress() == node);

    bslalg::BidirectionalLink *result = node->nextLink();

    bslalg::HashTableImpUtil::remove(&d_anchor,
                                     node,
                                     hashCodeForNode(node));
    --d_size;

    d_parameters.nodeFactory().deleteNode((NodeType *)node);

    return result;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::removeAll()
{
    this->removeAllImp();
    native_std::memset(d_anchor.bucketArrayAddress(),
                0,
                sizeof(bslalg::HashTableBucket) * d_anchor.bucketArraySize());

    d_anchor.setListRootAddress(0);
    d_size = 0;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::swap(HashTable& other)
{
    typedef typename ImplParameters::AllocatorTraits AllocatorTraits;

    if (AllocatorTraits::propagate_on_container_swap::VALUE) {
        bslalg::SwapUtil::swap(&this->d_parameters.nodeFactory().allocator(),
                               &other.d_parameters.nodeFactory().allocator());
        quickSwap(&other);
    }
    else {
        // C++11 behavior: undefined for unequal allocators
        // BSLS_ASSERT(allocator() == other.allocator());

        // backward compatible behavior: swap with copies
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
               d_parameters.nodeFactory().allocator() ==
               other.d_parameters.nodeFactory().allocator())) {
            quickSwap(&other);
        }
        else {
            BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
            HashTable thisCopy(*this, other.allocator());
            HashTable otherCopy(other, this->allocator());

            quickSwap(&otherCopy);
            other.quickSwap(&thisCopy);
        }
    }
}

// observers
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const HASHER&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::hasher() const
{
    return d_parameters.hasher();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const COMPARATOR&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::comparator() const
{
    return d_parameters.comparator();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
ALLOCATOR HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::allocator() const
{
    return d_parameters.nodeFactory().allocator();
}

// lookup
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::find(
                                                      const KeyType& key) const
{
    return bslalg::HashTableImpUtil::find<KEY_CONFIG>(d_anchor,
                                                      key,
                                                      this->comparator(),
                                                      this->hasher()(key));
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable< KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findRange(
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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslalg::BidirectionalLink *
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::findEndOfRange(
                                        bslalg::BidirectionalLink *first) const
{
    BSLS_ASSERT_SAFE(first);

    typedef bslalg::HashTableImpUtil ImpUtil;
    const KeyType& k = ImpUtil::extractKey<KEY_CONFIG>(first);
    while ((first = first->nextLink()) &&
           this->comparator()(k,ImpUtil::extractKey<KEY_CONFIG>(first))) {
    }
    return first;
}

// bucket interface
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::numBuckets() const
{
    return d_anchor.bucketArraySize();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxNumBuckets() const
{
    return this->max_size();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::countElementsInBucket(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return bucketAtIndex(index).countElements();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
typename HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::SizeType
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::bucketIndexForKey(
                                                      const KeyType& key) const
{
    size_t hashCode = this->hasher()(key);
    return bslalg::HashTableImpUtil::computeBucketIndex(
                                                   hashCode,
                                                   d_anchor.bucketArraySize());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
const bslalg::HashTableBucket&
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::bucketAtIndex(
                                                          SizeType index) const
{
    BSLS_ASSERT_SAFE(index < this->numBuckets());

    return d_anchor.bucketArrayAddress()[index];
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
float HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::loadFactor() const
{
    return (double)size() / this->numBuckets();
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
float
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::maxLoadFactor() const
{
    return d_maxLoadFactor;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::setMaxLoadFactor(
                                                              float loadFactor)
{
    d_maxLoadFactor = loadFactor;
    d_capacity = static_cast<native_std::size_t>(native_std::ceil(
                         static_cast<float>(this->numBuckets()) * loadFactor));

    if (d_capacity < this->size()) {
        this->rehashForNumElements(this->size());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::rehashForNumBuckets(
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
            bslalg::HashTableImpUtil::rehash<KEY_CONFIG>(
                                                    &newAnchor,
                                                    d_anchor.listRootAddress(),
                                                    hasher());
        }
        d_anchor.swap(newAnchor);
        d_capacity = static_cast<native_std::size_t>(native_std::ceil(
                   static_cast<float>(newNumBuckets) * this->maxLoadFactor()));

        HashTable_Util<ALLOCATOR>::destroyBucketArray(
                                                newAnchor.bucketArrayAddress(),
                                                newAnchor.bucketArraySize(),
                                                this->allocator());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::rehashForNumElements(
                                                          SizeType numElements)
{
    this->rehashForNumBuckets(static_cast<native_std::size_t>(native_std::ceil(
                    static_cast<float>(numElements) / this->maxLoadFactor())));
}

}  // close namespace BloombergLP::bslstl

//----------------------------------------------------------------------------
//                  free functions and operators
//----------------------------------------------------------------------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
void
bslstl::swap(bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& x,
             bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& y)
{
    x.swap(y);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bool
bslstl::operator==(
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs)
{
    // The template bloat of this function can be significantly reduced
    // What matters is that the two hash tables:
    // i/   are the same size
    // ii/  have lists that are permutations of each other according to the
    //      element's 'operator=='
    // This means that the implementation should be independent of all four
    // template parameters, but will depend on VALUE_TYPE deduced from the
    // KEY_CONFIG.  Otherwise, after the initial size comparison, the rest
    // depends only on the anchors.
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
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
                             rhs.find(ImpUtil::extractKey<KEY_CONFIG>(cursor));
        if (!rhsFirst) {
            return false;  // no matching key                         // RETURN
        }

        bslalg::BidirectionalLink *endRange = lhs.findEndOfRange(cursor);
        bslalg::BidirectionalLink *rhsLast  = rhs.findEndOfRange(rhsFirst);

        // Check the key-groups have the same length - a quick-fail test.
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
                 (ImpUtil::extractValue<KEY_CONFIG>(cursor) ==
                  ImpUtil::extractValue<KEY_CONFIG>(rhsFirst)))
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
                                    ImpUtil::extractValue<KEY_CONFIG>(marker);

            if (cursor != marker) {  // skip on first pass only
                bslalg::BidirectionalLink *scanner = cursor;
                while (scanner != marker &&
                   ImpUtil::extractValue<KEY_CONFIG>(scanner) == valueAtMarker)
                                                                              {
                    scanner = scanner->nextLink();
                }
                if (scanner != marker) {
                    // We have seen 'lhs' one before.
                    continue;
                }
            }

            SizeType matches = 0;
            for (bslalg::BidirectionalLink *scanner = rhsFirst;
                 scanner != rhsLast;
                 scanner = scanner->nextLink()) {
                if (ImpUtil::extractValue<KEY_CONFIG>(scanner) ==
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

                if (ImpUtil::extractValue<KEY_CONFIG>(scanner) ==
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

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
inline
bool
bslstl::operator!=(
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& lhs,
       const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& rhs)
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
