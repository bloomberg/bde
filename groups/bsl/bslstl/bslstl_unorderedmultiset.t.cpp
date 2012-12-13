// bslstl_unorderedmultiset.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedmultiset.h>

#include <bslstl_iterator.h>  // for testing only

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
// [  ] unordered_multiset(size_type, hasher, key_equal, allocator);
// [  ] unordered_multiset(ITER, ITER, size_type, hasher, key_equal, allocator)
// [  ] unordered_multiset(const unordered_multiset& original);
// [  ] unordered_multiset(const A& allocator);
// [  ] unordered_multiset(const u_multiset& original, const A& allocator);
// [ 2] ~unordered_multiset();
// [  ] unordered_multiset& operator=(const set& rhs);
// [ 4] allocator_type get_allocator() const;
//
// iterators:
// [  ] iterator begin();
// [  ] const_iterator begin() const;
// [  ] iterator end();
// [  ] const_iterator end() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
// [  ] local_iterator begin(size_type n);
// [  ] const_local_iterator begin(size_type n) const;
// [  ] local_iterator end(size_type n);
// [  ] const_local_iterator end(size_type n) const;
// [  ] const_local_iterator cbegin(size_type n) const;
// [  ] const_local_iterator cend(size_type n) const;
//
// capacity:
// [  ] bool empty() const;
// [ 4] size_type size() const;
// [  ] size_type max_size() const;
//
// modifiers:
// [  ] bsl::pair<iterator, bool> insert(const value_type& value);
// [  ] iterator insert(const_iterator position, const value_type& value);
// [  ] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
//
// [  ] iterator erase(const_iterator position);
// [  ] size_type erase(const key_type& key);
// [  ] iterator erase(const_iterator first, const_iterator last);
// [  ] void swap(set& other);
// [ 2] void clear();
//
// observers:
// [  ] hasher hash_function() const;
// [  ] key_equal key_eq() const;
//
// set operations:
// [  ] iterator find(const key_type& key);
// [  ] const_iterator find(const key_type& key) const;
// [  ] size_type count(const key_type& key) const;
// [  ] bsl::pair<iterator, iterator> equal_range(const key_type& key);
// [  ] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// bucket interface:
// [  ] size_type bucket_count() const;
// [  ] size_type max_bucket_count() const;
// [  ] size_type bucket_size(size_type n) const;
// [  ] size_type bucket(const key_type& k) const;
//
// hash policy:
// [  ] float load_factor() const;
// [  ] float max_load_factor() const;
// [  ] void max_load_factor(float z);
// [  ] void rehash(size_type n);
// [  ] void reserve(size_type n);
//
// specialized algorithms:
// [  ] bool operator==(u_multiset<K, H, E, A>& a, u_multiset<K, H, E, A>& b);
// [  ] bool operator!=(u_multiset<K, H, E, A>& a, u_multiset<K, H, E, A>& b);
// [  ] void swap(u_multiset<K, H, E, A>& a, u_multiset<K, H, E, A>& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] default construction (only)
// [  ] USAGE EXAMPLE
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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                              TEST SUPPORT
//-----------------------------------------------------------------------------

bool g_verbose;
bool g_veryVerbose;
bool g_veryVeryVerbose;
bool g_veryVeryVeryVerbose;
bool g_veryVeryVeryVeryVerbose;

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
    return v;        // for 'set' containers
//    return v.first;  // for 'map' containers
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
void testContainerHasData(const CONTAINER&                      x,
                          typename CONTAINER::size_type         nCopies,
                          const typename CONTAINER::value_type *data,
                          typename CONTAINER::size_type         size)
{
    typedef          CONTAINER                 TestType;
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    ASSERT(nCopies);
    ASSERT(x.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        const typename CONTAINER::value_type& testValue = data[i];
        TestIterator it = x.find(keyForValue<CONTAINER>(testValue));
        ASSERT(x.end() !=  it);
        ASSERT(data[i] == *it);
        const SizeType countValues = x.count(keyForValue<CONTAINER>(testValue));
        LOOP3_ASSERT(i, countValues, nCopies,
                     countValues == nCopies);

        bsl::pair<TestIterator, TestIterator> range =
                              x.equal_range(keyForValue<CONTAINER>(testValue));

        const SizeType rangeDist = bsl::distance(range.first, range.second);
        LOOP2_ASSERT(countValues,   rangeDist,
                     countValues == rangeDist);

        ASSERT(range.first == it);
        for(SizeType iterations = nCopies; --iterations; ++it) {
            ASSERT(testValue == *it);
        }
        // Need one extra increment to reach past-the-range iterator.
        if (++it != x.end()) {
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
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
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
    ASSERTV(ggg(object, spec) < 0);
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

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj((bslma::Allocator *)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(&sa1);
                      objAllocatorPtr = &sa1;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(&sa2);
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
    //   unordered_multiset<K,H,E,A>& gg(u_multiset<K,H,E,A> *, const char *);
    //   int ggg(unordered_multiset<K,H,E,A> *, const char *spec, int verbose);
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
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
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
    //:       (C-5..6, 14..15)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-12..13)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //:
    //:    11 Insert 'L' distinct elements and record the iterators returned.
    //:
    //:    12 Insert the same 'L' elements again and verify that incrementing
    //:       the iterators returned gives the iterator to the next smallest
    //:       value.
    //:
    //:    13 Perform P-1.2.12 again.  (C-11)
    //
    //
    // Testing:
    //   default construction (only)
    //   unordered_multiset(const allocator_type&);  // bslma::Allocator* only
    //   ~unordered_multiset();
    //   iterator insert(const value_type& value);
    //   void clear();
    // ------------------------------------------------------------------------

    const bool VALUE_TYPE_USES_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

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
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj((bslma::Allocator *)0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  return;
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
//            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

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

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Iter RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                }

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    bslma::TestAllocatorMonitor tam(&oa);
                    Iter RESULT = mX.insert(VALUES[LENGTH - 1]);

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

                    ASSERTV(LENGTH, CONFIG, VALUES[LENGTH - 1] == *RESULT);

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    size_t *foundValues = new size_t[X.size()];
                    for (size_t j = 0;j != X.size(); ++j) {
                        foundValues[j] = 0;
                    }

                    size_t i = 0;
                    for (CIter it = X.cbegin(); it != X.cend(); ++it, ++i) {
                        size_t j = 0;
                        do {
                            if (VALUES[j] == *it) {
                                ASSERTV(LENGTH, CONFIG, VALUES[j],
                                        !foundValues[j]);
                                ++foundValues[j];
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

            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'clear'.\n"); }
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.clear();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
//                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
//                ASSERTV(LENGTH, CONFIG, B, A,
//                        B - (int)LENGTH * TYPE_ALLOC == A);


                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                                "\n\tTesting 'insert' duplicated values.\n"); }
            {
                CIter ITER[MAX_LENGTH + 1];

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *ITER[tj]);
                }
                ITER[LENGTH] = X.end();

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Iter RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                    ++RESULT;
//                    ASSERTV(LENGTH, tj, CONFIG, ITER[tj + 1] == RESULT);
                }

                ASSERTV(LENGTH, CONFIG, 2 * LENGTH == X.size());

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    Iter RESULT = mX.insert(VALUES[tj]);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                    ++RESULT;
//                    ASSERTV(LENGTH, tj, CONFIG, ITER[tj + 1] == RESULT);
                }

                ASSERTV(LENGTH, CONFIG, 3 * LENGTH == X.size());
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

        typedef bsl::unordered_multiset<int> TestType;

        if (veryVerbose)
            printf("Default construct an unordered_mutliset, 'x'\n");

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

        const int MAX_SAMPLE = 10000;
//        const int MAX_SAMPLE = 7;  // minimum size to support all erase tests
        int *dataSamples = new int[MAX_SAMPLE];
        for(int i = 0; i != MAX_SAMPLE; ++i) {
            dataSamples[i] = i;
        }

        if (veryVerbose)  printf(
          "Range-construct an unordered_multiset, 'y', from the test array\n");

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

        testBuckets(mX);

        if (veryVerbose) printf("Try to fill 'x' with duplicate values\n");

        for (int i = 0; i != MAX_SAMPLE; ++i) {
            LOOP_ASSERT(i, 1 == x.count(dataSamples[i]));
            TestType::iterator it = mX.insert(dataSamples[i]);
            ASSERT(x.end() != it);
            ASSERT(*it == dataSamples[i]);
            LOOP_ASSERT(i, 2 == x.count(dataSamples[i]));
        }

        testBuckets(mX);
        validateIteration(mX);

        if (veryVerbose) printf(
          "Confirm the value of 'x' with the successfully inserted values.\n");

        testContainerHasData(x, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                "Create an unordered_multiset, 'z', that is a copy of 'x'.\n");

        TestType mZ = x;
        const TestType &z = mZ;

        if (veryVerbose)
            printf("Validate behavior of freshly constructed 'z'.\n");

        ASSERT(1.0f == z.max_load_factor());
        ASSERT(x == z);
        ASSERT(!(x != z));
        ASSERT(z == x);
        ASSERT(!(z != x));

        validateIteration(mZ);
        testContainerHasData(z, 2, dataSamples, MAX_SAMPLE);

        if (veryVerbose) printf(
                             "Expand 'z' with additional duplicate values.\n");

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

        if (veryVerbose) printf(
             "Call any remaining methods to be sure they at least compile.\n");

        mX.insert(1);

        const bsl::allocator<int> alloc   = x.get_allocator();
        const bsl::hash<int>      hasher  = x.hash_function();
        const bsl::equal_to<int>  compare = x.key_eq();
        
        const size_t maxSize    = x.max_size();
        const size_t buckets    = x.bucket_count();
        const float  loadFactor = x.load_factor();
        const float  maxLF      = x.max_load_factor();

        ASSERT(loadFactor < maxLF);

        mX.rehash(2 * buckets);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

        mX.reserve(0);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

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
