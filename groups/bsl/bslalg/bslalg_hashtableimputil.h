// bslalg_hashtableimputil.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#define INCLUDED_BSLALG_HASHTABLEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

// HMV -
//
// + I think 'spliceSegmentIntoBucket' should be updated:
//   + Its argument order should change (probably [bucket, newRoot, cursor,
//      nextCursor])
//   
//   + 'cursor', 'nextCursor' should be renamed 'first', 'last'
//
//   + I think the signature should change to:
//..
//  spliceListIntoBucket(HashTableAnchor   *hashTable,
//                       int                bucketIndex,
//                       BidirectionalLink *first,
//                       BidirectionalLink *last);
//..
//    I think this would be clearer, as well as (a little) faster. Note that
//    the implementation repeatedly calls 'findBucketForHashCode', converting
//    an index to a bucket address (requiring a bit of arithmetic & possibly
//    memory derefernce), for every node, where it really only needs to do
//    that for the last node in the bucket.  My primary motivation is clarity
//    though. 


//@PURPOSE: Provide a hash table data structure for unordered containers

///-HMV
// This is not a data structure:
//@PURPOSE: Provide algorithms for implementing a hash-table.
//

//@CLASSES:
//  bslalg::HashTableImpUtil: functions used to implement a hash table
//
//@SEE_ALSO: bslalg_bidirectionallinklistutil, bslalg_hashtableanchor,
//           bslstl_hashtable
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico(spacifico1)
//
//@DESCRIPTION: This component provides a namespace for utility functions used
// to implement a hash table container.  Almost all the functions provided by
// this component operate on a 'HashTableAnchor', a type encapsulating the key
// data members of a hash table.
//
//...
///Hash Table Structure
///--------------------
// The utilities provided by this component are used to create and manipulate
// a hash-table that resolves collisions using a linked-list of elements
// (i.e., chaining).  Many of the operations provided by 'HashTableImpUtil'
// operate on a 'HashTableAnchor', which encapsulates the data members of a
// hash table.  A 'HashTableAnchor' has the address of a single, doubly-linked
// list holding all the elements in the hash-table, as well as the address of
// an array of buckets.  Each bucket in the array of buckets holds a reference
// to the first and last element in the linked-list whose *adjusted* hash
// value is equal to the index of the bucket.  Further, the functions in this
// component ensure (and require) that all elements that fall within a bucket
// form a contiguous sequence in the linked list, as can be seen in the
// diagram below:
//..
//  FIG. 1: a hash table holding 5 elements
// 
//  Hash Function:  h(n) -> n  [identity function]   
//  F: First Element
//  L: Last Element
//             
//                     0       1       2       3       4
//                 +-------+-------+-------+-------+-------+-- 
//  bucket array   |  F L  |  F L  |  F L  |  F L  |  F L  |  ...
//                 +--+-+--+-------+-------+--+-+--+-------+--   
//                    | \___         _________/ /
//                     \    \       /          | 
//                     V     V     V           V
//                    ,-.   ,-.   ,-.   ,-.   ,-.
//  doubly        |---|0|---|0|---|3|---|3|---|3|--|
//  linked-list       `-'   `-'   `-'   `-'   `-'    
//..
//
///Hash Function and the Adjusted Hash Value
///-----------------------------------------
// The C++11 standard defines a hash function as a function 'h(k)' returning a
// (integral) value of type 'size_t', which, for different values of 'k', the
// probablity of 'h(k1)' and 'h(k2)' returning the same value should approach
// '1.0 / number_limitz<size_t>::max()' (see 17.6.3.4 [hash.requirements]).
// Such a function 'h(k)' may return values within the entire range of values
// that can be described using 'size_t', [0 .. number_limitz<size_t>::max()],
// however the array of buckets maintained by a hash-table is typically
// significantly smaller than 'number_limits<size_t>::max()', therefore a
// hash-table implementation must adjust the returned hash function so that it
// falls in the valid range of bucket indices (typically either using an
// integer division or modulo operation) -- we refer to this as the "adjusted
// hash function".  Note that currently 'HashTableImpUtil' adjusts the value
// returned by a supplied hash function using '%' (modulo), which prevents
// pathological behavior when used with a hash-function that may cluster more
// commonly hased values (e.g., the identity function), however the means of
// adjustment may change in the future.
//
// [HMV: I'd like to rewrite the "cluster more commonly hased values" bit, but
// leaving it for now].
///
///Well-Formed 'HashTableAnchor' Objects
///--------------------------------------
// Many of the algorithms defined in this component operate on
// 'HashTableAnchor' objects, which describe the attributes of a hash-table.
// The 'HashTableAnchor' objects supplied to 'HashTableImpUtil' are required
// to meet a series of constraints that are not enforced by the
// 'HashTableAnchor' type itself.  A 'HashTableAnchor' object meeting these
// requirements is said to be "well-formed" and
// 'HashTableImpUtil::isWellFormed' will return 'true' for such an object.  A
// 'HastTableAnchor' is considered well-formed for a particular key policy,
// 'KEY_POLICY', and hash functor, 'HASHER', if all of the following are true:
//
//: 1 The list refers to a well-formed doubly-linked list (see
//:   'bslalg_bidirectionallinklistutil'), where the previous address of the
//:   first node and the next address of the last node are 0.
//:
//: 2 Each link in the list is an instance of
//:   'BidirectionalNode<KEY_POLICY::ValueType>' 
//:
//: 3 Links in the doubly-linked list having the same adjusted hash value are
//:   contiguous, where the adjusted hash value is the value returned by
//:   'HashTableImpUtil::computeBucketIndex', for
//:   'HashTableImpUtil::extractKey<KEY_POLICY>(link)' and the size of the
//:   bucket array. 
//: 
//: 4 The first and last links in each bucket in the bucket array refer to a
//:   the first and last element in the doubly linked list having an adjusted
//:   hash value equal to that buckets index.  If no values in the doubly
//:   linked list have and adjust hash value equal to a bucket's index, then
//:   the addresses of the first and last links for that bcuket are 0.
//
///'HASHER', 'KEY_EQUAL' and 'KEY_POLICY' Template Parameters
///----------------------------------------------------------
// Several of the operations provided by 'HashTableImpUtil' are template
// functions.
//
///'KEY_POLICY'
/// - - - - - -
// The 'KEY_POLICY' template parameter must provide the the following type
// aliases and functions: 
//..
//  typedef <VALUE_TYPE> ValueType;
//  typedef <KEY_TYPE>   KeyType;
//  static const KeyType& extractKey(const ValueType& obj);
//..

//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#include <bslalg_hashtableanchor.h>
#endif

 #ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // for 'std::size_t', prefer bsls::Types::SizeType, but that is signed
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {
namespace bslalg {

                          // ======================
                          // class HashTableImpUtil
                          // ======================


struct HashTableImpUtil {
    // This 'struct' provides a namespace for a suite of utility functions
    // for creating and manipulating a hash table.
    // 

  private:
    // PRIVATE CLASS METHODS
    static HashTableBucket *findBucketForHashCode(
                                              const HashTableAnchor& hashTable,
                                              native_std::size_t     hashCode);
        // Return the address of the 'HashTableBucket' in the array of buckets
        // referred to by the specified hash-table 'anchor' whose index is the
        // adjusted value of the specified 'hashCode' (see
        // 'computeBucketIndex').  The behavior is undefined if 'hashTable'
        // has 0 buckets.


        
    static void spliceSegmentIntoBucket(BidirectionalLink  *cursor,
                                        BidirectionalLink  *nextCursor,
                                        HashTableBucket    *bucket,
                                        BidirectionalLink **newRoot);        
        // TBD (for Alisdair)
        // HMV 
        // See note at the top of the component.

    static bool bucketContainsLink(const HashTableBucket& bucket, 
                                   BidirectionalLink     *linkAddress);
        // Return true the specified 'link' is contained in the specified
        // 'bucket' and false otherwise.

    static
    bool isReachable(const BidirectionalLink *dst,
                     const BidirectionalLink *src);
        // Return 'true' if the specified 'linkAddress' is the address of one
        // of the links between the first and the last links referred 
        // to by the specified 'bucket'.
  
  public:
    // CLASS METHODS
    template<class KEY_POLICY>
    static const typename KEY_POLICY::KeyType& extractKey(
                                                const BidirectionalLink *link);
        // Return a reference providing non-modifiable access to the
        // key (of type 'KEY_POLICY::KeyType') held by the specified
        // 'link'.  The behavior is undefined unless 'link' refers to a node
        // of type 'BidirectionalNode<KEY_POLICY::ValueType>'.  'KEY_POLICY'
        // shall be a namespace providing the type names 'KeyType' and
        // 'ValueType', as well as a function that can be called as if it had
        // the following signature:
        //..
        //  const KeyType& extractKey(const ValueType& obj);
        //..

    template <class KEY_POLICY>
    static typename KEY_POLICY::ValueType& extractValue(
                                                      BidirectionalLink *link);
        // Return a reference provoding non-modifiable access to the
        // value (of type 'KEY_POLICY::ValueType') held by the specified
        // 'link'.  The behavior is undefined unless 'link' refers to a node
        // of type 'BidirectionalNode<KEY_POLICY::ValueType>'.  'KEY_POLICY'
        // shall be a namespace providing the type name 'ValueType'.

    
    template <class KEY_POLICY, class HASHER>
    static bool isWellFormed(const HashTableAnchor& hashTable);
        // Return 'true' if the specified 'hashTable' is well-formed.  For a
        // 'HastTableAnchor' to be considered well-formed for a particular key
        // policy, 'KEY_POLICY', and hash functor, 'HASHER', all of the
        // following must be true: 
        //
        //: 1 The 'hashTable.listRootAddress()' is the address of a
        //:   well-formed doubly-linked list (see
        //:   'bslalg_bidirectionallinklistutil'), where the 'previousLink'
        //:   attribute of the first node and the 'nextLink' attribute of the
        //:   last node are 0:
        //:
        //: 2 Each link in the list is an instance of
        //:   'BidirectionalNode<KEY_POLICY::ValueType>'  
        //:
        //: 3 Links in the doubly-linked list having the same adjusted hash
        //:   value are contiguous, where the adjusted hash value is the value
        //:   returned by 'computeBucketIndex', for
        //:   'extractKey<KEY_POLICY>(link)' and
        //:   'hashTable->bucketArraySize()'. 
        //: 
        //: 4 The first and last links in each bucket (in the bucket array,
        //:   hashTable->bucketArrayAddress()') refer to a the first and last 
        //:   element in the doubly linked list having an adjusted hash value
        //:   equal to that bucket's array index.  If no values in the doubly
        //:   linked list have an adjusted hash value equal to a bucket's
        //:   index, then the addresses of the first and last links for that
        //:   bucket are 0.
   
    static native_std::size_t computeBucketIndex(
                                                native_std::size_t hashCode,
                                                native_std::size_t numBuckets);
        // Return the index of the bucket referring to the elements whose
        // adjusted hash codes are the same as the adjusted value of the
        // specified 'hashCode', where 'hashCode' (and the 
        // hash-codes of the elements) are adjusted for the specified
        // 'numBuckets'.  The behavior is undefined if 'numBuckets' is 0.

    static void insertAtFrontOfBucket(HashTableAnchor    *hashTable,
                                      BidirectionalLink  *link,
                                      native_std::size_t  hashCode);
        // Insert, into the array of buckets referenced by the specified
        // 'anchor', the specified 'link' at the front of the bucket
        // associated to the specified 'hashCode'.  The behavior is undefined
        // unless 'link' references a node of type 'BidirectionalNode'
        // parametrized on the same type as the other 'BidirectionalNode'
        // values referenced by all the links accessible from
        // 'anchor->listRootAddress()', and unless 'hashCode' was computed
        // using a 'KEY_POLICY' and 'HASHER' types such that:
        // ..
        //   true == isWellFormed<KEY_POLICY, HASHER>(hashTable);
        // ..
    
    static void insertAtPosition(HashTableAnchor    *hashTable,
                                 BidirectionalLink  *link,
                                 native_std::size_t  hashCode,
                                 BidirectionalLink  *position);
        // Insert, in the specified hash-table 'anchor', the specified 'link',
        // into the bucket corresponding to the specified 'hashCode', before
        // the specified before the specified 'poistion'.  The behavior is
        // undefined unless 'position' belongs to
        // the bucket corresponding to 'hashCode' in 'anchor', and 'hashCode'
        // was obtained
        // from 'link' using a 'KEY_POLICY' and 'HASHER' types
        // such that:
        // ..
        //   true == isWellFormed<KEY_POLICY, HASHER>(anchor);
        // ..

        // HMV 
        // Insert the specified 'link', having the specified (non-adjusted)
        // 'hashCode', into the specified 'hashTable' immeditately before the
        // specified 'position' in the bi-directional linked list of
        // 'hashTable'.  The behavior is undefined unless 'position' is valid
        // link falling within the 'computeBucketIndex(hashCode)' bucket, and
        // for some combination of 'KEY_POLICY' and 'HASHER', 'hashTable' is
        // well-formed (see 'isWellFormed') and 'link' refers to a node of type
        // 'BidirectionalNode<KEY_POLICY::ValueType>', where
        // 'HASHER(extractKey<KEY_POLICY>(link))' returns 'hashCode'.

    static void remove(HashTableAnchor    *hashTable,
                       BidirectionalLink  *link,
                       native_std::size_t  hashCode);
        // Remove, in the specified hash-table 'anchor', the specified 'link',
         // from the bucket corresponding to the specified 'hashCode'.  The
        // behavior is undefined unless 'link' belongs to the bucket
        // corresponding to 'hashCode' in 'anchor', and 'hashCode' was obtained
        // from 'link' using a 'KEY_POLICY' and 'HASHER' types such
        // that:
        // ..
        //   true == isWellFormed<KEY_POLICY, HASHER>(anchor);        
        // ..


        // HMV
        // Remove the specified 'link', having the spacified (non-adjusted)
        // 'hashCode', from the specified 'hashTable'. 
        // 'anchor'.  The behavior is undefined unless, for some combination
        // of 'KEY_POLICY' and 'HASHER', 'hashTable' is well-formed (see
        // 'isWellFormed') and 'link' refers to a node in 'hashTable' for which
        // 'HASHER(extractKey<KEY_POLICY>(link))' returns 'hashCode'.

    template <class KEY_POLICY, class KEY_EQUAL>
    static BidirectionalLink *find(
                           const HashTableAnchor&              hashTable,
                           const typename KEY_POLICY::KeyType& key,
                           const KEY_EQUAL&                    equalityFunctor,
                           native_std::size_t                  hashCode);
        // Return the address, if found, of the first link in the specified
        // hash-table 'anchor' that holds a value matching the specified 'key'
        // according the specified parametrized type 'KEY_POLICY' and the
        // specified 'equalityFunctor', in the bucket storing elements having
        // the specified 'hashCode'.  Return 0, otherwise.  The behavior is
        // undefined unless 'link' belongs to the bucket corresponding to
        // 'hashCode' in 'anchor', and 'hashCode' was obtained from 'link'
        // using a 'KEY_POLICY' and 'HASHER' types such that: 
        // ..  
        //   true == isWellFormed<KEY_POLICY, HASHER>(anchor); 
        // ..

        // HMV
        // Return the add address of the first link in the bi-direction list
        // of the specified 'hashTable', having a value matching (according to
        // the specified 'equalityFunctor') the specified 'key' in the bucket
        // that holds elements with the specified 'hashCode' if such a
        // link exists, and return 0 otherwise.  The behavior is undefined
        // unless, for the provided 'KEY_POLICY' and some hash function,
        // 'HASHER', 'hashTable' is well-formed (see 'isWellFormed') and
        // 'HASHER(key)' returns 'hashCode'.  'KEY_POLICY' shall be a
        // namespace providing the type names 'KeyType' and 'ValueType', as
        // well as a function that can be called as if it had the following
        // signature: 
        //..
        //  const KeyType& extractKey(const ValueType& obj);
        //..
        // 'KEY_EQUAL' shall be a functor that can be called as if it had the
        // following signature:
        //..
        //  bool operator()(const KEY_POLICY::KeyType& key1,
        //                  const KEY_POLICY::KeyType& key2)
        //..

    template <class KEY_POLICY, class HASHER>
    static void rehash(HashTableAnchor   *newHashTable,
                       BidirectionalLink *elementList,
                       const HASHER&      hasher);
        // Redistribute, into the specified 'newAnchor', all the links starting
        // from the specified 'oldRoot', recomputing for each node the new
        // bucket index using the specified 'hasher' functor on the keys
        // extracted by the specified parametrized 'KEY_POLICY' type.  Note
        // that this function is not exception safe in the presence of a
        // throwing 'hash' functor. 

        // HMV: Must 'newHashTable' be empty? I've assumed so:
        
        // HMV
        // Populate the specified 'newHashTable' with all the elements
        // in the specified 'elementList', using the specified 'hasher' to
        // determine the (non-adjusted) hash code for each element.  This
        // operation provides the strong exception guarantee unless the
        // supplied 'hasher' throws, in which case it provides no exception
        // safety guarantee.  The behavior is undefined unless, 'newHashTable'
        // holds no elements and has one or more (empty) buckets, and
        // 'elementList' is a well-formed bi-directional list (see
        // 'BidirectionalLinkListUtil::isWellFormed') whose nodes are each of 
        // type 'BidirectionalNode<KEY_POLICY::ValueType>', the previous
        // address of the first node and the next address of the last node are
        // 0.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================


// HMV
// + re-order definitions per their declarations
// + Update parameter names per declarations

                        //-----------------------
                        // class HashTableImpUtil
                        //-----------------------

// PRIVATE CLASS METHODS
inline
HashTableBucket *HashTableImpUtil::findBucketForHashCode(
                                               const HashTableAnchor& anchor,
                                               native_std::size_t     hashCode)
{
    BSLS_ASSERT_SAFE(anchor.bucketArrayAddress());
    BSLS_ASSERT_SAFE(anchor.bucketArraySize());

    native_std::size_t bucketId = HashTableImpUtil::computeBucketIndex(
                                                     hashCode,
                                                     anchor.bucketArraySize());
    return &(anchor.bucketArrayAddress()[bucketId]);
}

inline
native_std::size_t HashTableImpUtil::computeBucketIndex(
                                                 native_std::size_t hashCode,
                                                 native_std::size_t numBuckets)
{
    BSLS_ASSERT_SAFE(0 != numBuckets);
    return hashCode % numBuckets;
}

inline
bool HashTableImpUtil::bucketContainsLink(const HashTableBucket&   bucket,
                                          BidirectionalLink       *linkAddress)
    // Return true the specified 'link' is contained in the specified 'bucket'
    // and false otherwise.
{
    BSLS_ASSERT_SAFE(!bucket.first() == !bucket.last());

    if(!bucket.first()) {
        return false;
    }

    const BidirectionalLink *cursor = bucket.first();
    while (bucket.last()->nextLink() != cursor) {
        if (cursor == linkAddress) {
            return true;
        }
        cursor = cursor->nextLink();
    }
    return false;
}

// CLASS METHODS
template<class KEY_POLICY>
inline
typename KEY_POLICY::ValueType& HashTableImpUtil::extractValue(
                                                       BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    typedef BidirectionalNode<typename KEY_POLICY::ValueType> BNode;
    return static_cast<BNode *>(link)->value();
}

template<class KEY_POLICY>
inline
const typename KEY_POLICY::KeyType& HashTableImpUtil::extractKey(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);
    typedef BidirectionalNode<typename KEY_POLICY::ValueType> BNode;
    const BNode *node = static_cast<const BNode *>(link);
    return KEY_POLICY::extractKey(node->value());
}

    // lookup
template <class KEY_POLICY, class KEY_EQUAL>
inline
BidirectionalLink *HashTableImpUtil::find(
                           const HashTableAnchor&              anchor,
                           const typename KEY_POLICY::KeyType& key,
                           const KEY_EQUAL&                    equalityFunctor,
                           native_std::size_t                  hashCode)
{
    BSLS_ASSERT_SAFE(anchor.bucketArrayAddress());
    BSLS_ASSERT_SAFE(anchor.bucketArraySize());

    const native_std::size_t bucketId = computeBucketIndex(
                                                     hashCode,
                                                     anchor.bucketArraySize());
    const HashTableBucket& bucket = anchor.bucketArrayAddress()[bucketId];

    // Odd loop structure as we must test on both first/last before terminating
    // the loop as not-found.

    if (BidirectionalLink *cursor = bucket.first()) {
        for ( ; ; cursor = cursor->nextLink() ) {
            if (equalityFunctor(key, extractKey<KEY_POLICY>(cursor))) {
                return cursor;
            }
            if (cursor == bucket.last()) {
                break;
            }
        }
    }

    return 0;
}

template <class KEY_POLICY, class HASHER>
inline
void HashTableImpUtil::rehash(HashTableAnchor   *newAnchor,
                              BidirectionalLink *oldRoot,
                              const HASHER&      hasher)
{
    BSLS_ASSERT_SAFE(newAnchor);
    BSLS_ASSERT_SAFE(oldRoot);           // empty lists do not need a rehash
    BSLS_ASSERT_SAFE(!oldRoot->previousLink());  // otherwise, not a 'root'

    // HMV
    // BSLS_ASSERT_SAFE(0 != newAnchor.bucketArraySize()); 

    BidirectionalLink *newRoot = 0;

    do {
        BidirectionalLink *cursor = oldRoot;
        HashTableBucket   *bucket = findBucketForHashCode(
                                       *newAnchor,
                                       hasher(extractKey<KEY_POLICY>(cursor)));

        BidirectionalLink *nextCursor  = cursor;

        // Walk list of nodes that will rehash to the same bucket
        // This will advance the list extraction point *before* we splice

        while ((oldRoot = oldRoot->nextLink()) &&
                bucket == findBucketForHashCode(
                                    *newAnchor,
                                    hasher(extractKey<KEY_POLICY>(oldRoot)))) {
             nextCursor = oldRoot;
        }

        spliceSegmentIntoBucket(cursor, nextCursor, bucket, &newRoot);
    }
    while (oldRoot);

    newAnchor->setListRootAddress(newRoot);
}

template <class KEY_POLICY, class HASHER>
inline
bool HashTableImpUtil::isWellFormed(const HashTableAnchor& anchor)
{
    HashTableBucket    *array = anchor.bucketArrayAddress();
    native_std::size_t  size  = anchor.bucketArraySize();
    BidirectionalLink  *root  = anchor.listRootAddress();

    // Check that all nodes are in the correct bucket.

    if (size) {
        BidirectionalLink *cursor = anchor.listRootAddress();
        while (cursor) {
            size_t hash  = HASHER()(extractKey<KEY_POLICY>(cursor));
            size_t index = computeBucketIndex(hash, size);
            if (!bucketContainsLink(array[index], cursor)) {
                return false;                                         // RETURN
            }
            cursor = cursor->nextLink();
        }
    }


    // Check that all the buckets have consistent 'first' and 'last', and that
    // 'first' and 'last', if not 0, are part of the list of elements.

    for (size_t i = 0; i < size; ++i) {

        HashTableBucket& bucket = array[i];

        if (!bucket.first() != !bucket.last()) {  // 'first' XOR 'second
            return false;
        }

        if (bucket.first()) {
            if (   !isReachable(bucket.first(), root)
                || !isReachable(bucket.last(),  root)) {
                return false;                                         // RETURN
            }
            BidirectionalLink *cursor = bucket.first();
            while (cursor != bucket.last()->nextLink()) {
                size_t hash  = HASHER()(extractKey<KEY_POLICY>(cursor));
                size_t index = computeBucketIndex(hash, size);
                if (!bucketContainsLink(array[index], cursor)) {
                    return false;                                     // RETURN
                }
            cursor = cursor->nextLink();
            }
        }
    }
    return true;
}

} // namespace BloombergLP::bslalg
} // namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
