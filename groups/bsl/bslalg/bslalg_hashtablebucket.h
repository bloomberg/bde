// bslalg_hashtablebucket.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#define INCLUDED_BSLALG_HASHTABLEBUCKET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bucket representation for hash table data structures.
//
//@CLASSES:
//   bslalg::HashTableBucket : hash-table, manages externally allocated nodes
//
//@SEE_ALSO: bslalg_hashtableimputil, bslalg_bidirectionallink,
//           bslalg_bidirectionalnode
//
//@DESCRIPTION: This component provides an ability to keep track of a segment
// of a linked list of 'bslalg::BidirectionalLink' objects.  It contains
// pointers to the first and last elements of the list segment in question, or
// two null pointers for an empty list.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a List Template Class
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a linked list template class, it will be called
// 'MyList'.
//
// First, we create the iterator helper class, which will eventually be
// defined as a nested type within the 'MyList' class.
//..
//                              // ===============
//                              // MyList_Iterator
//                              // ===============
//
//  template <class PAYLOAD>
//  class MyList_Iterator {
//      // 'Iterator' type for class 'MyList'.  This class will be typedef'ed
//      // to be a nested class within 'MyList'.
//
//      // PRIVATE TYPES
//      typedef bslalg::BidirectionalNode<PAYLOAD> Node;
//
//      // DATA
//      Node *d_node;
//
//      // FRIENDS
//      template <class PL>
//      friend bool operator==(MyList_Iterator<PL>,
//                             MyList_Iterator<PL>);
//
//    public:
//      // CREATORS
//      MyList_Iterator() : d_node(0) {}
//      explicit
//      MyList_Iterator(Node *node) : d_node(node) {}
//      //! MyList_Iterator(const MyList_Iterator& original) = default;
//      //! ~MyList_Iterator() = default;
//
//      // MANIPULATORS
//      //! MyList_Iterator& operator=(const MyList_Iterator& other) = default;
//
//      MyList_Iterator operator++();
//
//      // ACCESSORS
//      PAYLOAD& operator*() const { return d_node->value(); }
//  };
//..
// Then, we define our 'MyList' class, which will inherit from
// 'bslalg::HashTableBucket'.  'MyList::Iterator' will be a public typedef of
// 'MyList_Iterator'.  For brevity, we will omit a lot of functionality that a
// full, general-purpose list class would have, implementing only what we will
// need for this example.
//..
//                                  // ======
//                                  // MyList
//                                  // ======
//
//  template <class PAYLOAD>
//  class MyList : public bslalg::HashTableBucket {
//      // This class stores a doubly-linked list containing objects of type
//      // 'PAYLOAD'.
//
//      // PRIVATE TYPES
//      typedef bslalg::BidirectionalNode<PAYLOAD> Node;
//
//    public:
//      // PUBLIC TYPES
//      typedef PAYLOAD                            ValueType;
//      typedef MyList_Iterator<ValueType>         Iterator;
//
//      // DATA
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // CREATORS
//      explicit
//      MyList(bslma::Allocator *basicAllocator = 0)
//      : d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          reset();
//      }
//      ~MyList();
//
//      // MANIPULATORS
//      Iterator begin() { return Iterator((Node *) first()); }
//      Iterator end()   { return Iterator(0); }
//      void pushBack(const ValueType& value);
//      void popBack();
//  };
//..
// Next, we implement the functions for the iterator type.
//..
//                              // ---------------
//                              // MyList_Iterator
//                              // ---------------
//
//  // MANIPULATORS
//  template <class PAYLOAD>
//  MyList_Iterator<PAYLOAD> MyList_Iterator<PAYLOAD>::operator++()
//  {
//      d_node = (Node *) d_node->nextLink();
//      return *this;
//  }
//
//  template <class PAYLOAD>
//  inline
//  bool operator==(MyList_Iterator<PAYLOAD> lhs,
//                  MyList_Iterator<PAYLOAD> rhs)
//  {
//      return lhs.d_node == rhs.d_node;
//  }
//
//  template <class PAYLOAD>
//  inline
//  bool operator!=(MyList_Iterator<PAYLOAD> lhs,
//                  MyList_Iterator<PAYLOAD> rhs)
//  {
//      return !(lhs == rhs);
//  }
//..
// Then, we implement the functions for the 'MyList' class:
//..
//                                  // ------
//                                  // MyList
//                                  // ------
//
//  // CREATORS
//  template <class PAYLOAD>
//  MyList<PAYLOAD>::~MyList()
//  {
//      typedef bslalg::BidirectionalLink BDL;
//
//      for (Node *p = (Node *) first(); p; ) {
//          Node *toDelete = p;
//          p = (Node *) p->nextLink();
//
//          toDelete->value().~ValueType();
//          d_allocator_p->deleteObjectRaw(static_cast<BDL *>(toDelete));
//      }
//
//      reset();
//  }
//
//  // MANIPULATORS
//  template <class PAYLOAD>
//  void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
//  {
//      Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//      node->setNextLink(0);
//      node->setPreviousLink(last());
//      bslalg::ScalarPrimitives::copyConstruct(&node->value(),
//                                              value,
//                                              d_allocator_p);
//
//      if (0 == last()) {
//          BSLS_ASSERT_SAFE(0 == first());
//
//          setFirstAndLast(node, node);
//      }
//      else {
//          last()->setNextLink(node);
//          setLast(node);
//      }
//  }
//
//  template <class PAYLOAD>
//  void MyList<PAYLOAD>::popBack()
//  {
//      BSLS_ASSERT_SAFE(first() && last());
//
//      Node *toDelete = (Node *) last();
//
//      if (first() != toDelete) {
//          BSLS_ASSERT_SAFE(0 != last());
//          setLast(last()->previousLink());
//          last()->setNextLink(0);
//      }
//      else {
//          reset();
//      }
//
//      d_allocator_p->deleteObject(toDelete);
//  }
//..
// Next, in 'main', we use our 'MyList' class to store a list of ints:
//..
//  MyList<int> intList;
//..
// Then, we declare an array of ints to populate it with:
//..
//  int intArray[] = { 8, 2, 3, 5, 7, 2 };
//  enum { NUM_INTS = sizeof intArray / sizeof *intArray };
//..
// Now, we iterate, pushing ints to the list:
//..
//  for (const int *pInt = intArray; pInt < intArray + NUM_INTS; ++pInt) {
//      intList.pushBack(*pInt);
//  }
//..
// Finally, we use our 'Iterator' type to traverse the list and observe its
// values:
//..
//  MyList<int>::Iterator it = intList.begin();
//  assert(8 == *it);
//  assert(2 == *++it);
//  assert(3 == *++it);
//  assert(5 == *++it);
//  assert(7 == *++it);
//  assert(2 == *++it);
//  assert(intList.end() == ++it);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {
namespace bslalg {

                          // =====================
                          // class HashTableBucket
                          // =====================

struct HashTableBucket {
  public:
    // DATA
    BidirectionalLink *d_first_p;
    BidirectionalLink *d_last_p;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(HashTableBucket,
                                   bsl::is_trivially_copyable);

  public:
    // No creators -- must be a POD so that aggregate initialization can be
    // done.

    // MANIPULATORS
    void setFirst(BidirectionalLink *node);
        // Set the 'first' element of this bucket to the specified 'node'.  The
        // behavior is undefined unless 'node' is an element from the same
        // bidirectional list as the 'last' element in this bucket, and 'node'
        // either precedes 'last' in that list, or is the same node, or this
        // bucket is empty and 'node' has a null pointer value.

    void setLast(BidirectionalLink *node);
        // Set the 'last' element of this bucket to the specified 'node'.  The
        // behavior is undefined unless 'node' is an element from the same
        // bidirectional list as the 'first' element in this bucket, and 'node'
        // either follows 'first' in that list, or is the same node, or this
        // bucket is empty and 'node' has a null pointer value.

    void setFirstAndLast(BidirectionalLink *first, BidirectionalLink *last);
        // Set 'first' and 'last' to the specified values.  Behavior is
        // undefined unless unless 'first == last', or unless 'first' and
        // 'last' are links from the same list, where 'first' precedes 'last'
        // in the list.  Note that 'first' and 'last' may both have a null
        // pointer value, indicating an empty bucket.

    void reset();
        // Set 'first' and 'last' to a null pointer value.

    // ACCESSORS
    BidirectionalLink *end() const;
        // Return the next node after the end of this bucket, or 0 if
        // '0 == last()', so the range to traverse to traverse all nodes in the
        // bucket is always '[ first(), end() )' regardless of whether the
        // bucket is empty.

    BidirectionalLink *first() const;
        // Return the address of the first element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    BidirectionalLink *last() const;
        // Return the address of the last element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    native_std::size_t countElements() const;
        // Return the number of nodes in this hash bucket.
};

// ============================================================================
//                               FREE OPERATORS
// ============================================================================

bool operator==(const HashTableBucket& lhs, const HashTableBucket& rhs);
    // Return 'true' if the specified hash table buckets 'lhs' and 'rhs' are
    // equivalent and 'false' otherwise.

bool operator!=(const HashTableBucket& lhs, const HashTableBucket& rhs);
    // Return 'true' if the specified hash table buckets 'lhs' and 'rhs' are
    // not equivalent and 'false' otherwise.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //----------------------
                        // class HashTableBucket
                        //----------------------

// MANIPULATORS
inline
void HashTableBucket::setFirst(BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(!d_first_p == !node);

    d_first_p = node;
}

inline
void HashTableBucket::setLast(BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(!d_last_p == !node);

    d_last_p = node;
}

inline
void HashTableBucket::setFirstAndLast(BidirectionalLink *first,
                                      BidirectionalLink *last)
{
    BSLS_ASSERT_SAFE(!first == !last);

    d_first_p = first;
    d_last_p  = last;
}

inline
void HashTableBucket::reset()
{
    d_first_p = d_last_p = 0;
}

// ACCESSORS
inline
BidirectionalLink *HashTableBucket::end() const
{
    return d_last_p ? d_last_p->nextLink() : 0;
}

inline
BidirectionalLink *HashTableBucket::first() const
{
    return d_first_p;
}

inline
BidirectionalLink *HashTableBucket::last() const
{
    return d_last_p;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bslalg::operator==(const bslalg::HashTableBucket& lhs,
                        const bslalg::HashTableBucket& rhs)
{
    return lhs.first() == rhs.first() && lhs.last() == rhs.last();
}

inline
bool bslalg::operator!=(const bslalg::HashTableBucket& lhs,
                        const bslalg::HashTableBucket& rhs)
{
    return lhs.first() != rhs.first() || lhs.last() != rhs.last();
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
