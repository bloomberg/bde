// bslstl_hashtableiterator.t.cpp                                     -*-C++-*-
#include <bslstl_hashtableiterator.h>

#include <bslstl_allocator.h>
#include <bslstl_bidirectionalnodepool.h>

#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_hashtableanchor.h>
#include <bslalg_hashtablebucket.h>
#include <bslalg_hashtableimputil.h>

#include <bslma_testallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_util.h>

#include <bsltf_templatetestfacility.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test defines a class template,
// 'bslstl::HashTableIterator', that is an in-core value-semantic type.  This
// class provides a standard-conforming forward iterator (see section 24.2.5
// [forward.iterators] of the C++11 standard) over a list of
// 'bslalg::BidirectionalLink' objects.
//
// The primary manipulators of this class is the value constructor, which is
// sufficient to allow an object to attain any achievable state.  The basic
// accessor of the class is the 'node' method, which returns the address of
// the node ('BidirectionalNode' object) referred to by a iterator.
//
// Primary Manipulator:
//: o explicit HashTableIterator(bslalg::BidirectionalLink *node);
//
// Basic Accessors:
//: o bslalg::BidirectionalLink *node() const;
//
// Since the state of an object will be meaningful if the object refers to a
// valid node.  The facilities provided by 'bslalg::BidirectionalLinkListUtil'
// will be used to create a list of nodes to be referred to by objects of this
// class.  This particular class provides a value constructor, which is also
// the primary manipulator, that is capable of creating an object in any state,
// obviating the primitive generator function, 'gg', which is normally used for
// this purpose.
//
// We will follow our standard 10-case approach to testing value-semantic types
// with some exceptions:
//: o We do not need to test anything in case 3 and 8, because the value
//:   constructor will have been already tested as the primary manipulator in
//:   case 2.
//: o We do not need to test anything in case 5, because this class does not
//:   provide a 'print' method or 'operator<<'.
//: o We do not need to test anything in case 10, because this class does not
//:   provide bdex stream operators.
//: o We will test 'operator*' and 'operator->' together with the 'node' method
//:   in test case 2, because these operator simply provide different ways to
//:   access to the value of the node.
//
// Global Concerns:
//: o No memory is ever allocated.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] HashTableIterator();
// [ 2] explicit HashTableIterator(bslalg::BidirectionalLink *node);
// [ 7] HashTableIterator(const NcIter& original);
// [ 7] HashTableIterator(const HashTableIterator& original) = default;
// [ 2] ~HashTableIterator();
//
// MANIPULATORS
// [ 9] HashTableIterator& operator=(const HashTableIterator& rhs);
// [11] HashTableIterator& operator++();
//
// ACCESSORS
// [ 4] reference operator*() const;
// [ 4] pointer operator->() const;
// [ 4] bslalg::BidirectionalLink *node() const;
//
// FREE OPERATORS
// [ 6] bool operator==(HashTableIterator& lhs, HashTableIterator& rhs);
// [ 6] bool operator!=(HashTableIterator& lhs, HashTableIterator& rhs);
// [12] HashTableIterator operator++(iter, int);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
// [ *] CONCERN: In no case does memory come from the global allocator.
// [13] TYPE TRAITS

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

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//             GLOBAL TYPEDEFS, FUNCTIONS AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

typedef bsltf::TemplateTestFacility Ttf;
typedef bslalg::BidirectionalLinkListUtil Util;

namespace {

template <class VALUE>
class NodePool {
  private:
    // DATA
    bslstl::BidirectionalNodePool<VALUE, bsl::allocator<VALUE> > d_pool;

  public:
    // TYPES
    typedef bslalg::BidirectionalNode<VALUE> Node;

    // CREATORS
    explicit NodePool(bslma::Allocator *basicAllocator)
    : d_pool(basicAllocator)
    {
    }

    // MANIPULATORS
    Node *createNode(int value)
    {
        return static_cast<Node *>(d_pool.createNode(
                                                   Ttf::create<VALUE>(value)));
    }

    void deleteNode(Node *node)
    {
        d_pool.deleteNode(node);
    }
};

template <class VALUE>
class TestDriver {

  private:
    // TYPES
    typedef bslstl::HashTableIterator<VALUE, ptrdiff_t> Obj;
    typedef NodePool<VALUE> Pool;
    typedef bslalg::BidirectionalNode<VALUE> Node;
    typedef bslalg::BidirectionalLink Link;

  public:
    // TEST CASES
    static void testCase13();
        // Test type-traits.

    static void testCase12();
        // Test post-increment operator.

    static void testCase11();
        // Test pre-increment operator.

    static void testCase9();
        // Test copy-assignment operator.

    static void testCase7();
        // Test copy-constructors.

    static void testCase6();
        // Test equality-comparison operators.

    static void testCase4();
        // Test basic accessors.

    static void testCase2();
        // Test primary manipulators.
};

template<class VALUE>
void TestDriver<VALUE>::testCase13()
{
    // ------------------------------------------------------------------------
    // TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits for a standard-conforming
    //:   forward iterator.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits.
    // ------------------------------------------------------------------------

    // Verify set defines the expected traits.
    BSLMF_ASSERT((1 == bslmf::IsSame<typename Obj::value_type, VALUE>::value));
    BSLMF_ASSERT((1 == bslmf::IsSame<typename Obj::difference_type,
                                     ptrdiff_t>::value));
    BSLMF_ASSERT((1 == bslmf::IsSame<typename Obj::pointer, VALUE*>::value));
    BSLMF_ASSERT((1 == bslmf::IsSame<typename Obj::reference, VALUE&>::value));
    BSLMF_ASSERT((1 == bslmf::IsSame<typename Obj::iterator_category,
                                     bsl::forward_iterator_tag>::value));
}


template <class VALUE>
void TestDriver<VALUE>::testCase12()
{
    // ------------------------------------------------------------------------
    // POST-INCREMENT OPERATOR
    //   Ensure that 'operator++(iter, int)' behaves according to its
    //   contract.
    //
    // Concerns:
    //:  1 The post-increment operator changes the value of the object to
    //:    refer to the next element in the list.
    //:
    //:  2 The signature and return type are standard.
    //:
    //:  3 The value returned is the value of the object prior to the
    //:    operator call.
    //:
    //:  4 Post-incrementing an object referring to the last element in the
    //:    list moves the object to point to the sentinel node.
    //
    // Plan:
    //: 1 Use the address of 'operator++(iter, int)' to initialize a
    //:   member-function pointer having the appropriate signature and
    //:   return type for the post-increment operator defined in this
    //:   component.  (C-2)
    //:
    //: 2 Create a list with N nodes.  (C-1,3,4)
    //:
    //: 3 For each node 'N1' node in the list of P-2:
    //:
    //:   1 Create a modifiable 'Obj', 'mX', and a 'const' 'Obj', 'Y', both
    //:     pointing to 'N1'.
    //:
    //:   2 Create a 'const' 'Obj', 'Z', pointing the node to the right of
    //:     'N1'.
    //:
    //:   2 Invoke the post-increment operator on 'mX'.
    //:
    //:   3 Verify that value returned compare equals to that of 'Y'.
    //:     (C-3)
    //:
    //:   4 Verify using the equality-comparison operator that 'mX' has the
    //:     same value as that of 'Z'.  (C-1, 4)
    //
    // Testing:
    //   HashTableIterator operator++(iter, int);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nPost-Increment OPERATOR"
                        "\n=======================\n");

    // Assign the address of the operator to a variable.
    {
        typedef Obj (*operatorPtr)(Obj&, int);

        // Verify that the signature and return type are standard.

        operatorPtr operatorPostincrement = &bslstl::operator++;

        (void) operatorPostincrement;  // quash potential compiler warning
    }

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    // Test the post-increment operator.
    for (int ti = 1; ti < N; ++ti) {

        Obj mX(nodes[ti]);
        const Obj Y(nodes[ti]);

        Link *address;
        if (ti == N - 1) {
            address = 0;
        }
        else {
            address = nodes[ti + 1];
        }
        const Obj Z(address);

        const Obj mR = mX++;

        if (veryVerbose) { T_ T_ P_(ti) }

        ASSERTV(ti, mR == Y);
        ASSERTV(ti, Z == mX);
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase11()
{
    // ------------------------------------------------------------------------
    // PRE-INCREMENT OPERATOR
    //   Ensure that 'operator++' behaves according to its contract.
    //
    // Concerns:
    //:  1 The pre-increment operator changes the value of the object to
    //:    refer to the next element in the list.
    //:
    //:  2 The signature and return type are standard.
    //:
    //:  3 The reference returned refers to the object on which the operator
    //:    was invoked.
    //:
    //:  4 Pre-increment an object referring to the rightmost node in
    //:    the list moves the object to point to the past-the-end address.
    //
    // Plan:
    //: 1 Use the address of 'operator++' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   pre-decrement operator defined in this component.  (C-2)
    //:
    //: 2 Create a list with N nodes.  (C-1,3,4)
    //:
    //: 3 For each node 'N1' (including the sentinel node) that is not the
    //:   leftmost node in the tree of P-2:
    //:
    //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
    //:
    //:   2 Create a 'const' 'Obj', 'Z', pointing to the node to the right
    //:     of 'N1'.
    //:
    //:   2 Invoke the pre-increment operator on 'mX'.
    //:
    //:   3 Verify that the address of the return value is the same as
    //:     that of 'mX'.  (C-3)
    //:
    //:   4 Verify using the equality-comparison operator that 'mX' has the
    //:     same value as that of 'Z'.  (C-1, 4)
    //
    // Testing:
    //   HashTableIterator operator++();
    // ------------------------------------------------------------------------

    if (verbose) printf("\nPRE-INCREMENT OPERATOR"
                        "\n======================\n");

    //Assign the address of the operator to a variable.
    {
        typedef Obj& (Obj::*operatorPtr)();

        // Verify that the signature and return type are standard.

        operatorPtr operatorPredecrement = &Obj::operator++;

        (void) operatorPredecrement;  // quash potential compiler warning
    }

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    //Test the pre-increment operator.
    for (int ti = 0; ti < N; ++ti) {

        Obj mX(nodes[ti]);
        Link *address;
        if (ti == N - 1) {
            address = 0;
        }
        else {
            address = nodes[ti + 1];
        }
        const Obj Z(address);

        Obj *mR = &(++mX);

        if (veryVerbose) {
            T_ T_ P_(ti) P_(mR) }

        ASSERTV(ti, mR, &mX, mR == &mX);

        ASSERTV(ti, Z == mX);
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase9()
{
    // ------------------------------------------------------------------------
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
    //: 2 Create a list with N nodes.
    //:
    //: 3 For each node 'N1' in the list of P-2:  (C-1,3..4)
    //:
    //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', pointing to 'N1'.
    //:
    //:   2 For each node 'N2' in the list of P-2:  (C-1,3..4)
    //:
    //:     1 Create a modifiable 'Obj', 'mX', pointing to 'N2'.
    //:
    //:     2 Assign 'mX' from 'Z'.  (C-1)
    //:
    //:     3 Verify that the address of the return value is the same as
    //:       that of 'mX'.  (C-3)
    //:
    //:     4 Use the equality-comparison operator to verify that:
    //:
    //:       1 The target object, 'mX', now has the same value as that of
    //:         'Z'.  (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
    //:
    //: 4 For each node 'N1' in list of P-2:  (C-3, 5)
    //:
    //:   1 Create a modifiable 'Obj', 'mX', pointing to 'N1'.
    //:
    //:   1 Create a 'const' 'Obj', 'ZZ', pointing to 'N1'.
    //:
    //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   3 Assign 'mX' from 'Z'.
    //:
    //:   4 Verify that the address of the return value is the same as
    //:       that of 'mX'.  (C-3)
    //:
    //:   5 Use the equal-comparison operator to verify that 'mX' has the
    //:     same value as 'ZZ'.  (C-5)
    //
    // Testing:
    //   HashTableIterator& operator=(const HashTableIterator& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                        "\n========================\n");

    // Assign the address of the operator to a variable.

    {
        typedef Obj& (Obj::*operatorPtr)(const Obj&);

        // Verify that the signature and return type are standard.

        operatorPtr operatorAssignment = &Obj::operator=;

        (void) operatorAssignment;  // quash potential compiler warning
    }

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    // Test the assignment operator on the cross product of the iterators.

    for (int ti = 0; ti < N; ++ti) {

        const Obj Z(nodes[ti]);
        const Obj ZZ(nodes[ti]);

        for (int tj = 0; tj < N; ++tj) {

            Obj mX(nodes[tj]);

            Obj *mR = &(mX = Z);
            ASSERTV(ti, tj, mR, &mX, mR == &mX);

            ASSERTV(ti, tj, Z == mX);
            ASSERTV(ti, tj, Z == ZZ);
        }
    }

    // Test for aliasing.

    for (int ti = 0; ti < N; ++ti) {

        Obj mX(nodes[ti]);
        const Obj ZZ(nodes[ti]);

        const Obj& Z = mX;
        Obj *mR = &(mX = Z);
        ASSERTV(ti, mR, &mX, mR == &mX);

        ASSERTV(ti, Z == mX);
        ASSERTV(ti, Z == ZZ);
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase7()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTORS
    //   Ensure that we can create a distinct object of the class from any
    //   other one, such that the two objects have the same value.
    //
    // Concerns:
    //: 1 The copy constructor creates an object having the same value as that
    //:   of the supplied original object.
    //:
    //: 2 The original object is passed as a reference providing non-modifiable
    //:   access to that object.
    //:
    //: 3 The value of the original object is unchanged.
    //:
    //: 4 An object having the (template parameter) non-'const'-qualified value
    //:    type can be used to create an object having a the same
    //:    'const'-qualified value type.
    //
    // Plan:
    //: 1 Create a list with N nodes.
    //:
    //: 2 For each node 'N1' in the list of P-1:  (C-1..4)
    //:
    //:   1 Create two 'const' 'Obj' 'Z', and 'ZZ' both pointing to 'N1'.
    //:
    //:   2 Use the copy constructor to create an object 'X1', supplying it the
    //:     'const' object 'Z'.  (C-2)
    //:
    //:   3 Verify that the newly constructed object 'X1', has the same value
    //:     as that of 'Z'.  Verify that 'Z' still has the same value as that
    //:     of 'ZZ'.  (C-1,3)
    //:
    //:   4 Use the copy constructor to create an object having
    //:     'const'-qualified value type 'X2', supplying the object 'Z'.
    //      (C-2, 4)
    //:
    //:   5 Verify that the newly constructed object 'X2', has the same value
    //:     as that of 'ZZ'.  Verify that 'Z' still has the same value as that
    //:     of 'ZZ'.  (C-1,3)
    //
    // Testing:
    //   HashTableIterator(const NcIter& original);
    //   HashTableIterator(const HashTableIterator& original);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nCOPY CONSTRUCTORS"
                        "\n=================\n");

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    // Test using the copy constructor on every value.
    for (int ti = 0; ti < N; ++ti) {

        const Obj Z(nodes[ti]);
        const Obj ZZ(nodes[ti]);

        const Obj X1(Z);

        if (veryVerbose) { T_ T_ P(ti) }

        ASSERTV(Z == X1);
        ASSERTV(Z == ZZ);

        typedef bslstl::HashTableIterator<const VALUE, ptrdiff_t> ObjC;
        const ObjC X2(Z);

        ASSERTV(Z == X2);
        ASSERTV(Z == ZZ);
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase6()
{
    // ------------------------------------------------------------------------
    // EQUALITY-COMPARISON OPERATORS
    //   Ensure that '==' and '!=' are the operational definition of value.
    //
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they point
    //:   to the same node in the same list.
    //:
    //: 2 'true  == (X == X)'  (i.e., identity)
    //:
    //: 3 'false == (X != X)'  (i.e., identity)
    //:
    //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
    //:
    //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if '!(X == Y)'
    //:
    //: 7 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 8 Non-modifiable objects can be compared (i.e., objects or
    //:   references providing only non-modifiable access).
    //:
    //: 9 The equality operator's signature and return type are standard.
    //:
    //:10 The inequality operator's signature and return type are standard.
    //:
    //:11 The equality-comparison operators can be used on objects
    //:   parameterized on both a 'const' and non-'const' value type.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality-comparison
    //:   operators defined in this component.  (C-7..11)
    //:
    //: 2 Create a list with N nodes.
    //:
    //: 3 For each node 'N1' in the list of P-2:  (C-1..6)
    //:
    //:   1 Create a single object pointing to 'N1', and use it to verity
    //:     the reflexive (anti-reflexive) property of equality
    //:     (inequality) in the presence of aliasing.  (C-2..3)
    //:
    //:   2 For each node 'N2' in the list of P-2:  (C-1, 4..6)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created
    //:       from 'N1' and 'N2', respectively, are expected to have the
    //:       same value.
    //:
    //:     2 Create objects 'X1' and 'X2' parameterized on 'const' and
    //:       non-'const' value type having the value of 'N1'.  Create objects
    //:       'Y1' and 'Y2' parameterized on 'const' and non-'const' value type
    //:       having the value of 'N2'.
    //:
    //:     3 Verify the commutativity property and the expected return value
    //:       of both '==' and '!=' for each of the sets ('X1', 'Y1'), ('X1',
    //:       'Y2'), ('X2', 'Y1'), and ('X2', 'Y2').  (C-1, 4..6, 11)
    //
    // Testing:
    //   bool operator==(const HashTableIterator& lhs, rhs);
    //   bool operator!=(const HashTableIterator& lhs, rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    typedef bslstl::HashTableIterator<const VALUE, ptrdiff_t> ObjC;

    //Assign the address of each operator to a variable.
    {
        typedef bool (*operatorPtrNN)(const Obj&, const Obj&);
        typedef bool (*operatorPtrNC)(const Obj&, const ObjC&);
        typedef bool (*operatorPtrCN)(const ObjC&, const Obj&);
        typedef bool (*operatorPtrCC)(const ObjC&, const ObjC&);

        // Verify that the signatures and return types are standard.

        operatorPtrNN operatorEqNN = bslstl::operator==;
        operatorPtrNN operatorNeNN = bslstl::operator!=;
        (void) operatorEqNN;  // quash potential compiler warnings
        (void) operatorNeNN;

        operatorPtrNC operatorEqNC = bslstl::operator==;
        operatorPtrNC operatorNeNC = bslstl::operator!=;
        (void) operatorEqNC;  // quash potential compiler warnings
        (void) operatorNeNC;

        operatorPtrCN operatorEqCN = bslstl::operator==;
        operatorPtrCN operatorNeCN = bslstl::operator!=;
        (void) operatorEqCN;  // quash potential compiler warnings
        (void) operatorNeCN;

        operatorPtrCC operatorEqCC = bslstl::operator==;
        operatorPtrCC operatorNeCC = bslstl::operator!=;
        (void) operatorEqCC;  // quash potential compiler warnings
        (void) operatorNeCC;
    }

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    //Compare every value with every value.

    for (int ti = 0; ti < N; ++ti) {

        Obj  mX1(nodes[ti]); const Obj&  X1 = mX1;
        ObjC mX2(nodes[ti]); const ObjC& X2 = mX2;

        // Ensure an object compares correctly with itself (alias test).
        ASSERTV(  X1 == X1);
        ASSERTV(!(X1 != X1));

        ASSERTV(  X2 == X2);
        ASSERTV(!(X2 != X2));

        ASSERTV(  X1 == X2);
        ASSERTV(!(X1 != X2));

        ASSERTV(  X2 == X1);
        ASSERTV(!(X2 != X1));

        for (int tj = 0; tj < N; ++tj) {

            bool EXP = ti == tj;

            Obj  mY1(nodes[tj]); const Obj&  Y1 = mY1;
            ObjC mY2(nodes[tj]); const ObjC& Y2 = mY2;

            if (veryVerbose) {
                T_ T_ P_(EXP) P_(ti) P(tj) }

            // Verify value, commutativity

            ASSERTV(EXP == (X1 == Y1));
            ASSERTV(EXP == (Y1 == X1));
            ASSERTV(EXP == (X1 == Y2));
            ASSERTV(EXP == (Y2 == X1));
            ASSERTV(EXP == (X2 == Y1));
            ASSERTV(EXP == (Y1 == X2));
            ASSERTV(EXP == (X2 == Y2));
            ASSERTV(EXP == (Y2 == X2));

            ASSERTV(!EXP == (X1 != Y1));
            ASSERTV(!EXP == (Y1 != X1));
            ASSERTV(!EXP == (X1 != Y2));
            ASSERTV(!EXP == (Y2 != X1));
            ASSERTV(!EXP == (X2 != Y1));
            ASSERTV(!EXP == (Y1 != X2));
            ASSERTV(!EXP == (X2 != Y2));
            ASSERTV(!EXP == (Y2 != X2));
        }
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS, 'operator*', and 'operator->'
    //   Ensure that the basic accessor 'node' as well as 'operator*' and
    //   'operator->' properly interprets object state.
    //
    // Concerns:
    //: 1 The 'node' method returns the address of the node to which this
    //:   object refers.
    //:
    //: 2 The 'operator*' returns the reference to the value of the node
    //:   to which this object refers.
    //:
    //: 3 The 'operator->' returns the address to the value of the node to
    //:   which this object refers.
    //:
    //: 4 Each of the three methods are declared 'const'.
    //:
    //: 5 The signature and return type are standard.
    //
    // Plan:
    //: 1 Use the addresses of 'operator*' and 'operator->' to initialize
    //:   member-function pointers having the appropriate signatures and
    //:   return types for the operators defined in this component.  (C-5)
    //:
    //: 2 Create a list with N nodes.
    //:
    //: 3 Use the primary manipulators to create an object, 'mX', and use it to
    //:   iterate through the nodes.  For each iteration do the following:
    //:   (C-1..4)
    //:
    //:   1 Create a const reference to the object 'X'.
    //:
    //:   2 Invoke the method 'node' on 'X' and verify that it returns the
    //:     expected value.  (C-1,4)
    //:
    //:   3 Invoke 'operator*' on 'X' and verify that it returns the
    //:     expected value.  (C-2,4)
    //:
    //:   4 Invoke 'operator->' on 'X' and verify that it returns the
    //:     expected value.  (C-3,4)
    //
    // Testing:
    //   reference operator*() const;
    //   pointer operator->() const;
    //   const bslalg::BidirectionalLink *node() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nBASIC ACCESSORS, 'operator*', and 'operator->'"
               "\n==============================================\n");

    // Assign the address of the of the methods to variables.
    {
        typedef VALUE& (Obj::*operatorPtr)() const;

        // Verify that the signature and return type are standard.
        operatorPtr operatorDereference = &Obj::operator*;
        (void) operatorDereference;  // quash potential compiler warning
    }
    {
        typedef VALUE* (Obj::*operatorPtr)() const;

        // Verify that the signature and return type are standard.
        operatorPtr operatorArrow = &Obj::operator->;
        (void) operatorArrow;  // quash potential compiler warning
    }

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    // Test Accessors
    Obj mX(nodes[0]);
    Obj end;

    for (int ti = 0; mX != end; ++mX, ++ti) {
        const Obj& X = mX;
        ASSERTV(ti, X.node() == nodes[ti]);
        ASSERTV(ti, *X == nodes[ti]->value());
        ASSERTV(ti, X.operator->() == bsls::Util::addressOf(nodes[ti]->value()));
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

template <class VALUE>
void TestDriver<VALUE>::testCase2()
{
    // ------------------------------------------------------------------------
    // DEFAULT CTOR & PRIMARY MANIPULATORS
    //   Ensure that we can use the default constructor to create an object
    //   (having the default constructed value).  Also ensure that we can
    //   use the primary manipulators to put that object into any state
    //   relevant for thorough testing.
    //
    // Concerns:
    //:  1 An object can be created with the default constructor.  Two
    //:    objects created with the default constructor has the same value.
    //:
    //:  2 An object created with the value constructor have the
    //:    contractually specified value.
    //:
    //:  3 An object initialized using a value constructor to a valid
    //:    'BidirectionalLink' can be post-incremented using the (as yet
    //:    unproven) 'operator++'.
    //:
    //:  4 An object can be used to traverse the all nodes in a list in
    //:    order using the value constructor and the (as yet unproven)
    //:    'operator++'.
    //
    // Plan:
    //:  1 Construct two objects using the default constructor.  Verify
    //:    that the two objects refers to the same node by using the (as
    //:    yet unproven) salient attribute accessor.  (C-1)
    //:
    //:  2 Create a linked list with N nodes.
    //:
    //:  3 For each node in the list, use the value constructor to create
    //:    an object to point to that node.  Verify the state of the object
    //:    with the (as yet unproven) salient attribute accessor.  (C-2)
    //:
    //:  4 Instantiate an object with the value constructor, passing in a
    //:    pointer to the first node of the list.  Iterate over the nodes of
    //:    the list with the pre-increment.  After construction and each
    //:    iteration, verify the state of the object with the (as yet unproven)
    //:    salient attribute accessor.  (C-2..4)
    //
    // Testing:
    //   HashTableIterator();
    //   explicit HashTableIterator(bslalg::BidirectionalLink *node);
    //   HashTableIterator& operator++();
    //   ~HashTableIterator();
    // ------------------------------------------------------------------------

    if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                        "\n===================================\n");

    // Test the default constructor.
    Obj D1, D2;
    ASSERTV(D1.node(), D2.node(), D1.node() == D2.node());

    // Create a linked list with N nodes.
    const int N = 10;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Pool pool(&scratch);

    Node *nodes[N];

    nodes[0] = pool.createNode(0);
    nodes[0]->setNextLink(0);
    nodes[0]->setPreviousLink(0);
    for (int ti = 1; ti < N; ++ti) {
        nodes[ti] = pool.createNode(ti);
        Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
    }

    // Test value-constructing object pointing to every node.
    for (int ti = 1; ti < N; ++ti) {
        const Obj X(nodes[ti]);
        ASSERTV(X.node(), nodes[ti], X.node() == nodes[ti]);
    }

    // Test pre-increment operator.
    Obj mX(nodes[0]);
    Obj end;

    for (int ti = 0; mX != end; ++mX, ++ti) {
        const Obj& X = mX;
        ASSERTV(X.node(), nodes[ti], X.node() == nodes[ti]);
    }

    // Clean up the nodes.
    for (int i = 0; i < N; ++i) {
        pool.deleteNode(nodes[i]);
    }
}

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

void usageExample()
{

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Iterating a Hash Table Using 'HashTableIterator'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a simple hashtable and then use a
// 'HashTableIterator' to iterate through its elements.
//
// First, we define a typedef, 'Node', prepresenting a bidirectional node
// holding an integer value:
//..
    typedef bslalg::BidirectionalNode<int> Node;
//..
// Then, we construct a test allocator, and we use it to allocate an array of
// 'Node' objects, each holding a unique integer value:
//..
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    const int NUM_NODES = 5;
    const int NUM_BUCKETS = 3;

    Node *nodes[NUM_NODES];
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = static_cast<Node *>(scratch.allocate(sizeof(Node)));
        nodes[i]->value() = i;
    }
//..
// Next, we create an array of 'HashTableBuckets' objects, and we use the array
// to construct an empty hash table characterized by a 'HashTableAnchor'
// object:
//..
    bslalg::HashTableBucket buckets[NUM_BUCKETS];
    for (int i = 0; i < NUM_BUCKETS; ++i) {
        buckets[i].reset();
    }
    bslalg::HashTableAnchor hashTable(buckets, NUM_BUCKETS, 0);
//..
// Then, we insert each node in the array of nodes into the hash table using
// 'bslalg::HashTableImpUtil', supplying the integer value held by each node as
// its hash value:
//..
    for (int i = 0; i < NUM_NODES; ++i) {
        bslalg::HashTableImpUtil::insertAtFrontOfBucket(&hashTable,
                                                        nodes[i],
                                                        nodes[i]->value());
    }
//..
// Next, we define a 'typedef' that is an alias an instance of
// 'HashTableIterator' that can traverse hash tables holding integer values.
//..
    typedef bslstl::HashTableIterator<int, ptrdiff_t> Iter;
//..
// Now, we create two iterators: one pointing to the start of the bidirectional
// linked list held by the hash table, and the other representing the end
// sentinel.  We use the iterators to navigate and print the elements of the
// hash table:
//..
    Iter iter(hashTable.listRootAddress());
    Iter end;
    for (;iter != end; ++iter) {
        printf("%d\n", *iter);
    }
//..
// Then, we observe the following output:
//..
// 2
// 4
// 1
// 3
// 0
//..
// Finally, we deallocate the memory used by the hash table:
//..
    for (int i = 0; i < NUM_NODES; ++i) {
        scratch.deallocate(nodes[i]);
    }
//..
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No memory is ever allocated from the global allocator.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 14: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUsage Example"
                            "\n=============\n");

        usageExample();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // POST-INCREMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PRE-INCREMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS, 'operator*', and 'operator->'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      }
      case 3: {
        // --------------------------------------------------------------------
        // TESTING (PRIMITIVE) GENERATORS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR & PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
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

        {
            const int N = 10;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            NodePool<int> pool(&scratch);

            bslalg::BidirectionalNode<int> *nodes[N];

            nodes[0] = pool.createNode(0);
            nodes[0]->setNextLink(0);
            nodes[0]->setPreviousLink(0);

            for (int ti = 1; ti < N; ++ti) {
                nodes[ti] = pool.createNode(ti);
                Util::insertLinkAfterTarget(nodes[ti], nodes[ti - 1]);
            }

            typedef bslstl::HashTableIterator<int, ptrdiff_t> Iter;
            int i = 0;

            for (Iter it(nodes[0]); it.node(); ++it, ++i) {
                ASSERT(i == *it);
            }
            ASSERT(10 == i);

            for (int i = 0; i < N; ++i) {
                pool.deleteNode(nodes[i]);
            }
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated.

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
