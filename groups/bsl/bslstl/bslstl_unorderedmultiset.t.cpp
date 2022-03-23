// bslstl_unorderedmultiset.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedmultiset.h>

#include <bslstl_iterator.h>
#include <bslstl_pair.h>

#include <bslalg_rangecompare.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_issame.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdalloctesttype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, 'bslstl_unorderedmultiset.t.cpp' (cases 1-11, plus the usage
// example), and 'bslstl_unorderedmultiset_test.cpp' (cases 12 and higher).
//
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
// Primary Manipulators:
//: o 'insert'  (via 'primaryManipulator' helper function)
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
// 'insert' and 'clear' methods to be used by the generator functions 'gg' and
// 'ggg'.  Note that some manipulators must support aliasing, and those that
// perform memory allocation must be tested for exception neutrality via the
// 'bslma_testallocator' component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.
//
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
// NEED A PROPER TEST TO EVALUATE unordered_multiset INVARIANTS:
//    equivalent keys form contiguous ranges
//    once inserted, relative order is preserved through all operations
//    contained values correspond to inserted values, counting duplicates
// ----------------------------------------------------------------------------
// [unord.set] construct/copy/destroy:
//*[ 2] unordered_multiset(size_type, hasher, key_equal, allocator);
//*[ 2] unordered_multiset(size_type, hasher, allocator);
//*[ 2] unordered_multiset(size_type, allocator);
// [12] unordered_multiset(ITER, ITER, allocator);
// [12] unordered_multiset(ITER, ITER, size_type, allocator);
// [12] unordered_multiset(ITER, ITER, size_type, hasher, allocator);
// [12] unordered_multiset(ITER, ITER, size_type, hasher, key_equal, alloc);
//*[29] unordered_multiset(unordered_multiset&& original);
//*[11] unordered_multiset(const A& allocator);
//*[ 7] unordered_multiset(const unordered_multiset& original);
//*[ 7] unordered_multiset(const unordered_multiset& original, const A& alloc);
// [29] unordered_multiset(unordered_multiset&& original, const A& allocator);
// [34] unordered_multiset(initializer_list, const A&);
// [34] unordered_multiset(initializer_list, size_type, const A&);
// [34] unordered_multiset(initializer_list, size_type, hasher, const A&);
// [34] unordered_multiset(initializer_list, size_type, hasher, pred,const A&);
// [34] unordered_set(initializer_list);
// [34] unordered_set(initializer_list, size_t);
// [34] unordered_set(initializer_list, size_t, HASH);
// [34] unordered_set(initializer_list, size_t, HASH, EQUAL);
// [34] unordered_set(initializer_list, const A&);
// [34] unordered_set(initializer_list, size_t, const A&);
// [34] unordered_set(initializer_list, size_t, HASH, const A&);
// [34] unordered_set(initializer_list, size_t, HASH, EQUAL, const A&);
//*[ 2] ~unordered_multiset();
// [11] unordered_multiset& operator=(const unordered_multiset& rhs);
// [29] unordered_multiset& operator=(unordered_multiset&& rhs);
// [34] unordered_multiset& operator=(initializer_list<value_type>);
//
// iterators:
// [14] iterator begin();
// [14] const_iterator begin() const;
// [14] iterator end();
// [14] const_iterator end() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
//
// capacity:
// [20] bool empty() const;
// [ 4] size_type size() const;
// [20] size_type max_size() const;
//
// modifiers:
// [ 2] void clear();
// [32] iterator emplace(Args&&... arguments);
//*[33] iterator emplace_hint(const_iterator hint, Args&&... args);
// [18] iterator erase(const_iterator position);
// [18] size_type erase(const key_type& key);
// [18] iterator erase(const_iterator first, const_iterator last);
// [15] iterator insert(const value_type& value);
// [30] iterator insert(value_type&& value);
// [ 2] iterator insert(const_iterator hint, const value_type& value);
//*[31] iterator insert(const_iterator hint, const value_type&& value);
// [34] iterator insert(initializer_list<value_type>);
// [17] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [ 8] void swap(set& other);
//
// observers:
// [21] hasher hash_function() const;
// [21] key_equal key_eq() const;
// [ 4] allocator_type get_allocator() const;
//
// unordered_multiset operations:
// [13] bool contains(const key_type& key);
// [13] bool contains(const LOOKUP_KEY& key);
// [13] iterator find(const key_type& key);
// [13] const_iterator find(const key_type& key) const;
// [13] size_type count(const key_type& key) const;
// [13] bsl::pair<iterator, iterator> equal_range(const key_type& key);
// [13] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// bucket interface:
// [ 4] size_type bucket_count() const;
// [ 4] size_type max_bucket_count() const;
// [ 4] size_type bucket_size(size_type n) const;
// [ 4] size_type bucket(const key_type& k) const;
//
// bucket iterators:
// [26] local_iterator begin(size_type n);
// [26] const_local_iterator begin(size_type n) const;
// [26] local_iterator end(size_type n);
// [26] const_local_iterator end(size_type n) const;
// [26] const_local_iterator cbegin(size_type n) const;
// [26] const_local_iterator cend(size_type n) const;
//
// hash policy:
// [ 4] float load_factor() const;
// [ 4] float max_load_factor() const;
// [26] void max_load_factor(float z);
// [26] void rehash(size_type n);
// [26] void reserve(size_type n);
//
// specialized algorithms:
// [ 6] bool operator==(unordered_multiset& lhs, unordered_multiset& rhs);
// [ 6] bool operator!=(unordered_multiset& lhs, unordered_multiset& rhs);
// [ 8] void swap(unordered_multiset& a, unordered_multiset& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] default construction (only)
// [36] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(unordered_multiset *object, const char *s, int verbose);
// [ 3] unordered_multiset& gg(unordered_multiset *object, const char *s);
//
// [22] CONCERN: The object is compatible with STL allocators.
// [23] CONCERN: The object has the necessary type traits
// [27] CONCERN: The values are spread into different buckets.
// [35] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [36] CLASS TEMPLATE DEDUCTION GUIDES

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

enum {
#if defined(BDE_BUILD_TARGET_EXC)
    PLAT_EXC = 1
#else
    PLAT_EXC = 0
#endif
};

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

typedef bsls::Types::UintPtr UintPtr;
typedef bsls::Types::Int64   Int64;

// Define DEFAULT DATA used in multiple test cases.

// Define DEFAULT DATA used in multiple test cases.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  -------------------  -------------------
    { L_,    0, "",                  ""                  },
    { L_,    1, "A",                 "A"                 },
    { L_,    2, "B",                 "B"                 },
    { L_,    3, "AA",                "AA"                },
    { L_,    4, "AB",                "AB"                },
    { L_,    4, "BA",                "AB"                },
    { L_,    5, "AC",                "AC"                },
    { L_,    6, "CD",                "CD"                },
    { L_,    7, "ABC",               "ABC"               },
    { L_,    7, "ACB",               "ABC"               },
    { L_,    7, "BAC",               "ABC"               },
    { L_,    7, "BCA",               "ABC"               },
    { L_,    7, "CAB",               "ABC"               },
    { L_,    7, "CBA",               "ABC"               },
    { L_,   25, "AAB",               "AAB"               },
    { L_,   25, "ABA",               "AAB"               },
    { L_,   25, "BAA",               "AAB"               },
    { L_,   26, "ABB",               "ABB"               },
    { L_,   26, "BBA",               "ABB"               },
    { L_,   26, "BAB",               "ABB"               },
    { L_,    8, "BAD",               "ABD"               },
    { L_,    9, "ABCA",              "AABC"              },
    { L_,   10, "ABCB",              "ABBC"              },
    { L_,   11, "ABCC",              "ABCC"              },
    { L_,   13, "ABCD",              "ABCD"              },
    { L_,   13, "ACBD",              "ABCD"              },
    { L_,   13, "BDCA",              "ABCD"              },
    { L_,   13, "DCBA",              "ABCD"              },
    { L_,   14, "BEAD",              "ABDE"              },
    { L_,   15, "BCDE",              "BCDE"              },
    { L_,   16, "ABCDE",             "ABCDE"             },
    { L_,   16, "ACBDE",             "ABCDE"             },
    { L_,   16, "CEBDA",             "ABCDE"             },
    { L_,   16, "EDCBA",             "ABCDE"             },
    { L_,   17, "FEDCB",             "BCDEF"             },
    { L_,   12, "ABCABC",            "AABBCC"            },
    { L_,   12, "AABBCC",            "AABBCC"            },
    { L_,   12, "CABBAC",            "AABBCC"            },
    { L_,   18, "FEDCBA",            "ABCDEF"            },
    { L_,   18, "DAFCBE",            "ABCDEF"            },
    { L_,   19, "ABCDEFG",           "ABCDEFG"           },
    { L_,   20, "ABCDEFGH",          "ABCDEFGH"          },
    { L_,   21, "ABCDEFGHI",         "ABCDEFGHI"         },
    { L_,   22, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP"  },
    { L_,   22, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP"  },
    { L_,   24, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   24, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};

enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

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
    TYPE tolerance = my_max(my_abs(x), my_abs(y)) * 0.0001f;
    return my_abs(x - y) <= tolerance;
}

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
    return v;        // for 'multiset' containers
}

size_t numCharInstances(const char *SPEC, const char c)
{
    size_t ret = 0;
    for (const char *pc = SPEC; *pc; ++pc) {
        ret += (c == *pc);
    }
    return ret;
}

template <class CONTAINER>
ptrdiff_t verifySpec(const CONTAINER& object, const char *spec)
{
    typedef typename CONTAINER::key_type       Key;
    typedef typename CONTAINER::const_iterator CIter;
    typedef bsltf::TestValuesArray<Key>        TestValues;

    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    const TestValues  VALUES;
    const char       *ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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
            count += (C == *pcB);
        }

        // We can't rely on the 'count' member function yet, so we have to do
        // it by hand.

        // We do not use the line below due to move-only types
        //const Key K = VALUES[C - 'A'];

        // Entries with equivalent values must be contiguos.
        CIter it = object.begin();
        while (object.end() != it && (VALUES[C - 'A'] != *it)) {
            ++it;
        }

        while (object.end() != it && (VALUES[C - 'A'] == *it)) {
            ++it;
            --count;
        }

        if (0 != count) {
            return pc - ALPHABET;                                     // RETURN
        }
    }

    return -1;    // it's a match
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator const_iterator;

    ASSERT(x.empty());
    ASSERT(0 == x.size());
    ASSERT(0.f == x.load_factor());

    ASSERT(x.begin()  == x.end());
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

    ASSERT(0       == x.count(37));
    ASSERT(x.end() == x.find(26));

    ASSERT(x == x);
    ASSERT(!(x != x));
}

template <class CONTAINER>
void testEmptyContainer(CONTAINER& x)
{
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
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    ASSERT(nCopies);
    ASSERT(X.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        const typename CONTAINER::value_type& testValue = data[i];
        TestIterator   it = X.find(keyForValue<CONTAINER>(testValue));
        ASSERT(X.end() !=  it);
        ASSERT(data[i] == *it);
        const SizeType countValues = X.count(
                                            keyForValue<CONTAINER>(testValue));
        ASSERTV(i, countValues, nCopies, countValues == nCopies);

        bsl::pair<TestIterator, TestIterator> range = X.equal_range(
                                            keyForValue<CONTAINER>(testValue));

        const SizeType rangeDist = bsl::distance(range.first, range.second);
        ASSERTV(countValues, rangeDist, countValues == rangeDist);

        ASSERT(range.first == it);
        for (SizeType iterations = nCopies; --iterations; ++it) {
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
void fillContainerWithData(CONTAINER&                            x,
                           const typename CONTAINER::value_type *data,
                           int                                   size)
{
    typedef typename CONTAINER::size_type SizeType;
    typedef typename CONTAINER::iterator  iterator;

    ASSERT(0 < size);
    const SizeType nCopies = x.count(data[0]) + 1;

    SizeType initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (int i = 0; i != size; ++i) {
        iterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(data[i] == *it);
        ASSERTV(i, nCopies, x.count(keyForValue<CONTAINER>(data[i])),
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
    ASSERTV(size, counter, size == counter);

    counter = 0;
    for (const_iterator it = c.cbegin(); it != c.cend(); ++it, ++counter) {}
    ASSERTV(size, counter, size == counter);

    const CONTAINER& cc = c;

    counter = 0;
    for (const_iterator it = cc.begin(); it != cc.end(); ++it, ++counter) {}
    ASSERTV(size, counter, size == counter);

    counter = 0;
    for (const_iterator it = cc.cbegin(); it != cc.cend(); ++it, ++counter) {}
    ASSERTV(size, counter, size == counter);
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
    typedef typename CONTAINER::size_type            SizeType;
    typedef typename CONTAINER::local_iterator       local_iterator;
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
        ASSERTV(count, bucketItems, count == bucketItems);

        bucketItems = 0;
        for (local_iterator iter = mX.begin(i); iter != mX.end(i); ++iter) {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERTV(count, bucketItems, count == bucketItems);

        bucketItems = 0;
        for (const_local_iterator iter = mX.cbegin(i);
             iter != mX.cend(i);
             ++iter)
        {
            ASSERT(i == x.bucket(keyForValue<CONTAINER>(*iter)));
            ++bucketItems;
        }
        ASSERTV(count, bucketItems, count == bucketItems);

    }
    ASSERTV(itemCount, x.size(), itemCount == x.size());
}


template <class CONTAINER>
void testErase(CONTAINER& mX)
{
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;

    const CONTAINER& x = mX;
    SizeType         size = x.size();

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
    SizeType                     duplicates = x.count(key);

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

// unordered_multiset-specific print function.
template <class KEY, class HASH, class EQUAL, class ALLOC>
void debugprint(const bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC> TObj;
        typedef typename TObj::const_iterator                    CIter;

        for (CIter it = s.begin(); it != s.end(); ++it) {
            bsls::BslTestUtil::callDebugprint(
           static_cast<char>(bsltf::TemplateTestFacility::getIdentifier(*it)));
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

namespace {

                            // ==========================
                            // class StatefulStlAllocator
                            // ==========================

template <class VALUE>
class StatefulStlAllocator : public bsltf::StdTestAllocator<VALUE>
    // This class implements a standard compliant allocator that has an
    // attribute, 'id'.
{
    // DATA
    int d_id;  // identifier

  private:
    // TYPES
    typedef bsltf::StdTestAllocator<VALUE> StlAlloc;
        // Alias for the base class.

  public:
    template <class BDE_OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some
        // 'BDE_OTHER_TYPE', provides a namespace for an 'other' type alias,
        // which is an allocator type following the same template as this one
        // but that allocates elements of 'BDE_OTHER_TYPE'.  Note that this
        // allocator type is convertible to and from 'other' for any
        // 'BDE_OTHER_TYPE' including 'void'.

        typedef StatefulStlAllocator<BDE_OTHER_TYPE> other;
    };

    // CREATORS
    StatefulStlAllocator()
        // Create a 'StatefulStlAllocator' object.
    : StlAlloc()
    {
    }

    //! StatefulStlAllocator(const StatefulStlAllocator& original) = default;
        // Create a 'StatefulStlAllocator' object having the same id as the
        // specified 'original'.

    template <class BDE_OTHER_TYPE>
    StatefulStlAllocator(const StatefulStlAllocator<BDE_OTHER_TYPE>& original)
        // Create a 'StatefulStlAllocator' object having the same id as the
        // specified 'original' with a different template type.
    : StlAlloc(original)
    , d_id(original.id())
    {
    }

    // MANIPULATORS
    void setId(int value)
        // Set the 'id' attribute of this object to the specified 'value'.
    {
        d_id = value;
    }

    // ACCESSORS
    int id() const
        // Return the value of the 'id' attribute of this object.
    {
        return d_id;
    }
};

                            // ======================
                            // class ExceptionProctor
                            // ======================

template <class OBJECT, class ALLOCATOR>
struct ExceptionProctor {
    // This class provides a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores a copy
    // of an object of the (template parameter) type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // created on construction.  This class requires that the copy constructor
    // and 'operator ==' be tested before use.

    // DATA
    int           d_line;      // line number at construction
    OBJECT        d_control;   // copy of the object being proctored
    const OBJECT *d_object_p;  // address of the original object

  private:
    // NOT IMPLEMENTED
    ExceptionProctor(const ExceptionProctor&);
    ExceptionProctor& operator=(const ExceptionProctor&);

  public:
    // CREATORS
    ExceptionProctor(const OBJECT     *object,
                     int               line,
                     const ALLOCATOR&  basicAllocator)
    : d_line(line)
    , d_control(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception proctor for the specified 'object' at the
        // specified 'line' number that uses the specified 'basicAllocator' to
        // supply memory for the control object.
    {
    }

    ExceptionProctor(const OBJECT              *object,
                     int                        line,
                     bslmf::MovableRef<OBJECT>  control)
    : d_line(line)
    , d_control(bslmf::MovableRefUtil::move(control))
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
    {
    }

    ~ExceptionProctor()
        // Destroy this exception proctor.  If the proctor was not released,
        // verify that the state of the object supplied at construction has not
        // changed.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_control, *d_object_p, d_control == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release this proctor from verifying the state of the object
        // supplied at construction.
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
    int         d_id;     // identifier for the functor
    mutable int d_count;  // number of times 'operator()' is called

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
    //!                                                                default;
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

        return bsltf::TemplateTestFacility::getIdentifier(lhs)
            == bsltf::TemplateTestFacility::getIdentifier(rhs);
    }

    bool operator==(const TestEqualityComparator& rhs) const
        // Return 'true' if object's 'id' is equal to the 'id' of the specified
        // 'rhs' and 'false' otherwise.
    {
        return (id() == rhs.id());  // && d_compareLess == rhs.d_compareLess);
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
    TestNonConstEqualityComparator()
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    {
    }

    //! TestNonConstEqualityComparator(const TestEqualityComparator& original)
    //!                                                              = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs)
        // Return 'true' if the integer representation of the specified 'lhs'
        // is less than integer representation of the specified 'rhs'.
    {
        return bsltf::TemplateTestFacility::getIdentifier(lhs)
            == bsltf::TemplateTestFacility::getIdentifier(rhs);
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
    int         d_id;            // identifier for the functor
    mutable int d_count;         // number of times 'operator()' is called
    bool        d_shortCircuit;  // always returns 0 if true

  public:
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
        ++d_count;

        return d_shortCircuit
               ? 0
               : bsltf::TemplateTestFacility::getIdentifier(obj);
    }

    bool operator== (const TestHashFunctor& rhs) const
        // Return 'true' if object's 'id' is equal to the 'id' of the specified
        // 'rhs' and 'false' otherwise.
    {
        return (id() == rhs.id());  // && d_compareLess == rhs.d_compareLess);
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
        return bsltf::TemplateTestFacility::getIdentifier(obj);
    }

    bool operator==(const TestNonConstHashFunctor&)
    {
        return true;
    }
};

                            // ====================
                            // class CompareProctor
                            // ====================

template <class OBJECT>
struct CompareProctor {
    // Take pointers to two non-owned objects of the same type, and ensure that
    // they compare equal upon destruction.

    const OBJECT *d_a_p;
    const OBJECT *d_b_p;

    // CREATOR
    CompareProctor(const OBJECT& a, const OBJECT& b)
    : d_a_p(&a)
    , d_b_p(&b)
    {}

    ~CompareProctor()
    {
        ASSERTV(!d_a_p == !d_b_p);

        if (d_a_p) {
            ASSERTV(*d_a_p, *d_b_p, *d_a_p == *d_b_p);
        }
    }

    // MANIPULATORS
    void release()
    {
        d_a_p = d_b_p = 0;
    }
};

}  // close unnamed namespace

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

template <class ITER, class VALUE_TYPE>
class TestMovableTypeUtil
{
  public:
    static ITER findFirstNotMovedInto(ITER, ITER end)
    {
        return end;
    }
};

template <class ITER>
class TestMovableTypeUtil<ITER, bsltf::MovableAllocTestType>
{
  public:
    static ITER findFirstNotMovedInto(ITER begin, ITER end)
    {
        for (; begin != end; ++begin) {
            if (!begin->movedInto())
            {
                break;
            }
        }
        return begin;
    }
};

class TestAllocatorUtil
{
  public:
    template <class TYPE>
    static void test(const TYPE&, const bslma::Allocator&)
    {
    }

    static void test(const bsltf::AllocEmplacableTestType& value,
                     const bslma::Allocator&               oa)
    {
        ASSERTV(&oa == value.arg01().allocator());
        ASSERTV(&oa == value.arg02().allocator());
        ASSERTV(&oa == value.arg03().allocator());
        ASSERTV(&oa == value.arg04().allocator());
        ASSERTV(&oa == value.arg05().allocator());
        ASSERTV(&oa == value.arg06().allocator());
        ASSERTV(&oa == value.arg07().allocator());
        ASSERTV(&oa == value.arg08().allocator());
        ASSERTV(&oa == value.arg09().allocator());
        ASSERTV(&oa == value.arg10().allocator());
    }
};

template <class KEY,
          class HASH  = TestHashFunctor<KEY>,
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

    typedef typename Obj::iterator             Iter;
    typedef typename Obj::const_iterator       CIter;
    typedef typename Obj::size_type            SizeType;
    typedef typename Obj::value_type           ValueType;
        // Shorthands

    typedef bsltf::TestValuesArray<KEY, ALLOC> TestValues;
    typedef bsltf::TemplateTestFacility        TstFacility;
    typedef bslma::ConstructionUtil            ConsUtil;
    typedef bslmf::MovableRefUtil              MoveUtil;
    typedef bsltf::MoveState                   MoveState;
    typedef TestMovableTypeUtil<Iter, KEY>     TstMoveUtil;

    typedef typename ALLOC::template rebind<KEY>::other KeyAllocator;

    enum { k_TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value };

  public:
    typedef bsltf::StdTestAllocator<KEY>       StlAlloc;

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

    static void storeFirstNElemAddr(typename Obj::const_pointer *pointers,
                                    const Obj&                   object,
                                    size_t                       n)
    {
        size_t i = 0;
        for (CIter b = object.cbegin(); b != object.cend() && i < n; ++b)
        {
            pointers[i++] = bsls::Util::addressOf(*b);
        }
    }
    static
    int checkFirstNElemAddr(typename Obj::const_pointer *pointers,
                            const Obj&                   object,
                            size_t                       n)
    {
        int    count = 0;
        size_t i = 0;
        for (CIter b = object.cbegin(); b != object.end() && i < n; ++b)
        {
            if (pointers[i++] != bsls::Util::addressOf(*b)) {
                ++count;
            }
        }
        return count;
    }

    static Iter primaryManipulator(Obj   *container,
                                   int    identifier,
                                   ALLOC  allocator)
        // Insert into the specified 'container' the value object indicated by
        // the specified 'identifier', ensuring that the overload of the
        // primary manipulator taking a modifiable rvalue is invoked (rather
        // than the one taking an lvalue).  Return the result of invoking the
        // primary manipulator.
    {
        bsls::ObjectBuffer<ValueType> buffer;
        TstFacility::emplace(buffer.address(), identifier, allocator);
        bslma::DestructorGuard<ValueType> guard(buffer.address());

        return container->insert(MoveUtil::move(buffer.object()));
    }

    static int getIndexForIter(const Obj& obj, Iter it);
        // Find the index corresponding to the specified iterator 'it', which
        // must be a valid iterator referring to the specified 'obj'.  Note
        // that 'obj.end() == it' is allowed.

    static Iter getIterForIndex(const Obj& obj, size_t idx);
        // Return the iterator relating to the specified 'obj' with specified
        // index 'idx'.  It is an error if 'idx >= obj.size()'.

  public:
    // TEST CASES

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9();
        // Test assignment operator ('operator=').

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
              bool OTHER_FLAGS>
    static void testCase8_propagate_on_container_swap_dispatch();
    static void testCase8_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase8();
        // Test 'swap' member.

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase7_select_on_container_copy_construction_dispatch();
    static void testCase7_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

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
        // Test primary manipulators.

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
                                             bsltf::StdTestAllocator<KEY> >
{
};

template <class KEY>
class StdBslmaTestDriver : public TestDriver<
                             KEY,
                             TestHashFunctor<KEY>,
                             TestEqualityComparator<KEY>,
                             bsltf::StdAllocatorAdaptor<bsl::allocator<KEY> > >
{
};
                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, HASH, EQUAL, ALLOC>::ggg(Obj        *object,
                                             const char *spec,
                                             int         verbose)
{
    enum { SUCCESS = -1 };

    bslma::TestAllocator scratch;
    KeyAllocator         xscratch(&scratch);

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            primaryManipulator(object, spec[i], xscratch);
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
typename bsl::unordered_multiset<KEY, HASH, EQUAL, ALLOC>::iterator
TestDriver<KEY, HASH, EQUAL, ALLOC>::getIterForIndex(const Obj& obj,
                                                     size_t     idx)
{
    if (idx > obj.size()) {
        ASSERTV(idx <= obj.size());

        return obj.end();                                             // RETURN
    }

    Iter   ret = obj.begin();
    size_t i = 0;
    for (; i < idx && obj.end() != ret; ++i) {
        ++ret;
    }

    ASSERTV(idx == i);

    return ret;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>                 StdAlloc;

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Create control and source objects.
    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator oas("source", veryVeryVeryVerbose);
        bslma::TestAllocator oat("target", veryVeryVeryVerbose);

        StdAlloc mas(&oas);
        StdAlloc mat(&oat);
        StdAlloc scratch(&da);

        const Obj W(IVALUES.begin(),
                    IVALUES.end(),
                    1,
                    HASH(),
                    EQUAL(),
                    scratch);    // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj        mY(IVALUES.begin(),
                              IVALUES.end(),
                              1,
                              HASH(),
                              EQUAL(),
                              mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ T_ P_(ISPEC) P_(Y) P(W) }

                Obj        mX(JVALUES.begin(),
                              JVALUES.end(),
                              1,
                              HASH(),
                              EQUAL(),
                              mat);
                const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC,  W,   Y,  W == Y);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());

                if (PROPAGATE) {
                    ASSERTV(ISPEC, JSPEC, 0 == oat.numBlocksInUse());
                }
                else {
                    ASSERTV(ISPEC, JSPEC, oasm.isInUseSame());
                }
            }
            ASSERTV(ISPEC, 0 == oas.numBlocksInUse());
            ASSERTV(ISPEC, 0 == oat.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                             testCase9_propagate_on_container_copy_assignment()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_copy_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_copy_assignment' trait is 'true', the
    //:   allocator used by the target object is updated to be a copy of that
    //:   used by the source object (i.e., the source object's allocator *is*
    //:   propagated).
    //:
    //: 3 The allocator used by the source object remains unchanged whether or
    //;   not it is propagated to the target object.
    //:
    //: 4 If the allocator is propagated from the source object to the target
    //:   object, all memory allocated from the target object's original
    //:   allocator is released.
    //:
    //: 5 The effect of the 'propagate_on_container_copy_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_copy_assignment' property configured to
    //:   'false'.  In two successive iterations of P-3, first configure the
    //:   three properties not under test to be 'false', then configure them
    //:   all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize an object 'X' from 'x' using one of the allocators from
    //:     P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'W' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Copy-assign 'Y' to 'X' and use 'operator==' to verify that both
    //:     'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocator of 'Y'
    //:     is *not* propagated to 'X' and that the allocator used by 'Y'
    //:     remains unchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'Y' *is*
    //:   propagated to 'X'.  Also verify that all memory is released to the
    //:   allocator that was in use by 'X' prior to the assignment.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_copy_assignment
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == false'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
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
    //   unordered_multiset& operator=(const unordered_multiset& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\tTesting signature.\n");
    {
        typedef Obj& (Obj::*MP)(const Obj&);
        MP mp = &Obj::operator=;
        (void) mp;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\tTesting behavior.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(SPEC1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mZ(xscratch);  const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(xscratch); const Obj& ZZ = gg(&mZZ, SPEC1);

            mZ.max_load_factor(2.0f);

            if (veryVerbose) { T_ T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2  = DATA[tj].d_line;
                const int         INDEX2 = DATA[tj].d_index;
                const char *const SPEC2  = DATA[tj].d_spec;

                const bool EXP = INDEX1 == INDEX2;  // expected result

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                {
                    Obj mX( xoa);  const Obj& X  = gg(&mX,  SPEC2);
                    Obj mXX(xoa);  const Obj& XX = gg(&mXX, SPEC2);

                    mX.max_load_factor(3.0f);

                    if (veryVerbose) { T_ T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, EXP == (Z == X));

                    bslma::TestAllocatorMonitor oam(&oa);
                    bslma::TestAllocatorMonitor sam(&scratch);

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        ASSERT(XX == X);

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV((!PLAT_EXC || 0 == LENGTH1) || numPasses > 1);

                    ASSERTV(2.0f == X.max_load_factor());

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

                    ASSERTV(LINE1, LINE2, da.numBlocksTotal(),
                            0 == da.numBlocksTotal());
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
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                               testCase8_propagate_on_container_swap_dispatch()
{

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<KEY,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS>            StdAlloc;

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const ISPEC   = SPECS[ti];

        TestValues IVALUES(ISPEC);

        bslma::TestAllocator xoa("x-original", veryVeryVeryVerbose);
        bslma::TestAllocator yoa("y-original", veryVeryVeryVerbose);

        // if 'false == PROPAGATE', the allocators must compare equal

        StdAlloc xma(&xoa);
        StdAlloc yma(PROPAGATE ? &yoa : &xoa);

        StdAlloc scratch(&da);

        const Obj ZZ(IVALUES.begin(),
                     IVALUES.end(),
                     1,
                     HASH(),
                     EQUAL(),
                     scratch);        // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const Obj WW(JVALUES.begin(),
                         JVALUES.end(),
                         1,
                         HASH(),
                         EQUAL(),
                         scratch);    // control

            {
                IVALUES.resetIterators();

                Obj        mX(IVALUES.begin(),
                              IVALUES.end(),
                              1,
                              HASH(),
                              EQUAL(),
                              xma);
                const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(ISPEC) P_(X) P(ZZ) }

                JVALUES.resetIterators();

                Obj         mY(JVALUES.begin(),
                               JVALUES.end(),
                               1,
                               HASH(),
                               EQUAL(),
                               yma);
                const Obj& Y = mY;

                ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    mX.swap(mY);

                    ASSERTV(ISPEC, JSPEC, WW, X, WW == X);
                    ASSERTV(ISPEC, JSPEC, ZZ, Y, ZZ == Y);

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, yma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, xma == Y.get_allocator());

                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                    else {
                        ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
                    }
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    swap(mX, mY);

                    ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                    ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                    ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());

                    if (PROPAGATE) {
                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
                }
            }
            ASSERTV(ISPEC, 0 == xoa.numBlocksInUse());
            ASSERTV(ISPEC, 0 == yoa.numBlocksInUse());
        }
    }
    ASSERTV(0 == da.numBlocksInUse());
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                                        testCase8_propagate_on_container_swap()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_swap' trait is 'false', the
    //:   allocators used by the source and target objects remain unchanged
    //:   (i.e., the allocators are *not* exchanged).
    //:
    //: 2 If the 'propagate_on_container_swap' trait is 'true', the
    //:   allocator used by the target (source) object is updated to be a copy
    //:   of that used by the source (target) object (i.e., the allocators
    //:   *are* exchanged).
    //:
    //: 3 If the allocators are propagated (i.e., exchanged), there is no
    //:   additional allocation from any allocator.
    //:
    //: 4 The effect of the 'propagate_on_container_swap' trait is independent
    //:   of the other three allocator propagation traits.
    //:
    //: 5 Following the swap operation, neither object holds on to memory
    //:   allocated from the other object's allocator.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_swap' property configured to 'false'.  In two
    //:   successive iterations of P-3, first configure the three properties
    //:   not under test to be 'false', then configure them all to be 'true'.
    //:
    //: 3 For each value '(x, y)' in the cross product S x S:  (C-1)
    //:
    //:   1 Initialize two objects from 'x', a control object 'ZZ' using a
    //:     scratch allocator and an object 'X' using one of the allocators
    //:     from P-2.
    //:
    //:   2 Initialize two objects from 'y', a control object 'WW' using a
    //:     scratch allocator and an object 'Y' using the other allocator from
    //:     P-2.
    //:
    //:   3 Using both member 'swap' and free function 'swap', swap 'X' with
    //:     'Y' and use 'operator==' to verify that 'X' and 'Y' have the
    //:     expected values.
    //:
    //:   4 Use the 'get_allocator' method to verify that the allocators of 'X'
    //:     and 'Y' are *not* exchanged.  (C-1)
    //:
    //: 4 Repeat P-2..3 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocators of 'X' and 'Y'
    //:   *are* exchanged.  Also verify that there is no additional allocation
    //:   from any allocator.  (C-2..5)
    //
    // Testing:
    //   propagate_on_container_swap
    // ------------------------------------------------------------------------


    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\t'propagate_on_container_swap::value == false'\n");

    testCase8_propagate_on_container_swap_dispatch<false, false>();
    testCase8_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\t'propagate_on_container_swap::value == true'\n");

    testCase8_propagate_on_container_swap_dispatch<true, false>();
    testCase8_propagate_on_container_swap_dispatch<true, true>();
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
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is not empty,
    //:         and temporary memory were allocated from 'oaz' if 'mX' is not
    //:         empty.  (C-5)
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
    //:       AllocatorTraits supports it) and use the member and free 'swap'
    //:       functions to swap the values 'mX' and 'mZ' respectively; verify,
    //:       after each swap, that: (C-1, 6)
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
    //:      AllocatorTraits supports it) and use the member and free 'swap'
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
    //   void swap(unordered_multiset& other);
    //   void swap(unordered_multiset& a, uuordered_multiset& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\tTesting signatures.\n");
    {
        typedef void (Obj::*FuncPtr)(Obj&);
        typedef void (*FreeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        FuncPtr     memberSwap = &Obj::swap;
        FreeFuncPtr freeSwap   = bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (verbose) printf("\tTesting behavior.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    // Use a table of distinct object values and expected memory usage.

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator oa     ("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        KeyAllocator         xoa(&oa);
        KeyAllocator         xscratch(&scratch);

        Obj        mW(xoa);
        const Obj& W = gg(&mW,  SPEC1);
        const Obj  XX(W, xscratch);

        if (veryVerbose) { T_ T_ P_(LINE1) P_(W) P(XX) }

        // Ensure the first row of the table contains the
        // default-constructed value.

        if (0 == ti) {
            ASSERTV(LINE1, Obj(), W, Obj() == W);
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
            const int         LINE2 = DATA[tj].d_line;
            const char *const SPEC2 = DATA[tj].d_spec;

            Obj mX(XX, xoa); const Obj& X = mX;

            Obj        mY(xoa);
            const Obj& Y = gg(&mY, SPEC2);
            const Obj  YY(Y, xscratch);

            mX.max_load_factor(2.0f);
            mY.max_load_factor(3.0f);

            ASSERT(2.0f == X.max_load_factor());
            ASSERT(3.0f == Y.max_load_factor());

            if (veryVerbose) { T_ T_ P_(LINE2) P_(X) P_(Y) P(YY) }

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

#if 0       // Unlike 'multiset', 'unordered multiset' does not support
            // swapping with unequal bslma allocators.

            bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

            Obj mZ(&oaz);  const Obj& Z = gg(&mZ, SPEC2);
            const Obj ZZ(Z, &scratch);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                EXCEPTION_TEST_BEGIN(mX) {
                    ExceptionProctor<Obj> proctorX(&X, L_, &scratch);
                    ExceptionProctor<Obj> proctorZ(&Z, L_, &scratch);

                    mX.swap(mZ);

                    proctorX.release();
                    proctorZ.release();
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
                    ExceptionProctor<Obj> proctorX(&X, L_, &scratch);
                    ExceptionProctor<Obj> proctorZ(&Z, L_, &scratch);

                    swap(mX, mZ);

                    proctorX.release();
                    proctorZ.release();
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

    // Invoke free 'swap' function in a context where ADL is used.
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator oa     ("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        KeyAllocator         xoa(&oa);
        KeyAllocator         xscratch(&scratch);

        Obj        mX(xoa);
        const Obj& X = mX;
        const Obj  XX(X, xscratch);

        Obj        mY(xoa);
        const Obj& Y = gg(&mY, "ABC");
        const Obj  YY(Y, xscratch);

        mX.max_load_factor(2.0f);
        mY.max_load_factor(3.0f);

        if (veryVerbose) { T_ T_ P_(X) P(Y) }

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

        if (veryVerbose) { T_ T_ P_(X) P(Y) }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                     testCase7_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    KEY,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS>                StdAlloc;

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    for (int ti = 0; ti < NUM_SPECS; ++ti) {
        const char *const SPEC   = SPECS[ti];
        const size_t      LENGTH = strlen(SPEC);

        TestValues VALUES(SPEC);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        StdAlloc ma(&oa);

        {
            const Obj W(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), ma);
                                                                     // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\t\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), ma);
            const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor oam(&oa);

            const Obj Y(X);

            ASSERTV(SPEC, W == Y);
            ASSERTV(SPEC, W == X);
            ASSERTV(SPEC, PROPAGATE, PROPAGATE == (ma == Y.get_allocator()));
            ASSERTV(SPEC, PROPAGATE,               ma == X.get_allocator());

            if (PROPAGATE) {
                ASSERTV(SPEC, 0 != TYPE_ALLOC || dam.isInUseSame());
                ASSERTV(SPEC, 0 ==     LENGTH || oam.isInUseUp());
            }
            else {
                ASSERTV(SPEC, 0 ==     LENGTH || dam.isInUseUp());
                ASSERTV(SPEC, oam.isTotalSame());
            }
        }
        ASSERTV(SPEC, 0 == da.numBlocksInUse());
        ASSERTV(SPEC, 0 == oa.numBlocksInUse());
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                              testCase7_select_on_container_copy_construction()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 The allocator of a source object using a standard allocator is
    //:   propagated to the newly constructed object according to the
    //:   'select_on_container_copy_construction' method of the allocator.
    //:
    //: 2 In the absence of a 'select_on_container_copy_construction' method,
    //:   the allocator of a source object using a standard allocator is always
    //:   propagated to the newly constructed object (C++03 semantics).
    //:
    //: 3 The effect of the 'select_on_container_copy_construction' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create a 'bsltf::StdStatefulAllocator' with its
    //:   'select_on_container_copy_construction' property configured to
    //:   'false'.  In two successive iterations of P-3..5, first configure the
    //:   three properties not under test to be 'false', then confgiure them
    //:   all to be 'true'.
    //:
    //: 3 For each value in S, initialize objects 'W' (a control) and 'X' using
    //:   the allocator from P-2.
    //:
    //: 4 Copy construct 'Y' from 'X' and use 'operator==' to verify that both
    //:   'X' and 'Y' subsequently have the same value as 'W'.
    //:
    //: 5 Use the 'get_allocator' method to verify that the allocator of 'X'
    //:   is *not* propagated to 'Y'.
    //:
    //: 6 Repeat P-2..5 except that this time configure the allocator property
    //:   under test to 'true' and verify that the allocator of 'X' *is*
    //:   propagated to 'Y'.  (C-1)
    //:
    //: 7 Repeat P-2..5 except that this time use a 'StatefulStlAllocator',
    //:   which does not define a 'select_on_container_copy_construction'
    //:   method, and verify that the allocator of 'X' is *always* propagated
    //:   to 'Y'.  (C-2..3)
    //
    // Testing:
    //   select_on_container_copy_construction
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\t'select_on_container_copy_construction' "
                        "propagates *default* allocator.\n");

    testCase7_select_on_container_copy_construction_dispatch<false, false>();
    testCase7_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\t'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    testCase7_select_on_container_copy_construction_dispatch<true, false>();
    testCase7_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\tVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<KEY>                            Allocator;
    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, Allocator> Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
        };

        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);
            TestValues        VALUES(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;
            a.setId(ALLOC_ID);

            const Obj W(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), a);
                                                                     // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\t\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), a);
            const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC,        W == Y);
            ASSERTV(SPEC,        W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR
    //
    // Concerns:
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
    //   unordered_multiset(const unordered_multiset& original);
    //   unordered_multiset(const unordered_multiset& original, const A& a);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

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

        enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

        const TestValues VALUES;

        bslma::TestAllocator oa(veryVeryVerbose);
        KeyAllocator         xoa(&oa);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object 'W'.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\\ttControl Obj: "); P(W); }

            Obj mX(xoa); const Obj& X = gg(&mX, SPEC);

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
                ASSERTV(RESULT != Y1.end());

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

                const bsls::Types::Int64 A = oa.numBlocksInUse();

                Obj Y11(X, xoa);

                ASSERT(0 == LENGTH || oa.numBlocksTotal() > A);

                // Due of pooling of memory alloctioon, we can't predict
                // whether this insert will allocate or not.

                Iter RESULT = Y11.insert(VALUES['Z' - 'A']);
                ASSERTV(RESULT != Y11.end());

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

                    const Obj Y2(X, xoa);
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
    // TESTING EQUALITY OPERATORS
    //
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
    //   bool operator==(const unordered_multiset& lhs,
    //                   const unordered_multiset& rhs);
    //   bool operator!=(const unordered_multiset& lhs,
    //                   const unordered_multiset& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\tTesting signatures.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = bsl::operator==;
        (void) operatorEq;  // quash potential compiler warnings

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        (void) [](const Obj& lhs, const Obj& rhs) -> bool {
            return lhs != rhs;
        };
#else
        operatorPtr operatorNe = bsl::operator!=;
        (void) operatorNe;
#endif
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\tCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_results);

            if (veryVerbose) {
                T_ T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1)
            }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                Obj mX(xscratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results);

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2)
                }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;
                    KeyAllocator          xxa(&xa);
                    KeyAllocator          xya(&ya);

                    Obj mX(xxa); const Obj& X = gg(&mX, SPEC1);
                    Obj mY(xya); const Obj& Y = gg(&mY, SPEC2);

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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\tTesting signatures\n");
    {
        typedef ALLOC (Obj::*MP)() const;
        MP mp = &Obj::get_allocator;
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

    if (verbose) printf("\tTesting basic accessors\n");

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        const char *d_results;  // expected results
    } DATA[] = {
        //line  spec      result
        //----  -------   -------
        { L_,   "",       ""      },
        { L_,   "A",      "A"     },
        { L_,   "B",      "B"     },
        { L_,   "AA",     "AA"    },
        { L_,   "AB",     "AB"    },
        { L_,   "BA",     "AB"    },
        { L_,   "CD",     "CD"    },
        { L_,   "AAA",    "AAA"   },
        { L_,   "AAB",    "AAB"   },
        { L_,   "ABA",    "AAB"   },
        { L_,   "ABB",    "ABB"   },
        { L_,   "BAA",    "AAB"   },
        { L_,   "BAB",    "ABB"   },
        { L_,   "BBA",    "ABB"   },
        { L_,   "ABC",    "ABC"   },
        { L_,   "ABCD",   "ABCD"  },
        { L_,   "DCBA",   "ABCD"  },
        { L_,   "ABCDE",  "ABCDE" },
        { L_,   "HIJLK",  "HIJKL" },
        { L_,   "HIJJJ",  "JJJHI" },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const HASH  defaultHasher     = HASH();
    const EQUAL defaultComparator = EQUAL();

    if (verbose) { printf(
                "\tCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const EXP_SPEC = DATA[ti].d_results;
            const size_t      LENGTH   = strlen(EXP_SPEC);
            const TestValues  EXP(EXP_SPEC);

            if (veryVerbose) { T_ T_ P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",    veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint",  veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2", veryVeryVeryVerbose);

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
                  } return;                                           // RETURN
                }

                Obj&                  mX = *objPtr;
                const Obj&            X = gg(&mX, SPEC);
                bslma::TestAllocator& oa  = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                          ? da
                                          : sa1;

                // Verify basic accessors

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());

                ASSERTV(-1 == verifySpec(X, EXP_SPEC));

                ASSERTV(oam.isTotalSame());

                ASSERTV(X.begin()  == X.cbegin());
                ASSERTV(X.end()    == X.cend());
                ASSERTV((LENGTH == 0) == (X.begin()  == X.end()));
                ASSERTV((LENGTH == 0) == (X.cbegin() == X.cend()));

                ASSERTV(LENGTH != 0 || 1 == X.bucket_count());

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
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'
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
    //   unordered_multiset& gg(unordered_multiset *object, const char *);
    //   int ggg(unordered_multiset *object, const char *spec);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    bslma::TestAllocator oa(veryVeryVerbose);
    KeyAllocator         xoa(&oa);

    if (verbose) printf("\tTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            const char *d_results;  // expected element values
        } DATA[] = {
            //line  spec      results
            //----  --------  -------
            { L_,   "",       ""      },
            { L_,   "A",      "A"     },
            { L_,   "B",      "B"     },
            { L_,   "AA",     "AA"    },
            { L_,   "AB",     "AB"    },
            { L_,   "BA",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "AAA",    "AAA"   },
            { L_,   "AAB",    "AAB"   },
            { L_,   "ABA",    "AAB"   },
            { L_,   "ABB",    "ABB"   },
            { L_,   "BAA",    "AAB"   },
            { L_,   "BAB",    "ABB"   },
            { L_,   "BBA",    "ABB"   },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "DCBA",   "ABCD"  },
            { L_,   "ABCDE",  "ABCDE" },
            { L_,   "HIJLK",  "HIJKL" },
            { L_,   "HIJJJ",  "JJJHI" },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        size_t oldLen = 0;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE     = DATA[ti].d_line;
            const char *const SPEC     = DATA[ti].d_spec;
            const char *const EXP_SPEC = DATA[ti].d_results;
            const size_t      LENGTH   = strlen(EXP_SPEC);
            const TestValues  EXP(EXP_SPEC);
            const size_t      curLen   = strlen(SPEC);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            Obj        mY(xoa);
            const Obj& Y = gg(&mY, SPEC);   // extended spec

            if (curLen != oldLen) {
                if (veryVerbose) printf("\t\tof length " ZU ":\n", curLen);
                ASSERTV(LINE, oldLen, curLen, oldLen <= curLen);
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                T_ T_ T_ P(X);
                T_ T_ T_ P(Y);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());

            ptrdiff_t  retX = verifySpec(X, EXP_SPEC);
            ASSERTV(retX, -1 == retX);

            ptrdiff_t  retY = verifySpec(Y, EXP_SPEC);
            ASSERTV(retY, -1 == retY);
        }
    }

    if (verbose) printf("\tTesting 'ggg' generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;   // source line number
            const char *d_spec;   // specification string
            int         d_index;  // offending character index
        } DATA[] = {
            //line  spec     index
            //----  -------  -----
            { L_,   "",      -1   },  // control

            { L_,   "A",     -1   },  // control
            { L_,   " ",      0   },
            { L_,   ".",      0   },
            { L_,   "E",     -1   },  // control
            { L_,   "a",      0   },
            { L_,   "z",      0   },

            { L_,   "AE",    -1   },  // control
            { L_,   "aE",     0   },
            { L_,   "Ae",     1   },
            { L_,   ".~",     0   },
            { L_,   "~!",     0   },
            { L_,   "  ",     0   },

            { L_,   "ABC",   -1   },  // control
            { L_,   " BC",    0   },
            { L_,   "A C",    1   },
            { L_,   "AB ",    2   },
            { L_,   "?#:",    0   },
            { L_,   "   ",    0   },

            { L_,   "ABCDE", -1   },  // control
            { L_,   "aBCDE",  0   },
            { L_,   "ABcDE",  2   },
            { L_,   "ABCDe",  4   },
            { L_,   "AbCdE",  1   }
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(xoa);    const Obj& X = mX;

            if (LENGTH != oldLen) {
                if (veryVerbose) printf("\t\tof length %d:\n", LENGTH);
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
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
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
    //   unordered_multiset(const allocator_type&);  // bslma::Allocator* only
    //   ~unordered_multiset();
    //   iterator insert(value_type&& value);
    //   void clear();
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const TestValues VALUES;          // contains 52 distinct increasing values
    const size_t     MAX_LENGTH = 9;

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
            printf("\tTesting with various allocator configurations.\n");
        }
        bool done = false;
        for (char cfg = 'a'; cfg <= 't'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            KeyAllocator         xsa(&sa);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            Obj                   *objPtr = 0;
            bslma::TestAllocator&  oa  = strchr("cgikprt", CONFIG) ? sa : da;
            bslma::TestAllocator&  noa = &sa == &oa ? da : sa;

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                  } break;
                  case 'b': {
                      // Ambiguous -- does '0' mean 'initialNumBuckets' or
                      // 'allocator'? -- Result is same either way.
                      // Note: resolves to 'initialNumBuckets' as it does not
                      // require any conversion.
                      objPtr = new (fa) Obj(0);
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(xsa);
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
                                            ALLOC(0));
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            defaultComparator,
                                            xsa);
                  } break;
                  case 'h': {
                      objPtr = new (fa) Obj(0,
                                            ALLOC(0));
                  } break;
                  case 'i': {
                      objPtr = new (fa) Obj(0,
                                            xsa);
                  } break;
                  case 'j': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            ALLOC(0));
                  } break;
                  case 'k': {
                      objPtr = new (fa) Obj(0,
                                            defaultHasher,
                                            xsa);
                  } break;
                  case 'l': {
                      objPtr = new (fa) Obj(100);
                  } break;
                  case 'm': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher);
                  } break;
                  case 'n': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator);
                  } break;
                  case 'o': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator,
                                            ALLOC(0));
                  } break;
                  case 'p': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            defaultComparator,
                                            xsa);
                  } break;
                  case 'q': {
                      objPtr = new (fa) Obj(100,
                                            ALLOC(0));
                  } break;
                  case 'r': {
                      objPtr = new (fa) Obj(100,
                                            xsa);
                  } break;
                  case 's': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            ALLOC(0));
                  } break;
                  case 't': {
                      objPtr = new (fa) Obj(100,
                                            defaultHasher,
                                            xsa);
                      done = true;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || CONFIG <= 'k') == (1 == numPasses));

            Obj& mX = *objPtr; const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            if (CONFIG <= 'k') {
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
                        100 <= X.bucket_count());
            }

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            ASSERTV((strchr("abchilqr",       CONFIG) ? HASH()
                                              : HASH(7)) == X.hash_function());
            ASSERTV((strchr("abcdhijklmqrst", CONFIG) ? EQUAL()
                                              : EQUAL(9)) == X.key_eq());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\t\tTesting 'insert' (bootstrap).\n"); }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            if (0 < LENGTH) {
                if (veryVerbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }
                    int  id = TstFacility::getIdentifier(VALUES[tj]);
                    Iter RESULT = primaryManipulator(&mX, id, xscratch);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
                }

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                if (veryVerbose) {
                    printf("\t\t Testing allocator exceptions\n");
                }

                if (&oa == &da) {
                    int id = TstFacility::getIdentifier(VALUES[LENGTH - 1]);
                    primaryManipulator(&mX, id, xscratch);
                }
                else {
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(xscratch); const Obj& Z = mZ;

                        for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                            int id = TstFacility::getIdentifier(VALUES[tj]);
                            primaryManipulator(&mZ, id, xscratch);
                        }
                        ASSERTV(Z, X, Z == X);

                        ExceptionProctor<Obj, ALLOC> proctor(
                                                           &X,
                                                           L_,
                                                           MoveUtil::move(mZ));

                        if (veryVeryVeryVerbose) {
                            printf("\t\t\t\t Inserting: ");
                            P(VALUES[LENGTH - 1]);
                        }

                        int  id = TstFacility::getIdentifier(
                                                            VALUES[LENGTH -1]);
                        Iter RESULT = primaryManipulator(&mX, id, xscratch);

                        ASSERTV(LENGTH, CONFIG, VALUES[LENGTH - 1] == *RESULT);

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                ASSERTV(LENGTH, CONFIG, LENGTH, X.size(), LENGTH == X.size());

                {
                    bool *foundValues = new bool[X.size()];
                    for (size_t j = 0; j != X.size(); ++j) {
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
                    int  id = TstFacility::getIdentifier(VALUES[tj]);
                    Iter RESULT = primaryManipulator(&mX, id, xscratch);
                    ASSERTV(LENGTH, tj, CONFIG, *RESULT,
                            VALUES[tj] == *RESULT);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\t\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    mX.clear();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // No allocations done by 'clear'
                ASSERTV(1 == numPasses);

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
                // 2x time due to inserting full set of duplicated values
                ASSERTV(LENGTH, CONFIG, B, A,
                        0 == B - LENGTH * TYPE_ALLOC * 2 - A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int  id = TstFacility::getIdentifier(VALUES[tj]);
                    Iter RESULT = primaryManipulator(&mX, id, xscratch);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *RESULT);
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

        enum { NUM_INITIAL_NUM_BUCKETS =
                        sizeof initialNumBuckets / sizeof *initialNumBuckets };

        for (int ti = 0; ti < NUM_INITIAL_NUM_BUCKETS; ++ti) {
            {
                bslma::TestAllocator         da("default",
                                                veryVeryVeryVerbose);
                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj        mX(initialNumBuckets[ti]);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj        mX(initialNumBuckets[ti],
                                  defaultHasher);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj        mX(initialNumBuckets[ti],
                                  defaultHasher,
                                  defaultComparator);
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }
                {
                    Obj        mX(initialNumBuckets[ti],
                                  defaultHasher,
                                  defaultComparator,
                                  ALLOC(0));
                    const Obj& X = mX;
                    ASSERTV(X.bucket_count() >= initialNumBuckets[ti]);
                }

                ASSERTV(ti, da.numBlocksTotal(),
                                       (ti > 0) || (0 == da.numBlocksTotal()));
            }
            {
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
                KeyAllocator         xsa(&sa);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj        mX(initialNumBuckets[ti],
                              defaultHasher,
                              defaultComparator,
                              xsa);
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
// Unordered sets are useful in situations when there is no meaningful way to
// order key values, when the order of the values is irrelevant to the problem
// domain, and (even if there is a meaningful ordering) the value of ordering
// the results is outweighed by the higher performance provided by unordered
// sets (compared to ordered sets).
//
// One uses a multiset (ordered or unordered) when there may be more than one
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
// nothing is lost by using an unordered multiset instead of an ordered
// multiset:
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
// Next, we define the type of the unordered multiset and a convenience
// aliases:
//..
    typedef bsl::unordered_multiset<CustomerDatum,
                                    CustomerDatumHash,
                                    CustomerDatumEqual> DataByProfile;
    typedef DataByProfile::const_iterator               DataByProfileConstItr;
//..
// Now, create a helper function to calculate the average financials for a
// category of customer profiles within the unordered multiset.
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
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 37: {
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
// Then, we create an unordered multiset and insert each item of 'data'.
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
      case 36: // falls through
      case 35: // falls through
      case 34: // falls through
      case 33: // falls through
      case 32: // falls through
      case 31: // falls through
      case 30: // falls through
      case 29: // falls through
      case 28: // falls through
      case 27: // falls through
      case 26: // falls through
      case 25: // falls through
      case 24: // falls through
      case 23: // falls through
      case 22: // falls through
      case 21: // falls through
      case 20: // falls through
      case 19: // falls through
      case 18: // falls through
      case 17: // falls through
      case 16: // falls through
      case 15: // falls through
      case 14: // falls through
      case 13: // falls through
      case 12: {
        if (verbose) printf(
      "\nTEST CASE %d IS DELEGATED TO 'bslstl_unorderedmultiset_test.t.cpp'"
      "\n==================================================================\n",
      test);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTION 'g'"
                            "\n==============================\n");

        if (verbose) printf("\nThis test has been disabled.\n");
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STREAMING FUNCTIONALITY"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ASSIGNMENT OPERATOR\n"
                            "===========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase9,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'swap'\n"
                            "==============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // 'propagate_on_container_swap' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // TBD test 'bsltf::MoveOnlyAllocTestType' here

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase8,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // 'select_on_container_copy_construction' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase7,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY OPERATORS"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

       RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase6,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING OUTPUT (<<) OPERATOR"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING GENERATOR FUNCTIONS 'gg' and 'ggg'\n"
                            "==========================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

       RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase3,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("PRIMARY MANIPULATORS\n"
                            "====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase2,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
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
        for (int i = 0; i != MAX_SAMPLE; ++i) {
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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose) printf("\nAdditional tests: initializer lists.\n");
        {
            ASSERT((0 == []() -> bsl::unordered_multiset<int> {
                return {};
            }().size()));
            ASSERT((1 == []() -> bsl::unordered_multiset<int> {
                return {1};
            }().size()));
            ASSERT((3 == []() -> bsl::unordered_multiset<int> {
                return {3, 1, 3};
            }().size()));
        }
#endif
        if (veryVerbose)
            printf("Final message to confim the end of the breathing test.\n");
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
