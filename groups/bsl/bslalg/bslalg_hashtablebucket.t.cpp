// bslalg_hashtablebucket.t.cpp                                       -*-C++-*-
#include <bslalg_hashtablebucket.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_bidirectionalnode.h>
#include <bslalg_scalarprimitives.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'

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
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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

template <class PAYLOAD>
class MyList_Iterator {
    // 'Iterator' type for class 'MyList'.  This class will be typedef'ed to be
    // a nested class within 'MyList'.

    // PRIVATE TYPES
    typedef bslalg::BidirectionalNode<PAYLOAD> Node;

    // DATA
    Node *d_node;

    // FRIENDS
    template <class PL>
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

template <class PAYLOAD>
class MyList : public bslalg::HashTableBucket {
    // This class stores a doubly-linked list containing objects of type
    // 'PAYLOAD'.

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
    MyList(bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
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
template <class PAYLOAD>
MyList_Iterator<PAYLOAD> MyList_Iterator<PAYLOAD>::operator++()
{
    d_node = (Node *) d_node->nextLink();
    return *this;
}

template <class PAYLOAD>
inline
bool operator==(MyList_Iterator<PAYLOAD> lhs,
                MyList_Iterator<PAYLOAD> rhs)
{
    return lhs.d_node == rhs.d_node;
}

template <class PAYLOAD>
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
template <class PAYLOAD>
MyList<PAYLOAD>::~MyList()
{
    typedef bslalg::BidirectionalLink BDL;

    for (Node *p = (Node *) first(); p; ) {
        Node *toDelete = p;
        p = (Node *) p->nextLink();

        toDelete->value().~ValueType();
        d_allocator_p->deleteObjectRaw(static_cast<BDL *>(toDelete));
    }

    reset();
}

// MANIPULATORS
template <class PAYLOAD>
void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
{
    Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
    node->setNextLink(0);
    node->setPreviousLink(last());
    bslalg::ScalarPrimitives::copyConstruct(&node->value(),
                                            value,
                                            d_allocator_p);

    if (0 == last()) {
        BSLS_ASSERT_SAFE(0 == first());

        setFirstAndLast(node, node);
    }
    else {
        last()->setNextLink(node);
        setLast(node);
    }
}

template <class PAYLOAD>
void MyList<PAYLOAD>::popBack()
{
    BSLS_ASSERT_SAFE(first() && last());

    Node *toDelete = (Node *) last();

    if (first() != toDelete) {
        BSLS_ASSERT_SAFE(0 != last());
        setLast(last()->previousLink());
        last()->setNextLink(0);
    }
    else {
        reset();
    }

    d_allocator_p->deleteObject(toDelete);
}

//=============================================================================
//                     General Functions for Testing
//-----------------------------------------------------------------------------

void myCheckInvariants(const bslalg::HashTableBucket& bucket)
{
    ASSERT(bslalg::BidirectionalLinkListUtil::isWellFormed(bucket.first(),
                                                           bucket.last()));
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

//..
// Next, in 'main', we use our 'MyList' class to store a list of ints:
//..
        MyList<int> intList;
//..
// Then, we declare an array of ints to populate it with:
//..
        int intArray[] = { 8, 2, 3, 5, 7, 2 };
        enum { NUM_INTS = sizeof intArray / sizeof *intArray };
//..
// Now, we iterate, pushing ints to the list:
//..
        for (const int *pInt = intArray; pInt != intArray + NUM_INTS; ++pInt) {
            intList.pushBack(*pInt);
        }
//..
// Finally, we use our 'Iterator' type to traverse the list and observe its
// values:
//..
        MyList<int>::Iterator it = intList.begin();
        ASSERT(8 == *it);
        ASSERT(2 == *++it);
        ASSERT(3 == *++it);
        ASSERT(5 == *++it);
        ASSERT(7 == *++it);
        ASSERT(2 == *++it);
        ASSERT(intList.end() == ++it);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // EQUALITY, COPY C'TOR, and ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("EQUALITY, COPY C'TOR, and ASSIGNMENT\n"
                            "====================================\n");

        Link l1;
        Link l2;
        Link l3;
        Link l4;
        Link l5;

        l1.setNextLink(&l2);
        l2.setNextLink(&l3);
        l3.setNextLink(&l4);
        l4.setNextLink(&l5);
        l5.setNextLink(0);

        l1.setPreviousLink(0);
        l2.setPreviousLink(&l1);
        l3.setPreviousLink(&l2);
        l4.setPreviousLink(&l3);
        l5.setPreviousLink(&l4);

        Obj mX = { &l1, &l3 }; const Obj& X = mX;
        myCheckInvariants(X);
        ASSERT(3 == X.countElements());
        ASSERT(&l1 == X.first());
        ASSERT(&l3 == X.last());

        ASSERT(X == X);         ASSERT(!(X != X));

        Obj mY(X);             const Obj& Y = mY;
        myCheckInvariants(Y);
        ASSERT(3 == Y.countElements());
        ASSERT(&l1 == Y.first());
        ASSERT(&l3 == Y.last());

        ASSERT(X == Y);         ASSERT(!(X != Y));
        ASSERT(Y == X);         ASSERT(!(Y != X));

        mX.reset();
        myCheckInvariants(X);
        ASSERT(0 == X.countElements());
        ASSERT(0 == X.first());
        ASSERT(0 == X.last());

        ASSERT(X == X);         ASSERT(!(X != X));

        ASSERT(X != Y);         ASSERT(!(X == Y));
        ASSERT(Y != X);         ASSERT(!(Y == X));

        mX = Y;
        myCheckInvariants(X);
        ASSERT(3 == X.countElements());
        ASSERT(&l1 == X.first());
        ASSERT(&l3 == X.last());

        ASSERT(X == X);         ASSERT(!(X != X));

        ASSERT(X == Y);         ASSERT(!(X != Y));
        ASSERT(Y == X);         ASSERT(!(Y != X));

        mY.setLast(&l5);
        myCheckInvariants(Y);
        ASSERT(5 == Y.countElements());
        ASSERT(&l1 == Y.first());
        ASSERT(&l5 == Y.last());

        ASSERT(Y == Y);         ASSERT(!(Y != Y));

        ASSERT(X != Y);         ASSERT(!(X == Y));
        ASSERT(Y != X);         ASSERT(!(Y == X));

        mX = Y;

        ASSERT(X == X);         ASSERT(!(X != X));

        ASSERT(X == Y);         ASSERT(!(X != Y));
        ASSERT(Y == X);         ASSERT(!(Y != X));

        Obj mZ = { 0, 0 };      const Obj& Z = mZ;
        myCheckInvariants(Z);
        ASSERT(0 == Z.countElements());
        ASSERT(0 == Z.first());
        ASSERT(0 == Z.last());

        ASSERT(Z == Z);         ASSERT(!(Z != Z));

        ASSERT(X != Z);         ASSERT(!(X == Z));
        ASSERT(Z != X);         ASSERT(!(Z == X));

        Obj mA(Z);              const Obj& A = mA;

        ASSERT(A == A);         ASSERT(!(A != A));

        ASSERT(A == Z);         ASSERT(!(A != Z));
        ASSERT(Z == A);         ASSERT(!(Z != A));

        ASSERT(X != A);         ASSERT(!(X == A));
        ASSERT(A != X);         ASSERT(!(A == X));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE C'TOR AND BASIC ACCESSORS
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
            ASSERT(0 == bucket.end());
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
            ASSERT(0   == bucket.end());
        }

        Link l4;
        l3.setNextLink(&l4);
        l4.setPreviousLink(&l3);
        l4.setNextLink(0);

        {
            Obj bucket = { &l1, &l3 };
            myCheckInvariants(bucket);
            ASSERT(3 == bucket.countElements());
            ASSERT(&l1 == bucket.first());
            ASSERT(&l3 == bucket.last());
            ASSERT(&l4 == bucket.end());
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
