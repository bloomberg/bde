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
// address of it successor node.  A null-pointer value for either address
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
//  template <typename PAYLOAD>
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
//  template <typename PAYLOAD>
//  class MyList_Iterator {
//      // PRIVATE TYPES
//      typedef MyNode<PAYLOAD> Node;
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
//  template <typename PAYLOAD>
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
//      MyList(bslma::Allocator *basicAllocator)
//      : d_begin(0)
//      , d_end(0)
//      , d_allocator_p(basicAllocator)
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
//
// Then, we implement the functions for the 'MyList' class:
//
//                                  // ------
//                                  // MyList
//                                  // ------
//
//  // CREATORS
//  template <typename PAYLOAD>
//  MyList<PAYLOAD>::~MyList()
//  {
//      for (Node *p = d_begin; p; ) {
//          Node *condemned = p;
//          p = (Node *) p->nextLink();
//
//          d_allocator_p->deleteObjectRaw(condemned);
//      }
//  }
//
//  // MANIPULATORS
//  template <typename PAYLOAD>
//  typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::begin()
//  {
//      return Iterator(d_begin);
//  }
//
//  template <typename PAYLOAD>
//  typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::end()
//  {
//      return Iterator(0);
//  }
//
//  template <typename PAYLOAD>
//  void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
//  {
//      Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
//      node->setNextLink(0);
//      node->setPreviousLink(d_end);
//      new (&node->value()) ValueType(value);
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
//  template <typename PAYLOAD>
//  void MyList<PAYLOAD>::popBack()
//  {
//      BSLS_ASSERT_SAFE(d_begin && d_end);
//
//      Node *condemned = d_end;
//      d_end = (Node *) d_end->previousLink();
//
//      if (d_begin != condemned) {
//          BSLS_ASSERT_SAFE(0 != d_end);
//          d_end->setNextLink(0);
//      }
//      else {
//          BSLS_ASSERT_SAFE(0 == d_end);
//          d_begin = 0;
//      }
//
//      condemned->value().~ValueType();
//      d_allocator_p->deallocate(condemned);
//  }
//..
// Next, we have finished implementing our 'MyList' class and its 'Iterator'
// type, we will use one to store a fibonacci sequence of ints.  In 'main',
// We declare the memory allocator that we will use:
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

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

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

}  // close namespace bslalg
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
