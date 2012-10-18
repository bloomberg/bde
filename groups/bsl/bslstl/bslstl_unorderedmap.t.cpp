// bslstl_unorderedmap.t.cpp                                          -*-C++-*-

#include <bslstl_unorderedmap.h>

#include <bslstl_pair.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_stdtestallocator.h>

#include <algorithm>
#include <functional>
#include <stdexcept>   // to confirm that the contractual exceptions are thrown

#include <stdio.h>
#include <stdlib.h>
#include <cstring>  // for 'strtok'

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Initial breathing test iterates all operations with a single template
// instantiation and test obvious boundary conditions and iterator stability
// guarantees.
//-----------------------------------------------------------------------------
// [ ]
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [ ] USAGE EXAMPLE
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

#if defined BSLS_PLATFORM_CMP_MSVC
#  define ZU "%Iu"
#else
#  define ZU "%zu"
#endif

bool g_verbose;
bool g_veryVerbose;
bool g_veryVeryVerbose;
bool g_veryVeryVeryVerbose;
bool g_veryVeryVeryVeryVerbose;

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void debugPrint(const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC> TObj;
        typedef typename TObj::const_iterator CIter;
        typedef typename TObj::const_local_iterator LCIter;

        for (size_t n = 0; n < s.bucket_count(); ++n) {
            if (s.cbegin(n) == s.cend(n)) {
                continue;
            }
            printf("\nBucket ["ZU"]: ", n);
            for (LCIter lci = s.cbegin(n); lci != s.cend(n); ++lci) {
                printf("[%d, %d], ", lci->first, lci->second);

//              bsls::BslTestUtil::callDebugprint(
//                       static_cast<char>(
//                          bsltf::TemplateTestFacility::getIdentifier(*lci)));
            }
            printf("\n");
        }
    }
    fflush(stdout);
}


//=============================================================================
//    FREE FUNCTIONS FOR REVISED TESTING SCHEME
//    IMPLEMENTATION SHOULD BE (MOSTLY) CONTAINER INDEPENDENT
//=============================================================================

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
    LOOP_ASSERT(x.size(), 0 == x.size());
    LOOP_ASSERT(x.load_factor(), 0.f == x.load_factor());

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

    // should not assert

    typename TestType::iterator it = x.erase(x.begin(), x.end());
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
    typedef CONTAINER TestType;
    typedef typename TestType::size_type      SizeType;
    typedef typename TestType::const_iterator TestIterator;

    ASSERT(x.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        TestIterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(*it == data[i]);
        ASSERT(x.count(keyForValue<CONTAINER>(data[i])) == nCopies);

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
void fillContainerWithData(CONTAINER& x,
                           const typename CONTAINER::value_type *data,
                           int       size)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    SizeType initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (SizeType i = 0; i != size; ++i) {
        typename TestType::iterator it =
                                       x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(*it == data[i]);
    }
}

template<class CONTAINER>
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
    typedef typename             CONTAINER::size_type            SizeType;
    typedef typename             CONTAINER::iterator             iterator;
    typedef typename       CONTAINER::const_iterator       const_iterator;
    typedef typename       CONTAINER::local_iterator       local_iterator;
    typedef typename CONTAINER::const_local_iterator const_local_iterator;


    const CONTAINER &x = mX;

    SizeType bucketCount = x.bucket_count();
    SizeType collisions = 0;
    SizeType itemCount  = 0;

    for (SizeType i = 0; i != bucketCount; ++i ) {
        const SizeType count = x.bucket_size(i);
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
        SizeType bucketItems = 0;
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
    typedef typename CONTAINER::      size_type      SizeType;
    typedef typename CONTAINER::      iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const CONTAINER& x = mX;
    SizeType size = x.size();

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
    SizeType duplicates = x.count(key);
    ASSERT(0 != duplicates);  // already tested, just a sanity check
    bsl::pair<const_iterator, const_iterator> valRange = x.equal_range(key);
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
    ASSERT(mX.erase(valRange.first, valRange.second) == valRange.second);
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    // cIter is invalidated, so reset and start next sub-test
    // 10 range-erase all matching values for a given key value
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
    //ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    //ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
    SizeType erasures = 0;
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
    // LOOP2_ASSERT(
    //         key,
    //         cIter->first,
    //         x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*cIter)));
    // ASSERT(x.bucket(key) != x.bucket(keyForValue<CONTAINER>(*next)));
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


template <class CONTAINER>
void testMapLookup(CONTAINER& mX)
{
    typedef typename CONTAINER::      size_type      SizeType;
    typedef typename CONTAINER::      iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    typedef typename CONTAINER::   key_type    key_type;
    typedef typename CONTAINER::mapped_type mapped_type;

    const CONTAINER& x = mX;
    // Assume there are no default key-values.
    // Enforce assumption with an erase!
    if (mX.erase(key_type())) {
        // do we write a note that we just erased an element?
    }

    SizeType size = x.size();

    iterator it = mX.begin();
    key_type    key   = it->first;
    mapped_type value = it->second;
    ASSERT(value != mapped_type());  // value is not defaulted - known value

    ASSERT(x.at(key) == value);
    ASSERT(x.size() == size);

    ASSERT(mX[key] == value);
    ASSERT(x.size() == size);

    mX[key] = mapped_type();
    ASSERT(x.size() == size);

    ASSERT(x.at(key) == mapped_type());
    ASSERT(x.size() == size);

    ASSERT(it->second == mapped_type());
    ASSERT(mX[key] == mapped_type());
    ASSERT(x.size() == size);

    try {
        mapped_type v = x.at(key_type());
        ASSERT(false); // prior line should throw
        (void)v;       // resolve unused variable warning
    }
    catch(const std::exception&) {
        // expected code path
        // need to catch more specific exception though
    }

    try {
        mapped_type v = mX.at(key_type());
        ASSERT(false); // prior line should throw
        (void)v;       // resolve unused variable warning
    }
    catch(const std::exception&) {
        // expected code path
        // need to catch more specific exception though
    }

    ASSERT(mX[key_type()] == mapped_type());
    ASSERT(x.size() == ++size);  // want to see an error here before continuing

    try {
        mapped_type v = x.at(key_type());
        ASSERT(mapped_type() == v);
    }
    catch(const std::exception&) {
        ASSERT(false); // default key has been inserted, should not throw
    }

    try {
        mapped_type v = mX.at(key_type());
        ASSERT(mapped_type() == v);
    }
    catch(const std::exception&) {
        ASSERT(false); // default key has been inserted, should not throw
    }
}


template <class CONTAINER>
void testImplicitInsert(CONTAINER& mX)
{
    typedef typename CONTAINER::   key_type    key_type;
    typedef typename CONTAINER::mapped_type mapped_type;

    const CONTAINER& x = mX;
    ASSERT(x.empty());  // assumption on entry

    try {
        mapped_type v = x.at(key_type());
        ASSERT(false); // prior line should throw
        (void)v;       // resolve unused variable warning
    }
    catch(const std::out_of_range&) {
        // expected code path
        // need to catch more specific exception though
    }

    try {
        mapped_type v = mX.at(key_type());
        ASSERT(false); // prior line should throw
        (void)v;       // resolve unused variable warning
    }
    catch(const std::out_of_range&) {
        // expected code path
        // need to catch more specific exception though
    }

    ASSERT(mX[key_type()] == mapped_type());
    ASSERT(x.size() == 1);  // want to see an error here before continuing

    try {
        mapped_type v = x.at(key_type());
        ASSERT(mapped_type() == v);
    }
    catch(const std::exception&) {
        ASSERT(false); // default key has been inserted, should not throw
    }

    try {
        mapped_type v = mX.at(key_type());
        ASSERT(mapped_type() == v);
    }
    catch(const std::exception&) {
        ASSERT(false); // default key has been inserted, should not throw
    }
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
} // close namespace UsageExample

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

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
///Example 1: Gathering Document Statistics
/// - - - - - - - - - - - - - - - - - - - - 
// Unordered maps are useful in situations when there is no meaningful way to
// compare key values, when the order of the keys is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// maps (compared to ordered maps).
//
// Suppose one wished to gather statistics on the words appearing in a large
// set of documents on disk or in a data base.  Gathering those statics is
// intrusive (as one is competing for access to the documents with the regular
// users) and must be done as quickly as possible.  Moreover, the set of
// unique words appearing in those documents may be high. The English language
// has in excess of a million words (albeit many appear infrequently), and, if
// the documents contain serial numbers, or Social Security numbers, or
// chemical formulas, etc. then the O[log(n)] insertion time of ordered maps
// may well be inadequate.  The unordered map, having an O[1] average insersion
// cost, is a viable alternative.  In many problem domains, sorting, if needed,
// can be done after the data is gathered.
//
// This example illustrates the use of 'bsl::unordered_map' to gather one
// simple statistic (counts of unique words) on a single document.  To avoid
// irrelevant details of acquiring the data, two modestly sized documents are
// stored as a static arrays:
//..
    static char document0[] =
    " *IN* *CONGRESS*, *July* *4*, *1776*.\n"
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
    " Honor.\n"
    "\n"
    " Button Gwinnett Lyman Hall George Walton William Hooper Joseph Hewes\n"
    " John Penn Edward Rutledge Thomas Heyward, Jr.  Thomas Lynch, Jr.\n"
    " Arthur Middleton John Hancock Maryland: Samuel Chase William Paca\n"
    " Thomas Stone Charles Carroll of Carrollton George Wythe Richard Henry\n"
    " Lee Thomas Jefferson Benjamin Harrison Thomas Nelson, Jr.  Francis\n"
    " Lightfoot Lee Carter Braxton Robert Morris Benjamin Rush Benjamin\n"
    " Franklin John Morton George Clymer James Smith George Taylor James\n"
    " Wilson George Ross Caesar Rodney George Read Thomas McKean William\n"
    " Floyd Philip Livingston Francis Lewis Lewis Morris Richard Stockton\n"
    " John Witherspoon Francis Hopkinson John Hart Abraham Clark Josiah\n"
    " Bartlett William Whipple Samuel Adams John Adams Robert Treat Paine\n"
    " Elbridge Gerry Stephen Hopkins William Ellery Roger Sherman Samuel\n"
    " Huntington William Williams Oliver Wolcott Matthew Thornton\n";

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

    static char * const documents[] = { &document0[0], &document1[0] };
    const int          numDocuments = sizeof documents / sizeof *documents;
//..
// First, we define several aliases to make our code more comprehensible.
//..
    typedef bsl::unordered_map<bsl::string, int> WordTally;
    typedef bsl::pair         <bsl::string, int> WordTallyEntry;
  //typedef WordTally::value_type                WordTallyEntry;
  //typedef bsl::pair   <const bsl::string, int> WordTallyEntry;
    typedef bsl::pair<WordTally::iterator, bool> WordTallyInsertStatus;
//..
// Next, we (default) create an unordered map to hold our word tallies.  The
// output from the 'printf' statements will be discussed in {Example 2}.
//..
    WordTally wordTally;

    printf("size             %4d initial\n", wordTally.size());
    printf("bucket_count     %4d initial\n", wordTally.bucket_count());
    printf("load_factor      %f  initial\n", wordTally.load_factor());
    printf("max_load_factor  %f  initial\n", wordTally.max_load_factor());
//..
// Then, we define the set of characters that define word boundaries:
//..
    const char *delimiters = " \n\t,:;.()[]?!/-";
//..
// Next, we extract the words from our documents.  Note that 'strtok'
// modifies the document arrays (which were not made 'const').
//
// We tentatively assume that we are seeing each word for the first time and
// attempt to insert an initial record for that word.  If that succeeds (a
// 'true' value in the 'second' member of the 'bsl::pair' returned by the
// 'insert' method), we have correctly recorded that word; otherwise, we are
// returned an iterator (the 'first' member of the 'bsl::pair' returned) to the
// entry that had previously been added to the map.  In that case, we increment
// the data portion (the 'second' member) of that entry.
//..
    for (int idx = 0; idx < numDocuments; ++idx) {
        for (char *cur = strtok(documents[idx], delimiters);
                   cur;
                   cur = strtok(NULL,     delimiters)) {
            WordTallyEntry        initialEntry(bsl::string(cur), 1);
            WordTallyInsertStatus status = wordTally.insert(initialEntry);
            if (!status.second) {
                ++status.first->second;
            }
        }
    }
//..
// Then, now that the data has been (quickly) gathered, we can indulge in
// analysis that is more time consuming.  For example, we can define a
// comparison, sort the entries, and determine the 16 most commonly used words
// in the given document:
//..
    struct WordTallyEntryCompare {
        static bool lessValue(const WordTallyEntry& a,
                              const WordTallyEntry& b) {
            return a.second < b.second;
        }
        static bool moreValue(const WordTallyEntry& a,
                              const WordTallyEntry& b) {
            return !lessValue(a, b);
        }
    };

    bsl::vector<WordTallyEntry> array(wordTally.cbegin(), wordTally.cend());
    std::partial_sort(array.begin(),
                      array.begin() + 20,
                      array.end(),
                      WordTallyEntryCompare::moreValue);
//..
//  Notice that 'partial_sort' suffices since we seek only the 16 most used
//  words, and not a complete distribution of word counts.
//
//  Finally, we print the sorted portion of 'array':
//..
    for (bsl::vector<WordTallyEntry>::const_iterator cur  = array.begin(),
                                                     end  = cur + 16;
                                                     end != cur; ++cur) {    
        printf("%-10s %4d\n", cur->first.c_str(), cur->second);
    }
//..
// and standard output shows:
//..
//  the         193
//  of          169
//  and         161
//  to          147
//  in           60
//  has          48
//  be           40
//  right        38
//  for          37
//  a            34
//  or           32
//  Article      30
//  his          30
//  Everyone     29
//  shall        28
//  by           26
//..
//
///Example 2: Examining and Setting Unordered Map Configuration
///------------------------------------------------------------
// The unordered map interfaces all some insight into and control of its inner
// workings.  The unordered map is implemented using a hash table (see
// 'bslstl_hashtable'}), a dynamically sized array of "buckets".  If two
// elements hash to the same position the the table (the same bucket), then
// that bucket will house multiple elements.  As elements are added to the
// unordered map, the number of buckets is increased (and the existing elements
// redistributed) to keep the average number of elements per bucket (the
// "loading factor") below the specified maximum (the "maximum load factor", 1
// by default).
// 
// First, when 'wordTally' was created in {Example 1}, its metrics were:
//..
//  size                0 initial
//  bucket_count        1 initial
//  load_factor      0.000000  initial
//  max_load_factor  1.000000  initial
//..
// Notice that even when there are no elements ('size' is 0) there is one 
// bucket.  Since there are no elements, the average number of elements per
// bucket must be 0 (the 'load_factor').
//
// Next, after 'wordTally' has been loaded, we examine its metrics:
//..
    printf("size             %4d\n", wordTally.size());
    printf("bucket_count     %4d\n", wordTally.bucket_count());
    printf("load_factor      %f\n",  wordTally.load_factor());
    printf("max_load_factor  %f\n",  wordTally.max_load_factor());
//..
// and find at standard output:
//..
//  size             1108
//  bucket_count     2099
//  load_factor      0.527870
//  max_load_factor  1.000000
//..
// Notice how the number of buckets has increased.  (Sampling this metric
// as the map was loaded would show that the increase was done in several
// stages.)
//
// Then, we see that the load factor is indeed below the specified maximum;
// however we obtain further details of how the buckets are used.

// The unordered map provides and interface giving the element count of of elements in each bucket, we
// can easily determine the bucket with the greatest number of elements (i.e.,
// (greatest number of has collisions):
//..
    bsl::vector<int> bucketSizes;
    for (int idx = 0; idx < wordTally.bucket_count(); ++idx) {
       bucketSizes.push_back(wordTally.bucket_size(idx));
    }
    int maxBucketSize = *std::max_element(bucketSizes.begin(),
                                          bucketSizes.end());
    printf("maxBucketSize    %4d\n", maxBucketSize);
//..
// and find on standard output:
//..
//  maxBucketSize       4
//..
// We can also count the number of empty buckets, and the number of buckets
// at 'maxBucketSize'.
//..
    int numEmptyBuckets = std::count_if(bucketSizes.begin(),
                                        bucketSizes.end(),
                                        std::bind1st(std::equal_to<int>(), 0));
    printf("numEmptyBuckets  %4d\n", numEmptyBuckets);

    int numMaxBuckets = std::count_if(
                           bucketSizes.begin(),
                           bucketSizes.end(),
                           std::bind1st(std::equal_to<int>(), maxBucketSize));
    printf("numMaxBuckets    %4d\n", numMaxBuckets);
//..
// which shows on standard output:
//..
//  numEmptyBuckets 1236
//  numMaxBuckets      1
//..
// Suppose we are not satisfied with this distribution.  (Perhaps the load
// factor is too high.)  We can create a second, differently configured table.
//
// Next, create a new table 'wordTally2' with twice the bucket count shown by
// the first table ('wordTally'), and examine its initial metrics.
//..
    WordTally wordTally2(wordTally.bucket_count() * 2);

    printf("size2            %4d initial\n", wordTally2.size());
    printf("bucket_count2    %4d initial\n", wordTally2.bucket_count());
    printf("load_factor2     %f  initial\n", wordTally2.load_factor());
    printf("max_load_factor2 %f  initial\n", wordTally2.max_load_factor());
//..
// Standard output shows:
//..
//  size2               0 initial
//  bucket_count2    4201 initial
//  load_factor2     0.000000  initial
//  max_load_factor2 1.000000  initial
//..
// Notice that although we requested 4198 buckets (2 * 2099), we created a
// table with 4201 buckets.  (4201 is the smallest prime number greater than
// 4198).
//
// Then, we load our new table and examine its metrics.  For simplicity, we
// load data from the first table rather than re-tokenize our documents.
//..
    wordTally2.insert(wordTally.begin(), wordTally.end());

    printf("size2            %4d\n", wordTally2.size());
    printf("bucket_count2    %4d\n", wordTally2.bucket_count());
    printf("load_factor2     %f\n",  wordTally2.load_factor());
    printf("max_load_factor2 %f\n",  wordTally2.max_load_factor());

    bsl::vector<int> bucketSizes2;
    for (int idx = 0; idx < wordTally2.bucket_count(); ++idx) {
       bucketSizes2.push_back(wordTally2.bucket_size(idx));
    }
    int maxBucketSize2 = *std::max_element(bucketSizes2.begin(),
                                           bucketSizes2.end());
    printf("maxBucketSize2   %4d\n", maxBucketSize2);

    int numEmptyBuckets2 = std::count_if(
                                        bucketSizes2.begin(),
                                        bucketSizes2.end(),
                                        std::bind1st(std::equal_to<int>(), 0));
    printf("numEmptyBuckets2 %4d\n", numEmptyBuckets2);
    int numMaxBuckets2 = std::count_if(
                           bucketSizes2.begin(),
                           bucketSizes2.end(),
                           std::bind1st(std::equal_to<int>(), maxBucketSize2));
    printf("numMaxBuckets2   %4d\n", numMaxBuckets2);
//..
// Finally, we see on standard output:
//..
//  size2            1108
//  bucket_count2    4201
//  load_factor2     0.263747
//  max_load_factor2 1.000000
//  maxBucketSize2      3
//  numEmptyBuckets2 3242
//  numMaxBuckets2     15
//..
// Notice that the loading factor has been (roughly) cut in half; we have
// achieved our goal.  Also notice that the bucket count is unchanged since
// construction; thus, there were no rehashs during the loading this unordered
// map.  Finally, notice that the number of empty (unused) buckets is
// significantly higher, and there's been a modest decrease in the largest
// bucket size, but more instances of them.
//
// Thus, the unordered map provides facilities by which we can make trade-offs
// in performance characteristics of the containers we create.
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

        typedef bsl::unordered_map<int, int> TestType;

        if (veryVerbose) printf("Default construct an unordered map, 'x'\n");

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
        const int MAX_SAMPLE = 5000;
        BaseValue *dataSamples = new BaseValue[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            new(&dataSamples[i]) BaseValue(i, i*i);  // inplace-new needed to
                                                     // supply 'const' key
        }

        if (veryVerbose)  printf(
               "Range-construct an unordered_map, 'y', from the test array\n");

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
            typedef bsl::pair<TestType::iterator, bool> InsertResult;
            InsertResult iterBool = mX.insert(dataSamples[i]);
            ASSERT(!iterBool.second); // Already inserted with initial value
            ASSERT(x.end() != iterBool.first);
            ASSERT(*iterBool.first == dataSamples[i]);
        }

        if (veryVerbose) printf("Validate 'x' with the expected value\n");

        validateIteration(mX);
        testContainerHasData(x, 1, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf("Fill 'y' with the same values as 'x'\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            typedef bsl::pair<TestType::iterator, bool> InsertResult;
            InsertResult iterBool = mY.insert(dataSamples[i]);
            ASSERT(iterBool.second); // Already inserted with initial value
            ASSERT(x.end() != iterBool.first);
            ASSERT(*iterBool.first == dataSamples[i]);
        }

        if (veryVerbose) printf("Validate 'y' with the expected value\n");

        validateIteration(mY);
        testContainerHasData(y, 1, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf("'x' and 'y' should now compare equal\n");

        ASSERT(x == x);
        ASSERT(!(x != x));
        ASSERT(y == x);
        ASSERT(!(y != x));
        ASSERT(x == y);
        ASSERT(!(x != y));
        ASSERT(y == y);
        ASSERT(!(y != y));

        testContainerHasData(x, 1, dataSamples, MAX_SAMPLE);
        testContainerHasData(y, 1, dataSamples, MAX_SAMPLE);

        if (veryVerbose)
            printf("Create an unordered_map, 'z', that is a copy of 'x'\n");

        TestType mZ = x;
        const TestType &z = mZ;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'z'\n");

        ASSERT(1.0f == z.max_load_factor());
        ASSERT(x == z);
        ASSERT(!(x != z));
        ASSERT(z == x);
        ASSERT(!(z != x));

        validateIteration(mZ);

        if (veryVerbose)
            printf("Confirm that 'x' is unchanged by making the copy.\n");

        testBuckets(mX);
        validateIteration(mX);
        ASSERT(y == x);

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

        if (veryVeryVerbose) {
            debugPrint(mZ);
        }
        testErase(mZ);


        // quickly confirm assumptions on state of the containers
        ASSERT(z.empty());
        ASSERT(!x.empty());

        testMapLookup(mX);
        testImplicitInsert(mZ);  // double-check with an empty map

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
}  // Breathing test driver

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
