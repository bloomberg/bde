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
//@AUTHOR: Alisdair Meredith (ameredith1) Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides a single POD-like class,
// 'bslalg::BidirectionalNode', used to represent a node in a doubly-linked
// (bidirectional) list holding a value of a parameterized type.  A
// 'bslalg::BidirectionalNode' publicly derives from
// 'bslalg::BidirectionalLink', so it may be used with
// 'bslalg::BidirectionalListUtil' functions, and adds an attribute 'value' of
// the parameterized 'VALUE_TYPE'.  The following inheritance hierarchy diagram
// shows the classes involved and their methods:
//..
//                  ,-------------------------.
//                 ( bslalg::BidirectionalNode )
//                  `-------------------------'
//                               |      value
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
///----------------------------------------------------------------------------
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
//      typedef bslalg::BidirectionalNode<PAYLOAD> Node;
//
//    public:
//      // PUBLIC TYPES
//      typedef PAYLOAD                            ValueType;
//      typedef MyList_Iterator<ValueType>         Iterator;
//
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
//      for (Node *p = d_begin; p; ) {
//          Node *condemned = p;
//          p = (Node *) p->nextLink();
//
//          condemned->value().~ValueType();
//          d_allocator_p->deleteObjectRaw(static_cast<BDL *>(condemned));
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
// Next, in 'main', we have finished implementing our 'MyList' class and its
// 'Iterator' type, we will use one to store a fibonacci sequence of ints.  We
// declare the memory allocator that we will use:
//..
//  bslma::TestAllocator oa("oa");
//..
// Then, we enter a block and declare our list 'fibonacciList' to contain the
// sequence:
//..
//  {
//      MyList<int> fibonacciList(&oa);
//      typedef MyList<int>::Iterator Iterator;
//
//      {
//..
// Next, we initialize the list to containing the first 2 values, '0' and '1':
//..
//          fibonacciList.pushBack(0);
//          fibonacciList.pushBack(1);
//..
// Then, we create iterators 'first' and 'second' and point them to those first
// two elements:
//..
//          Iterator first  = fibonacciList.begin();
//          Iterator second = first;
//          ++second;
//
//          assert(0 == *first);
//          assert(1 == *second);
//..
// Next, we iterate a dozen times, each time adding a new element to the end of
// the list containing a value that is the sum of the values of the previous
// two elements:
//..
//          for (int i = 0; i < 12; ++i, ++first, ++second) {
//              fibonacciList.pushBack(*first + *second);
//          }
//      }
//..
// Now, we traverse the list and print out its elements:
//..
//      if (verbose) printf("Fibonacci Numbers: ");
//
//      const Iterator begin = fibonacciList.begin();
//      const Iterator end   = fibonacciList.end();
//      for (Iterator it = begin; end != it; ++it) {
//          if (verbose) printf("%s%d", begin == it ? "" : ", ", *it);
//      }
//      if (verbose) printf("\n");
//  }
//..
// Finally, we check the allocator and verify that it's been used, and that
// the destruction of 'fibonacciList' freed all the memory allocated:
//..
//  assert(oa.numBlocksTotal() > 0);
//  assert(0 == oa.numBlocksInUse());
//..
// The above code prints out:
//..
//  Fibonacci Numbers: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233
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

template <class VALUE_TYPE>
class BidirectionalNode : public bslalg::BidirectionalLink {
    // This POD-like 'class' describes a node suitable for use in a
    // doubly-linked list of values of the parameterized 'VALUE'.  This class
    // is a "POD-like" to facilitate efficient allocation and use in the
    // context of a container implementation.  In order to meet the essential
    // requirements of a POD type, this 'class' does not define a constructor
    // or destructor.  The manipulator, 'value', returns a modifiable reference
    // to 'd_value' so that it may be constructed in-place by the appropriate
    // 'bsl::allocator_traits' object.

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE    ValueType;      // payload type

  private:
    // DATA
    VALUE_TYPE d_value;      // payload value

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

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -----------------------
                        // class BidirectionalNode
                        // -----------------------

template <class VALUE_TYPE>
inline
VALUE_TYPE& BidirectionalNode<VALUE_TYPE>::value()
{
    return d_value;
}

template <class VALUE_TYPE>
inline
const VALUE_TYPE& BidirectionalNode<VALUE_TYPE>::value() const
{
    return d_value;
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
