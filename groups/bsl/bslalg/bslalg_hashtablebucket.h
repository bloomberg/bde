// bslalg_hashtablebucket.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#define INCLUDED_BSLALG_HASHTABLEBUCKET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bucket represantation for hash table data structures.
//
//@CLASSES:
//   bslalg::HashTableBucket : hash-table, manages externally allocated nodes
//
//@SEE_ALSO: bslalg_hashtableimputil, bslalg_bidirectionallink,
//           bslalg_bidirectionalnode
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an ability to keep track of a segment
// of a linked list of 'bslalg::BidirectionalLink' objects.  It contains
// pointers to the first and last elements of the list segment in question, or
// two null pointers for an empty list.
//-----------------------------------------------------------------------------
//..
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
//  template <typename PAYLOAD>
//  class MyList_Iterator {
//      // PRIVATE TYPES
//      typedef bslalg::BidirectionalNode<PAYLOAD> Node;
//
//      // DATA
//      Node *d_node;
//
//      // FRIENDS
//      template <typename PL>
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
// full, general-purpose list class would have, implmenting only what we will
// need for this example.
//..
//                                  // ======
//                                  // MyList
//                                  // ======
//
//  template <typename PAYLOAD>
//  class MyList : public bslalg::HashTableBucket {
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
//      MyList(bslma::Allocator *basicAllocator)
//      : d_allocator_p(basicAllocator)
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
// Next, we implment the functions for the iterator type.
//..
//                              // ---------------
//                              // MyList_Iterator
//                              // ---------------
//
//  // MANIPULATORS
//  template <typename PAYLOAD>
//  MyList_Iterator<PAYLOAD> MyList_Iterator<PAYLOAD>::operator++()
//  {
//      d_node = (Node *) d_node->nextLink();
//      return *this;
//  }
//
//  template <typename PAYLOAD>
//  inline
//  bool operator==(MyList_Iterator<PAYLOAD> lhs,
//                  MyList_Iterator<PAYLOAD> rhs)
//  {
//      return lhs.d_node == rhs.d_node;
//  }
//
//  template <typename PAYLOAD>
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
//  template <typename PAYLOAD>
//  MyList<PAYLOAD>::~MyList()
//  {
//      typedef bslalg::BidirectionalLink BDL;
//
//      for (Node *p = (Node *) first(); p; ) {
//          Node *condemned = p;
//          p = (Node *) p->nextLink();
//
//          condemned->value().~ValueType();
//          d_allocator_p->deleteObjectRaw(static_cast<BDL *>(condemned));
//      }
//
//      reset();
//  }
//
//  // MANIPULATORS
//  template <typename PAYLOAD>
//  void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
//  {
//      Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//      node->setNextLink(0);
//      node->setPreviousLink(last());
//      new (&node->value()) ValueType(value);
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
//  template <typename PAYLOAD>
//  void MyList<PAYLOAD>::popBack()
//  {
//      BSLS_ASSERT_SAFE(first() && last());
//
//      Node *condemned = (Node *) last();
//
//      if (first() != condemned) {
//          BSLS_ASSERT_SAFE(0 != last());
//          setLast(last()->previousLink());
//          last()->setNextLink(0);
//      }
//      else {
//          reset();
//      }
//
//      condemned->value().~ValueType();
//      d_allocator_p->deallocate(condemned);
//  }
//..
// Next, in our 'main', we have finished implmenting our 'MyList' class and its
// 'Iterator' type, we will use one to store a fibonacci sequence of ints.  We
// declare the memory allocator that we will use:
//..
//          bslma::TestAllocator oa("oa");
//..
// Then, we enter a block and declare our list 'fibonacciList' to contain the
// sequence:
//..
//          {
//              MyList<int> fibonacciList(&oa);
//              typedef MyList<int>::Iterator Iterator;
//
//              {
//..
// Next, we initialize the list to containing the first 2 values, '0' and '1':
//..
//                  fibonacciList.pushBack(0);
//                  fibonacciList.pushBack(1);
//..
// Then, we create iterators 'first' and 'second' and point them to those first
// two elements:
//..
//                  Iterator first  = fibonacciList.begin();
//                  Iterator second = first;
//                  ++second;
//
//                  assert(0 == *first);
//                  assert(1 == *second);
//..
// Next, we iterate a dozen times, each time adding a new element to the end of
// the list containing a value that is the sum of the values of the previous
// two elements:
//..
//                  for (int i = 0; i < 12; ++i, ++first, ++second) {
//                      fibonacciList.pushBack(*first + *second);
//                  }
//              }
//..
// Now, we traverse the list and print out its elements:
//..
//              if (verbose) printf("Fibonacci Numbers: ");
//
//              const Iterator begin = fibonacciList.begin();
//              const Iterator end   = fibonacciList.end();
//              for (Iterator it = begin; end != it; ++it) {
//                  if (verbose) printf("%s%d", begin == it ? "" : ", ", *it);
//              }
//              if (verbose) printf("\n");
//          }
//..
// Finally, we check the allocator and verify that it's been used, and that
// the destruction of 'fibonacciList' freed all the memory allocated:
//..
//          assert(oa.numBlocksTotal() > 0);
//          assert(0 == oa.numBlocksInUse());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#endif

namespace BloombergLP
{
namespace bslalg
{

class BidirectionalLink;

                          // =====================
                          // class HashTableBucket
                          // =====================

struct HashTableBucket {
  public:
    // DATA
    BidirectionalLink *d_first_p;
    BidirectionalLink *d_last_p;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(HashTableBucket, bslalg::TypeTraitsGroupPod);

  public:
    // No creators -- must be a POD.

    // MANIPULATORS
    void setFirst(BidirectionalLink *node);
        // Set the 'first' element of this bucket to the specified 'node'.  The
        // behavior is undefined unless 'node' is an element from the same
        // birectional list as the 'last' element in this bucket, and 'node'
        // either precedes 'last' in that list, or is the same node, or this
        // bucket is empty and 'node' has a null pointer value.

    void setLast(BidirectionalLink *node);
        // Set the 'last' element of this bucket to the specified 'node'.  The
        // behavior is undefined unless 'node' is an element from the same
        // birectional list as the 'first' element in this bucket, and 'node'
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
    BidirectionalLink *first() const;
        // Return the address of the first element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    BidirectionalLink *last() const;
        // Return the address of the last element in this hash bucket, or a
        // null pointer value if the bucket is empty.

    native_std::size_t countElements() const;
        // Return the number of nodes in this hash bucket.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
BidirectionalLink *HashTableBucket::first() const
{
    return d_first_p;
}

inline
BidirectionalLink *HashTableBucket::last() const
{
    return d_last_p;
}

}  // close namespace bslalg

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
