// bslalg_hashtableanchor.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEANCHOR
#define INCLUDED_BSLALG_HASHTABLEANCHOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type holding the constituent parts of a hash table.
//
//@CLASSES:
// bslalg::HashTableAnchor: (in-core) bucket-array and node list
//
//@SEE_ALSO: bslstl_hashtable, bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides a single, complex-constrained
// *in*-*core* (value-semantic) attribute class, 'bslalg::HashTableAnchor',
// that is used to hold (not own) the array of buckets and the list of nodes
// that form the key data elements of a hash-table.  This class is typically
// used with the utilities provided in 'bslstl_hashtableimputil'.  Note that
// the decision to store nodes in a linked list (i.e., resolving collisions
// through chaining) is intended to facilitate a hash-table implementation
// meeting the requirements of a C++11 standard unordered container.
//
///Attributes
///----------
//..
//  Name                Type                  Simple Constraints
//  ------------------  -------------------   ------------------
//  bucketArrayAddress  HashTableBucket *     none
//
//  bucketArraySize     size_t                none
//
//  listRootAddress     BidirectionalLink *   none
//
//
//  Complex Constraint
//  -------------------------------------------------------------------------
//  'bucketArrayAddress' must refer to a contiguous sequence of valid
//  'bslalg::HashTableBucket' objects of at least the specified
//  'bucketArraySize' or both 'bucketArrayAddress' and 'bucketArraySize' must
//  be 0.
//..
//
//: o 'listRootAddress': address of the head of the linked list of nodes
//:   holding the elements contained in a hash table
//:
//: o 'bucketArrayAddress': address of the first element of the sequence of
//:   'HashTableBucket' objects, each of which refers to the first and last
//:   node (from 'listRootAddress') in that bucket
//:
//: o 'bucketArraySize': the number of (contiguous) buckets in the array of
//:   buckets at 'bucketArrayAddress'
//
///Usage
///-----
// This section illustrates intended use of this component.
//
// Suppose we want to create a hash table that keeps track of pointers.
// Pointers can be added ('insert'ed) or removed ('erase'd) from the table, and
// the table will keep track, at any time, of whether a pointer is currently
// stored in the table using the 'count' method.  It will also be table to
// return the total number of objects stored in the table (the 'size' method).
// Redundant 'insert's have no effect -- a given pointer may only be stored in
// the table once.
//
// First, we create our class:
//..
//  class PtrHashSet : public bslalg::HashTableAnchor {
//      // PRIVATE TYPES
//      typedef bsls::Types::UintPtr              UintPtr;
//      typedef bslalg::BidirectionalNode<void *> Node;
//      typedef bslalg::HashTableBucket           Bucket;
//      typedef bslalg::BidirectionalLinkListUtil Util;
//
//      // DATA
//      double            d_maxLoadFactor;
//      unsigned          d_numNodes;
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
//      bool find(Node **node, Bucket **bucket, const void *ptr) const;
//          // If the specified value 'ptr' is stored in this table, return
//          // pointers to its node and bucket in the specified 'node' and
//          // 'bucket'.  If it is not in this table, return the bucket it
//          // should be in, and a pointer to the first node, if any, in that
//          // bucket.  If the bucket is empty, return with
//          // '*node == listRootAddress()'.  Return 'true' if 'ptr' was found
//          // in the table and 'false' otherwise.  Note that it is permissible
//          // to pass 0 to 'node' and / or 'bucket', in which case these
//          // arguments are ignored.
//
//    private:
//      // NOT IMPLEMENTED
//      PtrHashSet(const PtrHashSet&, bslma::Allocator *);
//      PtrHashSet& operator=(const PtrHashSet&);
//
//    public:
//      // CREATORS
//      explicit
//      PtrHashSet(bslma::Allocator *allocator = 0);
//          // Create a 'PtrHashSet', using the specified 'allocator'.  If no
//          // allocator is specified, use the default allocator.
//
//      ~PtrHashSet();
//          // Destroy this 'PtrHashSet', freeing all its memory.
//
//      // MANIPULATORS
//      bool insert(void *ptr);
//          // If the specfied 'ptr' is not in this hash table, add it,
//          // returning 'true'.  If it is already in the table, return 'false'
//          // with no action taken.
//
//      bool erase(void *ptr);
//          // If the specfied 'ptr' is in this hash table, remove it,
//          // returning 'true'.  If it is not found in the table, return
//          // 'false' with no action taken.
//
//      // ACCESSORS
//      native_std::size_t count(void *ptr) const;
//          // Return 1 if the specified value 'ptr' is in this table and 0
//          // otherwise.
//
//      native_std::size_t size() const;
//          // Return the number of discrete values that are stored in this
//          // table.
//  };
//
//  // PRIVATE MANIPULATORS
//  void PtrHashSet::grow()
//  {
//      // 'bucketArraySize' will always be '2^N - 1', so that when pointers
//      // are aligned by some 2^N they're likely to be relatively prime.
//
//      native_std::size_t newBucketArraySize = bucketArraySize() * 2 + 1;
//      native_std::size_t newBucketArraySizeInBytes =
//                                         newBucketArraySize * sizeof(Bucket);
//      memset(bucketArrayAddress(), 0x5a, size() * sizeof(Bucket));
//      d_allocator_p->deallocate(bucketArrayAddress());
//      setBucketArrayAddressAndSize(
//               (Bucket *) d_allocator_p->allocate(newBucketArraySizeInBytes),
//               newBucketArraySize);
//      memset(bucketArrayAddress(), 0, newBucketArraySizeInBytes);
//      Node *newListRootAddress = 0;
//
//      unsigned numNodes = 0;
//      for (Node *node = (Node *) listRootAddress(); node; ++numNodes) {
//          Node *rippedOut = node;
//          node = (Node *) node->nextLink();
//
//          native_std::size_t index =
//                            (UintPtr) rippedOut->value() % bucketArraySize();
//          Bucket& bucket = bucketArrayAddress()[index];
//          if (bucket.first()) {
//              if (0 == bucket.first()->previousLink()) {
//                  newListRootAddress = rippedOut;
//              }
//              Util::insertLinkBeforeTarget(rippedOut, bucket.first());
//              bucket.setFirst(rippedOut);
//          }
//          else {
//              Util::insertLinkBeforeTarget(rippedOut,
//                                           newListRootAddress);
//              newListRootAddress = rippedOut;
//              bucket.setFirstAndLast(rippedOut, rippedOut);
//          }
//      }
//      assert(size() == numNodes);
//
//      setListRootAddress(newListRootAddress);
//
//      checkInvariants();
//  }
//
//  // PRIVATE ACCESSORS
//  bool PtrHashSet::checkInvariants() const
//  {
//      bool ok;
//
//      unsigned numNodes = 0;
//      Node *prev = 0;
//      for (Node *node = (Node *) listRootAddress(); node;
//                             prev = node, node = (Node *) node->nextLink()) {
//          ok = node->previousLink() == prev;
//          assert(ok && "node->previousLink() == prev");
//          if (!ok) return false;                                    // RETURN
//          ++numNodes;
//      }
//      ok = size() == numNodes;
//      assert(ok && "size() == numNodes");
//      if (!ok) return false;                                        // RETURN
//
//      numNodes = 0;
//      for (unsigned i = 0; i < bucketArraySize(); ++i) {
//          Bucket& bucket = bucketArrayAddress()[i];
//          if (bucket.first()) {
//              ++numNodes;
//              for (Node *node = (Node *) bucket.first();
//                                bucket.last() != node;
//                                          node = (Node *) node->nextLink()) {
//                  ++numNodes;
//              }
//          }
//          else {
//              ok = !bucket.last();
//              assert(ok && "!bucket.last()");
//              if (!ok) return false;                                // RETURN
//          }
//      }
//      ok = size() == numNodes;
//      assert(ok && "size() == numNodes");
//
//      return ok;
//  }
//
//  bool PtrHashSet::find(Node **node, Bucket **bucket, const void *ptr) const
//  {
//      Node   *dummyNodePtr;
//      Bucket *dummyBucketPtr;
//      if (!node  ) node   = &dummyNodePtr;
//      if (!bucket) bucket = &dummyBucketPtr;
//
//      Node *& nodePtrRef = *node;
//      unsigned index = (UintPtr) ptr % bucketArraySize();
//      Bucket& bucketRef = bucketArrayAddress()[index];
//      *bucket = &bucketRef;
//      if (bucketRef.first()) {
//          Node *begin      = (Node *) bucketRef.first();
//          Node * const end = (Node *) bucketRef.last()->nextLink();
//          for (Node *n = begin; end != n; n = (Node *) n->nextLink()) {
//              if (n->value() == ptr) {
//                  // found
//
//                  nodePtrRef = n;
//                  return true;                                      // RETURN
//              }
//          }
//          // not found
//
//          nodePtrRef = begin;
//          return false;                                             // RETURN
//      }
//      // empty bucket
//
//      nodePtrRef = (Node *) listRootAddress();
//      return false;
//  }
//
//  // CREATORS
//  PtrHashSet::PtrHashSet(bslma::Allocator *allocator)
//  : HashTableAnchor(0, 0, 0)
//  , d_maxLoadFactor(0.4)
//  , d_numNodes(0)
//  {
//      enum { NUM_BUCKETS = 3 };
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
//  PtrHashSet::~PtrHashSet()
//  {
//      BSLS_ASSERT_SAFE(checkInvariants());
//
//      for (Node *node = (Node *) listRootAddress(); node; ) {
//          Node *toDelete = node;
//          node = (Node *) node->nextLink();
//
//          memset(toDelete, 0x5a, sizeof(*toDelete));
//          d_allocator_p->deallocate(toDelete);
//      }
//
//      d_allocator_p->deallocate(bucketArrayAddress());
//  }
//
//  // MANIPULATORS
//  bool PtrHashSet::erase(void *ptr)
//  {
//      Bucket *bucket;
//      Node   *node;
//
//      if (!find(&node, &bucket, ptr)) {
//          return false;                                             // RETURN
//      }
//
//      if (bucket->first() == node) {
//          if (bucket->last() == node) {
//              bucket->reset();
//          }
//          else {
//              bucket->setFirst(node->nextLink());
//          }
//      }
//      else if (bucket->last() == node) {
//          bucket->setLast(node->previousLink());
//      }
//
//      --d_numNodes;
//      Util::unlink(node);
//
//      d_allocator_p->deallocate(node);
//
//      checkInvariants();
//
//      return true;
//  }
//
//  bool PtrHashSet::insert(void *ptr)
//  {
//      Bucket *bucket;
//      Node *insertionPoint;
//
//      if (find(&insertionPoint, &bucket, ptr)) {
//          // Already in set, do nothing.
//
//          return false;                                             // RETURN
//      }
//
//      if (bucketArraySize() * d_maxLoadFactor < d_numNodes + 1) {
//          grow();
//          bool found = find(&insertionPoint, &bucket, ptr);
//          BSLS_ASSERT_SAFE(!found);
//      }
//
//      ++d_numNodes;
//      Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//
//      Util::insertLinkBeforeTarget(node, insertionPoint);
//      node->value() = ptr;
//      if (listRootAddress() == insertionPoint) {
//          BSLS_ASSERT_SAFE(0 == node->previousLink());
//          setListRootAddress(node);
//      }
//
//      if (bucket->first()) {
//          BSLS_ASSERT_SAFE(bucket->first() == insertionPoint);
//
//          bucket->setFirst(node);
//      }
//      else {
//          BSLS_ASSERT_SAFE(!bucket->last());
//
//          bucket->setFirstAndLast(node, node);
//      }
//
//      assert(count(ptr));
//
//      checkInvariants();
//
//      return true;
//  }
//
//  // ACCESSORS
//  native_std::size_t PtrHashSet::count(void *ptr) const
//  {
//      return find(0, 0, ptr);
//  }
//
//  native_std::size_t PtrHashSet::size() const
//  {
//      return d_numNodes;
//  }
//..
// Then, in 'main', we create a test allocator for use in this example to
// ensure that no memory is leaked:
//..
//  bslma::TestAllocator ta("test", veryVeryVeryVerbose);
//..
// Next, we declare our table using that allocator:
//..
//  PtrHashSet phs(&ta);
//..
// Then, we create an area of memory from which our pointers will come:
//..
//  enum { SEGMENT_LENGTH = 1000 };
//  char *pc = (char *) ta.allocate(SEGMENT_LENGTH);
//..
// Next, we iterate through the length of the segment, insert those pointers
// for which 'ptr - pc == N * 7' is true.  We keep a count of the number of
// items we insert into the table in the variable 'sevens':
//..
//  unsigned sevens = 0;
//  for (int i = 0; i < SEGMENT_LENGTH; i += 7) {
//      ++sevens;
//      bool status = phs.insert(&pc[i]);
//      assert(status);
//  }
//..
// Then, we verify the number of objects we've placed in the table:
//..
//  assert(phs.size() == sevens);
//..
// Next, we iterate through ALL pointers in the 'pc' array, using the 'count'
// method to verify that the ones we expect are in the table:
//..
//  for (int i = 0; i < SEGMENT_LENGTH; ++i) {
//      assert(phs.count(&pc[i]) == (0 == i % 7));
//  }
//..
// Then, we iterate, deleting all elements from the table for which
// 'ptr - pc == 3 * N' is true.  We keep a count of the number of elements
// which were in the table which we delete in the variable 'killed':
//..
//  unsigned killed = 0;
//  for (int i = 0; i < SEGMENT_LENGTH; i += 3) {
//      const bool deleted = phs.erase(&pc[i]);
//      assert(deleted == (0 == i % 7));
//      killed += deleted;
//  }
//..
// Next, we verify the number of remaining elements in the table:
//..
//  assert(killed < sevens);
//  assert(phs.size() == sevens - killed);
//..
// Then, in verbose mode we print our tallies:
//..
//  if (verbose) {
//      printf("sevens = %u, killed = %u, phs.size() = %u\n", sevens,
//                                          killed, (unsigned) phs.size());
//  }
//..
// Now, we iterate through every element of the 'pc' array, verifying that the
// surviving elements are exactly those for which 'ptr - pc' was divisible by 7
// and not by 3:
//..
//  for (int i = 0; i < SEGMENT_LENGTH; ++i) {
//      const bool present = phs.count(&pc[i]);
//      assert(present == ((0 == i % 7) && (0 != i % 3)));
//  }
//..
// Finally, we clean up our 'pc' array:
//..
//  ta.deallocate(pc);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslalg {

struct HashTableBucket;  // This is known to be a POD struct.

                        // =============================
                        // class bslalg::HashTableAnchor
                        // =============================

class HashTableAnchor {
    // This complex constrained *in*-*core* (value-semantic) attribute class
    // characterizes the key data elements of a hash table.  See the
    // "Attributes" section under @DESCRIPTION in the component-level
    // documentation for/ information on the class attributes.  Note that the
    // class invariant is the identically the complex constraint of this
    // component.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization and default construction
    //: o is *in-core*
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    HashTableBucket     *d_bucketArrayAddress_p;  // address of the array of
                                                  // buckets (held, not owned)

    native_std::size_t   d_bucketArraySize;       // size of 'd_bucketArray'

    BidirectionalLink   *d_listRootAddress_p;     // head of the list of
                                                  // elements in the hash-table
                                                  // (held, not owned)

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(HashTableAnchor,
                                   bsl::is_trivially_copyable);

    // CREATORS
    HashTableAnchor(HashTableBucket    *bucketArrayAddress,
                    native_std::size_t  bucketArraySize,
                    BidirectionalLink  *listRootAddress);
        // Create a 'bslalg::HashTableAnchor' object having the specified
        // 'bucketArrayAddress', 'bucketArraySize', and 'listRootAddress'
        // attributes.  The behavior is undefined unless 'bucketArrayAddress'
        // refers to a contiguous sequence of valid 'bslalg::HashTableBucket'
        // objects of at least 'bucketArraySize' or unless both
        // 'bucketArrayAddress' and 'bucketArraySize' are 0.

    HashTableAnchor(const HashTableAnchor& original);
        // Create a 'bslalg::HashTableAnchor' object having the same value
        // as the specified 'original' object.

    // ~bslalg::HashTableAnchor(); = default
        // Destroy this object.

    // MANIPULATORS
    bslalg::HashTableAnchor& operator=(const bslalg::HashTableAnchor& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setBucketArrayAddressAndSize(HashTableBucket    *bucketArrayAddress,
                                      native_std::size_t  bucketArraySize);
        // Set the bucket array address and bucket array size attributes of
        // this object to the specified 'bucketArrayAddress' and
        // 'bucketArraySize' values.  The behavior is undefined unless
        // 'bucketArrayAddress' refers to a contiguous sequence of valid
        // 'bslalg::HashTableBucket' objects of at least 'bucketArraySize', or
        // unless both 'bucketArrayAddress' and 'bucketArraySize' are 0.

    void setListRootAddress(BidirectionalLink *value);
        // Set the 'listRootAddress' attribute of this object to the
        // specified 'value'.

                                  // Aspects

    void swap(HashTableAnchor& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    // ACCESSORS
    HashTableBucket *bucketArrayAddress() const;
        // Return the value of the 'bucketArrayAddress' attribute of this
        // object.

    native_std::size_t bucketArraySize() const;
        // Return the value of the 'bucketArraySize' attribute of this object.

    BidirectionalLink *listRootAddress() const;
        // Return the value 'listRootAddress' attribute of this object.
};

// FREE OPERATORS
bool operator==(const HashTableAnchor& lhs, const HashTableAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bslalg::HashTableAnchor' objects
    // have the same value if all of the corresponding values of their
    // 'bucketArrayAddress', 'bucketArraySize', and 'listRootAddress'
    // attributes are the same.

bool operator!=(const HashTableAnchor& lhs, const HashTableAnchor& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bslalg::HashTableAnchor'
    // objects do not have the same value if any of the corresponding values of
    // their 'bucketArrayAddress', 'bucketArraySize', or 'listRootAddress'
    // attributes are not the same.

// FREE FUNCTIONS
void swap(HashTableAnchor& a, HashTableAnchor& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // class bslalg::HashTableAnchor
                        // -----------------------------

// CREATORS
inline
HashTableAnchor::HashTableAnchor(bslalg::HashTableBucket   *bucketArrayAddress,
                                 native_std::size_t         bucketArraySize,
                                 bslalg::BidirectionalLink *listRootAddress)
: d_bucketArrayAddress_p(bucketArrayAddress)
, d_bucketArraySize(bucketArraySize)
, d_listRootAddress_p(listRootAddress)
{
    BSLS_ASSERT_SAFE(   (!bucketArrayAddress && !bucketArraySize)
                     || (bucketArrayAddress && 0 < bucketArraySize));
    BSLS_ASSERT_SAFE(!listRootAddress || !(listRootAddress->previousLink()));
}

inline
HashTableAnchor::HashTableAnchor(const HashTableAnchor& original)
: d_bucketArrayAddress_p(original.d_bucketArrayAddress_p)
, d_bucketArraySize(original.d_bucketArraySize)
, d_listRootAddress_p(original.d_listRootAddress_p)
{
}

// MANIPULATORS
inline
HashTableAnchor& HashTableAnchor::operator=(const HashTableAnchor& rhs)
{
    d_bucketArrayAddress_p = rhs.d_bucketArrayAddress_p;
    d_bucketArraySize      = rhs.d_bucketArraySize;
    d_listRootAddress_p    = rhs.d_listRootAddress_p;
    return *this;
}

inline
void HashTableAnchor::setBucketArrayAddressAndSize(
                                        HashTableBucket    *bucketArrayAddress,
                                        native_std::size_t  bucketArraySize)
{
    BSLS_ASSERT_SAFE(( bucketArrayAddress && 0 < bucketArraySize)
                  || (!bucketArrayAddress &&    !bucketArraySize));

    d_bucketArrayAddress_p = bucketArrayAddress;
    d_bucketArraySize      = bucketArraySize;
}

inline
void HashTableAnchor::setListRootAddress(BidirectionalLink *value)
{
    BSLS_ASSERT_SAFE(!value || !value->previousLink());

    d_listRootAddress_p = value;
}

                                  // Aspects

inline
void HashTableAnchor::swap(HashTableAnchor& other)
{
    bslalg::ScalarPrimitives::swap(*this, other);
}

// ACCESSORS
inline
BidirectionalLink *HashTableAnchor::listRootAddress() const
{
    return d_listRootAddress_p;
}

inline
std::size_t HashTableAnchor::bucketArraySize() const
{
    return d_bucketArraySize;
}

inline
HashTableBucket *HashTableAnchor::bucketArrayAddress() const
{
    return d_bucketArrayAddress_p;
}

}  // close package namespace

// FREE OPERATORS
inline
void bslalg::swap(bslalg::HashTableAnchor& a, bslalg::HashTableAnchor& b)
{
    a.swap(b);
}

inline
bool bslalg::operator==(const bslalg::HashTableAnchor& lhs,
                        const bslalg::HashTableAnchor& rhs)
{
    return lhs.bucketArrayAddress() == rhs.bucketArrayAddress()
        && lhs.bucketArraySize()    == rhs.bucketArraySize()
        && lhs.listRootAddress()    == rhs.listRootAddress();
}

inline
bool bslalg::operator!=(const bslalg::HashTableAnchor& lhs,
                        const bslalg::HashTableAnchor& rhs)
{
    return lhs.bucketArrayAddress() != rhs.bucketArrayAddress()
        || lhs.bucketArraySize()    != rhs.bucketArraySize()
        || lhs.listRootAddress()    != rhs.listRootAddress();
}

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
