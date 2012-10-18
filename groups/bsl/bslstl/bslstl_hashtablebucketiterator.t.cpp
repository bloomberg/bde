// bslstl_hashtablebucketiterator.t.cpp                               -*-C++-*-
#include <bslstl_hashtablebucketiterator.h>

#include <bslstl_allocator.h>
#include <bslstl_bidirectionalnodepool.h>

#include <bsltf_templatetestfacility.h>

#include <bslalg_bidirectionallinklistutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

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
// 'bslstl::HashTableBucketIterator', that is an in-core value semantic type.
// This class provides a standard-conforming forward iterator (see section
// 24.2.5 [forward.iterators] of the C++11 standard) over a list of
// 'bslalg::BidirectionalLink' objects.
//
// The primary manipulators of this class is the value constructor, which is
// sufficient to allow an object to attain any achievable state.  The basic
// accessor of the class is the 'bucket' method and the 'node' method, which
// returns the address of the node ('bslalg::BidirectionalNode' object) and
// address of the bucket ('bslalg::HashTableBucket' object) referred to by a
// iterator.
//
// Primary Manipulator:
//: o explicit HashTableBucketIterator(const bslalg::HashTableBucket *bucket,
//                                     bslalg::BidirectionalLink *node);
//
// Basic Accessors:
//: o bslalg::BidirectionalLink *node() const;
//: o const bslalg::HashTableBucket *bucket() const;
//
// Since the state of an object will be meaningful if the object refers to a
// valid node in a hash table bucket.  The facilities provided by
// 'bslalg::BidirectionalLinkListUtil' will be used to create a list of nodes
// to be referred to by objects of this class.  This particular class provides
// a value constructor, which is also the primary manipulator, that is capable
// of creating an object in any state, obviating the primitive generator
// function, 'gg', which is normally used for this purpose.
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
// [ 2] HashTableBucketIterator();
// [ 2] explicit HashTableBucketIterator(bucket);
// [ 2] explicit HashTableBucketIterator(node, bucket);
// [ 7] HashTableBucketIterator(const NcIter& original);
// [ 7] HashTableBucketIterator(const HashTableBucketIterator& original)
// [ 2] ~HashTableBucketIterator() = default;
//
// MANIPULATORS
// [ 9] HashTableBucketIterator& operator=(const HashTableBucketIterator& rhs)
// [11] HashTableBucketIterator& operator++();
//
// ACCESSORS
// [ 4] reference operator*() const;
// [ 4] pointer operator->() const;
// [ 4] bslalg::BidirectionalLink *node() const;
// [ 4] const bslalg::HashTableBucket *bucket() const;
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
// [12] HashTableBucketIterator operator++(iter, int)
//
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

//=============================================================================
//             GLOBAL TYPEDEFS, FUNCTIONS AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

typedef bsltf::TemplateTestFacility Ttf;
typedef bslalg::BidirectionalLinkListUtil Util;

typedef bslstl::HashTableBucketIterator<int, ptrdiff_t > Obj;
typedef bslalg::BidirectionalLink                        Link;

struct NodePoolInt {
  private:
    // DATA
    bslstl::BidirectionalNodePool<int, bsl::allocator<int> > d_subPool;

  public:
    // CREATORS
    explicit
    NodePoolInt(bslma::Allocator *allocator)
    : d_subPool(allocator)
    {}

    // MANIPULATORS
    Link *createNode(int value, Link *prev, Link *next)
    {
        Link *result = d_subPool.createNode(value);
        result->setPreviousLink(prev);
        result->setNextLink(    next);

        return result;
    }
};

Link *makeSequence(NodePoolInt *pool, int from, const int to, int by)
{
    Link *result = 0, *prev = 0;
    for (int i = from; to != i; i += by) {
        Link *current = pool->createNode(i, prev, 0);
        if (0 == result) {
            result = current;
        }
        else {
            prev->setNextLink(current);
        }
        prev = current;
    }

    return result;
}

struct MyStruct {
    int d_data;

    // CREATORS
    MyStruct() {}
    explicit MyStruct(int data) : d_data(data) {}
};

typedef bslstl::HashTableBucketIterator<MyStruct, ptrdiff_t > PairObj;

struct NodePoolMyStruct {
  private:
    // DATA
    bslstl::BidirectionalNodePool<MyStruct, bsl::allocator<MyStruct> >
                                                                     d_subPool;

  public:
    // CREATORS
    explicit
    NodePoolMyStruct(bslma::Allocator *allocator)
    : d_subPool(allocator)
    {}

    // MANIPULATORS
    Link *createNode(int value, Link *prev, Link *next)
    {
        Link *result = d_subPool.createNode(value);
        result->setPreviousLink(prev);
        result->setNextLink(    next);

        return result;
    }
};

Link *makeSequence(NodePoolMyStruct *pool, int from, const int to, int by)
{
    Link *result = 0, *prev = 0;
    for (int i = from; to != i; i += by) {
        Link *current = pool->createNode(i, prev, 0);
        if (0 == result) {
            result = current;
        }
        else {
            prev->setNextLink(current);
        }
        prev = current;
    }

    return result;
}

namespace {

template <class VALUE>
class NodePool {

    // DATA
    bslstl::BidirectionalNodePool<VALUE, bsl::allocator<VALUE> > d_pool;

  public:
    // TYPES
    typedef bslalg::BidirectionalNode<VALUE> Node;

    // CREATORS
    NodePool(bslma::Allocator *basicAllocator)
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
class BucketsWrapper {

  public:
    // TYPES
    typedef bslalg::HashTableBucket Bucket;
    typedef bslalg::BidirectionalLink Link;
    typedef bslalg::BidirectionalNode<VALUE> Node;

  private:
    // DATA
    int d_numBuckets;
    int d_numNodesPerBucket;

    Bucket *d_buckets;
    NodePool<VALUE> d_nodePool;
    bslma::Allocator *d_allocator_p;

  public:

    // CREATORS
    BucketsWrapper(int numBuckets,
                   int numNodesPerBucket,
                   bslma::Allocator *basicAllocator)
    : d_numBuckets(numBuckets)
    , d_numNodesPerBucket(numNodesPerBucket)
    , d_nodePool(basicAllocator)
    , d_allocator_p(basicAllocator)
    {
        BSLS_ASSERT_OPT(numBuckets > 0);
        BSLS_ASSERT_OPT(numNodesPerBucket > 0);
        BSLS_ASSERT_OPT(basicAllocator);

        d_buckets = static_cast<Bucket *>(
                         d_allocator_p->allocate(sizeof(Bucket) * numBuckets));

        int nodeValue = 0;
        d_buckets[0].d_first_p = d_nodePool.createNode(nodeValue++);
        d_buckets[0].d_first_p->setNextLink(0);
        d_buckets[0].d_first_p->setPreviousLink(0);


        for (int b = 0; b < numBuckets; ++b) {
            if (b > 0) {
                d_buckets[b].d_first_p = d_nodePool.createNode(nodeValue++);
                Util::insertLinkAfterTarget(d_buckets[b].d_first_p,
                                            d_buckets[b - 1].d_last_p);
            }

            Node *prevNode = static_cast<Node *>(d_buckets[b].d_first_p);
            for (int n = 1; n < numNodesPerBucket; ++ n) {

                Node *node = d_nodePool.createNode(nodeValue++);
                Util::insertLinkAfterTarget(node, prevNode);
                prevNode = node;
            }

            d_buckets[b].d_last_p = prevNode;
        }
    }

    ~BucketsWrapper()
    {
        Link *link = d_buckets[0].d_first_p;
        while(link)
        {
            Link *next = link->nextLink();
            d_nodePool.deleteNode(static_cast<Node *>(link));
            link = next;
        }
        d_allocator_p->deallocate(d_buckets);
    }

    // ACCESSORS
    Bucket * const bucket(int index) const
    {
        return &d_buckets[index];
    }

    Node * const node(int bucketIndex, int pos) const
    {
        Bucket& bucket = d_buckets[bucketIndex];
        Link *link = bucket.d_first_p;
        for(int p = 0; p < pos; ++p) {
            link = link->nextLink();
        }
        return static_cast<Node *>(link);
    }

    int numBuckets() const
    {
        return d_numBuckets;
    }

    int numNodesPerBucket() const
    {
        return d_numNodesPerBucket;
    }
};

template <class VALUE>
class TestDriver {

  private:
    // TYPES
    typedef bslstl::HashTableBucketIterator<VALUE, ptrdiff_t> Obj;
    typedef bslalg::HashTableBucket Bucket;
    typedef BucketsWrapper<VALUE> Buckets;
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
    //: 2 The 'bucket' method returns the address of the bucket to which this
    //:   object refers.
    //:
    //: 3 The 'operator*' returns the reference to the value of the node
    //:   to which this object refers.
    //:
    //: 4 The 'operator->' returns the address to the value of the node to
    //:   which this object refers.
    //:
    //: 5 Each of the three methods are declared 'const'.
    //:
    //: 6 The signature and return type are standard.
    //
    // Plan:
    //: 1 Use the addresses of 'operator*' and 'operator->' to initialize
    //:   member-function pointers having the appropriate signatures and
    //:   return types for the operators defined in this component.  (C-6)
    //:
    //: 2 Create M buckets each containing N nodes.
    //:
    //: 3 For each bucket created in P-2:
    //:
    //:   1 Use the primary manipulators to create an object, 'mX', and use it
    //:     to iterate through the nodes in the bucket.  For each iteration do
    //:     the following: (C-1..5)
    //:
    //:     1 Create a const reference to the object 'X'.
    //:
    //:     2 Invoke the 'node' method on 'X' and verify that it returns the
    //:       expected value.  (C-1, 5)
    //:
    //:     3 Invoke the 'bucket' method on 'X' and verify that it returns the
    //:       expected value.  (C-2, 5)
    //:
    //:     4 Invoke 'operator*' on 'X' and verify that it returns the expected
    //:       value.  (C-3, 5)
    //:
    //:     5 Invoke 'operator->' on 'X' and verify that it returns the
    //:       expected value.  (C-4, 5)
    //
    // Testing:
    //   reference operator*() const;
    //   pointer operator->() const;
    //   const bslalg::BidirectionalLink *node() const;
    // ------------------------------------------------------------------------

    if (verbose)
        printf("\nBASIC ACCESSORS, 'operator*', and 'operator->'"
               "\n==============================================\n");

    {
        typedef VALUE& (Obj::*operatorPtr)() const;

        // Verify that the signature and return type are standard.
        operatorPtr operatorDereference = &Obj::operator*;
        (void)operatorDereference;  // quash potential compiler warning
    }
    {
        typedef VALUE* (Obj::*operatorPtr)() const;

        // Verify that the signature and return type are standard.
        operatorPtr operatorArrow = &Obj::operator->;
        (void)operatorArrow;  // quash potential compiler warning
    }


    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    const int M = 5, N = 6;
    Buckets buckets(M, N, &scratch);

    for (int b = 0; b < M; ++b) {
        Bucket *BUCKET = buckets.bucket(b);
        Obj mX(BUCKET); const Obj& X = mX;
        Obj end;
        for (int n = 0; mX != end; ++mX, ++n) {
            Node *NODE = buckets.node(b, n);
            ASSERT(BUCKET == X.bucket());
            ASSERT(NODE == X.node());
            ASSERT(NODE->value() == *X);
            ASSERT(&NODE->value() == X.operator->());
        }
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
    //:  1 An object can be created with the default constructor.  Two objects
    //:    created with the default constructor has the same value.
    //:
    //:  2 An object created with the value constructor have the contractually
    //:    specified value.
    //:
    //:  3 An object initialized using a value constructor to a valid
    //:    'BidirectionalLink' can be post-incremented using the (as yet
    //:    unproven) 'operator++'.
    //:
    //:  4 An object can be used to traverse the all nodes in a list in order
    //:    using the value constructor and the (as yet unproven) 'operator++'.
    //
    // Plan:
    //:  1 Construct two objects using the default constructor.  Verify that
    //:    the two objects refers to the same node by using the (as yet
    //:    unproven) salient attribute accessor.  (C-1)
    //:
    //:  2 Create M buckets each containing N nodes.
    //:
    //:  3 For each bucket created in P-2:
    //:
    //:    1 For each node in the bucket, use the value constructor to create
    //:      an object to point to that node.  Verify the state of the object
    //:      with the (as yet unproven) salient attribute accessor.  (C-2)
    //:
    //:    1 Instantiate an object with the value constructor, passing in a
    //:      pointer to the first node of the bucket.  Iterate over the nodes
    //:      of the bucket with the pre-increment.  After construction and each
    //:      iteration, verify the state of the object with the (as yet
    //:      unproven) salient attribute accessor.  (C-2..4)
    //
    // Testing:
    //   HashTableBucketIterator();
    //   explicit HashTableBucketIterator(bucket);
    //   explicit HashTableBucketIterator(node, bucket);
    //   ~HashTableBucketIterator() = default;
    //   HashTableBucketIterator& operator++();
    // ------------------------------------------------------------------------

    if (verbose) printf("\nDEFAULT CTOR & PRIMARY MANIPULATORS"
                        "\n===================================\n");

    // Test the default constructor.
    Obj D1, D2;
    ASSERTV(D1.node(), D2.node(), D1.node() == D2.node());

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    const int M = 5, N = 6;
    Buckets buckets(M, N, &scratch);

    // Test value-constructing object.
    for (int b = 0; b < M; ++b) {
        for (int n = 0; n < N; ++n) {
            Bucket *BUCKET = buckets.bucket(b);
            Node *NODE = buckets.node(b, n);
            const Obj X(NODE, BUCKET);

            ASSERT(BUCKET == X.bucket());
            ASSERT(NODE == X.node());
        }
    }

    for (int b = 0; b < M; ++b) {
        Bucket *BUCKET = buckets.bucket(b);
        Obj mX(BUCKET); const Obj& X = mX;
        ASSERT(BUCKET == X.bucket());
        ASSERT(BUCKET->d_first_p == X.node());

        Obj end;
        for (int n = 0; mX != end; ++mX, ++n) {
            Node *NODE = buckets.node(b, n);
            ASSERT(BUCKET == X.bucket());
            ASSERT(NODE == X.node());
        }
    }
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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

        bslma::TestAllocator ta;

        const Obj X;
        ASSERT(  X == X );
        ASSERT(!(X != X));

        {
            BucketsWrapper<int> bWrapper(3, 5, &ta);

            typedef bslstl::HashTableBucketIterator<int, ptrdiff_t> IntObj;

            int value = 0;
            for(int b = 0; b < bWrapper.numBuckets(); ++b) {

                IntObj mX(bWrapper.bucket(b)); const IntObj& X = mX;
                for (;X.node(); ++mX, ++value) {
                    ASSERT(*X == value);
                }
            }

            ASSERTV(value, 15 == value);
        }

        {
            NodePoolInt pool(&ta);

            Link *list = makeSequence(&pool, 0, 10, 1);

            // Create a bucket of 3 elements at the start of the list
            bslalg::HashTableBucket bucket = { list,
                                               list->nextLink()->nextLink() };

            const Obj bucketBegin(&bucket);
            ASSERT(bucketBegin == bucketBegin);
            // ASSERT(bucketBegin != X);  // This should be a test for an
                                          // assertion, comparing iterators to
                                          // different buckets.

            Obj cursor = bucketBegin;
            ASSERT(bucketBegin == cursor);

            *cursor = 1;

            const Obj bucketSecond(list->nextLink(), &bucket);
            ASSERT(bucketSecond != cursor);
            *bucketSecond = 2;

            // Check post-increment
            const Obj original = cursor++;
            ASSERT(bucketSecond == cursor);
            ASSERT(original == bucketBegin);
            ASSERT(2 == *cursor);

            // Check pre-increment
            *++cursor = 3;
            ASSERT(3 == *cursor);
        }

        // need to test a struct to validate operator->
        {
            NodePoolMyStruct pool(&ta);

            Link *list = makeSequence(&pool, 0, 10, 1);

            // Create a bucket of 3 elements at the start of the list
            bslalg::HashTableBucket bucket = { list,
                                               list->nextLink()->nextLink() };

            const PairObj bucketBegin(&bucket);
            PairObj cursor = bucketBegin;
            ASSERT(bucketBegin == cursor);

            cursor->d_data = 1;

            const PairObj bucketSecond(list->nextLink(), &bucket);
            ASSERT(bucketSecond != cursor);
            bucketSecond->d_data = 2;

            // Check post-increment
            const PairObj original = cursor++;
            ASSERT(bucketSecond == cursor);
            ASSERT(original == bucketBegin);
            ASSERT(2 == cursor->d_data);

            // Check pre-increment
            (++cursor)->d_data = 3;
            ASSERT(3 == cursor->d_data);
        }
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
