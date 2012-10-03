// bslalg_bidirectionalnode.t.cpp                                     -*-C++-*-
#include <bslalg_bidirectionalnode.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <new>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// MANIPULATORS
// [ 2] VALUE_TYPE& value();
// [ 2] const VALUE_TYPE& value() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [  ] CONCERN: 'value' can be constructed with 'allocator_traits'.

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class TestType1 {
    // CLASS DATA
    static int s_numConstructions;

    // DATA
    int d_value;

  public:
    // CLASS METHODS
    static int numConstructions() { return s_numConstructions; }

    // CREATORS
    explicit
    TestType1(int i) : d_value(i) { ++s_numConstructions; }
    ~TestType1()                  { --s_numConstructions; }

    // MANIPULATOR
    void set(int i) { d_value = i; }

    // ACCESSOR
    int get() const { return d_value; }
};

int TestType1::s_numConstructions = 0;

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
    //! MyList_Iterator& operator=(const MyList_Iterator& other) = default;
    //! ~MyList_Iterator() = default;

    // MANIPULATORS
    MyList_Iterator operator++();

    // ACCESSORS
    PAYLOAD& operator*() const { return d_node->value(); }
};

// Then, we define our 'MyList' class, with 'MyList::Iterator' being a public
// typedef of 'MyList_Iterator'.  For brevity, we will omit a lot of
// functionality that a full, general-purpose list class would have,
// implmenting only what we will need for this example.

                                // ======
                                // MyList
                                // ======

template <typename PAYLOAD>
class MyList {
    // PRIVATE TYPES
    typedef bslalg::BidirectionalNode<PAYLOAD> Node;

  public:
    // PUBLIC TYPES
    typedef PAYLOAD                            ValueType;
    typedef MyList_Iterator<ValueType>         Iterator;

    // DATA
    Node             *d_begin;
    Node             *d_end;
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit
    MyList(bslma::Allocator *basicAllocator)
    : d_begin(0)
    , d_end(0)
    , d_allocator_p(basicAllocator)
    {}

    ~MyList();

    // MANIPULATORS
    Iterator begin();
    Iterator end();
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

    for (Node *p = d_begin; p; ) {
        Node *condemned = p;
        p = (Node *) p->nextLink();

        condemned->value().~ValueType();
        d_allocator_p->deleteObjectRaw(static_cast<BDL *>(condemned));
    }
}

// MANIPULATORS
template <typename PAYLOAD>
typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::begin()
{
    return Iterator(d_begin);
}

template <typename PAYLOAD>
typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::end()
{
    return Iterator(0);
}

template <typename PAYLOAD>
void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
{
    Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
    node->setNextLink(0);
    node->setPreviousLink(d_end);
    new (&node->value()) ValueType(value);

    if (d_end) {
        BSLS_ASSERT_SAFE(d_begin);

        d_end->setNextLink(node);
        d_end = node;
    }
    else {
        BSLS_ASSERT_SAFE(0 == d_begin);

        d_begin = d_end = node;
    }
}

template <typename PAYLOAD>
void MyList<PAYLOAD>::popBack()
{
    BSLS_ASSERT_SAFE(d_begin && d_end);

    Node *condemned = d_end;
    d_end = (Node *) d_end->previousLink();

    if (d_begin != condemned) {
        BSLS_ASSERT_SAFE(0 != d_end);
        d_end->setNextLink(0);
    }
    else {
        BSLS_ASSERT_SAFE(0 == d_end);
        d_begin = 0;
    }

    condemned->value().~ValueType();
    d_allocator_p->deallocate(condemned);
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
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case is memory allocated from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE
        //
        // Concern:
        //   Demonstrate the usefulness of the 'BidirectionalNode' type.
        //
        // Plan:
        //   Use it to build a linked list.
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Next, we have finished implmenting our 'MyList' class and its 'Iterator'
// type, we will use one to store a fibonacci sequence of ints.  We declare the
// memory allocator that we will use:

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
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //
        // Concerns:
        //: 1 Manipulators can set value.
        //:
        //: 2 Accessor return value set by maninpulator.
        //:
        //: 3 Accessor is declared const.
        //
        // Plan:
        //: 1 Create a 'BidirectionalNode' with 'VALUE_TYPE' as 'int' and set
        //:   'value' distinct numbers.  Verify the values are set with the
        //:   accessor.
        //:
        //: 2 Create a 'BidirectionalNode' with a type that has a constructor
        //:   that can be verified if it has been invoked.  Verify that the
        //:   constructor is invoked when 'allocator_traits::construct' is
        //:   used.
        //
        // Testing:
        //   VALUE_TYPE& value();
        //   const VALUE_TYPE& value() const;
        // --------------------------------------------------------------------

        bslma::TestAllocator da("default");
        bslma::TestAllocator oa("object");

        bslma::DefaultAllocatorGuard defaultGuard(&da);

        if (verbose) printf("\nTesting for payload of 'int'.\n");
        {
            typedef bslalg::BidirectionalNode<int> Obj;

            typedef Obj::ValueType VT;

            Obj *xPtr = (Obj *) oa.allocate(sizeof(Obj));
            Obj& mX = *xPtr; const Obj& X = mX;

            ::new (&xPtr->value()) VT(7);
            ASSERTV(X.value(), 7 == X.value());

            mX.value() = 5;
            ASSERTV(X.value(),  5 == X.value());

            mX.value() = 21;
            ASSERTV(X.value(), 21 == X.value());

            mX.value() = -3;
            ASSERTV(X.value(), -3 == X.value());

            ASSERTV(0 == da.numBlocksTotal());
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value().~VT();
            oa.deallocate(xPtr);
            ASSERTV(0 == oa.numBlocksInUse());
        }

        if (verbose) printf("\nTesting for payload of 'TestType1'.\n");
        {
            typedef bslalg::BidirectionalNode<TestType1> Obj;
            typedef Obj::ValueType VT;

            Obj *xPtr = (Obj *) oa.allocate(sizeof(Obj));
            Obj& mX = *xPtr; const Obj& X = mX;

            ::new (&xPtr->value()) Obj::ValueType(7);
            ASSERTV(1 == TestType1::numConstructions());
            ASSERTV(X.value().get(), 7 == X.value().get());

            mX.value().set(5);
            ASSERTV(X.value().get(),  5 == X.value().get());

            mX.value().set(21);
            ASSERTV(X.value().get(), 21 == X.value().get());

            mX.value().set(-3);
            ASSERTV(X.value().get(), -3 == X.value().get());

            ASSERTV(0 == da.numBlocksTotal());
            ASSERTV(1 == oa.numBlocksInUse());

            mX.value().~VT();
            oa.deallocate(&mX);
            ASSERTV(0 == oa.numBlocksInUse());

            ASSERTV(0 == TestType1::numConstructions());
        }
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

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator da("default");
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        typedef bslalg::BidirectionalNode<int> Obj;

        Obj *xPtr = static_cast<Obj *>(da.allocate(sizeof(Obj)));
        typedef bslalg::BidirectionalNode<int> Obj;
        Obj& mX = *xPtr; const Obj& X = mX;

        mX.value() = 0;
        ASSERTV(X.value(), 0 == X.value());

        mX.value() = 1;
        ASSERTV(X.value(), 1 == X.value());

        da.deallocate(&mX);
        ASSERTV(0 == da.numBytesInUse());

       } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(0 == TestType1::numConstructions());

    // CONCERN: In no case is memory allocated from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
