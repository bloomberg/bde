// bslalg_hashtableimputil.h                                          -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEIMPUTIL
#define INCLUDED_BSLALG_HASHTABLEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide algorithms for implementing a hash table.
//
//@CLASSES:
//  bslalg::HashTableImpUtil: functions used to implement a hash table
//
//@SEE_ALSO: bslalg_bidirectionallinklistutil, bslalg_hashtableanchor,
//           bslstl_hashtable
//
//@DESCRIPTION: This component provides a namespace for utility functions used
// to implement a hash table container.  Almost all the functions provided by
// this component operate on a 'HashTableAnchor', a type encapsulating the key
// data members of a hash table.
//
///Hash Table Structure
///--------------------
// The utilities provided by this component are used to create and manipulate
// a hash table that resolves collisions using a linked-list of elements
// (i.e., chaining).  Many of the operations provided by 'HashTableImpUtil'
// operate on a 'HashTableAnchor', which encapsulates the key data members of a
// hash table.  A 'HashTableAnchor' has the address of a single, doubly linked
// list holding all the elements in the hash table, as well as the address of
// an array of buckets.  Each bucket holds a reference to the first and last
// element in the linked-list whose *adjusted* *hash* *value* is equal to the
// index of the bucket.  Further, the functions in this component ensure (and
// require) that all elements that fall within a bucket form a contiguous
// sequence in the linked list, as can be seen in the
// diagram below:
//..
//  FIG 1: a hash table holding 5 elements
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
// The C++11 standard defines a hash function as a function 'h(k)' returning
// (integral) values of type 'size_t', such that, for two different values of
// 'k1' and 'k2', the probability that 'h(k1) == h(k2)' is true should approach
// '1.0 / numeric_limits<size_t>::max()' (see 17.6.3.4 [hash.requirements]).
// Such a function 'h(k)' may return values within the entire range of values
// that can be described using 'size_t', [0 ..  numeric_limits<size_t>::max()],
// however the array of buckets maintained by a hash table is typically
// significantly smaller than 'number_limits<size_t>::max()', therefore a
// hash-table implementation must adjust the returned hash function so that it
// falls in the valid range of bucket indices (typically either using an
// integer division or modulo operation) -- we refer to this as the *adjusted*
// *hash* *value*.  Note that currently 'HashTableImpUtil' adjusts the value
// returned by a supplied hash function using 'operator%' (modulo), which
// is more resilient to pathological behaviors when used in conjunction with a
// hash function that may produce contiguous hash values (with the 'div' method
// lower order bits do not participate to the final adjusted value); however,
// the means of adjustment may change in the future.
//
///Well-Formed 'HashTableAnchor' Objects
///-------------------------------------
// Many of the algorithms defined in this component operate on
// 'HashTableAnchor' objects, which describe the attributes of a hash table.
// The 'HashTableAnchor' objects supplied to 'HashTableImpUtil' are required
// to meet a series of constraints that are not enforced by the
// 'HashTableAnchor' type itself.  A 'HashTableAnchor' object meeting these
// requirements is said to be "well-formed" and the method
// 'HashTableImpUtil::isWellFormed' returns 'true' for such an object.  A
// 'HastTableAnchor' is considered well-formed for a particular key policy,
// 'KEY_CONFIG', and hash functor, 'HASHER', if all of the following are true:
//
//: 1 The list refers to a well-formed doubly linked list (see
//:   'bslalg_bidirectionallinklistutil').
//:
//: 2 Each link in the list is an object of type
//:   'BidirectionalNode<KEY_CONFIG::ValueType>'
//:
//: 3 For each bucket, the range of nodes '[ bucket.first(), bucket.last() ]'
//:   contains all nodes in the hash table for which
//:   'computeBucketIndex(HASHER(extractKey(link)' is the index of the bucket,
//:   and no other nodes.
//
///'KEY_CONFIG' Template Parameter
///-------------------------------
// Several of the operations provided by 'HashTableImpUtil' are template
// functions parameterized on the typename 'KEY_CONFIG'.
//
///'KEY_CONFIG'
/// - - - - - -
// The 'KEY_CONFIG' template parameter must provide the the following type
// aliases and functions:
//..
//  typedef <VALUE_TYPE> ValueType;
//     // Alias for the type of the values stored by the 'BidirectionalNode'
//     // elements in the hash table.
//
//  typedef <KEY_TYPE> KeyType;
//     // Alias for the type of the key value extracted from the 'ValueType'
//     // stored in the 'BidirectionalNode' elements of a hash table.
//
//  static const KeyType& extractKey(const ValueType& obj);
//      // Return the 'KeyType' information associated with the specified
//      // 'object'.
//..
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a Hash Set
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to build a hash set that will keep track of keys stored in
// set.
//
// First, we define an abstract template class 'HashSet' that will provide a
// hash set for any type that has a copy constructor, a destructor, an equality
// comparator and a hash function.  We inherit from the 'HashTableAnchor' class
// use the the 'BidirectionalLinkListUtil' and 'HashTableImpUtil' classes to
// facilitate building the table:
//..
//  template <class KEY, class HASHER, class EQUAL>
//  class HashSet : public bslalg::HashTableAnchor {
//      // PRIVATE TYPES
//      typedef bslalg::BidirectionalLink         Link;
//      typedef bslalg::BidirectionalNode<KEY>    Node;
//      typedef bslalg::HashTableBucket           Bucket;
//      typedef bslalg::BidirectionalLinkListUtil ListUtil;
//      typedef bslalg::HashTableImpUtil          ImpUtil;
//      typedef native_std::size_t                size_t;
//
//      struct Policy {
//          typedef KEY KeyType;
//          typedef KEY ValueType;
//
//          static const KeyType& extractKey(const ValueType& value)
//          {
//              return value;
//          }
//      };
//
//      // DATA
//      double            d_maxLoadFactor;
//      unsigned          d_numNodes;
//      HASHER            d_hasher;
//      EQUAL             d_equal;
//      bslma::Allocator *d_allocator_p;
//
//      // PRIVATE MANIPULATORS
//      void grow();
//          // Roughly double the number of buckets, such that the number of
//          // buckets shall always be '2^N - 1'.
//
//      // PRIVATE ACCESSORS
//      bool checkInvariants() const;
//          // Perform sanity checks on this table, returning 'true' if all the
//          // tests pass and 'false' otherwise.  Note that many of the checks
//          // are done with the 'ASSERTV' macro and will cause messages to be
//          // written to the console.
//
//      Node* find(const KEY& key,
//                 size_t     hashCode) const;
//          // Return a pointer to the node containing the specified 'key', and
//          // 0 if no such node is in the table.
//
//    private:
//      // NOT IMPLEMENTED
//      HashSet(const HashSet&, bslma::Allocator *);
//      HashSet& operator=(const HashSet&);
//
//    public:
//      // CREATORS
//      explicit
//      HashSet(bslma::Allocator *allocator = 0);
//          // Create a 'HashSet', using the specified 'allocator'.  If no
//          // allocator is specified, use the default allocator.
//
//      ~HashSet();
//          // Destroy this 'HashSet', freeing all its memory.
//
//      // MANIPULATORS
//      bool insert(const KEY& key);
//          // If the specfied 'key' is not in this hash table, add it,
//          // returning 'true'.  If it is already in the table, return 'false'
//          // with no action taken.
//
//      bool erase(const KEY& key);
//          // If the specfied 'key' is in this hash table, remove it,
//          // returning 'true'.  If it is not found in the table, return
//          // 'false' with no action taken.
//
//      // ACCESSORS
//      native_std::size_t count(const KEY& key) const;
//          // Return 1 if the specified 'key' is in this table and 0
//          // otherwise.
//
//      native_std::size_t size() const;
//          // Return the number of discrete keys that are stored in this
//          // table.
//  };
//
//  // PRIVATE MANIPULATORS
//  template <class KEY, class HASHER, class EQUAL>
//  void HashSet<KEY, HASHER, EQUAL>::grow()
//  {
//      // 'bucketArraySize' will always be '2^N - 1', so that if hashed values
//      // are aligned by some 2^N they're likely to be relatively prime to the
//      // length of the hash table.
//
//      d_allocator_p->deallocate(bucketArrayAddress());
//      size_t newBucketArraySize = bucketArraySize() * 2 + 1;
//      setBucketArrayAddressAndSize((Bucket *) d_allocator_p->allocate(
//                                        newBucketArraySize * sizeof(Bucket)),
//                                        newBucketArraySize);
//
//      ImpUtil::rehash<Policy, HASHER>(this,
//                                      listRootAddress(),
//                                      d_hasher);
//  }
//
//  // PRIVATE ACCESSORS
//  template <class KEY, class HASHER, class EQUAL>
//  bool HashSet<KEY, HASHER, EQUAL>::checkInvariants() const
//  {
//..
// 'HashTableImpUtil's 'isWellFormed' will verify that all nodes are in their
// proper buckets, that there are no buckets containing nodes that are not in
// the main linked list, and no nodes in the main linked list that are not in
// buckets.  To verify that 'd_numNodes' is correct we have to traverse the
// list and count the nodes ourselves.
//..
//      size_t numNodes = 0;
//      for (BidirectionalLink *cursor = listRootAddress;
//                                       cursor; cursor = cursor->nextLink()) {
//          ++numNodes;
//      }
//
//      return size() == numNodes &&
//                  ImpUtil::isWellFormed<Policy, HASHER>(this, d_allocator_p);
//  }
//
//  template <class KEY, class HASHER, class EQUAL>
//  bslalg::BidirectionalNode<KEY> *HashSet<KEY, HASHER, EQUAL>::find(
//                                           const KEY&         key,
//                                           native_std::size_t hashCode) const
//  {
//      return (Node *) ImpUtil::find<Policy, EQUAL>(*this,
//                                                   key,
//                                                   d_equal,
//                                                   hashCode);
//  }
//
//  // CREATORS
//  template <class KEY, class HASHER, class EQUAL>
//  HashSet<KEY, HASHER, EQUAL>::HashSet(bslma::Allocator *allocator)
//  : HashTableAnchor(0, 0, 0)
//  , d_maxLoadFactor(0.4)
//  , d_numNodes(0)
//  {
//      enum { NUM_BUCKETS = 3 };    // 'NUM_BUCKETS' must be '2^N - 1' for
//                                   // some 'N'.
//
//      d_allocator_p = bslma::Default::allocator(allocator);
//      native_std::size_t bucketArraySizeInBytes =
//                                                NUM_BUCKETS * sizeof(Bucket);
//      setBucketArrayAddressAndSize(
//                  (Bucket *) d_allocator_p->allocate(bucketArraySizeInBytes),
//                  NUM_BUCKETS);
//      memset(bucketArrayAddress(), 0, bucketArraySizeInBytes);
//  }
//
//  template <class KEY, class HASHER, class EQUAL>
//  HashSet<KEY, HASHER, EQUAL>::~HashSet()
//  {
//      BSLS_ASSERT_SAFE(checkInvariants());
//
//      for (Link *link = listRootAddress(); link; ) {
//          Node *toDelete = (Node *) link;
//          link = link->nextLink();
//
//          toDelete->value().~KEY();
//          d_allocator_p->deallocate(toDelete);
//      }
//
//      d_allocator_p->deallocate(bucketArrayAddress());
//  }
//
//  // MANIPULATORS
//  template <class KEY, class HASHER, class EQUAL>
//  bool HashSet<KEY, HASHER, EQUAL>::erase(const KEY& key)
//  {
//      size_t hashCode = d_hasher(key);
//      Node *node = find(key, hashCode);
//
//      if (!node) {
//          return false;                                             // RETURN
//      }
//
//      size_t bucketIdx = ImpUtil::computeBucketIndex(hashCode,
//                                                     bucketArraySize());
//      Bucket& bucket = bucketArrayAddress()[bucketIdx];
//
//      BSLS_ASSERT_SAFE(bucket.first() && bucket.last());
//
//      if (bucket.first() == node) {
//          if (bucket.last() == node) {
//              bucket.reset();
//          }
//          else {
//              bucket.setFirst(node->nextLink());
//          }
//      }
//      else if (bucket.last() == node) {
//          bucket.setLast(node->previousLink());
//      }
//
//      if (listRootAddress() == node) {
//          setListRootAddress(node->nextLink());
//      }
//
//      ListUtil::unlink(node);
//
//      node->value().~KEY();
//      d_allocator_p->deallocate(node);
//
//      --d_numNodes;
//      BSLS_ASSERT_SAFE(checkInvariants());
//
//      return true;
//  }
//
//  template <class KEY, class HASHER, class EQUAL>
//  bool HashSet<KEY, HASHER, EQUAL>::insert(const KEY& key)
//  {
//      size_t hashCode = d_hasher(key);
//
//      if (find(key, hashCode)) {
//          // Already in set, do nothing.
//
//          return false;                                             // RETURN
//      }
//
//      if (bucketArraySize() * d_maxLoadFactor < d_numNodes + 1) {
//          grow();
//      }
//
//      ++d_numNodes;
//      Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//      bslalg::ScalarPrimitives::copyConstruct(&node->value(),
//                                              key,
//                                              d_allocator_p);
//
//      ImpUtil::insertAtBackOfBucket(this, node, hashCode);
//
//      BSLS_ASSERT_SAFE(find(key, hashCode));
//      BSLS_ASSERT_SAFE(checkInvariants());
//
//      return true;
//  }
//
//  // ACCESSORS
//  template <class KEY, class HASHER, class EQUAL>
//  native_std::size_t HashSet<KEY, HASHER, EQUAL>::count(const KEY& key) const
//  {
//      return 0 != find(key, d_hasher(key));
//  }
//
//  template <class KEY, class HASHER, class EQUAL>
//  native_std::size_t HashSet<KEY, HASHER, EQUAL>::size() const
//  {
//      return d_numNodes;
//  }
//..
// Then, we customize our table to manipulate zero-terminated 'const char *'
// strings.  We make the simplifying assumption that the strings pointed at by
// the 'const char *'s are longer-lived that the 'HashSet' will be.  We must
// provide an equality comparator so that two copies, in different locations,
// of the same sequence of characters will evaluate equal:
//..
//  struct StringEqual {
//      bool operator()(const char *lhs, const char *rhs) const
//      {
//          return !strcmp(lhs, rhs);
//      }
//  };
//..
// Next, we must provide a string hash function to convert a 'const char *' to
// a 'size_t':
//..
//  struct StringHash {
//      native_std::size_t operator()(const char *string) const;
//  };
//
//  native_std::size_t StringHash::operator()(const char *string) const
//  {
//      enum { BITS_IN_SIZE_T = sizeof(size_t) * 8 };
//
//      native_std::size_t result = 0;
//      for (int shift = 0; *string;
//                            ++string, shift = (shift + 7) % BITS_IN_SIZE_T) {
//          unsigned char c = *string;
//          if (shift <= BITS_IN_SIZE_T - 8) {
//              result += c << shift;
//          }
//          else {
//              result += c << shift;
//              result += c >> (BITS_IN_SIZE_T - shift);
//          }
//      }
//
//      return result;
//  };
//..
// Then, we declare a couple of 'TestAllocator's to use during our example:
//..
//  bslma::TestAllocator da("defaultAllocator");
//  bslma::DefaultAllocatorGuard defaultGuard(&da);
//
//  bslma::TestAllocator ta("testAllocator");
//..
// Next, in 'main', we create an instance of our 'HashSet' type, configured to
// contain 'const char *' strings:
//..
//  HashSet<const char *, StringHash, StringEqual> hs(&ta);
//..
// Then, we insert a few values:
//..
//  assert(1 == hs.insert("woof"));
//  assert(1 == hs.insert("arf"));
//  assert(1 == hs.insert("meow"));
//..
// Next, we attempt to insert a redundant value, and observe that the 'insert'
// method returns 'false' to indicate that the insert was refused:
//..
//  assert(0 == hs.insert("woof"));
//..
// Then, we use to 'size' method to observe that there are 3 strings stored in
// our 'HashSet':
//..
//  assert(3 == hs.size());
//..
// Next, we use the 'count' method to observe, specifically, which strings are
// and are not in our 'HashSet':
//..
//  assert(1 == hs.count("woof"));
//  assert(1 == hs.count("arf"));
//  assert(1 == hs.count("meow"));
//  assert(0 == hs.count("ruff"));
//  assert(0 == hs.count("chomp"));
//..
// Then, we attempt to erase a string which is not in our 'HashSet' and observe
// that 'false' is returned, which tells us the 'erase' attempt was
// unsuccessful:
//..
//  assert(0 == hs.erase("ruff"));
//..
// Next, we erase the string "meow", which is stored in our 'HashSet' and
// observe that 'true' is returned, telling us the 'erase' attempt succeeded:
//..
//  assert(1 == hs.erase("meow"));
//..
// Now, we use the 'size' method to verify there are 2 strings remaining in our
// 'HashSet':
//..
//  assert(2 == hs.size());
//..
// Finally, we use the 'count' method to observe specifically which strings are
// still in our 'HashSet'.  Note that "meow" is no longer there.  We observe
// that the default allocator was never used.  When we leave the block, our
// 'HashSet' will be destroyed, freeing its memory, then our 'TestAllocator'
// will be destroyed, verifying that our destructor worked correctly and that
// no memory was leaked:
//..
//  assert(1 == hs.count("woof"));
//  assert(1 == hs.count("arf"));
//  assert(0 == hs.count("meow"));
//  assert(0 == hs.count("ruff"));
//  assert(0 == hs.count("chomp"));
//
//  assert(0 == da.numAllocations());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTUTIL
#include <bslalg_bidirectionallinklistutil.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#include <bslalg_hashtableanchor.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORGUARD
#include <bslma_deallocatorguard.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {
namespace bslalg {

                    // =======================================
                    // class HashTableImpUtil_ExtractKeyResult
                    // =======================================

template <class KEY_CONFIG>
struct HashTableImpUtil_ExtractKeyResult {

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    struct ConstMatch      { char dummy[ 1]; };
    struct NonConstMatch   { char dummy[17]; };
    struct ConversionMatch { char dummy[65]; };

    struct Impl {
        template <class ARG>
        static ConstMatch test(const KeyType& (*)(const ARG &));

        template<class ARG>
        static NonConstMatch test(KeyType& (*)(ARG &));

        template<class RESULT, class ARG>
        static ConversionMatch test(RESULT (*)(ARG));
    };

    enum { RESULT_SELECTOR = sizeof(Impl::test(&KEY_CONFIG::extractKey)) };

    typedef typename bsl::conditional<RESULT_SELECTOR == sizeof(ConstMatch),
                                      const KeyType&,
            typename bsl::conditional<RESULT_SELECTOR == sizeof(NonConstMatch),
                                      KeyType&,
                                      KeyType>::type>::type Type;
};

                          // ======================
                          // class HashTableImpUtil
                          // ======================

struct HashTableImpUtil {
    // This 'struct' provides a namespace for a suite of utility functions
    // for creating and manipulating a hash table.

  private:
    // PRIVATE TYPES
    typedef native_std::size_t size_t;

    // PRIVATE CLASS METHODS
    static HashTableBucket *findBucketForHashCode(
                                              const HashTableAnchor& anchor,
                                              native_std::size_t     hashCode);
        // Return the address of the 'HashTableBucket' in the array of buckets
        // referred to by the specified hash-table 'anchor' whose index is the
        // adjusted value of the specified 'hashCode' (see
        // 'computeBucketIndex').  The behavior is undefined if 'anchor'
        // has 0 buckets.

  public:
    // CLASS METHODS
    static bool bucketContainsLink(const HashTableBucket&  bucket,
                                   BidirectionalLink      *linkAddress);
        // Return 'true' if the specified 'linkAddress' is the address of one
        // of the links in the list of elements in the closed range
        // '[bucket.first(), bucket.last()]'.

    template<class KEY_CONFIG>
    static typename HashTableImpUtil_ExtractKeyResult<KEY_CONFIG>::Type
                                           extractKey(BidirectionalLink *link);
        // Return a reference providing non-modifiable access to the
        // key (of type 'KEY_CONFIG::KeyType') held by the specified
        // 'link'.  The behavior is undefined unless 'link' refers to a node
        // of type 'BidirectionalNode<KEY_CONFIG::ValueType>'.  'KEY_CONFIG'
        // shall be a namespace providing the type names 'KeyType' and
        // 'ValueType', as well as a function that can be called as if it had
        // the following signature:
        //..
        //  const KeyType& extractKey(const ValueType& obj);
        //..

    template <class KEY_CONFIG>
    static typename KEY_CONFIG::ValueType& extractValue(
                                                      BidirectionalLink *link);
        // Return a reference providing non-modifiable access to the
        // value (of type 'KEY_CONFIG::ValueType') held by the specified
        // 'link'.  The behavior is undefined unless 'link' refers to a node
        // of type 'BidirectionalNode<KEY_CONFIG::ValueType>'.  'KEY_CONFIG'
        // shall be a namespace providing the type name 'ValueType'.

    template <class KEY_CONFIG, class HASHER>
    static bool isWellFormed(const HashTableAnchor&  anchor,
                             const HASHER&           hasher,
                             bslma::Allocator       *allocator = 0);
        // Return 'true' if the specified 'anchor' is well-formed for the
        // specified 'hasher'.  Use the specified 'allocator' for temporary
        // memory, or the default allocator if none is specified.  For a
        // 'HashTableAnchor' to be considered well-formed for a particular key
        // policy, 'KEY_CONFIG', and hash functor, 'hasher', all of the
        // following must be true:
        //
        //: 1 The 'anchor.listRootAddress()' is the address of a
        //:   well-formed doubly linked list (see
        //:   'bslalg_bidirectionallinklistutil').
        //:
        //: 2 Links in the doubly linked list having the same adjusted hash
        //:   value are contiguous, where the adjusted hash value is the value
        //:   returned by 'computeBucketIndex', for
        //:   'extractKey<KEY_CONFIG>(link)' and 'anchor.bucketArraySize()'.
        //:
        //: 3 Links in the doubly linked list having the same hash value are
        //:   contiguous.
        //:
        //: 4 The first and last links in each bucket (in the bucket array,
        //:   anchor.bucketArrayAddress()') refer to a the first and last
        //:   element in the well-formed doubly linked list of all nodes in the
        //:   list having an adjusted hash value equal to that bucket's array
        //:   index.  If no values in the doubly linked list have an adjusted
        //:   hash value equal to a bucket's index, then the addresses of the
        //:   first and last links for that bucket are 0.

    static native_std::size_t computeBucketIndex(
                                                native_std::size_t hashCode,
                                                native_std::size_t numBuckets);
        // Return the index of the bucket referring to the elements whose
        // adjusted hash codes are the same as the adjusted value of the
        // specified 'hashCode', where 'hashCode' (and the
        // hash-codes of the elements) are adjusted for the specified
        // 'numBuckets'.  The behavior is undefined if 'numBuckets' is 0.

    static void insertAtFrontOfBucket(HashTableAnchor    *anchor,
                                      BidirectionalLink  *link,
                                      native_std::size_t  hashCode);
        // Insert the specified 'link', having the specified (non-adjusted)
        // 'hashCode',  into the the specified 'anchor', at the front of the
        // bucket with index
        // 'computeBucketIndex(hashCode, anchor->bucketArraySize())'.  The
        // behavior is undefined unless 'anchor' is well-formed (see
        // 'isWellFormed') for some combination of 'KEY_CONFIG' and
        // 'HASHER' such that 'link' refers to a node of type
        // 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    static void insertAtBackOfBucket(HashTableAnchor    *anchor,
                                     BidirectionalLink  *link,
                                     native_std::size_t  hashCode);
        // Insert the specified 'link', having the specified (non-adjusted)
        // 'hashCode', into the the specified 'anchor', into the bucket with
        // index 'computeBucketIndex(hashCode, anchor->bucketArraySize())',
        // after the last node in the bucket.  The behavior is undefined unless
        // 'anchor' is well-formed (see 'isWellFormed') for some combination of
        // 'KEY_CONFIG' and 'HASHER' such that 'link' refers to a node of type
        // 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    static void insertAtPosition(HashTableAnchor    *anchor,
                                 BidirectionalLink  *link,
                                 native_std::size_t  hashCode,
                                 BidirectionalLink  *position);
        // Insert the specified 'link', having the specified (non-adjusted)
        // 'hashCode', into the specified 'anchor' immediately before the
        // specified 'position' in the bi-directional linked list of 'anchor'.
        // The behavior is undefined unless position is in the bucket having
        // index 'computeBucketIndex(hashCode, anchor->bucketArraySize())' and
        // 'anchor' is well-formed (see 'isWellFormed') for some combination of
        // 'KEY_CONFIG' and 'HASHER' such that 'link' refers to a node of type
        // 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    static void remove(HashTableAnchor    *anchor,
                       BidirectionalLink  *link,
                       native_std::size_t  hashCode);
        // Remove the specified 'link', having the specified (non-adjusted)
        // 'hashCode', from the specified 'anchor'.  The behavior is undefined
        // unless 'anchor' is well-formed (see 'isWellFormed') for some
        // combination of 'KEY_CONFIG' and 'HASHER' such that 'link' refers to
        // a node of type 'BidirectionalNode<KEY_CONFIG::ValueType>' and
        // 'HASHER(extractKey<KEY_CONFIG>(link))' returns 'hashCode'.

    template <class KEY_CONFIG, class KEY_EQUAL>
    static BidirectionalLink *find(
              const HashTableAnchor&                                    anchor,
              typename HashTableImpUtil_ExtractKeyResult<KEY_CONFIG>::Type key,
              const KEY_EQUAL&                                 equalityFunctor,
              native_std::size_t                                     hashCode);
        // Return the address of the first link in the list element of
        // the specified 'anchor', having a value matching (according to the
        // specified 'equalityFunctor') the specified 'key' in the bucket that
        // holds elements with the specified 'hashCode' if such a link exists,
        // and return 0 otherwise.  The behavior is undefined unless, for the
        // provided 'KEY_CONFIG' and some hash function, 'HASHER', 'anchor' is
        // well-formed (see 'isWellFormed') and 'HASHER(key)' returns
        // 'hashCode'.  'KEY_CONFIG' shall be a
        // namespace providing the type names 'KeyType' and 'ValueType', as
        // well as a function that can be called as if it had the following
        // signature:
        //..
        //  const KeyType& extractKey(const ValueType& obj);
        //..
        // 'KEY_EQUAL' shall be a functor that can be called as if it had the
        // following signature:
        //..
        //  bool operator()(const KEY_CONFIG::KeyType& key1,
        //                  const KEY_CONFIG::KeyType& key2)
        //..

    template <class KEY_CONFIG, class HASHER>
    static void rehash(HashTableAnchor   *newAnchor,
                       BidirectionalLink *elementList,
                       const HASHER&      hasher);
        // Populate the specified 'newHashTable' with all the elements in the
        // specified 'elementList', using the specified 'hasher' to determine
        // the (non-adjusted) hash code for each element.  This operation
        // provides the strong exception guarantee unless the supplied 'hasher'
        // throws, in which case it provides no exception safety guarantee.
        // The buckets in the array in 'newAnchor' and the list root address in
        // 'newAnchor' are assumed to be garbage and overwritten.  The behavior
        // is undefined unless, 'newHashTable' holds no elements and has one or
        // more (empty) buckets, and 'elementList' is a well-formed
        // bi-directional list (see 'BidirectionalLinkListUtil::isWellFormed')
        // whose nodes are each of type
        // 'BidirectionalNode<KEY_CONFIG::ValueType>', the previous address of
        // the first node and the next address of the last node are 0.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

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

    for (BidirectionalLink *cursor     = bucket.first(),
                           * const end = bucket.end(); end != cursor;
                                                 cursor = cursor->nextLink()) {
        if (linkAddress == cursor) {
            return true;                                              // RETURN
        }

        BSLS_ASSERT_SAFE(cursor);
    }

    return false;
}

// CLASS METHODS
template<class KEY_CONFIG>
inline
typename KEY_CONFIG::ValueType& HashTableImpUtil::extractValue(
                                                       BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    typedef BidirectionalNode<typename KEY_CONFIG::ValueType> BNode;
    return static_cast<BNode *>(link)->value();
}

template<class KEY_CONFIG>
inline
typename HashTableImpUtil_ExtractKeyResult<KEY_CONFIG>::Type
HashTableImpUtil::extractKey(BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    typedef BidirectionalNode<typename KEY_CONFIG::ValueType> BNode;

    BNode *node = static_cast<BNode *>(link);
    return KEY_CONFIG::extractKey(node->value());
}

template <class KEY_CONFIG, class KEY_EQUAL>
inline
BidirectionalLink *HashTableImpUtil::find(
  const HashTableAnchor&                                       anchor,
  typename HashTableImpUtil_ExtractKeyResult<KEY_CONFIG>::Type key,
  const KEY_EQUAL&                                             equalityFunctor,
  native_std::size_t                                           hashCode)
{
    BSLS_ASSERT_SAFE(anchor.bucketArrayAddress());
    BSLS_ASSERT_SAFE(anchor.bucketArraySize());

    const HashTableBucket *bucket = findBucketForHashCode(anchor, hashCode);
    BSLS_ASSERT_SAFE(bucket);

    for (BidirectionalLink *cursor     = bucket->first(),
                           * const end = bucket->end();
                                 end != cursor; cursor = cursor->nextLink() ) {
        if (equalityFunctor(key, extractKey<KEY_CONFIG>(cursor))) {
            return cursor;                                            // RETURN
        }
    }

    return 0;
}

template <class KEY_CONFIG, class HASHER>
void HashTableImpUtil::rehash(HashTableAnchor   *newAnchor,
                              BidirectionalLink *elementList,
                              const HASHER&      hasher)
{
    BSLS_ASSERT_SAFE(newAnchor);
    BSLS_ASSERT_SAFE(newAnchor->bucketArrayAddress());
    BSLS_ASSERT_SAFE(0 != newAnchor->bucketArraySize());
    BSLS_ASSERT_SAFE(!elementList || !elementList->previousLink());

    class Proctor {
        // An object of this proctor class guarantees that, on leaving scope,
        // any remaining elements in the original specified 'elementList' are
        // spliced to the front of the list rooted in the specified 'newAnchor'
        // so that there is only one list for the client to clear if an
        // exception is thrown by a user supplied hash functor.  Note that it
        // might be possible to avoid creating such a proctor in C++11 if the
        // hash functor is determined to be 'noexcept'.

      private:
        BidirectionalLink **d_sourceList;
        HashTableAnchor    *d_targetAnchor;

#if !defined(BSLS_PLATFORM_CMP_MSVC)           // Microsoft warns if these
        Proctor(const Proctor&); // = delete;  // methods are declared private.
        Proctor& operator=(const Proctor&); // = delete;
#endif

      public:
        Proctor(BidirectionalLink **sourceList,
                HashTableAnchor    *targetAnchor)
        : d_sourceList(sourceList)
        , d_targetAnchor(targetAnchor)
        {
            BSLS_ASSERT(sourceList);
            BSLS_ASSERT(targetAnchor);
        }

        ~Proctor()
        {
            if (BidirectionalLink *lastLink = *d_sourceList) {
                for( ; lastLink->nextLink(); lastLink = lastLink->nextLink()) {
                    // This loop body is intentionally left blank.
                }
                BidirectionalLinkListUtil::spliceListBeforeTarget(
                                           *d_sourceList,
                                            lastLink,
                                            d_targetAnchor->listRootAddress());
            }
        }
    };

    // The callers of this function should be rewritten to take into account
    // that it is the responsibility of this function, not its callers, to zero
    // out the buckets.

    for (void **cursor     = (void **)  newAnchor->bucketArrayAddress(),
              ** const end = (void **) (newAnchor->bucketArrayAddress() +
                                        newAnchor->bucketArraySize());
                                                      cursor < end; ++cursor) {
        *cursor = 0;
    }
    newAnchor->setListRootAddress(0);

    Proctor enforceSingleListOnExit(&elementList, newAnchor);

    while (elementList) {
        BidirectionalLink *nextNode = elementList;
        elementList = elementList->nextLink();

        insertAtBackOfBucket(newAnchor,
                             nextNode,
                             hasher(extractKey<KEY_CONFIG>(nextNode)));
    }
}

template <class KEY_CONFIG, class HASHER>
bool HashTableImpUtil::isWellFormed(const HashTableAnchor&  anchor,
                                    const HASHER&           hasher,
                                    bslma::Allocator       *allocator)
{
    HashTableBucket    *array = anchor.bucketArrayAddress();
    size_t              size  = anchor.bucketArraySize();
    BidirectionalLink  *root  = anchor.listRootAddress();

    if (!array || !size) {
        return false;                                                 // RETURN
    }

    if (!root) {
        // An empty list, so there should be no pointers set in the bucket
        // array.
        for (size_t i = 0; i < size; ++i) {
            const HashTableBucket& b = array[i];
            if  (b.first() || b.last()) {
                return false;                                         // RETURN
            }
        }

        return true;                                                  // RETURN
    }

    if (!allocator) {
        allocator = bslma::Default::defaultAllocator();
    }

    bool *bucketsUsed = (bool *) allocator->allocate(size);
    bslma::DeallocatorGuard<bslma::Allocator> guard(bucketsUsed, allocator);
    for (size_t i = 0; i < size; ++i) {
        bucketsUsed[i] = false;
    }

    size_t hash = hasher(extractKey<KEY_CONFIG>(root));
    size_t bucketIdx = computeBucketIndex(hash, size);
    if (array[bucketIdx].first() != root) {
        return false;                                                 // RETURN
    }

    bucketsUsed[bucketIdx] = true;

    BidirectionalLink *prev = root;
    size_t prevBucketIdx    = bucketIdx;
    while (BidirectionalLink *cursor = prev->nextLink()) {
        if (cursor->previousLink() != prev) {
            return false;                                             // RETURN
        }

        hash      = hasher(extractKey<KEY_CONFIG>(cursor));
        bucketIdx = computeBucketIndex(hash, size);

        if (bucketIdx != prevBucketIdx) {
            // New bucket

            // We should be the first node in the new bucket, so if this
            // bucket's been visited before, it's an error.

            if (bucketsUsed[bucketIdx]) {
                return false;                                         // RETURN
            }
            bucketsUsed[bucketIdx] = true;

            // Since we're the first node in the bucket, bucket.first()
            // should point at us.

            if (array[bucketIdx].first() != cursor) {
                return false;                                         // RETURN
            }

            // 'last()' of the previous bucket should point at the
            // previous node.

            if (array[prevBucketIdx].last() != prev) {
                return false;                                         // RETURN
            }
        }

        // Set 'prev' variables for next iteration
        prev          = cursor;
        prevBucketIdx = bucketIdx;
    }

    if (array[prevBucketIdx].last() != prev) {
        return false;                                                 // RETURN
    }

    // Check that traversing the root list traversed all non-empty buckets.

    for (size_t i = 0; i < size; ++i) {
        const HashTableBucket& b = array[i];
        if (bucketsUsed[i]) {
            if  (!b.first() || !b.last()) {
                return false;                                         // RETURN
            }
        }
        else if ( b.first() ||  b.last()) {
            return false;                                             // RETURN
        }
    }

    return true;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
