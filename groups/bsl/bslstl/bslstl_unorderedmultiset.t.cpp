// bslstl_unorderedmultiset.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedmultiset.h>

#include <bslstl_iterator.h>  // for testing only

#include <bslalg_rangecompare.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_haspointersemantics.h>
#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_types.h>

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

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// Initial breathing test iterates all operations with a single template
// instantiation and test obvious boundary conditions and iterator stability
// guarantees.
//
// MULTISET TEST SHOULD TEST DUPLICATE KEYS (by equivalence functor) WITH
// DIFFERENT VALUES (by operator==).
//
// MEMORY CONSUMPTION TESTS ARE GENERALLY DISABLED, OTHER THAN LEAKS AT THE END
// OF TEST SEQUENCES
//
// NEED A PROPER TEST TO EVALUATE unordered_set INVARIANTS:
//    equivalent keys form contiguous ranges
//    once inserted, relative order is preserved through all operations
//    contained values correspond to inserted values, counting duplicates
// ----------------------------------------------------------------------------
// [unord.set] construct/copy/destroy:
// [  ] unordered_multiset(size_type =, hasher =, key_equal =, allocator =);
// [ 2] unordered_multiset(const A& allocator);
// [  ] unordered_multiset(const unordered_multiset& original);
// [  ] unordered_multiset(const u_multiset& original, const A& allocator);
// [  ] unordered_multiset(ITER, ITER, size_type =, hasher =, key_equal =,
//                                                                 allocator =)
// [ 2] ~unordered_multiset();
// [  ] unordered_multiset& operator=(const set& rhs);
// [ 4] allocator_type get_allocator() const;
//
// iterators:
// [ 4] iterator begin();
// [ 4] const_iterator begin() const;
// [ 4] iterator end();
// [ 4] const_iterator end() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
// [ 4] local_iterator begin(size_type n);
// [ 4] const_local_iterator begin(size_type n) const;
// [ 4] local_iterator end(size_type n);
// [ 4] const_local_iterator end(size_type n) const;
// [ 4] const_local_iterator cbegin(size_type n) const;
// [ 4] const_local_iterator cend(size_type n) const;
//
// capacity:
// [ 4] bool empty() const;
// [ 4] size_type size() const;
// [ 4] size_type max_size() const;
//
// modifiers:
// [ 2] iterator insert(const value_type& value);
// [ *] iterator insert(value_type&& value);    // TBA: C++11
// [ 2] iterator insert(const_iterator position, const value_type& value);
// [ 2] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
//
// [ 2] iterator erase(const_iterator position);
// [ 2] size_type erase(const key_type& key);
// [ 2] iterator erase(const_iterator first, const_iterator last);
// [ 8] void swap(set& other);
// [ 2] void clear();
//
// observers:
// [  ] hasher hash_function() const;
// [  ] key_equal key_eq() const;
//
// set operations:
// [ 4] iterator find(const key_type& key);
// [ 4] const_iterator find(const key_type& key) const;
// [ 4] size_type count(const key_type& key) const;
// [ 4] bsl::pair<iterator, iterator> equal_range(const key_type& key);
// [ 4] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// bucket interface:
// [ 4] size_type bucket_count() const;
// [ 4] size_type max_bucket_count() const;
// [ 4] size_type bucket_size(size_type n) const;
// [ 4] size_type bucket(const key_type& k) const;
//
// hash policy:
// [ 4] float load_factor() const;
// [ 4] float max_load_factor() const;
// [  ] void max_load_factor(float z);
// [  ] void rehash(size_type n);
// [  ] void reserve(size_type n);
//
// specialized algorithms:
// [ 5] bool operator==(u_multiset<K, H, E, A>& a, u_multiset<K, H, E, A>& b);
// [ 5] bool operator!=(u_multiset<K, H, E, A>& a, u_multiset<K, H, E, A>& b);
// [ 8] void swap(u_multiset<K, H, E, A>& a, u_multiset<K, H, E, A>& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] default construction (only)
// [ 5] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(unordered_multiset<T,H,E,A> *o, const char *s, int verbose);
// [ 3] unordered_multiset<T,H,E,A>& gg(u_multiset<T,H,E,A> *o, const char *s);
// [  ] unordered_multiset<T,H,E,A> g(const char *spec);
//
// [  ] CONCERN: The object is compatible with STL allocators.
// [  ] CONCERN: The object has the necessary type traits
// [  ] CONCERN: The type provides the full interface defined by the standard.

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
//                       GLOBAL TYPEDEFS AND VARIABLES
// ----------------------------------------------------------------------------

typedef bsls::Types::UintPtr UintPtr;
typedef bsls::Types::Int64   Int64;

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line spec
    //---- --------
    { L_,  "" },
    { L_,  "A" },
    { L_,  "AA" },
    { L_,  "B" },
    { L_,  "AB" },
    { L_,  "AC" },
    { L_,  "CD" },
    { L_,  "ABC" },
    { L_,  "BAD" },
    { L_,  "ABCA" },
    { L_,  "ABCB" },
    { L_,  "ABCC" },
    { L_,  "LLLLQQQQ" },
    { L_,  "AABBCC" },
    { L_,  "ABCD" },
    { L_,  "BEADDDDD" },
    { L_,  "BCDE" },
    { L_,  "ABCDE" },
    { L_,  "FEDCB" },
    { L_,  "FEDCBFEDCB" },
    { L_,  "FEDCBA" },
    { L_,  "ABCDEFG" },
    { L_,  "ABCDEFGHAB" },
    { L_,  "ABCDEFGHI" },
    { L_,  "ABCDEFGHIJKLMNOP" },
    { L_,  "ABCDEFGHIJKLMNOPP" },
    { L_,  "DHBIMACOPELGFKNJQ" },
    { L_,  "KEEPCOOLWITHCOOLIDGE" },
    { L_,  "ILIKEIKE" },
    { L_,  "NOONECANFIGHTLIKEKENNEDYCAN" },
    { L_,  "ALLTHEWAYWITHLBJ" }
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
//    return v.first;  // for 'map' containers
}

size_t numCharInstances(const char *SPEC, const char c)
{
    size_t ret = 0;
    for (const char *pc = SPEC; *pc; ++pc) {
        ret += (c == *pc);
    }
    return ret;
}

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
    TYPE tolerance = my_max(my_abs(x), my_abs(y)) * 0.0001f;
    return my_abs(x - y) <= tolerance;
}

template <class CONTAINER>
ptrdiff_t verifySpec(const CONTAINER& object, const char *spec)
{
    typedef typename CONTAINER::key_type       Key;
    typedef typename CONTAINER::const_iterator CIter;
    typedef bsltf::TestValuesArray<Key>        TestValues;

    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    const TestValues VALUES;
    const char *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // For the purposes of this test driver, test values must not be outside
    // the upper case alphabet.

    for (const char *pc = spec; *pc; ++pc) {
        if (*pc < 'A' || *pc > 'Z') {
            return pc - spec;                                         // RETURN
        }
    }

    for (const char *pc = ALPHABET; *pc; ++pc) {
        const char C = *pc;

        size_t count = 0;
        for (const char *pcB = spec; *pcB; ++pcB) {
            count += C == *pcB;
        }

        // We can't rely on the 'count' member function yet, so we have to do
        // it by hand.

        const Key K = VALUES[C - 'A'];
        for (CIter it = object.begin(); object.end() != it; ++it) {
            if (K == *it) {
                --count;
            }
        }

        if (0 != count) {
            return pc - spec;                                         // RETURN
        }
    }

    return -1;    // it's a match
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef          CONTAINER                 TestType;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator const_iterator;

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

    const bsl::pair<const_iterator, const_iterator> emptyRange(x.end(),
                                                               x.end());
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
    typedef typename CONTAINER::iterator   iterator;

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

    const bsl::pair<iterator, iterator> emptyRange(x.end(), x.end());
    ASSERT(x.equal_range(42) == emptyRange);

    ASSERT(0 == x.count(37));
    ASSERT(x.end() == x.find(26));

    iterator it = x.erase(x.begin(), x.end());
    ASSERT(x.end() == it);

    ASSERT(0 == x.erase(93));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testContainerHasData(const CONTAINER&                      X,
                          typename CONTAINER::size_type         nCopies,
                          const typename CONTAINER::value_type *data,
                          typename CONTAINER::size_type         size)
{
    typedef          CONTAINER                 TestType;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    ASSERT(nCopies);
    ASSERT(X.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        const typename CONTAINER::value_type& testValue = data[i];
        TestIterator it = X.find(keyForValue<CONTAINER>(testValue));
        ASSERT(X.end() !=  it);
        ASSERT(data[i] == *it);
        const SizeType countValues =
                                    X.count(keyForValue<CONTAINER>(testValue));
        LOOP3_ASSERT(i, countValues, nCopies,
                     countValues == nCopies);

        bsl::pair<TestIterator, TestIterator> range =
                              X.equal_range(keyForValue<CONTAINER>(testValue));

        const SizeType rangeDist = bsl::distance(range.first, range.second);
        LOOP2_ASSERT(countValues,   rangeDist,
                     countValues == rangeDist);

        ASSERT(range.first == it);
        for(SizeType iterations = nCopies; --iterations; ++it) {
            ASSERT(testValue == *it);
        }

        // Need one extra increment to reach past-the-range iterator.

        if (++it != X.end()) {
            ASSERT(testValue != *it);
        }
        ASSERT(range.second == it);
    }
}

template <class CONTAINER>
void fillContainerWithData(CONTAINER& x,
                           const typename CONTAINER::value_type *data,
                           int       size)
{
    typedef typename CONTAINER::size_type  SizeType;
    typedef typename CONTAINER::iterator   iterator;

    ASSERT(0 < size);
    const SizeType nCopies = x.count(data[0]) + 1;

    SizeType initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (int i = 0; i != size; ++i) {
        iterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(data[i] == *it);
        LOOP3_ASSERT(i, nCopies, x.count(keyForValue<CONTAINER>(data[i])),
                     x.count(keyForValue<CONTAINER>(data[i])) == nCopies);
    }
}

template <class CONTAINER>
void validateIteration(CONTAINER &c)
{
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator const_iterator;

    const SizeType size = c.size();

    SizeType counter = 0;
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
    typedef                      CONTAINER                       TestType;
    typedef typename             CONTAINER::size_type            SizeType;
    typedef typename             CONTAINER::iterator             iterator;
    typedef typename       CONTAINER::const_iterator       const_iterator;
    typedef typename       CONTAINER::local_iterator       local_iterator;
    typedef typename CONTAINER::const_local_iterator const_local_iterator;


    const CONTAINER &x = mX;

    SizeType bucketCount = x.bucket_count();
    SizeType collisions = 0;
    SizeType itemCount  = 0;

    for (unsigned i = 0; i != bucketCount; ++i ) {
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
        LOOP2_ASSERT(count, bucketItems, count == bucketItems);

        bucketItems = 0;
        for (local_iterator iter = mX.begin(i); iter != mX.end(i); ++iter) {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        LOOP2_ASSERT(count, bucketItems, count == bucketItems);

        bucketItems = 0;
        for (const_local_iterator iter = mX.cbegin(i);
             iter != mX.cend(i);
             ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        LOOP2_ASSERT(count, bucketItems, count == bucketItems);

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
void debugprint(const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC> TObj;
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

bool g_enableEqualityFunctorFlag = true;

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
        g_enableEqualityFunctorFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableEqualityFunctorFlag = true;
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
        if (!g_enableEqualityFunctorFlag) {
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
    size_t operator() (const TYPE& obj) const
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
    {
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
    // 'unordered_multiset' container.  The parameterized 'KEY', 'HASH',
    // 'EQUAL' and 'ALLOC' specify the value type, the hash function type, the
    // key-comparator type and allocator type respectively.  Each "testCase*"
    // method tests a specific aspect of
    // 'unordered_multiset<KEY, HASH, EQUAL, ALLOC>'.  Every test cases should
    // be invoked with various parameterized types to fully test the container.

  private:
    // TYPES
    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC> Obj;
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

  public:
    // TEST CASES

    static void testCase16();
        // Test type traits.

    static void testCase15();
        // Test 'hash_function' and 'key_eq'.

    static void testCase14();
        // Test STL allocator.

    static void testCase13();
        // Test max_load_factor(float)

    static void testCase12();
        // Test range constructors.

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

#if 0
    static void testCase5();
        // Reserved for (<<) operator. N/A
#endif

    static void testCase4();
        // Test all accessors

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators, call all manipulators

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
bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC>&
TestDriver<KEY, HASH, EQUAL, ALLOC>::gg(Obj        *object,
                                        const char *spec)
{
    int result = ggg(object, spec);
    ASSERTV(result, -1 == result);
    return *object;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC>
TestDriver<KEY,HASH, EQUAL, ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase16()
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
               bslalg::HasStlIterators<bsl::unordered_multiset<KEY> >::value));
    BSLMF_ASSERT((1 ==
             bslma::UsesBslmaAllocator<bsl::unordered_multiset<KEY> >::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL,StlAlloc> ObjStlAlloc;
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<ObjStlAlloc>::value));

    // Verify unordered_multiset does not define other common traits.

    BSLMF_ASSERT((0 ==
            bsl::is_trivially_copyable<bsl::unordered_multiset<KEY> >::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<
                                       bsl::unordered_multiset<KEY> >::value));

    BSLMF_ASSERT((1 ==
              bslmf::IsBitwiseMoveable<bsl::unordered_multiset<KEY> >::value));

    BSLMF_ASSERT((0 ==
            bslmf::HasPointerSemantics<bsl::unordered_multiset<KEY> >::value));

    BSLMF_ASSERT((0 ==
               bsl::is_trivially_default_constructible<
                                       bsl::unordered_multiset<KEY> >::value));

    // --------------------------------
    // Repeat the above tests for 'Obj'

    // Verify set defines the expected traits.

    BSLMF_ASSERT((1 == bslalg::HasStlIterators<Obj>::value));
    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<Obj>::value));

    // Verify unordered_multiset does not define other common traits.

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<Obj>::value));

    BSLMF_ASSERT((1 == bslmf::IsBitwiseMoveable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<Obj>::value));
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING HASH_FUNCTION AND KEY_EQ
    //
    // Concern:
    //   That the value of hash function and equality comparator objects passed
    //   at construction are properly preserved.
    //
    // Plan:
    //   Pass function objects at construction and then access copies of them
    //   through the 'hash_function' and 'key_eq' accessors, and verify that
    //   the values are as expected.
    // ------------------------------------------------------------------------

    {
        typedef HASH (Obj::*MP)() const;
        MP mp = &Obj::hash_function;
        (void) mp;
    }

    {
        typedef EQUAL (Obj::*MP)() const;
        MP mp = &Obj::key_eq;
        (void) mp;
    }

    HASH  hsh5(5), hsh12(12);
    EQUAL  eq7(7), eq17( 17);

    TestValues VALUES;

    {
        Obj mX(10, hsh5, eq7);      const Obj& X = mX;

        ASSERTV( 5 == X.hash_function().id());
        ASSERTV( 7 == X.key_eq().id());
    }

    {
        Obj mX(VALUES.begin(), VALUES.end(), 10, hsh12, eq17);
        const Obj& X = mX;

        ASSERTV(12 == X.hash_function().id());
        ASSERTV(17 == X.key_eq().id());
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase14()
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

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, StlAlloc> ObjStlAlloc;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const ptrdiff_t   LENGTH = strlen(SPEC);
        const TestValues  EXP(SPEC, &scratch);
        ASSERT(0 <= LENGTH);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            ObjStlAlloc mX(BEGIN, END);  const ObjStlAlloc& X = mX;

            const ptrdiff_t v = verifySpec(X, SPEC);
            ASSERTV(LINE, v, SPEC, X.size(), -1 == v);
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH <= da.numBlocksInUse());

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, -1 == verifySpec(Y, SPEC));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH == da.numBlocksInUse());

            ObjStlAlloc mZ;  const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, -1 == verifySpec(Z, SPEC));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH <= da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH <= da.numBlocksInUse());
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::unordered_multiset<int,
                                        TestHashFunctor<int>,
                                        TestEqualityComparator<int>,
                                        StlAlloc> TestObjIntStlAlloc;

        TestObjIntStlAlloc mX;
        mX.insert(1);
        TestObjIntStlAlloc mY;
        mY = mX;

        ASSERTV(mY.count(1));
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase13()
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

    bslma::TestAllocator da     ("default", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);


    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const size_t LINE   = DATA[ti].d_line;
        const char  *SPEC   = DATA[ti].d_spec;
        const size_t LENGTH = strlen(SPEC);

        TestValues values(SPEC, &scratch);

        if (veryVeryVerbose) Q(Test 'reserve');
        {
            Obj mX(values.begin(), values.end());  const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                             const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            mX.reserve(my_max<size_t>(X.size(), 1) * 3);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        if (veryVeryVerbose) Q(Test 'reserve' before insert);
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            for (size_t len2 = 0; len2 <= LENGTH; ++len2) {
                Obj mX(&sa);    const Obj& X = mX;

                TestValues valuesB(SPEC, &sa);

                mX.insert(valuesB.begin(), valuesB.index(len2));

                int numPasses = 0;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    mX.reserve(LENGTH);
                } EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || len2 || 0 == LENGTH || numPasses > 1);

                const size_t BC = X.bucket_count();
                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(0.9999
                        * static_cast<double>(LENGTH)
                          / static_cast<double>(X.bucket_count()) <
                                                          X.max_load_factor());

                mX.insert(valuesB.index(len2), valuesB.end());

                ASSERTV(LINE, SPEC, LENGTH == X.size());
                ASSERTV(-1 == verifySpec(X, SPEC));

                ASSERTV(BC == X.bucket_count());
            }
        }

        {
            Obj mX(values.begin(), values.end());  const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                             const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            int numPasses = 0;
            EXCEPTION_TEST_BEGIN(mX) {
                ++numPasses;

                mX.rehash(X.bucket_count() + 1);
            } EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        if (values.size() > 0) {
            Obj mX(values.begin(), values.end());  const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                             const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();
            const float  LOAD  = X.load_factor();
            const double LOAD_EXPECTED =
                                       static_cast<double>(X.size())
                                       / static_cast<double>(X.bucket_count());
            ASSERTV(LOAD,
                    LOAD_EXPECTED,
                    nearlyEqual<double>(LOAD, LOAD_EXPECTED));
            ASSERTV(1.0f == X.max_load_factor());

            const float NEW_MAX = LOAD / 4;

            int numPasses = 0;
            EXCEPTION_TEST_BEGIN(mX) {
                ++numPasses;

                mX.max_load_factor(NEW_MAX);
            } EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(nearlyEqual<float>(NEW_MAX, X.max_load_factor()));
            ASSERTV(NEW_MAX >= X.load_factor());

            const float LOAD2 = X.load_factor();
            const float LOAD_EXPECTED2 =
                                        static_cast<float>(X.size())
                                        / static_cast<float>(X.bucket_count());
            ASSERTV(LOAD2,
                    LOAD_EXPECTED2,
                    nearlyEqual<float>(LOAD2, LOAD_EXPECTED2));

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
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

            size_t sz = 0;
            for (char c = 'A'; c <= 'Z'; ++c) {
                KEY k = VALUES[c - 'A'];

                size_t EXP = numCharInstances(SPEC, c);

                ASSERTV(LINE, SPEC, c, EXP, EXP == X.count(k));
                sz += EXP;
            }
            ASSERTV(X.size() == sz);
        }
    }

    HASH  defaultHash(7);
    EQUAL defaultEqual(9);

    ASSERTV(!(HASH()  == defaultHash));
    ASSERTV(!(EQUAL() == defaultEqual));

    if (verbose) Q(Now test range creation);
    {
        bslma::TestAllocator sc(veryVeryVeryVerbose);

        for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
            const int LINE   = DEFAULT_DATA[i].d_line;
            const char *SPEC = DEFAULT_DATA[i].d_spec;

            bslma::DefaultAllocatorGuard dagSc(&sc);
            bsltf::TestValuesArray<KEY> tv(SPEC, &sc);

            if (veryVerbose) P(SPEC);

            int done = 0;
            for (char cfg = 'a'; cfg <= 'i' ; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator fa(veryVeryVeryVerbose);
                bslma::TestAllocator sa(veryVeryVeryVerbose);
                bslma::TestAllocator da(veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator&  oa = strchr("ei", CONFIG) ? sa : da;
                bslma::TestAllocator& noa = &oa == &da           ? sa : da;

                Obj *pmX;
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    tv.resetIterators();

                    switch (CONFIG) {
                      case 'a': {
                        pmX = new (fa) Obj(tv.begin(), tv.end());
                      } break;
                      case 'b': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 100);
                      } break;
                      case 'c': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 100,
                                           defaultHash);
                      } break;
                      case 'd': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           defaultHash,
                                           defaultEqual);
                      } break;
                      case 'e': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           defaultHash,
                                           defaultEqual,
                                           &sa);
                      } break;
                      case 'f': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 0);
                      } break;
                      case 'g': {
                        pmX = new (fa) Obj(tv.begin(), tv.end(), 0,
                                           defaultHash);
                      } break;
                      case 'h': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           defaultHash,
                                           defaultEqual);
                      } break;
                      case 'i': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           defaultHash,
                                           defaultEqual,
                                           &sa);
                        ++done;
                      } break;
                      default: {
                        ASSERTV(0);
                        return;                                       // RETURN
                      } break;
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                Obj& mX = *pmX;    const Obj& X = mX;

                ASSERTV(0 == noa.numBlocksTotal());
                ASSERTV((numPasses > 1) ==
                                        (PLAT_EXC && oa.numBlocksTotal() > 0));
                ASSERTV((X.bucket_count() >= 100 || tv.size() > 0) ==
                                                    (oa.numBlocksTotal() > 0));
                ASSERTV(CONFIG, SPEC,
                    (strchr("cdeghi", CONFIG) ? defaultHash
                                              : HASH())  == X.hash_function());
                ASSERTV(CONFIG, SPEC,
                    (strchr("dehi",   CONFIG) ? defaultEqual
                                              : EQUAL()) == X.key_eq());
                ASSERTV(CONFIG, SPEC, X.bucket_count(),
                           !strchr("bcde", CONFIG) || 100 <= X.bucket_count());

                size_t sz = 0;
                for (char c = 'A'; c <= 'Z'; ++c) {
                    KEY k = VALUES[c - 'A'];

                    size_t EXP = numCharInstances(SPEC, c);

                    ASSERTV(LINE, SPEC, c, EXP, EXP == X.count(k));
                    sz += EXP;
                }
                ASSERTV(X.size() == sz);

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
    //   set g(const char *spec);
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
        const Obj& r3 = g(SPEC);
        const Obj& r4 = g(SPEC);
        ASSERT(&x  == &r1);
        ASSERT(&r4 != &r3);
        ASSERT(&x  != &r3);

        ASSERT(r3 == x);
        ASSERT(r4 == x);
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
    //   set& operator=(const set& rhs);
    // ------------------------------------------------------------------------

    {
        typedef Obj& (Obj::*MP)(const Obj&);
        MP mp = &Obj::operator=;
        (void) mp;
    }

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
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(SPEC1);

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
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX( &oa);  const Obj& X  = gg(&mX,   SPEC2);
                    Obj mXX(&oa);  const Obj& XX = gg(&mXX,  SPEC2);

                    mX.max_load_factor(3.0);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (ti == tj));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    int numPasses = 0;
                    EXCEPTION_TEST_BEGIN(mX) {
                        ++numPasses;
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        ASSERT(XX == X);

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } EXCEPTION_TEST_END
                    ASSERTV((!PLAT_EXC || 0 == LENGTH1) || numPasses > 1);

                    ASSERTV(nearlyEqual<float>(2.0, X.max_load_factor()));

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
    //:
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

    if (veryVerbose) printf("SWAP MEMBER AND FREE FUNCTIONS\n"
                             "==============================\n");

    if (veryVerbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*FuncPtr)(Obj&);
        typedef void (*FreeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        FuncPtr     memberSwap = &Obj::swap;
        FreeFuncPtr freeSwap   = bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (veryVerbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) printf(
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

        if (0 == ti) {
            ASSERTV(LINE1, Obj(), W, Obj() == W);
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

            mX.max_load_factor(2.0f);
            mY.max_load_factor(3.0f);

            ASSERT(nearlyEqual<float>(2.0f, X.max_load_factor()));
            ASSERT(nearlyEqual<float>(3.0f, Y.max_load_factor()));

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERT(nearlyEqual<float>(3.0f, X.max_load_factor()));
                ASSERT(nearlyEqual<float>(2.0f, Y.max_load_factor()));

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

                ASSERT(nearlyEqual<float>(2.0f, X.max_load_factor()));
                ASSERT(nearlyEqual<float>(3.0f, Y.max_load_factor()));

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

                EXCEPTION_TEST_BEGIN(mX) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    mX.swap(mZ);

                    guardX.release();
                    guardZ.release();
                } EXCEPTION_TEST_END


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

                EXCEPTION_TEST_BEGIN(mX) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    swap(mX, mZ);

                    guardX.release();
                    guardZ.release();
                } EXCEPTION_TEST_END

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

    if (veryVerbose) printf(
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

#if 0
        invokeAdlSwap(mX, mY);
#else
        // We know that the types of 'mX' and 'mY' do not overload the unary
        // address-of 'operator&'.

        bslalg::SwapUtil::swap(&mX, &mY);
#endif

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        ASSERT(nearlyEqual<float>(3.0, X.max_load_factor()));
        ASSERT(nearlyEqual<float>(2.0, Y.max_load_factor()));

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

    if (veryVerbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "AAAA",
            "BCC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEFG",
            "HFGEDCBA",
            "CFHEBIDGA",
            "BENCKHGMALJDFOI",
            "IDMLNEFHOPKGBCJA",
            "OIQGDNPMLKBACHFEJ",
            "KEEPCOOLWITHCOOLIDGE"
        };

        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (veryVerbose) {
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

                Iter RESULT = Y1.insert(VALUES['Z' - 'A']);

                ASSERTV(VALUES['Z' - 'A'] == *RESULT);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
                ASSERTV(SPEC, X == W);
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

                Iter RESULT = Y11.insert(VALUES['Z' - 'A']);
                ASSERTV(VALUES['Z' - 'A'] == *RESULT);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, X == W);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

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
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase6()
{
    // ------------------------------------------------------------------------
    // TESTING 'OPERATOR=='
    // ------------------------------------------------------------------------

    {
        typedef bool (*FP)(const Obj&, const Obj&);
        FP fp = &bsl::operator==;
        fp    = &bsl::operator!=;
        (void) fp;
    }

    static const struct {
        int         d_line;                 // source line number
        const char *d_spec;                 // specification string
    } DATA[] = {
        //line  spec
        //----  --------
        { L_,   ""      },
        { L_,   "A"     },
        { L_,   "AA"    },
        { L_,   "B"     },
        { L_,   "AB"    },
        { L_,   "AABB"  },
        { L_,   "CD"    },
        { L_,   "ABCBA" },
        { L_,   "ABCD"  },
        { L_,   "ABCDE" },
        { L_,   "ABEEE" },
        { L_,   "HIJKL" },
        { L_,   "AAAFF" },
        { L_,   "CCCCC" },
        { L_,   "AAAAAB" },
        { L_,   "AAAAABCDEFG" } };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
    bslma::TestAllocator xa("xsupplied", veryVeryVeryVerbose);
    bslma::TestAllocator ya("ysupplied", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *const ISPEC = DATA[ti].d_spec;

        Obj mX(&xa);    const Obj& X = mX;
        ASSERTV(-1 == ggg(&mX, ISPEC));

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const char *const JSPEC = DATA[tj].d_spec;

            Obj mY(&ya);    const Obj& Y = mY;
            ASSERTV(-1 == ggg(&mY, JSPEC));

            // EQUAL::disableFunctor();
                        // TBD -- fails this test EQUAL is used to determine
                        // the equality groups, then bsl::permutation is used
                        // to determine if they're isomorphic, and
                        // bsl::permutation uses 'operator=='.  It will take a
                        // lot of work to verify that this is the case, putting
                        // it off for later.

            const bool EXP = (ti == tj);

            const bsls::Types::Int64 BLOCKS = da.numBlocksTotal();

            ASSERT( EXP == (X == Y));
            ASSERT(!EXP == (X != Y));

            ASSERT( EXP == (Y == X));
            ASSERT(!EXP == (Y != X));

            ASSERTV(da.numBlocksTotal() == BLOCKS);

            if (Y.empty()) {
                continue;
            }
            float load      = Y.load_factor();
            const size_t NB = Y.bucket_count();
            ASSERTV(load > 0);

            load /= 2;

            mY.max_load_factor(load);
            ASSERTV(Y.load_factor() <= load * 1.00001);
            ASSERTV(Y.bucket_count() > NB);

            ASSERTV(-1 == verifySpec(Y, JSPEC));

            ASSERT( EXP == (X == Y));
            ASSERT(!EXP == (X != Y));

            ASSERT( EXP == (Y == X));
            ASSERT(!EXP == (Y != X));

            // EQUAL::enableFunctor();
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - begin, cbegin, begin(i), cbegin(i)
    //     - end, cend, end(i), cend(i)
    //     - find
    //     - count
    //     - equal_range
    //     - bucket
    //     - bucket_size
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

    typedef typename Obj::local_iterator LIter;
    typedef bsl::pair<Iter, Iter>        Range;

    if (veryVerbose) printf("Testing Signatures\n");

    {
        typedef bool (Obj::*MP)() const;
        MP mp = &Obj::empty;
        (void) mp;
    }
    {
        typedef size_t (Obj::*MP)() const;
        MP mp = &Obj::size;
        mp    = &Obj::max_size;
        mp    = &Obj::bucket_count;
        mp    = &Obj::max_bucket_count;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)();
        MP mp = &Obj::begin;
        mp    = &Obj::end;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)() const;
        MP mp = &Obj::begin;
        mp    = &Obj::end;
        mp    = &Obj::cbegin;
        mp    = &Obj::cend;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)(const KEY&);
        MP mp = &Obj::find;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::find;
        (void) mp;
    }
    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::count;
        mp    = &Obj::bucket;
        (void) mp;
    }
    {
        typedef Range (Obj::*MP)(const KEY&);
        MP mp = &Obj::equal_range;
        (void) mp;
    }
    {
        typedef Range (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::equal_range;
        (void) mp;
    }
    {
        typedef size_t (Obj::*MP)(size_t) const;
        MP mp = &Obj::bucket_size;
        (void) mp;
    }
    {
        typedef LIter (Obj::*MP)(size_t);
        MP mp = &Obj::begin;
        mp    = &Obj::end;
        (void) mp;
    }
    {
        typedef LIter (Obj::*MP)(size_t) const;
        MP mp = &Obj::begin;
        mp    = &Obj::end;
        mp    = &Obj::cbegin;
        mp    = &Obj::cend;
        (void) mp;
    }
    {
        typedef float (Obj::*MP)() const;
        MP mp = &Obj::load_factor;
        mp    = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(float);
        MP mp = &Obj::max_load_factor;
        (void) mp;
    }

    static const struct {
        int         d_line;                 // source line number
        const char *d_spec;                 // specification string
    } DATA[] = {
        //line  spec
        //----  --------
        { L_,   ""      },
        { L_,   "A"     },
        { L_,   "AA"    },
        { L_,   "B"     },
        { L_,   "AB"    },
        { L_,   "AABB"  },
        { L_,   "CD"    },
        { L_,   "ABCBA" },
        { L_,   "ABCD"  },
        { L_,   "ABCDE" },
        { L_,   "ABEEE" },
        { L_,   "HIJKL" },
        { L_,   "AAAFF" },
        { L_,   "CCCCC" },
        { L_,   "AAAAAB" },
        { L_,   "AAAAABCDEFG" },
        { L_,   "AAAAABCDEFGHIIII" },
        { L_,   "AAAAABCDEFGHIIIIJJJJ" },
        { L_,   "THERAININSPAINFALLSMAINLYINTHEPLAIN" },
        { L_,   "APENNYSAVEDISAPENNYEARNED" },
        { L_,   "ASTITCHINTIMESAVESNINE" },
        { L_,   "THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG" },
        { L_,   "EVERYGOODBOYDESERVESFUDGE" },
        { L_,   "THREEMAYKEEPASECRETIFTWOOFTHEMAREDEAD" },
        { L_,   "LETNOMANPULLYOULOWENOUGHTOHATEHIM" },
        { L_,   "RESPECTFORTHETRUTHISANACQUIREDTASTE" },
        { L_,   "ITISPROBABLETHATIMPROBABLEEVENTSWILLOCCUR" },
        { L_,   "SILENCEISTHEVIRTUEOFFOOLS" },
        { L_,   "AHOMEWITHOUTEQUITYISJUSTARENTALWITHDEBT" },
        { L_,   "NOTHINGISSOFIRMLYBELIEVEDASWHATWELEASTKNOW" },
        { L_,   "AGEISAVERYHIGHPRICETOPAYFORMATURIY" },
        { L_,   "ALLIASKISACHANCETOPROVETHATMONEYWONTMAKEMEHAPPY" } };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    TestValues VALUES;
    const char *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);

        static const float LOADS[] = { 1.0f, 2.5f, 6.0f, 15.0f };
        enum { NUM_LOADS = sizeof LOADS / sizeof *LOADS };

        for (int fi = 0; fi < NUM_LOADS; ++fi) {
            Obj mX(&sa);    const Obj& X = mX;

            ASSERTV(X.get_allocator().mechanism() == &sa);

            const size_t MAX_SIZE = X.max_size();
            ASSERTV((size_t) -1 >= MAX_SIZE);             // TBD: should be '>'

            const size_t MAX_BUCKET_COUNT = X.max_bucket_count();
            ASSERTV((size_t) -1 >= MAX_BUCKET_COUNT);     // TBD: should be '>'

            // empty()

            ASSERTV(X.empty());

            if (fi) {
                mX.max_load_factor(LOADS[fi]);
            }
            ASSERTV(LINE, X.max_load_factor() == LOADS[fi]);

            ASSERTV(X.empty());

            ASSERTV(-1 == ggg(&mX, SPEC));

            if (veryVerbose) printf(".size(), .empty() (again), max_size()\n");

            ASSERTV(LENGTH == X.size());
            ASSERTV(!LENGTH == X.empty());
            ASSERTV(X.max_size()         == MAX_SIZE);            // unaffected
            ASSERTV(X.max_bucket_count() == MAX_BUCKET_COUNT);    // unaffected

            if (ti) {
                ASSERTV(X.bucket_count() > 1);
            }

            if (veryVerbose) {
                printf("max_load_factor, load_factor, bucket_count\n");
            }

            const double LOAD_EXPECTED =
                                        static_cast<double>(LENGTH)
                                        / static_cast<double>(X.bucket_count());
            ASSERTV(X.load_factor(),
                    LENGTH,
                    X.bucket_count(),
                    LOAD_EXPECTED,
                    nearlyEqual<double>(X.load_factor(), LOAD_EXPECTED));
            ASSERTV(X.load_factor() <= X.max_load_factor());
            if (veryVerbose) {
                printf("mX.begin, mX.end, X.begin, X.end, X.cbegin, X.cend\n");
                printf("X.count, mX.find, X.find, mX.equal_range,"
                       " X.equal_range, X.size (again)\n");
                printf("Like nodes are contiguous\n");
            }
            {
                int numPasses = 0;
                Iter begin;
                Iter end;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    begin = mX.begin();
                    end   = mX.end();
                    ASSERTV(X.begin()  == begin);
                    ASSERTV(X.end()    == end);
                    ASSERTV(X.cbegin() == begin);
                    ASSERTV(X.cend()   == end);
                } EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

                ASSERTV((0 == ti) == (begin == end));

                size_t count = 0, countKey = 0;
                bool firstTime = true;
                KEY prevKey = KEY();
                Iter endRange = end;
                for (Iter it = begin; end != it; ++it) {
                    ++count;
                    KEY k = *it;
                    if (firstTime || k != prevKey) {
                        if (!firstTime) {
                            ASSERTV(endRange == it);
                        }
                        firstTime = false;
                        ASSERTV(0 == countKey);
                        countKey = X.count(k);
                        ASSERTV(mX.find(k) == it);
                        ASSERTV( X.find(k) == it);
                        Range   pr =  mX.equal_range(k);
                        ASSERTV(pr ==  X.equal_range(k));
                        ASSERTV(pr.first == it);
                        endRange = pr.second;
                    }
                    --countKey;
                    prevKey = k;
                }
                ASSERTV(0 == countKey);
                ASSERTV(end == endRange);
                ASSERTV(X.size() == count);
            }

            if (veryVerbose) {
                printf("X.bucket_count() (again)\n");
                printf("X.bucket(KEY), X.begin(i), X.end(i), mX.begin(i)\n");
                printf("mX.end(i), X.cbegin(i), X.cend(i)\n");
                printf("Like nodes are contiguous and all in X.bucket(KEY)\n");
                printf("X.bucket_size(), X.count(KEY) (again)\n");
            }
            for (const char *pc = ALPHABET; *pc; ++pc) {
                const char C = *pc;

                const KEY K = VALUES[C - 'A'];
                const size_t KB = X.bucket(K);
                const size_t NUM_K = X.count(K);
                const bool inSet = NUM_K;

                ASSERTV(NUM_K == numCharInstances(SPEC, C));

                size_t numFoundBuckets = 0;
                const size_t BC = X.bucket_count();
                ASSERTV(0 == ti || BC > 1);
                for (size_t b = 0; b < BC; ++b) {
                    LIter bBegin;
                    LIter bEnd;

                    int numPasses = 0;
                    EXCEPTION_TEST_BEGIN(mX) {
                        ++numPasses;

                        bBegin = X.begin(b);
                        bEnd   = X.end(  b);

                        ASSERTV(mX.begin(b) == bBegin);
                        ASSERTV(X.cbegin(b) == bBegin);
                        ASSERTV(mX.end(  b) == bEnd);
                        ASSERTV(X.cend(  b) == bEnd);
                    } EXCEPTION_TEST_END
                    ASSERTV(1 == numPasses);

                    size_t matchCount = 0, nodeCount = 0;
                    enum Chain {
                        CHAIN_NOT_STARTED,
                        IN_CHAIN,
                        CHAIN_FINISHED };
                    Chain chain = CHAIN_NOT_STARTED;
                    for (LIter it = bBegin; bEnd != it; ++it, ++nodeCount) {
                        const bool match = K == *it;
                        matchCount += match;
                        switch (chain) {
                          case CHAIN_NOT_STARTED: {
                            if (match) {
                                chain = IN_CHAIN;
                            }
                          } break;
                          case IN_CHAIN: {
                            if (!match) {
                                chain = CHAIN_FINISHED;
                            }
                          } break;
                          case CHAIN_FINISHED: {
                            ASSERTV(!match);
                          } break;
                        }
                    }

                    ASSERTV(nodeCount >= matchCount);
                    ASSERTV(X.bucket_size(b) == nodeCount);
                    if (KB == b) {
                        ASSERTV(inSet == (matchCount > 0));
                        ASSERTV(!inSet || bBegin != bEnd);
                        ASSERTV(NUM_K == matchCount);
                    }
                    else {
                        ASSERTV(!matchCount);
                    }

                    numFoundBuckets += 0 != matchCount;
                }
                ASSERTV(inSet == numFoundBuckets);

                const Iter it = mX.find(K);
                ASSERTV(  it ==  X.find(K));
                if (inSet) {
                    ASSERTV(*it == K);
                }
                else {
                    ASSERTV(X.end() == it);
                }
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
    //   unordered_multiset<K,H,E,A>& gg(u_multiset<K,H,E,A> *, const char *);
    //   int ggg(unordered_multiset<K,H,E,A> *, const char *spec, int verbose);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);


    if (verbose) printf(
                      "\nTesting 'ggg' generator on valid & invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1  }, // control

            { L_,   "A",      -1  }, // control
            { L_,   " ",       0  },
            { L_,   ".",       0  },
            { L_,   "E",      -1  }, // control
            { L_,   "a",       0  },
            { L_,   "z",       0  },

            { L_,   "AE",     -1  }, // control
            { L_,   "aE",      0  },
            { L_,   "Ae",      1  },
            { L_,   ".~",      0  },
            { L_,   "~!",      0  },
            { L_,   "  ",      0  },

            { L_,   "ABC",    -1  }, // control
            { L_,   " BC",     0  },
            { L_,   "A C",     1  },
            { L_,   "AB ",     2  },
            { L_,   "?#:",     0  },
            { L_,   "   ",     0  },

            { L_,   "ABCDE",  -1  }, // control
            { L_,   "aBCDE",   0  },
            { L_,   "ABcDE",   2  },
            { L_,   "ABCDe",   4  },
            { L_,   "AbCdE",   1  }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);    const Obj& X = mX;

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                 ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);

            ptrdiff_t retX = verifySpec(X, SPEC);
            ASSERTV(ti, retX, (INDEX == -1) == (-1 == retX));
        }
    }

    if (verbose) printf("\nTesting 'gg' generators on valid specs.\n");
    {
        static const struct {
            int         d_line;                 // source line number
            const char *d_spec;                 // specification string
        } DATA[] = {
            //line  spec
            //----  --------
            { L_,   ""      },
            { L_,   "A"     },
            { L_,   "AA"    },
            { L_,   "B"     },
            { L_,   "AB"    },
            { L_,   "AABB"  },
            { L_,   "CD"    },
            { L_,   "ABCBA" },
            { L_,   "ABCD"  },
            { L_,   "ABCDE" },
            { L_,   "ABEEE" },
            { L_,   "HIJKL" },
            { L_,   "AAAFF" },
            { L_,   "CCCCC" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            ASSERTV(&X == &mX);

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                T_ T_ T_ P(X);
            }

            ASSERTV(LINE, LENGTH == X.size());

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const char *JSPEC = DATA[tj].d_spec;

                ptrdiff_t retX = verifySpec(X, JSPEC);
                ASSERTV(ti, tj, retX, (ti == tj) == (-1 == retX));
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    // The basic concern is the default constructor, the destructor, and, under
    // normal conditions (i.e., no aliasing), the primary manipulators
    //      - insert
    //      - erase
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
    //:10 'insert' adds an additional element to the object and returns the
    //:   iterator to the newly added element.
    //:
    //:11 Duplicated values are inserted contiguously in the range of existing
    //:   equivalent elements, without changing their relative order.
    //:
    //:12 'clear' properly destroys each contained element value.
    //:
    //:13 'clear' does not allocate memory.
    //:
    //:14 Any argument can be 'const'.
    //:
    //:15 Any memory allocation is exception neutral.
    // ------------------------------------------------------------------------

    // Function signatures check

    typedef bsltf::TestValuesArrayIterator<KEY> TVAIter;

    {
        typedef Iter (Obj::*MP)(const KEY&);
        MP mp = &Obj::insert;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)(Iter, const KEY&);
        MP mp = &Obj::insert;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(TVAIter, TVAIter);
        MP mp = &Obj::insert;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)(Iter);
        MP mp = &Obj::erase;
        (void) mp;
    }
    {
        typedef size_t (Obj::*MP)(const KEY&);
        MP mp = &Obj::erase;
        (void) mp;
    }
    {
        typedef Iter (Obj::*MP)(Iter, Iter);
        MP mp = &Obj::erase;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)();
        MP mp = &Obj::clear;
        (void) mp;
    }

    const bool VALUE_TYPE_USES_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOC); }

    TestValues values;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    const HASH  defaultHash(7);
    const EQUAL defaultEqual(9);

    ASSERTV(!(HASH()  == defaultHash));
    ASSERTV(!(EQUAL() == defaultEqual));

    static int typeCounter = 0;
    ++typeCounter;
    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        bool done = false;
        for (char cfg = 'a'; cfg <= 'k'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            bslma::TestAllocator&  oa = strchr("cgk", CONFIG) ? sa : da;
            bslma::TestAllocator& noa = &oa == &da ? sa : da;

            Obj                  *objPtr;

            {
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj((bslma::Allocator *) 0);
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(&sa);
                      } break;
                      case 'd': {
                        objPtr = new (fa) Obj((size_t) 0);
                      } break;
                      case 'e': {
                        objPtr = new (fa) Obj(0, defaultHash);
                      } break;
                      case 'f': {
                        objPtr = new (fa) Obj(0, defaultHash, defaultEqual);
                      } break;
                      case 'g': {
                        objPtr = new (fa) Obj(0, defaultHash, defaultEqual,
                                              &sa);
                      } break;
                      case 'h': {
                        objPtr = new (fa) Obj(100);
                      } break;
                      case 'i': {
                        objPtr = new (fa) Obj(100, defaultHash);
                      } break;
                      case 'j': {
                        objPtr = new (fa) Obj(100, defaultHash, defaultEqual);
                      } break;
                      case 'k': {
                        objPtr = new (fa) Obj(100, defaultHash, defaultEqual,
                                               &sa);
                        done = true;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                        return;                                       // RETURN
                      } break;
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(CONFIG,
                               (PLAT_EXC && CONFIG >= 'h') == (numPasses > 1));
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;

            ASSERTV(0 == noa.numBlocksTotal());
            ASSERTV(CONFIG, (CONFIG >= 'h') == (oa.numBlocksTotal() > 0));

            ASSERTV(CONFIG,
                     (strchr("efgijk", CONFIG) ? defaultHash
                                               : HASH()) == X.hash_function());
            ASSERTV(CONFIG,
                     (strchr("fgjk",   CONFIG) ? defaultEqual
                                               : EQUAL()) == X.key_eq());

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

#if 0
            // Verify no allocation from the object/non-object allocators.
            // NOTE THAT THIS QoI TEST IS STILL AN OPEN DESIGN ISSUE

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());
#endif
            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());
            ASSERTV(LENGTH, CONFIG, 0 < X.bucket_count());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'insert' (bootstrap).\n"); }

            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                Iter resultA;
                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    const KEY& K = values[tj];

                    ASSERTV(tj, 0 == X.count(K));

                    // Test with no hint and misleading hint

                    resultA = 0 == (tj & 1)
                            ? mX.insert(         K)
                            : mX.insert(resultA, K);
                    ASSERTV(LENGTH, tj, CONFIG, K == *resultA);
                    ASSERTV(BSLS_UTIL_ADDRESSOF(K) !=
                                                BSLS_UTIL_ADDRESSOF(*resultA));
                    ASSERTV(tj, 1 == X.count(K));

                    // Test with accurate hint

                    Iter resultB = mX.insert(resultA, K);
                    ASSERTV(LENGTH, tj, CONFIG, K == *resultB);
                    ASSERTV(resultB != resultA);
                    ASSERTV(BSLS_UTIL_ADDRESSOF(K) !=
                                                BSLS_UTIL_ADDRESSOF(*resultB));
                    ASSERTV(BSLS_UTIL_ADDRESSOF(*resultA) !=
                                                BSLS_UTIL_ADDRESSOF(*resultB));
                    ASSERTV(tj, 2 == X.count(K));
                }

                ASSERTV(LENGTH, CONFIG, 2 * (LENGTH - 1) == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const KEY& K = values[LENGTH - 1];

                int exceptionBlockPass = 0;
                if (&oa == &da) {
                    mX.insert(K);
                }
                else {
                    EXCEPTION_TEST_BEGIN(mX) {
                        ASSERTV(LENGTH, CONFIG, X.size(), exceptionBlockPass,
                                                 0 == scratch.numBytesInUse());

                        ExceptionGuard<Obj> guard(&X, L_, &scratch);

                        bslma::TestAllocatorMonitor tam(&oa);
                        Iter RESULT = mX.insert(K);

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
                        ASSERTV(LENGTH, CONFIG, K == *RESULT);

                        ASSERTV(exceptionBlockPass, X.size(), LENGTH,
                                                   2 * LENGTH - 1 == X.size());

                        guard.release();
                        ++exceptionBlockPass;
                    } EXCEPTION_TEST_END
                }
                mX.insert(values[LENGTH - 1]);

                ASSERTV(LENGTH, CONFIG, X.size(), exceptionBlockPass,
                                                 0 == scratch.numBytesInUse());

                ASSERTV(LENGTH, CONFIG, X.size(), 2 * LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    size_t *foundValues = new size_t[values.size()];
                    for (size_t j = 0; j < values.size(); ++j) {
                        foundValues[j] = 0;
                    }

                    size_t i = 0;
                    for (CIter it = X.cbegin(); it != X.cend(); ++it, ++i) {
                        for (size_t j = 0; j < values.size(); ++j) {
                            if (values[j] == *it) {
                                ++foundValues[j];
                            }
                        }
                    }
                    ASSERTV(LENGTH, i, 2 * LENGTH == i);
                    size_t missing = 0;
                    for (size_t j = 0; j != values.size(); ++j) {
                        if (foundValues[j] != (j < LENGTH ? 2 : 0)) {
                            ++missing;
                        }
                    }
                    ASSERTV(LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundValues;

                    ASSERTV(LENGTH, CONFIG, X.size() == i);
                }

            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'clear'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                int numPasses = 0;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    mX.clear();
                } EXCEPTION_TEST_END
                ASSERTV(1 == numPasses);

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf(
                      "\n\tTesting 'insert' duplicated values and 'count'.\n");
            {
                for (size_t i = 0; i < 3; ++i) {
                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const KEY& K = values[tj];

                        for (size_t tk = 0; tk < values.size(); ++tk) {
                            const KEY& KK = values[tk];
                            const size_t EXPECTED = tk >= LENGTH
                                                  ? 0
                                                  : tk <  tj ? i + 1 : i;
                            ASSERTV(LENGTH, i, EXPECTED == X.count(KK));
                        }
                        Iter RESULT = mX.insert(K);
                        ASSERTV(LENGTH, tj, CONFIG, K == *RESULT);
                        for (size_t tk = 0; tk < values.size(); ++tk) {
                            const KEY& KK = values[tk];
                            const size_t EXPECTED = tk >= LENGTH
                                                  ? 0
                                                  : tk <= tj ? i + 1 : i;
                            ASSERTV(LENGTH, i, EXPECTED == X.count(KK));
                        }
                    }
                }
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

    // mX.insert(KEY&&) -- not tested / C++11

    if (verbose) printf("Test insert with hint\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mX(&sa);    const Obj& X = mX;

            Iter it;
            bool firstTime = true;
            for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
                const KEY& K = values[ti];

                for (size_t tj = 0; tj < 3; ++tj) {
                    ASSERTV(ti, tj, tj == X.count(K));
                    it = firstTime ? mX.insert(K) : mX.insert(it, K);
                    firstTime = false;
                    ASSERTV(ti, tj, K == *it);
                }
                ASSERTV(ti, 3 == X.count(K));
            }

            for (size_t tj = 0; tj < values.size(); ++tj) {
                ASSERTV(MAX_LENGTH, tj, (tj < MAX_LENGTH ? 3 : 0) ==
                                                          X.count(values[tj]));
            }
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // mX.insert(hint, KEY&&) -- not tested / C++11

    if (verbose) printf("Testing range insert\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mX(&sa);    const Obj& X = mX;

            mX.insert(values.begin(), values.index(MAX_LENGTH));

            for (size_t ti = 0; ti < values.size(); ++ti) {
                ASSERTV((ti < MAX_LENGTH ? 1 : 0) == X.count(values[ti]));
            }

            values.resetIterators();

            mX.insert(values.begin(), values.index('Z' + 1 - 'A'));

            for (size_t ti = 0; ti < values.size(); ++ti) {
                const size_t EXP = ti < MAX_LENGTH
                                 ? 2
                                 : ti <= 'Z' - 'A'
                                 ? 1
                                 : 0;
                ASSERTV(EXP == X.count(values[ti]));
            }

            values.resetIterators();
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // void insert(initializer_list<value_type>) -- not tested / C++11

    if (verbose) printf("Test non-const find and erase(it)\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        for (size_t ti = 0; ti <= MAX_LENGTH; ++ti) {
            Obj mX(&sa);    const Obj& X = mX;
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();

            // There should be no throwing for the rest of the loop body.

            int numPasses = 0;
            EXCEPTION_TEST_BEGIN(mX) {
                ++numPasses;

                KEY itKey = values[ti];
                Iter it = mX.find(itKey);
                if (MAX_LENGTH == ti) {
                    ASSERTV(mX.end() == it);
                }
                else {
                    ASSERTV(mX.end() != it);
                    ASSERTV(itKey == *it);

                    Iter eraseIt = mX.erase(it);
                    ASSERT(MAX_LENGTH - 1 == X.size());

                    // Verify that 'eraseIt' is a valid iterator, though it may
                    // be 'X.end()'.

                    if (X.end() != eraseIt) {
                        const KEY& eraseItKey = *eraseIt;

                        bool found = false;
                        for (it = mX.begin(); mX.end() != it; ++it) {
                            ASSERTV(itKey != *it);
                            if (eraseItKey == *it) {
                                ASSERT(!found);
                                found = true;
                            }
                        }
                        ASSERTV(found);
                    }
                }

                for (size_t tj = 0; tj < values.size(); ++tj) {
                    const KEY& K = values[tj];

                    const size_t EXPECTED = tj < MAX_LENGTH && K != itKey;
                    ASSERTV(EXPECTED == X.count(K));
                }
            } EXCEPTION_TEST_END
            ASSERTV(1 == numPasses);
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    if (verbose) printf("Test non-const equal_range and erase(it1, it2)\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        for (size_t ti = 0; ti <= MAX_LENGTH; ++ti) {
            Obj mX(&sa);    const Obj& X = mX;
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();

            // Nothing should throw for the rest of the loop body.

            int numPasses = 0;
            EXCEPTION_TEST_BEGIN(mX) {
                ASSERTV(2 * MAX_LENGTH == X.size());

                const KEY itKey = values[ti];

                ++numPasses;

                bsl::pair<Iter, Iter> pr = mX.equal_range(itKey);

                if (MAX_LENGTH == ti) {
                    ASSERTV(pr.first == pr.second);
                    ASSERTV(mX.end() == pr.first);
                }
                Iter after = mX.erase(pr.first, pr.second);

                ASSERTV(pr.second == after);
                ASSERTV(2 * MAX_LENGTH - (MAX_LENGTH == ti ? 0 : 2) ==
                                                                     X.size());
                ASSERTV(0 == X.count(itKey));

                for (size_t tj = 0; tj < values.size(); ++tj) {
                    const KEY& K = values[tj];

                    const size_t EXPECTED = 2*(tj < MAX_LENGTH && K != itKey);
                    ASSERTV(EXPECTED == X.count(K));
                }
            } EXCEPTION_TEST_END
            ASSERTV(1 == numPasses);
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    if (verbose) printf("Test non-const begin(i) & end(i)\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mX(&sa);    const Obj& X = mX;
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();

            typedef typename Obj::local_iterator LIter;

            size_t count = 0;
            for (size_t tb = 0; tb < X.bucket_count(); ++tb) {
                const LIter END = mX.end(tb);
                for (LIter li = mX.begin(tb); END != li; ++li) {
                    ++count;
                }
                ASSERTV(0 == (count & 1));    // always even # in each bucket
            }

            ASSERTV(X.size() == count);
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    if (verbose) printf("Test erase(key)\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        for (size_t ti = 0; ti <= MAX_LENGTH; ++ti) {
            Obj mX(&sa);    const Obj& X = mX;
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();

            // Should not throw for the rest of the loop body.

            int numPasses = 0;
            EXCEPTION_TEST_BEGIN(mX) {
                ++numPasses;

                KEY itKey = values[ti];
                const size_t COUNT = mX.erase(itKey);
                if (MAX_LENGTH == ti) {
                    ASSERT(0 == COUNT);
                    ASSERT(2 * MAX_LENGTH     == X.size());
                }
                else {
                    ASSERT(2 == COUNT);
                    ASSERT(2 * MAX_LENGTH - 2 == X.size());
                }

                // Verify that 'eraseIt' is a valid iterator, though it may be
                // 'X.end()'.

                for (size_t tj = 0; tj < values.size(); ++tj) {
                    const KEY& K = values[tj];

                    const size_t EXPECTED = 2*(tj < MAX_LENGTH && K != itKey);
                    ASSERTV(EXPECTED == X.count(K));
                }
            } EXCEPTION_TEST_END
            ASSERTV(1 == numPasses);
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // Note 'rehash' will not shrink the # of buckets.

    if (verbose) printf("Test 'reserve' and 'rehash'\n");
    {
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        enum { MIN_LENGTH = MAX_LENGTH - 2 };

        {
            Obj mX(&sa);    const Obj& X = mX;
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();
            mX.insert(values.begin(), values.index(MAX_LENGTH));
            values.resetIterators();

            const size_t ORIG_BUCKETS  = X.bucket_count();
            float maxLoadFactor = X.max_load_factor();

            if (veryVerbose) P(ORIG_BUCKETS);
            if (veryVerbose) P(maxLoadFactor);

            ASSERTV(static_cast<float>(ORIG_BUCKETS) * maxLoadFactor >=
                                                                     X.size());

            maxLoadFactor /= 10;

            mX.max_load_factor(maxLoadFactor);

            // Do something to make sure the hash table still works.

            for (size_t ti = 0; ti < values.size(); ++ti) {
                ASSERTV((ti < MAX_LENGTH ? 2 : 0) == X.count(values[ti]));
            }

            const size_t MORE_BUCKETS = X.bucket_count();

            if (veryVerbose) { P(MORE_BUCKETS); P(X.max_load_factor()); }

            ASSERTV(maxLoadFactor == X.max_load_factor());
            ASSERTV(MORE_BUCKETS > ORIG_BUCKETS);
            ASSERTV(static_cast<float>(MORE_BUCKETS) * maxLoadFactor >=
                                                                     X.size());

            mX.reserve(10 * X.size());

            // Do something to make sure the hash table still works.

            for (size_t ti = 0; ti < values.size(); ++ti) {
                ASSERTV((ti < MAX_LENGTH ? 2 : 0) == X.count(values[ti]));
            }

            const size_t EVEN_MORE_BUCKETS = X.bucket_count();

            if (veryVerbose) { P(MORE_BUCKETS); P(X.max_load_factor()); }

            ASSERTV(nearlyEqual<float>(maxLoadFactor, X.max_load_factor()));
            ASSERTV(EVEN_MORE_BUCKETS > MORE_BUCKETS);
            ASSERTV(static_cast<float>(EVEN_MORE_BUCKETS) * maxLoadFactor >=
                                                                10 * X.size());

            mX.rehash(EVEN_MORE_BUCKETS * 2);

            // Do something to make sure the hash table still works.

            for (size_t ti = 0; ti < values.size(); ++ti) {
                ASSERTV((ti < MAX_LENGTH ? 2 : 0) == X.count(values[ti]));
            }

            const size_t MAX_BUCKETS = X.bucket_count();

            ASSERTV(maxLoadFactor == X.max_load_factor());
            ASSERTV(MAX_BUCKETS, MAX_BUCKETS > EVEN_MORE_BUCKETS);
            ASSERTV(static_cast<float>(MAX_BUCKETS) * maxLoadFactor >=
                                                                20 * X.size());

            if (veryVerbose) { P(MAX_BUCKETS); P(X.max_load_factor()); }
        }

        // Verify all memory is released on object destruction.

        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
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
// Unordered sets are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).
//
// One uses a multi-set (ordered or unordered) when there may be more than one
// instance of an element of a set and when that multiplicity must be
// preserved.
//
// Note that the data type described below is an augmentation of that used in
// {'bslstl_unorderedset|Example 1}.  The data itself (randomly generated) is
// different.
//
// Suppose one is analyzing data on a set of customers, and each customer is
// categorized by several attributes: customer type, geographic area, and
// (internal) project code; and that each attribute takes on one of a limited
// set of values.  Additionally, there is some financial data associated with
// each customer: past sales and pending sales.
//
// The several customer attributes are modeled by several enumerations:
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
    static const struct CustomerDatum {
        CustomerCode d_customer;
        LocationCode d_location;
        ProjectCode  d_project;
        double       d_past;
        double       d_pending;
    } customerData[] = {
       { REPEAT    , RUSSIA  , SMITH,   75674.00,     455.00 },
       { REPEAT    , ENGLAND , TOAST,   35033.00,    8377.00 },
       { BUSINESS  , USA_EAST, SMITH,   53942.00,    2782.00 },
       { NON_PROFIT, RUSSIA  , TIDY ,   97498.00,    1136.00 },
       { NON_PROFIT, SCOTLAND, TIDY ,   14490.00,    9648.00 },
       { INSTITUTE , RUSSIA  , PEARL,   90812.00,    5600.00 },
       { NEED_BASED, ENGLAND , TIDY ,   86111.00,    1110.00 },
       { DISCOUNT  , USA_WEST, PEARL,   88588.00,    5330.00 },
       { INSTITUTE , ENGLAND , TIDY ,   38360.00,    1903.00 },
       { IMPULSE   , RUSSIA  , SMITH,    4698.00,    5687.00 },
       { IMPULSE   , ENGLAND , GREEN,   24000.00,    7017.00 },
       { REPEAT    , USA_EAST, TIDY ,   61027.00,    1883.00 },
       { IMPULSE   , SCOTLAND, GREEN,   39279.00,    3197.00 },
       { REPEAT    , RUSSIA  , SMITH,   62097.00,     906.00 },
       { IMPULSE   , GERMANY , TOAST,    2523.00,    8680.00 },
       { DISCOUNT  , CANADA  , PEARL,    2862.00,    7786.00 },
       { IMPULSE   , SCOTLAND, TOAST,   54458.00,    7363.00 },
       { BUSINESS  , CANADA  , TIDY ,   22851.00,    3691.00 },
       { NEED_BASED, GERMANY , PEARL,   89616.00,    3799.00 },
       { IMPULSE   , RUSSIA  , GREEN,   99643.00,    7220.00 },
       { NEED_BASED, CANADA  , PEARL,   49153.00,    3446.00 },
       { DISCOUNT  , ENGLAND , GREEN,   59747.00,    6514.00 },
       { DISCOUNT  , CANADA  , TOAST,   80057.00,    3951.00 },
       { IMPULSE   , USA_WEST, TOAST,   90030.00,    9840.00 },
       { NEED_BASED, RUSSIA  , GREEN,    8879.00,    8362.00 },
       { DISCOUNT  , SCOTLAND, TIDY ,   10474.00,    5305.00 },
       { REPEAT    , USA_EAST, PEARL,   45020.00,    1496.00 },
       { BUSINESS  , RUSSIA  , PEARL,   29386.00,    3623.00 },
       { IMPULSE   , GERMANY , GREEN,   41218.00,    6356.00 },
       { BUSINESS  , RUSSIA  , TIDY ,    1542.00,    6080.00 },
       { BUSINESS  , USA_EAST, TOAST,   48644.00,    6414.00 },
       { REPEAT    , RUSSIA  , GREEN,    4328.00,    5767.00 },
       { INSTITUTE , GERMANY , TIDY ,   21184.00,    1230.00 },
       { REPEAT    , RUSSIA  , TOAST,   54475.00,    1823.00 },
       { NON_PROFIT, ENGLAND , FAST ,   17887.00,    6231.00 },
       { NEED_BASED, MEXICO  , FAST ,   58513.00,    1832.00 },
       { REPEAT    , GERMANY , TOAST,   37185.00,    2836.00 },
       { INSTITUTE , CANADA  , FAST ,   85849.00,    2792.00 },
       { BUSINESS  , GERMANY , SMITH,   50467.00,    9453.00 },
       { NON_PROFIT, USA_EAST, SMITH,   44695.00,    4672.00 },
       { NON_PROFIT, USA_WEST, GREEN,    5561.00,    4756.00 },
       { NEED_BASED, ENGLAND , FAST ,   74287.00,    9272.00 },
       { NON_PROFIT, SCOTLAND, SMITH,   17136.00,    4325.00 },
       { REPEAT    , RUSSIA  , PEARL,   18733.00,    6399.00 },
       { NEED_BASED, SCOTLAND, TOAST,   26851.00,    9798.00 },
       { INSTITUTE , CANADA  , TIDY ,   76474.00,    6930.00 },
       { NEED_BASED, FRANCE  , FAST ,   78430.00,    9171.00 },
       { INSTITUTE , GERMANY , SMITH,   34939.00,    4734.00 },
       { NON_PROFIT, SCOTLAND, FAST ,   95477.00,    5591.00 },
       { INSTITUTE , FRANCE  , FAST ,   19349.00,    3982.00 },
       { REPEAT    , MEXICO  , TOAST,    7469.00,    5958.00 },
       { DISCOUNT  , CANADA  , PEARL,     576.00,    9260.00 },
       { DISCOUNT  , USA_WEST, GREEN,   43456.00,    6305.00 },
       { IMPULSE   , SCOTLAND, GREEN,   92301.00,    8408.00 },
       { BUSINESS  , SCOTLAND, FAST ,   24443.00,    4247.00 },
       { NEED_BASED, RUSSIA  , TIDY ,   50712.00,    8647.00 },
       { INSTITUTE , CANADA  , TIDY ,   77761.00,    2941.00 },
       { INSTITUTE , SCOTLAND, TIDY ,   78240.00,    6635.00 },
       { IMPULSE   , GERMANY , GREEN,   85796.00,    1634.00 },
       { NEED_BASED, SCOTLAND, TOAST,   27761.00,     371.00 },
       { INSTITUTE , FRANCE  , PEARL,   47747.00,    3533.00 },
       { BUSINESS  , USA_WEST, TIDY ,   39360.00,    1132.00 },
       { DISCOUNT  , ENGLAND , SMITH,   82644.00,    8622.00 },
       { INSTITUTE , CANADA  , TIDY ,   96349.00,    4296.00 },
       { BUSINESS  , GERMANY , FAST ,   73965.00,    3527.00 },
       { REPEAT    , ENGLAND , TIDY ,   57219.00,    8957.00 },
       { BUSINESS  , ENGLAND , SMITH,   34815.00,    9135.00 },
       { NEED_BASED, USA_WEST, FAST ,   11387.00,    8332.00 },
       { IMPULSE   , ENGLAND , TIDY ,   60982.00,    3094.00 },
       { NEED_BASED, FRANCE  , FAST ,   89053.00,    1622.00 },
       { NEED_BASED, USA_EAST, TIDY ,   85607.00,    2515.00 },
       { IMPULSE   , MEXICO  , TIDY ,   42198.00,    1652.00 },
       { NON_PROFIT, FRANCE  , FAST ,   65512.00,    2502.00 },
       { INSTITUTE , CANADA  , GREEN,   66175.00,    6099.00 },
       { NEED_BASED, FRANCE  , PEARL,   19735.00,    2504.00 },
       { DISCOUNT  , GERMANY , TIDY ,   58853.00,     454.00 },
       { BUSINESS  , SCOTLAND, SMITH,   66101.00,    4416.00 },
       { NEED_BASED, ENGLAND , GREEN,   25517.00,    9096.00 },
       { NON_PROFIT, RUSSIA  , TIDY ,   25109.00,    3813.00 },
       { DISCOUNT  , MEXICO  , TIDY ,   18660.00,    5430.00 },
       { REPEAT    , USA_WEST, TIDY ,   83483.00,    6099.00 },
       { REPEAT    , CANADA  , SMITH,   33225.00,     741.00 },
       { NEED_BASED, USA_EAST, SMITH,   75453.00,    6297.00 },
       { DISCOUNT  , FRANCE  , TOAST,   74624.00,    4745.00 },
       { IMPULSE   , USA_WEST, SMITH,   90923.00,    8314.00 },
       { NEED_BASED, ENGLAND , TIDY ,   57465.00,    7906.00 },
       { NEED_BASED, USA_WEST, TIDY ,   23203.00,    1218.00 },
       { IMPULSE   , MEXICO  , GREEN,   26972.00,    3087.00 },
       { IMPULSE   , RUSSIA  , GREEN,   73822.00,    7107.00 },
       { INSTITUTE , MEXICO  , FAST ,   55671.00,    2009.00 },
       { IMPULSE   , FRANCE  , TIDY ,    3512.00,    7849.00 },
       { NON_PROFIT, USA_WEST, PEARL,   80276.00,    5940.00 },
       { NEED_BASED, CANADA  , FAST ,   97479.00,     681.00 },
       { NEED_BASED, GERMANY , GREEN,   15332.00,    6094.00 },
       { DISCOUNT  , ENGLAND , TIDY ,   74110.00,    2706.00 },
       { DISCOUNT  , ENGLAND , FAST ,   68620.00,    9449.00 },
       { REPEAT    , ENGLAND , SMITH,    4301.00,    4985.00 },
       { BUSINESS  , GERMANY , TIDY ,   84553.00,    3379.00 },
       { BUSINESS  , CANADA  , GREEN,   60716.00,    7603.00 },
       { DISCOUNT  , MEXICO  , GREEN,   99737.00,    3872.00 },
    };

    const size_t numCustomerData = sizeof customerData / sizeof *customerData;
//..
// Suppose, as a step in analysis, we wish to determine the average of the past
// sales and the average of the pending sales for each customer for each unique
// combination of customer attributes (i.e., for each customer profile in the
// data set).  To do so, we must aggregate our data items by customer profile
// but also retain the unique financial data for each item.  The
// 'bslstl_unorderedmultiset' provides those semantics.
//
// First, as there are no standard methods for hashing or comparing our user
// defined types, we define 'CustomerDatumHash' and 'CustomerDatumEqual'
// classes, each a stateless functor.  Note that there is no meaningful
// ordering of the attribute values, they are merely arbitrary code numbers;
// nothing is lost by using an unordered set instead of an ordered set:
//..
    class CustomerDatumHash
    {
      public:
        // CREATORS
        //! CustomerDatumHash() = default;
            // Create a 'CustomerDatumHash' object.

        //! hash(const CustomerDatumHash& original) = default;
            // Create a 'CustomerDatumHash' object.  Note that as
            // 'CustomerDatumHash' is an empty (stateless) type, this operation
            // will have no observable effect.

        //! ~CustomerDatumHash() = default;
            // Destroy this object.

        // ACCESSORS
        std::size_t operator()(CustomerDatum x) const;
            // Return a hash value computed using the specified 'x'.
    };

    // ACCESSORS
    std::size_t CustomerDatumHash::operator()(CustomerDatum x) const
    {
        return bsl::hash<int>()(x.d_location * 100 * 100
                              + x.d_customer * 100
                              + x.d_project);
    }

    class CustomerDatumEqual
    {
      public:
        // CREATORS
        //! CustomerDatumEqual() = default;
            // Create a 'CustomerDatumEqual' object.

        //! CustomerDatumEqual(const CustomerDatumEqual& original) = default;
            // Create a 'CustomerDatumEqual' object.  Note that as
            // 'CustomerDatumEqual' is an empty (stateless) type, this
            // operation will have no observable effect.

        //! ~CustomerDatumEqual() = default;
            // Destroy this object.

        // ACCESSORS
        bool operator()(const CustomerDatum& lhs,
                        const CustomerDatum& rhs) const;
    };

    // ACCESSORS
    bool CustomerDatumEqual::operator()(const CustomerDatum& lhs,
                                        const CustomerDatum& rhs) const
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
// Also notice that the boolean operation provided by 'CustomerDatumEqual' is
// more properly thought of as "equivalence", not "equality".  There may be
// more than one data item with the same customer profile (i.e., the same for
// our purpose here), but they have distinct financial data so the two items
// are not equal (unless the financial data also happens to match).
//
// Next, we define the type of the unordered set and a convenience aliases:
//..
    typedef bsl::unordered_multiset<CustomerDatum,
                                    CustomerDatumHash,
                                    CustomerDatumEqual> DataByProfile;
    typedef DataByProfile::const_iterator               DataByProfileConstItr;
//..
// Now, create a helper function to calculate the average financials for a
// category of customer profiles within the unordered map.
//..
    void processCategory(DataByProfileConstItr  start,
                         DataByProfileConstItr  end,
                         FILE                  *out)
        // Print to the specified 'out' in some human-readable format the
        // averages of the 'past' and 'pending' attributes of every
        // 'CustomerInfoData' object from the specified 'start' up to (but not
        // including) the specified 'end'.  The behavior is undefined unless
        // 'end != start'.
    {
        ASSERT(end != start);
        ASSERT(out);

        double sumPast    = 0.0;
        double sumPending = 0.0;
        int    count      = 0;

        for (DataByProfileConstItr itr = start; end != itr; ++itr) {
            sumPast    += itr->d_past;
            sumPending += itr->d_pending;
            ++count;
        }
if (verbose) {
        printf("%-10s %-8s %-5s %10.2f %10.2f\n",
               toAscii(start->d_customer),
               toAscii(start->d_location),
               toAscii(start->d_project),
               sumPast/count,
               sumPending/count);
}
    }
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
      case 16: {
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
    DataByProfile dataByProfile;

    for (size_t idx = 0; idx < numCustomerData; ++idx) {
       dataByProfile.insert(customerData[idx]);
    }
    ASSERT(numCustomerData == dataByProfile.size());
//..
// Finally, to calculate the statistics we need, we must detect the transition
// between categories as we iterate through 'customerInfoData'.
//..
    CustomerDatumEqual    areEquivalent;
    DataByProfileConstItr end             = dataByProfile.end();
    DataByProfileConstItr startOfCategory = end;

    for (DataByProfileConstItr itr  = dataByProfile.begin();
                               end != itr; ++itr) {
        if (end == startOfCategory) {
            startOfCategory = itr;
            continue;
        }

        if (!areEquivalent(*startOfCategory, *itr)) {
            processCategory(startOfCategory, itr, stdout);
            startOfCategory = itr;
        }
    }
    if (end != startOfCategory) {
        processCategory(startOfCategory, end, stdout);
    }
//..
// We find on standard output:
//..
//  BUSINESS   GERMANY  TIDY    84553.00    3379.00
//  DISCOUNT   ENGLAND  TIDY    74110.00    2706.00
//  NEED_BASED CANADA   FAST    97479.00     681.00
//  NON_PROFIT USA_WEST PEARL   80276.00    5940.00
//  IMPULSE    FRANCE   TIDY     3512.00    7849.00
//  IMPULSE    MEXICO   GREEN   26972.00    3087.00
//  DISCOUNT   MEXICO   GREEN   99737.00    3872.00
//  IMPULSE    USA_WEST SMITH   90923.00    8314.00
//  DISCOUNT   FRANCE   TOAST   74624.00    4745.00
//  NEED_BASED USA_EAST SMITH   75453.00    6297.00
//  NEED_BASED GERMANY  GREEN   15332.00    6094.00
//  DISCOUNT   MEXICO   TIDY    18660.00    5430.00
//  NEED_BASED FRANCE   PEARL   19735.00    2504.00
//  DISCOUNT   ENGLAND  FAST    68620.00    9449.00
//  BUSINESS   SCOTLAND SMITH   66101.00    4416.00
//  INSTITUTE  CANADA   GREEN   66175.00    6099.00
//  IMPULSE    MEXICO   TIDY    42198.00    1652.00
//  NEED_BASED USA_EAST TIDY    85607.00    2515.00
//  NON_PROFIT FRANCE   FAST    65512.00    2502.00
//  NEED_BASED USA_WEST FAST    11387.00    8332.00
//  BUSINESS   GERMANY  FAST    73965.00    3527.00
//  BUSINESS   CANADA   GREEN   60716.00    7603.00
//  REPEAT     CANADA   SMITH   33225.00     741.00
//  DISCOUNT   ENGLAND  SMITH   82644.00    8622.00
//  REPEAT     ENGLAND  TIDY    57219.00    8957.00
//  BUSINESS   USA_WEST TIDY    39360.00    1132.00
//  NEED_BASED RUSSIA   GREEN    8879.00    8362.00
//  DISCOUNT   USA_WEST GREEN   43456.00    6305.00
//  NEED_BASED GERMANY  PEARL   89616.00    3799.00
//  NEED_BASED CANADA   PEARL   49153.00    3446.00
//  DISCOUNT   GERMANY  TIDY    58853.00     454.00
//  IMPULSE    RUSSIA   GREEN   86732.50    7163.50
//  REPEAT     GERMANY  TOAST   37185.00    2836.00
//  INSTITUTE  MEXICO   FAST    55671.00    2009.00
//  IMPULSE    SCOTLAND TOAST   54458.00    7363.00
//  REPEAT     USA_EAST TIDY    61027.00    1883.00
//  REPEAT     RUSSIA   GREEN    4328.00    5767.00
//  DISCOUNT   CANADA   PEARL    1719.00    8523.00
//  DISCOUNT   ENGLAND  GREEN   59747.00    6514.00
//  REPEAT     USA_EAST PEARL   45020.00    1496.00
//  IMPULSE    SCOTLAND GREEN   65790.00    5802.50
//  NON_PROFIT RUSSIA   TIDY    61303.50    2474.50
//  NON_PROFIT USA_EAST SMITH   44695.00    4672.00
//  DISCOUNT   SCOTLAND TIDY    10474.00    5305.00
//  NEED_BASED USA_WEST TIDY    23203.00    1218.00
//  BUSINESS   USA_EAST SMITH   53942.00    2782.00
//  DISCOUNT   CANADA   TOAST   80057.00    3951.00
//  REPEAT     USA_WEST TIDY    83483.00    6099.00
//  BUSINESS   CANADA   TIDY    22851.00    3691.00
//  BUSINESS   ENGLAND  SMITH   34815.00    9135.00
//  INSTITUTE  GERMANY  SMITH   34939.00    4734.00
//  INSTITUTE  RUSSIA   PEARL   90812.00    5600.00
//  IMPULSE    GERMANY  GREEN   63507.00    3995.00
//  REPEAT     ENGLAND  TOAST   35033.00    8377.00
//  INSTITUTE  ENGLAND  TIDY    38360.00    1903.00
//  NEED_BASED ENGLAND  TIDY    71788.00    4508.00
//  BUSINESS   GERMANY  SMITH   50467.00    9453.00
//  DISCOUNT   USA_WEST PEARL   88588.00    5330.00
//  IMPULSE    USA_WEST TOAST   90030.00    9840.00
//  REPEAT     RUSSIA   SMITH   68885.50     680.50
//  IMPULSE    RUSSIA   SMITH    4698.00    5687.00
//  REPEAT     RUSSIA   PEARL   18733.00    6399.00
//  NON_PROFIT SCOTLAND TIDY    14490.00    9648.00
//  IMPULSE    ENGLAND  GREEN   24000.00    7017.00
//  BUSINESS   RUSSIA   TIDY     1542.00    6080.00
//  BUSINESS   USA_EAST TOAST   48644.00    6414.00
//  REPEAT     RUSSIA   TOAST   54475.00    1823.00
//  NEED_BASED ENGLAND  GREEN   25517.00    9096.00
//  INSTITUTE  GERMANY  TIDY    21184.00    1230.00
//  IMPULSE    ENGLAND  TIDY    60982.00    3094.00
//  NON_PROFIT ENGLAND  FAST    17887.00    6231.00
//  INSTITUTE  CANADA   FAST    85849.00    2792.00
//  REPEAT     ENGLAND  SMITH    4301.00    4985.00
//  NON_PROFIT SCOTLAND FAST    95477.00    5591.00
//  NEED_BASED MEXICO   FAST    58513.00    1832.00
//  NON_PROFIT USA_WEST GREEN    5561.00    4756.00
//  NEED_BASED ENGLAND  FAST    74287.00    9272.00
//  IMPULSE    GERMANY  TOAST    2523.00    8680.00
//  NON_PROFIT SCOTLAND SMITH   17136.00    4325.00
//  NEED_BASED SCOTLAND TOAST   27306.00    5084.50
//  INSTITUTE  CANADA   TIDY    83528.00    4722.33
//  NEED_BASED FRANCE   FAST    83741.50    5396.50
//  REPEAT     MEXICO   TOAST    7469.00    5958.00
//  BUSINESS   SCOTLAND FAST    24443.00    4247.00
//  INSTITUTE  FRANCE   FAST    19349.00    3982.00
//  NEED_BASED RUSSIA   TIDY    50712.00    8647.00
//  INSTITUTE  SCOTLAND TIDY    78240.00    6635.00
//  BUSINESS   RUSSIA   PEARL   29386.00    3623.00
//  INSTITUTE  FRANCE   PEARL   47747.00    3533.00
//..
//
///Example 2: Examining and Setting Unordered Multi-Set Configuration
///------------------------------------------------------------------
// The unordered multi-set interfaces provide some insight into and control of
// its inner workings.  The syntax and semantics of these interfaces for
// 'bslstl_unoroderedmultiset' are identical to those of 'bslstl_unorderedmap'.
// See the material in {'bslstl_unorderedmap'|Example 2}.

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING HASH_FUNCTION AND KEY_EQ
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING HASH_FUNCTION AND KEY_EQ\n"
                            "================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING STL ALLOCATOR\n"
                            "=====================\n");

        RUN_EACH_TYPE(StdAllocTestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING REHASH, RESERVE, and SET MAX_LOAD_FACTOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Rehash, Reserve, and max_load_factor\n"
                            "============================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING RANGE C'TORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Range C'tors\n"
                            "====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'G' FUNCTION
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'g' function\n"
                            "====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX -- N/A
        // --------------------------------------------------------------------

        if (verbose) printf("BSLX: N/A\n"
                            "=========\n");
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
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Swap\n"
                            "============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CTOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy C'tor\n"
                            "==================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // OPERATOR==
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Equality Comparison\n"
                            "===========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // OPERATOR<<
        // --------------------------------------------------------------------

        if (verbose) printf("<< Operator: N/A\n"
                            "================\n");
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

        typedef bsl::unordered_multiset<int> TestType;

        if (veryVerbose)
            printf("Default construct an unordered_mutliset, 'x'\n");

        TestType mX;
        const TestType& X = mX;

        if (veryVerbose) printf("Validate default behavior of 'x'\n");

        ASSERT(nearlyEqual<float>(1.0f, X.max_load_factor()));

        testConstEmptyContainer(X);
        testEmptyContainer(mX);

        swap(mX, mX);

        testConstEmptyContainer(X);
        testEmptyContainer(mX);

        validateIteration(mX);

        if (veryVerbose) printf("Prepare a test array of data samples\n");

        const int MAX_SAMPLE = 10000;
//        const int MAX_SAMPLE = 7;  // minimum size to support all erase tests
        int *dataSamples = new int[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            dataSamples[i] = i;
        }

        if (veryVerbose)  printf(
          "Range-construct an unordered_multiset, 'y', from the test array\n");

        TestType mY(dataSamples, dataSamples + MAX_SAMPLE);
        const TestType& Y = mY;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'y'\n");

        ASSERT(nearlyEqual<float>(1.0f, Y.max_load_factor()));

        testContainerHasData(Y, 1, dataSamples, MAX_SAMPLE);
        validateIteration(mY);

        if (veryVerbose)
            printf("Assert equality relationships, noting 'x != y'\n");

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

        if (veryVerbose)
            printf("Assert swapped equality relationships, noting 'x != y'\n");

        ASSERT(X == X);
        ASSERT(!(X != X));
        ASSERT(Y != X);
        ASSERT(!(Y == X));
        ASSERT(X != Y);
        ASSERT(!(X == Y));
        ASSERT(Y == Y);
        ASSERT(!(Y != Y));

        testBuckets(mX);

        if (veryVerbose) printf("Try to fill 'x' with duplicate values\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            LOOP_ASSERT(i, 1 == X.count(dataSamples[i]));
            TestType::iterator it = mX.insert(dataSamples[i]);
            ASSERT(X.end() != it);
            ASSERT(*it == dataSamples[i]);
            LOOP_ASSERT(i, 2 == X.count(dataSamples[i]));
        }

        testBuckets(mX);
        validateIteration(mX);

        if (veryVerbose) printf(
          "Confirm the value of 'x' with the successfully inserted values.\n");

        testContainerHasData(X, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                "Create an unordered_multiset, 'z', that is a copy of 'x'.\n");

        TestType mZ = X;
        const TestType& Z = mZ;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'z'.\n");

        ASSERT(nearlyEqual<float>(1.0f, Z.max_load_factor()));
        ASSERT(X == Z);
        ASSERT(!(X != Z));
        ASSERT(Z == X);
        ASSERT(!(Z != X));

        validateIteration(mZ);
        testContainerHasData(Z, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                             "Expand 'z' with additional duplicate values.\n");

        for (int i = 3; i != 6; ++i) {
            fillContainerWithData(mZ, dataSamples, MAX_SAMPLE);
            testContainerHasData(Z, i, dataSamples, MAX_SAMPLE);
            validateIteration(mZ);
        }
        ASSERT(X != Z);
        ASSERT(!(X == Z));
        ASSERT(Z != X);
        ASSERT(!(Z == X));

        if (veryVerbose)
            printf("Confirm that 'x' is unchanged by making the copy.\n");

        testBuckets(mX);
        validateIteration(mX);
        testContainerHasData(X, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose)
            printf("Clear 'x' and confirm that it is empty.\n");

        mX.clear();
        testEmptyContainer(mX);
        testBuckets(mX);

        if (veryVerbose)
            printf("Assign the value of 'y' to 'x'.\n");

        mX = Y;

        if (veryVerbose) printf("Confirm 'x' has the expected value.\n");

        ASSERT(X == Y);

        validateIteration(mX);
        testBuckets(mX);

        testErase(mZ);

        if (veryVerbose) printf(
             "Call any remaining methods to be sure they at least compile.\n");

        mX.insert(1);

        const bsl::allocator<int> alloc   = X.get_allocator();
        (void) alloc;
        const bsl::hash<int>      hasher  = X.hash_function();
        (void) hasher;
        const bsl::equal_to<int>  compare = X.key_eq();
        (void) compare;

        const size_t maxSize    = X.max_size();    (void) maxSize;
        const size_t buckets    = X.bucket_count();
        const float  loadFactor = X.load_factor();
        const float  maxLF      = X.max_load_factor();

        ASSERT(loadFactor < maxLF);

        mX.rehash(2 * buckets);
        ASSERTV(X.bucket_count(), 2 * buckets, X.bucket_count() > 2 * buckets);
        ASSERTV(X.load_factor(), loadFactor, X.load_factor() < loadFactor);

        mX.reserve(0);
        ASSERTV(X.bucket_count(), 2 * buckets, X.bucket_count() > 2 * buckets);
        ASSERTV(X.load_factor(), loadFactor, X.load_factor() < loadFactor);

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
