// bslstl_unorderedset.t.cpp                                          -*-C++-*-

#include <bslstl_unorderedset.h>

#include <bslalg_rangecompare.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdio.h>
#include <stdlib.h>

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

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    BSLS_ASSERT_OPT(a.get_allocator() == b.get_allocator());

    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The object under test is a container whose interface and contract is
// dictated by the C++ standard.  The general concerns are compliance,
// exception safety, and proper dispatching (for member function templates such
// as assign and insert).  This container is implemented in the form of a class
// template, and thus its proper instantiation for several types is a concern.
// Regarding the allocator template argument, we use mostly a 'bsl::allocator'
// together with a 'bslma::TestAllocator' mechanism, but we also verify the C++
// standard.
//
// The Primary Manipulators and Basic Accessors are decided to be:
//
// Primary Manipulators:
//: o 'insert'
//: o 'clear'
//
// Basic Accessors:
//: o 'cbegin'
//: o 'cend'
//: o 'size'
//: o 'get_allocator'
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'insert' and 'clear' methods to be used by the generator functions 'g' and
// 'gg'.  Note that some manipulators must support aliasing, and those that
// perform memory allocation must be tested for exception neutrality via the
// 'bslma_testallocator' component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.
//
// ----------------------------------------------------------------------------
// standard typedefs:
//*[26] typedef ... key_type;
//*[26] typedef ... value_type;
//*[26] typedef ... hasher;
//*[26] typedef ... key_equal;
//*[26] typedef ... allocator_type;
//*[26] typedef ... reference;
//*[26] typedef ... const_reference;
//*[26] typedef ... size_type;
//*[26] typedef ... difference_type;
//*[26] typedef ... pointer;
//*[26] typedef ... const_pointer;
//*[26] typedef ... iterator
//*[26] typedef ... const_iterator
//
// [unord.set] construct/copy/destroy:
//*[ 2] unordered_set(size_type, hasher, key_equal, allocator);
//*[12] unordered_set(ITER, ITER, size_type, hasher, key_equal, allocator);
//*[ 7] unordered_set(const unordered_set& original);
//*[11] unordered_set(const A& allocator);
//*[ 7] unordered_set(const unordered_set& original, const A& allocator);
//*[ 2] ~unordered_set();
//*[ 9] unordered_set& operator=(const unordered_set& rhs);
//
// iterators:
//*[13] iterator begin();
//*[13] const_iterator begin() const;
//*[13] iterator end();
//*[13] const_iterator end() const;
//*[ 4] const_iterator cbegin() const;
//*[ 4] const_iterator cend() const;
//
// capacity:
//*[19] bool empty() const;
//*[ 4] size_type size() const;
//*[25] size_type max_size() const;
//
// modifiers:
//*[ 2] bsl::pair<iterator, bool> insert(const value_type& value);
//*[17] iterator insert(const_iterator position, const value_type& value);
//*[18] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
//
//*[14] iterator erase(const_iterator position);
//*[15] size_type erase(const key_type& key);
//*[16] iterator erase(const_iterator first, const_iterator last);
//*[ 8] void swap(unordered_set& other);
//*[ 2] void clear();
//
// observers:
//*[ 4] hasher hash_function() const;
//*[ 4] key_equal key_eq() const;
//*[ 4] allocator_type get_allocator() const;
//
// set operations:
//*[20] iterator find(const key_type& key);
//*[20] const_iterator find(const key_type& key) const;
//*[21] size_type count(const key_type& key) const;
//*[21] bsl::pair<iterator, iterator> equal_range(const key_type& key);
//*[21] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// bucket interface:
//*[22] size_type bucket_count() const;
//*[25] size_type max_bucket_count() const;
//*[22] size_type bucket_size(size_type n) const;
//*[22] size_type bucket(const key_type& k) const;
//
// bucket iterators:
//*[23] local_iterator begin(size_type n);
//*[23] const_local_iterator begin(size_type n) const;
//*[23] local_iterator end(size_type n);
//*[23] const_local_iterator end(size_type n) const;
//*[23] const_local_iterator cbegin(size_type n) const;
//*[23] const_local_iterator cend(size_type n) const;
//
// hash policy:
//*[19] float load_factor() const;
//*[ 4] float max_load_factor() const;
//*[ 2] void max_load_factor(float z);
//*[24] void rehash(size_type n);
//*[24] void reserve(size_type n);
//
// specialized algorithms:
//*[ 6] bool operator==(unordered_set<K, H, E, A>, unordered_set<K, H, E, A>);
//*[ 6] bool operator!=(unordered_set<K, H, E, A>, unordered_set<K, H, E, A>);
//*[ 8] void swap(unordered_set<K, H, E, A>& a, unordered_set<K, H, E, A>& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//*[ 2] default construction (only)
//*[29] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
//*[ 3] int ggg(unordered_set<K,H,E,A> *object, const char *spec, int verbose);
//*[ 3] unordered_set<K,H,E,A>& gg(unordered_set<K,H,E,A> *, const char *spec);
//*[ 3] unordered_set<K,H,E,A> g(const char *spec);
//
//*[27] CONCERN: The object is compatible with STL allocators.
//*[28] CONCERN: The object has the necessary type traits
//*[  ] CONCERN: The type provides the full interface defined by the standard.

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// Define DEFAULT DATA used in multiple test cases.

static const size_t DEFAULT_MAX_LENGTH = 32;

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "A",                 "A" },
    { L_,    1, "AA",                "A" },
    { L_,   15, "B",                 "B" },
    { L_,    2, "AB",                "AB" },
    { L_,    2, "BA",                "AB" },
    { L_,   14, "AC",                "AC" },
    { L_,   18, "CD",                "CD" },
    { L_,    3, "ABC",               "ABC" },
    { L_,    3, "ACB",               "ABC" },
    { L_,    3, "BAC",               "ABC" },
    { L_,    3, "BCA",               "ABC" },
    { L_,    3, "CAB",               "ABC" },
    { L_,    3, "CBA",               "ABC" },
    { L_,   12, "BAD",               "ABD" },
    { L_,    3, "ABCA",              "ABC" },
    { L_,    3, "ABCB",              "ABC" },
    { L_,    3, "ABCC",              "ABC" },
    { L_,    3, "ABCABC",            "ABC" },
    { L_,    3, "AABBCC",            "ABC" },
    { L_,    4, "ABCD",              "ABCD" },
    { L_,    4, "ACBD",              "ABCD" },
    { L_,    4, "BDCA",              "ABCD" },
    { L_,    4, "DCBA",              "ABCD" },
    { L_,   13, "BEAD",              "ABDE" },
    { L_,   16, "BCDE",              "BCDE" },
    { L_,    5, "ABCDE",             "ABCDE" },
    { L_,    5, "ACBDE",             "ABCDE" },
    { L_,    5, "CEBDA",             "ABCDE" },
    { L_,    5, "EDCBA",             "ABCDE" },
    { L_,   17, "FEDCB",             "BCDEF" },
    { L_,    6, "FEDCBA",            "ABCDEF" },
    { L_,    7, "ABCDEFG",           "ABCDEFG" },
    { L_,    8, "ABCDEFGH",          "ABCDEFGH" },
    { L_,    9, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,   10, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP" },
    { L_,   10, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP" },
    { L_,   11, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   11, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};
static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

//=============================================================================
//                              TEST SUPPORT
//-----------------------------------------------------------------------------

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
    return v;        // for 'set' containers
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef          CONTAINER             TestType;
    typedef typename CONTAINER::size_type  SizeType;

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
        LOOP_ASSERT(i, x.begin(i) == x.end(i));
        LOOP_ASSERT(i, x.cbegin(i) == x.cend(i));
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
    typedef          CONTAINER             TestType;
    typedef typename CONTAINER::size_type  SizeType;

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
    typedef          CONTAINER                 TestType;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

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

    int initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (int i = 0; i != size; ++i) {
        typename TestType::iterator it =
                                       x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(data[i] == *it);
    }
}

template <class CONTAINER>
void validateIteration(CONTAINER &c)
{
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const int size = c.size();

    int counter = 0;
    for (iterator it = c.begin(); it != c.end(); ++it, ++counter) {}
    LOOP2_ASSERT(size, counter, size == counter);

    counter = 0;
    for (const_iterator it = c.cbegin(); it != c.cend(); ++it, ++counter) {}
    LOOP2_ASSERT(size, counter, size == counter);

    const CONTAINER& cc = c;

    counter = 0;
    for (const_iterator it = cc.begin(); it != cc.end(); ++it, ++counter) {}
    LOOP2_ASSERT(size, counter, size == counter);

    counter = 0;
    for (const_iterator it = cc.cbegin(); it != cc.cend(); ++it, ++counter) {}
    LOOP2_ASSERT(size, counter, size == counter);
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
    typedef                CONTAINER                             TestType;
    typedef typename       CONTAINER::     size_type             SizeType;
    typedef typename       CONTAINER::      iterator             iterator;
    typedef typename       CONTAINER::const_iterator       const_iterator;
    typedef typename       CONTAINER::local_iterator       local_iterator;
    typedef typename CONTAINER::const_local_iterator const_local_iterator;


    const CONTAINER &x = mX;

    SizeType bucketCount = x.bucket_count();
//    SizeType collisions = 0;
    SizeType itemCount  = 0;

    for (unsigned i = 0; i != bucketCount; ++i ) {
        const unsigned count = x.bucket_size(i);
        if (0 == count) {
            // if (veryVeryVerbose) cout << i << "\t(EMPTY)" << endl;

            LOOP_ASSERT(i, x.begin(i) == x.end(i));
            LOOP_ASSERT(i, mX.begin(i) == mX.end(i));
            LOOP_ASSERT(i, mX.cbegin(i) == mX.cend(i));
            // compile test iterator compatibility here, not needed later
            LOOP_ASSERT(i, mX.cbegin(i) == mX.end(i));
            LOOP_ASSERT(i, mX.begin(i) == mX.cend(i));
            continue;
        }

        itemCount += count;
        //collisions += count-1;
        SizeType bucketItems = 0;
        for (const_local_iterator iter = x.begin(i); iter != x.end(i); ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        LOOP3_ASSERT(i, count, bucketItems, count == bucketItems);

        bucketItems = 0;
        for (local_iterator iter = mX.begin(i); iter != mX.end(i); ++iter) {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        LOOP3_ASSERT(i, count, bucketItems, count == bucketItems);

        bucketItems = 0;
        for (const_local_iterator iter = mX.cbegin(i);
             iter != mX.cend(i);
             ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        LOOP3_ASSERT(i, count, bucketItems, count == bucketItems);

    }
    LOOP2_ASSERT(itemCount, x.size(), itemCount == x.size());
}


template <class CONTAINER>
void testErase(CONTAINER& mX)
{
    typedef          CONTAINER                       TestType;
    typedef typename CONTAINER::     size_type       SizeType;
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


//------ Test machinery borrowed from associative container test drivers ------

namespace bsl {

// set-specific print function.
template <class KEY, class HASH, class EQUAL, class ALLOC>
void debugprint(const bsl::unordered_set<KEY, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef bsl::unordered_set<KEY, HASH, EQUAL, ALLOC> TObj;
        typedef typename TObj::const_iterator CIter;
        for (CIter it = s.begin(); it != s.end(); ++it) {
            bsls::BslTestUtil::callDebugprint(
           static_cast<char>(bsltf::TemplateTestFacility::getIdentifier(*it)));
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

namespace {

bool expectToAllocate(int n)
    // Return 'true' if the container is expected to allocate memory on the
    // specified 'n'th element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

    struct BoolArray {
        // This class holds a set of boolean flags...

        explicit BoolArray(size_t n)
        : d_data(new bool[n])
        {
            for (size_t i = 0; i != n; ++i) {
                d_data[i] = false;
            }
        }

        ~BoolArray()
        {
            delete[] d_data;
        }

        bool& operator[](size_t index) { return d_data[index]; }
        bool *d_data;
    };



template<class CONTAINER, class VALUES>
int verifyContainer(const CONTAINER& container,
                    const VALUES&    expectedValues,
                    size_t           expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if(expectedSize != container.size()) {
        return -1;                                                    // RETURN
    }

    // Check to avoid creating an array of length zero.
    if (0 == expectedSize) {
        ASSERTV(container.empty());
        return 0;                                                     // RETURN
    }

    typedef typename CONTAINER::const_iterator CIter;

    BoolArray foundValues(container.size());
    size_t i = 0;
    for (CIter it = container.cbegin(); it != container.cend(); ++it, ++i) {
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(*it);
        size_t j = 0;
        do {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                ASSERTV(j, expectedValues[j], *it, !foundValues[j]);
                foundValues[j] = true;
                break;
            }
        }
        while (++j != container.size());
    }
    ASSERTV(expectedSize, i, expectedSize == i);
    if (expectedSize != i) {
        return -2;                                                    // RETURN
    }

    size_t missing = 0;
    for (size_t j = 0; j != expectedSize; ++j) {
        if (!foundValues[j]) {
            ++missing;
        }
    }

    return missing;  // 0 indicates a successful test!
}

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class OBJECT>
struct ExceptionGuard {
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores the
    // a copy of an object of the parameterized type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_copy;      // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionGuard(const OBJECT    *object,
                   int              line,
                   bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_copy(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception guard for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
    {}

    ~ExceptionGuard()
        // Destroy the exception guard.  If the guard was not released, verify
        // that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_copy == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release the guard from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

bool g_enableLessThanFunctorFlag = true;

                       // ====================
                       // class TestComparator
                       // ====================

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableLessThanFunctorFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableLessThanFunctorFlag = true;
    }

    // CREATORS
    //! TestEqualityComparator(const TestEqualityComparator& original) =
    //                                                                 default;
        // Create a copy of the specified 'original'.

    explicit TestEqualityComparator(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }

    bool operator== (const TestEqualityComparator& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class TYPE>
class TestHashFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CLASS METHOD
    static void disableFunctor()
        // Disable all objects of 'TestComparator' such that an 'ASSERT' will
        // be triggered if 'operator()' is invoked
    {
        g_enableLessThanFunctorFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableLessThanFunctorFlag = true;
    }

    // CREATORS
    //! TestHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestHashFunctor(int id = 0)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator== (const TestHashFunctor& rhs) const
    {
        return (id() == rhs.id());// && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

}  // close unnamed namespace

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

template <class KEY,
          class HASH = TestHashFunctor<KEY>,
          class EQUAL = TestEqualityComparator<KEY>,
          class ALLOC = bsl::allocator<KEY> >
class TestDriver {
    // This templatized struct provide a namespace for testing the
    // 'unordered_set' container.  The parameterized 'KEY', 'COMP' and 'ALLOC'
    // specifies the value type, comparator type and allocator type
    // respectively.  Each "testCase*" method test a specific aspect of
    // 'unordered_set<KEY, HASH, EQUAL, ALLOC>'.
    // Every test cases should be invoked with various parameterized type to
    // fully test the container.

  private:
    // TYPES
    typedef bsl::unordered_set<KEY, HASH, EQUAL, ALLOC> Obj;
        // Type under testing.

    // typedef TestComparatorNonConst<KEY> NonConstComp;
    // Comparator functor with a non-const function call operator.

    typedef typename Obj::iterator                Iter;
    typedef typename Obj::const_iterator          CIter;
    typedef typename Obj::size_type               SizeType;
        // Shorthands

    typedef bsltf::TestValuesArray<KEY> TestValues;

  public:
    typedef bsltf::StdTestAllocator<KEY> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'set<KEY, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'verbose' to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

  public:
    // TEST CASES
#if 0
    static void testCase24();
        // Test standard interface coverage.

    static void testCase23();
        // Test type traits.

    static void testCase22();
        // Test STL allocator.

    static void testCase21();
        // Test comparators.

    static void testCase20();
        // Test 'max_size' and 'empty'.

    static void testCase19();
        // Test comparison free operators.  'operator <' must be defined for
        // the parameterized 'KEY'.

    static void testCase18();
        // Test 'erase'.

    static void testCase17();
        // Test range 'insert'.

    static void testCase16();
        // Test 'insert' with hint.

    static void testCase15();
        // Test 'insert'.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test find, upper_bound, lower_bound.

    static void testCase12();
        // Test user-supplied constructors.

    static void testCase11();
        // Test generator functions 'g'.

    static void testCase10();
        // Reserved for BSLX.

#endif
    static void testCase9_1();
        // Test assignment operator ('operator=') for allocator propagation;

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8_1();
        // Test 'swap' member and free function for allocator propagation.

    static void testCase8();
        // Test 'swap' member.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Reserved for (<<) operator.

    static void testCase4();
        // Test basic accessors ('size', 'cbegin', 'cend' and 'get_allocator').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators ('insert' and 'clear').

#if 0
    static void testCase1(const COMP&  comparator,
                          KEY         *testKeys,
                          size_t       numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
#endif
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, HASH, EQUAL, ALLOC>::ggg(Obj        *object,
                                             const char *spec,
                                             int         verbose)
{
    bslma::DefaultAllocatorGuard
                                guard(&bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->insert(VALUES[spec[i] - 'A']);
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
bsl::unordered_set<KEY, HASH, EQUAL, ALLOC>&
TestDriver<KEY, HASH, EQUAL, ALLOC>::gg(Obj        *object,
                                        const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
bsl::unordered_set<KEY, HASH, EQUAL, ALLOC>
                        TestDriver<KEY,HASH, EQUAL, ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 If allocator propagation is not enabled for copy assignment, the
    //:   allocator address held by the target object is unchanged.
    //:
    //: 3 If allocator propagation is not enabled for copy assignment, any
    //:   memory allocation is from the target object's allocator.
    //:
    //: 4 The signature and return type are standard.
    //:
    //: 5 The reference returned is to the target object (i.e., '*this').
    //:
    //: 6 The value of the source object is not modified.
    //:
    //: 7 The allocator address held by the source object is unchanged.
    //:
    //: 8 QoI: Assigning a source object having the default-constructed value
    //:   allocates no memory.
    //:
    //: 9 Any memory allocation is exception neutral.
    //:
    //:10 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 If allocator propagation is enabled for copy assignment,
    //:   any memory allocation is from the source object's
    //:   allocator.
    //:
    //:13 If allocator propagation is enabled for copy assignment, the
    //:   allocator address held by the target object is changed to that of the
    //:   source.
    //:
    //:14 If allocator propagation is enabled for copy assignment, any memory
    //:   allocation is from the original target allocator will be released
    //:   after copy assignment.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 11)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create two
    //:     'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3:
    //:
    //:   3 For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //:
    //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:
    //:     4 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify that
    //:       the respective allocator addresses held by the target and source
    //:       objects are unchanged.  (C-2, 7)
    //:
    //:     7 Use the appropriate test allocators to verify that: (C-8, 11)
    //:
    //:       1 For an object that (a) is initialized with a value that did NOT
    //:         require memory allocation, and (b) is then assigned a value
    //:         that DID require memory allocation, the target object DOES
    //:         allocate memory from its object allocator only (irrespective of
    //:         the specific number of allocations or the total amount of
    //:         memory allocated); also cross check with what is expected for
    //:         'mX' and 'Z'.
    //:
    //:       2 An object that is assigned a value that did NOT require memory
    //:         allocation, does NOT allocate memory from its object allocator;
    //:         also cross check with what is expected for 'Z'.
    //:
    //:       3 No additional memory is allocated by the source object.  (C-8)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-11)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions (using
    //:     the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still the
    //:     object allocator.
    //:
    //:   8 Use the appropriate test allocators to verify that:
    //:
    //:     1 Any memory that is allocated is from the object allocator.
    //:
    //:     2 No additional (e.g., temporary) object memory is allocated when
    //:       assigning an object value that did NOT initially require
    //:       allocated memory.
    //:
    //:     3 All object memory is released when the object is destroyed.
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   set& operator=(const set& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);


            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch == Z.get_allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(&scratch);  const Obj& ZZ  = gg(&mZZ,  SPEC1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR,  &X, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa == Z.get_allocator());

                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase8_1()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS ALLOCATOR PROPAGATION
    //   Ensure that the 'swap' functions properly swap the objects when
    //   allocator propagation is enabled.  This function implements the test
    //   plan which address C-6 from 'testCase8' and is implemented as a
    //   separate function from 'testCase8', because at the time of writing,
    //   'AllocatorTraits' doesn't fully support allocator propagation, so some
    //   manual source code changes are needed to run this test.
    //
    //   TODO: integrate this test function to 'testCase8' once
    //   'AllocatorTraits' fully support allocator propagation.
    //
    // Concerns:
    //: 1 C-6 from 'testCase8':
    //:
    //:   If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //
    // Plan:
    //: 1 Manually modify 'AllocatorTraits::propagate_on_container_swap' to be
    //:   an alias to 'true_type'.
    //:
    //: 2 Follow P-3, P-4.1, P-4.2, P-4.4.7..P-4.4.9 from 'testCase8'.
    //:
    // Testing:
    //   void swap(set& other);
    //   void swap(set<K, C, A>& a, set<K, C, A>& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");


    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, &scratch);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mX(XX, &oa);  const Obj& X = mX;

            bslma::TestAllocator oap("p_object", veryVeryVeryVerbose);

            Obj mP(&oap);  const Obj& P = gg(&mP, SPEC2);
            const Obj PP(P, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(P) P(PP) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oapm(&oap);

                mX.swap(mP);

                ASSERTV(LINE1, LINE2, PP, X, PP == X);
                ASSERTV(LINE1, LINE2, XX, P, XX == P);
                ASSERTV(LINE1, LINE2, &oap == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == P.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
                ASSERTV(LINE1, LINE2, oapm.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oapm(&oap);

                swap(mX, mP);

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, PP, P, PP == P);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oap == P.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
                ASSERTV(LINE1, LINE2, oapm.isTotalSame());
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase8()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same
    //   allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped uses the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both function may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 Both functions provides the strong exception guarantee w.t.r. to
    //:   memory allocation .
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the
    //:   default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values (one per row) in
    //:     terms of their individual attributes, including (a) first, the
    //:     default value, (b) boundary values corresponding to every range
    //:     of values that each individual attribute can independently
    //:     attain, and (c) values that should require allocation from each
    //:     individual attribute that can independently allocate memory.
    //:
    //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
    //:     the expectation of memory allocation for all typical
    //:     implementations of individual attribute types: ('Y') "Yes",
    //:     ('N') "No", or ('?') "implementation-dependent".
    //:
    //: 4 For each row 'R1' in the table of P-3:  (C-1, 3..7)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1, 4)
    //:
    //:     1 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX' (P-4.2).
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mY', and having the value described by 'R2'; also use
    //:       the copy constructor to create, using a "scratch" allocator,
    //:       a 'const' 'Obj', 'YY', from 'Y'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) under the presence of exception; verify, after each
    //:       swap, that:  (C-1, 5, 7)
    //:
    //:       1 If exception occurred during the swap, both values are
    //:         unchanged.  (C-7)
    //
    //:       2 If no exception occurred, the values have been exchanged.
    //:         (C-1)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is is not
    //:         empty, and temporary memory were allocated from 'oaz' if 'mX'
    //:         is not empty.  (C-5)
    //:
    //:     7 Create a new object allocator, 'oap'.
    //:
    //:     8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:       'mP', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'PP', from 'P.
    //:
    //:     9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:       be an alias to 'true_type' (Instead of this manual step, use an
    //:       allocator that enables propagate_on_container_swap when
    //:       AllocatorTraits supports it) and use the the member and free
    //:       'swap functions to swap the values 'mX' and 'mZ' respectively;
    //:       verify, after each swap, that: (C-1, 6)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:       3 There was no additional object memory allocation.  (C-6)
    //:
    //:    7 Create a new object allocator, 'oap'.
    //:
    //:    8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:      'mP', having the value described by 'R2'; also use the copy
    //:      constructor to create, using a "scratch" allocator, a const 'Obj',
    //:      'PP', from 'P.
    //:
    //:    9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:      be an alias to 'true_type' (Instead of this manual step, use an
    //:      allocator that enables propagate_on_container_swap when
    //:      AllocatorTraits supports it) and use the the member and free 'swap
    //:      functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:      after each swap, that:  (C-1, 6)
    //:
    //:      1 The values have been exchanged.  (C-1)
    //:
    //:      2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:      3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-8)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   3 Use the default constructor and 'oa' to create a modifiable
    //:     'Obj' 'mX' (having default attribute values); also use the copy
    //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
    //:     'XX' from 'mX'.
    //:
    //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mY' having the value described by the 'Ai' attributes; also
    //:     use the copy constructor and a "scratch" allocator to create a
    //:     'const' 'Obj' 'YY' from 'mY'.
    //:
    //:   5 Use the 'invokeAdlSwap' helper function template to swap the
    //:     values of 'mX' and 'mY', using the free 'swap' function defined
    //:     in this component, then verify that:  (C-8)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(set& other);
    //   void swap(set<K, C, A>& a, set<K, C, A>& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, &scratch);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // Ensure the first row of the table contains the
        // default-constructed value.

        static bool firstFlag = true;
        if (firstFlag) {
            ASSERTV(LINE1, Obj(), W, Obj() == W);
            firstFlag = false;
        }

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mX(XX, &oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

            Obj mZ(&oaz);  const Obj& Z = gg(&mZ, SPEC2);
            const Obj ZZ(Z, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    mX.swap(mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END


                ASSERTV(LINE1, LINE2, ZZ, X, ZZ == X);
                ASSERTV(LINE1, LINE2, XX, Z, XX == Z);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oaz == Z.get_allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    swap(mX, mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oaz == Z.get_allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }

        }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;
        const Obj XX(X, &scratch);

        Obj mY(&oa);  const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   capacity.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original and change the capacity of the new object
    //:   correctly.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //:
    //: 3 For each value in S initialize objects w and x, and copy construct y
    //:   from x.  Change the state of y, by using the *primary* *manipulator*
    //:   'push_back'.  Using the 'operator!=' verify that y differs from x and
    //:   w, and verify that the capacity of y changes correctly.  (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    //: 5 Perform tests as P-2 in the presence of exceptions during memory
    //:   allocations using a 'bslma::TestAllocator' and varying its
    //:   *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   set(const set& original);
    //   set(const set& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    const TestValues VALUES;

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEFG",
            "HFGEDCBA",
            "CFHEBIDGA",
            "BENCKHGMALJDFOI",
            "IDMLNEFHOPKGBCJA",
            "OIQGDNPMLKBACHFEJ"
        };

        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = (int) strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object w.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(&oa);
                gg(pX, SPEC);

                const Obj Y0(*pX);

                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                bsl::pair<Iter, bool> RESULT = Y1.insert(VALUES['Z' - 'A']);

                ASSERTV(true == RESULT.second);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, &oa);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

#if 0  // Bad assumption that an empty container allocates no memory
       // This *is* a QoI concern we are likely to revisit though.
                if (LENGTH == 0) {
                    ASSERTV(SPEC, BB + 0 == AA);
                    ASSERTV(SPEC,  B + 0 ==  A);
                }
                else {
#elif 1  // In the meantime, we do expect one single extra allocation for the
         // index array
                    const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS ==  A);
#else
                }
#endif

                const bsls::Types::Int64 CC = oa.numBlocksTotal();
                const bsls::Types::Int64  C = oa.numBlocksInUse();

                bsl::pair<Iter, bool> RESULT = Y11.insert(VALUES['Z' - 'A']);
                ASSERTV(true == RESULT.second);

                const bsls::Types::Int64 DD = oa.numBlocksTotal();
                const bsls::Types::Int64  D = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, CC + 1 + TYPE_ALLOC == DD);
                ASSERTV(SPEC, C  + 1 + TYPE_ALLOC ==  D);

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                if (LENGTH == 0) {
                    ASSERTV(SPEC, BB + 0 == AA);
                    ASSERTV(SPEC,  B + 0 ==  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC *
                        (LENGTH + LENGTH * (1 + LENGTH) / 2);
                    ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(SPEC, B + 0 == A);
                }
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes (i.e., 'allocator') participate.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 'operator==' is defined in terms of 'operator==(KEY)' instead of the
    //:   supplied comparator function.
    //:
    //:11 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:12 The equality operator's signature and return type are standard.
    //:
    //:13 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 12..13)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a"scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create two (object) allocators, 'oax' and 'oay'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2'.
    //:
    //:       4 Disable the comparator so that it will cause an error if it's
    //:         used.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    //   bool operator!=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = bsl::operator==;
        operatorPtr operatorNe = bsl::operator!=;

        (void)operatorEq;  // quash potential compiler warnings
        (void)operatorNe;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_results);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(&xa); const Obj& X = gg(&mX, SPEC1);
                    Obj mY(&ya); const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    TestEqualityComparator<KEY>::disableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    TestEqualityComparator<KEY>::enableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - cbegin
    //     - cend
    //     - size
    //     - get_allocator
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 The range '[cbegin(), cend())' contains inserted elements, but not
    //:   necessarily in any give order.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'get_allocator' method.
    //:
    //:     3 Verify the object contains the expected number of elements.
    //:
    //:     4 Use 'cbegin' and 'cend' to iterate through all elements and
    //:       verify the values are as expected.  (C-1..2, 4)
    //:
    //:     5 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   const_iterator cbegin();
    //   const_iterator cend();
    //   size_type size() const;
    //   allocator_type get_allocator() const;
    // ------------------------------------------------------------------------

    static const struct {
        int         d_line;                     // source line number
        const char *d_spec;                     // specification string
        const char *d_results;                  // expected results
    } DATA[] = {
        //line  spec      result
        //----  --------  ------
        { L_,   "",       ""      },
        { L_,   "A",      "A"     },
        { L_,   "AB",     "AB"    },
        { L_,   "ABC",    "ABC"   },
        { L_,   "ABCD",   "ABCD"  },
        { L_,   "ABCDE",  "ABCDE" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const HASH  defaultHasher     = HASH();
    const EQUAL defaultComparator = EQUAL();

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            (bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            &sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            &sa2);
                      objAllocatorPtr = &sa2;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                          ? da
                                          : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == (int)X.size());

                ASSERT(0 == verifyContainer(X, EXP, LENGTH));

                ASSERT(oam.isTotalSame());

                // --------------------------------------------------------

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa1.numBlocksInUse(),
                        0 == sa1.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa2.numBlocksInUse(),
                        0 == sa2.numBlocksInUse());
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results
    //:
    //: 2 Invalid syntax is detected and reported.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length:
    //:
    //:   1 Use the primitive generator function 'gg' to set the state of a
    //:     newly created object.
    //:
    //:   2 Verify that 'gg' returns a valid reference to the modified argument
    //:     object.
    //:
    //:   3 Use the basic accessors to verify that the value of the object is
    //:     as expected.  (C-1)
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //   set<K,A>& gg(set<K,A> *object, const char *spec);
    //   int ggg(set<K,A> *object, const char *spec, int verbose = 1);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;                 // source line number
            const char *d_spec;                 // specification string
            const char *d_results;  // expected element values
        } DATA[] = {
            //line  spec      results
            //----  --------  -------
            { L_,   "",       ""      },
            { L_,   "A",      "A"     },
            { L_,   "B",      "B"     },
            { L_,   "AB",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "ABCDE",  "ABCDE" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            Obj mY(&oa);
            const Obj& Y = gg(&mY, SPEC);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                T_ T_ T_ P(X);
                T_ T_ T_ P(Y);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            ASSERT(0 == verifyContainer(X, EXP, LENGTH));
            ASSERT(0 == verifyContainer(Y, EXP, LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // control

            { L_,   "A",      -1,     }, // control
            { L_,   " ",       0,     },
            { L_,   ".",       0,     },
            { L_,   "E",       -1,    }, // control
            { L_,   "a",       0,     },
            { L_,   "z",       0,     },

            { L_,   "AE",     -1,     }, // control
            { L_,   "aE",      0,     },
            { L_,   "Ae",      1,     },
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",      0,     },

            { L_,   "ABC",    -1,     }, // control
            { L_,   " BC",     0,     },
            { L_,   "A C",     1,     },
            { L_,   "AB ",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",     0,     },

            { L_,   "ABCDE",  -1,     }, // control
            { L_,   "aBCDE",   0,     },
            { L_,   "ABcDE",   2,     },
            { L_,   "ABCDe",   4,     },
            { L_,   "AbCdE",   1,     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = (int)strlen(SPEC);

            Obj mX(&oa);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                 ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - insert
    //      - clear
    //
    // Concerns:
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    //:10 'insert' adds an additional element to the object if the element
    //:   being inserted does not already exist.
    //:
    //:11 'insert' returns a pair with an iterator of the element that was just
    //:   inserted or the element that already exist in the object, and a
    //:   boolean indicating whether element being inserted already exist in
    //:   the object.
    //:
    //:12 'clear' properly destroys each contained element value.
    //:
    //:13 'clear' does not allocate memory.
    //:
    //:14 Any argument can be 'const'.
    //:
    //:15 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly, and
    //:     (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations: (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       as the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'get_allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presense of exception and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 13..14)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-11..12)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   default construction (only)
    //   unordered_set(const allocator_type&);  // bslma::Allocator* only
    //   ~unordered_set();
    //   bsl::pair<iterator, bool> insert(const value_type& value);
    //   void clear();
    // ------------------------------------------------------------------------

    const bool VALUE_TYPE_USES_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    const HASH  defaultHasher     = HASH();
    const EQUAL defaultComparator = EQUAL();

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

#if 0   // First pass, we want to check no memory is allocated on construction
        // May run a second pass where we create a default number of buckets,
        // and allocator usage is harder to compute.
        const size_t DEFAULT_BUCKETS = ti % 2
                                     ? ti
                                     : 0;
#endif
        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj(0,
                                        defaultHasher,
                                        defaultComparator);
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj(0,
                                        defaultHasher,
                                        defaultComparator,
                                        (bslma::Allocator *)0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(0,
                                        defaultHasher,
                                        defaultComparator,
                                        &sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  return;
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.
            // NOTE THAT THIS QoI TEST IS STILL AN OPEN DESIGN ISSUE

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            // If default constructed, only the the static bucket is present.
            // QOI test that exactly 1 bucket is present.

            ASSERTV(LENGTH, CONFIG, X.bucket_count(), 1 == X.bucket_count());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'insert' (bootstrap).\n"); }

            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }
                    bsl::pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, true       == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                }

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                if(veryVerbose) {
                    printf("\t\t Testing allocator exceptions\n");
                }
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[LENGTH - 1]);
                    }
                    bslma::TestAllocatorMonitor tam(&oa);
                    bsl::pair<Iter, bool> RESULT =
                                                 mX.insert(VALUES[LENGTH - 1]);

#if defined(AJM_NEEDS_TO_UNDERSTAND_THESE_FAILURES_BETTER)
                    if (VALUE_TYPE_USES_ALLOC || expectToAllocate(LENGTH)) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }
#endif

#if 0
                    // Verify no temporary memory is allocated from the object
                    // allocator.
                    // BROKEN TEST CONDITION
                    // We need to think carefully about how we allow for the
                    // allocation of the bucket-array

                    ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());
#endif
                    ASSERTV(LENGTH, CONFIG, true == RESULT.second);
                    ASSERTV(LENGTH, CONFIG,
                            VALUES[LENGTH - 1] == *(RESULT.first));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LENGTH, CONFIG, LENGTH, X.size(), LENGTH == X.size());

                {
                    bool *foundValues = new bool[X.size()];
                    for (size_t j = 0;j != X.size(); ++j) {
                        foundValues[j] = false;
                    }

                    size_t i = 0;
                    for (CIter it = X.cbegin(); it != X.cend(); ++it, ++i) {
                        size_t j = 0;
                        do {
                            if (VALUES[j] == *it) {
                                ASSERTV(LENGTH, CONFIG, VALUES[j],
                                        !foundValues[j]);
                                foundValues[j] = true;
                            }
                        }
                        while (++j != X.size());
                    }
                    size_t missing = 0;
                    for (size_t j = 0; j != X.size(); ++j) {
                        if (!foundValues[j]) { ++missing; }
                    }
                    ASSERTV(LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundValues;

                    ASSERTV(LENGTH, CONFIG, X.size() == i);
                }

                // Verify behavior when element already exist in the object

                if(veryVerbose) {
                    printf("\t\t Verifying already inserted values\n");
                }
                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }
                    bsl::pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, RESULT.second, false ==
                                                                RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, *(RESULT.first), VALUES[tj] ==
                                                              *(RESULT.first));
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                if (veryVeryVeryVerbose) printf("mX.clear();\n");
                mX.clear();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    bsl::pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, true       == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAlloc("A");
    bslma::Default::setDefaultAllocator(&testAlloc);

    switch (test) { case 0:
#if 0
      case 25: {
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
            using namespace UsageExample;

            bslma::TestAllocator defaultAllocator("defaultAllocator");
            bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

            bslma::TestAllocator objectAllocator("objectAllocator");

            HolidayCalendar cal(&objectAllocator);

            ASSERT(false == cal.isHolidayDate(MyDate(2012, 7, 4)));

            cal.addHolidayDate(MyDate(2012, 7, 4));

            ASSERT(true == cal.isHolidayDate(MyDate(2012, 7, 4)));

            cal.addHolidayDate(MyDate(2012, 1, 1));

            ASSERT(true == cal.isHolidayDate(MyDate(2012, 1, 1)));

            cal.removeHolidayDate(MyDate(2012, 1, 1));

            ASSERT(false == cal.isHolidayDate(MyDate(2012, 1, 1)));

            cal.addHolidayDate(MyDate(2012, 12, 25));

            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 < objectAllocator.numBytesInUse());
        }

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING STANDARD INTERFACE COVERAGE
        // --------------------------------------------------------------------
        // Test only 'int' and 'char' parameter types, because map's
        // 'operator<' and related operators only support parameterized types
        // that defines 'operator<'.
        RUN_EACH_TYPE(TestDriver, testCase24, int, char);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING COMPARATOR
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' and 'empty'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver, testCase19, int, char);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE 'insert'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'insert' WITH HINT
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'insert'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Constructor"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'g'"
                            "\n===========\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Streaming Functionality"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // RUN_EACH_TYPE(TestDriver, testCase9_1, int);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // RUN_EACH_TYPE(TestDriver, testCase8_1, int);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
#endif
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
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

        typedef bsl::unordered_set<int> TestType;

        if (veryVerbose) printf("Default construct an unordered set, 'x'\n");

        TestType mX;
        const TestType& X = mX;

        if (veryVerbose) printf("Validate default behavior of 'x'\n");

        ASSERT(1.0f == X.max_load_factor());

        testConstEmptyContainer(X);
        testEmptyContainer(mX);

        swap(mX, mX);

        testConstEmptyContainer(X);
        testEmptyContainer(mX);

        validateIteration(mX);

        if (veryVerbose) printf("Prepare a test array of data samples\n");

        const int MAX_SAMPLE = 10000;
        int *dataSamples = new int[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            dataSamples[i] = i;
        }


        if (veryVerbose)  printf(
               "Range-construct an unordered_set, 'y', from the test array\n");
        TestType mY(dataSamples, dataSamples + MAX_SAMPLE);
        const TestType& Y = mY;

        if (veryVerbose) {
            printf("Validate behavior of freshly constructed 'y'\n");
        }
        ASSERT(1.0f == Y.max_load_factor());

        testContainerHasData(Y, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mY);


        if (veryVerbose) {
            printf("Assert equality relationships, noting 'x != y'\n");
        }

        ASSERT(X == X);
        ASSERT(!(X != X));
        ASSERT(Y != X);
        ASSERT(!(Y == X));
        ASSERT(X != Y);
        ASSERT(!(X == Y));
        ASSERT(Y == Y);
        ASSERT(!(Y != Y));


        if (veryVerbose) printf("Swap 'x' and 'y'\n");
        swap(mX, mY);

        if (veryVerbose) printf("Validate swapped values\n");

        testConstEmptyContainer(Y);
        testEmptyContainer(mY);
        validateIteration(mY);

        testContainerHasData(X, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mX);

        if (veryVerbose) {
            printf("Assert swapped equality relationships, noting 'x != y'\n");
        }

        ASSERT(X == X);
        ASSERT(!(X != X));
        ASSERT(Y != X);
        ASSERT(!(Y == X));
        ASSERT(X != Y);
        ASSERT(!(X == Y));
        ASSERT(Y == Y);
        ASSERT(!(Y != Y));

        if (veryVerbose) printf("Try to fill 'x' with duplicate values\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            typedef bsl::pair<TestType::iterator, bool> InsertResult;
            InsertResult iterBool = mX.insert(dataSamples[i]);
            ASSERT(!iterBool.second); // Already inserted with initial value
            ASSERT(X.end() != iterBool.first);
            ASSERT(*iterBool.first == dataSamples[i]);
        }

        if (veryVerbose) printf("Validate 'x' with the expected value\n");

        validateIteration(mX);
        testContainerHasData(X, 1, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf("Confirm 'y' unchanged by inserts to 'x'\n");
        testConstEmptyContainer(Y);
        testEmptyContainer(mY);
        validateIteration(mY);

        if (veryVerbose) printf("Repopulate 'y' with test data\n");
        // mY.reserve(MAX_SAMPLE);  // See if this fixes a bug, by deferring
        // rehash

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            typedef bsl::pair<TestType::iterator, bool> InsertResult;
            InsertResult iterBool = mY.insert(dataSamples[i]);
            ASSERT(iterBool.second); // Successfully inserted
            ASSERT(Y.end() != iterBool.first);
            ASSERT(*iterBool.first == dataSamples[i]);
        }

        if (veryVerbose) printf("Validate 'y' with the expected value\n");

        validateIteration(mY);
        testContainerHasData(Y, 1, dataSamples, MAX_SAMPLE);

        validateIteration(mX);

        if (veryVerbose) printf("'x' and 'y' should now compare equal\n");

        ASSERT(X == X);
        ASSERT(!(X != X));
        ASSERT(Y == X);
        ASSERT(!(Y != X));
        ASSERT(X == Y);
        ASSERT(!(X != Y));
        ASSERT(Y == Y);
        ASSERT(!(Y != Y));

        if (veryVerbose) {
            printf("Create an unordered_map, 'z', that is a copy of 'x'\n");
        }

        TestType mZ = X;
        const TestType& Z = mZ;

        if (veryVerbose) {
            printf("Validate behavior of freshly constructed 'z'\n");
        }

        ASSERT(1.0f == Z.max_load_factor());
        ASSERT(X == Z);
        ASSERT(!(X != Z));
        ASSERT(Z == X);
        ASSERT(!(Z != X));

        validateIteration(mZ);

        if (veryVerbose) {
            printf("Confirm that 'x' is unchanged by making the copy.\n");
        }

        testBuckets(mX);
        validateIteration(mX);
        testContainerHasData(X, 1, dataSamples, MAX_SAMPLE);

        if (veryVerbose) {
            printf("Clear 'x' and confirm that it is empty.\n");
        }

        mX.clear();
        testEmptyContainer(mX);
        testBuckets(mX);

        if (veryVerbose) {
            printf("Assign the value of 'y' to 'x'.\n");
        }

        mX = Y;

        if (veryVerbose) printf("Confirm 'x' has the expected value.\n");

        ASSERT(X == Y);
        validateIteration(mX);
        testBuckets(mX);

        testErase(mZ);

        if (veryVerbose) {
            printf("Final message to confim the end of the breathing test.\n");
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
