// bslalg_hashtableimputil.t.cpp                                      -*-C++-*-

#include <bslalg_hashtableimputil.h>

#include <bslalg_hashtablebucket.h>
#include <bslalg_bidirectionalnode.h>
#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_scalarprimitives.h>
#include <bslalg_scalardestructionprimitives.h>

#include <bslma_testallocatormonitor.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <cstddef>
#include <climits>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using namespace BloombergLP::bslalg;
using namespace std;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [  ] const KeyType& extractKey(const BidirectionalLink *link);
// [  ] typename ValueType& extractValue(BidirectionalLink *link);
// [  ] isWellFormed(const HashTableAnchor& anchor);
// [  ] computeBucketIndex(size_t hashCode, size_t numBuckets);
// [  ] insertAtFrontOfBucket(Anchor *a, BidirectionalLink *l, size_t h);
// [  ] insertAtPosition(Anchor *a, Link *l, size_t h, Link  *p);
// [  ] remove(HashTableAnchor *a, BidirectionalLink *l, size_t  h);
// [  ] find(const HashTableAnchor& a, KeyType& key, comparator, size_t h);
// [  ] rehash(  HashTableAnchor *a, BidirectionalLink *r, const HASHER& h);
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

template<class KEY>
struct TestSetKeyPolicy
{
    typedef KEY KeyType;
    typedef KEY ValueType;

    static const KeyType& extractKey(const ValueType& value) {
        return value;
    }

    static const ValueType& extractValue(const ValueType& value) {
        return value;
    }
};

template <class VALUE>
struct NodeUtil {
    typedef BidirectionalNode<VALUE> Node;
    typedef VALUE                    ValueType;

    static
    BidirectionalLink *create(const VALUE&      value,
                              bslma::Allocator *basicAllocator)
    {
        BSLS_ASSERT(basicAllocator);

        Node *node = static_cast<Node *>(
                                       basicAllocator->allocate(sizeof(Node)));
        bslalg::ScalarPrimitives::construct(&node->value(),
                                            VALUE(value),
                                            basicAllocator);
        node->reset();
        return node;
    }

    static
    void destroy(BidirectionalLink *link, bslma::Allocator *basicAllocator)
    {
        BSLS_ASSERT(basicAllocator);
        BSLS_ASSERT(link);

        Node *node = static_cast<Node *>(link);

        bslalg::ScalarDestructionPrimitives::destroy(&node->value());
        basicAllocator->deallocate(node);
    }

    static
    void disposeList(BidirectionalLink *head, bslma::Allocator *basicAllocator)
    {
        if (!head) {
            return;                                                   // RETURN
        }
        while(head) {
            BidirectionalLink *next = head->nextLink();
            NodeUtil<VALUE>::destroy(head, basicAllocator);
            head = next;
        }
    }
};

struct IntTestHasherIdent {

    size_t operator()(int value) const
    {
        return value;
    }
};

struct IntTestHasherHalf {

    size_t operator()(int value) const
    {
        return value / 2;
    }
};

void debugPrint(const HashTableAnchor& anchor)
{
    printf("Debug Print Anchor:\n");
    if (0 == anchor.bucketArraySize()) {
        printf("<empty>\n");
    }
    else {
        for (size_t n = 0; n < anchor.bucketArraySize(); ++n) {
            printf("\nBucket [%d]: ", n);
            const HashTableBucket& bucket = anchor.bucketArrayAddress()[n];
            if (!bucket.first()) {
                continue;
            }
            for (const BidirectionalLink *c = bucket.first();
                 bucket.last()->nextLink() != c;
                 c = c->nextLink()) {
                const int v =
                       static_cast<const BidirectionalNode<int> *>(c)->value();
                printf("[%d], ", v);
            }
            printf("\n");
        }
         printf("\n");
    }
    fflush(stdout);
}


typedef HashTableBucket           Bucket;
typedef HashTableAnchor           Anchor;
typedef HashTableImpUtil          Obj;
typedef BidirectionalLink         Link;
typedef BidirectionalLinkListUtil Util;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
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

        typedef BidirectionalNode<int> Node;
        typedef TestSetKeyPolicy<int>  TestPolicy;
        typedef NodeUtil<int>          IntNodeUtil;

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dm(&da);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor om(&oa);


        // [  ] const KeyType& extractKey(const BidirectionalLink *link);
        if (veryVerbose) printf("\t\t Testing 'extractKey'\n");
        {
            const int DATA[] = { INT_MIN, -2, 1, 3, INT_MAX };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);
            for(size_t i = 0; i < DATA_SIZE; ++i) {
                Link *n = IntNodeUtil::create(DATA[i], &oa);
                ASSERTV(Obj::extractKey<TestPolicy>(n) == DATA[i]);
                IntNodeUtil::destroy(n, &oa);
            }
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());

        // [  ] typename ValueType& extractValue(BidirectionalLink *link);
        if (veryVerbose) printf("\t\t Testing 'extractValue'\n");
        {
            const int DATA[] = { INT_MIN, -2, 1, 3, INT_MAX };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);
            for(size_t i = 0; i < DATA_SIZE; ++i) {
                Link *n = IntNodeUtil::create(DATA[i], &oa);
                ASSERTV(Obj::extractValue<TestPolicy>(n) == DATA[i]);
                IntNodeUtil::destroy(n, &oa);
            }
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());

        // [  ] computeBucketIndex(size_t hashCode, size_t numBuckets);

        if (veryVerbose) printf("\t\t Testing 'computeBucketIndex'\n");
        {
            const struct {
               size_t d_hash;
               size_t d_numBuckets;
            } DATA[] = {   //HASH   //NUM BUCKETS
                         {      0,              1},
                         {      0,     1234567890},
                         {      1,              1},
                         {     10,              1},
                         {      1,             10},
                         {      2,              2},
                         {      4,              2},
                         {      4,              7},
            };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);
            for(size_t i = 0; i < DATA_SIZE; ++i) {
                const size_t HASH        = DATA[i].d_hash;
                const size_t NUM_BUCKETS = DATA[i].d_numBuckets;
                const size_t EXP  = HASH % NUM_BUCKETS;
                ASSERTV(EXP == Obj::computeBucketIndex(HASH, NUM_BUCKETS));
            }
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());

// [  ] isWellFormed(const Anchor *anchor);
        if (veryVerbose) printf("\t\t Testing 'isWellFormed'\n");
        {
            if(veryVeryVerbose) printf("\t\t\t Testing malformed anchor 1\n");
            Bucket badArray[7];
            memset(badArray, 0, sizeof(badArray));

            const int DATA[] = { 0, 1, 2, 3, 4, 5, 6 };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);

            Link *head = IntNodeUtil::create(DATA[0], &oa);
            Link *tail = head;

            for(size_t i = 1; i < DATA_SIZE; ++i) {
                if (veryVeryVeryVerbose) {
                    T_ P_(i) P(DATA[i]);
                }
                Link *n = IntNodeUtil::create(DATA[i], &oa);
                ASSERTV(Obj::extractValue<TestPolicy>(n) == DATA[i]);
                Util::insertLinkAfterTarget(n, tail);
                tail = n;
            }

            Link *cursor = head;
            size_t listLength = 0;
            while (cursor) {
                ++listLength;
                cursor = cursor->nextLink();
            }
            ASSERTV(DATA_SIZE == listLength);

            Anchor anchor(badArray, DATA_SIZE, head);
            const Anchor& ANCHOR = anchor;
            const bool IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);

            ASSERTV(!IS_VALID);
            IntNodeUtil::disposeList(head, &oa);
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());
        {
            if(veryVeryVerbose) printf("\t\t\t Testing well formed  "
                                       "empty anchor\n");

            const int DATA[] = { 0, 1, 2, 3, 4, 5, 6 };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);

            Bucket goodArray[DATA_SIZE];
            memset(goodArray, 0, sizeof(goodArray));

            Anchor anchor(goodArray, DATA_SIZE, 0);
            const Anchor& ANCHOR = anchor;
            const bool IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);

            ASSERTV(IS_VALID);
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());
        {
            if(veryVeryVerbose) printf("\t\t\t Testing non-empty anchor\n");
            const int DATA[] = { 0, 1, 2 };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);

            Bucket goodArray[DATA_SIZE];
            memset(goodArray, 0, sizeof(goodArray));


            Link *head = IntNodeUtil::create(DATA[0], &oa);
            Link *tail = head;

            for(size_t i = 1; i < DATA_SIZE; ++i) {
                if (veryVeryVeryVerbose) {
                    T_ P_(i) P(DATA[i]);
                }
                Link *n = IntNodeUtil::create(DATA[i], &oa);
                ASSERTV(Obj::extractValue<TestPolicy>(n) == DATA[i]);
                Util::insertLinkAfterTarget(n, tail);
                tail = n;
            }

            goodArray[0].setFirstAndLast(head, head);
            goodArray[1].setFirstAndLast(head->nextLink(), head->nextLink());
            goodArray[2].setFirstAndLast(head->nextLink()->nextLink(),
                                                 head->nextLink()->nextLink());
            Link *cursor = head;
            size_t listLength = 0;
            while (cursor) {
                ++listLength;
                cursor = cursor->nextLink();
            }
            ASSERTV(DATA_SIZE == listLength);

            Anchor anchor(goodArray, DATA_SIZE, head);
            const Anchor& ANCHOR = anchor;
            const bool IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);

            ASSERTV(IS_VALID);
            IntNodeUtil::disposeList(head, &oa);
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());

// [  ] insertAtFrontOfBucket(Anchor *a, BidirectionalLink *l, size_t h);
        if(veryVerbose) printf("\t\t Testing 'insertAtFrontOfBucket'\n");
        {
            if(veryVeryVerbose) printf(
                                    "\t\t\t Test 'insertAtFrontOfBucket' 0\n");

            const int DATA[] = { 0, 1, 2 };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);

            Bucket goodArray[DATA_SIZE];
            memset(goodArray, 0, sizeof(goodArray));

            // Emtpy Hash Table
            //..
            //              Root
            // [0F]->x       ~
            // [0L]->x
            // [1F]->x
            // [1L]->x
            // [2F]->x
            // [2L]->x
            //..

            Anchor anchor(goodArray, DATA_SIZE, 0);
            const Anchor& ANCHOR = anchor;

            Link *link = IntNodeUtil::create(DATA[0], &oa);

            Obj::insertAtFrontOfBucket(&anchor, link, 0);
            ASSERTV(anchor.listRootAddress() == link);
            ASSERTV(anchor.bucketArrayAddress()[0].first() == link);
            ASSERTV(anchor.bucketArrayAddress()[0].last() == link);
            bool IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After insert 0
            //..
            //                x
            //                |
            // [0F]--------> +-+  root
            //               |0|
            // [0L]--------> +-+
            //                |
            //                x
            // [1F]->x
            // [1L]->x
            // [2F]->x
            // [2L]->x
            //..

            if(veryVeryVerbose) printf("\t\t\t Test 'remove' 0\n");
            Obj::remove(&anchor, link, 0);
            ASSERTV(anchor.listRootAddress() == 0);
            ASSERTV(anchor.bucketArrayAddress()[0].first() == 0);
            ASSERTV(anchor.bucketArrayAddress()[0].last() == 0);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After remove 0
            //..
            //              Root
            // [0F]->x       ~
            // [0L]->x
            // [1F]->x
            // [1L]->x
            // [2F]->x
            // [2L]->x
            //..

            if(veryVeryVerbose) printf(
                                    "\t\t\t Test 'insertAtFrontOfBucket' 0\n");
            Obj::insertAtFrontOfBucket(&anchor, link, 0);
            ASSERTV(anchor.listRootAddress() == link);
            ASSERTV(anchor.bucketArrayAddress()[0].first() == link);
            ASSERTV(anchor.bucketArrayAddress()[0].last() == link);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After insert 0
            //..
            //                x
            //                |
            // [0F]--------> +-+  root
            //               |0|
            // [0L]--------> +-+
            //                |
            //                x
            // [1F]->x
            // [1L]->x
            // [2F]->x
            // [2L]->x
            //..

            if(veryVeryVerbose) printf(
                                    "\t\t\t 'Test insertAtFrontOfBucket' 1\n");
            Link *link2 = IntNodeUtil::create(DATA[1], &oa);

            Obj::insertAtFrontOfBucket(&anchor, link2, 1);
            ASSERTV(anchor.listRootAddress() == link2);
            ASSERTV(anchor.listRootAddress()->nextLink() == link);
            ASSERTV(anchor.bucketArrayAddress()[1].first() == link2);
            ASSERTV(anchor.bucketArrayAddress()[1].last() == link2);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After insert 1
            //..
            //                x
            //                |   root
            // [0F]-----+    +-+
            //          |    |1| <---+
            // [0L]--+  |    +-+ <-+ |
            //       |  |      |   | |
            //       |  +--->+-+   | |
            //       +------>|0|   | |
            //               +-+   | |
            //                |    | |
            //                x    | |
            //                     | |
            // [1F]----------------+ |
            // [1L]------------------+
            // [2F]->x
            // [2L]->x
            //..

            if(veryVeryVerbose) printf("\t\t\t Test insert again 1\n");
            Link *link3 = IntNodeUtil::create(DATA[1], &oa);

            Obj::insertAtFrontOfBucket(&anchor, link3, 1);
            ASSERTV(anchor.listRootAddress() == link3);
            ASSERTV(anchor.listRootAddress()->nextLink() == link2);
            ASSERTV(anchor.bucketArrayAddress()[1].first() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].last() == link2);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After insert again 1
            //..
            //                x
            //                |   root
            // [0F]-----+    +-+
            //          |    |1|
            // [0L]--+  |    +-+ <-+
            //       |  |     |    |
            //       |  |    +-+   |
            //       |  |    |1| <-+-+
            //       |  |    +-+   | |
            //       |  |     |    | |
            //       |  +--->+-+   | |
            //       +------>|0|   | |
            //               +-+   | |
            //                |    | |
            //                x    | |
            //                     | |
            // [1F]----------------+ |
            // [1L]------------------+
            // [2F]->x
            // [2L]->x
            //..

            if(veryVeryVerbose) printf("\t\t\t Test remove 1\n");
            Obj::remove(&anchor, link2, 1);
            ASSERTV(anchor.listRootAddress() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].first() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].last() == link3);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);
            IntNodeUtil::destroy(link2, &oa);

            // After remove again 1
            //..
            //                x
            //                |   root
            // [0F]-----+    +-+
            //          |    |1| <---+
            // [0L]--+  |    +-+ <-+ |
            //       |  |      |   | |
            //       |  +--->+-+   | |
            //       +------>|0|   | |
            //               +-+   | |
            //                |    | |
            //                x    | |
            //                     | |
            // [1F]----------------+ |
            // [1L]------------------+
            // [2F]->x
            // [2L]->x
            //..
            if (veryVeryVerbose) printf("\tTest 'insertAtPosition' 1\n");

            Link *link4 = IntNodeUtil::create(DATA[1], &oa);

            Obj::insertAtPosition(&anchor, link4, 1, link3);
            ASSERTV(anchor.listRootAddress() == link4);
            ASSERTV(anchor.listRootAddress()->nextLink() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].first() == link4);
            ASSERTV(anchor.bucketArrayAddress()[1].last() == link3);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After insert at position 1
            //..
            //                x
            //                |   root
            // [0F]-----+    +-+
            //          |    |1|
            // [0L]--+  |    +-+ <-+
            //       |  |     |    |
            //       |  |    +-+   |
            //       |  |    |1| <-+-+
            //       |  |    +-+   | |
            //       |  |     |    | |
            //       |  +--->+-+   | |
            //       +------>|0|   | |
            //               +-+   | |
            //                |    | |
            //                x    | |
            //                     | |
            // [1F]----------------+ |
            // [1L]------------------+
            // [2F]->x
            // [2L]->x
            //..

            if(veryVeryVerbose) printf("\t\t\t Test remove 1\n");
            Obj::remove(&anchor, link4, 1);
            ASSERTV(anchor.listRootAddress() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].first() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].last() == link3);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            // After remove again 1
            //..
            //                x
            //                |   root
            // [0F]-----+    +-+
            //          |    |1| <---+
            // [0L]--+  |    +-+ <-+ |
            //       |  |     |    | |
            //       |  |    +-+   | |
            //       |  +--->|0|   | |
            //       |       +-+   | |
            //       |        |    | |
            //       |       +-+   | |
            //       +------>|0|   | |
            //               +-+   | |
            //                |    | |
            //                x    | |
            //                     | |
            // [1F]----------------+ |
            // [1L]------------------+
            // [2F]->x
            // [2L]->x
            //..
            IntNodeUtil::destroy(link4, &oa);

            if (veryVeryVerbose) printf("\tTest 'insertAtPosition' 0\n");

            Link *link5 = IntNodeUtil::create(DATA[0], &oa);

            Obj::insertAtPosition(&anchor, link5, 0, link);
            ASSERTV(anchor.listRootAddress() == link3);
            ASSERTV(anchor.listRootAddress()->nextLink() == link5);
            ASSERTV(anchor.bucketArrayAddress()[0].first() == link5);
            ASSERTV(anchor.bucketArrayAddress()[0].last() == link);
            ASSERTV(anchor.bucketArrayAddress()[1].first() == link3);
            ASSERTV(anchor.bucketArrayAddress()[1].last() == link3);
            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);
            IntNodeUtil::disposeList(link3, &oa);
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());

// [  ] find(const Anchor& a, KeyType& key, comparator, size_t h);
// [  ] rehash(Anchor *a, BidirectionalLink *r, const HASHER& h);

        if(veryVerbose) printf("\tTest 'rehash'\n");
        {

            const int DATA[] = { 0, 1, 2, 3, 4, 5, 6 };
            const size_t DATA_SIZE = sizeof(DATA) / sizeof(*DATA);

            Bucket goodArray[1];
            memset(goodArray, 0, sizeof(goodArray));

            // Emtpy Hash Table
            //..
            //              Root
            // [0F]->x       ~
            // [0L]->x
            //..

            Anchor anchor(goodArray, 1, 0);
            const Anchor& ANCHOR = anchor;

            // Assume hash is identity.

            Link *link0 = IntNodeUtil::create(DATA[0], &oa);
            Link *link1 = IntNodeUtil::create(DATA[1], &oa);
            Link *link2 = IntNodeUtil::create(DATA[2], &oa);
            Link *link3 = IntNodeUtil::create(DATA[3], &oa);
            Link *link4 = IntNodeUtil::create(DATA[4], &oa);
            Link *link5 = IntNodeUtil::create(DATA[5], &oa);
            Link *link6 = IntNodeUtil::create(DATA[6], &oa);

            bool IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            Obj::insertAtFrontOfBucket(&anchor, link0, DATA[0]);
            Obj::insertAtFrontOfBucket(&anchor, link1, DATA[1]);
            Obj::insertAtFrontOfBucket(&anchor, link2, DATA[2]);
            Obj::insertAtFrontOfBucket(&anchor, link3, DATA[3]);
            Obj::insertAtFrontOfBucket(&anchor, link4, DATA[4]);
            Obj::insertAtFrontOfBucket(&anchor, link5, DATA[5]);
            Obj::insertAtFrontOfBucket(&anchor, link6, DATA[6]);

            //..
            //                x
            //                |   root
            //               +-+
            // [0F]--------->|6|
            // [0L]--+       +-+
            //       |        |
            //       |       +-+
            //       |       |5|
            //       |       +-+
            //       |        |
            //       |       +-+
            //       |       |4|
            //       |       +-+
            //       |        |
            //       |       +-+
            //       |       |3|
            //       |       +-+
            //       |        |
            //       |       +-+
            //       |       |2|
            //       |       +-+
            //       |        |
            //       |       +-+
            //       |       |1|
            //       |       +-+
            //       |        |
            //       |       +-+
            //       +------>|0|
            //               +-+
            //                |
            //                x
            //..

            IS_VALID =
                     Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(ANCHOR);
            ASSERTV(IS_VALID);

            Bucket newArray[DATA_SIZE];
            memset(newArray, 0, sizeof(newArray));
            Anchor newAnchor(newArray, DATA_SIZE, 0);

            Obj::rehash<TestPolicy, IntTestHasherIdent>(
                                                      &newAnchor,
                                                      anchor.listRootAddress(),
                                                      IntTestHasherIdent());
            for (size_t i = 0; i < DATA_SIZE; ++i) {
                ASSERTV(i, newArray[i].first() == newArray[i].last());
            }

            ASSERTV(newArray[0].first() == link0);
            ASSERTV(newArray[1].first() == link1);
            ASSERTV(newArray[2].first() == link2);
            ASSERTV(newArray[3].first() == link3);
            ASSERTV(newArray[4].first() == link4);
            ASSERTV(newArray[5].first() == link5);
            ASSERTV(newArray[6].first() == link6);

            IS_VALID =
                  Obj::isWellFormed<TestPolicy, IntTestHasherIdent>(newAnchor);
            ASSERTV(IS_VALID);
            IntNodeUtil::disposeList(newAnchor.listRootAddress(), &oa);
        }
        ASSERTV(om.isInUseSame());
        ASSERTV(dm.isInUseSame());
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
