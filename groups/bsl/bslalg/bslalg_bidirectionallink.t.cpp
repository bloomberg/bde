// bslalg_bidirectionallink.t.cpp                                     -*-C++-*-
#include <bslalg_bidirectionallink.h>

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
// Certain standard value-semantic-type test cases are omitted:
//: o [ 8] -- 'swap' is not implemented for this class.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//: o No memory is allocated from any allocator.
//
// Note that all contracts are wide, so there are no precondition violations
// to be detect in any build mode.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] BidirectionalLink()
// [ 6] BidirectionalLink(const BidirectionalLink& original);
// [ 2] ~BidirectionalLink();
//
// MANIPULATORS
// [ 7] BidirectionalLink& operator=(const BidirectionalLink& rhs);
// [ 2] void setNextLink(BidirectionalLink *next);
// [ 2] void setPreviousLink(BidirectionalLink *previous);
//
// ACCESSORS
// [ 4] BidirectionalLink *nextLink() const;
// [ 4] BidirectionalLink *previousLink() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ 3] 'gg' FUNCTION
// [ 2] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 4] CONCERN: All accessor methods are declared 'const'.

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslalg::BidirectionalLink Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
namespace BloombergLP {
namespace bslalg {

void debugprint(const Obj& val)
{
    printf("{ previousLink=0x%p, nextLink=0x%p }",
           val.previousLink(),
           val.nextLink());
}

}  // close package namespace
}  // close enterprise namespace

static Obj& gg(Obj *result, Obj *prev, Obj *next)
    // Initialize the specified 'result' with the specified 'prev', and 'next'.
{
    ASSERT(result);

    result->setPreviousLink(prev);
    result->setNextLink(next);
    return *result;
}

static bool haveSameState(const Obj& lhs, const Obj& rhs)
    // Convenience function to verify all attributes of the specified 'lhs' has
    // the same value as the attributes of the specified 'rhs'.  This is not
    // called '==', because two copies of 'Obj' can't really be equivalent
    // unless they reside in the same location of memory.
{
    return (lhs.previousLink() == rhs.previousLink()
             && lhs.nextLink() == rhs.nextLink());
}

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA (potentially) used by test cases 3, 6 and 7.

struct DefaultDataRow {
    int    d_line;           // source line number
    Obj *  d_prevNode;
    Obj *  d_nextNode;
};

Obj *const PTR1 = (Obj *)0xbaadf00ddeadc0deULL;
Obj *const PTR2 = (Obj *)0xbaadf00ddeadbeefULL;

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE     PREV        NEXT
    //----     ----        ----

    { L_,         0,          0 },

    // 'previousLink'
    { L_,  (Obj *)4,          0 },
    { L_,  PTR2,              0 },

    // 'nextLink'
    { L_,         0,   (Obj *)4 },
    { L_,         0,       PTR2 },

    // other
    { L_,  (Obj *)4,       PTR2 },
    { L_,      PTR2,   (Obj *)4 },
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
template <class PAYLOAD>
class MyNode : public bslalg::BidirectionalLink {
  public:
    // PUBLIC TYPES
    typedef PAYLOAD  ValueType;

  private:
    // DATA
    ValueType     d_value;

  private:
    // NOT IMPLEMENTED
    MyNode();
    MyNode(const MyNode&);
    MyNode& operator=(const MyNode&);

  public:
    // CREATOR
    ~MyNode() {}
        // Destroy this object.

    // MANIPULATOR
    ValueType& value() { return d_value; }
        // Return a reference to the modifiable value stored in this node.

    // ACCESSOR
    const ValueType& value() const { return d_value; }
        // Return a reference to the non-modifiable value stored in this node.
};
//..
// Next, we create the iterator helper class, which will eventually be
// defined as a nested type within the 'MyList' class.
//..
                            // ===============
                            // MyList_Iterator
                            // ===============

template <class PAYLOAD>
class MyList_Iterator {
    // PRIVATE TYPES
    typedef MyNode<PAYLOAD> Node;

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
    //! MyList_Iterator& operator=(const MyList_Iterator& other) = default;
    //! ~MyList_Iterator() = default;

    // MANIPULATORS
    MyList_Iterator operator++();

    // ACCESSORS
    PAYLOAD& operator*() const { return d_node->value(); }
};
//..
// Then, we define our 'MyList' class, with 'MyList::Iterator' being a public
// typedef of 'MyList_Iterator'.  For brevity, we will omit a lot of
// functionality that a full, general-purpose list class would have,
// implmenting only what we will need for this example.
//..
                                // ======
                                // MyList
                                // ======

template <class PAYLOAD>
class MyList {
    // PRIVATE TYPES
    typedef MyNode<PAYLOAD> Node;

  public:
    // PUBLIC TYPES
    typedef PAYLOAD                            ValueType;
    typedef MyList_Iterator<ValueType>         Iterator;

  private:
    // DATA
    Node             *d_begin;
    Node             *d_end;
    bslma::Allocator *d_allocator_p;

  public:
    // CREATORS
    explicit
    MyList(bslma::Allocator *basicAllocator = 0)
    : d_begin(0)
    , d_end(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {}

    ~MyList();

    // MANIPULATORS
    Iterator begin();
    Iterator end();
    void pushBack(const ValueType& value);
    void popBack();
};
//..
// Next, we implment the functions for the iterator type.
//..
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
//..
// Then, we implement the functions for the 'MyList' class:
//..
                                // ------
                                // MyList
                                // ------

// CREATORS
template <class PAYLOAD>
MyList<PAYLOAD>::~MyList()
{
    for (Node *p = d_begin; p; ) {
        Node *toDelete = p;
        p = (Node *) p->nextLink();

        d_allocator_p->deleteObjectRaw(toDelete);
    }
}

// MANIPULATORS
template <class PAYLOAD>
typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::begin()
{
    return Iterator(d_begin);
}

template <class PAYLOAD>
typename MyList<PAYLOAD>::Iterator MyList<PAYLOAD>::end()
{
    return Iterator(0);
}

template <class PAYLOAD>
void MyList<PAYLOAD>::pushBack(const PAYLOAD& value)
{
    Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
    node->setNextLink(0);
    node->setPreviousLink(d_end);
    bslalg::ScalarPrimitives::copyConstruct(&node->value(),
                                            value,
                                            d_allocator_p);

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

template <class PAYLOAD>
void MyList<PAYLOAD>::popBack()
{
    BSLS_ASSERT_SAFE(d_begin && d_end);

    Node *toDelete = d_end;
    d_end = (Node *) d_end->previousLink();

    if (d_begin != toDelete) {
        BSLS_ASSERT_SAFE(0 != d_end);
        d_end->setNextLink(0);
    }
    else {
        BSLS_ASSERT_SAFE(0 == d_end);
        d_begin = 0;
    }

    d_allocator_p->deleteObject(toDelete);
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

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case is memory allocated from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    // CONCERN: In no case is memory allocated from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    int expectedDefaultAllocations = 0;

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        expectedDefaultAllocations = -1;
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
      case 7: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-3.2):  (C-1, 3..4)
        //:
        //:     1 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       having the value 'W'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, the source object, 'Z', is a reference to the target
        //:   object, 'mX', and both 'mX' and 'ZZ' are initialized to have the
        //:   value 'V'.  For each row (representing a distinct object value,
        //:   'V') in the table described in P-2:  (C-5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-3)
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-5)
        //
        // Testing:
        //   BidirectionalLink& operator=(const BidirectionalLink& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose) printf(
                      "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCopy-assign every value into every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1 = DATA[ti].d_line;
            Obj *const PREV1 = DATA[ti].d_prevNode;
            Obj *const NEXT1 = DATA[ti].d_nextNode;

            Obj  mZ; gg( &mZ, PREV1, NEXT1);
            const Obj& Z = mZ;
            Obj mZZ; gg(&mZZ, PREV1, NEXT1);
            const Obj& ZZ = mZZ;

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2 = DATA[tj].d_line;
                Obj *const PREV2 = DATA[tj].d_prevNode;
                Obj *const NEXT2 = DATA[tj].d_nextNode;

                Obj mX; const Obj& X = gg(&mX, PREV2, NEXT2);

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                ASSERTV(LINE1, LINE2, Z, X,
                                      haveSameState(Z, X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, LINE2,  Z,   X, haveSameState(Z, X));
                ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                ASSERTV(LINE1, LINE2, ZZ, Z, haveSameState(ZZ, Z));
            }

            if (verbose) printf("Testing self-assignment\n");

            {
                Obj  mX; gg( &mX, PREV1, NEXT1);
                Obj mZZ; const Obj& ZZ = gg(&mZZ, PREV1, NEXT1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, haveSameState(ZZ, Z));

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, ZZ,   Z, haveSameState(ZZ, Z));
                ASSERTV(LINE1, mR, &mX, mR == &mX);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Use the copy constructor to create an object 'X',
        //:     supplying it the 'const' object 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:
        //:     (C-1, 3)
        //:
        //:     1 The newly constructed object, 'X', has the same value as 'Z'.
        //:       (C-1)
        //:
        //:     2 'Z' still has the same value as 'ZZ'.  (C-3)
        //
        // Testing:
        //   BidirectionalLink(const BidirectionalLink& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCopy construct an object from every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_line;
            Obj *const  PREV  = DATA[ti].d_prevNode;
            Obj *const  NEXT  = DATA[ti].d_nextNode;

            Obj  mZ;  const Obj  Z = gg( &mZ, PREV, NEXT);
            Obj mZZ;  const Obj ZZ = gg(&mZZ, PREV, NEXT);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX(Z);  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Verify the value of the object.

            ASSERTV(LINE,  Z, X,  haveSameState(Z, X));

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, haveSameState(ZZ, Z));
        }  // end for each row

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   This component does not support printing or streaming.

        if (verbose) printf("\nPRINT AND OUTPUT OPERATOR"
                            "\n=========================\n");

        if (verbose) printf(
                 "\nThis component does not support printing or streaming.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Use the default constructor to create an object (having default
        //:   attribute values).
        //:
        //: 2 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 3 For each salient attribute (contributing to value):  (C-1)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //
        // Testing:
        //   BidirectionalLink *previousLink() const
        //   BidirectionalLink *nextLink()() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        // Attribute Types

        typedef Obj * T1;  // 'prev'
        typedef Obj * T2;  // 'next'

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = 0;    // 'prev'
        const T2 D2 = 0;    // 'next'

        // -------------------------------------------------------
        // 'A' values: Boundary values.
        // -------------------------------------------------------

        const T1 A1 = PTR1;
        const T2 A2 = PTR2;

        if (verbose) printf("\nCreate an object.\n");

        Obj mX;  const Obj& X = gg(&mX, D1, D2);

        if (verbose) printf(
                     "\nVerify all basic accessors report expected values.\n");
        {
            ASSERTV(D1, X.previousLink(), D1 == X.previousLink());
            ASSERTV(D2, X.nextLink(),     D2 == X.nextLink());
        }

        if (verbose) printf(
                 "\nApply primary manipulators and verify expected values.\n");

        if (veryVerbose) { T_ Q(previousLink) }
        {
            mX.setPreviousLink(A1);

            const Obj *const prev = X.previousLink();
            ASSERTV(A1, prev, A1 == prev);
        }

        if (veryVerbose) { T_ Q(nextLink) }
        {
            mX.setNextLink(A2);

            const Obj *const next = X.nextLink();
            ASSERTV(A2, next, A2 == next);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'gg' FUNCTION
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The 'gg' can create an object having any value that does not
        //:   violate the constructor's documented preconditions.
        //:
        //: 2 Any argument can be 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..2)
        //:
        //:   1 Use the value constructor to create an object having the value
        //:     'V' supplying all the arguments as 'const'.  (C-2)
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to verify
        //:     that all of the attributes of each object have their expected
        //:     values.  (C-1)
        //
        // Testing:
        //   Obj& gg(Obj *r, Obj *l, Obj *r)
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR"
                            "\n==========\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCreate an object with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            Obj *const  LEFT   = DATA[ti].d_prevNode;
            Obj *const  RIGHT  = DATA[ti].d_nextNode;

            if (veryVerbose) { T_ P_(LEFT) P_(RIGHT) }

            Obj mX;  const Obj& X = gg(&mX, LEFT, RIGHT);

            if (veryVerbose) { T_ T_ P(X) }

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(LINE, LEFT,  X.previousLink(), LEFT  == X.previousLink());
            ASSERTV(LINE, RIGHT, X.nextLink(),     RIGHT == X.nextLink());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: 'D', 'A',
        //:   and 'B'.  'D' values correspond to the default attribute values,
        //:   and 'A' and 'B' values are chosen to be distinct boundary values
        //:   where possible.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //:
        //: 4 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-2, 4)
        //:
        //: 5 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to its 'B' value and verify
        //:   after each manipulation that only that attribute's value
        //:   changed.  (C-3)
        //
        // Testing:
        //   BidirectionalLink();
        //   ~BidirectionalLink();
        //   void setPreviousLink(BidirectionalLink *address)
        //   void setNextLink(BidirectionalLink *address)
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                            "\n==========================================\n");

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // 'D' values: These are the default-constructed values.

        Obj *const  D1 = 0;          // 'prev'
        Obj *const  D2 = 0;          // 'next'

        // 'A' values.

        Obj *const  A1 = (Obj *)0x08;
        Obj *const  A2 = (Obj *)0x0c;

        // 'B' values.

        Obj *const  B1 = PTR1;
        Obj *const  B2 = PTR2;

        if (verbose) printf(
                          "Create an object using the default constructor.\n");

        Obj mX;  const Obj& X = mX;

        if (verbose) printf("Verify the object's attribute values.\n");

        // -------------------------------------
        // Verify the object's attribute values.
        // -------------------------------------

        mX.setPreviousLink(D1);
        mX.setNextLink(D2);

        ASSERTV(D1, X.previousLink(), D1 == X.previousLink());
        ASSERTV(D2, X.nextLink(),     D2 == X.nextLink());

        if (verbose) printf(
                    "Verify that each attribute is independently settable.\n");

        // -----------
        // 'prev'
        // -----------
        {
            mX.setPreviousLink(A1);
            ASSERT(A1 == X.previousLink());
            ASSERT(D2 == X.nextLink());

            mX.setPreviousLink(B1);
            ASSERT(B1 == X.previousLink());
            ASSERT(D2 == X.nextLink());

            mX.setPreviousLink(D1);
            ASSERT(D1 == X.previousLink());
            ASSERT(D2 == X.nextLink());
        }

        // ------------
        // 'next'
        // ------------
        {
            mX.setNextLink(A2);
            ASSERT(D1 == X.previousLink());
            ASSERT(A2 == X.nextLink());

            mX.setNextLink(B2);
            ASSERT(D1 == X.previousLink());
            ASSERT(B2 == X.nextLink());

            mX.setNextLink(D2);
            ASSERT(D1 == X.previousLink());
            ASSERT(D2 == X.nextLink());
        }

        if (verbose) printf("Corroborate attribute independence.\n");
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            mX.setPreviousLink(A1);
            mX.setNextLink(A2);

            ASSERT(A1 == X.previousLink());
            ASSERT(A2 == X.nextLink());


            // ---------------------------------------
            // Set all attributes to their 'B' values.
            // ---------------------------------------

            mX.setPreviousLink(B1);
            ASSERT(B1 == X.previousLink());
            ASSERT(A2 == X.nextLink());

            mX.setNextLink(B2);
            ASSERT(B1 == X.previousLink());
            ASSERT(B2 == X.nextLink());

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

        // Attribute Types

        typedef Obj * T1;  // 'prev'
        typedef Obj * T2;  // 'next'

        // Attribute 1 Values: 'prev'

        const T1 D1 = 0;        // default value
        const T1 A1 = PTR2;

        // Attribute 2 Values: 'next'

        const T2 D2 = 0;        // default value
        const T2 A2 = PTR2;


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create an object 'w'.\n");

        Obj mW;  const Obj& W = mW;
        mW.setPreviousLink(D1);
        mW.setNextLink(D2);

        if (veryVerbose) printf("\ta. Check initial value of 'w'.\n");
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.previousLink());
        ASSERT(D2 == W.nextLink());

        if (veryVerbose) printf(
                               "\tb. Try equality operators: 'w' <op> 'w'.\n");

        ASSERT(1 == haveSameState(W, W));    ASSERT(0 == !haveSameState(W, W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create 'X' from 'W').\n");

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.previousLink());
        ASSERT(D2 == X.nextLink());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(1 == haveSameState(X, W));    ASSERT(0 == !haveSameState(X, W));
        ASSERT(1 == haveSameState(X, X));    ASSERT(0 == !haveSameState(X, X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
                    "\n Set 'X' with values 'A' (value distinct from 'D').\n");

        mX.setPreviousLink(A1);
        mX.setNextLink(A2);

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.previousLink());
        ASSERT(A2 == X.nextLink());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(0 == haveSameState(X, W));  ASSERT(1 == !haveSameState(X, W));
        ASSERT(1 == haveSameState(X, X));  ASSERT(0 == !haveSameState(X, X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create 'Y' and set 'Y' with 'X'.\n");

        Obj mY;  const Obj& Y = mY;
        mY = X;

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == Y.previousLink());
        ASSERT(A2 == Y.nextLink());

        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(0 == haveSameState(X, W));    ASSERT(1 == !haveSameState(X, W));
        ASSERT(1 == haveSameState(X, X));    ASSERT(0 == !haveSameState(X, X));
        ASSERT(1 == haveSameState(X, Y));    ASSERT(0 == !haveSameState(X, Y));

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case is memory allocated from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            expectedDefaultAllocations < 0 || expectedDefaultAllocations ==
                                      (int) defaultAllocator.numBlocksTotal());

    // CONCERN: In no case is memory allocated from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
