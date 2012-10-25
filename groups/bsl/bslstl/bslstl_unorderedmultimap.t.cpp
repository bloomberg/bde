// bslstl_unorderedmultimap.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedmultimap.h>

#include <bslstl_pair.h>
#include <bslstl_string.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_stdtestallocator.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Initial breathing test iterates all operations with a single template
// instantiation and test obvious boundary conditions and iterator stability
// guarantees.
//
// MULTIMAP TEST SHOULD MAP *DIFFERENT* VALUES AGAINST DUPLICATE KEYS AND TEST
// ACCORDINGLY.
//-----------------------------------------------------------------------------
// [ ]
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                              TEST SUPPORT
//-----------------------------------------------------------------------------
namespace bsl {

// set-specific print function.
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void debugPrint(
              const bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef bsl::unordered_multimap<KEY, VALUE, HASH, EQUAL, ALLOC> TObj;
        typedef typename TObj::const_iterator CIter;
        typedef typename TObj::const_local_iterator LCIter;

        for (size_t n = 0; n < s.bucket_count(); ++n) {
            if (s.cbegin(n) == s.cend(n)) {
                continue;
            }
            printf("\nBucket [%d]: ", n);
            for (LCIter lci = s.cbegin(n); lci != s.cend(n); ++lci) {
                printf("[%d, %d], ", lci->first, lci->second);
                  //    bsls::BslTestUtil::callDebugprint(
                  //        static_cast<char>(
                  //        bsltf::TemplateTestFacility::getIdentifier(*lci)));
            }
            printf("\n");
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

bool g_verbose;
bool g_veryVerbose;
bool g_veryVeryVerbose;
bool g_veryVeryVeryVerbose;
bool g_veryVeryVeryVeryVerbose;

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
//    return v;        // for 'set' containers
    return v.first;  // for 'map' containers
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin() == x.end());
    ASSERT(x.cbegin() == x.cend());

    ASSERT(0 == x.count(5));
    ASSERT(x.end() == x.find(5));

    SizeType nBuckets = x.bucket_count();
    for (SizeType i = 0; i != nBuckets; ++i) {
        ASSERT(0 == x.bucket_size(i));
        ASSERT(x.begin(i) == x.end(i));
        ASSERT(x.cbegin(i) == x.cend(i));
    }

    const bsl::pair<typename TestType::const_iterator,
                    typename TestType::const_iterator>
                                                  emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testEmptyContainer(CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin() == x.end());
    ASSERT(x.cbegin() == x.cend());
    // Check iterator/const_iterator comparisons compile
    ASSERT(x.begin() == x.cend());
    ASSERT(x.cbegin() == x.end());

    ASSERT(0 == x.count(5));
    ASSERT(x.end() == x.find(5));

    SizeType nBuckets = x.bucket_count();
    for (SizeType i = 0; i != nBuckets; ++i) {
        ASSERT(0 == x.bucket_size(i));
        ASSERT(x.begin(i) == x.end(i));
        ASSERT(x.cbegin(i) == x.cend(i));
    }

    const bsl::pair<typename TestType::iterator, typename TestType::iterator>
                                                  emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    typename TestType::iterator it = x.erase(x.begin(), x.end());
                                                          // should not assert

    ASSERT(x.end() == it);

    ASSERT(0 == x.erase(93));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testContainerHasData(const CONTAINER&                      x,
                          typename CONTAINER::size_type         nCopies,
                          const typename CONTAINER::value_type *data,
                          typename CONTAINER::size_type         size)
{
    typedef          CONTAINER                 TestType;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    LOOP2_ASSERT(x.size(),   nCopies * size,
                 x.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        TestIterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(*it == data[i]);
        LOOP2_ASSERT(keyForValue<CONTAINER>(data[i]),
                     x.count(keyForValue<CONTAINER>(data[i])),
                     nCopies == x.count(keyForValue<CONTAINER>(data[i])));

        bsl::pair<TestIterator, TestIterator> range =
                                x.equal_range(keyForValue<CONTAINER>(data[i]));
        ASSERT(range.first == it);
        for(int iterations = nCopies; --iterations; ++it) {
            ASSERT(*it == data[i]);
        }
        // Need one extra increment to reach past-the-range iterator.
        if (++it != x.end()) {
            ASSERT(*it != data[i]);
        }
        ASSERT(range.second == it);
    }
}

template <class CONTAINER>
void fillContainerWithData(CONTAINER&                            x,
                           const typename CONTAINER::value_type *data,
                           size_t                                size)
{
    typedef CONTAINER TestType;

    int initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (size_t i = 0; i != size; ++i) {
        typename TestType::iterator it =
                                       x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(data[i] == *it);
    }
}

template<typename CONTAINER>
void validateIteration(CONTAINER &c)
{
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const int size = c.size();

    int counter = 0;
    for (iterator it = c.begin(); it != c.end(); ++it, ++counter) {}
    ASSERT(size == counter);

    counter = 0;
    for (const_iterator it = c.cbegin(); it != c.cend(); ++it, ++counter) {}
    ASSERT(size == counter);

    const CONTAINER& cc = c;

    counter = 0;
    for (const_iterator it = cc.begin(); it != cc.end(); ++it, ++counter) {}
    ASSERT(size == counter);

    counter = 0;
    for (const_iterator it = cc.cbegin(); it != cc.cend(); ++it, ++counter) {}
    ASSERT(size == counter);
}

template <class CONTAINER>
void testBuckets(CONTAINER& mX)
{
    // Basic test of buckets:
    // number of buckets is returned by bucket_count
    // Number of buckets should reflect load_factor and max_load_factor
    // Each bucket hold a number of elements specified by bucket_size
    // bucket can be iterated from bucket_begin to bucket_end
    //    each element should match the bucket number via bucket(key)
    //    should have as many elements as reported by bucket_count
    // adding elements from all buckets should exactly equal 'size'
    // large buckets imply many hash collisions, which is undesirable
    //    large buckets may be consequence of multicontainers
    typedef typename             CONTAINER::iterator             iterator;
    typedef typename       CONTAINER::const_iterator       const_iterator;
    typedef typename       CONTAINER::local_iterator       local_iterator;
    typedef typename CONTAINER::const_local_iterator const_local_iterator;


    const CONTAINER &x = mX;

    size_t bucketCount = x.bucket_count();
    size_t collisions = 0;
    size_t itemCount  = 0;

    for (unsigned i = 0; i != bucketCount; ++i ) {
        const unsigned count = x.bucket_size(i);
        if (0 == count) {
            ASSERT(x.begin(i) == x.end(i));
            ASSERT(mX.begin(i) == mX.end(i));
            ASSERT(mX.cbegin(i) == mX.cend(i));
            // compile test iterator compatibility here, not needed later
            ASSERT(mX.cbegin(i) == mX.end(i));
            ASSERT(mX.begin(i) == mX.cend(i));
            continue;
        }

        itemCount += count;
        collisions += count-1;
        unsigned int bucketItems = 0;
        for (const_local_iterator iter = x.begin(i); iter != x.end(i); ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

        bucketItems = 0;
        for (local_iterator iter = mX.begin(i); iter != mX.end(i); ++iter) {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

        bucketItems = 0;
        for (const_local_iterator iter = mX.cbegin(i);
             iter != mX.cend(i);
             ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERT(count == bucketItems);

    }
    ASSERT(itemCount == x.size());
}


template <class CONTAINER>
void testErase(CONTAINER& mX)
{
    typedef typename CONTAINER::      iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const CONTAINER& x = mX;
    size_t size = x.size();

    // test empty sub-ranges
    ASSERT(x.begin() == mX.erase(x.cbegin(), x.cbegin()));
    ASSERT(x.size() == size);

    ASSERT(x.begin() == mX.erase(x.begin(), x.begin()));
    ASSERT(x.size() == size);

    ASSERT(x.end() == mX.erase(x.cend(), x.cend()));
    ASSERT(x.size() == size);

    ASSERT(x.end() == mX.erase(x.end(), x.end()));
    ASSERT(x.size() == size);

    // eliminate all elements with a matching key
    const_iterator cIter = x.begin();
    bsl::advance(cIter, x.size()/10);
    typename CONTAINER::key_type key = keyForValue<CONTAINER>(*cIter);
    size_t duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    bsl::pair<const_iterator, const_iterator> valRange = x.equal_range(key);
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
    ASSERT(mX.erase(valRange.first, valRange.second) == valRange.second);
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    bsl::advance(cIter, x.size()/5);
    key = keyForValue<CONTAINER>(*cIter);
    duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    valRange = x.equal_range(keyForValue<CONTAINER>(*cIter));
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
                            // Will handle subranges terminating in 'end' later
    cIter = valRange.first;
    while (cIter != valRange.second) {
        iterator next = mX.erase(cIter);  // compile check for return type
        cIter = next;
    }
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    bsl::advance(cIter, x.size()/3);
    key = keyForValue<CONTAINER>(*cIter);
    duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    ASSERT(mX.erase(key) == duplicates);
    ASSERT(x.end() == x.find(key));
    ASSERT(x.size() + duplicates == size);
    validateIteration(mX);

    // erase elements straddling a bucket
    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    const_iterator next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    size_t erasures = 0;
    while (cIter != next) {
        cIter = mX.erase(cIter);  // compile check for return type
        ++erasures;
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // erase elements straddling a bucket, in reverse order
    // cIter is invalidated, so reset and start next sub-test
    // range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}
    ++next;
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    erasures = 0;
    while (cIter != next) {
        const_iterator cursor = cIter;
        const_iterator testCursor = cursor;
        while(++testCursor != next) {
            cursor = testCursor;
        }
        if (cursor == cIter) {
            cIter = mX.erase(cursor);  // compile check for return type
            ASSERT(cIter == next);
            ++erasures;
        }
        else {
            cursor = mX.erase(cursor);  // compile check for return type
            ASSERT(cursor == next);
            ++erasures;
        }
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // Finally, test erasing the tail of the container, as 'end' is often
    // invalidated.
    // first truncate with a range-based erase
    cIter = mX.erase(cIter, mX.end());  // mixed const/mutable iterators
    ASSERT(cIter == x.end());
    validateIteration(mX);

    // then erase the rest of the container, one item at a time, from the front
    for(iterator it = mX.begin(); it != x.end(); it = mX.erase(it)) {}
    testEmptyContainer(mX);
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
namespace {
    template <class TYPE>
    struct MyHashFunctor {
        std::size_t operator()(const TYPE& value) const
        {
            return static_cast<std::size_t>(value);
        }

    };

    template <>
    struct MyHashFunctor<int> {
        std::size_t operator()(int value) const
        {
            return static_cast<std::size_t>(value);
            // return static_cast<std::size_t>(bdeu_HashUtil::hash1(value));
        }
    };
}  // close unnamed namespace

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

                g_verbose =             verbose;
            g_veryVerbose =         veryVerbose;
        g_veryVeryVerbose =     veryVeryVerbose;
    g_veryVeryVeryVerbose = veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAlloc("A");
    bslma::Default::setDefaultAllocator(&testAlloc);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        {
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Concordance
///- - - - - - - - - - - - - - - - -
// Unordered multimap maps are useful in situations when there is no meaningful
// way to compare key values, when the order of the keys is irrelevant to the
// problem domain, and (even if there is a meaningful ordering) the value of
// ordering the results is outweighed by the higher performance provided by
// unordered maps (compared to ordered maps).
//
// One uses a multi-map (ordered or unordered) when there may be more than one
// value associated with a key value.  In this example we will use
// 'bslstl_unorderedmultimap' to create a concordance (an index of where each
// unique word appears).
//
// Our source of documents is a set of statically initialized arrrays:
//..
    static char document0[] =
    " IN CONGRESS, July 4, 1776.\n"
    "\n"
    " The unanimous Declaration of the thirteen united States of America,\n"
    "\n"
    " When in the Course of human events, it becomes necessary for one\n"
    " people to dissolve the political bands which have connected them with\n"
    " another, and to assume among the powers of the earth, the separate\n"
    " and equal station to which the Laws of Nature and of Nature's God\n"
    " entitle them, a decent respect to the opinions of mankind requires\n"
    " that they should declare the causes which impel them to the\n"
    " separation.  We hold these truths to be self-evident, that all men\n"
    " are created equal, that they are endowed by their Creator with\n"
    " certain unalienable Rights, that among these are Life, Liberty and\n"
    " the pursuit of Happiness.--That to secure these rights, Governments\n"
    " are instituted among Men, deriving their just powers from the consent\n"
    " of the governed, --That whenever any Form of Government becomes\n"
    " destructive of these ends, it is the Right of the People to alter or\n"
    " to abolish it, and to institute new Government, laying its foundation\n"
    " on such principles and organizing its powers in such form, as to them\n"
    " shall seem most likely to effect their Safety and Happiness.\n"
    " Prudence, indeed, will dictate that Governments long established\n"
    " should not be changed for light and transient causes; and accordingly\n"
    " all experience hath shewn, that mankind are more disposed to suffer,\n"
    " while evils are sufferable, than to right themselves by abolishing\n"
    " the forms to which they are accustomed.  But when a long train of\n"
    " abuses and usurpations, pursuing invariably the same Object evinces a\n"
    " design to reduce them under absolute Despotism, it is their right, it\n"
    " is their duty, to throw off such Government, and to provide new\n"
    " Guards for their future security.--Such has been the patient\n"
    " sufferance of these Colonies; and such is now the necessity which\n"
    " constrains them to alter their former Systems of Government.  The\n"
    " history of the present King of Great Britain is a history of repeated\n"
    " injuries and usurpations, all having in direct object the\n"
    " establishment of an absolute Tyranny over these States.  To prove\n"
    " this, let Facts be submitted to a candid world.\n"
    "\n"
    ": o He has refused his Assent to Laws, the most wholesome and\n"
    ":   necessary for the public good.\n"
    ":\n"
    ": o He has forbidden his Governors to pass Laws of immediate and\n"
    ":   pressing importance, unless suspended in their operation till his\n"
    ":   Assent should be obtained; and when so suspended, he has utterly\n"
    ":   neglected to attend to them.\n"
    ":\n"
    ": o He has refused to pass other Laws for the accommodation of large\n"
    ":   districts of people, unless those people would relinquish the\n"
    ":   right of Representation in the Legislature, a right inestimable to\n"
    ":   them and formidable to tyrants only.\n"
    ":\n"
    ": o He has called together legislative bodies at places unusual,\n"
    ":   uncomfortable, and distant from the depository of their public\n"
    ":   Records, for the sole purpose of fatiguing them into compliance\n"
    ":   with his measures.\n"
    ":\n"
    ": o He has dissolved Representative Houses repeatedly, for opposing\n"
    ":   with manly firmness his invasions on the rights of the people.\n"
    ":\n"
    ": o He has refused for a long time, after such dissolutions, to cause\n"
    ":   others to be elected; whereby the Legislative powers, incapable of\n"
    ":   Annihilation, have returned to the People at large for their\n"
    ":   exercise; the State remaining in the mean time exposed to all the\n"
    ":   dangers of invasion from without, and convulsions within.\n"
    ":\n"
    ": o He has endeavoured to prevent the population of these States; for\n"
    ":   that purpose obstructing the Laws for Naturalization of\n"
    ":   Foreigners; refusing to pass others to encourage their migrations\n"
    ":   hither, and raising the conditions of new Appropriations of Lands.\n"
    ":\n"
    ": o He has obstructed the Administration of Justice, by refusing his\n"
    ":   Assent to Laws for establishing Judiciary powers.\n"
    ":\n"
    ": o He has made Judges dependent on his Will alone, for the tenure of\n"
    ":   their offices, and the amount and payment of their salaries.\n"
    ":\n"
    ": o He has erected a multitude of New Offices, and sent hither swarms\n"
    ":   of Officers to harrass our people, and eat out their substance.\n"
    ":\n"
    ": o He has kept among us, in times of peace, Standing Armies without\n"
    ":   the Consent of our legislatures.\n"
    ":\n"
    ": o He has affected to render the Military independent of and superior\n"
    ":   to the Civil power.\n"
    ":\n"
    ": o He has combined with others to subject us to a jurisdiction\n"
    ":   foreign to our constitution, and unacknowledged by our laws;\n"
    ":   giving his Assent to their Acts of pretended Legislation:\n"
    ":\n"
    ": o For Quartering large bodies of armed troops among us: For\n"
    ":   protecting them, by a mock Trial, from punishment for any Murders\n"
    ":   which they should commit on the Inhabitants of these States:\n"
    ":\n"
    ": o For cutting off our Trade with all parts of the world:\n"
    ":\n"
    ": o For imposing Taxes on us without our Consent: For depriving us in\n"
    ":   many cases, of the benefits of Trial by Jury:\n"
    ":\n"
    ": o For transporting us beyond Seas to be tried for pretended offences\n"
    ":\n"
    ": o For abolishing the free System of English Laws in a neighbouring\n"
    ":   Province, establishing therein an Arbitrary government, and\n"
    ":   enlarging its Boundaries so as to render it at once an example and\n"
    ":   fit instrument for introducing the same absolute rule into these\n"
    ":   Colonies:\n"
    ":\n"
    ": o For taking away our Charters, abolishing our most valuable Laws,\n"
    ":   and altering fundamentally the Forms of our Governments:\n"
    ":\n"
    ": o For suspending our own Legislatures, and declaring themselves\n"
    ":   invested with power to legislate for us in all cases whatsoever.\n"
    ":\n"
    ": o He has abdicated Government here, by declaring us out of his\n"
    ":   Protection and waging War against us.\n"
    ":\n"
    ": o He has plundered our seas, ravaged our Coasts, burnt our towns,\n"
    ":   and destroyed the lives of our people.  He is at this time\n"
    ":   transporting large Armies of foreign Mercenaries to compleat the\n"
    ":   works of death, desolation and tyranny, already begun with\n"
    ":   circumstances of Cruelty & perfidy scarcely paralleled in the most\n"
    ":   barbarous ages, and totally unworthy the Head of a civilized\n"
    ":   nation.\n"
    ":\n"
    ": o He has constrained our fellow Citizens taken Captive on the high\n"
    ":   Seas to bear Arms against their Country, to become the\n"
    ":   executioners of their friends and Brethren, or to fall themselves\n"
    ":   by their Hands.\n"
    ":\n"
    ": o He has excited domestic insurrections amongst us, and has\n"
    ":   endeavoured to bring on the inhabitants of our frontiers, the\n"
    ":   merciless Indian Savages, whose known rule of warfare, is an\n"
    ":   undistinguished destruction of all ages, sexes and conditions.\n"
    "\n"
    " In every stage of these Oppressions We have Petitioned for Redress in\n"
    " the most humble terms: Our repeated Petitions have been answered only\n"
    " by repeated injury.  A Prince whose character is thus marked by every\n"
    " act which may define a Tyrant, is unfit to be the ruler of a free\n"
    " people.\n"
    "\n"
    " Nor have We been wanting in attentions to our Brittish brethren.  We\n"
    " have warned them from time to time of attempts by their legislature\n"
    " to extend an unwarrantable jurisdiction over us.  We have reminded\n"
    " them of the circumstances of our emigration and settlement here.  We\n"
    " have appealed to their native justice and magnanimity, and we have\n"
    " conjured them by the ties of our common kindred to disavow these\n"
    " usurpations, which, would inevitably interrupt our connections and\n"
    " correspondence.  They too have been deaf to the voice of justice and\n"
    " of consanguinity.  We must, therefore, acquiesce in the necessity,\n"
    " which denounces our Separation, and hold them, as we hold the rest of\n"
    " mankind, Enemies in War, in Peace Friends.\n"
    "\n"
    " We, therefore, the Representatives of the united States of America,\n"
    " in General Congress, Assembled, appealing to the Supreme Judge of the\n"
    " world for the rectitude of our intentions, do, in the Name, and by\n"
    " Authority of the good People of these Colonies, solemnly publish and\n"
    " declare, That these United Colonies are, and of Right ought to be\n"
    " Free and Independent States; that they are Absolved from all\n"
    " Allegiance to the British Crown, and that all political connection\n"
    " between them and the State of Great Britain, is and ought to be\n"
    " totally dissolved; and that as Free and Independent States, they have\n"
    " full Power to levy War, conclude Peace, contract Alliances, establish\n"
    " Commerce, and to do all other Acts and Things which Independent\n"
    " States may of right do.  And for the support of this Declaration,\n"
    " with a firm reliance on the protection of divine Providence, we\n"
    " mutually pledge to each other our Lives, our Fortunes and our sacred\n"
    " Honor.\n";

    static char document1[] =
    "/The Universal Declaration of Human Rights\n"
    "/-----------------------------------------\n"
    "/Preamble\n"
    "/ - - - -\n"
    " Whereas recognition of the inherent dignity and of the equal and\n"
    " inalienable rights of all members of the human family is the\n"
    " foundation of freedom, justice and peace in the world,\n"
    "\n"
    " Whereas disregard and contempt for human rights have resulted in\n"
    " barbarous acts which have outraged the conscience of mankind, and the\n"
    " advent of a world in which human beings shall enjoy freedom of speech\n"
    " and belief and freedom from fear and want has been proclaimed as the\n"
    " highest aspiration of the common people,\n"
    "\n"
    " Whereas it is essential, if man is not to be compelled to have\n"
    " recourse, as a last resort, to rebellion against tyranny and\n"
    " oppression, that human rights should be protected by the rule of law,\n"
    "\n"
    " Whereas it is essential to promote the development of friendly\n"
    " relations between nations,\n"
    "\n"
    " Whereas the peoples of the United Nations have in the Charter\n"
    " reaffirmed their faith in fundamental human rights, in the dignity\n"
    " and worth of the human person and in the equal rights of men and\n"
    " women and have determined to promote social progress and better\n"
    " standards of life in larger freedom,\n"
    "\n"
    " Whereas Member States have pledged themselves to achieve, in\n"
    " co-operation with the United Nations, the promotion of universal\n"
    " respect for and observance of human rights and fundamental freedoms,\n"
    "\n"
    " Whereas a common understanding of these rights and freedoms is of the\n"
    " greatest importance for the full realization of this pledge, Now,\n"
    " Therefore THE GENERAL ASSEMBLY proclaims THIS UNIVERSAL DECLARATION\n"
    " OF HUMAN RIGHTS as a common standard of achievement for all peoples\n"
    " and all nations, to the end that every individual and every organ of\n"
    " society, keeping this Declaration constantly in mind, shall strive by\n"
    " teaching and education to promote respect for these rights and\n"
    " freedoms and by progressive measures, national and international, to\n"
    " secure their universal and effective recognition and observance, both\n"
    " among the peoples of Member States themselves and among the peoples\n"
    " of territories under their jurisdiction.\n"
    "\n"
    "/Article 1\n"
    "/- - - - -\n"
    " All human beings are born free and equal in dignity and rights.  They\n"
    " are endowed with reason and conscience and should act towards one\n"
    " another in a spirit of brotherhood.\n"
    "\n"
    "/Article 2\n"
    "/- - - - -\n"
    " Everyone is entitled to all the rights and freedoms set forth in this\n"
    " Declaration, without distinction of any kind, such as race, colour,\n"
    " sex, language, religion, political or other opinion, national or\n"
    " social origin, property, birth or other status.  Furthermore, no\n"
    " distinction shall be made on the basis of the political,\n"
    " jurisdictional or international status of the country or territory to\n"
    " which a person belongs, whether it be independent, trust,\n"
    " non-self-governing or under any other limitation of sovereignty.\n"
    "\n"
    "/Article 3\n"
    "/- - - - -\n"
    " Everyone has the right to life, liberty and security of person.\n"
    "\n"
    "/Article 4\n"
    "/- - - - -\n"
    " No one shall be held in slavery or servitude; slavery and the slave\n"
    " trade shall be prohibited in all their forms.\n"
    "\n"
    "/Article 5\n"
    "/- - - - -\n"
    " No one shall be subjected to torture or to cruel, inhuman or\n"
    " degrading treatment or punishment.\n"
    "\n"
    "/Article 6\n"
    "/- - - - -\n"
    " Everyone has the right to recognition everywhere as a person before\n"
    " the law.\n"
    "\n"
    "/Article 7\n"
    "/- - - - -\n"
    " All are equal before the law and are entitled without any\n"
    " discrimination to equal protection of the law.  All are entitled to\n"
    " equal protection against any discrimination in violation of this\n"
    " Declaration and against any incitement to such discrimination.\n"
    "\n"
    "/Article 8\n"
    "/- - - - -\n"
    " Everyone has the right to an effective remedy by the competent\n"
    " national tribunals for acts violating the fundamental rights granted\n"
    " him by the constitution or by law.\n"
    "\n"
    "/Article 9\n"
    "/- - - - -\n"
    " No one shall be subjected to arbitrary arrest, detention or exile.\n"
    "\n"
    "/Article 10\n"
    "/ - - - - -\n"
    " Everyone is entitled in full equality to a fair and public hearing by\n"
    " an independent and impartial tribunal, in the determination of his\n"
    " rights and obligations and of any criminal charge against him.\n"
    "\n"
    "/Article 11\n"
    "/ - - - - -\n"
    ": 1 Everyone charged with a penal offence has the right to be presumed\n"
    ":   innocent until proved guilty according to law in a public trial at\n"
    ":   which he has had all the guarantees necessary for his defence.\n"
    ":\n"
    ": 2 No one shall be held guilty of any penal offence on account of any\n"
    ":   act or omission which did not constitute a penal offence, under\n"
    ":   national or international law, at the time when it was committed.\n"
    ":   Nor shall a heavier penalty be imposed than the one that was\n"
    ":   applicable at the time the penal offence was committed.\n"
    "\n"
    "/Article 12\n"
    "/ - - - - -\n"
    " No one shall be subjected to arbitrary interference with his privacy,\n"
    " family, home or correspondence, nor to attacks upon his honour and\n"
    " reputation.  Everyone has the right to the protection of the law\n"
    " against such interference or attacks.\n"
    "\n"
    "/Article 13\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to freedom of movement and residence within\n"
    ":   the borders of each state.\n"
    ":\n"
    ": 2 Everyone has the right to leave any country, including his own,\n"
    ":   and to return to his country.\n"
    "\n"
    "/Article 14\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to seek and to enjoy in other countries\n"
    ":   asylum from persecution.\n"
    ":\n"
    ": 2 This right may not be invoked in the case of prosecutions\n"
    ":   genuinely arising from non-political crimes or from acts contrary\n"
    ":   to the purposes and principles of the United Nations.\n"
    "\n"
    "/Article 15\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to a nationality.\n"
    ":\n"
    ": 2 No one shall be arbitrarily deprived of his nationality nor denied\n"
    ":   the right to change his nationality.\n"
    "\n"
    "/Article 16\n"
    "/ - - - - -\n"
    ": 1 Men and women of full age, without any limitation due to race,\n"
    ":   nationality or religion, have the right to marry and to found a\n"
    ":   family.  They are entitled to equal rights as to marriage, during\n"
    ":   marriage and at its dissolution.\n"
    ":\n"
    ": 2 Marriage shall be entered into only with the free and full consent\n"
    ":   of the intending spouses.\n"
    ":\n"
    ": 3 The family is the natural and fundamental group unit of society\n"
    ":   and is entitled to protection by society and the State.\n"
    "\n"
    "/Article 17\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to own property alone as well as in\n"
    ":   association with others.\n"
    ":\n"
    ": 2 No one shall be arbitrarily deprived of his property.\n"
    "\n"
    "/Article 18\n"
    "/ - - - - -\n"
    " Everyone has the right to freedom of thought, conscience and\n"
    " religion; this right includes freedom to change his religion or\n"
    " belief, and freedom, either alone or in community with others and in\n"
    " public or private, to manifest his religion or belief in teaching,\n"
    " practice, worship and observance.\n"
    "\n"
    "/Article 19\n"
    "/ - - - - -\n"
    " Everyone has the right to freedom of opinion and expression; this\n"
    " right includes freedom to hold opinions without interference and to\n"
    " seek, receive and impart information and ideas through any media and\n"
    " regardless of frontiers.\n"
    "\n"
    "/Article 20\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and\n"
    ":   association.\n"
    ":\n"
    ": 2 No one may be compelled to belong to an association.\n"
    "\n"
    "/Article 21\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to take part in the government of his\n"
    ":   country, directly or through freely chosen representatives.\n"
    ":\n"
    ": 2 Everyone has the right of equal access to public service in his\n"
    ":   country.\n"
    ":\n"
    ": 3 The will of the people shall be the basis of the authority of\n"
    ":   government; this will shall be expressed in periodic and genuine\n"
    ":   elections which shall be by universal and equal suffrage and shall\n"
    ":   be held by secret vote or by equivalent free voting procedures.\n"
    "\n"
    "/Article 22\n"
    "/ - - - - -\n"
    " Everyone, as a member of society, has the right to social security\n"
    " and is entitled to realization, through national effort and\n"
    " international co-operation and in accordance with the organization\n"
    " and resources of each State, of the economic, social and cultural\n"
    " rights indispensable for his dignity and the free development of his\n"
    " personality.\n"
    "\n"
    "/Article 23\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to work, to free choice of employment, to\n"
    ":   just and favourable conditions of work and to protection against\n"
    ":   unemployment.\n"
    ":\n"
    ": 2 Everyone, without any discrimination, has the right to equal pay\n"
    ":   for equal work.\n"
    ":\n"
    ": 3 Everyone who works has the right to just and favourable\n"
    ":   remuneration ensuring for himself and his family an existence\n"
    ":   worthy of human dignity, and supplemented, if necessary, by other\n"
    ":   means of social protection.\n"
    ":\n"
    ": 4 Everyone has the right to form and to join trade unions for the\n"
    ":   protection of his interests.\n"
    "\n"
    "/Article 24\n"
    "/ - - - - -\n"
    " Everyone has the right to rest and leisure, including reasonable\n"
    " limitation of working hours and periodic holidays with pay.\n"
    "\n"
    "/Article 25\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to a standard of living adequate for the\n"
    ":   health and well-being of himself and of his family, including\n"
    ":   food, clothing, housing and medical care and necessary social\n"
    ":   services, and the right to security in the event of unemployment,\n"
    ":   sickness, disability, widowhood, old age or other lack of\n"
    ":   livelihood in circumstances beyond his control.\n"
    ":\n"
    ": 2 Motherhood and childhood are entitled to special care and\n"
    ":   assistance.  All children, whether born in or out of wedlock,\n"
    ":   shall enjoy the same social protection.\n"
    "\n"
    "/Article 26\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right to education.  Education shall be free, at\n"
    ":   least in the elementary and fundamental stages.  Elementary\n"
    ":   education shall be compulsory.  Technical and professional\n"
    ":   education shall be made generally available and higher education\n"
    ":   shall be equally accessible to all on the basis of merit.\n"
    ":\n"
    ": 2 Education shall be directed to the full development of the human\n"
    ":   personality and to the strengthening of respect for human rights\n"
    ":   and fundamental freedoms.  It shall promote understanding,\n"
    ":   tolerance and friendship among all nations, racial or religious\n"
    ":   groups, and shall further the activities of the United Nations for\n"
    ":   the maintenance of peace.\n"
    ":\n"
    ": 3 Parents have a prior right to choose the kind of education that\n"
    ":   shall be given to their children.\n"
    "\n"
    "/Article 27\n"
    "/ - - - - -\n"
    ": 1 Everyone has the right freely to participate in the cultural life\n"
    ":   of the community, to enjoy the arts and to share in scientific\n"
    ":   advancement and its benefits.\n"
    ":\n"
    ": 2 Everyone has the right to the protection of the moral and material\n"
    ":   interests resulting from any scientific, literary or artistic\n"
    ":   production of which he is the author.\n"
    "\n"
    "/Article 28\n"
    "/ - - - - -\n"
    " Everyone is entitled to a social and international order in which the\n"
    " rights and freedoms set forth in this Declaration can be fully\n"
    " realized.\n"
    "\n"
    "/Article 29\n"
    "/ - - - - -\n"
    ": 1 Everyone has duties to the community in which alone the free and\n"
    ":   full development of his personality is possible.\n"
    ":\n"
    ": 2 In the exercise of his rights and freedoms, everyone shall be\n"
    ":   subject only to such limitations as are determined by law solely\n"
    ":   for the purpose of securing due recognition and respect for the\n"
    ":   rights and freedoms of others and of meeting the just requirements\n"
    ":   of morality, public order and the general welfare in a democratic\n"
    ":   society.\n"
    ":\n"
    ": 3 These rights and freedoms may in no case be exercised contrary to\n"
    ":   the purposes and principles of the United Nations.\n"
    "\n"
    "/Article 30\n"
    "/ - - - - -\n"
    " Nothing in this Declaration may be interpreted as implying for any\n"
    " State, group or person any right to engage in any activity or to\n"
    " perform any act aimed at the destruction of any of the rights and\n"
    " freedoms set forth herein.\n";

    static char document2[] =
    "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
    "/---------------------------------------------------\n"
    " PREAMBLE\n"
    "\n"
    " The peoples of Europe, in creating an ever closer union among them,\n"
    " are resolved to share a peaceful future based on common values.\n"
    "\n"
    " Conscious of its spiritual and moral heritage, the Union is founded\n"
    " on the indivisible, universal values of human dignity, freedom,\n"
    " equality and solidarity; it is based on the principles of democracy\n"
    " and the rule of law.  It places the individual at the heart of its\n"
    " activities, by establishing the citizenship of the Union and by\n"
    " creating an area of freedom, security and justice.\n"
    "\n"
    " The Union contributes to the preservation and to the development of\n"
    " these common values while respecting the diversity of the cultures\n"
    " and traditions of the peoples of Europe as well as the national\n"
    " identities of the Member States and the organisation of their public\n"
    " authorities at national, regional and local levels; it seeks to\n"
    " promote balanced and sustainable development and ensures free\n"
    " movement of persons, goods, services and capital, and the freedom of\n"
    " establishment.\n"
    "\n"
    " To this end, it is necessary to strengthen the protection of\n"
    " fundamental rights in the light of changes in society, social\n"
    " progress and scientific and technological developments by making\n"
    " those rights more visible in a Charter.\n"
    "\n"
    " This Charter reaffirms, with due regard for the powers and tasks of\n"
    " the Community and the Union and the principle of subsidiarity, the\n"
    " rights as they result, in particular, from the constitutional\n"
    " traditions and international obligations common to the Member States,\n"
    " the Treaty on European Union, the Community Treaties, the European\n"
    " Convention for the Protection of Human Rights and Fundamental\n"
    " Freedoms, the Social Charters adopted by the Community and by the\n"
    " Council of Europe and the case-law of the Court of Justice of the\n"
    " European Communities and of the European Court of Human Rights.\n"
    "\n"
    " Enjoyment of these rights entails responsibilities and duties with\n"
    " regard to other persons, to the human community and to future\n"
    " generations.\n"
    "\n"
    " The Union therefore recognises the rights, freedoms and principles\n"
    " set out hereafter.\n"
    "\n"
    "/CHAPTER I\n"
    "/- - - - -\n"
    " DIGNITY\n"
    "\n"
    "/Article 1\n"
    "/  -  -  -\n"
    " Human dignity\n"
    "\n"
    " Human dignity is inviolable.  It must be respected and protected.\n"
    "\n"
    "/Article 2\n"
    "/  -  -  -\n"
    " Right to life\n"
    "\n"
    ": 1 Everyone has the right to life.\n"
    ": 2 No one shall be condemned to the death penalty, or executed.\n"
    "\n"
    "/Article 3\n"
    "/  -  -  -\n"
    " Right to the integrity of the person\n"
    "\n"
    ": 1 Everyone has the right to respect for his or her physical and\n"
    ":   mental integrity.\n"
    ":\n"
    ": 2 In the fields of medicine and biology, the following must be\n"
    ":   respected in particular:\n"
    ":\n"
    ":   o the free and informed consent of the person concerned, according\n"
    ":     to the procedures laid down by law,\n"
    ":\n"
    ":   o the prohibition of eugenic practices, in particular those aiming\n"
    ":     at the selection of persons,\n"
    ":\n"
    ":   o the prohibition on making the human body and its parts as such a\n"
    ":     source of financial gain,\n"
    ":\n"
    ":   o the prohibition of the reproductive cloning of human beings.\n"
    "\n"
    "/Article 4\n"
    "/  -  -  -\n"
    " Prohibition of torture and inhuman or degrading treatment or\n"
    " punishment\n"
    "\n"
    " No one shall be subjected to torture or to inhuman or degrading\n"
    " treatment or punishment.\n"
    "\n"
    "/Article 5\n"
    "/  -  -  -\n"
    " Prohibition of slavery and forced labour\n"
    "\n"
    ": 1 No one shall be held in slavery or servitude.\n"
    ": 2 No one shall be required to perform forced or compulsory labour.\n"
    ": 3 Trafficking in human beings is prohibited.\n"
    "\n"
    "/CHAPTER II\n"
    "/ - - - - -\n"
    " FREEDOMS\n"
    "\n"
    "/Article 6\n"
    "/  -  -  -\n"
    " Right to liberty and security\n"
    "\n"
    " Everyone has the right to liberty and security of person.\n"
    "\n"
    "/Article 7\n"
    "/  -  -  -\n"
    " Respect for private and family life\n"
    "\n"
    " Everyone has the right to respect for his or her private and family\n"
    " life, home and communications.\n"
    "\n"
    "/Article 8\n"
    "/  -  -  -\n"
    " Protection of personal data\n"
    "\n"
    ": 1 Everyone has the right to the protection of personal data\n"
    ":   concerning him or her.\n"
    ":\n"
    ": 2 Such data must be processed fairly for specified purposes and on\n"
    ":   the basis of the consent of the person concerned or some other\n"
    ":   legitimate basis laid down by law.  Everyone has the right of\n"
    ":   access to data which has been collected concerning him or her, and\n"
    ":   the right to have it rectified.\n"
    ":\n"
    ": 3 Compliance with these rules shall be subject to control by an\n"
    ":   independent authority.\n"
    "\n"
    "/Article 9\n"
    "/  -  -  -\n"
    " Right to marry and right to found a family\n"
    "\n"
    " The right to marry and the right to found a family shall be\n"
    " guaranteed in accordance with the national laws governing the\n"
    " exercise of these rights.\n"
    "\n"
    "/Article 10\n"
    "/-  -  -  -\n"
    " Freedom of thought, conscience and religion\n"
    "\n"
    ": 1 Everyone has the right to freedom of thought, conscience and\n"
    ":   religion.  This right includes freedom to change religion or\n"
    ":   belief and freedom, either alone or in community with others and\n"
    ":   in public or in private, to manifest religion or belief, in\n"
    ":   worship, teaching, practice and observance.\n"
    ":\n"
    ": 2 The right to conscientious objection is recognised, in accordance\n"
    ":   with the national laws governing the exercise of this right.\n"
    "\n"
    "/Article 11\n"
    "/-  -  -  -\n"
    " Freedom of expression and information\n"
    "\n"
    ": 1 Everyone has the right to freedom of expression.  This right shall\n"
    ":   include freedom to hold opinions and to receive and impart\n"
    ":   information and ideas without interference by public authority and\n"
    ":   regardless of frontiers.\n"
    ":\n"
    ": 2 The freedom and pluralism of the media shall be respected.\n"
    "\n"
    "/Article 12\n"
    "/-  -  -  -\n"
    " Freedom of assembly and of association\n"
    "\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and to\n"
    ":   freedom of association at all levels, in particular in political,\n"
    ":   trade union and civic matters, which implies the right of everyone\n"
    ":   to form and to join trade unions for the protection of his or her\n"
    ":   interests.\n"
    ":\n"
    ": 2 Political parties at Union level contribute to expressing the\n"
    ":   political will of the citizens of the Union.\n"
    "\n"
    "/Article 13\n"
    "/-  -  -  -\n"
    " Freedom of the arts and sciences\n"
    "\n"
    " The arts and scientific research shall be free of constraint.\n"
    " Academic freedom shall be respected.\n"
    "\n"
    "/Article 14\n"
    "/-  -  -  -\n"
    " Right to education\n"
    "\n"
    ": 1 Everyone has the right to education and to have access to\n"
    ":   vocational and continuing training.\n"
    ":\n"
    ": 2 This right includes the possibility to receive free compulsory\n"
    ":   education.\n"
    ":\n"
    ": 3 The freedom to found educational establishments with due respect\n"
    ":   for democratic principles and the right of parents to ensure the\n"
    ":   education and teaching of their children in conformity with their\n"
    ":   religious, philosophical and pedagogical convictions shall be\n"
    ":   respected, in accordance with the national laws governing the\n"
    ":   exercise of such freedom and right.\n"
    "\n"
    "/Article 15\n"
    "/-  -  -  -\n"
    " Freedom to choose an occupation and right to engage in work\n"
    "\n"
    ": 1 Everyone has the right to engage in work and to pursue a freely\n"
    ":   chosen or accepted occupation.\n"
    ":\n"
    ": 2 Every citizen of the Union has the freedom to seek employment, to\n"
    ":   work, to exercise the right of establishment and to provide\n"
    ":   services in any Member State.\n"
    ":\n"
    ": 3 Nationals of third countries who are authorised to work in the\n"
    ":   territories of the Member States are entitled to working\n"
    ":   conditions equivalent to those of citizens of the Union.\n"
    "\n"
    "/Article 16\n"
    "/-  -  -  -\n"
    " Freedom to conduct a business\n"
    "\n"
    " The freedom to conduct a business in accordance with Community law\n"
    " and national laws and practices is recognised.\n"
    "\n"
    "/Article 17\n"
    "/-  -  -  -\n"
    " Right to property\n"
    "\n"
    ": 1 Everyone has the right to own, use, dispose of and bequeath his or\n"
    ":   her lawfully acquired possessions.  No one may be deprived of his\n"
    ":   or her possessions, except in the public interest and in the cases\n"
    ":   and under the conditions provided for by law, subject to fair\n"
    ":   compensation being paid in good time for their loss.  The use of\n"
    ":   property may be regulated by law in so far as is necessary for the\n"
    ":   general interest.\n"
    ":\n"
    ": 2 Intellectual property shall be protected.\n"
    "\n"
    "/Article 18\n"
    "/-  -  -  -\n"
    " Right to asylum\n"
    "\n"
    " The right to asylum shall be guaranteed with due respect for the\n"
    " rules of the Geneva Convention of 28 July 1951 and the Protocol of 31\n"
    " January 1967 relating to the status of refugees and in accordance\n"
    " with the Treaty establishing the European Community.\n"
    "\n"
    "/Article 19\n"
    "/-  -  -  -\n"
    " Protection in the event of removal, expulsion or extradition\n"
    "\n"
    ": 1 Collective expulsions are prohibited.\n"
    ":\n"
    ": 2 No one may be removed, expelled or extradited to a State where\n"
    ":   there is a serious risk that he or she would be subjected to the\n"
    ":   death penalty, torture or other inhuman or degrading treatment or\n"
    ":   punishment.\n"
    "\n"
    "/CHAPTER III\n"
    "/- - - - - -\n"
    " EQUALITY\n"
    "\n"
    "/Article 20\n"
    "/-  -  -  -\n"
    " Equality before the law\n"
    "\n"
    " Everyone is equal before the law.\n"
    "\n"
    "/Article 21\n"
    "/-  -  -  -\n"
    " Non-discrimination\n"
    "\n"
    ": 1 Any discrimination based on any ground such as sex, race, colour,\n"
    ":   ethnic or social origin, genetic features, language, religion or\n"
    ":   belief, political or any other opinion, membership of a national\n"
    ":   minority, property, birth, disability, age or sexual orientation\n"
    ":   shall be prohibited.\n"
    ":\n"
    ": 2 Within the scope of application of the Treaty establishing the\n"
    ":   European Community and of the Treaty on European Union, and\n"
    ":   without prejudice to the special provisions of those Treaties, any\n"
    ":   discrimination on grounds of nationality shall be prohibited.\n"
    "\n"
    "/Article 22\n"
    "/-  -  -  -\n"
    " Cultural, religious and linguistic diversity\n"
    "\n"
    " The Union shall respect cultural, religious and linguistic diversity.\n"
    "\n"
    "/Article 23\n"
    "/-  -  -  -\n"
    " Equality between men and women\n"
    "\n"
    " Equality between men and women must be ensured in all areas,\n"
    " including employment, work and pay.  The principle of equality shall\n"
    " not prevent the maintenance or adoption of measures providing for\n"
    " specific advantages in favour of the under-represented sex.\n"
    "\n"
    "/Article 24\n"
    "/-  -  -  -\n"
    " The rights of the child\n"
    "\n"
    ": 1 Children shall have the right to such protection and care as is\n"
    ":   necessary for their well-being.  They may express their views\n"
    ":   freely.  Such views shall be taken into consideration on matters\n"
    ":   which concern them in accordance with their age and maturity.\n"
    ":\n"
    ": 2 In all actions relating to children, whether taken by public\n"
    ":   authorities or private institutions, the child's best interests\n"
    ":   must be a primary consideration.\n"
    ":\n"
    ": 3 Every child shall have the right to maintain on a regular basis a\n"
    ":   personal relationship and direct contact with both his or her\n"
    ":   parents, unless that is contrary to his or her interests.\n"
    "\n"
    "/Article 25\n"
    "/-  -  -  -\n"
    " The rights of the elderly\n"
    "\n"
    " The Union recognises and respects the rights of the elderly to lead a\n"
    " life of dignity and independence and to participate in social and\n"
    " cultural life.\n"
    "\n"
    "/Article 26\n"
    "/-  -  -  -\n"
    " Integration of persons with disabilities\n"
    "\n"
    " The Union recognises and respects the right of persons with\n"
    " disabilities to benefit from measures designed to ensure their\n"
    " independence, social and occupational integration and participation\n"
    " in the life of the community.\n"
    "\n"
    "/CHAPTER IV\n"
    "/ - - - - -\n"
    " SOLIDARITY\n"
    "\n"
    "/Article 27\n"
    "/-  -  -  -\n"
    " Workers' right to information and consultation within the undertaking\n"
    "\n"
    " Workers or their representatives must, at the appropriate levels, be\n"
    " guaranteed information and consultation in good time in the cases and\n"
    " under the conditions provided for by Community law and national laws\n"
    " and practices.\n"
    "\n"
    "/Article 28\n"
    "/-  -  -  -\n"
    " Right of collective bargaining and action\n"
    "\n"
    " Workers and employers, or their respective organisations, have, in\n"
    " accordance with Community law and national laws and practices, the\n"
    " right to negotiate and conclude collective agreements at the\n"
    " appropriate levels and, in cases of conflicts of interest, to take\n"
    " collective action to defend their interests, including strike action.\n"
    "\n"
    "/Article 29\n"
    "/-  -  -  -\n"
    " Right of access to placement services\n"
    "\n"
    " Everyone has the right of access to a free placement service.\n"
    "\n"
    "/Article 30\n"
    "/-  -  -  -\n"
    " Protection in the event of unjustified dismissal\n"
    "\n"
    " Every worker has the right to protection against unjustified\n"
    " dismissal, in accordance with Community law and national laws and\n"
    " practices.\n"
    "\n"
    "/Article 31\n"
    "/-  -  -  -\n"
    " Fair and just working conditions\n"
    "\n"
    ": 1 Every worker has the right to working conditions which respect his\n"
    ":   or her health, safety and dignity.\n"
    ":\n"
    ": 2 Every worker has the right to limitation of maximum working hours,\n"
    ":   to daily and weekly rest periods and to an annual period of paid\n"
    ":   leave.\n"
    "\n"
    "/Article 32\n"
    "/-  -  -  -\n"
    " Prohibition of child labour and protection of young people at work\n"
    "\n"
    " The employment of children is prohibited.  The minimum age of\n"
    " admission to employment may not be lower than the minimum\n"
    " school-leaving age, without prejudice to such rules as may be more\n"
    " favourable to young people and except for limited derogations.  Young\n"
    " people admitted to work must have working conditions appropriate to\n"
    " their age and be protected against economic exploitation and any work\n"
    " likely to harm their safety, health or physical, mental, moral or\n"
    " social development or to interfere with their education.\n"
    "\n"
    "/Article 33\n"
    "/-  -  -  -\n"
    " Family and professional life\n"
    "\n"
    ": 1 The family shall enjoy legal, economic and social protection.\n"
    ":\n"
    ": 2 To reconcile family and professional life, everyone shall have the\n"
    ":   right to protection from dismissal for a reason connected with\n"
    ":   maternity and the right to paid maternity leave and to parental\n"
    ":   leave following the birth or adoption of a child.\n"
    "\n"
    "/Article 34\n"
    "/-  -  -  -\n"
    " Social security and social assistance\n"
    "\n"
    ": 1 The Union recognises and respects the entitlement to social\n"
    ":   security benefits and social services providing protection in\n"
    ":   cases such as maternity, illness, industrial accidents, dependency\n"
    ":   or old age, and in the case of loss of employment, in accordance\n"
    ":   with the rules laid down by Community law and national laws and\n"
    ":   practices.\n"
    ":\n"
    ": 2 Everyone residing and moving legally within the European Union is\n"
    ":   entitled to social security benefits and social advantages in\n"
    ":   accordance with Community law and national laws and practices.\n"
    ":\n"
    ": 3 In order to combat social exclusion and poverty, the Union\n"
    ":   recognises and respects the right to social and housing assistance\n"
    ":   so as to ensure a decent existence for all those who lack\n"
    ":   sufficient resources, in accordance with the rules laid down by\n"
    ":   Community law and national laws and practices.\n"
    "\n"
    "/Article 35\n"
    "/-  -  -  -\n"
    " Health care\n"
    "\n"
    " Everyone has the right of access to preventive health care and the\n"
    " right to benefit from medical treatment under the conditions\n"
    " established by national laws and practices.  A high level of human\n"
    " health protection shall be ensured in the definition and\n"
    " implementation of all Union policies and activities.\n"
    "\n"
    "/Article 36\n"
    "/-  -  -  -\n"
    " Access to services of general economic interest\n"
    "\n"
    " The Union recognises and respects access to services of general\n"
    " economic interest as provided for in national laws and practices, in\n"
    " accordance with the Treaty establishing the European Community, in\n"
    " order to promote the social and territorial cohesion of the Union.\n"
    "\n"
    "/Article 37\n"
    "/-  -  -  -\n"
    " Environmental protection\n"
    "\n"
    " A high level of environmental protection and the improvement of the\n"
    " quality of the environment must be integrated into the policies of\n"
    " the Union and ensured in accordance with the principle of sustainable\n"
    " development.\n"
    "\n"
    "/Article 38\n"
    "/-  -  -  -\n"
    " Consumer protection\n"
    "\n"
    " Union policies shall ensure a high level of consumer protection.\n"
    "\n"
    "/CHAPTER V\n"
    "/- - - - -\n"
    " CITIZENS' RIGHTS\n"
    "\n"
    "/Article 39\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at elections to the\n"
    " European Parliament\n"
    "\n"
    ": 1 Every citizen of the Union has the right to vote and to stand as a\n"
    ":   candidate at elections to the European Parliament in the Member\n"
    ":   State in which he or she resides, under the same conditions as\n"
    ":   nationals of that State.\n"
    ":\n"
    ": 2 Members of the European Parliament shall be elected by direct\n"
    ":   universal suffrage in a free and secret ballot.\n"
    "\n"
    "/Article 40\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at municipal elections\n"
    "\n"
    " Every citizen of the Union has the right to vote and to stand as a\n"
    " candidate at municipal elections in the Member State in which he or\n"
    " she resides under the same conditions as nationals of that State.\n"
    "\n"
    "/Article 41\n"
    "/-  -  -  -\n"
    " Right to good administration\n"
    "\n"
    ": 1 Every person has the right to have his or her affairs handled\n"
    ":   impartially, fairly and within a reasonable time by the\n"
    ":   institutions and bodies of the Union.\n"
    ":\n"
    ": 2 This right includes:\n"
    ":\n"
    ":   o the right of every person to be heard, before any individual\n"
    ":     measure which would affect him or her adversely is taken;\n"
    ":\n"
    ":   o the right of every person to have access to his or her file,\n"
    ":     while respecting the legitimate interests of confidentiality and\n"
    ":     of professional and business secrecy;\n"
    ":\n"
    ":   o the obligation of the administration to give reasons for its\n"
    ":     decisions.\n"
    ":\n"
    ": 3 Every person has the right to have the Community make good any\n"
    ":   damage caused by its institutions or by its servants in the\n"
    ":   performance of their duties, in accordance with the general\n"
    ":   principles common to the laws of the Member States.\n"
    ":\n"
    ": 4 Every person may write to the institutions of the Union in one of\n"
    ":   the languages of the Treaties and must have an answer in the same\n"
    ":   language.\n"
    "\n"
    "/Article 42\n"
    "/-  -  -  -\n"
    " Right of access to documents\n"
    "\n"
    " Any citizen of the Union, and any natural or legal person residing or\n"
    " having its registered office in a Member State, has a right of access\n"
    " to European Parliament, Council and Commission documents.\n"
    "\n"
    "/Article 43\n"
    "/-  -  -  -\n"
    " Ombudsman\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to refer\n"
    " to the Ombudsman of the Union cases of maladministration in the\n"
    " activities of the Community institutions or bodies, with the\n"
    " exception of the Court of Justice and the Court of First Instance\n"
    " acting in their judicial role.\n"
    "\n"
    "/Article 44\n"
    "/-  -  -  -\n"
    " Right to petition\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to\n"
    " petition the European Parliament.\n"
    "\n"
    "/Article 45\n"
    "/-  -  -  -\n"
    " Freedom of movement and of residence\n"
    "\n"
    ": 1 Every citizen of the Union has the right to move and reside freely\n"
    ":   within the territory of the Member States.\n"
    ":\n"
    ": 2 Freedom of movement and residence may be granted, in accordance\n"
    ":   with the Treaty establishing the European Community, to nationals\n"
    ":   of third countries legally resident in the territory of a Member\n"
    ":   State.\n"
    "\n"
    "/Article 46\n"
    "/-  -  -  -\n"
    " Diplomatic and consular protection\n"
    "\n"
    " Every citizen of the Union shall, in the territory of a third country\n"
    " in which the Member State of which he or she is a national is not\n"
    " represented, be entitled to protection by the diplomatic or consular\n"
    " authorities of any Member State, on the same conditions as the\n"
    " nationals of that Member State.\n"
    "\n"
    "/CHAPTER VI\n"
    "/ - - - - -\n"
    " JUSTICE\n"
    "\n"
    "/Article 47\n"
    "/-  -  -  -\n"
    " Right to an effective remedy and to a fair trial\n"
    "\n"
    " Everyone whose rights and freedoms guaranteed by the law of the Union\n"
    " are violated has the right to an effective remedy before a tribunal\n"
    " in compliance with the conditions laid down in this Article.\n"
    " Everyone is entitled to a fair and public hearing within a reasonable\n"
    " time by an independent and impartial tribunal previously established\n"
    " by law.  Everyone shall have the possibility of being advised,\n"
    " defended and represented.  Legal aid shall be made available to those\n"
    " who lack sufficient resources in so far as such aid is necessary to\n"
    " ensure effective access to justice.\n"
    "\n"
    "/Article 48\n"
    "/-  -  -  -\n"
    " Presumption of innocence and right of defence\n"
    "\n"
    ": 1 Everyone who has been charged shall be presumed innocent until\n"
    ":   proved guilty according to law.\n"
    ":\n"
    ": 2 Respect for the rights of the defence of anyone who has been\n"
    ":   charged shall be guaranteed.\n"
    "\n"
    "/Article 49\n"
    "/-  -  -  -\n"
    " Principles of legality and proportionality of criminal offences and\n"
    " penalties\n"
    "\n"
    ": 1 No one shall be held guilty of any criminal offence on account of\n"
    ":   any act or omission which did not constitute a criminal offence\n"
    ":   under national law or international law at the time when it was\n"
    ":   committed.  Nor shall a heavier penalty be imposed than that which\n"
    ":   was applicable at the time the criminal offence was committed.\n"
    ":   If, subsequent to the commission of a criminal offence, the law\n"
    ":   provides for a lighter penalty, that penalty shall be applicable.\n"
    ":\n"
    ": 2 This Article shall not prejudice the trial and punishment of any\n"
    ":   person for any act or omission which, at the time when it was\n"
    ":   committed, was criminal according to the general principles\n"
    ":   recognised by the community of nations.\n"
    ":\n"
    ": 3 The severity of penalties must not be disproportionate to the\n"
    ":   criminal offence.\n"
    "\n"
    "/Article 50\n"
    "/-  -  -  -\n"
    " Right not to be tried or punished twice in criminal proceedings for\n"
    " the same criminal offence\n"
    "\n"
    " No one shall be liable to be tried or punished again in criminal\n"
    " proceedings for an offence for which he or she has already been\n"
    " finally acquitted or convicted within the Union in accordance with\n"
    " the law.\n"
    "\n"
    "/CHAPTER VII\n"
    "/- - - - - -\n"
    " GENERAL PROVISIONS\n"
    "\n"
    "/Article 51\n"
    "/-  -  -  -\n"
    " Scope\n"
    "\n"
    ": 1 The provisions of this Charter are addressed to the institutions\n"
    ":   and bodies of the Union with due regard for the principle of\n"
    ":   subsidiarity and to the Member States only when they are\n"
    ":   implementing Union law.  They shall therefore respect the rights,\n"
    ":   observe the principles and promote the application thereof in\n"
    ":   accordance with their respective powers.\n"
    ":\n"
    ": 2 This Charter does not establish any new power or task for the\n"
    ":   Community or the Union, or modify powers and tasks defined by the\n"
    ":   Treaties.\n"
    "\n"
    "/Article 52\n"
    "/-  -  -  -\n"
    " Scope of guaranteed rights\n"
    "\n"
    ": 1 Any limitation on the exercise of the rights and freedoms\n"
    ":   recognised by this Charter must be provided for by law and respect\n"
    ":   the essence of those rights and freedoms.  Subject to the\n"
    ":   principle of proportionality, limitations may be made only if they\n"
    ":   are necessary and genuinely meet objectives of general interest\n"
    ":   recognised by the Union or the need to protect the rights and\n"
    ":   freedoms of others.\n"
    ":\n"
    ": 2 Rights recognised by this Charter which are based on the Community\n"
    ":   Treaties or the Treaty on European Union shall be exercised under\n"
    ":   the conditions and within the limits defined by those Treaties.\n"
    ":\n"
    ": 3 In so far as this Charter contains rights which correspond to\n"
    ":   rights guaranteed by the Convention for the Protection of Human\n"
    ":   Rights and Fundamental Freedoms, the meaning and scope of those\n"
    ":   rights shall be the same as those laid down by the said\n"
    ":   Convention.  This provision shall not prevent Union law providing\n"
    ":   more extensive protection.\n"
    "\n"
    "/Article 53\n"
    "/-  -  -  -\n"
    " Level of protection\n"
    "\n"
    " Nothing in this Charter shall be interpreted as restricting or\n"
    " adversely affecting human rights and fundamental freedoms as\n"
    " recognised, in their respective fields of application, by Union law\n"
    " and international law and by international agreements to which the\n"
    " Union, the Community or all the Member States are party, including\n"
    " the European Convention for the Protection of Human Rights and\n"
    " Fundamental Freedoms, and by the Member States' constitutions.\n"
    "\n"
    "/Article 54\n"
    "/-  -  -  -\n"
    " Prohibition of abuse of rights\n"
    "\n"
    " Nothing in this Charter shall be interpreted as implying any right to\n"
    " engage in any activity or to perform any act aimed at the destruction\n"
    " of any of the rights and freedoms recognised in this Charter or at\n"
    " their limitation to a greater extent than is provided for herein.\n";

    static char * const documents[] = { &document0[0],
                                        &document1[0],
                                        &document2[0]
                                      };
    const int           numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define several aliases to make our code more comprehensible.
//..
    typedef bsl::pair<int, int>                  WordLocation;
        // Document code number and word offset in that document specify
        // a word location.

    typedef bsl::pair<bsl::string, WordLocation> ConcordanceValue;
    typedef bsl::unordered_multimap<bsl::string, WordLocation>
                                                 Concordance;
    typedef Concordance::iterator                ConcordanceItr;
//..
// Next, we (default) create an unordered map to hold our word tallies.
//..
    Concordance concordance;
//..
// Then, we define the set of characters that define word boundaries:
//..
    const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// As each word is located, we create a map value -- a pair of the word
// converted to a 'bsl::string' and a 'WordLocation' object (itself a pair of
// document code and (word) offset of that word in the document) -- and insert
// the map value into the map.  Note that (unlike maps and unordered maps)
// there is no status to check; the insertion succeeds even if the key is
// already present in the (multi) map.
//..
    for (int idx = 0; idx < numDocuments; ++idx) {
        int wordCount = 0;
        for (char *cur = strtok(documents[idx], delimiters);
                   cur;
                   cur = strtok(NULL,     delimiters)) {
            WordLocation     location(idx, wordCount++);
            ConcordanceValue value(bsl::string(cur), location);
            concordance.insert(value);
        }
    }
//..
// Then, we can readily print a complete concordance by interating through the
// map.
//..
    for (ConcordanceItr itr  = concordance.begin(),
                        end  = concordance.end();
                        end != itr; ++itr) {
if (verbose) {
        printf("\"%s\", %2d, %4d\n",
               itr->first.c_str(),
               itr->second.first,
               itr->second.second);
}
    }
//..
// Standard output shows:
//..
//  "extent",  2, 3837
//  "greater",  2, 3836
//  "abuse",  2, 3791
//  "constitutions",  2, 3782
//  "affecting",  2, 3727
//  ...
//  "he",  1, 1746
//  "he",  1,  714
//  "he",  0,  401
//  "include",  2,  847
//..
// Next, if we there are some particular words of interest, we seek them out
// using the 'equal_range' method of the 'concordance' object:
//..
    const char *wordsOfInterest[] = { "human",
                                      "rights",
                                      "unalienable",
                                      "inalienable"
                                    };
    const int   numWordsOfInterest = sizeof  wordsOfInterest
                                   / sizeof *wordsOfInterest;
    for (int idx = 0; idx < numWordsOfInterest; ++idx) {
       const char                                *wordOfInterest
                                                        = wordsOfInterest[idx];
       bsl::pair<ConcordanceItr, ConcordanceItr>  found =
                          concordance.equal_range(bsl::string(wordOfInterest));
       for (ConcordanceItr itr  = found.first,
                           end  = found.second;
                           end != itr; ++itr) {
if (verbose) {
           printf("\"%s\", %2d, %4d\n",
                  itr->first.c_str(),
                  itr->second.first,
                  itr->second.second);
}
       }
if (verbose) {
       printf("\n");
}
    }
//..
// Finally, we see on standard output:
//..
//  "human",  2, 3492
//  "human",  2, 2192
//  "human",  2,  534
//  ...
//  "human",  1,   65
//  "human",  1,   43
//  "human",  1,   25
//  "human",  0,   20
//
//  "rights",  2, 3583
//  "rights",  2, 3553
//  "rights",  2, 3493
//  ...
//  "rights",  1,   44
//  "rights",  1,   19
//  "rights",  0,  496
//  "rights",  0,  126
//
//  "unalienable",  0,  109
//
//  "inalienable",  1,   18
//
//..
// {'bslstl_unorderedmap'|Example 3} shows how to use the concordance to create
// an inverse concordance, and how to use the inverse concordance to find the
// context (surrouding words) of a word of interest.
//
///Example 2: Examining and Setting Unordered Multi-Map Configuration
///------------------------------------------------------------------
// The unordered multi-map interfaces provide some insight into and control of
// its inner workings.  The syntax and semantics of these interfaces for
// 'bslstl_unoroderedmultimap' are identical to those of 'bslstl_unorderedmap'.
// See the material in {'bslstl_unorderedmap'|Example 2}.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That basic functionality appears to work as advertised before
        //   before beginning testing in earnest:
        //   - default and copy constructors
        //   - assignment operator
        //   - primary manipulators, basic accessors
        //   - 'operator==', 'operator!='
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bsl::unordered_multimap<int, int > TestType;

        if (veryVerbose)
            printf("Default construct an unordered_mutlimap, 'x'\n");

        TestType mX;
        const TestType &x = mX;

        if (veryVerbose) printf("Validate default behavior of 'x'\n");

        ASSERT(1.0f == x.max_load_factor());

        testConstEmptyContainer(x);
        testEmptyContainer(mX);

        swap(mX, mX);

        testConstEmptyContainer(x);
        testEmptyContainer(mX);

        validateIteration(mX);

        if (veryVerbose) printf("Prepare a test array of data samples\n");

        typedef TestType::value_type BaseValue;
        const int MAX_SAMPLE = 1000;
        BaseValue *dataSamples = new BaseValue[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            new(&dataSamples[i]) BaseValue(i, i*i);  // inplace-new needed to
                                                     // supply 'const' key
        }

        if (veryVerbose)  printf(
          "Range-construct an unordered_multimap, 'y', from the test array\n");

        TestType mY(dataSamples, dataSamples + MAX_SAMPLE);
        const TestType &y = mY;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'y'\n");

        ASSERT(1.0f == y.max_load_factor());

        testContainerHasData(y, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mY);

        if (veryVerbose)
            printf("Assert equality relationships, noting 'x != y'\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y != x);
        ASSERT(!(y == x));
        ASSERT(x != y);
        ASSERT(!(x == y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        if (veryVerbose) printf("Swap 'x' and 'y'\n");

        swap(mX, mY);

        if (veryVerbose) printf("Validate swapped values\n");

        testConstEmptyContainer(y);
        testEmptyContainer(mY);

        validateIteration(mY);

        testContainerHasData(x, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mX);

        if (veryVerbose)
            printf("Assert swapped equality relationships, noting 'x != y'\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y != x);
        ASSERT(!(y == x));
        ASSERT(x != y);
        ASSERT(!(x == y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        if (veryVerbose) printf("Try to fill 'x' with duplicate values\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            TestType::iterator it = mX.insert(dataSamples[i]);
            ASSERT(x.end() != it);
            ASSERT(*it == dataSamples[i]);
        }
        validateIteration(mX);

        if (veryVerbose) printf(
          "Confirm the value of 'x' with the successfully inserted values.\n");

        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                "Create an unordered_multimap, 'z', that is a copy of 'x'.\n");

        TestType mZ = x;
        const TestType &z = mZ;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'z'.\n");

        ASSERT(1.0f == z.max_load_factor());
        ASSERT(x == z);
        ASSERT(!(x != z));
        ASSERT(z == x);
        ASSERT(!(z != x));
        testContainerHasData(z, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                             "Expand 'z' with additional duplicate values.\n");

        validateIteration(mZ);
        for (int i = 3; i != 6; ++i) {
            fillContainerWithData(mZ, dataSamples, MAX_SAMPLE);
            testContainerHasData(z, i, dataSamples, MAX_SAMPLE);
            validateIteration(mZ);
        }
        ASSERT(x != z);
        ASSERT(!(x == z));
        ASSERT(z != x);
        ASSERT(!(z == x));


        if (veryVerbose)
            printf("Confirm that 'x' is unchanged by making the copy.\n");

        testBuckets(mX);
        validateIteration(mX);
        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose)
            printf("Clear 'x' and confirm that it is empty.\n");

        mX.clear();
        testEmptyContainer(mX);
        testBuckets(mX);

        if (veryVerbose)
            printf("Assign the value of 'y' to 'x'.\n");

        mX = y;

        if (veryVerbose) printf("Confirm 'x' has the expected value.\n");

        ASSERT(x == y);

        validateIteration(mX);
        testBuckets(mX);

        testErase(mZ);

        if (veryVerbose)
            printf("Final message to confim the end of the breathing test.\n");
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
}  // Empty test driver

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
