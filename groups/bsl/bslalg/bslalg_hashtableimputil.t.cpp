// bslalg_hashtableimputil.t.cpp                                      -*-C++-*-

#include <bslalg_hashtableimputil.h>

#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_bidirectionalnode.h>
#include <bslalg_hashtablebucket.h>
#include <bslalg_scalardestructionprimitives.h>
#include <bslalg_scalarprimitives.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// To resolve gcc warnings, while printing 'size_t' arguments portably on
// Windows, we use a macro and string literal concatenation to produce the
// correct 'printf' format flag.
#ifdef ZU
#undef ZU
#endif

#if defined BSLS_PLATFORM_CMP_MSVC
#  define ZU "%Iu"
#else
#  define ZU "%zu"
#endif

using namespace BloombergLP;
using namespace BloombergLP::bslalg;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [10] remove(HashTableAnchor *a, BidirectionalLink *l, size_t  h);
// [10] bucketContainsLink(const Bucket& b, BidirectionalLink *l);
// [ 9] find(const HashTableAnchor& a, KeyType& key, comparator, size_t h);
// [ 8] rehash(  HashTableAnchor *a, BidirectionalLink *r, const HASHER& h);
// [ 7] isWellFormed(const HashTableAnchor& anchor, bslma::Allocator *a = 0);
// [ 6] insertAtPosition(Anchor *a, Link *l, size_t h, Link  *p);
// [ 5] insertAtBackOfBucket( Anchor *a, BidirectionalLink *l, size_t h);
// [ 4] insertAtFrontOfBucket(Anchor *a, BidirectionalLink *l, size_t h);
// [ 3] const KeyType& extractKey(const BidirectionalLink *link);
// [ 3] typename ValueType& extractValue(BidirectionalLink *link);
// [ 2] computeBucketIndex(size_t hashCode, size_t numBuckets);
// [ 1] BREATHING TEST
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

// ============================================================================
//                       GLOBAL TYPEDEFS AND FUNCTIONS
// ----------------------------------------------------------------------------

typedef HashTableBucket           Bucket;
typedef HashTableAnchor           Anchor;
typedef HashTableImpUtil          Obj;
typedef BidirectionalLink         Link;
typedef BidirectionalLinkListUtil Util;

template <int N>
struct ArrayLength_Imp {
    char d_array[N];
};

// The following routine is never implemented, it is only called within a
// 'sizeof' so the code to call it is never generated.

template <class TYPE, size_t N>
inline
const ArrayLength_Imp<N>& arrayLength_Imp(const TYPE (&)[N]);

#define ARRAY_LENGTH(array) ((int) sizeof(arrayLength_Imp(array)))

template<class KEY>
struct TestSetKeyPolicy
{
    typedef KEY KeyType;
    typedef KEY ValueType;

    static const KeyType& extractKey(const ValueType& value)
    {
        return value;
    }
};

template <class VALUE>
struct NodeUtil {
    typedef BidirectionalNode<VALUE> Node;
    typedef VALUE                    ValueType;

    static
    Node *create(const VALUE&      value,
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

size_t countElements(bslalg::BidirectionalLink *first,
                     bslalg::BidirectionalLink *last = 0)
{
    BSLS_ASSERT(first);

    size_t result = !!last;
    for (; first != last; first = first->nextLink()) {
        ++result;
    }

    return result;
}

template <class EXPECTED_TYPE>
struct IsExpectedType {
    // Pass argument by pointer rather than by reference to these methods so as
    // not to break BDE rules and upset bdeflag.

    template <class OBJECT_TYPE>
    bool operator()(OBJECT_TYPE *) const
    {
        return false;
    }

    bool operator()(EXPECTED_TYPE *) const
    {
        return true;
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

template <class TYPE>
struct Equals {
    bool operator()(const TYPE& lhs, const TYPE& rhs) const
    {
        return lhs == rhs;
    }
};

bool listMatches(Link *first,
                 Link *last,
                 Link **arrayBegin,
                 Link **arrayEnd)
{
    if (!Util::isWellFormed(first, last)) {
        return false;                                                 // RETURN
    }

    for (; arrayBegin < arrayEnd; ++arrayBegin, first = first->nextLink()) {
        if (!first) {
            return !last;                                             // RETURN
        }

        if (*arrayBegin != first) {
            return false;                                             // RETURN
        }

        if (first == last) {
            return arrayBegin + 1 == arrayEnd;                        // RETURN
        }
    }

    return false;
}

void debugPrint(const HashTableAnchor& anchor)
{
    printf("Debug Print Anchor:\n");
    if (0 == anchor.bucketArraySize()) {
        printf("<empty>\n");
    }
    else {
        for (size_t n = 0; n < anchor.bucketArraySize(); ++n) {
            printf("\nBucket [" ZU "]: ", n);
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

struct WeirdRecord {
    int d_key;
    int d_payload;

    bool operator==(const WeirdRecord& rhs) const
    {
        return d_key == rhs.d_key && d_payload == rhs.d_payload;
    }
};

struct WeirdRecordKeyConfiguration {
    typedef WeirdRecord ValueType;
    typedef int         KeyType;

    static const KeyType& extractKey(const WeirdRecord& record)
    {
        return record.d_key;
    }
};

struct Mod100Hasher {
    size_t operator()(int value) const
    {
        return (unsigned) value % 100;
    }
};

struct Mod8Hasher {
    size_t operator()(int value) const
    {
        return value & 7;
    }
};

template <class HASHER, class POLICY>
struct HashNodeUsingHasherAndPolicy {
    HASHER d_hasher;

    size_t operator()(bslalg::BidirectionalLink *link) const
    {
        return d_hasher(Obj::extractKey<POLICY>(link));
    }
};

// ============================================================================
//                                 USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a Hash Set
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to build a hash set that will keep track of keys stored in
// set.
//
// First, we define an abstract template class 'HashSet' that will provide a
// hash set for any type that has a copy constructor, a destructor, an equality
// comparator and a hash function.  We inherit from the 'HashTableAnchor' class
// use the the 'BidirectionalLinkListUtil' and 'HashTableImpUtil' classes to
// facilitate building the table:
//..
template <class KEY, class HASHER, class EQUAL>
class HashSet : public bslalg::HashTableAnchor {
    // PRIVATE TYPES
    typedef bslalg::BidirectionalLink         Link;
    typedef bslalg::BidirectionalNode<KEY>    Node;
    typedef bslalg::HashTableBucket           Bucket;
    typedef bslalg::BidirectionalLinkListUtil ListUtil;
    typedef bslalg::HashTableImpUtil          ImpUtil;
    typedef native_std::size_t                size_t;

    struct Policy {
        typedef KEY KeyType;
        typedef KEY ValueType;

        static const KeyType& extractKey(const ValueType& value)
        {
            return value;
        }
    };

    // DATA
    double            d_maxLoadFactor;
    unsigned          d_numNodes;
    HASHER            d_hasher;
    EQUAL             d_equal;
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

    Node* find(const KEY& key,
               size_t     hashCode) const;
        // Return a pointer to the node containing the specified 'key', and 0
        // if no such node is in the table.

  private:
    // NOT IMPLEMENTED
    HashSet(const HashSet&, bslma::Allocator *);
    HashSet& operator=(const HashSet&);

  public:
    // CREATORS
    explicit
    HashSet(bslma::Allocator *allocator = 0);
        // Create a 'HashSet', using the specified 'allocator'.  If no
        // allocator is specified, use the default allocator.

    ~HashSet();
        // Destroy this 'HashSet', freeing all its memory.

    // MANIPULATORS
    bool insert(const KEY& key);
        // If the specfied 'key' is not in this hash table, add it, returning
        // 'true'.  If it is already in the table, return 'false' with no
        // action taken.

    bool erase(const KEY& key);
        // If the specfied 'key' is in this hash table, remove it, returning
        // 'true'.  If it is not found in the table, return 'false' with no
        // action taken.

    // ACCESSORS
    native_std::size_t count(const KEY& key) const;
        // Return 1 if the specified 'key' is in this table and 0 otherwise.

    native_std::size_t size() const;
        // Return the number of discrete keys that are stored in this table.
};

// PRIVATE MANIPULATORS
template <class KEY, class HASHER, class EQUAL>
void HashSet<KEY, HASHER, EQUAL>::grow()
{
    // 'bucketArraySize' will always be '2^N - 1', so that if hashed values
    // are aligned by some 2^N they're likely to be relatively prime to the
    // length of the hash table.

    d_allocator_p->deallocate(bucketArrayAddress());
    size_t newBucketArraySize = bucketArraySize() * 2 + 1;
    setBucketArrayAddressAndSize((Bucket *) d_allocator_p->allocate(
                                          newBucketArraySize * sizeof(Bucket)),
                                          newBucketArraySize);

    ImpUtil::rehash<Policy, HASHER>(this,
                                    listRootAddress(),
                                    d_hasher);
}

// PRIVATE ACCESSORS
template <class KEY, class HASHER, class EQUAL>
bool HashSet<KEY, HASHER, EQUAL>::checkInvariants() const
{
    // 'HashTableImpUtil's 'isWellFormed' will verify that all nodes are in
    // their proper buckets, that there are no buckets containing nodes that
    // are not in the main linked list, and no nodes in the main linked list
    // that are not in buckets.  To verify that 'd_numNodes' is correct we have
    // to traverse the list and count the nodes ourselves.

    size_t numNodes = 0;
    for (BidirectionalLink *cursor = listRootAddress();
                                         cursor; cursor = cursor->nextLink()) {
        ++numNodes;
    }

    return size() == numNodes &&
                 ImpUtil::isWellFormed<Policy>(*this, HASHER(), d_allocator_p);
}

template <class KEY, class HASHER, class EQUAL>
bslalg::BidirectionalNode<KEY> *HashSet<KEY, HASHER, EQUAL>::find(
                                             const KEY&         key,
                                             native_std::size_t hashCode) const
{
    return (Node *) ImpUtil::find<Policy, EQUAL>(*this,
                                                 key,
                                                 d_equal,
                                                 hashCode);
}

// CREATORS
template <class KEY, class HASHER, class EQUAL>
HashSet<KEY, HASHER, EQUAL>::HashSet(bslma::Allocator *allocator)
: HashTableAnchor(0, 0, 0)
, d_maxLoadFactor(0.4)
, d_numNodes(0)
{
    enum { NUM_BUCKETS = 3 };    // 'NUM_BUCKETS' must be '2^N - 1' for some
                                 // 'N'.

    d_allocator_p = bslma::Default::allocator(allocator);
    native_std::size_t bucketArraySizeInBytes = NUM_BUCKETS * sizeof(Bucket);
    setBucketArrayAddressAndSize(
                    (Bucket *) d_allocator_p->allocate(bucketArraySizeInBytes),
                    NUM_BUCKETS);
    memset(bucketArrayAddress(), 0, bucketArraySizeInBytes);
}

template <class KEY, class HASHER, class EQUAL>
HashSet<KEY, HASHER, EQUAL>::~HashSet()
{
    BSLS_ASSERT_SAFE(checkInvariants());

    for (Link *link = listRootAddress(); link; ) {
        Node *toDelete = (Node *) link;
        link = link->nextLink();

        toDelete->value().~KEY();
        d_allocator_p->deallocate(toDelete);
    }

    d_allocator_p->deallocate(bucketArrayAddress());
}

// MANIPULATORS
template <class KEY, class HASHER, class EQUAL>
bool HashSet<KEY, HASHER, EQUAL>::erase(const KEY& key)
{
    size_t hashCode = d_hasher(key);
    Node *node = find(key, hashCode);

    if (!node) {
        return false;                                                 // RETURN
    }

    size_t bucketIdx = ImpUtil::computeBucketIndex(hashCode,
                                                   bucketArraySize());
    Bucket& bucket = bucketArrayAddress()[bucketIdx];

    BSLS_ASSERT_SAFE(bucket.first() && bucket.last());

    if (bucket.first() == node) {
        if (bucket.last() == node) {
            bucket.reset();
        }
        else {
            bucket.setFirst(node->nextLink());
        }
    }
    else if (bucket.last() == node) {
        bucket.setLast(node->previousLink());
    }

    Link *root = listRootAddress() == node ? node->nextLink() : 0;
    ListUtil::unlink(node);
    if (root) {
        setListRootAddress(root);
    }

    node->value().~KEY();
    d_allocator_p->deallocate(node);

    --d_numNodes;
    BSLS_ASSERT_SAFE(checkInvariants());

    return true;
}

template <class KEY, class HASHER, class EQUAL>
bool HashSet<KEY, HASHER, EQUAL>::insert(const KEY& key)
{
    size_t hashCode = d_hasher(key);

    if (find(key, hashCode)) {
        // Already in set, do nothing.

        return false;                                                 // RETURN
    }

    if (static_cast<double>(bucketArraySize()) * d_maxLoadFactor <
                                                              d_numNodes + 1) {
        grow();
    }

    ++d_numNodes;
    Node *node = (Node *) d_allocator_p->allocate(sizeof(Node));
    bslalg::ScalarPrimitives::copyConstruct(&node->value(),
                                            key,
                                            d_allocator_p);

    ImpUtil::insertAtBackOfBucket(this, node, hashCode);

    BSLS_ASSERT_SAFE(find(key, hashCode));
    BSLS_ASSERT_SAFE(checkInvariants());

    return true;
}

// ACCESSORS
template <class KEY, class HASHER, class EQUAL>
native_std::size_t HashSet<KEY, HASHER, EQUAL>::count(const KEY& key) const
{
    return 0 != find(key, d_hasher(key));
}

template <class KEY, class HASHER, class EQUAL>
native_std::size_t HashSet<KEY, HASHER, EQUAL>::size() const
{
    return d_numNodes;
}
//..
// Then, we customize our table to manipulate zero-terminated 'const char *'
// strings.  We make the simplifying assumption that the strings pointed at by
// the 'const char *'s are longer-lived that the 'HashSet' will be.  We must
// provide an equality comparator so that two copies, in diffferent locations,
// of the same sequence of characters will evaluate equal:
//..
struct StringEqual {
    bool operator()(const char *lhs, const char *rhs) const
    {
        return !strcmp(lhs, rhs);
    }
};
//..
// Next, we must provide a string hash function to convert a 'const char *' to
// a 'size_t':
//..
struct StringHash {
    native_std::size_t operator()(const char *string) const;
};

native_std::size_t StringHash::operator()(const char *string) const
{
    enum { BITS_IN_SIZE_T = sizeof(size_t) * 8 };

    native_std::size_t result = 0;
    for (int shift = 0; *string;
                              ++string, shift = (shift + 7) % BITS_IN_SIZE_T) {
        unsigned char c = *string;
        if (shift <= BITS_IN_SIZE_T - 8) {
            result += c << shift;
        }
        else {
            result += c << shift;
            result += c >> (BITS_IN_SIZE_T - shift);
        }
    }

    return result;
};

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

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //   Demonstrate the functioning of this component.
        //
        // Plan:
        //   Create a hash table using the functions in this component and
        //   'HashTableAnchor'.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING USAGE EXAMPLE\n"
                            "=====================\n");
//..
// Then, we declare a test allocator and make it the default allocator to use
// during our example, to observe if we leak any memory:
//..
        bslma::TestAllocator da("defaultAllocator");
        bslma::DefaultAllocatorGuard defaultGuard(&da);
//..
// Next, in 'main', we create an instance of our 'HashSet' type, configured to
// contain 'const char *' strings:
//..
        HashSet<const char *, StringHash, StringEqual> hs;
//..
// Then, we insert a few values:
//..
        ASSERT(1 == hs.insert("woof"));
        ASSERT(1 == hs.insert("arf"));
        ASSERT(1 == hs.insert("meow"));
//..
// Next, we attempt to insert a redundant value, and observe that the 'insert'
// mthod returns 'false' to indicate that the insert was refused:
//..
        ASSERT(0 == hs.insert("woof"));
//..
// Then, we use to 'size' method to observe that there are 3 strings stored in
// our 'HashSet':
//..
        ASSERT(3 == hs.size());
//..
// Next, we use the 'count' method to observe, specifically, which strings are
// and are not in our 'HashSet':
//..
        ASSERT(1 == hs.count("woof"));
        ASSERT(1 == hs.count("arf"));
        ASSERT(1 == hs.count("meow"));
        ASSERT(0 == hs.count("ruff"));
        ASSERT(0 == hs.count("chomp"));
//..
// Then, we attempt to erase a string which is not in our 'HashSet' and observe
// that 'false' is returned, which tells us the 'erase' attempt was
// unsuccesful:
//..
        ASSERT(0 == hs.erase("ruff"));
//..
// Next, we erase the string "meow", which is stored in our 'HashSet' and
// observe that 'true' is returned, telling us the 'erase' attempt succeeded:
//..
        ASSERT(1 == hs.erase("meow"));
//..
// Now, we use the 'size' method to verify there are 2 strings remaining in our
// 'HashSet':
//..
        ASSERT(2 == hs.size());
//..
// Finally, we use the 'count' method to observe specifically which strings are
// still in our 'HashSet'.  Note that "meow" is no longer there.  We observe
// that the default allocator was never used.  When we leave the block, our
// 'HashSet' will be destroyed, freeing its memory, then our 'TestAllocator'
// will be destroyed, verifying that our destructor worked correctly and that
// no memory was leaked:
//..
        ASSERT(1 == hs.count("woof"));
        ASSERT(1 == hs.count("arf"));
        ASSERT(0 == hs.count("meow"));
        ASSERT(0 == hs.count("ruff"));
        ASSERT(0 == hs.count("chomp"));
//..
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // ATTEMPTED USAGE EXAMPLE
        //
        // This is a failed attempt at a usage example.  After I wrote it I
        // realized it would never pass review.  As long as the code is done
        // and working, I'll leave it here for extra testing.  The problem
        // with this usage example is it isn't telling a "story".
        //
        // Now that I have written another usage example that I expect will
        // pass review, I would like to point out that the following code
        // actually demonstrates the details of the behavior of the functions
        // in this component a lot better than that one does.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ATTEMPTED USAGE EXAMPLE\n"
                            "===============================\n");

// Then, in 'main', we set up some typedefs to refer to frequently used
// types:

        typedef bslalg::HashTableImpUtil       Obj;
        typedef bslalg::BidirectionalNode<int> IntNode;
        typedef TestSetKeyPolicy<int>          TestPolicy;
        typedef NodeUtil<int>                  IntNodeUtil;

// Next, we create a hasher functor object that will take a link as input,
// interpret it as a an 'IntNode', and hash the int to a size_t using
// 'Mod8Hasher':

        HashNodeUsingHasherAndPolicy<Mod8Hasher, TestSetKeyPolicy<int> >
                                                                    nodeHasher;

// Then, we declare a default allocator to be used in this example:

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);
        bslma::TestAllocatorMonitor dam(&da);

// Next, we declare an array to hold our links, and an array of buckets:

        enum { NUM_LINK_ARRAY = 015 };
        bslalg::BidirectionalLink *links[NUM_LINK_ARRAY];
        memset(links, 0, sizeof(links));

        enum { NUM_BUCKET_ARRAY = 8 };
        bslalg::HashTableBucket buckets[NUM_BUCKET_ARRAY];
        memset(buckets, 0, sizeof(buckets));

// Then, we declare an anchor for our hash table:

        int numActiveBuckets = 4;
        bslalg::HashTableAnchor anchor(buckets, numActiveBuckets, 0);

// Next, we populate our table with some links:

#define CREATE_NODE(octal) do {                                               \
            links[octal] = IntNodeUtil::create(octal, &da);                   \
            Obj::insertAtBackOfBucket(                                        \
                            &anchor, links[octal], nodeHasher(links[octal])); \
        } while(false)

        CREATE_NODE(000);        CREATE_NODE(010);
        CREATE_NODE(001);
        CREATE_NODE(002);
        CREATE_NODE(004);        CREATE_NODE(014);

#undef CREATE_NODE

// Then, we make sure our table is well-formed.  Monitor the default allocator
// and observe that some default memory is temporarily used and freed:
        Mod8Hasher hasher;

        dam.reset();
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));
        ASSERT(dam.isTotalUp());
        ASSERT(dam.isInUseSame());

// Next, we call 'bucketContainsLink' to observe which bucket node '014' is in:

        for (int i = 0; i < numActiveBuckets; ++i) {
            ASSERTV(i, ((int) Obj::computeBucketIndex(nodeHasher(links[014]),
                                                numActiveBuckets) == i) ==
                              Obj::bucketContainsLink(buckets[i], links[014]));
        }

// Then, we observe there are 4 nodes in 'buckets[0]' (they are '000', '001',
// '004', '014'):

        ASSERT(4 == buckets[0].countElements());
        ASSERT(links[000] == buckets[0].first());
        ASSERT(links[010] == buckets[0].first()->nextLink());
        ASSERT(links[004] == buckets[0].first()->nextLink()->nextLink());
        ASSERT(links[014] == buckets[0].last());

// Next, we use 'remove' to remove node '014' from the table, and observe the
// state of the table and bucket after that:

        Obj::remove(&anchor, links[014], nodeHasher(links[014]));

        ASSERT(0 == Obj::computeBucketIndex(nodeHasher(links[014]),
                                            numActiveBuckets));
        ASSERT(3 == buckets[0].countElements());
        ASSERT(links[000] == buckets[0].first());
        ASSERT(links[004] == buckets[0].last());
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

// Then, we insert node '014' back into the table, but in the wrong bucket,
// and observe that the table is no longer well-formed:

        Obj::insertAtBackOfBucket(&anchor, links[014], 3);

        ASSERT(1 == buckets[3].countElements());
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

// Next, we move it back to the bucket it was originally in, but put it at the
// front of the bucket, rather than the rear, using 'insertAtFrontOfBucket'.
// It has the same hash value as node '004', which is at the rear of the
// bucket.  The two nodes with the same hash value are now separated from each,
// with the nodes '000' and '010' with hash value '0' in between.  In a former
// implementation, this would have meant the table was not well formed, but
// in the current implementation, this still passes muster:

        Obj::remove(&anchor, links[014], 3);
        Obj::insertAtFrontOfBucket(&anchor,
                                   links[014],
                                   nodeHasher(links[014]));

        ASSERT(4 == hasher(014));
        ASSERT(0 == Obj::computeBucketIndex(nodeHasher(links[014]),
                                            numActiveBuckets));
        ASSERT(4 == buckets[0].countElements());
        ASSERT(links[014] == buckets[0].first());
        ASSERT(links[000] == buckets[0].first()->nextLink());
        ASSERT(links[010] == buckets[0].first()->nextLink()->nextLink());
        ASSERT(links[004] == buckets[0].last());

        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

// Then, we move the node to before node '004', which has the same hash value,
// so they'll be adjacent and the hash table will be well-formed:

        ASSERT(links[004] == buckets[0].last());

        Obj::remove(&anchor, links[014], 4);
        Obj::insertAtPosition(&anchor,
                              links[014],
                              4,
                              links[004]);

        ASSERT(4 == buckets[0].countElements());
        ASSERT(links[000] == buckets[0].first());
        ASSERT(links[010] == buckets[0].first()->nextLink());
        ASSERT(links[014] == buckets[0].first()->nextLink()->nextLink());
        ASSERT(links[004] == buckets[0].last());

        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

// Now, we grow the table from having 4 buckets to having 8 buckets using
// 'rehash':

        numActiveBuckets = NUM_BUCKET_ARRAY;    // numActiveBuckets = 8;
        anchor.setBucketArrayAddressAndSize(anchor.bucketArrayAddress(),
                                            numActiveBuckets);

        Obj::rehash<TestPolicy>(&anchor, anchor.listRootAddress(), hasher);

// Finally, we observe our grown table in detail, and clean up:

        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        ASSERT(links[000] == buckets[0].first());
        ASSERT(links[010] == buckets[0].last());

        ASSERT(links[001] == buckets[1].first());
        ASSERT(links[001] == buckets[1].last());

        ASSERT(links[002] == buckets[2].first());
        ASSERT(links[002] == buckets[2].last());

        ASSERT(0          == buckets[3].countElements());

        ASSERT(links[014] == buckets[4].first());
        ASSERT(links[004] == buckets[4].last());

        ASSERT(0          == buckets[5].countElements());
        ASSERT(0          == buckets[6].countElements());
        ASSERT(0          == buckets[7].countElements());

        IntNodeUtil::disposeList(anchor.listRootAddress(), &da);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'remove' and 'bucketContainsLink'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'remove' and 'bucketContainsLink'\n"
                            "=========================================\n");

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);

        typedef BidirectionalNode<int> IntNode;
        typedef TestSetKeyPolicy<int>  TestPolicy;
        typedef NodeUtil<int>          IntNodeUtil;

#define CREATE(octal) IntNode *node ## octal = IntNodeUtil::create(octal, &oa)

        CREATE(000);
        CREATE(010);
        CREATE(020);
        CREATE(030);

        CREATE(004);
        CREATE(014);
        CREATE(024);
        CREATE(034);

        CREATE(001);
        CREATE(011);

        CREATE(002);
        CREATE(012);
        CREATE(022);
        CREATE(032);

#undef CREATE

        Bucket buckets[8];
        memset(buckets, 0, sizeof(buckets));

        Anchor anchor(buckets, 4, 0);
        Mod8Hasher hasher;

        Obj::insertAtBackOfBucket(&anchor, node000, 0);
        Obj::insertAtBackOfBucket(&anchor, node010, 0);
        Obj::insertAtBackOfBucket(&anchor, node020, 0);
        Obj::insertAtBackOfBucket(&anchor, node030, 0);

        Obj::insertAtBackOfBucket(&anchor, node001, 1);
        Obj::insertAtBackOfBucket(&anchor, node011, 1);

        Obj::insertAtBackOfBucket(&anchor, node002, 2);
        Obj::insertAtBackOfBucket(&anchor, node012, 2);
        Obj::insertAtBackOfBucket(&anchor, node022, 2);
        Obj::insertAtBackOfBucket(&anchor, node032, 2);

        Obj::insertAtBackOfBucket(&anchor, node004, 0);
        Obj::insertAtBackOfBucket(&anchor, node014, 0);
        Obj::insertAtBackOfBucket(&anchor, node024, 0);
        Obj::insertAtBackOfBucket(&anchor, node034, 0);

        ASSERT(8 == buckets[0].countElements());
        ASSERT(2 == buckets[1].countElements());
        ASSERT(4 == buckets[2].countElements());
        ASSERT(0 == buckets[3].countElements());

        Link *links[035];
        memset(links, 0, sizeof(links));

        links[000] = node000;
        links[010] = node010;
        links[020] = node020;
        links[030] = node030;

        links[001] = node001;
        links[011] = node011;

        links[002] = node002;
        links[012] = node012;
        links[022] = node022;
        links[032] = node032;

        links[004] = node004;
        links[014] = node014;
        links[024] = node024;
        links[034] = node034;

        ASSERT(14 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        for (int i = 0; i < ARRAY_LENGTH(links); ++i) {
            for (int j = 0; j < 4; ++j) {
                ASSERT((i % 4 == j && links[i]) ==
                                Obj::bucketContainsLink(buckets[j], links[i]));
            }
        }

        {
            Link *matches[] = { node001, node011 };
            ASSERT(2 == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[1].first(), buckets[1].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030,
                                node004, node014, node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030 };
            ASSERT(4 == countElements(node000, node030));
            ASSERT(listMatches(node000, node030,
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node004, node014, node024, node034 };
            ASSERT(4 == countElements(node004, node034));
            ASSERT(listMatches(node004, node034,
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node002, node012, node022, node032 };
            ASSERT(buckets[2].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[2].first(), buckets[2].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        if (verbose) Q(Remove from the middle of a bucket);
        {
            Obj::remove(&anchor, node014, 0);

            Link *matches[] = { node000, node010, node020, node030,
                                node004         , node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(Util::isWellFormed(node000, node034));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }
        {
            Obj::remove(&anchor, node020, 0);

            Link *matches[] = { node000, node010,          node030,
                                node004         , node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(Util::isWellFormed(node000, node034));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }
        {
            Obj::remove(&anchor, node030, 0);

            Link *matches[] = { node000, node010,
                                node004         , node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(Util::isWellFormed(node000, node034));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        if (verbose) Q(Remove from the front of the bucket);
        {
            ASSERT(node011 == node000->previousLink());

            Obj::remove(&anchor, node000, 0);

            ASSERT(node011 == node010->previousLink());

            Link *matches[] = {          node010,
                                node004, node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(Util::isWellFormed(node011, node034));
            ASSERT(buckets[0].countElements() + 1 == countElements(node011,
                                                                   node034));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        ASSERT(10 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Remove from the front of the bucket and front of root);
        {
            ASSERT(0 == node002->previousLink());

            Obj::remove(&anchor, node002, 2);

            ASSERT(0 == node012->previousLink());

            Link *matches[] = { node012, node022, node032 };
            ASSERT(node012 == anchor.listRootAddress());
            ASSERT(node012 == buckets[2].first());
            ASSERTV(buckets[2].countElements(),
                          buckets[2].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[2].first(), buckets[2].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        ASSERT(9 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Remove from the back of the bucket);
        {
            ASSERT(node001 == node032->nextLink());

            Obj::remove(&anchor, node032, 2);

            ASSERT(node001 == node022->nextLink());

            Link *matches[] = { node012, node022 };
            ASSERTV(buckets[2].countElements(),
                          buckets[2].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[2].first(), buckets[2].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        ASSERT(8 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Remove from the last link of the bucket);
        {
            ASSERT(0 == node012->previousLink());
            Obj::remove(&anchor, node012, 2);

            ASSERT(0 == node022->previousLink());
            Obj::remove(&anchor, node022, 2);

            ASSERT(node001 == anchor.listRootAddress());

            ASSERTV(buckets[2].countElements(),
                                              0 == buckets[2].countElements());
            ASSERT(0 == buckets[2].first());
            ASSERT(0 == buckets[2].last());
        }

        ASSERT(6 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

#define DELETE(octal) oa.deallocate(node ## octal)

        DELETE(000);
        DELETE(010);
        DELETE(020);
        DELETE(030);

        DELETE(004);
        DELETE(014);
        DELETE(024);
        DELETE(034);

        DELETE(001);
        DELETE(011);

        DELETE(002);
        DELETE(012);
        DELETE(022);
        DELETE(032);

#undef DELETE

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'find'\n"
                            "==============\n");

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);

        typedef BidirectionalNode<int> IntNode;
        typedef TestSetKeyPolicy<int>  TestPolicy;
        typedef NodeUtil<int>          IntNodeUtil;

#define CREATE(octal) IntNode *node ## octal = IntNodeUtil::create(octal, &oa)

        CREATE(000);
        CREATE(010);
        CREATE(020);
        CREATE(030);

        CREATE(004);
        CREATE(014);
        CREATE(024);
        CREATE(034);

        CREATE(001);
        CREATE(011);

        CREATE(002);
        CREATE(012);
        CREATE(022);
        CREATE(032);

#undef CREATE

        Bucket buckets[8];
        memset(buckets, 0, sizeof(buckets));

        Anchor anchor(buckets, 2, 0);    const Anchor& ANCHOR = anchor;
        Mod8Hasher hasher;

        Obj::insertAtBackOfBucket(&anchor, node000, 0);
        Obj::insertAtBackOfBucket(&anchor, node010, 0);
        Obj::insertAtBackOfBucket(&anchor, node020, 0);
        Obj::insertAtBackOfBucket(&anchor, node030, 0);

        Obj::insertAtBackOfBucket(&anchor, node001, 1);
        Obj::insertAtBackOfBucket(&anchor, node011, 1);

        Obj::insertAtBackOfBucket(&anchor, node002, 0);
        Obj::insertAtBackOfBucket(&anchor, node012, 0);
        Obj::insertAtBackOfBucket(&anchor, node022, 0);
        Obj::insertAtBackOfBucket(&anchor, node032, 0);

        Obj::insertAtBackOfBucket(&anchor, node004, 0);
        Obj::insertAtBackOfBucket(&anchor, node014, 0);
        Obj::insertAtBackOfBucket(&anchor, node024, 0);
        Obj::insertAtBackOfBucket(&anchor, node034, 0);

        ASSERT(0 == node001->previousLink());
        ASSERT(0 == node034->nextLink());
        ASSERT(14 == countElements(node001, node034));
        ASSERT(14 == countElements(anchor.listRootAddress()));

        ASSERT(12 == buckets[0].countElements());
        ASSERT(2  == buckets[1].countElements());

        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        {
            Link *matches[] = { node001, node011 };
            ASSERT(2 == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[1].first(), buckets[1].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030,
                                node002, node012, node022, node032,
                                node004, node014, node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        ASSERT(node014 == (Obj::find<TestPolicy, Equals<int> >(ANCHOR,
                                                               014,
                                                               Equals<int>(),
                                                               0)));

        Link *links[035];
        memset(links, 0, sizeof(links));

        links[000] = node000;
        links[010] = node010;
        links[020] = node020;
        links[030] = node030;

        links[001] = node001;
        links[011] = node011;

        links[002] = node002;
        links[012] = node012;
        links[022] = node022;
        links[032] = node032;

        links[004] = node004;
        links[014] = node014;
        links[024] = node024;
        links[034] = node034;

        for (int i = 0; i < ARRAY_LENGTH(links); ++i) {
            ASSERTV(i, links[i] == (Obj::find<TestPolicy, Equals<int> >(
                                                                 ANCHOR,
                                                                 i,
                                                                 Equals<int>(),
                                                                 i % 2)));
        }

        {
            Link *matches[] = { node001, node011 };
            ASSERT(2 == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[1].first(), buckets[1].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030,
                                node002, node012, node022, node032,
                                node004, node014, node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        ASSERT(14 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        for (int i = 0; i < ARRAY_LENGTH(links); ++i) {
            if (links[i]) {
                oa.deallocate(links[i]);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'rehash'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'rehash'\n"
                            "================\n");

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);

        typedef BidirectionalNode<int> IntNode;
        typedef TestSetKeyPolicy<int>  TestPolicy;
        typedef NodeUtil<int>          IntNodeUtil;

#define CREATE(octal) IntNode *node ## octal = IntNodeUtil::create(octal, &oa)

        CREATE(000);
        CREATE(010);
        CREATE(020);
        CREATE(030);

        CREATE(004);
        CREATE(014);
        CREATE(024);
        CREATE(034);

        CREATE(001);
        CREATE(011);

        CREATE(002);
        CREATE(012);
        CREATE(022);
        CREATE(032);

#undef CREATE

        Bucket buckets[8];
        memset(buckets, 0, sizeof(buckets));

        Anchor anchor(buckets, 2, 0);
        Mod8Hasher hasher;

        // Make sure 'rehash' doesn't segfault or anything given an empty
        // list.

        Obj::rehash<TestPolicy>(&anchor, 0, hasher);
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        Obj::insertAtBackOfBucket(&anchor, node000, 0);
        Obj::insertAtBackOfBucket(&anchor, node010, 0);
        Obj::insertAtBackOfBucket(&anchor, node020, 0);
        Obj::insertAtBackOfBucket(&anchor, node030, 0);

        Obj::insertAtBackOfBucket(&anchor, node001, 1);
        Obj::insertAtBackOfBucket(&anchor, node011, 1);

        Obj::insertAtBackOfBucket(&anchor, node002, 0);
        Obj::insertAtBackOfBucket(&anchor, node012, 0);
        Obj::insertAtBackOfBucket(&anchor, node022, 0);
        Obj::insertAtBackOfBucket(&anchor, node032, 0);

        Obj::insertAtBackOfBucket(&anchor, node004, 0);
        Obj::insertAtBackOfBucket(&anchor, node014, 0);
        Obj::insertAtBackOfBucket(&anchor, node024, 0);
        Obj::insertAtBackOfBucket(&anchor, node034, 0);

        ASSERT(0 == node001->previousLink());
        ASSERT(0 == node034->nextLink());
        ASSERT(14 == countElements(node001, node034));
        ASSERT(14 == countElements(anchor.listRootAddress()));

        ASSERT(12 == buckets[0].countElements());
        ASSERT(2  == buckets[1].countElements());

        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        {
            Link *matches[] = { node001, node011 };
            ASSERT(2 == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[1].first(), buckets[1].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030,
                                node002, node012, node022, node032,
                                node004, node014, node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        bslalg::BidirectionalLink *root = anchor.listRootAddress();
        memset(buckets, 0xa4, sizeof(buckets));
        anchor.setBucketArrayAddressAndSize(buckets, 4);

        Obj::rehash<TestPolicy>(&anchor, root, hasher);
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        ASSERT(8 == buckets[0].countElements());
        ASSERT(2 == buckets[1].countElements());
        ASSERT(4 == buckets[2].countElements());
        ASSERT(0 == buckets[3].countElements());

        {
            Link *matches[] = { node001, node011 };
            ASSERT(2 == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[1].first(), buckets[1].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030,
                                node004, node014, node024, node034 };
            ASSERT(buckets[0].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[0].first(), buckets[0].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node000, node010, node020, node030 };
            ASSERT(4 == countElements(node000, node030));
            ASSERT(listMatches(node000, node030,
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node004, node014, node024, node034 };
            ASSERT(4 == countElements(node004, node034));
            ASSERT(listMatches(node004, node034,
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        {
            Link *matches[] = { node002, node012, node022, node032 };
            ASSERT(buckets[2].countElements() == ARRAY_LENGTH(matches));
            ASSERT(listMatches(buckets[2].first(), buckets[2].last(),
                               matches, matches + ARRAY_LENGTH(matches)));
        }

        ASSERT(14 == countElements(anchor.listRootAddress()));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

#define DELETE(octal) oa.deallocate(node ## octal)

        DELETE(000);
        DELETE(010);
        DELETE(020);
        DELETE(030);

        DELETE(004);
        DELETE(014);
        DELETE(024);
        DELETE(034);

        DELETE(001);
        DELETE(011);

        DELETE(002);
        DELETE(012);
        DELETE(022);
        DELETE(032);

#undef DELETE

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'isWellFormed'
        //
        // Concerns:
        //: o 'isWellFormed' performs as specced.
        //
        // Plan:
        //: o Test on empty tables of various lengths.
        //: o Test on healthy table.
        //: o Sabotage table, one defect at a time, so that:
        //:   1 A 'previousLink()' is incorrect.
        //:   2 A node is in the wrong bucket.
        //:   3 A bucket.first() is wrong, but not null.
        //:   4 A bucket.last() is wrong, but not null.
        //:   5 Both first() and last() of a bucket are null when they have
        //:     a node in the root list.
        //:   6 A bucket contains a link that is not in the root list.
        //:   7 Two nodes with identical hash values are in the same bucket,
        //:     but not in a contiguous sequence for that hash value.
        //:     Was forbidden in earlier implementation, now permissible.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'isWellFormed'\n"
                            "======================\n");

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);

        typedef BidirectionalNode<int> IntNode;
        typedef TestSetKeyPolicy<int>  TestPolicy;
        typedef NodeUtil<int>          IntNodeUtil;

        Mod100Hasher hasher;

        if (verbose) Q(Testing empty tables of various lengths);
        {
            const int MAX_BUCKETS = 100;

            Bucket buckets[MAX_BUCKETS];
            memset(buckets, 0, sizeof(buckets));

            for (int i = 1; i < MAX_BUCKETS; ++i) {
                Anchor anchor(buckets, i, 0);

                ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));
            }
        }

        if (verbose) Q(Build somewhat complex healthy table to test);

        Bucket buckets[10];
        memset(buckets, 0, sizeof(buckets));

        Anchor anchor(buckets, 10, 0);

        IntNode *node010 = IntNodeUtil::create( 10, &oa);
        IntNode *node020 = IntNodeUtil::create( 20, &oa);
        IntNode *node030 = IntNodeUtil::create( 30, &oa);
        IntNode *node130 = IntNodeUtil::create(130, &oa);
        IntNode *node230 = IntNodeUtil::create(230, &oa);
        IntNode *node330 = IntNodeUtil::create(330, &oa);
        IntNode *node011 = IntNodeUtil::create( 11, &oa);
        IntNode *node012 = IntNodeUtil::create( 12, &oa);
        IntNode *node033 = IntNodeUtil::create( 13, &oa);
        IntNode *node043 = IntNodeUtil::create( 13, &oa);
        IntNode *node055 = IntNodeUtil::create( 13, &oa);

        Obj::insertAtFrontOfBucket(&anchor, node043, 3);
        Obj::insertAtPosition(&anchor,      node033, 3, node043);

        Obj::insertAtFrontOfBucket(&anchor, node011, 1);

        IntNode *root = (IntNode *) anchor.listRootAddress();

        ASSERT(node011 == root);
        ASSERT(0       == node011->previousLink());
        ASSERT(node033 == node011->nextLink());
        ASSERT(node011 == node033->previousLink());
        ASSERT(node043 == node033->nextLink());
        ASSERT(node033 == node043->previousLink());
        ASSERT(0       == node043->nextLink());

        ASSERT(node033 == buckets[3].first());
        ASSERT(node043 == buckets[3].last());

        ASSERTV(buckets[1].first(), node011 == buckets[1].first());
        ASSERTV(buckets[1].last(),  node011 == buckets[1].last());

        for (int i = 0; i < 10; ++i) {
            if (1 != i && 3 != i) {
                ASSERT(0 == buckets[i].first());
                ASSERT(0 == buckets[i].last());
            }
            else {
                ASSERT(0 != buckets[i].first());
                ASSERT(0 != buckets[i].last());
            }
        }

        ASSERT(Util::isWellFormed(node011, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        Obj::insertAtFrontOfBucket(&anchor, node330, 0);
        Obj::insertAtPosition(&anchor,      node230, 0, node330);
        Obj::insertAtPosition(&anchor,      node130, 0, node230);

        root = (IntNode *) anchor.listRootAddress();

        ASSERT(node130 == root);
        ASSERT(0       == node130->previousLink());
        ASSERT(node230 == node130->nextLink());
        ASSERT(node130 == node230->previousLink());
        ASSERT(node330 == node230->nextLink());
        ASSERT(node011 == node330->nextLink());
        ASSERT(node330 == node011->previousLink());

        ASSERT(node130 == buckets[0].first());
        ASSERT(node330 == buckets[0].last());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        Obj::insertAtFrontOfBucket(&anchor, node012, 2);

        ASSERT(node012 == buckets[2].first());
        ASSERT(node012 == buckets[2].last());

        Obj::insertAtPosition(&anchor, node030, 0, node130);
        Obj::insertAtPosition(&anchor, node020, 0, node030);
        Obj::insertAtPosition(&anchor, node010, 0, node020);

        ASSERT(node010 == buckets[0].first());
        ASSERT(node330 == buckets[0].last());

        ASSERT(6 == buckets[0].countElements());

        root = (IntNode *) anchor.listRootAddress();
        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        // --------------------------------------------------

        if (verbose) Q(Healthy table is complete -- ready for sabotage);

        if (verbose) Q(Faulty 'previousLink()');    // ------------------

        node330->setPreviousLink(node130);

        ASSERT(0 == Util::isWellFormed(root, node043));
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        node330->setPreviousLink(node230);

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Node in wrong bucket);    // ----------------

        buckets[5].setFirstAndLast(node012, node012);
        buckets[2].reset();

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        buckets[2].setFirstAndLast(node012, node012);
        buckets[5].reset();

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Bucket.first() is wrong);    // -----------------

        buckets[1].setFirst(buckets[1].first()->previousLink());
        ASSERT(buckets[1].first());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        buckets[1].setFirst(buckets[1].first()->nextLink());
        ASSERT(buckets[1].first());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Bucket.last() is wrong);    // -----------------

        buckets[1].setLast(buckets[1].last()->previousLink());
        ASSERT(buckets[1].last());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        buckets[1].setLast(buckets[1].last()->nextLink());
        ASSERT(buckets[1].last());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Orphan node in list -- not in bucket);    // -----------

        buckets[1].reset();

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        buckets[1].setFirstAndLast(node011, node011);

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Node in bucket that is not in root list);    // --------

        buckets[5].setFirstAndLast(node055, node055);

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT(0 == (Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        buckets[5].reset();

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Node in right bucket wrong hash sequence - permissible);

        {
            Bucket bigBucket = { node010, node130 };
            ASSERT(Util::isWellFormed(bigBucket.first(), bigBucket.last()));
            ASSERT(4 == bigBucket.countElements());
        }
        ASSERT(6 == buckets[0].countElements());

        Util::unlink(node020);
        Util::insertLinkBeforeTarget(node020, node130);

        {
            Bucket bigBucket = { node010, node130 };
            ASSERT(Util::isWellFormed(bigBucket.first(), bigBucket.last()));
            ASSERT(4 == bigBucket.countElements());
        }
        ASSERT(6 == buckets[0].countElements());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        Util::unlink(node020);
        Util::insertLinkBeforeTarget(node020, node030);
        ASSERT(6 == buckets[0].countElements());

        ASSERT(Util::isWellFormed(root, node043));
        ASSERT((Obj::isWellFormed<TestPolicy>(anchor, hasher)));

        if (verbose) Q(Done testing -- cleanup);    // --------------

        oa.deallocate(node010);
        oa.deallocate(node020);
        oa.deallocate(node030);
        oa.deallocate(node130);
        oa.deallocate(node230);
        oa.deallocate(node330);
        oa.deallocate(node011);
        oa.deallocate(node012);
        oa.deallocate(node033);
        oa.deallocate(node043);
        oa.deallocate(node055);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING insertAtPosition
        //
        // Concerns:
        //: o 'insertAtPosition' performs as specced.
        //
        // Plan:
        //: o Test insertion at beginning of a bucket, existing node is only
        //:   node.
        //: o Test insertion not at beginning of the bucket.
        //: o Test insertion before the last node in the bucket (same as middle
        //:   of many nodes in the bucket, same as if other buckets existed).
        //: o Test insertion at beginning of new bucket, root list not empty.
        //: o Test insertion at beginning of second bucket.
        // --------------------------------------------------------------------

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor om(&oa);

        {
            typedef BidirectionalNode<int> IntNode;
            typedef TestSetKeyPolicy<int>  TestPolicy;
            typedef NodeUtil<int>          IntNodeUtil;

            IntNode *nodes[] = { IntNodeUtil::create(0, &oa),
                                 IntNodeUtil::create(1, &oa),
                                 IntNodeUtil::create(2, &oa),
                                 IntNodeUtil::create(3, &oa),
                                 IntNodeUtil::create(4, &oa),
                                 IntNodeUtil::create(5, &oa) };
            const int NUM_NODES = sizeof nodes / sizeof *nodes;

            Link *garbage = (Link *) 0xbaddeed5;

            for (int i = 0; i < NUM_NODES; ++i) {
                nodes[i]->setPreviousLink(garbage);
                nodes[i]->setNextLink(    garbage);
            }

            Bucket buckets[] = { { 0, 0 }, { 0, 0 } };
            Anchor anchor(buckets, 2, 0);

            Obj::insertAtFrontOfBucket(&anchor, nodes[0], 0);

            // At the front of a bucket

            Obj::insertAtPosition(&anchor, nodes[1], 0, nodes[0]);

            ASSERT(0        == nodes[1]->previousLink());
            ASSERT(nodes[0] == nodes[1]->nextLink());
            ASSERT(0        == nodes[0]->nextLink());
            ASSERT(nodes[1] == nodes[0]->previousLink());
            ASSERT(nodes[1] == buckets[0].first());
            ASSERT(nodes[0] == buckets[0].last());
            ASSERT(nodes[1] == anchor.listRootAddress());
            ASSERT(2 == buckets[0].countElements());

            // In the middle / before the last node in the bucket

            Obj::insertAtPosition(&anchor, nodes[2], 0, nodes[0]);

            ASSERT(nodes[1] == nodes[2]->previousLink());
            ASSERT(nodes[2] == nodes[1]->nextLink());
            ASSERT(nodes[0] == nodes[2]->nextLink());
            ASSERT(nodes[1] == buckets[0].first());
            ASSERT(nodes[0] == buckets[0].last());
            ASSERT(nodes[1] == anchor.listRootAddress());
            ASSERT(3 == buckets[0].countElements());

            ASSERT(0 == nodes[1]->previousLink());
            ASSERT(0 == nodes[0]->nextLink());
            ASSERT(Util::isWellFormed(nodes[1], nodes[0]));

            // At the front of bucket, list not empty

            Obj::insertAtFrontOfBucket(&anchor, nodes[3], 1);

            ASSERT(0        == nodes[3]->previousLink());
            ASSERT(nodes[1] == nodes[3]->nextLink());
            ASSERT(nodes[3] == nodes[1]->previousLink());
            ASSERT(nodes[3] == buckets[1].first());
            ASSERT(nodes[3] == buckets[1].last());
            ASSERT(nodes[3] == anchor.listRootAddress());
            ASSERT(1 == buckets[1].countElements());

            ASSERT(nodes[1] == buckets[0].first());
            ASSERT(nodes[0] == buckets[0].last());
            ASSERT(3 == buckets[0].countElements());

            ASSERT(0 == nodes[3]->previousLink());
            ASSERT(0 == nodes[0]->nextLink());
            ASSERT(Util::isWellFormed(nodes[3], nodes[0]));

            {
                Bucket bigBucket = { nodes[3], nodes[0] };
                ASSERT(4 == bigBucket.countElements());
            }

            nodes[0]->setNextLink(nodes[3]);
            nodes[3]->setPreviousLink(nodes[0]);
            nodes[3]->setNextLink(0);
            nodes[1]->setPreviousLink(0);

            anchor.setListRootAddress(nodes[1]);

            ASSERT(0 == nodes[1]->previousLink());
            ASSERT(0 == nodes[3]->nextLink());
            ASSERT(Util::isWellFormed(nodes[1], nodes[3]));

            {
                Bucket bigBucket = { nodes[1], nodes[3] };
                ASSERT(4 == bigBucket.countElements());
            }

            Obj::insertAtPosition(&anchor, nodes[4], 1, nodes[3]);

            ASSERT(nodes[4] == nodes[3]->previousLink());
            ASSERT(nodes[3] == nodes[4]->nextLink());
            ASSERT(nodes[4] == buckets[1].first());
            ASSERT(nodes[3] == buckets[1].last());
            ASSERT(2 == buckets[1].countElements());

            ASSERT(nodes[1] == anchor.listRootAddress());

            ASSERT(0 == nodes[1]->previousLink());
            ASSERT(0 == nodes[3]->nextLink());
            ASSERT(Util::isWellFormed(nodes[1], nodes[3]));

            {
                Bucket bigBucket = { nodes[1], nodes[3] };
                ASSERT(5 == bigBucket.countElements());
            }

            for (int i = 0; i < NUM_NODES; ++i) {
                IntNodeUtil::destroy(nodes[i], &oa);
            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING insertAtBackOfBucket
        //
        // Concerns:
        //   That 'insertAtBackOfBucket' performs as specced
        //
        // Plan:
        //: Test in 4 cases:
        //: o Bucket empty, root empty
        //: o Bucket not empty, root contains only bucket
        //: o Bucket empty, root not empty
        //: o Bucket not empty, root not empty, not pointing to bucket
        // --------------------------------------------------------------------

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor om(&oa);

        {
            typedef BidirectionalNode<int> IntNode;
            typedef TestSetKeyPolicy<int>  TestPolicy;
            typedef NodeUtil<int>          IntNodeUtil;

            IntNode *nodes[] = { IntNodeUtil::create(0, &oa),
                                 IntNodeUtil::create(1, &oa),
                                 IntNodeUtil::create(2, &oa),
                                 IntNodeUtil::create(3, &oa),
                                 IntNodeUtil::create(4, &oa),
                                 IntNodeUtil::create(5, &oa) };
            const int NUM_NODES = sizeof nodes / sizeof *nodes;

            Link *garbage = (Link *) 0xbaddeed5;

            for (int i = 0; i < NUM_NODES; ++i) {
                nodes[i]->setPreviousLink(garbage);
                nodes[i]->setNextLink(    garbage);
            }

            Bucket bucket = { 0, 0 };
            Anchor anchor(&bucket, 1, 0);

            Obj::insertAtBackOfBucket(&anchor, nodes[0], 0);
            ASSERT(0 == nodes[0]->previousLink());
            ASSERT(0 == nodes[0]->nextLink());
            ASSERT(nodes[0] == bucket.first());
            ASSERT(nodes[0] == bucket.last());
            ASSERT(nodes[0] == anchor.listRootAddress());
            ASSERT(1 == bucket.countElements());

            Obj::insertAtBackOfBucket(&anchor, nodes[1], 1);
            ASSERT(nodes[0] == nodes[1]->previousLink());
            ASSERT(0        == nodes[1]->nextLink());
            ASSERT(nodes[1] == nodes[0]->nextLink());
            ASSERT(0        == nodes[0]->previousLink());
            ASSERT(nodes[0] == bucket.first());
            ASSERT(nodes[1] == bucket.last());
            ASSERT(nodes[0] == anchor.listRootAddress());
            ASSERT(2 == bucket.countElements());

            bucket.reset();

            Obj::insertAtBackOfBucket(&anchor, nodes[2], 2);
            ASSERT(0        == nodes[2]->previousLink());
            ASSERT(nodes[0] == nodes[2]->nextLink());
            ASSERT(nodes[2] == nodes[0]->previousLink());
            ASSERT(nodes[2] == bucket.first());
            ASSERT(nodes[2] == bucket.last());
            ASSERT(nodes[2] == anchor.listRootAddress());
            ASSERT(1 == bucket.countElements());

            // Now inserting with not empty bucket, with list root address
            // not pointing to bucket.

            nodes[0]->setPreviousLink(0);
            anchor.setListRootAddress(nodes[0]);

            Util::insertLinkAfterTarget(nodes[2], nodes[1]);
            ASSERT(0        == nodes[2]->nextLink());
            ASSERT(nodes[1] == nodes[2]->previousLink());

            ASSERT(1 == bucket.countElements());
            ASSERT(nodes[2] == bucket.first());
            ASSERT(nodes[2] == bucket.last());

            Obj::insertAtBackOfBucket(&anchor, nodes[3], 3);
            ASSERT(nodes[2] == nodes[3]->previousLink());
            ASSERT(0        == nodes[3]->nextLink());
            ASSERT(nodes[1] == nodes[2]->previousLink());
            ASSERT(nodes[2] == bucket.first());
            ASSERT(nodes[3] == bucket.last());
            ASSERT(nodes[0] == anchor.listRootAddress());
            ASSERT(2 == bucket.countElements());

            ASSERT(0 == nodes[0]->previousLink());
            ASSERT(Util::isWellFormed(nodes[0], nodes[2]));

            Bucket bigBucket = { nodes[0], nodes[3] };
            ASSERT(4 == bigBucket.countElements());

            for (int i = 0; i < NUM_NODES; ++i) {
                IntNodeUtil::destroy(nodes[i], &oa);
            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING insertAtFrontOfBucket
        //
        // Concerns:
        //   That 'insertAtFrontOfBucket' performs as specced
        //
        // Plan:
        //: Test in 4 cases:
        //: o Bucket empty, root empty
        //: o Bucket not empty, root contains only bucket
        //: o Bucket empty, root not empty
        //: o Bucket not empty, root not empty, not pointing to bucket
        // --------------------------------------------------------------------

        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor om(&oa);

        {
            typedef BidirectionalNode<int> IntNode;
            typedef TestSetKeyPolicy<int>  TestPolicy;
            typedef NodeUtil<int>          IntNodeUtil;

            IntNode *nodes[] = { IntNodeUtil::create(0, &oa),
                                 IntNodeUtil::create(1, &oa),
                                 IntNodeUtil::create(2, &oa),
                                 IntNodeUtil::create(3, &oa),
                                 IntNodeUtil::create(4, &oa),
                                 IntNodeUtil::create(5, &oa) };
            const int NUM_NODES = sizeof nodes / sizeof *nodes;

            Link *garbage = (Link *) 0xbaddeed5;

            for (int i = 0; i < NUM_NODES; ++i) {
                nodes[i]->setPreviousLink(garbage);
                nodes[i]->setNextLink(    garbage);
            }

            Bucket bucket = { 0, 0 };
            Anchor anchor(&bucket, 1, 0);

            Obj::insertAtFrontOfBucket(&anchor, nodes[0], 0);
            ASSERT(0 == nodes[0]->previousLink());
            ASSERT(0 == nodes[0]->nextLink());
            ASSERT(nodes[0] == bucket.first());
            ASSERT(nodes[0] == bucket.last());
            ASSERT(nodes[0] == anchor.listRootAddress());
            ASSERT(1 == bucket.countElements());

            Obj::insertAtFrontOfBucket(&anchor, nodes[1], 1);
            ASSERT(0        == nodes[1]->previousLink());
            ASSERT(nodes[0] == nodes[1]->nextLink());
            ASSERT(0        == nodes[0]->nextLink());
            ASSERT(nodes[1] == nodes[0]->previousLink());
            ASSERT(nodes[1] == bucket.first());
            ASSERT(nodes[0] == bucket.last());
            ASSERT(nodes[1] == anchor.listRootAddress());
            ASSERT(2 == bucket.countElements());

            bucket.reset();

            Obj::insertAtFrontOfBucket(&anchor, nodes[2], 2);
            ASSERT(0        == nodes[2]->previousLink());
            ASSERT(nodes[1] == nodes[2]->nextLink());
            ASSERT(nodes[2] == nodes[1]->previousLink());
            ASSERT(nodes[2] == bucket.first());
            ASSERT(nodes[2] == bucket.last());
            ASSERT(nodes[2] == anchor.listRootAddress());
            ASSERT(1 == bucket.countElements());

            nodes[1]->setPreviousLink(0);
            anchor.setListRootAddress(nodes[1]);

            Util::insertLinkAfterTarget(nodes[2], nodes[0]);
            ASSERT(0        == nodes[2]->nextLink());
            ASSERT(nodes[0] == nodes[2]->previousLink());

            // Now inserting with not empty bucket, with list root address
            // not pointing to bucket.

            Obj::insertAtFrontOfBucket(&anchor, nodes[3], 3);
            ASSERT(nodes[0] == nodes[3]->previousLink());
            ASSERT(nodes[2] == nodes[3]->nextLink());
            ASSERT(0        == nodes[1]->previousLink());
            ASSERT(nodes[3] == bucket.first());
            ASSERT(nodes[2] == bucket.last());
            ASSERT(nodes[1] == anchor.listRootAddress());
            ASSERT(2 == bucket.countElements());

            ASSERT(0 == nodes[1]->previousLink());
            ASSERT(0 == nodes[2]->nextLink());
            ASSERT(Util::isWellFormed(nodes[3], nodes[2]));

            Bucket bigBucket = { nodes[1], nodes[2] };
            ASSERT(4 == bigBucket.countElements());

            for (int i = 0; i < NUM_NODES; ++i) {
                IntNodeUtil::destroy(nodes[i], &oa);
            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING EXTRACTVALUE AND EXTRACTKEY
        //
        // Concerns:
        //   That 'extractValue' and 'extractKey' work as described;
        // --------------------------------------------------------------------


        bslma::TestAllocator da("defaultAllocator", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard defaultGuard(&da);

        bslma::TestAllocator oa("objectAllocator", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor om(&oa);

        {
            typedef BidirectionalNode<WeirdRecord> WeirdNode;
            typedef NodeUtil<WeirdRecord>          Util;
            typedef WeirdRecordKeyConfiguration    Config;

            const WeirdRecord wr = { 1, 2 };
            WeirdNode *wn = (WeirdNode *) Util::create(wr, &oa);

            ASSERT(wr == wn->value());

            ASSERT(1  == Obj::extractKey<Config>(wn));
            ASSERT(wr == Obj::extractValue<Config>(wn));

            ASSERT(IsExpectedType<const int>()(&Obj::extractKey<Config>(wn)));
            ASSERT(!IsExpectedType<int>()(&Obj::extractKey<Config>(wn)));
            ASSERT(!IsExpectedType<const WeirdRecord>()(
                                              &Obj::extractValue<Config>(wn)));
            ASSERT(IsExpectedType<WeirdRecord>()(
                                              &Obj::extractValue<Config>(wn)));

            Util::destroy(wn, &oa);
        }

        {
            typedef BidirectionalNode<int> IntNode;
            typedef TestSetKeyPolicy<int>  TestPolicy;
            typedef NodeUtil<int>          IntNodeUtil;

            const int K = 7;

            IntNode *in = (IntNode *) IntNodeUtil::create(K, &oa);

            ASSERT(K == in->value());

            ASSERT(K == Obj::extractKey<TestPolicy>(in));
            ASSERT(K == Obj::extractValue<TestPolicy>(in));

            ASSERT(!IsExpectedType<int>()(&Obj::extractKey<TestPolicy>(in)));
            ASSERT(IsExpectedType<const int>()(
                                           &Obj::extractKey<TestPolicy>(in)));
            ASSERT(!IsExpectedType<const int>()(
                                          &Obj::extractValue<TestPolicy>(in)));
            ASSERT(IsExpectedType<int>()(&Obj::extractValue<TestPolicy>(in)));
            IntNodeUtil::destroy(in, &oa);
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING ComputeBucketIndex
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ComputeBucketIndex\n"
                            "==========================\n");

        struct {
            int d_line;
            size_t d_hashCode;
            size_t d_numBuckets;
            size_t d_expected;
        } DATA[] = {
            { L_,   0,  1,  0 },
            { L_,  11, 11,  0 },
            { L_,  81, 11,  4 },
            { L_,  32, 11, 10 },
            { L_,  81,  1,  0 },
            { L_, 100, 11,  1 },
            { L_, 100, 12,  4 },
            { L_, 100,  7,  2 } };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE           = DATA[i].d_line;
            const size_t HASH_CODE   = DATA[i].d_hashCode;
            const size_t NUM_BUCKETS = DATA[i].d_numBuckets;
            const size_t EXPECTED    = DATA[i].d_expected;

            const size_t RESULT = Obj::computeBucketIndex(HASH_CODE,
                                                          NUM_BUCKETS);
            ASSERTV(LINE, RESULT, EXPECTED == RESULT);
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

        IntTestHasherIdent defaultHasher;

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
                          Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);

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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);

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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);

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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
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
                     Obj::isWellFormed<TestPolicy>(ANCHOR, defaultHasher);
            ASSERTV(IS_VALID);

            Bucket newArray[DATA_SIZE];
            memset(newArray, 0, sizeof(newArray));
            Anchor newAnchor(newArray, DATA_SIZE, 0);

            Obj::rehash<TestPolicy>(&newAnchor,
                                    anchor.listRootAddress(),
                                    defaultHasher);
            ASSERT((Obj::isWellFormed<TestPolicy>(newAnchor, defaultHasher)));
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
                  Obj::isWellFormed<TestPolicy>(newAnchor, defaultHasher);
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
