// bslalg_hashtablebucket.t.cpp                                       -*-C++-*-
#include <bslalg_hashtablebucket.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionalnode.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bslma_testallocator.h>

#include <new>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)


//=============================================================================
//                  GLOBAL TYPEDEFS and VARIABLES for TESTING
//-----------------------------------------------------------------------------

typedef bslalg::HashTableBucket   Obj;
typedef bslalg::BidirectionalLink Link;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// Suppose we want to create a linked list template class, it will be called
// 'MyList'.
//
// First, we create the iterator helper class, which will eventually be
// defined as a nested type within the 'MyList' class.

                            // ===============
                            // MyList_Iterator
                            // ===============

template <typename PAYLOAD>
class MyList_Iterator {
    // PRIVATE TYPES
    typedef bslalg::BidirectionalNode<PAYLOAD> Node;

    // DATA
    Node *d_node;

    // FRIENDS
    template <typename PL>
    friend bool operator==(MyList_Iterator<PL>,
                           MyList_Iterator<PL>);

  public:
    // CREATORS
    MyList_Iterator() : d_node(0) {}
    explicit
    MyList_Iterator(Node *node) : d_node(node) {}
    //! MyList_Iterator(const MyList_Iterator& original) = default;
    //! ~MyList_Iterator() = default;

    // MANIPULATORS
    //! MyList_Iterator& operator=(const MyList_Iterator& other) = default;

    MyList_Iterator operator++();

    // ACCESSORS
    PAYLOAD& operator*() const { return d_node->value(); }
};

// Then, we define our 'MyList' class, which will inherit from
// 'bslalg::HashTableBucket'.  'MyList::Iterator' will be a public typedef of
// 'MyList_Iterator'.  For brevity, we will omit a lot of functionality that a
// full, general-purpose list class would have, implmenting only what we will
// need for this example.

                                // ======
                                // MyList
                                // ======

template <typename PAYLOAD>
class MyList : public bslalg::HashTableBucket {
    // PRIVATE TYPES
    typedef bslalg::BidirectionalNode<PAYLOAD> Node;

  public:
    // PUBLIC TYPES
    typedef PAYLOAD                            ValueType;
    typedef MyList_Iterator<ValueType>         Iterator;

    // DATA
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit
    MyList(bslma::Allocator *basicAllocator)
    : d_allocator_p(basicAllocator)
    {
        reset();
    }
    ~MyList();

    // MANIPULATORS
    Iterator begin() { return Iterator((Node *) first()); }
    Iterator end()   { return Iterator(0); }
    void pushBack(const ValueType& value);
    void popBack();
};

// Next, we implment the functions for the iterator type.

                            // ---------------
                            // MyList_Iterator
                            // ---------------

// MANIPULATORS
template <typename PAYLOAD>
MyList_Iterator<PAYLOAD> MyList_Iterator<PAYLOAD>::operator++()
{
    d_node = (Node *) d_node->nextLink();
    return *this;
}

template <typename PAYLOAD>
inline
bool operator==(MyList_Iterator<PAYLOAD> lhs,
                MyList_Iterator<PAYLOAD> rhs)
{
    return lhs.d_node == rhs.d_node;
}

template <typename PAYLOAD>
inline
bool operator!=(MyList_Iterator<PAYLOAD> lhs,
                MyList_Iterator<PAYLOAD> rhs)
{
    return !(lhs == rhs);
}

// Then, we implement the functions for the 'MyList' class:

                                // ------
                                // MyList
                                // ------

// CREATORS
template <typename PAYLOAD>
MyList<PAYLOAD>::~MyList()
{
    typedef bslalg::BidirectionalLink BDL;

    for (Node *p = (Node *) first(); p; ) {
        Node *condemned = p;
        p = (Node *) p->nextLink();

        condemned->value().~ValueType();
        d_allocator_p->deleteObjectRaw(static_cast<BDL *>(condemned));
    }

    reset();
}

// MANIPULATORS
template <typename PAYLOAD>
void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
{
    Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
    node->setNextLink(0);
    node->setPreviousLink(last());
    new (&node->value()) ValueType(value);

    if (0 == last()) {
        BSLS_ASSERT_SAFE(0 == first());

        setFirstAndLast(node, node);
    }
    else {
        last()->setNextLink(node);
        setLast(node);
    }
}

template <typename PAYLOAD>
void MyList<PAYLOAD>::popBack()
{
    BSLS_ASSERT_SAFE(first() && last());

    Node *condemned = (Node *) last();

    if (first() != condemned) {
        BSLS_ASSERT_SAFE(0 != last());
        setLast(last()->previousLink());
        last()->setNextLink(0);
    }
    else {
        reset();
    }

    condemned->value().~ValueType();
    d_allocator_p->deallocate(condemned);
}

//=============================================================================
//                     General Functions for Testing
//-----------------------------------------------------------------------------

void myCheckInvariants(const bslalg::HashTableBucket& bucket)
{
    ASSERT(!bucket.first() == !bucket.last());

    if (bslalg::BidirectionalLink *cursor = bucket.first()) {
        bslalg::BidirectionalLink *prev   = cursor;
        while (cursor != bucket.last()) {
            cursor = cursor->nextLink();
            ASSERT(cursor);
            ASSERT(prev == cursor->previousLink());
            prev = cursor;
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
//  bool veryVerbose         = argc > 3;
//  bool veryVeryVerbose     = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

// Next, in our 'main', we have finished implmenting our 'MyList' class and its
// 'Iterator' type, we will use one to store a fibonacci sequence of ints.  We
// declare the memory allocator that we will use:

        bslma::TestAllocator oa("oa");

// Then, we enter a block and declare our list 'fibonacciList' to contain the
// sequence:

        {
            MyList<int> fibonacciList(&oa);
            typedef MyList<int>::Iterator Iterator;

            {
// Next, we initialize the list to containing the first 2 values, '0' and '1':

                fibonacciList.pushBack(0);
                fibonacciList.pushBack(1);

// Then, we create iterators 'first' and 'second' and point them to those first
// two elements:

                Iterator first  = fibonacciList.begin();
                Iterator second = first;
                ++second;

                ASSERT(0 == *first);
                ASSERT(1 == *second);

// Next, we iterate a dozen times, each time adding a new element to the end of
// the list containing a value that is the sum of the values of the previous
// two elements:

                for (int i = 0; i < 12; ++i, ++first, ++second) {
                    fibonacciList.pushBack(*first + *second);
                }
            }

// Now, we traverse the list and print out its elements:

            if (verbose) printf("Fibonacci Numbers: ");

            const Iterator begin = fibonacciList.begin();
            const Iterator end   = fibonacciList.end();
            for (Iterator it = begin; end != it; ++it) {
                if (verbose) printf("%s%d", begin == it ? "" : ", ", *it);
            }
            if (verbose) printf("\n");
        }

// Finally, we check the allocator and verify that it's been used, and that
// the destruction of 'fibonacciList' freed all the memory allocated:

        ASSERT(oa.numBlocksTotal() > 0);
        ASSERT(0 == oa.numBlocksInUse());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE C'TOR
        //
        // Concern:
        //   That the value c'tor properly initializes the object.  Note this
        //   type has no value c'tor, it must be a POD, but we can do aggregate
        //   initialization similar to a value c'tor.
        //
        // Plan:
        //   Use the value c'tor in the 2 valid cases -- initializing to an
        //   empty list, and initializing to a list with 3 elements.
        // --------------------------------------------------------------------

        if (verbose) printf("VALUE C'TOR\n"
                            "===========\n");

        {
            Obj bucket = { 0, 0 };
            myCheckInvariants(bucket);
            ASSERT(0 == bucket.countElements());
            ASSERT(0 == bucket.first());
            ASSERT(0 == bucket.last());
        }

        Link l1;
        Link l2;
        Link l3;

        l1.setNextLink(&l2);
        l2.setNextLink(&l3);
        l3.setNextLink(0);

        l1.setPreviousLink(0);
        l2.setPreviousLink(&l1);
        l3.setPreviousLink(&l2);

        {
            Obj bucket = { &l1, &l3 };
            myCheckInvariants(bucket);
            ASSERT(3 == bucket.countElements());
            ASSERT(&l1 == bucket.first());
            ASSERT(&l3 == bucket.last());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT C'TOR, PRIMARY MANIPULATORS, AND D'TOR
        //
        // Concerns:
        //   That the default c'tor and primary manipulators function properly.
        //
        // Plan:
        //   Create an object with the default c'tor, verify that it represents
        //   a valid, empty list, then slowly build the list up the 3 elements
        //   using the 'setFirstAndLast', 'setFirst', and 'setLast'
        //   manipulators, verifying the state of the list all along.  Then
        //   reset the bucket with the 'reset' manipulator and verify it
        //   represents an empty list.  Call the 'checkInvariants' accessor
        //   after every manipulator to verify that invariants are preserved
        //   after all manipulator calls.
        // --------------------------------------------------------------------

        if (verbose) printf("DEFAULT C'TOR, PRIMARY MANIPULATORS, & D'TOR\n"
                            "============================================\n");

        Obj bucket = { 0, 0 };
        myCheckInvariants(bucket);
        ASSERT(0 == bucket.countElements());
        ASSERT(0 == bucket.first());
        ASSERT(0 == bucket.last());

        Link l2;
        l2.setNextLink(0);
        l2.setPreviousLink(0);

        bucket.setFirstAndLast(&l2, &l2);
        myCheckInvariants(bucket);
        ASSERT(1   == bucket.countElements());
        ASSERT(&l2 == bucket.first());
        ASSERT(&l2 == bucket.last());

        Link l3;
        l3.setNextLink(0);
        l3.setPreviousLink(&l2);
        l2.setNextLink(&l3);

        bucket.setLast(&l3);
        myCheckInvariants(bucket);
        ASSERT(2   == bucket.countElements());
        ASSERT(&l2 == bucket.first());
        ASSERT(&l3 == bucket.last());

        Link l1;
        l1.setNextLink(&l2);
        l1.setPreviousLink(0);
        l2.setPreviousLink(&l1);

        bucket.setFirst(&l1);
        myCheckInvariants(bucket);
        ASSERT(3   == bucket.countElements());
        ASSERT(&l1 == bucket.first());
        ASSERT(&l3 == bucket.last());

        bucket.reset();
        myCheckInvariants(bucket);
        ASSERT(0 == bucket.countElements());
        ASSERT(0 == bucket.first());
        ASSERT(0 == bucket.last());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        Link l1;
        Link l2;
        Link l3;

        l1.setNextLink(&l2);
        l2.setNextLink(&l3);
        l3.setNextLink(0);

        l1.setPreviousLink(0);
        l2.setPreviousLink(&l1);
        l3.setPreviousLink(&l2);

        Obj bucket = { &l1, &l3 };
        myCheckInvariants(bucket);
        ASSERT(3 == bucket.countElements());

        ASSERT(&l1 == bucket.first());
        ASSERT(&l3 == bucket.last());

        bucket.setLast(&l2);
        myCheckInvariants(bucket);
        ASSERT(2 == bucket.countElements());
        ASSERT(&l2 == bucket.last());

        bucket.setFirst(&l2);
        myCheckInvariants(bucket);
        ASSERT(1 == bucket.countElements());
        ASSERT(&l2 == bucket.first());

        bucket.setFirstAndLast(&l1, &l3);
        myCheckInvariants(bucket);
        ASSERT(3 == bucket.countElements());

        bucket.setFirstAndLast(&l2, &l2);
        myCheckInvariants(bucket);
        ASSERT(1 == bucket.countElements());

        l2.setNextLink(0);
        myCheckInvariants(bucket);
        ASSERT(1 == bucket.countElements());

        l2.setPreviousLink(0);
        myCheckInvariants(bucket);
        ASSERT(1 == bucket.countElements());
        ASSERT(&l2 == bucket.first());
        ASSERT(&l2 == bucket.last());

        bucket.reset();
        myCheckInvariants(bucket);
        ASSERT(0 == bucket.countElements());
        ASSERT(0 == bucket.first());
        ASSERT(0 == bucket.last());

        Obj bucket2 = { 0, 0 };
        myCheckInvariants(bucket2);
        ASSERT(0 == bucket2.countElements());
        ASSERT(0 == bucket2.first());
        ASSERT(0 == bucket2.last());
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
