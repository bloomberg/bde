// bslalg_hashtableanchor.t.cpp                                       -*-C++-*-

#include <bslalg_hashtableanchor.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_bidirectionalnode.h>
#include <bslalg_hashtablebucket.h>

#include <bslalg_hastrait.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDescription'
//: o 'setDstInEffectFlag'
//: o 'setUtcOffsetInSeconds'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'description'
//: o 'dstInEffectFlag'
//: o 'utcOffsetInSeconds'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [10] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] HashTableAnchor(HashTableBucket *, size_t, BidirectionalLink *);
// [ 7] HashTableAnchor(const HashTableAnchor&)
// [ 2] ~HashTableAnchor();
//
// MANIPULATORS
// [ 9] operator=(const HashTableAnchor& rhs);
// [ 2] setBucketArrayAndSize(HashTableBucket *, size_t);
// [ 2] setListRootAddress(BidirectionalLink *);
// [ 8] void swap(HashTableAnchor& other);
//
// ACCESSORS
// [ 4] const HashTableBucket *bucketArrayAddress() const;
// [ 4] size_t bucketArraySize() const;
// [ 4] BidirectionalLink *listRootAddress() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const HashTableAnchor& lhs, rhs);
// [ 6] bool operator!=(const HashTableAnchor& lhs, rhs);
//
// FREE FUNCTIONS
// [ 8] void swap(HashTableAnchor& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [ 8] CONCERN: Precondition violations are detected when enabled.
// [10] Reserved for 'bslx' streaming.

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------
// size_t is an unsigned type
const size_t MY_SIZE_T_MAX = size_t(0)-1;
const size_t MY_SIZE_T_MIN = 1;  // actually, minimum valid array length

typedef bslalg::HashTableAnchor   Obj;
typedef bslalg::HashTableBucket   Bucket;
typedef bslalg::BidirectionalLink Link;
typedef std::size_t               size_t;

static Link DefaultLink1;
static Link DefaultLink2;

struct DefaultValueRow {
    int          d_line;  // source line number
    Bucket      *d_bucketArrayAddress;
    std::size_t  d_arraySize;
    Link        *d_listRootAddress;
};

static
const DefaultValueRow DEFAULT_VALUES[] =
{
    //LINE     BUCKETARRAYADDRESS  ARRAYSIZE LISTROOTADDRESS
    //----     ------------------  --------- ---------------

    // default (must be first)
    { L_,     (Bucket *) 0xd3adb33f, MY_SIZE_T_MIN, (Link *)    0 },

    // 'data'
    { L_,     (Bucket *) 0xd3adb33f, MY_SIZE_T_MIN, &DefaultLink1 },
    { L_,     (Bucket *) 0xf33db33f,             2, &DefaultLink1 },
    { L_,     (Bucket *) 0xd3adb33f,             3, &DefaultLink1 },
    { L_,     (Bucket *) 0xd3adb33f, MY_SIZE_T_MAX, &DefaultLink1 },
    { L_,     (Bucket *) 0xd3adb33f,             4, &DefaultLink2 },
    { L_,     (Bucket *) 0xd3adb33f,             5, (Link *)    0 },
};

const int DEFAULT_NUM_VALUES = sizeof DEFAULT_VALUES / sizeof *DEFAULT_VALUES;

void initializeGlobalData()
{
    // This function must be called from 'main' in order to perform any
    // runtime initialization of objects addressed in the default test tables.
    DefaultLink1.reset();
    DefaultLink2.reset();
}
//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implmenting a primitive hash table
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to create a hash table that keeps track of pointers.
// Pointers can be added ('insert'ed) or removed ('erase'd) from the table, and
// the table will keep track, at any time, of whether a pointer is currently
// stored in the table using the 'count' method.  It will also be table to
// return the total number of objects stored in the table (the 'size' method).
// Redundant 'insert's have no effect -- a given pointer may only be stored in
// the table once.
//
// First, we create our class:

class PtrHashSet : public bslalg::HashTableAnchor {
    // PRIVATE TYPES
    typedef bsls::Types::UintPtr              UintPtr;
    typedef bslalg::BidirectionalNode<void *> Node;
    typedef bslalg::HashTableBucket           Bucket;
    typedef bslalg::BidirectionalLinkListUtil Util;

    // DATA
    double            d_maxLoadFactor;
    unsigned          d_numNodes;
    bslma::Allocator *d_allocator_p;

    // PRIVATE MANIPULATORS
    void grow();
        // Roughly double the number of buckets, such that the number of
        // buckets shall always be '2^N - 1'.

    // PRIVATE ACCESSORS
    bool checkInvariants() const;
        // Perform sanity checks on this table, returning 'true' if all the
        // tests pass and 'false' otherwise.  Note that many of the checks
        // are done with the 'ASSERTV' macro and will cause messages to be
        // written to the console.

    bool find(Node **node, Bucket **bucket, const void *ptr) const;
        // If the specified value 'ptr' is stored in this table, return
        // pointers to its node and bucket in the specified 'node' and
        // 'bucket'.  If it is not in this table, return the bucket it should
        // be in, and a pointer to the first node, if any, in that bucket.  If
        // the bucket is empty, return with '*node == listRootAddress()'.
        // Return 'true' if 'ptr' was found in the table and 'false' otherwise.
        // Note that it is permissible to pass 0 to 'node' and / or 'bucket',
        // in which case these arguments are ignored.

  private:
    // NOT IMPLEMENTED
    PtrHashSet(const PtrHashSet&, bslma::Allocator *);
    PtrHashSet& operator=(const PtrHashSet&);

  public:
    // CREATORS
    explicit
    PtrHashSet(bslma::Allocator *allocator = 0);
        // Create a 'PtrHashSet', using the specified 'allocator'.  If no
        // allocator is specified, use the default allocator.

    ~PtrHashSet();
        // Destroy this 'PtrHashSet', freeing all its memory.

    // MANIPULATORS
    bool insert(void *ptr);
        // If the specfied 'ptr' is not in this hash table, add it, returning
        // 'true'.  If it is already in the table, return 'false' with no
        // action taken.

    bool erase(void *ptr);
        // If the specfied 'ptr' is in this hash table, remove it, returning
        // 'true'.  If it is not found in the table, return 'false' with no
        // action taken.

    // ACCESSORS
    native_std::size_t count(void *ptr) const;
        // Return 1 if the specified value 'ptr' is in this table and 0
        // otherwise.

    native_std::size_t size() const;
        // Return the number of discrete values that are stored in this table.
};

// PRIVATE MANIPULATORS
void PtrHashSet::grow()
{
    // 'bucketArraySize' will always be '2^N - 1', so that when pointers
    // are aligned by some 2^N they're likely to be relatively prime.

    native_std::size_t newBucketArraySize = bucketArraySize() * 2 + 1;
    native_std::size_t newBucketArraySizeInBytes =
                                           newBucketArraySize * sizeof(Bucket);
    memset(bucketArrayAddress(), 0x5a, size() * sizeof(Bucket));
    d_allocator_p->deallocate(bucketArrayAddress());
    setBucketArrayAddressAndSize(
             (Bucket *) d_allocator_p->allocate(newBucketArraySizeInBytes),
             newBucketArraySize);
    memset(bucketArrayAddress(), 0, newBucketArraySizeInBytes);
    Node *newListRootAddress = 0;

    unsigned numNodes = 0;
    for (Node *node = (Node *) listRootAddress(); node; ++numNodes) {
        Node *rippedOut = node;
        node = (Node *) node->nextLink();

        native_std::size_t index =
                              (UintPtr) rippedOut->value() % bucketArraySize();
        Bucket& bucket = bucketArrayAddress()[index];
        if (bucket.first()) {
            if (0 == bucket.first()->previousLink()) {
                newListRootAddress = rippedOut;
            }
            Util::insertLinkBeforeTarget(rippedOut, bucket.first());
            bucket.setFirst(rippedOut);
        }
        else {
            Util::insertLinkBeforeTarget(rippedOut,
                                         newListRootAddress);
            newListRootAddress = rippedOut;
            bucket.setFirstAndLast(rippedOut, rippedOut);
        }
    }
    ASSERT(size() == numNodes);

    setListRootAddress(newListRootAddress);

    checkInvariants();
}

// PRIVATE ACCESSORS
bool PtrHashSet::checkInvariants() const
{
    bool ok;

    size_t numNodes = 0;
    Node *prev = 0;
    for (Node *node = (Node *) listRootAddress(); node;
                               prev = node, node = (Node *) node->nextLink()) {
        ok = node->previousLink() == prev;
        ASSERT(ok && "node->previousLink() == prev");
        if (!ok) return false;                                        // RETURN
        ++numNodes;
    }
    ok = size() == numNodes;
    ASSERT(ok && "size() == numNodes");
    if (!ok) return false;                                            // RETURN

    numNodes = 0;
    for (unsigned i = 0; i < bucketArraySize(); ++i) {
        numNodes += bucketArrayAddress()[i].countElements();
    }
    ok = size() == numNodes;
    ASSERT(ok && "size() == numNodes");

    return ok;
}

bool PtrHashSet::find(Node **node, Bucket **bucket, const void *ptr) const
{
    Node   *dummyNodePtr;
    Bucket *dummyBucketPtr;
    if (!node  ) node   = &dummyNodePtr;
    if (!bucket) bucket = &dummyBucketPtr;

    Node *& nodePtrRef = *node;
    native_std::size_t index = reinterpret_cast<UintPtr>(ptr)
                                                           % bucketArraySize();
    Bucket& bucketRef = bucketArrayAddress()[index];
    *bucket = &bucketRef;
    if (bucketRef.first()) {
        Node *begin      = (Node *) bucketRef.first();
        Node * const end = (Node *) bucketRef.last()->nextLink();
        for (Node *n = begin; end != n; n = (Node *) n->nextLink()) {
            if (n->value() == ptr) {
                // found

                nodePtrRef = n;
                return true;                                          // RETURN
            }
        }
        // not found

        nodePtrRef = begin;
        return false;                                                 // RETURN
    }
    // empty bucket

    nodePtrRef = (Node *) listRootAddress();
    return false;
}

// CREATORS
PtrHashSet::PtrHashSet(bslma::Allocator *allocator)
: HashTableAnchor(0, 0, 0)
, d_maxLoadFactor(0.4)
, d_numNodes(0)
{
    enum { NUM_BUCKETS = 3 };

    d_allocator_p = bslma::Default::allocator(allocator);
    native_std::size_t bucketArraySizeInBytes = NUM_BUCKETS * sizeof(Bucket);
    setBucketArrayAddressAndSize(
                    (Bucket *) d_allocator_p->allocate(bucketArraySizeInBytes),
                    NUM_BUCKETS);
    memset(bucketArrayAddress(), 0, bucketArraySizeInBytes);
}

PtrHashSet::~PtrHashSet()
{
    BSLS_ASSERT_SAFE(checkInvariants());

    for (Node *node = (Node *) listRootAddress(); node; ) {
        Node *toDelete = node;
        node = (Node *) node->nextLink();

        memset(toDelete, 0x5a, sizeof(*toDelete));
        d_allocator_p->deallocate(toDelete);
    }

    d_allocator_p->deallocate(bucketArrayAddress());
}

// MANIPULATORS
bool PtrHashSet::erase(void *ptr)
{
    Bucket *bucket;
    Node   *node;

    if (!find(&node, &bucket, ptr)) {
        return false;                                                 // RETURN
    }

    if (bucket->first() == node) {
        if (bucket->last() == node) {
            bucket->reset();
        }
        else {
            bucket->setFirst(node->nextLink());
        }
    }
    else if (bucket->last() == node) {
        bucket->setLast(node->previousLink());
    }

    --d_numNodes;
    Util::unlink(node);

    d_allocator_p->deallocate(node);

    checkInvariants();

    return true;
}


bool PtrHashSet::insert(void *ptr)
{
    Bucket *bucket;
    Node *insertionPoint;

    if (find(&insertionPoint, &bucket, ptr)) {
        // Already in set, do nothing.

        return false;                                                 // RETURN
    }

    if (static_cast<double>(bucketArraySize()) * d_maxLoadFactor <
                                                              d_numNodes + 1) {
        grow();
        bool found = find(&insertionPoint, &bucket, ptr);
        (void) found; // Supress unused variable warnings in non-safe builds.
        BSLS_ASSERT_SAFE(!found);
    }

    ++d_numNodes;
    Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));

    Util::insertLinkBeforeTarget(node, insertionPoint);
    node->value() = ptr;
    if (listRootAddress() == insertionPoint) {
        BSLS_ASSERT_SAFE(0 == node->previousLink());
        setListRootAddress(node);
    }

    if (bucket->first()) {
        BSLS_ASSERT_SAFE(bucket->first() == insertionPoint);

        bucket->setFirst(node);
    }
    else {
        BSLS_ASSERT_SAFE(!bucket->last());

        bucket->setFirstAndLast(node, node);
    }

    ASSERT(count(ptr));

    checkInvariants();

    return true;
}

// ACCESSORS
native_std::size_t PtrHashSet::count(void *ptr) const
{
    return find(0, 0, ptr);
}

native_std::size_t PtrHashSet::size() const
{
    return d_numNodes;
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;      // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    initializeGlobalData();

    // CONCERN: No memory is ever allocated.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //   Demonstrate the usage of this component.
        //
        // Plan;
        //   Build a hash table based on it.
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        // Then, we create a test allocator for use in this example to ensure
        // that no memory is leaked:

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        // Next, we declare our table using that allocator:

        PtrHashSet phs(&ta);

        // Then, we create an area of memory from which our pointers will come:

        enum { SEGMENT_LENGTH = 1000 };
        char *pc = (char *) ta.allocate(SEGMENT_LENGTH);

        // Next, we iterate through the length of the segment, insert those
        // pointers for which 'ptr - pc == N * 7' is true.  We keep a count of
        // the number of items we insert into the table in the variable
        // 'sevens':

        unsigned sevens = 0;
        for (int i = 0; i < SEGMENT_LENGTH; i += 7) {
            ++sevens;
            bool status = phs.insert(&pc[i]);
            ASSERT(status);
        }

        // Then, we verify the number of objects we've placed in the table:

        ASSERT(phs.size() == sevens);

        // Next, we iterate through ALL pointers in the 'pc' array, using the
        // 'count' method to verify that the ones we expect are in the table:

        for (int i = 0; i < SEGMENT_LENGTH; ++i) {
            ASSERT(phs.count(&pc[i]) == (0 == i % 7));
        }

        // Then, we iterate, deleting all elements from the table for which
        // 'ptr - pc == 3 * N' is true.  We keep a count of the number of
        // elements which were in the table which we delete in the variable
        // 'killed':

        unsigned killed = 0;
        for (int i = 0; i < SEGMENT_LENGTH; i += 3) {
            const bool deleted = phs.erase(&pc[i]);
            ASSERT(deleted == (0 == i % 7));
            killed += deleted;
        }

        // Next, we verify the number of remaining elements in the table:

        ASSERT(killed < sevens);
        ASSERT(phs.size() == sevens - killed);

        // Then, in verbose mode we print our tallies:

        if (verbose) {
            printf("sevens = %u, killed = %u, phs.size() = %u\n", sevens,
                                                killed, (unsigned) phs.size());
        }

        // Now, we iterate through every element of the 'pc' array, verifying
        // that the surviving elements are exactly those for which 'ptr - pc'
        // was divisible by 7 and not by 3:

        for (int i = 0; i < SEGMENT_LENGTH; ++i) {
            const bool present = phs.count(&pc[i]);
            ASSERT(present == ((0 == i % 7) && (0 != i % 3)));
        }

        // Finally, we clean up our 'pc' array:

        ta.deallocate(pc);
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
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', having the value of
        //:     'R1'.
        //:
        //:   2 For each row 'R2 in the tree of P-2:  (C-1, 3..4)
        //:
        //:     1 Create a modifiable 'Obj', 'mX', having the the value of
        //:       'R2'.
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
        //: 4 For each node 'N1' in tree of P-2:  (C-3, 5)
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
        //   SimpleTestType& operator=(const SimpleTestType& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose)
            printf("\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void) operatorAssignment;  // quash potential compiler warning
        }

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1    = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) {
                T_ P_(LINE1) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            const Obj Z(EXP_ADD, EXP_SIZE, EXP_ROOT);
            const Obj ZZ(EXP_ADD, EXP_SIZE, EXP_ROOT);

            for (int tj = 0; tj < NUM_VALUES; ++tj) {
                const int LINE2    = VALUES[tj].d_line;
                Bucket *EXP_ADD2 = VALUES[tj].d_bucketArrayAddress;
                size_t EXP_SIZE2 = VALUES[tj].d_arraySize;
                Link *EXP_ROOT2 = VALUES[tj].d_listRootAddress;

                if (veryVerbose) {
                    T_ P_(LINE2) P_(EXP_ADD2) P_(EXP_SIZE2) P(EXP_ROOT2)
                }

                Obj mX(EXP_ADD2, EXP_SIZE2, EXP_ROOT2);

                Obj *mR = &(mX = Z);
                ASSERTV(ti, tj, mR, &mX, mR == &mX);

                ASSERTV(ti, tj, Z.bucketArraySize(), mX.bucketArraySize(),
                        Z == mX);
                ASSERTV(ti, tj, Z.bucketArraySize(), ZZ.bucketArraySize(),
                        Z == ZZ);
            }
        }

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) {
                T_ P_(LINE) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            Obj mX(EXP_ADD, EXP_SIZE, EXP_ROOT);
            const Obj ZZ(EXP_ADD, EXP_SIZE, EXP_ROOT);

            const Obj& Z = mX;
            Obj *mR = &(mX = Z);
            ASSERTV(ti, mR, &mX, mR == &mX);

            ASSERTV(ti, Z.bucketArraySize(), mX.bucketArraySize(), Z == mX);
            ASSERTV(ti, Z.bucketArraySize(), ZZ.bucketArraySize(), Z == ZZ);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 7: {
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
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1..3)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', having the value of
        //:     'R1'.
        //:
        //:   2 Use the copy constructor to create an object 'X', supplying it
        //:     the 'const' object 'Z'.  (C-2)
        //:
        //:   3 Verify that the newly constructed object 'X', has the same
        //:     value as that of 'Z'.  (C-1)
        //:
        //:   4 Verify that 'Z' still has the same value as that of 'ZZ'.
        //:     (C-3)
        //
        // Testing:
        //   SimpleTestType(const SimpleTestType& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) {
                T_ P_(LINE) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            const Obj  Z(EXP_ADD, EXP_SIZE, EXP_ROOT);
            const Obj ZZ(EXP_ADD, EXP_SIZE, EXP_ROOT);

            const Obj X(Z);

            ASSERTV(Z.bucketArraySize(), X.bucketArraySize(), Z == X);
            ASSERTV(Z.bucketArraySize(), ZZ.bucketArraySize(), Z == ZZ);
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they point
        //:   to the same node in the same tree.
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
        //; 9 The equality operator's signature and return type are standard.
        //:
        //:10 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-7..10)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create a single object, and use it to verify the reflexive
        //:     (anti-reflexive) property of equality (inequality) in the
        //:     presence of aliasing.  (C-2..3)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 Create an object 'X' having the value of 'R1'.  Create
        //:       another object 'Y' having the value of 'R2'.
        //:
        //:     3 Verify the commutativity property and the expected return
        //:       value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                            "\n=============================\n");


        if (verbose)
            printf("\nAssign the address of each operator to a variable.\n");
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = &bslalg::operator==;

            operatorPtr operatorNe = &bslalg::operator!=;

            (void) operatorEq;  // quash potential compiler warnings
            (void) operatorNe;
        }

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1 = VALUES[ti].d_line;
            Bucket *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t EXP_SIZE = VALUES[ti].d_arraySize;
            Link *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) {
                T_ P_(LINE1) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            Obj mX(EXP_ADD, EXP_SIZE, EXP_ROOT); const Obj& X = mX;

            // Ensure an object compares correctly with itself (alias test).
            ASSERTV(X.bucketArraySize(),   X == X);
            ASSERTV(X.bucketArraySize(), !(X != X));

            for (int tj = 0; tj < NUM_VALUES; ++tj) {
                const int LINE2 = VALUES[tj].d_line;
                Bucket *EXP_ADD2  = VALUES[tj].d_bucketArrayAddress;
                size_t EXP_SIZE2 = VALUES[tj].d_arraySize;
                Link *EXP_ROOT2 = VALUES[tj].d_listRootAddress;

                bool EXP = ti == tj;

                if (veryVerbose) {
                    T_ P_(LINE2) P_(EXP_ADD2) P_(EXP_SIZE2) P(EXP_ROOT2)
                }

                Obj mY(EXP_ADD2, EXP_SIZE2, EXP_ROOT2); const Obj& Y = mY;

                // Verify value, commutativity

                ASSERTV(X.bucketArrayAddress(),
                        Y.bucketArrayAddress(),
                        EXP,  (EXP == (X == Y)));
                ASSERTV(X.bucketArraySize(), Y.bucketArraySize(), EXP,
                       (EXP == (X == Y)));
                ASSERTV(X.listRootAddress(), Y.listRootAddress(), EXP,
                       (EXP == (X == Y)));

                ASSERTV(X.bucketArrayAddress(), Y.bucketArrayAddress(), EXP,
                       (EXP == (Y == X)));
                ASSERTV(X.bucketArraySize(), Y.bucketArraySize(), EXP,
                       (EXP == (Y == X)));
                ASSERTV(X.listRootAddress(), Y.listRootAddress(), EXP,
                       (EXP == (Y == X)));

                ASSERTV(X.bucketArraySize(), Y.bucketArraySize(), EXP,
                       (!EXP == (X != Y)));
                ASSERTV(X.bucketArraySize(), Y.bucketArraySize(), EXP,
                       (!EXP == (Y != X)));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   N/A
        // --------------------------------------------------------------------
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:    of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Use the default constructor, create an object having default
        //:   attribute values.  Verify that the accessor for the 'data'
        //:   attribute invoked on a reference providing non-modifiable access
        //:   to the object return the expected value.  (C-1)
        //:
        //: 2 Set the 'data' attribute of the object to another value.  Verify
        //:   that the accessor for the 'data' attribute invoked on a reference
        //:   providing non-modifiable access to the object return the expected
        //:   value.  (C-1, 2)
        //
        // Testing:
        //   int bucketArraySize() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBASIC ACCESSORS"
                   "\n===============\n");

        struct {
            Bucket  *d_array;
            size_t   d_size;
            Link    *d_root;
        } DATA[] = { // ARRAY             SIZE      ROOT
                     // ----------------  -------   -----------------
                   {                   0,       0,                  0 },
                   {(Bucket*) 0xdeadbeef,      13,      &DefaultLink1 },
                  };
        const size_t DATA_LEN = sizeof(DATA) / sizeof(*DATA);

        for (size_t i = 0; i < DATA_LEN; ++i) {
            Bucket* ARRAY = DATA[i].d_array;
            size_t  SIZE  = DATA[i].d_size;
            Link*   ROOT  = DATA[i].d_root;
            Obj mX(ARRAY, SIZE, ROOT); const Obj& X = mX;
            ASSERTV(X.bucketArrayAddress(), ARRAY == X.bucketArrayAddress());
            ASSERTV(X.bucketArraySize(),    SIZE == X.bucketArraySize());
            ASSERTV(X.listRootAddress(),    ROOT == X.listRootAddress());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the documented constraints.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row 'R1' in the table of P-1:  (C-1)
        //:
        //:   1 Use the value constructor to create an object 'X', having the
        //:     value of 'R1'.
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to verify
        //:     the attributes of the object have their expected value.  (C-1)
        //
        // Testing:
        //   SimpleTestType(int data);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR"
                            "\n==========\n");

        const int NUM_VALUES                        = DEFAULT_NUM_VALUES;
        const DefaultValueRow (&VALUES)[NUM_VALUES] = DEFAULT_VALUES;

        for (int ti = 0; ti < NUM_VALUES; ++ti) {
            const int LINE1    = VALUES[ti].d_line;
            Bucket   *EXP_ADD  = VALUES[ti].d_bucketArrayAddress;
            size_t    EXP_SIZE = VALUES[ti].d_arraySize;
            Link     *EXP_ROOT = VALUES[ti].d_listRootAddress;

            if (veryVerbose) {
                T_ P_(LINE1) P_(EXP_ADD) P_(EXP_SIZE) P(EXP_ROOT)
            }

            Obj mX(EXP_ADD, EXP_SIZE, EXP_ROOT); const Obj& X = mX;

            ASSERTV(EXP_ADD  == X.bucketArrayAddress());
            ASSERTV(EXP_SIZE == X.bucketArraySize());
            ASSERTV(EXP_ROOT == X.listRootAddress());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE CTOR & PRIMARY MANIPULATORS
        //   Ensure that we can use the default constructor to create an object
        //   (having the default constructed value).  Also ensure that we can
        //   use the primary manipulators to put that object into any state
        //   relevant for thorough testing.
        //
        // Concerns:
        //: 1 An object created with the value constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create three attribute values for the 'data' atrribute 'D', 'A',
        //:   and 'B'.  'D' should be the default value.  'A' and 'B' should be
        //:   the the boundary values.
        //:
        //: 2 Default-construct an object and use the individual (as yet
        //:   unproven) salient attribute accessors to verify the
        //:   default-constructed value.  (C-1)
        //:
        //: 3 Set and object's 'data' attribute to 'A' and 'B'.  Verify the
        //:   state of object using the (as yet unproven) salient attriubte
        //:   accessors.  (C-2)
        //
        // Testing:
        //   SimpleTestType();
        //   ~SimpleTestType();
        //   void setArraySize(int value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR & PRIMARY MANIPULATORS"
                            "\n=================================\n");

        bslalg::HashTableBucket ARRAY_D[1] = {};
        bslalg::HashTableBucket ARRAY_A[2] = {};
        bslalg::HashTableBucket ARRAY_B[5] = {};

        bslalg::BidirectionalLink LIST_A;
        LIST_A.reset();

        bslalg::BidirectionalLink LIST_B;
        LIST_B.reset();

        bslalg::HashTableBucket   *const D1 = ARRAY_D;
        const size_t                     D2 = 1;
        bslalg::BidirectionalLink *const D3 = 0;

        bslalg::HashTableBucket   *const A1 = ARRAY_A;
        const size_t                     A2 = 2;
        bslalg::BidirectionalLink *const A3 = &LIST_A;

        bslalg::HashTableBucket   *const B1 = ARRAY_B;
        const size_t                     B2 = 5;
        bslalg::BidirectionalLink *const B3 = &LIST_B;


        Obj mX(D1, D2, D3); const Obj& X = mX;
        ASSERTV(D1, X.bucketArrayAddress(), D1 == X.bucketArrayAddress());
        ASSERTV(D2, X.bucketArraySize(),    D2 == X.bucketArraySize());
        ASSERTV(D3, X.listRootAddress(),    D3 == X.listRootAddress());

        // 'bucketArrayAddress' and 'bucketArraySize'
        {
            mX.setBucketArrayAddressAndSize(A1, A2);
            ASSERTV(A1, X.bucketArrayAddress(), A1 == X.bucketArrayAddress());
            ASSERTV(A2, X.bucketArraySize(),    A2 == X.bucketArraySize());
            ASSERTV(D3, X.listRootAddress(),    D3 == X.listRootAddress());

            mX.setBucketArrayAddressAndSize(B1, B2);
            ASSERTV(B1, X.bucketArrayAddress(), B1 == X.bucketArrayAddress());
            ASSERTV(B2, X.bucketArraySize(),    B2 == X.bucketArraySize());
            ASSERTV(D3, X.listRootAddress(),    D3 == X.listRootAddress());

            mX.setBucketArrayAddressAndSize(D1, D2);
            ASSERTV(D1, X.bucketArrayAddress(), D1 == X.bucketArrayAddress());
            ASSERTV(D2, X.bucketArraySize(),    D2 == X.bucketArraySize());
            ASSERTV(D3, X.listRootAddress(),    D3 == X.listRootAddress());
        }

        // 'listRootAddress'
        {
            mX.setListRootAddress(A3);
            ASSERTV(D1, X.bucketArrayAddress(), D1 == X.bucketArrayAddress());
            ASSERTV(D2, X.bucketArraySize(),    D2 == X.bucketArraySize());
            ASSERTV(A3, X.listRootAddress(),    A3 == X.listRootAddress());

            mX.setListRootAddress(B3);
            ASSERTV(D1, X.bucketArrayAddress(), D1 == X.bucketArrayAddress());
            ASSERTV(D2, X.bucketArraySize(),    D2 == X.bucketArraySize());
            ASSERTV(B3, X.listRootAddress(),    B3 == X.listRootAddress());

            mX.setListRootAddress(D3);
            ASSERTV(D1, X.bucketArrayAddress(), D1 == X.bucketArrayAddress());
            ASSERTV(D2, X.bucketArraySize(),    D2 == X.bucketArraySize());
            ASSERTV(D3, X.listRootAddress(),    D3 == X.listRootAddress());
        }

        // Corroborate attribute independence.
        {

            // Set all attributes to their 'A' values.

            mX.setBucketArrayAddressAndSize(A1, A2);
            mX.setListRootAddress(A3);
            ASSERTV(A1, X.bucketArrayAddress(), A1 == X.bucketArrayAddress());
            ASSERTV(A2, X.bucketArraySize(),    A2 == X.bucketArraySize());
            ASSERTV(A3, X.listRootAddress(),    A3 == X.listRootAddress());

            // Set all attributes to their 'B' values.

            mX.setBucketArrayAddressAndSize(B1, B2);
            ASSERTV(B1, X.bucketArrayAddress(), B1 == X.bucketArrayAddress());
            ASSERTV(B2, X.bucketArraySize(),    B2 == X.bucketArraySize());
            ASSERTV(A3, X.listRootAddress(),    A3 == X.listRootAddress());

            mX.setListRootAddress(B3);
            ASSERTV(B1, X.bucketArrayAddress(), B1 == X.bucketArrayAddress());
            ASSERTV(B2, X.bucketArraySize(),    B2 == X.bucketArraySize());
            ASSERTV(B3, X.listRootAddress(),    B3 == X.listRootAddress());
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard handlerGuard;

            Obj obj(D1, D2, D3);

            if (veryVerbose)
                          printf("\tbucketArrayAddress and bucketArraySize\n");
            {
                ASSERT_SAFE_FAIL(obj.setBucketArrayAddressAndSize( 0, A2));
                ASSERT_SAFE_FAIL(obj.setBucketArrayAddressAndSize(A1,  0));
                ASSERT_SAFE_PASS(obj.setBucketArrayAddressAndSize(A1, A2));

                ASSERT_SAFE_FAIL(Obj obj2( 0, A2, A3));
                ASSERT_SAFE_FAIL(Obj obj2(A1,  0, A3));
                ASSERT_SAFE_PASS(Obj obj2(A1, A2, A3));
            }

            if (veryVerbose) printf("\tlistRootAddress\n");
            {
                bslalg::BidirectionalLink LIST_ERR;
                bslalg::BidirectionalLink *const BAD_PTR = &LIST_ERR;
                LIST_ERR.setNextLink(0);
                LIST_ERR.setPreviousLink(BAD_PTR);

                ASSERT_SAFE_FAIL(obj.setListRootAddress(BAD_PTR));
                ASSERT_SAFE_PASS(obj.setListRootAddress(A3));

                ASSERT_SAFE_FAIL(Obj obj2(A1, A2, BAD_PTR));
                ASSERT_SAFE_PASS(Obj obj2(A1, A2, A3));
            }
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

        bslalg::HashTableBucket array1[1] = {};
        bslalg::HashTableBucket array2[2] = {};

        Obj X(array1, 1, 0);
        ASSERT(X.bucketArraySize() == 1);

        X.setBucketArrayAddressAndSize(array2, 2);
        ASSERT(X.bucketArraySize() == 2);

        Obj Y(array1, 1, 0);
        ASSERT(Y.bucketArraySize() == 1);

        Obj Z(Y);
        ASSERT(Z == Y);
        ASSERT(X != Y);

        X = Z;
        ASSERT(Z == Y);
        ASSERT(X == Y);

        bslalg::BidirectionalLink root;
        root.reset();

        Z.setListRootAddress(&root);
        ASSERT(Z != Y);
        ASSERT(X == Y);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: No memory is ever allocated.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

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
