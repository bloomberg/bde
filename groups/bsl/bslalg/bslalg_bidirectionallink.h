// bslalg_bidirectionallink.h                                         -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#define INCLUDED_BSLALG_BIDIRECTIONALLINK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a basic link type for building doubly-linked lists.
//
//@CLASSES:
//   bslalg::BidirectionalLink : A node in a doubly-linked list
//
//@SEE_ALSO: bslalg_bidirectionallinklistutil, bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides a single POD-like class,
// 'BidirectionalLink', used to represent a node in a doubly-linked list.  A
// 'BidirectionalLink' provides the address to its preceding node, and the
// address of its successor node.  A null-pointer value for either address
// signifies the end of the list.  'BidirectionalLink' does not, however,
// contain "payload" data (e.g., a value), as it is intended to work with
// generalized list operations (see 'bslalg_bidirectionallinklistutil').
// Clients creating a doubly-linked list must define their own node type that
// incorporates 'BidirectionalLink' (generally via inheritance), and that
// maintains the "value" stored in that node.
//
//-----------------------------------------------------------------------------
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
// First, we create the 'MyNode' class, which derives from the
// BidirectionalLink class to carry a 'PAYLOAD' object.
//..
//  template <class PAYLOAD>
//  class MyNode : public bslalg::BidirectionalLink {
//    public:
//      // PUBLIC TYPES
//      typedef PAYLOAD  ValueType;
//
//    private:
//      // DATA
//      ValueType     d_value;
//
//    private:
//      // NOT IMPLEMENTED
//      MyNode();
//      MyNode(const MyNode&);
//      MyNode& operator=(const MyNode&);
//
//    public:
//      // CREATOR
//      ~MyNode() {}
//          // Destroy this object.
//
//      // MANIPULATOR
//      ValueType& value() { return d_value; }
//          // Return a reference to the modifiable value stored in this node.
//
//      // ACCESSOR
//      const ValueType& value() const { return d_value; }
//          // Return a reference to the non-modifiable value stored in this
//          // node.
//  };
//..
// Next, we create the iterator helper class, which will eventually be
// defined as a nested type within the 'MyList' class.
//..
//                              // ===============
//                              // MyList_Iterator
//                              // ===============
//
//  template <class PAYLOAD>
//  class MyList_Iterator {
//      // PRIVATE TYPES
//      typedef MyNode<PAYLOAD> Node;
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
//      //! MyList_Iterator& operator=(const MyList_Iterator& other) = default;
//      //! ~MyList_Iterator() = default;
//
//      // MANIPULATORS
//      MyList_Iterator operator++();
//
//      // ACCESSORS
//      PAYLOAD& operator*() const { return d_node->value(); }
//  };
//..
// Then, we define our 'MyList' class, with 'MyList::Iterator' being a public
// typedef of 'MyList_Iterator'.  For brevity, we will omit a lot of
// functionality that a full, general-purpose list class would have,
// implementing only what we will need for this example.
//..
//                                  // ======
//                                  // MyList
//                                  // ======
//
//  template <class PAYLOAD>
//  class MyList {
//      // PRIVATE TYPES
//      typedef MyNode<PAYLOAD> Node;
//
//    public:
//      // PUBLIC TYPES
//      typedef PAYLOAD                            ValueType;
//      typedef MyList_Iterator<ValueType>         Iterator;
//
//    private:
//      // DATA
//      Node             *d_begin;
//      Node             *d_end;
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // CREATORS
//      explicit
//      MyList(bslma::Allocator *basicAllocator = 0)
//      : d_begin(0)
//      , d_end(0)
//      , d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {}
//
//      ~MyList();
//
//      // MANIPULATORS
//      Iterator begin();
//      Iterator end();
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
//      for (Node *p = d_begin; p; ) {
//          Node *toDelete = p;
//          p = (Node *) p->nextLink();
//
//          d_allocator_p->deleteObjectRaw(toDelete);
//      }
//  }
//
//  // MANIPULATORS
//  template <class PAYLOAD>
//  typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::begin()
//  {
//      return Iterator(d_begin);
//  }
//
//  template <class PAYLOAD>
//  typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::end()
//  {
//      return Iterator(0);
//  }
//
//  template <class PAYLOAD>
//  void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
//  {
//      Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//      node->setNextLink(0);
//      node->setPreviousLink(d_end);
//      bslalg::ScalarPrimitives::copyConstruct(&node->value(),
//                                              value,
//                                              d_allocator_p);
//
//      if (d_end) {
//          BSLS_ASSERT_SAFE(d_begin);
//
//          d_end->setNextLink(node);
//          d_end = node;
//      }
//      else {
//          BSLS_ASSERT_SAFE(0 == d_begin);
//
//          d_begin = d_end = node;
//      }
//  }
//
//  template <class PAYLOAD>
//  void MyList<PAYLOAD>::popBack()
//  {
//      BSLS_ASSERT_SAFE(d_begin && d_end);
//
//      Node *toDelete = d_end;
//      d_end = (Node *) d_end->previousLink();
//
//      if (d_begin != toDelete) {
//          BSLS_ASSERT_SAFE(0 != d_end);
//          d_end->setNextLink(0);
//      }
//      else {
//          BSLS_ASSERT_SAFE(0 == d_end);
//          d_begin = 0;
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

namespace BloombergLP {
namespace bslalg {

                          // =======================
                          // class BidirectionalLink
                          // =======================

class BidirectionalLink {
    // This POD-like 'class' describes a node suitable for use in a doubly-
    // linked (bidirectional) list, holding the addresses of the preceding and
    // succeeding nodes, either or both of which may be 0.  This class is
    // "POD-like" to facilitate efficient allocation and use in the context of
    // a container implementations.  In order to meet the essential
    // requirements of a POD type, this 'class' does not declare a constructor
    // or destructor.  However its data members are private.  It satisfies the
    // requirements of a *trivial* type and a *standard* *layout* type defined
    // by the C++11 standard.  Note that this type does not contain any
    // "payload" member data: Clients creating a doubly-linked list of data
    // must define an appropriate node type that incorporates
    // 'BidirectionalLink' (generally via inheritance), and that holds the
    // "value" of any data stored in that node.

  private:
    // DATA
    BidirectionalLink *d_next_p;  // The next node in a list traversal
    BidirectionalLink *d_prev_p;  // The preceding node in a list traversal

  public:
    // CREATORS
    //! BidirectionalLink() = default;
        // Create a 'BidirectionalLink' object having uninitialized values,
        // or zero-initialized values if value-initialized.

    //! BidirectionalLink(const BidirectionalLink& original) = default;
        // Create a 'BidirectionalLink' object having the same data member
        // values as the specified 'original' object.

    //! ~BidirectionalLink() = default;
        // Destroy this object.

    // MANIPULATORS
    //! BidirectionalLink& operator= (const BidirectionalLink& rhs) = default;
        // Assign to the data members of this object the values of the data
        // members of the specified 'rhs' object, and return a reference
        // providing modifiable access to this object.

    void setNextLink(BidirectionalLink *next);
        // Set the successor of this node to be the specified 'next' link.

    void setPreviousLink(BidirectionalLink *previous);
        // Set the predecessor of this node to be the specified 'prev' link.

    void reset();
        // Set the 'nextLink' and 'previousLink' attributes of this value to 0.

    // ACCESSORS
    BidirectionalLink *nextLink() const;
        // Return the address of the next node linked from this node.

    BidirectionalLink *previousLink() const;
        // Return the address of the preceding node linked from this node.

};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //------------------------
                        // class BidirectionalLink
                        //------------------------

// MANIPULATORS
inline
void BidirectionalLink::setNextLink(BidirectionalLink *next)
{
    d_next_p = next;
}

inline
void BidirectionalLink::setPreviousLink(BidirectionalLink *previous)
{
    d_prev_p = previous;
}

inline
void BidirectionalLink::reset()
{
    d_prev_p = 0;
    d_next_p = 0;
}

// ACCESSORS
inline
BidirectionalLink *BidirectionalLink::nextLink() const
{
    return d_next_p;
}

inline
BidirectionalLink *BidirectionalLink::previousLink() const
{
    return d_prev_p;
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
