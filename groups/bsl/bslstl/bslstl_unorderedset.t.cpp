// bslstl_unorderedset.t.cpp                                          -*-C++-*-

#include <bslstl_unorderedset.h>

#include <bslalg_rangecompare.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_issame.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_util.h>

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

#if defined(BDE_BUILD_TARGET_EXC)
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
#endif

#if 0   // bslalg::SwapUtil is our componentized ADL swap-invoker.
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
#endif

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
//*[24] typedef ... key_type;
//*[24] typedef ... value_type;
//*[24] typedef ... hasher;
//*[24] typedef ... key_equal;
//*[24] typedef ... allocator_type;
//*[24] typedef ... reference;
//*[24] typedef ... const_reference;
//*[24] typedef ... size_type;
//*[24] typedef ... difference_type;
//*[24] typedef ... pointer;
//*[24] typedef ... const_pointer;
//*[24] typedef ... iterator
//*[24] typedef ... const_iterator
//*[24] typedef ... local_iterator
//*[24] typedef ... const_local_iterator
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
//*[14] iterator begin();
//*[14] const_iterator begin() const;
//*[14] iterator end();
//*[14] const_iterator end() const;
//*[14] const_iterator cbegin() const;
//*[14] const_iterator cend() const;
//
// capacity:
//*[20] bool empty() const;
//*[ 4] size_type size() const;
//*[20] size_type max_size() const;
//
// modifiers:
//*[15] bsl::pair<iterator, bool> insert(const value_type& value);
//*[15] iterator insert(const_iterator position, const value_type& value);
//*[17] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
//
//*[18] iterator erase(const_iterator position);
//*[18] size_type erase(const key_type& key);
//*[18] iterator erase(const_iterator first, const_iterator last);
//*[ 8] void swap(unordered_set& other);
//*[ 2] void clear();
//
// observers:
//*[ 4] hasher hash_function() const;
//*[ 4] key_equal key_eq() const;
//*[ 4] allocator_type get_allocator() const;
//
// set operations:
//*[13] iterator find(const key_type& key);
//*[13] const_iterator find(const key_type& key) const;
//*[13] size_type count(const key_type& key) const;
//*[13] bsl::pair<iterator, iterator> equal_range(const key_type& key);
//*[13] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// bucket interface:
//*[26] size_type bucket_count() const;
//*[26] size_type max_bucket_count() const;    <<NOT WORKING>>
//*[26] size_type bucket_size(size_type n) const;
//*[26] size_type bucket(const key_type& k) const;
//
// bucket iterators:
//*[26] local_iterator begin(size_type n);
//*[26] const_local_iterator begin(size_type n) const;
//*[26] local_iterator end(size_type n);
//*[26] const_local_iterator end(size_type n) const;
//*[26] const_local_iterator cbegin(size_type n) const;
//*[26] const_local_iterator cend(size_type n) const;
//
// hash policy:
//*[25] float load_factor() const;
//*[25] float max_load_factor() const;
//*[25] void max_load_factor(float z);
//*[25] void rehash(size_type n);
//*[25] void reserve(size_type n);
//
// specialized algorithms:
//*[ 6] bool operator==(unordered_set<K, H, E, A>, unordered_set<K, H, E, A>);
//*[ 6] bool operator!=(unordered_set<K, H, E, A>, unordered_set<K, H, E, A>);
//*[ 8] void swap(unordered_set<K, H, E, A>& a, unordered_set<K, H, E, A>& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] default construction (only)
// [29] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
//*[ 3] int ggg(unordered_set<K,H,E,A> *object, const char *spec, int verbose);
//*[ 3] unordered_set<K,H,E,A>& gg(unordered_set<K,H,E,A> *, const char *spec);
//*[11] unordered_set<K,H,E,A> g(const char *spec);
//
//*[22] CONCERN: The object is compatible with STL allocators.
//*[23] CONCERN: The object has the necessary type traits
//*[23] TBD: Not yet working for all types.
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

template <class TYPE>
const TYPE& my_max(const TYPE& x, const TYPE& y)
{
    return x > y ? x : y;
}

template <class TYPE>
TYPE my_abs(const TYPE& x)
{
    return x < 0 ? -x : x;
}

template <class TYPE>
bool nearlyEqual(const TYPE& x, const TYPE& y)
{
    TYPE tolerance = my_max(my_abs(x), my_abs(y)) * 0.0001;
    return my_abs(x - y) <= tolerance;
}

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
        for (SizeType iterations = nCopies; --iterations; ++it) {
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
    typedef typename CONTAINER::size_type      size_type;

    const size_type size = c.size();

    size_type counter = 0;
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

    for (SizeType i = 0; i != bucketCount; ++i ) {
        const SizeType count = x.bucket_size(i);
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
    size_t bucketA = x.bucket(key);
    while (bucketA == x.bucket(keyForValue<CONTAINER>(*++next))) {}
    // cIter/next now point to elements either side of a key-range
    // confirm they are not in the same bucket:
    ASSERTV(x.bucket(key)    != x.bucket(*next));
    ASSERTV(x.bucket(*cIter) != x.bucket(*next));
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
        while (++testCursor != next) {
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
    for (iterator it = mX.begin(); it != x.end(); it = mX.erase(it)) {}
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
size_t verifyContainer(const CONTAINER& container,
                       const VALUES&    expectedValues,
                       size_t           expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if (expectedSize != container.size()) {
        return static_cast<size_t>(-1);                               // RETURN
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
        } while (++j != container.size());
    }
    ASSERTV(expectedSize, i, expectedSize == i);
    if (expectedSize != i) {
        return static_cast<size_t>(-2);                               // RETURN
    }

    int missing = 0;
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
class TestNonConstEqualityComparator {
    // This test class provides a functor for equality comparison of objects
    // where the 'operator()' is not declared const.

  public:
    // CREATORS
    TestNonConstEqualityComparator() {}
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.

    //! TestNonConstEqualityComparator(const TestEqualityComparator& original)
    //                                                               = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
            == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }

    bool operator==(const TestNonConstEqualityComparator&) const
    {
        return true;
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
    bool        d_shortCircuit; // always returns 0 if true

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

    explicit TestHashFunctor(int id = 0, bool shortCircuit = false)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.  Also optionally specify
        // 'shortCircuit', which, if 'true' indicates that this functor will
        // always return 0.
    : d_id(id)
    , d_count(0)
    , d_shortCircuit(shortCircuit)
    {
    }

    // ACCESSORS
    size_t operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return d_shortCircuit
               ? 0
               : bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
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

template <class TYPE>
class TestNonConstHashFunctor {
    // This class provides a hash functor whose 'operator()()' hasn't been
    // declared 'const'.

  public:
    // CREATORS
    TestNonConstHashFunctor() {}
        // Create a copy of the specified 'original'.
    //! TestNonConstHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    size_t operator() (const TYPE& obj)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
    }

    bool operator==(const TestNonConstHashFunctor&)
    {
        return true;
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

    static
    bsltf::TestValuesArrayIterator<KEY> indexIterator(
                                                TestValues         *testValues,
                                                native_std::size_t  index);
        // Return, by value, an iterator to the specified 'index'th element of
        // the specified 'testValues'.

    static int getIndexForIter(const Obj& obj, Iter it);
        // Find the index corresponding to the specified iterator 'it', which
        // must be a valid iterator referring to the specified 'obj'.  Note
        // that 'obj.end() == it' is allowed.

    static Iter getIterForIndex(const Obj& obj, size_t idx);
        // Return the iterator relating to the specified 'obj' with specified
        // index 'idx'.  It is an error if 'idx >= obj.size()'.

  public:
    // TEST CASES

    static void testCase99();
        // Test spread of nodes into different buckets

    static void testCase26();
        // reserve, rehash, max_load_factor and bucket iterators

    static void testCase25();
        // reserve, rehash, max_load_factor

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

#if 0
    static void testCase19();
        // Test comparison free operators.  'operator <' must be defined for
        // the parameterized 'KEY'.  N/A
#endif

    static void testCase18();
        // Test 'erase'.

    static void testCase17();
        // Test range 'insert'.

#if 0
    static void testCase16();
        // N/A
#endif

    static void testCase15();
        // Test 'insert'.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test find

    static void testCase12();
        // Test value / range constructors.

    static void testCase11();
        // Test generator functions 'g'.

    static void testCase10();
        // Reserved for BSLX.

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

template <class KEY>
class StdAllocTestDriver : public TestDriver<KEY,
                                             TestHashFunctor<KEY>,
                                             TestEqualityComparator<KEY>,
                                             bsltf::StdTestAllocator<KEY> > {
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, HASH, EQUAL, ALLOC>::ggg(Obj        *object,
                                             const char *spec,
                                             int         verbose)
{
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

template <class TYPE>
bool isConstValue(TYPE&)
    // Template method to determine whether a returned value is declared
    // 'const'.
{
    return false;
}

template <class TYPE>
bool isConstValue(const TYPE&)
{
    return true;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, HASH, EQUAL, ALLOC>::getIndexForIter(const Obj& obj,
                                                         Iter       it)
{
    int ret = 0;
    for (Iter itb = obj.begin(); it != itb; ++itb) {
        ++ret;
    }

    return ret;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
typename bsl::unordered_set<KEY, HASH, EQUAL, ALLOC>::iterator
TestDriver<KEY, HASH, EQUAL, ALLOC>::getIterForIndex(const Obj& obj,
                                                     size_t     idx)
{
    if (idx > obj.size()) {
        ASSERTV(idx <= obj.size());

        return obj.end();                                             // RETURN
    }

    Iter ret = obj.begin();
    size_t i = 0;
    for (; i < idx && obj.end() != ret; ++i) {
        ++ret;
    }

    ASSERTV(idx == i);

    return ret;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase99()
{
    // ------------------------------------------------------------------------
    // TESTING SPREAD
    //
    // Concerns:
    //   That the hash table isn't unintentionally funneling all nodes into
    //   a few buckets.
    // ------------------------------------------------------------------------

    if (verbose) printf("Testing Spread\n"
                        "==============\n");

    TestValues VALUES;

    if (verbose) printf("Test spread\n");
    {
        Obj mX;  const Obj& X = mX;
        mX.max_load_factor(0.5);
        gg(&mX, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

        ASSERT(26 == X.size());
        ASSERT(X.bucket_count() >= 52);

        size_t total = 0;
        for (size_t ti = 0; ti < X.bucket_count(); ++ti) {
            size_t bs = X.bucket_size(ti);
            ASSERTV(ti, bs, bs < 5);
            total += bs;

            ASSERT((bs != 0) == (X. begin(ti) != X. end(ti)));
            ASSERT((bs != 0) == (X.cbegin(ti) != X.cend(ti)));
        }

        ASSERTV(X.size() == total);
    }

    if (verbose) printf("Test non-spread with delinquent hash function\n");
    {
        Obj mX(0, HASH(0, true), EQUAL());  const Obj& X = mX;
        mX.max_load_factor(0.5);
        gg(&mX, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

        ASSERT(26 == X.size());
        ASSERT(X.bucket_count() >= 52);

        size_t total = 0;
        for (size_t ti = 0; ti < X.bucket_count(); ++ti) {
            size_t bs = X.bucket_size(ti);
            if (0 == ti) {
                ASSERTV(ti, bs, X.size(), bs == X.size());
                ASSERT(X. begin(ti) != X. end(ti));
                ASSERT(X.cbegin(ti) != X.cend(ti));
            }
            else {
                ASSERTV(ti, bs, 0 == bs);
                ASSERT(X. begin(ti) == X. end(ti));
                ASSERT(X.cbegin(ti) == X.cend(ti));
            }
            total += bs;
        }

        ASSERTV(X.size() == total);
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // BUCKET INTERFACE AND BUCKET ITERATORS
    //
    // Concerns:
    //   Testing
    //     - max_load_factor (forcing rehash)
    //     - reserve
    //     - rehash
    //     - bucket_count
    //     - bucket_size(size_t)
    //     - begin(size_t) const
    //     - begin(size_t)
    //     - end(size_t) const
    //     - end(size_t)
    //     - cbegin(size_t) const
    //     - cend(size_t) const
    // ------------------------------------------------------------------------

    typedef typename Obj::local_iterator       LIter;
    typedef typename Obj::const_local_iterator CLIter;

    BSLMF_ASSERT((bslmf::IsSame<LIter, CLIter>::value));

    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    TestValues VALUES;

    const KEY kA(VALUES[0]);

    Obj mX;     const Obj& X = mX;

    ASSERTV(X.begin(0)  == X.end(0));
    ASSERTV(X.begin(0) == mX.begin(0));
    ASSERTV(X.begin(0) == mX.end(0));
    ASSERTV(X.begin(0) ==  X.cbegin(0));
    ASSERTV(X.begin(0) ==  X.cend(0));

    mX.max_load_factor(10.0);

    size_t maxSizeT = 0;
    --maxSizeT;
    ASSERTV(X.max_bucket_count() <= maxSizeT);    // TBD -- should be '<'

    const char *SPEC = "ABCDEFGH";

    gg(&mX, SPEC);

    ASSERTV(X.bucket_count(), 2 == X.bucket_count());   // 2 is the min #
                                                        // of buckets for a
                                                        // non-empty container

    {
        {
            size_t b = X.bucket(kA);
            const LIter end = X.end(b);
            for (LIter it = X.begin(b); true; ++it) {
                if (end == it) {
                    ASSERTV(0 && "kA not found");
                    break;
                }
                if (kA == *it) {
                    break;
                }
            }
        }

        size_t count = 0;
        for (size_t ti = 0; ti < X.bucket_count(); ++ti) {
            {
                const LIter begin = X.begin(ti);
                ASSERTV(begin == mX.begin(ti));
                ASSERTV(begin == mX.cbegin(ti));
                ASSERTV(begin == X.begin(ti));
                ASSERTV(begin == X.cbegin(ti));

                const LIter end = X.end(ti);
                ASSERTV(end == mX.end(ti));
                ASSERTV(end == mX.cend(ti));
                ASSERTV(end == X.end(ti));
                ASSERTV(end == X.cend(ti));

                if (0 == X.bucket_size(ti)) {
                    ASSERTV(begin == end);
                }
            }

            size_t bCount = 0;
            LIter end = X.end(ti);
            for (LIter it = X.begin(ti); end != it; ++it) {
                ++bCount;
            }
            ASSERTV(X.bucket_size(ti) == bCount);

            count += X.bucket_size(ti);
        }
        ASSERTV(count == X.size());
    }

    Obj mX10;    const Obj& X10 = mX10;
    mX10.max_load_factor(10.0);
    gg(&mX10, SPEC);

    ASSERTV(X   == X);
    ASSERTV(X10 == X);
    ASSERTV(X10.bucket_count() == X.bucket_count());

    {
        const size_t bucketCount = X.bucket_count();
        mX.max_load_factor(1.0);
        ASSERTV(X.bucket_count() > bucketCount);
    }

    ASSERTV(X   == X);
    ASSERTV(X10 == X);

    ASSERTV(X.bucket_count() > X.size());    // X.size() == 8, not prime

    {
        {
            size_t b = X.bucket(kA);
            const LIter end = X.end(b);
            for (LIter it = X.begin(b); true; ++it) {
                if (end == it) {
                    ASSERTV(0 && "kA not found");
                    break;
                }
                if (kA == *it) {
                    break;
                }
            }
        }

        size_t count = 0, emptyCount = 0;
        for (size_t ti = 0; ti < X.bucket_count(); ++ti) {
            {
                const LIter begin = X.begin(ti);
                ASSERTV(begin == mX.begin(ti));
                ASSERTV(begin == mX.cbegin(ti));
                ASSERTV(begin == X.begin(ti));
                ASSERTV(begin == X.cbegin(ti));

                const LIter end = X.end(ti);
                ASSERTV(end == mX.end(ti));
                ASSERTV(end == mX.cend(ti));
                ASSERTV(end == X.end(ti));
                ASSERTV(end == X.cend(ti));

                if (0 == X.bucket_size(ti)) {
                    ++emptyCount;
                    ASSERTV(begin == end);
                }
            }

            size_t bCount = 0;
            LIter end = X.end(ti);
            for (LIter it = X.begin(ti); end != it; ++it) {
                ++bCount;
            }
            ASSERTV(X.bucket_size(ti) == bCount);

            count += X.bucket_size(ti);
        }
        ASSERTV(count == X.size());
        ASSERTV(emptyCount > 0);
    }

    Obj mX1;    const Obj& X1 = mX1;
    mX1.max_load_factor(10.0);
    gg(&mX1, SPEC);
    mX1.max_load_factor(1.0);

    ASSERTV(X   == X);
    ASSERTV(X10 == X);
    ASSERTV(X1  == X);
    ASSERTV(X1. bucket_count() == X.bucket_count());
    ASSERTV(X10.bucket_count() <  X.bucket_count());

    {
        const size_t bucketCount = X.bucket_count();
        mX.reserve(X.size() * 3);
        ASSERTV(X.bucket_count() > bucketCount);
    }

    {
        {
            size_t b = X.bucket(kA);
            const LIter end = X.end(b);
            for (LIter it = X.begin(b); true; ++it) {
                if (end == it) {
                    ASSERTV(0 && "kA not found");
                    break;
                }
                if (kA == *it) {
                    break;
                }
            }
        }

        size_t count = 0, emptyCount = 0;
        for (size_t ti = 0; ti < X.bucket_count(); ++ti) {
            {
                const LIter begin = X.begin(ti);
                ASSERTV(begin == mX.begin(ti));
                ASSERTV(begin == mX.cbegin(ti));
                ASSERTV(begin == X.begin(ti));
                ASSERTV(begin == X.cbegin(ti));

                const LIter end = X.end(ti);
                ASSERTV(end == mX.end(ti));
                ASSERTV(end == mX.cend(ti));
                ASSERTV(end == X.end(ti));
                ASSERTV(end == X.cend(ti));

                if (0 == X.bucket_size(ti)) {
                    ++emptyCount;
                    ASSERTV(begin == end);
                }
            }

            size_t bCount = 0;
            LIter end = X.end(ti);
            for (LIter it = X.begin(ti); end != it; ++it) {
                ++bCount;
            }
            ASSERTV(X.bucket_size(ti) == bCount);

            count += X.bucket_size(ti);
        }
        ASSERTV(count == X.size());
        ASSERTV(emptyCount > 0);
    }

    ASSERTV(X   == X);
    ASSERTV(X10 == X);
    ASSERTV(X1  == X);
    ASSERTV(X10.bucket_count() < X1.bucket_count());
    ASSERTV(X1. bucket_count() < X. bucket_count());

    Obj mX3;    const Obj& X3 = mX3;
    mX3.max_load_factor(10.0);
    gg(&mX3, SPEC);
    mX3.max_load_factor(1.0);
    mX3.reserve(X3.size() * 3);

    ASSERTV(X3 == X);
    ASSERTV(X3.bucket_count() == X.bucket_count());

    {
        const size_t bucketCount = X.bucket_count();
        mX.rehash(bucketCount * 2);
        ASSERTV(X.bucket_count() > bucketCount * 2);    // must be prime,
                                                        // won't be even
    }

    {
        {
            size_t b = X.bucket(kA);
            const LIter end = X.end(b);
            for (LIter it = X.begin(b); true; ++it) {
                if (end == it) {
                    ASSERTV(0 && "kA not found");
                    break;
                }
                if (kA == *it) {
                    break;
                }
            }
        }

        size_t count = 0, emptyCount = 0;
        for (size_t ti = 0; ti < X.bucket_count(); ++ti) {
            {
                const LIter begin = X.begin(ti);
                ASSERTV(begin == mX.begin(ti));
                ASSERTV(begin == mX.cbegin(ti));
                ASSERTV(begin == X.begin(ti));
                ASSERTV(begin == X.cbegin(ti));

                const LIter end = X.end(ti);
                ASSERTV(end == mX.end(ti));
                ASSERTV(end == mX.cend(ti));
                ASSERTV(end == X.end(ti));
                ASSERTV(end == X.cend(ti));

                if (0 == X.bucket_size(ti)) {
                    ++emptyCount;
                    ASSERTV(begin == end);
                }
            }

            size_t bCount = 0;
            LIter end = X.end(ti);
            for (LIter it = X.begin(ti); end != it; ++it) {
                ++bCount;
            }
            ASSERTV(X.bucket_size(ti) == bCount);

            count += X.bucket_size(ti);
        }
        ASSERTV(count == X.size());
        ASSERTV(emptyCount > 0);
    }

    ASSERTV(X   == X);
    ASSERTV(X10 == X);
    ASSERTV(X1  == X);
    ASSERTV(X3  == X);
    ASSERTV(X10.bucket_count() < X1.bucket_count());
    ASSERTV(X1. bucket_count() < X3.bucket_count());
    ASSERTV(X3. bucket_count() < X. bucket_count());
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase25()
{
    // ------------------------------------------------------------------------
    // RESERVE, REHASH, MAX_LOAD_FACTOR
    //
    // Concern:
    //   That 'reserve', 'rehash', and 'max_load_factor' all grow the bucket
    //   array as expected, and have no effect on the salient attributes of
    //   the container.
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    HASH  defaultHash;
    EQUAL defaultEqual;

    const char *SPEC, *lastSpec = "+";
    for (size_t ti = 0; ti < NUM_DATA; ++ti, lastSpec = SPEC) {
        const size_t LINE   = DATA[ti].d_line;
                     SPEC   = DATA[ti].d_results;
        const size_t LENGTH = strlen(SPEC);

        bslma::TestAllocator oa("supplied", veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);

        // skip duplicates

        if (!strcmp(SPEC, lastSpec)) {
            continue;
        }

        TestValues values(SPEC, &scratch);

        {
            Obj mX(values.begin(),
                   values.end(),
                   0,
                   defaultHash,
                   defaultEqual,
                   &oa);      const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                                  const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                ExceptionGuard<Obj> guard(&X, L_, &sc);
                ASSERTV(COUNT == X.bucket_count());

                mX.reserve(my_max<size_t>(X.size(), 1) * 3);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        {
            Obj mX(values.begin(),
                   values.end(),
                   0,
                   defaultHash,
                   defaultEqual,
                   &oa);      const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                                  const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                ExceptionGuard<Obj> guard(&X, L_, &sc);

                mX.rehash(X.bucket_count() + 1);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        if (values.size() > 0) {
            Obj mX(values.begin(),
                   values.end(),
                   0,
                   defaultHash,
                   defaultEqual,
                   &oa);      const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                                  const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();
            const float  LOAD  = X.load_factor();

            double actualLoad = static_cast<double>(X.size())
                                       / static_cast<double>(X.bucket_count());

            ASSERTV(LOAD, actualLoad, nearlyEqual<double>(LOAD, actualLoad));
            ASSERTV(1.0f == X.max_load_factor());

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                ExceptionGuard<Obj> guard(&X, L_, &sc);

                mX.max_load_factor(1.0 / 4);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(1.0 / 4 == X.max_load_factor());
            ASSERTV(LOAD > X.load_factor());

            const float LOAD2 = X.load_factor();
            actualLoad = static_cast<double>(X.size())
                                       / static_cast<double>(X.bucket_count());
            ASSERTV(LOAD2, actualLoad, nearlyEqual<double>(LOAD2, actualLoad));

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        if (veryVeryVerbose) Q(Test insert wont alloc after 'reserve');
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            for (size_t len2 = 0; len2 <= LENGTH; ++len2) {
                Obj mX(&sa);    const Obj& X = mX;

                TestValues values(SPEC, &sa);

                mX.insert(values.begin(), values.index(len2));

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    ++numPasses;

                    mX.reserve(LENGTH);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || len2 || 0 == LENGTH  || numPasses > 1);

                const size_t BC = X.bucket_count();
                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(0.9999 * static_cast<double>(LENGTH)
                                      / static_cast<double>(X.bucket_count()) <
                        X.max_load_factor());

                mX.insert(values.index(len2), values.end());

                ASSERTV(LINE, SPEC, LENGTH == X.size());
                ASSERTV(0 == verifyContainer(X, values, LENGTH));

                ASSERTV(BC == X.bucket_count());
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase24()
{
    // ------------------------------------------------------------------------
    // PRESENCE OF FUNCTION SIGNATURES
    //
    // Concern:
    //   That all functions defined by the standard are at least defined.
    //
    // Plan:
    //   Take pointers to functions and verify they're non-zero.
    // ------------------------------------------------------------------------

    // TBD: For C++11, fix up all places where 'C++11' appears in comments
    // below.

    if (verbose) printf("PRESENCE OF FUNCTION SIGNATURES\n"
                        "===============================\n");

    if (verbose) printf("Establish types exist\n");
    {
        typedef typename Obj::key_type Kt;
        typedef typename Obj::value_type Vt;
        typedef typename Obj::hasher Ht;
        typedef typename Obj::key_equal Eq;
        typedef typename Obj::allocator_type Al;
        typedef typename Obj::pointer Pt;
        typedef typename Obj::const_pointer Cpt;
        typedef typename Obj::reference Ref;
        typedef typename Obj::const_reference Cref;
        typedef typename Obj::size_type St;
        typedef typename Obj::difference_type Dt;
        typedef typename Obj::iterator It;
        typedef typename Obj::const_iterator CIt;
        typedef typename Obj::local_iterator Lit;
        typedef typename Obj::const_local_iterator CLit;

        BSLMF_ASSERT((bslmf::IsSame<KEY, Kt  >::value));
        BSLMF_ASSERT((bslmf::IsSame<It,  CIt >::value));
        BSLMF_ASSERT((bslmf::IsSame<Lit, CLit>::value));
        BSLMF_ASSERT((bslmf::IsSame<Kt,  Vt  >::value));
        BSLMF_ASSERT((bslmf::IsSame<Kt *, Pt >::value));
        BSLMF_ASSERT((bslmf::IsSame<const Kt *, Cpt>::value));
        BSLMF_ASSERT((bslmf::IsSame<size_t,St>::value));
        BSLMF_ASSERT((bslmf::IsSame<HASH,  Ht>::value));
        BSLMF_ASSERT((bslmf::IsSame<EQUAL, Eq>::value));
        BSLMF_ASSERT((bslmf::IsSame<Ref, Kt &>::value));
        BSLMF_ASSERT((bslmf::IsSame<Cref, const Kt &>::value));
        BSLMF_ASSERT((bslmf::IsSame<ALLOC, Al>::value));
    }

    if (verbose) printf("Establish c'tors exist\n");

    const typename Obj::size_type zero = 0;

    {
        Obj o;
        (void) o;
    }

    {
        Obj o(zero);
        (void) o;
    }

    {
        Obj o(zero,
              HASH());
        (void) o;
    }

    {
        Obj o(zero,
              HASH(),
              EQUAL());
        (void) o;
    }

    {
        Obj o(zero,
              HASH(),
              EQUAL(),
              ALLOC());
        (void) o;
    }

    TestValues VALUES;

    {
        Obj o(VALUES.begin(), VALUES.end());
        (void) o;
    }

    VALUES.resetIterators();

    {
        Obj o(VALUES.begin(), VALUES.end(),
              zero);
        (void) o;
    }

    VALUES.resetIterators();

    {
        Obj o(VALUES.begin(), VALUES.end(),
              zero,
              HASH());
        (void) o;
    }

    VALUES.resetIterators();

    {
        Obj o(VALUES.begin(), VALUES.end(),
              zero,
              HASH(),
              EQUAL());
        (void) o;
    }

    VALUES.resetIterators();

    {
        Obj o(VALUES.begin(), VALUES.end(),
              zero,
              HASH(),
              EQUAL(),
              ALLOC());
        (void) o;
    }

    {
        Obj o;    const Obj& O = o;
        Obj o2(O);
        (void) o2;
    }

    // unordered_set(unordered_set&&);    // <- C++11

    {
        typename Obj::allocator_type a;
        Obj o(a);
        (void) o;
    }

    // ~unordered_set();                  // Tested by ALL the above

    if (verbose) printf("Establish member functions exist\n");

    {
        typedef Obj& (Obj::*MemPtr)(const Obj&);
        MemPtr mp = &Obj::operator=;
        (void) mp;
    }

    // Obj& operator=(Obj&&);             // <- C++11

    // Obj& operator=(initializer_list<Obj::value_type>);    // <- C++11

    {
        typedef typename Obj::allocator_type (Obj::*MemPtr)() const;
                                                              // C++11 noexcept
        MemPtr mp = &Obj::get_allocator;
        (void) mp;
    }

    {
        typedef bool (Obj::*MemPtr)() const;  // C++11 noexcept
        MemPtr mp = &Obj::empty;
        (void) mp;
    }

    {
        typedef typename Obj::size_type (Obj::*MemPtr)() const;
                                                              // C++11 noexcept
        MemPtr mp = &Obj::size;
        (void) mp;
        mp = &Obj::max_size;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MemPtr)();  // C++11 noexcept
        MemPtr mp = &Obj::begin;
        (void) mp;
        mp = &Obj::end;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MemPtr)() const;
                                                              // C++11 noexcept
        MemPtr mp = &Obj::begin;
        (void) mp;
        mp = &Obj::end;
        (void) mp;
        mp = &Obj::cbegin;
        (void) mp;
        mp = &Obj::cend;
        (void) mp;
    }

    // typedef template <class... Args>
    //                          bsl::pair <Iter, bool> emplace(Args&&... args);
    //         C++11

    // typedef template <class... Args>
    //                                Iter emplace_hint(CIter, Args&&... args);
    //         C++11

    typedef bsl::pair<Iter, bool> InsertPair;

    {
        typedef InsertPair (Obj::*MemPtr)(const typename Obj::value_type&);

        MemPtr mp = &Obj::insert;
        (void) mp;
    }

    //  InsertPair (Obj::*MemPtr)(typename Obj::value_type&&); C++11

    {
        typedef Iter (Obj::*MemPtr)(CIter, const typename Obj::value_type&);
        MemPtr mp = &Obj::insert;
        (void) mp;
    }

    // Iter (Obj::*MemPtr)(CIter, const Obj::value_type&&); C++11

    {
        typedef typename bsltf::TestValuesArray<KEY>::iterator InputIterator;
        typedef void (Obj::*MemPtr)(InputIterator, InputIterator);
        MemPtr mp = &Obj::insert;
        (void) mp;
    }

    // void insert(initializer_list<Obj::value_type>);    C++11

    {
        typedef Iter (Obj::*MemPtr)(CIter);
        MemPtr mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef SizeType (Obj::*MemPtr)(const KEY&);
        MemPtr mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MemPtr)(CIter, CIter);
        MemPtr mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef void (Obj::*MemPtr)();                       // C++11 noexcept
        MemPtr mp = &Obj::clear;
        (void) mp;
    }

    {
        typedef void (Obj::*MemPtr)(Obj&);
        MemPtr mp = &Obj::swap;
        (void) mp;
    }

    {
        typedef typename Obj::hasher (Obj::*MemPtr)() const;
        MemPtr mp = &Obj::hash_function;
        (void) mp;
    }

    {
        typedef typename Obj::key_equal (Obj::*MemPtr)() const;
        MemPtr mp = &Obj::key_eq;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MemPtr)(const KEY&);
        MemPtr mp = &Obj::find;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MemPtr)(const KEY&) const;
        MemPtr mp = &Obj::find;
        (void) mp;
    }

    {
        typedef SizeType (Obj::*MemPtr)(const KEY&) const;
        MemPtr mp = &Obj::count;
        (void) mp;
    }

    {
        typedef bsl::pair<Iter, Iter> (Obj::*MemPtr)(const KEY&);
        MemPtr mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef bsl::pair<CIter, CIter> (Obj::*MemPtr)(const KEY&) const;
        MemPtr mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef SizeType (Obj::*MemPtr)() const;    // C++ noexcept
        MemPtr mp = &Obj::bucket_count;
        (void) mp;
//      mp = &Obj::max_bucket_count;    // TBD: error.  method calls
//                                      // d_impl.maxNumOfBuckets();
//                                      // should be
//                                      // d_impl.maxNumBuckets();
//      (void) mp;
    }

    {
        typedef SizeType (Obj::*MemPtr)(SizeType) const;
        MemPtr mp = &Obj::bucket_size;
        (void) mp;
    }

    {
        typedef SizeType (Obj::*MemPtr)(const KEY&) const;
        MemPtr mp = &Obj::bucket;
        (void) mp;
    }

    typedef typename Obj::local_iterator       LocalIterator;
    typedef typename Obj::const_local_iterator ConstLocalIterator;

    {
        typedef LocalIterator (Obj::*MemPtr)(SizeType);
        MemPtr mp = &Obj::begin;
        (void) mp;
        mp = &Obj::end;
        (void) mp;
    }

    {
        typedef ConstLocalIterator (Obj::*MemPtr)(SizeType) const;
        MemPtr mp = &Obj::begin;
        (void) mp;
        mp = &Obj::end;
        (void) mp;
        mp = &Obj::cbegin;
        (void) mp;
        mp = &Obj::cend;
        (void) mp;
    }

    {
        typedef float (Obj::*MemPtr)() const;        // C++11 noexcept
        MemPtr mp = &Obj::load_factor;
        (void) mp;
        mp = &Obj::max_load_factor;
        (void) mp;
    }

    {
        typedef void (Obj::*MemPtr)(float);
        MemPtr mp = &Obj::max_load_factor;
        (void) mp;
    }

    {
        typedef void (Obj::*MemPtr)(SizeType);
        MemPtr mp = &Obj::rehash;
        (void) mp;
        mp = &Obj::reserve;
        (void) mp;
    }

    {
        using namespace bsl;

        {
            typedef void (*FuncPtr)(Obj&, Obj&);
            FuncPtr fp = &swap;
            (void) fp;
        }

        {
            typedef bool (*FuncPtr)(const Obj&, const Obj&);
            FuncPtr fp = &operator==;
            (void) fp;
            fp = &operator!=;
            (void) fp;
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits
    // ------------------------------------------------------------------------

    // Verify set defines the expected traits.
    BSLMF_ASSERT((1 ==
                    bslalg::HasStlIterators<bsl::unordered_set<KEY> >::value));
    BSLMF_ASSERT((1 ==
                  bslma::UsesBslmaAllocator<bsl::unordered_set<KEY> >::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.
    typedef bsl::unordered_set<KEY, HASH, EQUAL,StlAlloc> ObjStlAlloc;
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<ObjStlAlloc>::value));

    // Verify unordered_set does not define other common traits.
    BSLMF_ASSERT((0 ==
                 bsl::is_trivially_copyable<bsl::unordered_set<KEY> >::value));

    BSLMF_ASSERT((0 ==
         bslmf::IsBitwiseEqualityComparable<bsl::unordered_set<KEY> >::value));

    BSLMF_ASSERT((1 ==
                   bslmf::IsBitwiseMoveable<bsl::unordered_set<KEY> >::value));

    BSLMF_ASSERT((0 ==
                 bslmf::HasPointerSemantics<bsl::unordered_set<KEY> >::value));

    BSLMF_ASSERT((0 ==
               bsl::is_trivially_default_constructible<
                                            bsl::unordered_set<KEY> >::value));

}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING STL ALLOCATOR
    //
    // Concern:
    //: 1 A standard compliant allocator can be used instead of
    //:   'bsl::allocator'.
    //:
    //: 2 Methods that uses the allocator (e.g., variations of constructor,
    //:   'insert' and 'swap') can successfully populate the object.
    //:
    //: 3 'KEY' types that allocate memory uses the default allocator instead
    //:   of the object allocator.
    //:
    //: 4 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Using a loop base approach, create a list of specs and their
    //:   expected value.  For each spec:
    //:
    //:   1 Create an object using a standard allocator through multiple ways,
    //:     including: range-based constructor, copy constructor, range-based
    //:     insert, multiple inserts, and swap.
    //:
    //:   2 Verify the value of each objects is as expected.
    //:
    //:   3 For types that allocate memory, verify memory for the elements
    //:     comes from the default allocator.
    //
    // Testing:
    //  CONCERN: 'set' is compatible with a standard allocator.
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::unordered_set<KEY, HASH, EQUAL, StlAlloc> ObjStlAlloc;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_results;
        const ptrdiff_t   LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results, &scratch);
        ASSERT(0 <= LENGTH);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            ObjStlAlloc mX(BEGIN, END);  const ObjStlAlloc& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH <= da.numBlocksInUse());

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH == da.numBlocksInUse());

            ObjStlAlloc mZ;  const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH <= da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                bsl::pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, RESULT.second);
                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH <= da.numBlocksInUse());
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::unordered_set<int,
                                   TestHashFunctor<int>,
                                   TestEqualityComparator<int>,
                                   StlAlloc> TestObjIntStlAlloc;

        TestObjIntStlAlloc mX;
        mX.insert(1);
        TestObjIntStlAlloc mY;
        mY = mX;
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING 'HASH' and 'EQUAL'
    //
    // Concern:
    //: 1 Both functor and function type can be used.
    //:
    //: 2 If a comparator is not supplied, it defaults to 'std::less'.
    //:
    //: 3 The comparator is set correctly.
    //:
    //: 4 'key_comp' and 'value_comp' return the comparator that was passed in
    //:   on construction.
    //:
    //: 5 The supplied comparator is used in all operations involving
    //:   comparisons instead of 'operator <'.
    //:
    //: 6 Comparator is properly propagated on copy construction, copy
    //:   assignment, and swap.
    //:
    //: 7 Functor with a non-const function call operator can be used -- any
    //:   non-const operation on the set that utilizes the comparator can be
    //:   invoked from a non modifiable reference of a set.
    //
    // Plan:
    //: 1 Create default object and verify comparator is 'std::less'.  (C-2)
    //:
    //: 2 Create object with a function type comparator.
    //:
    //: 3 Create object with functor type comparator for all constructor.
    //:   (C-1, 3..4)
    //:
    //: 4 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 5 For each row (representing a distinct object value set, 'V') in the
    //:   table described in P-4:
    //:
    //:   1 Create a comparator object using the greater than operator for
    //:     comparisons.
    //:
    //:   2 Create an object having the value 'V' using the range constructor
    //:     passing in the the comparator created in P-5.1.  Verify that the
    //:     object's value.  (P-5)
    //:
    //:   3 Copy construct an object passing in the object created in P-5.2,
    //:     verify that the comparator compare equals to the one created in
    //:     P-5.1.  (P-6)
    //:
    //:   4 Default construct an object and assign to the object from the one
    //:     created in P-5.2.  Verify that the comparator of the new object
    //:     compare equals to the one created in P-5.1.  (P-6)
    //:
    //:   5 Default construct an object and swap this object with the one
    //:     created in P-5.2.  Verify that the comparator of the new object
    //:     compare equals to the one created in P-5.1, and the original object
    //:     has a default constructed comparator.  (P-6)
    //:
    //: 6 Repeat P-5 except with a comparator having non-const function call
    //:   operator.  (P-7
    //
    // Testing:
    //   key_compare key_comp() const;
    //   value_compare value_comp() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    {
        bsl::unordered_set<int> X;
        bsl::hash<int>     hsh = X.hash_function();
        bsl::equal_to<int> eq  = X.key_eq();

        (void) hsh;    // quash potential compiler warning
        (void) eq;     // quash potential compiler warning
    }

    static const int ID[] = { 0, 1, 2 };
    const int NUM_ID = sizeof ID / sizeof *ID;

    for (int ti = 0; ti < NUM_ID; ++ti) {
        const int hId = ti;
        const int eId = ti * 7 + 5;
        const HASH  H(hId);
        const EQUAL E(eId);
        {
            const Obj X(0, H, E);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
        {
            const Obj X(0, H, E, &scratch);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
        {
            const Obj X((KEY *) 0, (KEY *) 0, 0, H, E, &scratch);
            ASSERTV(ti, hId == X.hash_function().id());
            ASSERTV(ti, eId == X.key_eq().id());
            ASSERTV(ti, 0   == X.hash_function().count());
            ASSERTV(ti, 0   == X.key_eq().count());
        }
    }

    static const struct {
        int         d_line;             // source line number
        const char *d_spec;             // spec
    } DATA[] = {
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ACBD"      },
        { L_,  "BCDAE"     },
        { L_,  "GFEDCBA"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "BCDEFGHIA" }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const HASH DH(0, false);    // delinquent hash, always returns 0
    const HASH H;               // normal hash

    const EQUAL E;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);
        const TestValues  EXP(DATA[ti].d_spec, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mW(BEGIN, END, 0, DH, E);  const Obj& W = mW;

            ASSERTV(LINE, 0 == verifyContainer(W, EXP, LENGTH));
            ASSERTV(LINE, H == W.hash_function());
            ASSERTV(LINE, E == W.key_eq());

            Obj mX(W);  const Obj& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, H == X.hash_function());
            ASSERTV(LINE, E == X.key_eq());

            Obj mY;  const Obj& Y = mY;
            mY = mW;
            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, H == Y.hash_function());
            ASSERTV(LINE, E == Y.key_eq());

            Obj mZ;  const Obj& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, H == Z.hash_function());
            ASSERTV(LINE, E == Z.key_eq());
            ASSERTV(LINE, H == W.hash_function());
            ASSERTV(LINE, E == W.key_eq());
        }

        CONT.resetIterators();

        {
            Obj mX(0, DH, E);  const Obj& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        CONT.resetIterators();

        {
            Obj mX(0, DH, E);  const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                bsl::pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

#if 1 // TBD non-const comparator
    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);
        const TestValues  EXP(DATA[ti].d_spec, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        TestNonConstHashFunctor<KEY>   NCH;
        TestNonConstEqualityComparator<KEY> NCE;
        typedef bsl::unordered_set<KEY,
                                   TestNonConstHashFunctor<KEY>,
                                   TestNonConstEqualityComparator<KEY>,
                                   ALLOC> ObjNCH;


        {
            ObjNCH mW(BEGIN, END);  const ObjNCH& W = mW;

            ASSERTV(LINE, 0 == verifyContainer(W, EXP, LENGTH));
            ASSERTV(LINE, NCH == W.hash_function());
            ASSERTV(LINE, NCE == W.key_eq());

            ObjNCH mX(W);  const ObjNCH& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, NCH == X.hash_function());
            ASSERTV(LINE, NCE == X.key_eq());

            ObjNCH mY;  const ObjNCH& Y = mY;
            mY = mW;
            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, NCH == Y.hash_function());
            ASSERTV(LINE, NCE == Y.key_eq());

            ObjNCH mZ;  const ObjNCH& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, NCH == Z.hash_function());
            ASSERTV(LINE, NCH == W.hash_function());
            ASSERTV(LINE, NCE == Z.key_eq());
            ASSERTV(LINE, NCE == W.key_eq());
            ASSERTV(LINE, TestNonConstHashFunctor<KEY>() == W.hash_function());
            ASSERTV(LINE, TestNonConstEqualityComparator<KEY>() == W.key_eq());
        }

        CONT.resetIterators();

        {
            ObjNCH mX;  const ObjNCH& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        CONT.resetIterators();

        {
            ObjNCH mX;  const ObjNCH& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                bsl::pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING 'max_size' and 'empty'
    //
    // Concern:
    //: 1 'max_size' returns the 'max_size' of the supplied allocator.
    //:
    //: 2 'empty' returns 'true' only when the object is empty.
    //
    // Plan:
    //: 1 Run each function and verify the result.  (C-1..2)
    //
    // Testing:
    //  bool empty() const;
    //  size_type max_size() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa(veryVeryVerbose);

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj X;
        ALLOC a;
        ASSERTV(~(size_t)0 / sizeof(KEY) >= X.max_size());
        ASSERTV(a.max_size(), X.max_size(),
                a.max_size() ==  X.max_size());
    }

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEFG"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "ABCDEFGHI" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());

            mX.clear();

            ASSERTV(LINE, SPEC, true == X.empty());
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concern:
    //: 1 'erase' with iterators returns the iterator right after the erased
    //:   value(s).
    //:
    //: 2 'erase' with 'key' returns 1 if 'key' exist, and 0 with no other
    //:   effect otherwise.
    //:
    //: 3 Erased values are removed.
    //:
    //: 4 Erasing do not throw.
    //:
    //: 5 No memory is allocated.
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 For each distinct length, 'l':
    //:
    //:   1 For each value, v, that would be in the object with that length:
    //:
    //:     1 Create an object with length, 'l'.
    //:
    //:     2 Find 'v' to get its iterator
    //:
    //:     2 Erase 'v' with 'erase(const_iterator position)'.  (C-4)
    //:
    //:     2 Verify return value.  (C-1)
    //:
    //:     3 Verify value is erased with 'find'.  (C-3)
    //:
    //:     4 Verify no memory is allocated.  (C-5)
    //:
    //: 2 Repeat P-1 with 'erase(const key_type& key)' (C-2).
    //:
    //: 3 For range erase, call erase on all possible range of for each length,
    //:   'l' and verify result is as expected.
    //:
    //: 4 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid values, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //   size_type erase(const key_type& key);
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose)printf("\nTesting 'erase(pos)' on non-empty unordered_set.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_results;
            const size_t      LENGTH = strlen(DATA[ti].d_results);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);

                Obj mX(&oa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, &oa); const Obj& Y = mY;

                Iter pos    = getIterForIndex(X, tj);
                Iter before = 0 == tj ? mX.end()
                                      : getIterForIndex(X, tj - 1);
                Iter after = pos;    ++after;

                const KEY k(*pos);

                const size_t COUNT = X.count(k);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&oa);

                {
                    Iter RESULT;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        RESULT = mX.erase(pos);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(RESULT == after);
                    ASSERTV(1 == numPasses);
                }

                ASSERTV(oam.isTotalSame());
                ASSERTV(dam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }

                ASSERTV(COUNT - 1 == X.count(k));
                ASSERTV(X.size() == LENGTH - 1);

                for (CIter it = Y.begin(); Y.end() != it; ++it) {
                    if (k != *it) {
                        ASSERTV(X.count(*it) > 0);
                    }
                }

                if (0 == tj) {
                    ASSERTV(X.begin() == after);
                }
                else {
                    pos = before; ++pos;
                    ASSERT(pos == after);
                }
            }
        }
    }

    if (verbose)printf("\nTesting 'erase(key)' on non-empty unordered_set.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_results;
            const size_t      LENGTH = strlen(DATA[ti].d_results);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);

                Obj mX(&oa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, &oa); const Obj& Y = mY;

                Iter pos    = getIterForIndex(X, tj);
                Iter before = 0 == tj ? mX.end()
                                      : getIterForIndex(X, tj - 1);
                Iter after = pos;
                const KEY k(*pos);

                while (mX.end() != after && k == *after) {
                    ++after;
                }

                const size_t COUNT = X.count(k);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&oa);

                size_t RESULT;
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    RESULT = mX.erase(k);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(COUNT == RESULT);
                ASSERT(1 == numPasses);

                ASSERTV(oam.isTotalSame());
                ASSERTV(dam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }

                ASSERTV(!X.count(k));
                ASSERTV(X.size() == LENGTH - COUNT);

                for (CIter it = Y.begin(); Y.end() != it; ++it) {
                    ASSERT((k != *it) == !!X.count(*it));
                }

                if (0 == tj) {
                    ASSERTV(X.begin() == after);
                }
                else {
                    pos = before; ++pos;
                    ASSERT(pos == after);
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_results;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0;  tj <= LENGTH; ++tj) {
                for (size_t tk = tj; tk <= LENGTH; ++tk) {
                    bslma::TestAllocator oa("object", veryVeryVerbose);

                    Obj mX(&oa);    const Obj& X = gg(&mX, SPEC);
                    Obj mY(X, &oa); const Obj& Y = mY;

                    CIter start  = getIterForIndex(X, tj);
                    CIter finish = getIterForIndex(X, tk);

                    Obj mZ(start, finish, 2, HASH(), EQUAL(), &oa);
                    const Obj& Z = mZ;

                    const size_t NUM_ELEMENTS = tk - tj;
                    ASSERTV(NUM_ELEMENTS == Z.size());

                    CIter before;
                    if (tj > 0) {
                        before = getIterForIndex(X, tj - 1);
                    }

                    bslma::TestAllocatorMonitor oam(&oa);
                    bslma::TestAllocatorMonitor dam(&oa);

                    CIter RESULT;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;

                        RESULT = mX.erase(start, finish);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(RESULT == finish);
                    ASSERTV(1 == numPasses);

                    ASSERTV(oam.isTotalSame());
                    ASSERTV(dam.isTotalSame());
                    if (TYPE_ALLOC && 1 <= NUM_ELEMENTS) {
                        ASSERTV(LINE, tj, tk, oam.isInUseDown());
                    }

                    ASSERTV(X.size() == LENGTH - NUM_ELEMENTS);

                    for (CIter cit = Y.begin(); Y.end() != cit; ++cit) {
                        const size_t z = Z.count(*cit);
                        const size_t x = X.count(*cit);
                        ASSERT(1 == (x | z));
                        ASSERT(x != z);
                    }

                    if (tj > 0) {
                        CIter pos = before;
                        ++pos;
                        ASSERTV(finish == pos);
                    }
                    else {
                        ASSERTV(X.begin() == finish);
                    }
                }
            }
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("'erase\n");
        {
            const TestValues VALUES;

            Obj mX;
            Iter it = mX.insert(mX.end(), VALUES[0]);

            ASSERT_SAFE_FAIL(mX.erase(mX.end()));
            ASSERT_SAFE_PASS(mX.erase(it));
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase17()
{
    // ------------------------------------------------------------------------
    // RANGE 'insert'
    //
    // Concern:
    //: 1 All values within the range [first, last) are inserted.
    //:
    //: 2 Each iterator is deferenced only once.
    //:
    //: 3 Repeated values are ignored.
    //:
    //: 4 Any memory allocation is from the object allocator.
    //:
    //: 5 There is no temporary memory allocation from any allocator.
    //:
    //: 6 Inserting no elements allocates no memory.
    //:
    //: 7 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value set, 'V') in the
    //:   table described in P-1:
    //:
    //:   1 Use the range constructor to create a object with part of the
    //:     elements in 'V'.
    //:
    //:   2 Insert the rest of 'V' under the presence of exception.  (C-7)
    //:
    //:   3 Verify the object's value.  (C-1-3)
    //:
    //:   4 If the range is empty, verify no memory is allocated  (C-6)
    //:
    //:   5 Verify no temporary memory is allocated.  (C-5)
    //:
    //:   6 Verify no memory is allocated from the default allocator (C-4)
    //
    // Testing:
    //   void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
    // ------------------------------------------------------------------------

    if (verbose) printf("Testing Range Insert\n"
                        "====================\n");

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results);

        TestValues CONT(SPEC);
        for (size_t tj = 0; tj <= CONT.size(); ++tj) {

            CONT.resetIterators();
            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator MID   = CONT.index(tj);
            typename TestValues::iterator END   = CONT.end();

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator oa("object", veryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, 2, HASH(), EQUAL(), &oa);  const Obj& X = mX;

            bslma::TestAllocatorMonitor oam(&oa);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                CONT.resetIterators();

                mX.insert(MID, END);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            if (tj == CONT.size()) {
                ASSERTV(LINE, oam.isTotalSame());
            }
            ASSERTV(LINE, tj, 0 == verifyContainer(X, EXP, LENGTH));

            ASSERTV(LINE, tj, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
    //
    // Concerns:
    //: 1 'insert' returns a pair containing an iterator and a 'bool'
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and the the existing element if it did.
    //:
    //: 3 The 'bool' returned is 'true' if a new element is inserted, and
    //:   'false' otherwise.
    //:
    //: 4 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 When a hint is provided, it is ignored.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   3 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 2 Repeat P-1, providing a 'hint' iterator that has been initialized to
    //:   garbage and observe that it makes no difference.
    //:
    //: 3 Repeat P-1 under the presence of exception  (C-6)
    //
    // Testing:
    //   bsl::pair<iterator, bool> insert(const value_type& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<KEY>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  ----           --------

        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 10;

    if (verbose) printf("\nTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                bsl::pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB == AA);
                    ASSERTV(LINE, tj, A,  B,  B  ==  A);
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE));
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                Iter hint;        // Give it a garbage value.
                native_std::memset(&hint,
                                   0xaf ^ (tj * 97),
                                   sizeof(hint));
                Iter RESULT = mX.insert(hint, VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB == AA);
                    ASSERTV(LINE, tj, A,  B,  B  ==  A);
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE));
                }
            }
        }
    }

    if (verbose) printf("\nTesting 'insert' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::pair<Iter, bool> RESULT;
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    Iter hint;                  // Give it a garbage value.
                    native_std::memset(&hint,
                                       0xaf ^ (tj * 97),
                                       sizeof(hint));
                    if (tj & 1) {
                        RESULT.first  = mX.insert(hint, VALUES[tj]);
                        RESULT.second = IS_UNIQ;
                    }
                    else {
                        RESULT = mX.insert(VALUES[tj]);
                    }
                guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                ASSERTV(LINE, tj, SIZE,
                            VALUES[tj] == *(RESULT.first));
                ASSERTV(LINE, tj, IS_UNIQ, numPasses,
                                                    IS_UNIQ || 1 == numPasses);

                if (IS_UNIQ) {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
                else {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE));
                }
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    // Concerns:
    //: 1 The range '[begin(), end())' contains all values inserted into the
    //:   container in ascending order.
    //:
    //: 2 The range '[rbegin(), rend())' contains all values inserted into the
    //:   container in descending order.
    //:
    //: 3 'iterator' is a pointer to 'const KEY'.
    //:
    //: 4 'const_iterator' is a pointer to 'const KEY'.
    //:
    //: 5 'reverse_iterator' and 'const_reverse_iterator' are implemented by
    //:   the (fully-tested) 'bslstl_ReverseIterator' over a pointer to
    //:   'const TYPE'.
    //
    // Plan:
    //: 1 For each value given by variety of specifications of different
    //:   lengths:
    //:
    //:   1 Create an object this value, and access each element in sequence
    //:     and in reverse sequence, both as a modifiable reference (setting it
    //:     to a default value, then back to its original value, and as a
    //:     non-modifiable reference.  (C-1..3)
    //:
    //: 2 Use 'bsl::is_same' to assert the identity of iterator types.
    //:   (C-4..6)
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   const_iterator begin();
    //   const_iterator end();
    // ------------------------------------------------------------------------

    BSLMF_ASSERT((bsl::is_same<Iter, CIter>::value));

    const TestValues VALUES;

    bslma::TestAllocator oa(veryVeryVerbose);

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ACBD"      },
        { L_,  "BCDAE"     },
        { L_,  "GFEDCBA"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "BCDEFGHIA" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("Testing 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        ASSERTV(1 == (bsl::is_same<typename Iter::pointer,
                                   const KEY*>::value));
        ASSERTV(1 == (bsl::is_same<typename Iter::reference,
                                   const KEY&>::value));
        ASSERTV(1 == (bsl::is_same<typename CIter::pointer,
                                   const KEY*>::value));
        ASSERTV(1 == (bsl::is_same<typename CIter::reference,
                                   const KEY&>::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&oa);  const Obj& X = mX;
            gg(&mX, SPEC);

            if (verbose) { P_(LINE); P(SPEC); }

            ASSERT(mX.begin() == mX.cbegin());
            ASSERT(mX.begin() ==  X.begin());
            ASSERT(mX.begin() ==  X.cbegin());
            ASSERT(mX.end()   == mX.cend());
            ASSERT(mX.end()   ==  X.end());
            ASSERT(mX.end()   ==  X.cend());

            ASSERTV(LENGTH, mX.size(), LENGTH == mX.size());

            if (LENGTH == 0) {
                ASSERTV(mX.begin() == mX.end());
            }

            {
                Obj mY(&oa);
                size_t i = 0;
                for (Iter iter = mX.begin(); iter != mX.end(); ++iter,  ++i) {
                    bsl::pair<Iter, bool> ret = mY.insert(*iter);
                    ASSERTV(ret.second);             // hasn't been inserted
                                                     // before
                    ASSERTV(*ret.first == *iter);
                }
                ASSERTV(LINE, LENGTH == i);
                ASSERTV(X == mY);
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING FIND, EQUAL_RANGE, COUNT
    //
    // Concern:
    //: 1 If the key being searched exists in the container, 'find' and
    //:   'lower_bound' returns the iterator referring the the existing
    //:   element, 'upper_bound' returns the iterator to the element after the
    //:   searched element.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator, 'lower_bound' and 'upper_bound' returns
    //:   the iterator to the smallest element greater than searched element.
    //:
    //: 3 'equal_range(key)' returns
    //:   'std::make_pair(lower_bound(key), upper_bound(key))'.
    //:
    //: 2 'count' returns the number of elements with the same value as defined
    //:   by the comparator.
    //:
    //: 3 Both the 'const' and non-'const' versions returns the same value.
    //:
    //: 4 No memory is allocated, from either the object allocator nor from
    //:   the default allocator.
    // ------------------------------------------------------------------------

    if (verbose) printf("Testing find, equal_range, count\n"
                        "================================\n");

    BSLMF_ASSERT((bslmf::IsSame<Iter, CIter>::VALUE));

    typedef bsl::pair<Iter, Iter> Pair;

    bslma::TestAllocator         oa("object",  veryVeryVerbose);
    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    TestValues VALUES(&oa);

    for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
        const char *SPEC = DEFAULT_DATA[i].d_spec;

        TestValues tv(SPEC, &oa);

        Obj mX(tv.begin(), tv.end());    const Obj& X = mX;

        for (char c = 'A'; c < 'Z'; ++c) {
            KEY k = KEY(VALUES[c - 'A']);

            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            Iter it;
            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                it = mX.find(k);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERT(it == X.find(k));
            ASSERT(1 == numPasses);

            if (native_std::strchr(SPEC, c)) {
                ASSERT(isConstValue(*it));

                ASSERT(*it  ==  k);
                ASSERT(BSLS_UTIL_ADDRESSOF(*it) != BSLS_UTIL_ADDRESSOF(k));

                ASSERT(1 == mX.count(k));
                ASSERT(1 ==  X.count(k));
            }
            else {
                X.end() == it;

                ASSERT(0 == mX.count(k));
                ASSERT(0 ==  X.count(k));
            }

            Pair pr;
            {
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    pr = mX.equal_range(k);
                    ASSERT(X.equal_range(k) == pr);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);
            }

            ASSERTV(it == pr.first);

            if (native_std::strchr(SPEC, c)) {
                Iter after = pr.first; ++after;
                ASSERT(after == pr.second);
            }
            else {
                ASSERTV(pr.first == pr.second);
                ASSERTV(X.end() == pr.first);
            }

            ASSERTV(oam.isTotalSame());
            ASSERTV(dam.isTotalSame());
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE C'TORS
    //
    // Concern:
    //   That all c'tors taking a range of objects of type 'KEY' function
    //   correctly.
    // ------------------------------------------------------------------------

    TestValues VALUES;

    if (verbose) Q(First test 'count()');
    {
        for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
            const int LINE   = DEFAULT_DATA[i].d_line;
            const char *SPEC = DEFAULT_DATA[i].d_spec;

            Obj mX;    const Obj& X = mX;
            gg(&mX, SPEC);

            for (char c = 'A'; c <= 'Z'; ++c) {
                KEY k = VALUES[c - 'A'];

                bool EXP = native_std::strchr(SPEC, c);

                ASSERTV(LINE, SPEC, c, EXP, EXP == X.count(k));
            }
        }
    }

    if (verbose) Q(Now test range creation);
    {
        typedef bsltf::TestValuesArrayIterator<KEY> Iterator;

        HASH  defaultHash(7);
        EQUAL defaultEqual(9);

        ASSERTV(!(HASH()  == defaultHash));
        ASSERTV(!(EQUAL() == defaultEqual));

        for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
            const int LINE      = DEFAULT_DATA[i].d_line;
            const char *SPEC    = DEFAULT_DATA[i].d_spec;
            const char *RESULTS = DEFAULT_DATA[i].d_results;
            const size_t LENGTH = strlen(SPEC);

            if (veryVerbose) P(SPEC);

            int done = 0;
            for (char cfg = 'a'; cfg <= 'i' ; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator sc("scratch",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag2(&sc);

                bsltf::TestValuesArray<KEY> tv(SPEC, &sc);

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator& oa  = strchr("ei", CONFIG) ? sa : da;
                bslma::TestAllocator& noa = &oa == &da           ? sa : da;

                int numPasses = 0;
                Obj *pmX;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;
                    tv.resetIterators();

                    switch (CONFIG) {
                      case 'a': {
                        pmX = new (fa) Obj(tv.begin(), tv.end());
                      } break;
                      case 'b': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 0);
                      } break;
                      case 'c': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 0,
                                           defaultHash);
                      } break;
                      case 'd': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 0,
                                           defaultHash, defaultEqual);
                      } break;
                      case 'e': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 0,
                                           defaultHash, defaultEqual, &sa);
                      } break;
                      case 'f': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 100);
                      } break;
                      case 'g': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 100,
                                           defaultHash);
                      } break;
                      case 'h': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 100,
                                           defaultHash, defaultEqual);
                      } break;
                      case 'i': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 100,
                                           defaultHash, defaultEqual, &sa);
                        ++done;
                      } break;
                      default: {
                        ASSERTV(0);
                        return;                                       // RETURN
                      } break;
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(CONFIG, LENGTH,
                                 (PLAT_EXC && (LENGTH > 0 || CONFIG >= 'f')) ==
                                                              (numPasses > 1));

                Obj& mX = *pmX;    const Obj& X = mX;

                ASSERTV(CONFIG, LENGTH, noa.numBlocksTotal(), &oa == &da,
                                                    0 == noa.numBlocksTotal());

                TestValues EXP(RESULTS, &sc);
                ASSERT(0 == verifyContainer(X, EXP, strlen(RESULTS)));

                ASSERTV((strchr("cdeghi", CONFIG) ?
                                 defaultHash  : HASH())  == X.hash_function());
                ASSERTV((strchr("dehi",   CONFIG) ?
                                 defaultEqual : EQUAL()) == X.key_eq());

                ASSERTV(CONFIG, SPEC, X.bucket_count(),
                                      CONFIG < 'f' || 100 <= X.bucket_count());

                for (char c = 'A'; c <= 'Z'; ++c) {
                    int idx = c - 'A';
                    const KEY& k = VALUES[idx];

                    native_std::size_t EXP = !!native_std::strchr(SPEC, c);

                    ASSERTV(LINE, SPEC, c, EXP, EXP == X.count(k));
                }

                fa.deleteObject(pmX);
            }

            ASSERTV(1 == done);
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g:
    //
    // Concern:
    //: 1 Since 'g' is implemented almost entirely using 'gg', we need to
    //:   verify only that the arguments are properly forwarded.
    //:
    //: 2 'g' does not affect the test allocator, and that 'g' returns an
    //:   object by value.
    //
    // Plan:
    //: 1 For each SPEC in a short list of specifications:
    //:
    //:   1 Compare the object returned (by value) from the generator function,
    //:     'g(SPEC)' with the value of a newly constructed OBJECT configured
    //:     using 'gg(&OBJECT,  SPEC)'.
    //:
    //:   2 Compare the results of calling the allocator's 'numBlocksTotal' and
    //:     'numBytesInUse' methods before and after calling 'g' in order to
    //:     demonstrate that 'g' has no effect on the test allocator.
    //:
    //:   3 Use 'sizeof' to confirm that the (temporary) returned by 'g'
    //:     differs in size from that returned by 'gg'.
    //
    // Testing:
    //   unordeered_set g(const char *spec);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    static const char *SPECS[] = {
        "", "A", "B", "C", "D", "E", "ABCDE",
        0  // null string required as last element
    };

    if (verbose)
        printf("\nCompare values produced by 'g' and 'gg' "
               "for various inputs.\n");

    for (int ti = 0; SPECS[ti]; ++ti) {
        const char *SPEC = SPECS[ti];
        if (veryVerbose) { P_(ti);  P(SPEC); }

        Obj mX(&oa);
        gg(&mX, SPEC);  const Obj& X = mX;

        if (veryVerbose) {
            printf("\t g = ");
            bsls::BslTestUtil::callDebugprint(g(SPEC));
            printf("\n");

            printf("\tgg = ");
            bsls::BslTestUtil::callDebugprint(X);
            printf("\n");
        }
        const bsls::Types::Int64 TOTAL_BLOCKS_BEFORE = oa.numBlocksTotal();
        const bsls::Types::Int64 IN_USE_BYTES_BEFORE = oa.numBytesInUse();
        ASSERTV(ti, X == g(SPEC));
        const bsls::Types::Int64 TOTAL_BLOCKS_AFTER = oa.numBlocksTotal();
        const bsls::Types::Int64 IN_USE_BYTES_AFTER = oa.numBytesInUse();
        ASSERTV(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        ASSERTV(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");
    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

        Obj x(&oa);                      // runtime tests
        Obj& r1 = gg(&x, SPEC);
        Obj& r2 = gg(&x, SPEC);
        const Obj& r3 = g(SPEC);
        const Obj& r4 = g(SPEC);
        ASSERT(&r2 == &r1);
        ASSERT(&x  == &r1);
        ASSERT(&r4 != &r3);
        ASSERT(&x  != &r3);
    }
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
    //   unordered_set& operator=(const unordered_set& rhs);
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

            mZ.max_load_factor(2.0);

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
                    Obj mX( &oa);  const Obj& X  = gg(&mX,   SPEC2);
                    Obj mXX(&oa);  const Obj& XX = gg(&mXX,  SPEC2);

                    mX.max_load_factor(3.0);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        ASSERT(XX == X);

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(2.0 == X.max_load_factor());

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                    ASSERTV(LINE1, LINE2, &scratch == Z.get_allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                    // When mX and Z use different allocators, the assignment
                    // is done through the copy-swap idiom.  This means the
                    // copy is destroyed within the assignment, and in SAFE2
                    // mode the destructor calls 'isWellFormed', which
                    // allocates from the default allocator.

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
#endif
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }
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
    //   void swap(unordered_set& other);
    //   void swap(unordered_set<K, H, E, A>& a, unordered_set<K, H, E, A>& b);
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
    //   void swap(unordered_set& other);
    //   void swap(unordered_set<K, H, E, A>& a, unordered_set<K, H, E, A>& b);
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

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
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

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                mW.swap(mW);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(1 == numPasses);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                swap(mW, mW);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(1 == numPasses);

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

            mX.max_load_factor(2.0f);
            mY.max_load_factor(3.0f);

            ASSERT(2.0f == X.max_load_factor());
            ASSERT(3.0f == Y.max_load_factor());

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    mX.swap(mY);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

                ASSERT(3.0f == X.max_load_factor());
                ASSERT(2.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    swap(mX, mY);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

                ASSERT(2.0f == X.max_load_factor());
                ASSERT(3.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

#if 0       // Unlike 'set', 'uordered set' does not support swapping with
            // unequal bslma allocators.


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
#endif

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

        mX.max_load_factor(2.0);
        mY.max_load_factor(3.0);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        int numPasses = 0;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numPasses;

#if 0
            invokeAdlSwap(mX, mY);
#else
            // We know that the types of 'mX' and 'mY' do not overload the
            // unary address-of 'operator&'.

            bslalg::SwapUtil::swap(&mX, &mY);
#endif
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        ASSERTV(1 == numPasses);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        ASSERT(3.0 == X.max_load_factor());
        ASSERT(2.0 == Y.max_load_factor());

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
    //   unordered_set(const unordered_set& original);
    //   unordered_set(const unordered_set& original, const A& allocator);
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
            const size_t      LENGTH = strlen(SPEC);

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

                const Obj Y0(X);

                ASSERTV(SPEC, X == Y0);
                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

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
                if (veryVerbose) {
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");
                }

                const bsls::Types::Int64 A = oa.numBlocksTotal();

                Obj Y11(X, &oa);

                ASSERT(0 == LENGTH || oa.numBlocksTotal() > A);

                // Due of pooling of memory alloctioon, we can't predict
                // whether this insert will allocate or not.

                bsl::pair<Iter, bool> RESULT = Y11.insert(VALUES['Z' - 'A']);
                ASSERTV(true == RESULT.second);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

                size_t numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    const Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV((!PLAT_EXC || X.empty()) == (1 == numPasses));
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
    //   bool operator==(const unordered_set<K, H, E, A>& lhs,
    //                   const unordered_set<K, H, E, A>& rhs);
    //   bool operator!=(const unordered_set<K, H, E, A>& lhs,
    //                   const unordered_set<K, H, E, A>& rhs);
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

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
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

//                  bslma::TestAllocatorMonitor oaxm(&xa);
//                  bslma::TestAllocatorMonitor oaym(&ya);

                    // EQUAL::disableFunctor();
                            // TBD -- fails this test EQUAL is used to
                            // determine the equality groups, then
                            // bsl::permutation is used to determine if they're
                            // isomorphic, and bsl::permutation uses
                            // 'operator=='.  It will take a lot of work to
                            // verify that this is the case, putting it off for
                            // later.

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    const size_t NUM_BUCKETS = Y.bucket_count();
                    mY.reserve((Y.size() + 1) * 5);
                    ASSERTV(NUM_BUCKETS < Y.bucket_count());
                    ASSERTV(!EXP || X.bucket_count() != Y.bucket_count());

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

//                  EQUAL::enableFunctor();

//                  ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
//                  ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());

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
            const size_t      LENGTH = strlen(DATA[ti].d_results);
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
                      return;                                         // RETURN
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
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());

                ASSERT(0 == verifyContainer(X, EXP, LENGTH));

                ASSERT(oam.isTotalSame());

                ASSERT(X.begin()  == X.cbegin());
                ASSERT(X.end()    == X.cend());
                ASSERT((LENGTH == 0) == (X.begin()  == X.end()));
                ASSERT((LENGTH == 0) == (X.cbegin() == X.cend()));

                ASSERT(LENGTH != 0 || 1 == X.bucket_count());

                // --------------------------------------------------------

                // Verify no allocation from the non-object allocator (before
                // deletion -- in SAFE2 mode, the d'tor calls 'isWellFormed',
                // which uses the default allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

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
    //   unordered_set& gg(unordered_set *object, const char *spec);
    //   int ggg(unordered_set *object, const char *spec, int verbose = 1);
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
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
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

    const HASH  defaultHasher     = HASH(7);
    const EQUAL defaultComparator = EQUAL(9);

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
        bool done = false;
        for (char cfg = 'a'; cfg <= 'l'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                  *objPtr = 0;
            bslma::TestAllocator&  oa = strchr("cgl", CONFIG) ? sa : da;
            bslma::TestAllocator& noa = &sa == &oa ? da : sa;

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj;
                  } break;
                  case 'b': {
                      // Ambiguous -- does '0' mean 'initialNumBuckets' or
                      // 'allocator'? -- Result is same either way.

                      objPtr = new (fa) Obj(0);
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa);
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher);
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator);
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            &sa);
                  } break;
                  case 'h': {
                      objPtr = new (fa) Obj(100);
                  } break;
                  case 'i': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher);
                  } break;
                  case 'j': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator);
                  } break;
                  case 'k': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator,
                                            (bslma::Allocator *)0);
                  } break;
                  case 'l': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator,
                                            &sa);
                      done = true;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || CONFIG <= 'g') == (1 == numPasses));

            Obj&                   mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            if (CONFIG <= 'g') {
                // Verify no allocation from the object/non-object allocators.
                // NOTE THAT THIS QoI TEST IS STILL AN OPEN DESIGN ISSUE

                ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                        0 ==  oa.numBlocksTotal());
                ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());
                ASSERTV(LENGTH, CONFIG, X.bucket_count(),
                                                        1 == X.bucket_count());
            }
            else {
                ASSERTV(LENGTH, CONFIG, X.bucket_count(),
                                                      X.bucket_count() >= 100);
            }

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            ASSERTV((strchr("abch",   CONFIG) ? HASH()
                                              : HASH(7)) == X.hash_function());
            ASSERTV((strchr("abcdhi", CONFIG) ? EQUAL()
                                              : EQUAL(9)) == X.key_eq());

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

                if (veryVerbose) {
                    printf("\t\t Testing allocator exceptions\n");
                }
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                if (&oa == &da) {
                    mX.insert(VALUES[LENGTH - 1]);
                }
                else {
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
                        if (VALUE_TYPE_USES_ALLOC || expectToAllocate(LENGTH)){
                            ASSERTV(CONFIG, tam.isTotalUp());
                            ASSERTV(CONFIG, tam.isInUseUp());
                        }
                        else {
                            ASSERTV(CONFIG, tam.isTotalSame());
                            ASSERTV(CONFIG, tam.isInUseSame());
                        }
#endif

#if 0
                        // Verify no temporary memory is allocated from the
                        // object allocator.
                        // BROKEN TEST CONDITION
                        // We need to think carefully about how we allow for
                        // the allocation of the bucket-array

                        ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());
#endif
                        ASSERTV(LENGTH, CONFIG, true == RESULT.second);
                        ASSERTV(LENGTH, CONFIG,
                                VALUES[LENGTH - 1] == *(RESULT.first));

                        guard.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

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

                if (veryVerbose) {
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
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    mX.clear();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

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
        ASSERTV(done);
    }

    {
        const size_t initialNumBuckets[] = { 0, 1, 2, 3, 5, 7, 11, 13, 17, 19,
                                    23, 29, 31, 37, 41, 43, 47, 51, 100, 200 };
        enum { NUM_INITIAL_NUM_BUCKETS = sizeof initialNumBuckets /
                                                   sizeof *initialNumBuckets };

        for (int ti = 0; ti < NUM_INITIAL_NUM_BUCKETS; ++ti) {
            {
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(initialNumBuckets[ti]);    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           defaultHasher);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           defaultHasher,
                           defaultComparator);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj mX(initialNumBuckets[ti],
                           defaultHasher,
                           defaultComparator,
                           (bslma::Allocator *) 0);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }

                ASSERTV(ti, da.numBlocksTotal(),
                                       (ti > 0) || (0 == da.numBlocksTotal()));
            }
            {
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX(initialNumBuckets[ti],
                       defaultHasher,
                       defaultComparator,
                       &sa);
                const Obj& X = mX;
                ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);

                ASSERTV(0 == da.numBlocksTotal());
                ASSERTV(ti, sa.numBlocksTotal(),
                                       (ti > 0) || (0 == sa.numBlocksTotal()));
            }
        }
    }
}

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Categorizing Data
/// - - - - - - - - - - - - - -
// Unordered set are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).
//
// Suppose one is analyzing data on a set of customers, and each customer is
// categorized by several attributes: customer type, geographic area, and
// (internal) project code; and that each attribute takes on one of a limited
// set of values.  This data can be handled by creating an enumeration for each
// of the attributes:
//..
typedef enum {
    REPEAT
  , DISCOUNT
  , IMPULSE
  , NEED_BASED
  , BUSINESS
  , NON_PROFIT
  , INSTITUTE
    // ...
} CustomerCode;

typedef enum {
    USA_EAST
  , USA_WEST
  , CANADA
  , MEXICO
  , ENGLAND
  , SCOTLAND
  , FRANCE
  , GERMANY
  , RUSSIA
    // ...
} LocationCode;

typedef enum {
    TOAST
  , GREEN
  , FAST
  , TIDY
  , PEARL
  , SMITH
    // ...
} ProjectCode;
//..
// For printing these values in a human-readable form, we define these helper
// functions:
//..
static const char *toAscii(CustomerCode value)
{
    switch (value) {
      case REPEAT:     return "REPEAT";
      case DISCOUNT:   return "DISCOUNT";
      case IMPULSE:    return "IMPULSE";
      case NEED_BASED: return "NEED_BASED";
      case BUSINESS:   return "BUSINESS";
      case NON_PROFIT: return "NON_PROFIT";
      case INSTITUTE:  return "INSTITUTE";
      // ...
      default: return "(* UNKNOWN *)";
    }
}

static const char *toAscii(LocationCode value)
{
    switch (value) {
      case USA_EAST: return "USA_EAST";
      case USA_WEST: return "USA_WEST";
      case CANADA:   return "CANADA";
      case MEXICO:   return "MEXICO";
      case ENGLAND:  return "ENGLAND";
      case SCOTLAND: return "SCOTLAND";
      case FRANCE:   return "FRANCE";
      case GERMANY:  return "GERMANY";
      case RUSSIA:   return "RUSSIA";
      // ...
      default: return "(* UNKNOWN *)";
    }
}

static const char *toAscii(ProjectCode  value)
{
    switch (value) {
      case TOAST: return "TOAST";
      case GREEN: return "GREEN";
      case FAST:  return "FAST";
      case TIDY:  return "TIDY";
      case PEARL: return "PEARL";
      case SMITH: return "SMITH";
      // ...
      default: return "(* UNKNOWN *)";
    }
}
//..
// The data set (randomly generated for this example) is provided in a
// statically initialized array:
//..
static const struct CustomerProfile {
    CustomerCode d_customer;
    LocationCode d_location;
    ProjectCode  d_project;
} customerProfiles[] = {
    { IMPULSE   , CANADA  , SMITH },
    { NON_PROFIT, USA_EAST, GREEN },
    { INSTITUTE , USA_EAST, TOAST },
    { NON_PROFIT, CANADA  , PEARL },
    { NEED_BASED, CANADA  , FAST  },
    { BUSINESS  , ENGLAND , PEARL },
    { REPEAT    , SCOTLAND, TIDY  },
    { INSTITUTE , MEXICO  , PEARL },
    { DISCOUNT  , USA_EAST, GREEN },
    { BUSINESS  , USA_EAST, GREEN },
    { IMPULSE   , MEXICO  , TOAST },
    { DISCOUNT  , GERMANY , FAST  },
    { INSTITUTE , FRANCE  , FAST  },
    { NON_PROFIT, ENGLAND , PEARL },
    { BUSINESS  , ENGLAND , TIDY  },
    { BUSINESS  , CANADA  , GREEN },
    { INSTITUTE , FRANCE  , FAST  },
    { IMPULSE   , RUSSIA  , TOAST },
    { REPEAT    , USA_WEST, TOAST },
    { IMPULSE   , CANADA  , TIDY  },
    { NON_PROFIT, GERMANY , GREEN },
    { INSTITUTE , USA_EAST, TOAST },
    { INSTITUTE , FRANCE  , FAST  },
    { IMPULSE   , SCOTLAND, SMITH },
    { INSTITUTE , USA_EAST, PEARL },
    { INSTITUTE , USA_EAST, TOAST },
    { NON_PROFIT, ENGLAND , PEARL },
    { IMPULSE   , GERMANY , FAST  },
    { REPEAT    , GERMANY , FAST  },
    { REPEAT    , MEXICO  , PEARL },
    { IMPULSE   , GERMANY , TIDY  },
    { IMPULSE   , MEXICO  , TOAST },
    { NON_PROFIT, SCOTLAND, SMITH },
    { NEED_BASED, MEXICO  , TOAST },
    { NON_PROFIT, FRANCE  , SMITH },
    { INSTITUTE , MEXICO  , TIDY  },
    { NON_PROFIT, FRANCE  , TIDY  },
    { IMPULSE   , FRANCE  , FAST  },
    { DISCOUNT  , RUSSIA  , TIDY  },
    { IMPULSE   , USA_EAST, TIDY  },
    { IMPULSE   , USA_WEST, FAST  },
    { NON_PROFIT, FRANCE  , TIDY  },
    { BUSINESS  , ENGLAND , GREEN },
    { REPEAT    , FRANCE  , TOAST },
    { REPEAT    , RUSSIA  , SMITH },
    { REPEAT    , RUSSIA  , GREEN },
    { IMPULSE   , CANADA  , FAST  },
    { NON_PROFIT, USA_EAST, FAST  },
    { NEED_BASED, USA_WEST, TOAST },
    { NON_PROFIT, GERMANY , TIDY  },
    { NON_PROFIT, ENGLAND , GREEN },
    { REPEAT    , GERMANY , PEARL },
    { NEED_BASED, USA_EAST, PEARL },
    { NON_PROFIT, RUSSIA  , PEARL },
    { NEED_BASED, ENGLAND , SMITH },
    { INSTITUTE , CANADA  , SMITH },
    { NEED_BASED, ENGLAND , TOAST },
    { NON_PROFIT, MEXICO  , TIDY  },
    { BUSINESS  , GERMANY , FAST  },
    { NEED_BASED, SCOTLAND, PEARL },
    { NON_PROFIT, USA_WEST, TIDY  },
    { NON_PROFIT, USA_WEST, TOAST },
    { IMPULSE   , FRANCE  , PEARL },
    { IMPULSE   , ENGLAND , FAST  },
    { IMPULSE   , USA_WEST, GREEN },
    { DISCOUNT  , MEXICO  , SMITH },
    { INSTITUTE , GERMANY , TOAST },
    { NEED_BASED, CANADA  , PEARL },
    { NON_PROFIT, USA_WEST, FAST  },
    { DISCOUNT  , RUSSIA  , SMITH },
    { INSTITUTE , USA_WEST, GREEN },
    { INSTITUTE , RUSSIA  , TOAST },
    { INSTITUTE , FRANCE  , SMITH },
    { INSTITUTE , SCOTLAND, SMITH },
    { NON_PROFIT, ENGLAND , PEARL },
    { NON_PROFIT, CANADA  , SMITH },
    { NON_PROFIT, USA_EAST, TOAST },
    { REPEAT    , FRANCE  , TOAST },
    { NEED_BASED, FRANCE  , FAST  },
    { DISCOUNT  , MEXICO  , TOAST },
    { DISCOUNT  , FRANCE  , GREEN },
    { IMPULSE   , USA_EAST, FAST  },
    { REPEAT    , USA_EAST, GREEN },
    { NON_PROFIT, GERMANY , GREEN },
    { INSTITUTE , CANADA  , SMITH },
    { NEED_BASED, SCOTLAND, TOAST },
    { NEED_BASED, GERMANY , FAST  },
    { NON_PROFIT, RUSSIA  , TOAST },
    { BUSINESS  , ENGLAND , PEARL },
    { NEED_BASED, USA_EAST, TOAST },
    { INSTITUTE , USA_EAST, SMITH },
    { DISCOUNT  , USA_EAST, PEARL },
    { REPEAT    , SCOTLAND, FAST  },
    { IMPULSE   , GERMANY , TIDY  },
    { DISCOUNT  , CANADA  , TIDY  },
    { IMPULSE   , USA_EAST, TIDY  },
    { IMPULSE   , GERMANY , TIDY  },
    { NON_PROFIT, ENGLAND , FAST  },
    { NON_PROFIT, USA_WEST, TIDY  },
    { REPEAT    , MEXICO  , TOAST },
};
const size_t numCustomerProfiles = sizeof  customerProfiles
                                 / sizeof *customerProfiles;
//..
// Suppose, as the first step in analysis, we wish to determine the number of
// unique combinations of customer attributes that exist in our data set.  We
// can do that by inserting each data item into an (unordered) set: the first
// insert of a combination will succeed, the others will fail, but at the end
// of the process, the set will contain one entry for every unique combination
// in our data.
//
// First, as there are no standard methods for hashing or comparing our user
// defined types, we define 'CustomerProfileHash' and 'CustomerProfileEqual'
// classes, each a stateless functor.  Note that there is no meaningful
// ordering of the attribute values, they are merely arbitrary code numbers;
// nothing is lost by using an unordered set instead of an ordered set:
//..
class CustomerProfileHash
{
  public:
    // CREATORS
    //! CustomerProfileHash() = default;
        // Create a 'CustomerProfileHash' object.

    //! CustomerProfileHash(const CustomerProfileHash& original) = default;
        // Create a 'CustomerProfileHash' object.  Note that as
        // 'CustomerProfileHash' is an empty (stateless) type, this
        // operation will have no observable effect.

    //! ~CustomerProfileHash() = default;
        // Destroy this object.

    // ACCESSORS
    std::size_t operator()(CustomerProfile x) const;
        // Return a hash value computed using the specified 'x'.
};
//..
// The hash function combines the several enumerated values from the class
// (each a small 'int' value) into a single, unique 'int' value, and then
// applying the default hash function for 'int'.  See {Practical Requirements
// on 'HASH'}.
//..
// ACCESSORS
std::size_t CustomerProfileHash::operator()(CustomerProfile x) const
{
    return bsl::hash<int>()(x.d_location * 100 * 100
                          + x.d_customer * 100
                          + x.d_project);
}

class CustomerProfileEqual
{
  public:
    // CREATORS
    //! CustomerProfileEqual() = default;
        // Create a 'CustomerProfileEqual' object.

    //! CustomerProfileEqual(const CustomerProfileEqual& original)
    //!                                                          = default;
        // Create a 'CustomerProfileEqual' object.  Note that as
        // 'CustomerProfileEqual' is an empty (stateless) type, this
        // operation will have no observable effect.

    //! ~CustomerProfileEqual() = default;
        // Destroy this object.

    // ACCESSORS
    bool operator()(const CustomerProfile& lhs,
                    const CustomerProfile& rhs) const;
        // Return 'true' if the specified 'lhs' have the same value as the
        // specified 'rhs', and 'false' otherwise.
};

// ACCESSORS
bool CustomerProfileEqual::operator()(const CustomerProfile& lhs,
                                      const CustomerProfile& rhs) const
{
    return lhs.d_location == rhs.d_location
        && lhs.d_customer == rhs.d_customer
        && lhs.d_project  == rhs.d_project;
}
//..
// Notice that many of the required methods of the hash and comparitor types
// are compiler generated.  (The declaration of those methods are commented out
// and suffixed by an '= default' comment.)
//
// Next, we define the type of the unordered set and a convenience aliases:
//..
typedef bsl::unordered_set<CustomerProfile,
                           CustomerProfileHash,
                           CustomerProfileEqual> ProfileCategories;
typedef ProfileCategories::const_iterator        ProfileCategoriesConstItr;
//..

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
      case 28: {
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
//..
// Then, we create an unordered set and insert each item of 'data'.
//..
    ProfileCategories profileCategories;

    for (size_t idx = 0; idx < numCustomerProfiles; ++idx) {
       profileCategories.insert(customerProfiles[idx]);
    }

    ASSERT(numCustomerProfiles >= profileCategories.size());
//..
// Notice that we ignore the status returned by the 'insert' method.  We fully
// expect some operations to fail.
//
// Now, the size of 'profileCategories' matches the number of unique customer
// profiles in this data set.
//..
if (verbose) {
    printf(ZU " " ZU "\n", numCustomerProfiles, profileCategories.size());
}
//..
// Standard output shows:
//..
//  100 84
//..
// Finally, we can examine the unique set by iterating through the unordered
// set and printing each element.  Note the use of the several 'toAscii'
// functions defined earlier to make the output comprehensible:
//..
    for (ProfileCategoriesConstItr itr  = profileCategories.begin(),
                                   end  = profileCategories.end();
                                   end != itr; ++itr) {
if (verbose) {
        printf("%-10s %-8s %-5s\n",
               toAscii(itr->d_customer),
               toAscii(itr->d_location),
               toAscii(itr->d_project));
}
    }
//..
// We find on standard output:
//..
//  NON_PROFIT ENGLAND  FAST
//  DISCOUNT   CANADA   TIDY
//  NEED_BASED USA_EAST TOAST
//  NON_PROFIT RUSSIA   TOAST
//  NEED_BASED SCOTLAND TOAST
//  REPEAT     USA_EAST GREEN
//  DISCOUNT   FRANCE   GREEN
//  INSTITUTE  FRANCE   SMITH
//  NON_PROFIT CANADA   SMITH
//  INSTITUTE  RUSSIA   TOAST
//  INSTITUTE  USA_WEST GREEN
//  DISCOUNT   RUSSIA   SMITH
//  NON_PROFIT USA_WEST FAST
//  NEED_BASED MEXICO   TOAST
//  IMPULSE    GERMANY  TIDY
//  INSTITUTE  USA_EAST SMITH
//  REPEAT     MEXICO   PEARL
//  INSTITUTE  FRANCE   FAST
//  IMPULSE    FRANCE   FAST
//  REPEAT     RUSSIA   SMITH
//  IMPULSE    SCOTLAND SMITH
//  NON_PROFIT GERMANY  GREEN
//  NON_PROFIT FRANCE   SMITH
//  IMPULSE    GERMANY  FAST
//  INSTITUTE  USA_EAST PEARL
//  IMPULSE    RUSSIA   TOAST
//  NEED_BASED CANADA   FAST
//  IMPULSE    CANADA   SMITH
//  INSTITUTE  SCOTLAND SMITH
//  IMPULSE    USA_EAST TIDY
//  REPEAT     GERMANY  FAST
//  DISCOUNT   RUSSIA   TIDY
//  REPEAT     GERMANY  PEARL
//  BUSINESS   CANADA   GREEN
//  REPEAT     SCOTLAND FAST
//  NEED_BASED USA_EAST PEARL
//  INSTITUTE  GERMANY  TOAST
//  BUSINESS   ENGLAND  TIDY
//  DISCOUNT   USA_EAST PEARL
//  NON_PROFIT ENGLAND  PEARL
//  INSTITUTE  MEXICO   PEARL
//  IMPULSE    MEXICO   TOAST
//  NEED_BASED GERMANY  FAST
//  INSTITUTE  MEXICO   TIDY
//  INSTITUTE  USA_EAST TOAST
//  BUSINESS   ENGLAND  PEARL
//  NON_PROFIT SCOTLAND SMITH
//  NON_PROFIT USA_WEST TIDY
//  BUSINESS   GERMANY  FAST
//  REPEAT     USA_WEST TOAST
//  REPEAT     SCOTLAND TIDY
//  NON_PROFIT CANADA   PEARL
//  NON_PROFIT USA_WEST TOAST
//  DISCOUNT   MEXICO   TOAST
//  NEED_BASED FRANCE   FAST
//  DISCOUNT   GERMANY  FAST
//  NON_PROFIT FRANCE   TIDY
//  IMPULSE    USA_WEST FAST
//  REPEAT     FRANCE   TOAST
//  REPEAT     RUSSIA   GREEN
//  BUSINESS   USA_EAST GREEN
//  NON_PROFIT RUSSIA   PEARL
//  IMPULSE    CANADA   TIDY
//  IMPULSE    CANADA   FAST
//  IMPULSE    USA_EAST FAST
//  NON_PROFIT GERMANY  TIDY
//  NON_PROFIT ENGLAND  GREEN
//  NON_PROFIT USA_EAST TOAST
//  NON_PROFIT USA_EAST FAST
//  IMPULSE    ENGLAND  FAST
//  NEED_BASED ENGLAND  SMITH
//  NEED_BASED CANADA   PEARL
//  INSTITUTE  CANADA   SMITH
//  NON_PROFIT MEXICO   TIDY
//  NEED_BASED ENGLAND  TOAST
//  BUSINESS   ENGLAND  GREEN
//  NEED_BASED SCOTLAND PEARL
//  NON_PROFIT USA_EAST GREEN
//  IMPULSE    FRANCE   PEARL
//  REPEAT     MEXICO   TOAST
//  NEED_BASED USA_WEST TOAST
//  IMPULSE    USA_WEST GREEN
//  DISCOUNT   USA_EAST GREEN
//  DISCOUNT   MEXICO   SMITH
//..
//
///Example 2: Examining and Setting Unordered Set Configuration
///------------------------------------------------------------
// The unordered set interfaces provide some insight into and control of
// its inner workings.  The syntax and semantics of these interfaces for
// 'bslstl_unoroderedset' are identical to those of 'bslstl_unorderedmap'.
// See the material in {'bslstl_unorderedmap'|Example 2}.

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING SPREAD
        // --------------------------------------------------------------------

        if (verbose) printf("Testing SPREAD\n"
                            "==============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase99,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // RESERVE, REHASH, MAX_LOAD_FACTOR
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RESERVE, REHASH, AND MAX_LOAD_FACTOR\n"
                            "============================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // RESERVE, REHASH, MAX_LOAD_FACTOR
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RESERVE, REHASH, AND MAX_LOAD_FACTOR\n"
                            "============================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase25,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING STANDARD INTERFACE COVERAGE
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING STANDARD INTERFACE COVERAGE\n"
                            "===================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING TYPE TRAITS\n"
                            "===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      signed char,
                      int,
                      size_t,
                      void *);
#if 0
        // TBD: all of these types not covered above freak out the bslmf
        // macros, something to do with
        // 'bslstl::HashTable_HashWrapper<FUNCTOR>::d_functor'.

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
#endif
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING STL ALLOCATOR\n"
                            "=====================\n");

        RUN_EACH_TYPE(StdAllocTestDriver,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING COMPARATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'HASH' and 'EQUAL'\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' and 'empty'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'max_size' and 'empty'\n"
                            "==============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 19: {
#if 0
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        //
        // < <= > >= -- N/A for this container
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver, testCase19, int, char);
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'erase'\n"
                            "===============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE 'insert'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'insert'\n"
                            "================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // N/A
        // --------------------------------------------------------------------

        // N/A
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'insert'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'insert'\n"
                            "================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing ITERATORS\n"
                            "=================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'find'\n"
                            "==============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Value Constructor\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'g'\n"
                            "===========\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Streaming Functionality\n"
                            "===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Assignment Operator\n"
                            "===========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // RUN_EACH_TYPE(TestDriver, testCase9_1, int);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("MANIPULATOR AND FREE FUNCTION 'swap'\n"
                            "====================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // RUN_EACH_TYPE(TestDriver, testCase8_1, int);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy Constructors\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Equality Operators\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Output (<<) Operator\n"
                            "============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Basic Accessors\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'gg'\n"
                            "============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Primary Manipulators\n"
                            "============================\n");

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

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

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
        for (int i = 0; i != MAX_SAMPLE; ++i) {
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
