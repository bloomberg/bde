// bslalg_bidirectionalnode.h                                         -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#define INCLUDED_BSLALG_BIDIRECTIONALNODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a node holding a value in a doubly-linked list.
//
//@CLASSES:
//   bslalg::BidirectionalNode : Node holding a value in a linked list
//
//@SEE_ALSO: bslalg_bidirectionallink, bslalg_bidirectionallinklistutil,
//           bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides a single POD-like class,
// 'bslalg::BidirectionalNode', used to represent a node in a doubly-linked
// (bidirectional) list holding a value of a parameterized type.  A
// 'bslalg::BidirectionalNode' publicly derives from
// 'bslalg::BidirectionalLink', so it may be used with
// 'bslalg::BidirectionalLinkListUtil' functions, and adds an attribute 'value'
// of the template parameter type 'VALUE'.  The following inheritance hierarchy
// diagram shows the classes involved and their methods:
//..
//                  ,-------------------------.
//                 ( bslalg::BidirectionalNode )
//                  `-------------------------'
//                               |      value
//                               |      (all CREATORS unimplemented)
//                               V
//                  ,-------------------------.
//                 ( bslalg::BidirectionalLink )
//                  `-------------------------'
//                                      ctor
//                                      dtor
//                                      setNextLink
//                                      setPreviousLink
//                                      nextLink
//                                      previousLink
//..
// This class is "POD-like" to facilitate efficient allocation and use in the
// context of container implementations.  In order to meet the essential
// requirements of a POD type, neither this 'class' nor
// 'bslalg::BidirectionalLink' define a constructor or destructor.  The
// manipulator, 'value', returns a modifiable reference to the stored object
// that may be constructed in-place, for example, by the appropriate
// 'bsl::allocator_traits' methods.  While not strictly a POD, this class is a
// *standard-layout* type according to the C++11 standard.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a List Template Class
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a linked list template class called 'MyList'.
//
// First, we create an iterator helper class, which will eventually be defined
// as a nested type within the 'MyList' class.
//..
//                              // ===============
//                              // MyList_Iterator
//                              // ===============
//
//  template <class PAYLOAD>
//  class MyList_Iterator {
//      // This iterator is used to refer to positions within a list.
//
//      // PRIVATE TYPES
//      typedef bslalg::BidirectionalNode<PAYLOAD> Node;
//
//      // DATA
//      Node *d_node;    // Pointer to a node within a list.
//
//      // FRIENDS
//      template <class OTHER_PAYLOAD>
//      friend bool operator==(MyList_Iterator<OTHER_PAYLOAD>,
//                             MyList_Iterator<OTHER_PAYLOAD>);
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
//      const PAYLOAD& operator*() const { return d_node->value(); }
//  };
//
// ============================================================================
//                                FREE OPERATORS
// ----------------------------------------------------------------------------
//
//  template <class PAYLOAD>
//  bool operator==(MyList_Iterator<PAYLOAD> lhs,
//                  MyList_Iterator<PAYLOAD> rhs);
//
//  template <class PAYLOAD>
//  bool operator!=(MyList_Iterator<PAYLOAD> lhs,
//                  MyList_Iterator<PAYLOAD> rhs);
//..
// Then, we implement the functions for the iterator type.
//..
//                                  // ---------------
//                                  // MyList_Iterator
//                                  // ---------------
//
//  // MANIPULATORS
//  template <class PAYLOAD>
//  inline
//  MyList_Iterator<PAYLOAD> MyList_Iterator<PAYLOAD>::operator++()
//  {
//      d_node = static_cast<Node *>(d_node->nextLink());
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
// Next, we define our 'MyList' class, with 'MyList::Iterator' being a public
// typedef of 'MyList_Iterator'.  For brevity, we will omit much of te that a
// full, general-purpose list class would have.
//..
//                                  // ======
//                                  // MyList
//                                  // ======
//
//  template <class PAYLOAD>
//  class MyList {
//      // Doubly-linked list storing objects of type 'PAYLOAD'.
//
//      // PRIVATE TYPES
//      typedef bslalg::BidirectionalNode<PAYLOAD> Node;
//
//    public:
//      // PUBLIC TYPES
//      typedef PAYLOAD                    ValueType;
//      typedef MyList_Iterator<ValueType> Iterator;
//
//      // DATA
//      Node             *d_begin;          // First node, if any, in the list.
//      Node             *d_end;            // Last node, if any, in the list.
//      bslma::Allocator *d_allocator_p;    // Allocator used for allocating
//                                          // and freeing nodes.
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
//          bslalg::ScalarDestructionPrimitives::destroy(&toDelete->value());
//          d_allocator_p->deleteObjectRaw(
//                         static_cast<bslalg::BidirectionalLink *>(toDelete));
//      }
//  }
//
//  // MANIPULATORS
//  template <class PAYLOAD>
//  inline
//  typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::begin()
//  {
//      return Iterator(d_begin);
//  }
//
//  template <class PAYLOAD>
//  inline
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
//      bslalg::ScalarDestructionPrimitives::destroy(&toDelete->value());
//      d_allocator_p->deleteObjectRaw(
//                         static_cast<bslalg::BidirectionalLink *>(toDelete));
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

namespace BloombergLP {
namespace bslalg {

                        // =======================
                        // class BidirectionalNode
                        // =======================

template <class VALUE>
class BidirectionalNode : public bslalg::BidirectionalLink {
    // This POD-like 'class' describes a node suitable for use in a
    // doubly-linked list of values of the template parameter type 'VALUE'.
    // This class is a "POD-like" to facilitate efficient allocation and use in
    // the context of a container implementation.  In order to meet the
    // essential requirements of a POD type, this 'class' does not define a
    // constructor or destructor.  The manipulator, 'value', returns a
    // modifiable reference to 'd_value' so that it may be constructed in-place
    // by the appropriate 'bsl::allocator_traits' object.

  public:
    // PUBLIC TYPES
    typedef VALUE ValueType;      // payload type

  private:
    // DATA
    VALUE d_value;  // payload value

    // The following creators are not defined because a 'BidirectionalNode'
    // should never be constructed, destructed, or assigned.  The 'd_value'
    // member should be separately constructed and destroyed using an
    // appropriate 'bsl::allocator_traits' object.

  private:
    // NOT IMPLEMENTED
    BidirectionalNode();
    BidirectionalNode(const BidirectionalNode&);
    BidirectionalNode& operator=(const BidirectionalNode&);
    ~BidirectionalNode();

  public:
    // MANIPULATORS
    ValueType& value();
        // Return a reference providing modifiable access to the 'value' held
        // by this object.

    // ACCESSORS
    const ValueType& value() const;
        // Return a reference providing non-modifiable access to the 'value'
        // held by this object.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class BidirectionalNode
                        // -----------------------

template <class VALUE>
inline
VALUE& BidirectionalNode<VALUE>::value()
{
    return d_value;
}

template <class VALUE>
inline
const VALUE& BidirectionalNode<VALUE>::value() const
{
    return d_value;
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
