// bslstl_unorderedset_test.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedset_test.h>

#include <bslstl_pair.h>
#include <bslstl_unorderedset.h>

#include <bslalg_rangecompare.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <utility> // move

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;
using bsls::NameOf;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, 'bslstl_unorderedset.t.cpp' (cases 1-11, plus the usage
// example), and 'bslstl_unorderedset_test.cpp' (cases 12 and higher).
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
//*[ 2] unordered_set(size_type, hasher, allocator);
//*[ 2] unordered_set(size_type, allocator);
//*[12] unordered_set(ITER, ITER, size_type, hasher, key_equal, allocator);
// [  ] unordered_set(ITER, ITER, size_type, hasher, allocator);
// [  ] unordered_set(ITER, ITER, size_type, allocator);
// [  ] unordered_set(ITER, ITER, allocator);
//*[ 7] unordered_set(const unordered_set& original);
//*[28] unordered_set(unordered_set&& original);
//*[11] unordered_set(const A& allocator);
//*[ 7] unordered_set(const unordered_set& original, const A& allocator);
//*[28] unordered_set(unordered_set&& original, const A& allocator);
// [32] unordered_set(initializer_list, const A&);
// [32] unordered_set(initializer_list, size_type, const A&);
// [32] unordered_set(initializer_list, size_type, hasher, const A&);
// [32] unordered_set(initializer_list, size_type, hasher, pred, const A&);
// [32] unordered_set(initializer_list);
// [32] unordered_set(initializer_list, size_t);
// [32] unordered_set(initializer_list, size_t, HASH);
// [32] unordered_set(initializer_list, size_t, HASH, EQUAL);
// [32] void insert(initializer_list);
//*[ 2] ~unordered_set();
//*[ 9] unordered_set& operator=(const unordered_set& rhs);
//*[29] unordered_set& operator=(unordered_set&& rhs);
// [32] unordered_set& operator=(initializer_list<value_type>);
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
// [30] bsl::pair<iterator, bool> insert(value_type&& value);
//*[15] iterator insert(const_iterator position, const value_type& value);
// [30] iterator insert(const_iterator position, value_type&& value);
//*[17] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [32] bsl::pair<iterator, bool> insert(initializer_list<value_type>);
//
// [30] pair<iterator, bool> emplace(Args&&... args);
// [31] iterator emplace_hint(const_iterator position, Args&&... args);
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
// [36] USAGE EXAMPLE
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
// [33] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
// [34] CONCERN: 'find'        properly handles transparent comparators.
// [34] CONCERN: 'count'       properly handles transparent comparators.
// [34] CONCERN: 'equal_range' properly handles transparent comparators.
// [35] CLASS TEMPLATE DEDUCTION GUIDES

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
#define L_  BSLS_BSLTESTUTIL_L_  // current line number

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

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU   // 'printf' flag for 'size_t'

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
    // This leading branch is the preferred version for C++17, but the feature
    // test macro is (currently) for documentation purposes only, and never
    // defined.  This is the ideal (simplest) form for such declarations:
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of 'constexpr' in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // 'bsl::integral_constant' is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

#if defined(BDE_BUILD_TARGET_EXC)
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
#endif

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
    { L_,    1, "AA",                "A"                 },
    { L_,   15, "B",                 "B"                 },
    { L_,    2, "AB",                "AB"                },
    { L_,    2, "BA",                "AB"                },
    { L_,   14, "AC",                "AC"                },
    { L_,   18, "CD",                "CD"                },
    { L_,    3, "ABC",               "ABC"               },
    { L_,    3, "ACB",               "ABC"               },
    { L_,    3, "BAC",               "ABC"               },
    { L_,    3, "BCA",               "ABC"               },
    { L_,    3, "CAB",               "ABC"               },
    { L_,    3, "CBA",               "ABC"               },
    { L_,   12, "BAD",               "ABD"               },
    { L_,    3, "ABCA",              "ABC"               },
    { L_,    3, "ABCB",              "ABC"               },
    { L_,    3, "ABCC",              "ABC"               },
    { L_,    3, "ABCABC",            "ABC"               },
    { L_,    3, "AABBCC",            "ABC"               },
    { L_,    4, "ABCD",              "ABCD"              },
    { L_,    4, "ACBD",              "ABCD"              },
    { L_,    4, "BDCA",              "ABCD"              },
    { L_,    4, "DCBA",              "ABCD"              },
    { L_,   13, "BEAD",              "ABDE"              },
    { L_,   16, "BCDE",              "BCDE"              },
    { L_,    5, "ABCDE",             "ABCDE"             },
    { L_,    5, "ACBDE",             "ABCDE"             },
    { L_,    5, "CEBDA",             "ABCDE"             },
    { L_,    5, "EDCBA",             "ABCDE"             },
    { L_,   17, "FEDCB",             "BCDEF"             },
    { L_,    6, "FEDCBA",            "ABCDEF"            },
    { L_,    7, "ABCDEFG",           "ABCDEFG"           },
    { L_,    8, "ABCDEFGH",          "ABCDEFGH"          },
    { L_,    9, "ABCDEFGHI",         "ABCDEFGHI"         },
    { L_,   10, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP"  },
    { L_,   10, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP"  },
    { L_,   11, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   11, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};

enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

// Define values used to initialize positional arguments for
// 'bsltf::EmplacableTestType' and 'bsltf::AllocEmplacableTestType'
// constructors.  Note, that you cannot change those values as they are used by
// 'TemplateTestFacility::getIdentifier' to map the constructed emplacable
// objects to their integer identifiers.
static const int V01 = 1;
static const int V02 = 20;
static const int V03 = 23;
static const int V04 = 44;
static const int V05 = 66;
static const int V06 = 176;
static const int V07 = 878;
static const int V08 = 8;
static const int V09 = 912;
static const int V10 = 102;

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

template <class TYPE>
inline
int valueOf(const TYPE& value)
    // Return the 'id' with which the specified 'value' was created.
{
    return bsltf::TemplateTestFacility::getIdentifier(value);
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef          CONTAINER            TestType;
    typedef typename CONTAINER::size_type SizeType;

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
    typedef          CONTAINER            TestType;
    typedef typename CONTAINER::size_type SizeType;

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
    typedef typename CONTAINER::size_type      SizeType;
    typedef typename CONTAINER::const_iterator TestIterator;

    ASSERT(x.size() == nCopies * size);

    for (SizeType i = 0; i != size; ++i) {
        TestIterator it = x.find(keyForValue<CONTAINER>(data[i]));
        ASSERT(x.end() != it);
        ASSERT(*it == data[i]);
        ASSERT(x.count(keyForValue<CONTAINER>(data[i])) == nCopies);

        bsl::pair<TestIterator, TestIterator> range = x.equal_range(
                                              keyForValue<CONTAINER>(data[i]));
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
void fillContainerWithData(CONTAINER&                            x,
                           const typename CONTAINER::value_type *data,
                           int                                   size)
{
    typedef CONTAINER TestType;

    int initialSize = x.size();
    x.insert(data, data + size);
    ASSERT(x.size() == initialSize + size);

    for (int i = 0; i != size; ++i) {
        typename TestType::iterator it = x.find(
                                              keyForValue<CONTAINER>(data[i]));
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
    typedef typename CONTAINER::size_type            SizeType;
    typedef typename CONTAINER::local_iterator       local_iterator;
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
    size_t         bucketA = x.bucket(key);
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
        typedef typename TObj::const_iterator               CIter;
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
    struct rebind {
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
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores the
    // a copy of an object of the parameterized type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_control;   // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionProctor(const OBJECT     *object,
                     int               line,
                     const ALLOCATOR&  basicAllocator)
    : d_line(line)
    , d_control(*object, basicAllocator)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
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
        // Destroy the exception proctor.  If the proctor was not released,
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
        // Release this proctor from verifying the state of the object supplied
        // at construction.
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
    int         d_id;     // identifier for the functor
    mutable int d_count;  // number of times 'operator()' is called

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
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return bsltf::TemplateTestFacility::getIdentifier(lhs)
            == bsltf::TemplateTestFacility::getIdentifier(rhs);
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
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
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
        // called.   Return '0' if this functor was specified with
        // 'shortCircuit' parameter equal to 'true' on construction and
        // identifier of the specified 'obj' otherwise.
    {
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV(!"'TestComparator' was invoked when it was disabled");
        }

        ++d_count;

        return d_shortCircuit
               ? 0
               : bsltf::TemplateTestFacility::getIdentifier(obj);
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
    // This class provides a hash functor whose 'operator()' hasn't been
    // declared 'const'.

  public:
    // CREATORS
    TestNonConstHashFunctor()
        // Create a copy of the specified 'original'.
    {
    }

    //! TestNonConstHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    size_t operator() (const TYPE& obj)
        // Return identifier of the specified 'obj'.
    {
        return bsltf::TemplateTestFacility::getIdentifier(obj);
    }

    bool operator==(const TestNonConstHashFunctor&)
    {
        return true;
    }
};

                            // ===================
                            // struct ThrowingHash
                            // ===================

template <class TYPE>
struct ThrowingHash : public bsl::hash<TYPE> {
    // This dummy class implements the minimal interface that meets the
    // requirements for a hasher that can throw exceptions from the move
    // assignment operator and from the 'swap' method/free function.

  public:
    // CREATORS
    ThrowingHash()
        // Create a 'ThrowingHash' object.
    {
    }

    ThrowingHash(const ThrowingHash&)
        // Create a 'ThrowingHash' object.
    {
    }

    ThrowingHash(bslmf::MovableRef<ThrowingHash>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingHash' object.
    {
    }

    // MANIPULATORS
    ThrowingHash &operator=(const ThrowingHash&)
        // Return a reference, providing modifiable access to this object.
    {
        return *this;
    }

    ThrowingHash&
    operator=(BloombergLP::bslmf::MovableRef<ThrowingHash>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Return a reference, providing modifiable access to this object.
    {
        return *this;
    }

    void swap(ThrowingHash&) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Do nothing.
    {
    }
};

// FREE FUNCTIONS
template <class TYPE>
void swap(ThrowingHash<TYPE>&,
          ThrowingHash<TYPE>&) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    // Do nothing.
{
}

                     // =================================
                     // class ThrowingAssignmentPredicate
                     // =================================

template <class TYPE>
struct ThrowingAssignmentPredicate : public bsl::equal_to<TYPE> {
    // This dummy class implements the minimal interface that meets the
    // requirements for a predicate that can throw exceptions from the move
    // assignment operator and from the 'swap' method/free function.

  public:
    // CREATORS
    ThrowingAssignmentPredicate()
        // Create a 'ThrowingAssignmentPredicate' object.
    {
    }

    ThrowingAssignmentPredicate(const ThrowingAssignmentPredicate&)
        // Create a 'ThrowingAssignmentPredicate' object.
    {
    }

    ThrowingAssignmentPredicate(bslmf::MovableRef<ThrowingAssignmentPredicate>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingAssignmentPredicate' object.
    {
    }

    // MANIPULATORS
    ThrowingAssignmentPredicate &operator=(const ThrowingAssignmentPredicate&)
        // Return a reference, providing modifiable access to this object.
    {
        return *this;
    }

    ThrowingAssignmentPredicate&
    operator=(BloombergLP::bslmf::MovableRef<ThrowingAssignmentPredicate>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Return a reference, providing modifiable access to this object.
    {
        return *this;
    }

    void swap(ThrowingAssignmentPredicate&)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Do nothing.
    {
    }
};

// FREE FUNCTIONS
template <class TYPE>
void swap(ThrowingAssignmentPredicate<TYPE>&,
          ThrowingAssignmentPredicate<TYPE>&)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    // Do nothing.
{
}

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

                    // =============================
                    // class TransparentlyComparable
                    // =============================

class TransparentlyComparable {
    // DATA
    int d_conversionCount;  // number of times 'operator int' has been called
    int d_value;            // the value

    // NOT IMPLEMENTED
    TransparentlyComparable(const TransparentlyComparable&);  // = delete

  public:
    // CREATORS
    explicit TransparentlyComparable(int value)
        // Create an object having the specified 'value'.

    : d_conversionCount(0)
    , d_value(value)
    {
    }

    // MANIPULATORS
    operator int()
        // Return the current value of this object.
    {
        ++d_conversionCount;
        return d_value;
    }

    // ACCESSORS
    int conversionCount() const
        // Return the number of times 'operator int' has been called.
    {
        return d_conversionCount;
    }

    int value() const
        // Return the current value of this object.
    {
        return d_value;
    }

    friend bool operator==(const TransparentlyComparable& lhs, int rhs)
        // Return 'true' if the value of the specified 'lhs' is equal to the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs.d_value == rhs;
    }

    friend bool operator==(int lhs, const TransparentlyComparable& rhs)
        // Return 'true' if the specified 'lhs' is equal to the value of the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs == rhs.d_value;
    }
};

                    // ============================
                    // struct TransparentComparator
                    // ============================

struct TransparentComparator
    // This class can be used as a comparator for containers.  It has a nested
    // type 'is_transparent', so it is classified as transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction and can be used for
    // heterogeneous comparison.
 {
    typedef void is_transparent;

    template <class LHS, class RHS>
    bool operator()(const LHS& lhs, const RHS& rhs) const
        // Return 'true' if the specified 'lhs' is equivalent to the specified
        // 'rhs' and 'false' otherwise.
    {
        return lhs == rhs;
    }
};

                      // ========================
                      // struct TransparentHasher
                      // ========================

struct TransparentHasher
    // This class can be used as a comparator for containers.  It has a nested
    // type 'is_transparent', so it is classified as transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction and can be used for
    // heterogeneous comparison.
 {
    typedef void is_transparent;

    size_t operator () (const TransparentlyComparable &value) const
    {
        return static_cast<size_t>(value.value());
    }

    template <class VALUE>
    size_t operator()(const VALUE &value) const
    {
        return static_cast<size_t>(value);
    }
};

template <class Container>
void testTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
    // Search for a key equal to the specified 'initKeyValue' in the specified
    // 'container', and count the number of conversions expected based on the
    // specified 'isTransparent'.  Note that 'Container' may resolve to a
    // 'const'-qualified type, we are using the "reference" here as a sort of
    // universal reference; conceptually, the object remains constant, but we
    // want to test 'const'-qualified and non-'const'-qualified overloads.
{
    typedef typename Container::const_iterator Iterator;
    typedef typename Container::size_type      Count;

    int expectedConversionCount = 0;

    TransparentlyComparable existingKey(initKeyValue);
    TransparentlyComparable nonExistingKey(initKeyValue ? -initKeyValue
                                                        : -100);

    ASSERT(existingKey.conversionCount() == expectedConversionCount);

    // Testing 'find'.

    const Iterator EXISTING_F = container.find(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(container.end()               != EXISTING_F);
    ASSERT(existingKey.value()           == *EXISTING_F);
    ASSERT(existingKey.conversionCount() == expectedConversionCount);

    const Iterator NON_EXISTING_F = container.find(nonExistingKey);
    ASSERT(container.end()                  == NON_EXISTING_F);
    ASSERT(nonExistingKey.conversionCount() == expectedConversionCount);

    // Testing 'contains'.

    const bool EXISTING_CONTAINS = container.contains(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(true == EXISTING_CONTAINS);
    ASSERT(existingKey.conversionCount() == expectedConversionCount);

    const bool NON_EXISTING_CONTAINS = container.contains(nonExistingKey);
    ASSERT(false == NON_EXISTING_CONTAINS);
    ASSERT(nonExistingKey.conversionCount() == expectedConversionCount);

    // Testing 'count'.

    const Count EXPECTED_C = 1;
    const Count EXISTING_C = container.count(existingKey);

    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXPECTED_C              == EXISTING_C);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Count NON_EXISTING_C = container.count(nonExistingKey);
    ASSERT(0                       == NON_EXISTING_C);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'equal_range'.

    const bsl::pair<Iterator, Iterator> EXISTING_ER =
                                            container.equal_range(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(expectedConversionCount == existingKey.conversionCount());
    ASSERT(EXPECTED_C ==
     static_cast<Count>(bsl::distance(EXISTING_ER.first, EXISTING_ER.second)));

    for (Iterator it = EXISTING_ER.first; it != EXISTING_ER.second; ++it) {
        ASSERT(existingKey.value() == *it);
    }

    const bsl::pair<Iterator, Iterator> NON_EXISTING_ER =
                                         container.equal_range(nonExistingKey);
    ASSERT(NON_EXISTING_ER.first   == NON_EXISTING_ER.second);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
}

}  // close unnamed namespace

                             // ============
                             // class EqPred
                             // ============

template <class TYPE>
struct EqPred
    // A predicate for testing 'erase_if'; it takes a value at construction
    // and uses it for comparisons later.
{
    TYPE d_ch;
    EqPred(TYPE ch) : d_ch(ch) {}

    bool operator() (TYPE ch) const
        // return 'true' if the specified 'ch' is equal to the stored value,
        // and 'false' otherwise.
    {
        return d_ch == ch;
    }
};

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

    typedef typename Obj::iterator             Iter;
    typedef typename Obj::const_iterator       CIter;
    typedef typename Obj::size_type            SizeType;
    typedef typename Obj::value_type           ValueType;
        // Shorthands

    typedef bsltf::TestValuesArray<KEY, ALLOC> TestValues;
    typedef bsltf::MoveState                   MoveState;
    typedef bsltf::TemplateTestFacility        TstFacility;
    typedef bslma::ConstructionUtil            ConsUtil;
    typedef bslmf::MovableRefUtil              MoveUtil;
    typedef TestMovableTypeUtil<Iter, KEY>     TstMovUtil;

    typedef typename ALLOC::template rebind<KEY>::other KeyAllocator;

    enum { k_TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value };

    enum { k_KEY_IS_WELL_BEHAVED =
                      bsl::is_same<KEY,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,
           k_KEY_IS_MOVE_ENABLED =
                      bsl::is_same<KEY, bsltf::MovableTestType>::value ||
                      bsl::is_same<KEY, bsltf::MovableAllocTestType>::value ||
                      bsl::is_same<KEY, bsltf::MoveOnlyAllocTestType>::value ||
                      k_KEY_IS_WELL_BEHAVED };

  public:
    typedef bsltf::StdTestAllocator<KEY> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'unordered_set<KEY, HASHER, EQUAL, ALLOC>'
    // object.
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
    static pair<Iter, bool>
    primaryManipulator(Obj *container, int identifier, ALLOC allocator)
        // Insert into the specified 'container' the value object indicated by
        // the specified 'identifier', ensuring that the overload of the
        // primary manipulator taking a modifiable rvalue is invoked (rather
        // than the one taking an lvalue).  Return the result of invoking the
        // primary manipulator.
    {
        bsls::ObjectBuffer<ValueType> buffer;
        TstFacility::emplace(buffer.address(),
                             identifier,
                             allocator);
        bslma::DestructorGuard<KEY> guard(buffer.address());

        return container->insert(MoveUtil::move(buffer.object()));
    }

    static
    bsltf::TestValuesArrayIterator<KEY> indexIterator(TestValues  *testValues,
                                                      std::size_t  index);
        // Return, by value, an iterator to the specified 'index'th element of
        // the specified 'testValues'.

    static int getIndexForIter(const Obj& obj, Iter it);
        // Find the index corresponding to the specified iterator 'it', which
        // must be a valid iterator referring to the specified 'obj'.  Note
        // that 'obj.end() == it' is allowed.

    static Iter getIterForIndex(const Obj& obj, size_t idx);
        // Return the iterator relating to the specified 'obj' with specified
        // index 'idx'.  It is an error if 'idx >= obj.size()'.

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
    {
        return MoveUtil::move(t);
    }
    template <class T>
    static const T&             testArg(T& t, bsl::false_type)
    {
        return t;
    }

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static void testCase31a_RunTest(Obj *target, bool inserted);
        // Call 'emplace' on the specified 'target' container and verify that
        // a value was newly inserted if and only if the specified 'inserted'
        // flag is 'true'.  Forward (template parameter) 'N_ARGS' arguments to
        // the 'emplace' method and ensure 1) that values are properly passed
        // to the constructor of 'value_type', 2) that the allocator is
        // correctly configured for each argument in the newly inserted element
        // in 'target', and 3) that the arguments are forwarded using copy or
        // move semantics based on integer template parameters 'N01' ... 'N10'.

    template <int N_ARGS,
              int N01,
              int N02,
              int N03,
              int N04,
              int N05,
              int N06,
              int N07,
              int N08,
              int N09,
              int N10>
    static Iter testCase31b_RunTest(Obj *target, CIter hint, bool inserted);
        // Call 'emplace_hint' on the specified 'target' container and verify
        // that a value was newly inserted if and only if the specified
        // 'inserted' flag is 'true'.  Forward (template parameter) 'N_ARGS'
        // arguments to the 'emplace' method and ensure 1) that values are
        // properly passed to the constructor of 'value_type', 2) that the
        // allocator is correctly configured for each argument in the newly
        // inserted element in 'target', and 3) that the arguments are
        // forwarded using copy or move semantics based on integer template
        // parameters 'N01' ... 'N10'.

  public:
    // TEST CASES

    static void testCase36();
        // Test free function 'bsl::erase_if'

    static void testCase33();
        // Test 'noexcept' specifications

    static void testCase32_outOfLine();
    static void testCase32_inline();
        // Test initializer lists.

    static void testCase31b();
        // Test forwarding of arguments in 'emplace_hint' method.

    static void testCase31a();
        // Test forwarding of arguments in 'emplace' method.

    static void testCase31();
        // Test emplace.

    static void testCase30();
        // Test insertion on movable values.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase29_propagate_on_container_move_assignment_dispatch();
    static void testCase29_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase29();
        // Test move assignment.

    static void testCase27();
        // Test spread of nodes into different buckets

    static void testCase28();
        // Test move construction.

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
        // the parameterized 'KEY'.
        // N/A
#endif

    static void testCase18();
        // Test 'erase'.

    static void testCase17();
        // Test range 'insert'.

#if 0
    static void testCase16();
        // Test 'insert' with hint.
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
    const TestValues VALUES;

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
    Obj object(ALLOC(0));
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

    Iter   ret = obj.begin();
    size_t i = 0;
    for (; i < idx && obj.end() != ret; ++i) {
        ++ret;
    }

    ASSERTV(idx == i);

    return ret;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
void
TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase31a_RunTest(Obj  *target,
                                                         bool  inserted)
{
    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 30 is not a test allocator!");
        return;
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj&                  mX = *target;
    const Obj&            X  = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bsl::pair<Iter, bool> result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUF01;
        ConsUtil::construct(BUF01.address(), &aa, V01);
        typename KEY::ArgType01& A01 = BUF01.object();
        bslma::DestructorGuard<typename KEY::ArgType01> G01(&A01);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUF02;
        ConsUtil::construct(BUF02.address(), &aa, V02);
        typename KEY::ArgType02& A02 = BUF02.object();
        bslma::DestructorGuard<typename KEY::ArgType02> G02(&A02);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUF03;
        ConsUtil::construct(BUF03.address(), &aa, V03);
        typename KEY::ArgType03& A03 = BUF03.object();
        bslma::DestructorGuard<typename KEY::ArgType03> G03(&A03);

        bsls::ObjectBuffer<typename KEY::ArgType04> BUF04;
        ConsUtil::construct(BUF04.address(), &aa, V04);
        typename KEY::ArgType04& A04 = BUF04.object();
        bslma::DestructorGuard<typename KEY::ArgType04> G04(&A04);

        bsls::ObjectBuffer<typename KEY::ArgType05> BUF05;
        ConsUtil::construct(BUF05.address(), &aa, V05);
        typename KEY::ArgType05& A05 = BUF05.object();
        bslma::DestructorGuard<typename KEY::ArgType05> G05(&A05);

        bsls::ObjectBuffer<typename KEY::ArgType06> BUF06;
        ConsUtil::construct(BUF06.address(), &aa, V06);
        typename KEY::ArgType06& A06 = BUF06.object();
        bslma::DestructorGuard<typename KEY::ArgType06> G06(&A06);

        bsls::ObjectBuffer<typename KEY::ArgType07> BUF07;
        ConsUtil::construct(BUF07.address(), &aa, V07);
        typename KEY::ArgType07& A07 = BUF07.object();
        bslma::DestructorGuard<typename KEY::ArgType07> G07(&A07);

        bsls::ObjectBuffer<typename KEY::ArgType08> BUF08;
        ConsUtil::construct(BUF08.address(), &aa,  V08);
        typename KEY::ArgType08& A08 = BUF08.object();
        bslma::DestructorGuard<typename KEY::ArgType08> G08(&A08);

        bsls::ObjectBuffer<typename KEY::ArgType09> BUF09;
        ConsUtil::construct(BUF09.address(), &aa, V09);
        typename KEY::ArgType09& A09 = BUF09.object();
        bslma::DestructorGuard<typename KEY::ArgType09> G09(&A09);

        bsls::ObjectBuffer<typename KEY::ArgType10> BUF10;
        ConsUtil::construct(BUF10.address(), &aa, V10);
        typename KEY::ArgType10& A10 = BUF10.object();
        bslma::DestructorGuard<typename KEY::ArgType10> G10(&A10);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);

        switch (N_ARGS) {
          case 0: {
            result = mX.emplace();
          } break;
          case 1: {
            result = mX.emplace(testArg(A01, MOVE_01));
          } break;
          case 2: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02));
          } break;
          case 3: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03));
          } break;
          case 4: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04));
          } break;
          case 5: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04),
                                testArg(A05, MOVE_05));
          } break;
          case 6: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04),
                                testArg(A05, MOVE_05),
                                testArg(A06, MOVE_06));
          } break;
          case 7: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04),
                                testArg(A05, MOVE_05),
                                testArg(A06, MOVE_06),
                                testArg(A07, MOVE_07));
          } break;
          case 8: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04),
                                testArg(A05, MOVE_05),
                                testArg(A06, MOVE_06),
                                testArg(A07, MOVE_07),
                                testArg(A08, MOVE_08));
          } break;
          case 9: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04),
                                testArg(A05, MOVE_05),
                                testArg(A06, MOVE_06),
                                testArg(A07, MOVE_07),
                                testArg(A08, MOVE_08),
                                testArg(A09, MOVE_09));
          } break;
          case 10: {
            result = mX.emplace(testArg(A01, MOVE_01),
                                testArg(A02, MOVE_02),
                                testArg(A03, MOVE_03),
                                testArg(A04, MOVE_04),
                                testArg(A05, MOVE_05),
                                testArg(A06, MOVE_06),
                                testArg(A07, MOVE_07),
                                testArg(A08, MOVE_08),
                                testArg(A09, MOVE_09),
                                testArg(A10, MOVE_10));
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        ASSERTV(inserted, inserted == result.second);

        ASSERTV(MOVE_01, A01.movedFrom(),
               MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
        ASSERTV(MOVE_02, A02.movedFrom(),
               MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
        ASSERTV(MOVE_03, A03.movedFrom(),
               MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
        ASSERTV(MOVE_04, A04.movedFrom(),
               MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
        ASSERTV(MOVE_05, A05.movedFrom(),
               MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
        ASSERTV(MOVE_06, A06.movedFrom(),
               MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
        ASSERTV(MOVE_07, A07.movedFrom(),
               MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
        ASSERTV(MOVE_08, A08.movedFrom(),
               MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
        ASSERTV(MOVE_09, A09.movedFrom(),
               MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
        ASSERTV(MOVE_10, A10.movedFrom(),
               MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

        const KEY& V = *(result.first);

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
        ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
        ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
        ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
        ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
        ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
        ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
        ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

        TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
template <int N_ARGS,
          int N01,
          int N02,
          int N03,
          int N04,
          int N05,
          int N06,
          int N07,
          int N08,
          int N09,
          int N10>
typename TestDriver<KEY, HASH, EQUAL, ALLOC>::Iter
TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase31b_RunTest(Obj   *target,
                                                         CIter  hint,
                                                         bool   inserted)
{
    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    DECLARE_BOOL_CONSTANT(MOVE_01, N01 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_02, N02 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_03, N03 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_04, N04 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_05, N05 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_06, N06 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_07, N07 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_08, N08 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_09, N09 == 1);
    DECLARE_BOOL_CONSTANT(MOVE_10, N10 == 1);

    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 31 is not a test allocator!");
        return hint;
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj&                  mX = *target;
    const Obj&            X  = mX;

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);
    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    Iter result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUF01;
        ConsUtil::construct(BUF01.address(), &aa, V01);
        typename KEY::ArgType01& A01 = BUF01.object();
        bslma::DestructorGuard<typename KEY::ArgType01> G01(&A01);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUF02;
        ConsUtil::construct(BUF02.address(), &aa, V02);
        typename KEY::ArgType02& A02 = BUF02.object();
        bslma::DestructorGuard<typename KEY::ArgType02> G02(&A02);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUF03;
        ConsUtil::construct(BUF03.address(), &aa, V03);
        typename KEY::ArgType03& A03 = BUF03.object();
        bslma::DestructorGuard<typename KEY::ArgType03> G03(&A03);

        bsls::ObjectBuffer<typename KEY::ArgType04> BUF04;
        ConsUtil::construct(BUF04.address(), &aa, V04);
        typename KEY::ArgType04& A04 = BUF04.object();
        bslma::DestructorGuard<typename KEY::ArgType04> G04(&A04);

        bsls::ObjectBuffer<typename KEY::ArgType05> BUF05;
        ConsUtil::construct(BUF05.address(), &aa, V05);
        typename KEY::ArgType05& A05 = BUF05.object();
        bslma::DestructorGuard<typename KEY::ArgType05> G05(&A05);

        bsls::ObjectBuffer<typename KEY::ArgType06> BUF06;
        ConsUtil::construct(BUF06.address(), &aa, V06);
        typename KEY::ArgType06& A06 = BUF06.object();
        bslma::DestructorGuard<typename KEY::ArgType06> G06(&A06);

        bsls::ObjectBuffer<typename KEY::ArgType07> BUF07;
        ConsUtil::construct(BUF07.address(), &aa, V07);
        typename KEY::ArgType07& A07 = BUF07.object();
        bslma::DestructorGuard<typename KEY::ArgType07> G07(&A07);

        bsls::ObjectBuffer<typename KEY::ArgType08> BUF08;
        ConsUtil::construct(BUF08.address(), &aa,  V08);
        typename KEY::ArgType08& A08 = BUF08.object();
        bslma::DestructorGuard<typename KEY::ArgType08> G08(&A08);

        bsls::ObjectBuffer<typename KEY::ArgType09> BUF09;
        ConsUtil::construct(BUF09.address(), &aa, V09);
        typename KEY::ArgType09& A09 = BUF09.object();
        bslma::DestructorGuard<typename KEY::ArgType09> G09(&A09);

        bsls::ObjectBuffer<typename KEY::ArgType10> BUF10;
        ConsUtil::construct(BUF10.address(), &aa, V10);
        typename KEY::ArgType10& A10 = BUF10.object();
        bslma::DestructorGuard<typename KEY::ArgType10> G10(&A10);

        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &scratch);

        switch (N_ARGS) {
          case 0: {
            result = mX.emplace_hint(hint);
          } break;
          case 1: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01));
          } break;
          case 2: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02));
          } break;
          case 3: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03));
          } break;
          case 4: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04));
          } break;
          case 5: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04),
                                     testArg(A05, MOVE_05));
          } break;
          case 6: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04),
                                     testArg(A05, MOVE_05),
                                     testArg(A06, MOVE_06));
          } break;
          case 7: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04),
                                     testArg(A05, MOVE_05),
                                     testArg(A06, MOVE_06),
                                     testArg(A07, MOVE_07));
          } break;
          case 8: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04),
                                     testArg(A05, MOVE_05),
                                     testArg(A06, MOVE_06),
                                     testArg(A07, MOVE_07),
                                     testArg(A08, MOVE_08));
          } break;
          case 9: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04),
                                     testArg(A05, MOVE_05),
                                     testArg(A06, MOVE_06),
                                     testArg(A07, MOVE_07),
                                     testArg(A08, MOVE_08),
                                     testArg(A09, MOVE_09));
          } break;
          case 10: {
            result = mX.emplace_hint(hint,
                                     testArg(A01, MOVE_01),
                                     testArg(A02, MOVE_02),
                                     testArg(A03, MOVE_03),
                                     testArg(A04, MOVE_04),
                                     testArg(A05, MOVE_05),
                                     testArg(A06, MOVE_06),
                                     testArg(A07, MOVE_07),
                                     testArg(A08, MOVE_08),
                                     testArg(A09, MOVE_09),
                                     testArg(A10, MOVE_10));
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }
        proctor.release();

        ASSERTV(inserted, inserted ==
                               (hint == X.end()) || (&(*result) != &(*hint)));

        ASSERTV(MOVE_01, A01.movedFrom(),
               MOVE_01 == (MoveState::e_MOVED == A01.movedFrom()) || 2 == N01);
        ASSERTV(MOVE_02, A02.movedFrom(),
               MOVE_02 == (MoveState::e_MOVED == A02.movedFrom()) || 2 == N02);
        ASSERTV(MOVE_03, A03.movedFrom(),
               MOVE_03 == (MoveState::e_MOVED == A03.movedFrom()) || 2 == N03);
        ASSERTV(MOVE_04, A04.movedFrom(),
               MOVE_04 == (MoveState::e_MOVED == A04.movedFrom()) || 2 == N04);
        ASSERTV(MOVE_05, A05.movedFrom(),
               MOVE_05 == (MoveState::e_MOVED == A05.movedFrom()) || 2 == N05);
        ASSERTV(MOVE_06, A06.movedFrom(),
               MOVE_06 == (MoveState::e_MOVED == A06.movedFrom()) || 2 == N06);
        ASSERTV(MOVE_07, A07.movedFrom(),
               MOVE_07 == (MoveState::e_MOVED == A07.movedFrom()) || 2 == N07);
        ASSERTV(MOVE_08, A08.movedFrom(),
               MOVE_08 == (MoveState::e_MOVED == A08.movedFrom()) || 2 == N08);
        ASSERTV(MOVE_09, A09.movedFrom(),
               MOVE_09 == (MoveState::e_MOVED == A09.movedFrom()) || 2 == N09);
        ASSERTV(MOVE_10, A10.movedFrom(),
               MOVE_10 == (MoveState::e_MOVED == A10.movedFrom()) || 2 == N10);

        const KEY& V = *result;

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == N01);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == N02);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == N03);
        ASSERTV(V04, V.arg04(), V04 == V.arg04() || 2 == N04);
        ASSERTV(V05, V.arg05(), V05 == V.arg05() || 2 == N05);
        ASSERTV(V06, V.arg06(), V06 == V.arg06() || 2 == N06);
        ASSERTV(V07, V.arg07(), V07 == V.arg07() || 2 == N07);
        ASSERTV(V08, V.arg08(), V08 == V.arg08() || 2 == N08);
        ASSERTV(V09, V.arg09(), V09 == V.arg09() || 2 == N09);
        ASSERTV(V10, V.arg10(), V10 == V.arg10() || 2 == N10);

        TestAllocatorUtil::test(V, oa);

    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
    return result;
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase36()
{
    // --------------------------------------------------------------------
    // TESTING FREE FUNCTION 'BSL::ERASE_IF'
    //
    // Concerns:
    //: 1 The free function exists, and is callable with an unordered_set.
    //
    // Plan:
    //: 1 Fill a set with known values, then attempt to erase some of
    //:   the values using 'bsl::erase_if'.  Verify that the resultant set
    //:   is the right size, contains the correct values, and that the
    //:   value returned from the functions is correct.
    //
    // Testing:
    //   size_t erase_if(unordered_set&, PREDICATE);
    // --------------------------------------------------------------------

    static const struct {
        int         d_line;       // source line number
        const char *d_initial_p;  // initial values
        char        d_element;    // value to remove
        const char *d_results_p;  // expected result value
    } DATA[] = {
        //line  initial              element  results
        //----  -------------------  -------  -------------------
        { L_,   "",                  'A',     ""                  },
        { L_,   "A",                 'A',     ""                  },
        { L_,   "A",                 'B',     "A"                 },
        { L_,   "B",                 'A',     "B"                 },
        { L_,   "AB",                'A',     "B"                 },
        { L_,   "BA",                'A',     "B"                 },
        { L_,   "BC",                'D',     "BC"                },
        { L_,   "ABC",               'C',     "AB"                },
        { L_,   "CBADEGHIJKL",       'B',     "CADEGHIJKL"        }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *initial = DATA[i].d_initial_p;
        size_t          initialLen = strlen(initial);
        const char     *results = DATA[i].d_results_p;
        size_t          resultsLen = strlen(results);

        Obj mX;
        Obj mRes;

        for (size_t j = 0; j < initialLen; ++j)
        {
            mX.insert(initial[j]);
        }

        for (size_t j = 0; j < resultsLen; ++j)
        {
            mRes.insert(results[j]);
        }

        EqPred<KEY> pred(DATA[i].d_element);
        size_t      ret   = bsl::erase_if(mX, pred);

        // Is the modified container correct?
        ASSERTV(LINE, mX == mRes);

        // Is the return value correct?
        ASSERTV(LINE, ret == initialLen - resultsLen);
        }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase33()
{
    // ------------------------------------------------------------------------
    // 'noexcept' SPECIFICATION
    //
    // Concerns:
    //: 1 The 'noexcept' specification has been applied to all class interfaces
    //:   required by the standard.
    //
    // Plan:
    //: 1 Apply the uniary 'noexcept' operator to expressions that mimic those
    //:   appearing in the standard and confirm that calculated boolean value
    //:   matches the expected value.
    //:
    //: 2 Since the 'noexcept' specification does not vary with the 'TYPE'
    //:   of the container, we need test for just one general type and any
    //:   'TYPE' specializations.
    //
    // Testing:
    //   CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<KEY>())
        P(bsls::NameOf<HASH>())
        P(bsls::NameOf<EQUAL>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: 23.5.6.1: Class template 'unordered_set' overview

    // page 892
    //..
    //     unordered_set& operator=(unordered_set&&)
    //         noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //                  is_nothrow_move_assignable<Hash>::value &&
    //                  is_nothrow_move_assignable<Pred>::value);
    //     allocator_type get_allocator() const noexcept;
    //..

    {
        Obj s;    (void) s;
        Obj x;    (void) x;

        bool expected = false;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        expected = bsl::allocator_traits<ALLOC> ::is_always_equal::value
                && std::is_nothrow_move_assignable<HASH>::value
                && std::is_nothrow_move_assignable<EQUAL>::value;
#endif
        ASSERT(expected
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s = MoveUtil::move(x)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.get_allocator()));
    }

    // page 892
    //..
    // // iterators:
    //     iterator begin() noexcept;
    //     const_iterator begin() const noexcept;
    //     iterator end() noexcept;
    //     const_iterator end() const noexcept;
    //     const_iterator cbegin() const noexcept;
    //     const_iterator cend() const noexcept;
    //..

    {
        Obj s; const Obj& S = s;    (void) S;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(S.cend()));
    }

    // page 892-893
    //..
    // // capacity:
    //    bool empty() const noexcept;
    //    size_type size() const noexcept;
    //    size_type max_size() const noexcept;
    //..

    {
        Obj s;    (void) s;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.max_size()));
    }

    // page 893
    //..
    //     void swap(unordered_set&)
    //         noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //                  is_nothrow_swappable_v<Hash> &&
    //                  is_nothrow_swappable_v<Pred>);
    //     void clear() noexcept;
    //..

    {
        Obj s;    (void) s;
        Obj x;    (void) x;

        bool expected = false;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        expected = bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                   bsl::is_nothrow_swappable<HASH>::value &&
                   bsl::is_nothrow_swappable<EQUAL>::value;
#endif

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.swap(x)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.clear()));
    }

    // page 893
    //..
    // // bucket interface:
    //    size_type bucket_count() const noexcept;
    //    size_type max_bucket_count() const noexcept;
    //..

    {
        Obj s;    (void) s;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.bucket_count()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.max_bucket_count()));
    }

    // page 893
    //..
    // // hash policy:
    //    float load_factor() const noexcept;
    //    float max_load_factor() const noexcept;
    //..

    {
        Obj s;    (void) s;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.load_factor()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(s.max_load_factor()));
    }

    // page 894
    //..
    // // 23.5.6.3, swap:
    //    template <class Key, class Hash, class Pred, class Alloc>
    //    void swap(unordered_set<Key, Hash, Pred, Alloc>& x,
    //              unordered_set<Key, Hash, Pred, Alloc>& y)
    //                  noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        bool expected = BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y));

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(x, y)));
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase32_outOfLine()
{
    // ------------------------------------------------------------------------
    // TESTING FUNCTIONS TAKING INITIALIZER LISTS
    //
    // Concerns:
    //:  The three functions that take an initializer lists (a constructor, an
    //:  assignment operator, and the 'insert' function) simply forward to
    //:  another already tested function.  We are interested here only in
    //:  ensuring that the forwarding is working -- not retesting already
    //:  functionality.
    //
    // Plan:
    //:
    //:
    // Testing:
    //   unordered_set(initializer_list, const A&);
    //   unordered_set(initializer_list, size_type, const A&);
    //   unordered_set(initializer_list, size_type, hasher, const A&);
    //   unordered_set(initializer_list, size_type, hasher, pred, const A&);
    //   unordered_set& operator=(initializer_list<value_type>);
    //   bsl::pair<iterator, bool> insert(initializer_list<value_type>);
    // -----------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const TestValues V;

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&da);

    if (verbose)
        printf("\tTesting constructor with initializer lists\n");
    {
        const struct {
            int                         d_line;    // source line number
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""    },
            { L_,   { V[0]             },  "A"   },
            { L_,   { V[0], V[0]       },  "A"   },
            { L_,   { V[1], V[0]       },  "AB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC" },
            { L_,   { V[0], V[1], V[0] },  "AB"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());
                ASSERT(HASH() == X.hash_function());
                ASSERT(EQUAL() == X.key_eq());
                ASSERT(X.bucket_count() < 100);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || 0 == ti) == (1 == numPasses));

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }
    {
        const struct {
            int                         d_line;    // source line number
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""    },
            { L_,   { V[0]             },  "A"   },
            { L_,   { V[0], V[0]       },  "A"   },
            { L_,   { V[1], V[0]       },  "AB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC" },
            { L_,   { V[0], V[1], V[0] },  "AB"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, 100, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());
                ASSERT(HASH() == X.hash_function());
                ASSERT(EQUAL() == X.key_eq());
                ASSERT(X.bucket_count() >= 100);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC == (1 == numPasses));

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }
    {
        const struct {
            int                         d_line;    // source line number
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""    },
            { L_,   { V[0]             },  "A"   },
            { L_,   { V[0], V[0]       },  "A"   },
            { L_,   { V[1], V[0]       },  "AB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC" },
            { L_,   { V[0], V[1], V[0] },  "AB"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, 100, HASH(9), xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());
                ASSERT(HASH(9) == X.hash_function());
                ASSERT(EQUAL() == X.key_eq());
                ASSERT(X.bucket_count() >= 100);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC == (1 == numPasses));

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }
    {
        const struct {
            int                         d_line;    // source line number
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""    },
            { L_,   { V[0]             },  "A"   },
            { L_,   { V[0], V[0]       },  "A"   },
            { L_,   { V[1], V[0]       },  "AB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC" },
            { L_,   { V[0], V[1], V[0] },  "AB"  },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ T_ Q(ExceptionTestBody) }

                Obj        mX(DATA[ti].d_list, 100, HASH(9), EQUAL(6), xoa);
                const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());
                ASSERT(HASH(9) == X.hash_function());
                ASSERT(EQUAL(6) == X.key_eq());
                ASSERT(X.bucket_count() >= 100);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC == (1 == numPasses));

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\tTesting 'operator=' with initializer lists\n");

    {
        const struct {
            int                         d_line;    // source line number
            const char                 *d_spec;    // target string
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  lhs    list                   result
            //----  ----   --------------------   ------
            { L_,   "",    {                  },  ""    },
            { L_,   "",    { V[0]             },  "A"   },
            { L_,   "A",   {                  },  ""    },
            { L_,   "A",   { V[1]             },  "B"   },
            { L_,   "A",   { V[0], V[1]       },  "AB"  },
            { L_,   "A",   { V[1], V[2]       },  "BC"  },
            { L_,   "AB",  {                  },  ""    },
            { L_,   "AB",  { V[0], V[1], V[2] },  "ABC" },
            { L_,   "AB",  { V[2], V[3], V[4] },  "CDE" },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            Obj mX(xoa); const Obj& X = gg(&mX, DATA[ti].d_spec);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = DATA[ti].d_list);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(Y,    X,    Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\tTesting 'insert' with initializer lists\n");

    {
        const struct {
            int                         d_line;    // source line number
            const char                 *d_spec;    // target string
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  lhs    list                   result
            //----  ----   --------------------   -------
            { L_,   "",    {                  },  ""      },
            { L_,   "",    { V[0]             },  "A"     },
            { L_,   "A",   {                  },  "A"     },
            { L_,   "A",   { V[0]             },  "A"     },
            { L_,   "A",   { V[1]             },  "AB"    },
            { L_,   "AB",  { V[0], V[1]       },  "AB"    },
            { L_,   "AB",  { V[1], V[2]       },  "ABC"   },
            { L_,   "AB",  { V[0], V[1], V[2] },  "ABC"   },
            { L_,   "AB",  { V[2], V[3], V[4] },  "ABCDE" },
        };

        enum { NUM_SPECS = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            Obj mX(xoa); const Obj& X = gg(&mX, DATA[ti].d_spec);

            mX.insert(DATA[ti].d_list);

            ASSERTV(X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }

#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase32_inline()
{
    // ------------------------------------------------------------------------
    // TESTING INITIALIZER LIST C'TORS INLINE
    //
    // Concerns:
    //: 1 That the initializer list constructors can work with an inline
    //:   initializer list.
    //
    // Plan:
    //: 1 Repeat the tests done in 'testCase32_outOfLine', except test only
    //:   with a single, inline constructor list.
    //: 2 There is a bug in the GNU compiler where, if an exception is thrown
    //:   while creating an inline initializer_list, the partially completely
    //:   list is not correctly destroyed and memory can be leaked.  So if
    //:   we're on that compiler, disable the default allocator from throwing
    //:   exceptions (inline initiailizer_list's are always created using the
    //:   default allocator).
    //
    // Testing:
    //   unordered_set(initializer_list);
    //   unordered_set(initializer_list, size_t);
    //   unordered_set(initializer_list, size_t, HASH);
    //   unordered_set(initializer_list, size_t, HASH, EQUAL);
    //   unordered_set(initializer_list, const A&);
    //   unordered_set(initializer_list, size_t, const A&);
    //   unordered_set(initializer_list, size_t, HASH, const A&);
    //   unordered_set(initializer_list, size_t, HASH, EQUAL, const A&);
    //   void insert(initializer_list);
    //   unordered_set& operator=(initializer_list);
    // ------------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    if (verbose) printf("INITIALIZER_LIST not supported -- no test: %s\n",
                        NameOf<KEY>().name());
#else
    if (verbose) printf(
                "TESTING INITIALIZER LIST C'TORS AND MANIPULATORS INLINE: %s\n"
                "=======================================================\n",
                NameOf<KEY>().name());

    int numIters = 0;

    bslma::TestAllocator ta("ta", veryVeryVeryVerbose);    // testValues
    bslma::TestAllocator fa("fa", veryVeryVeryVerbose);    // footprint
    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);    // other
    bslma::TestAllocator da("da", veryVeryVeryVerbose);    // default
    bslma::DefaultAllocatorGuard dag(&da);

    const char   testValuesSpec[] = { "ABC" };
    const size_t testValuesSpecLen = sizeof(testValuesSpec) - 1;

    TestValues  testValues(testValuesSpec, &ta);
    ASSERT(testValues.size() == testValuesSpecLen);
    for (size_t ii = 0; ii < testValuesSpecLen; ++ii, ++numIters) {
        ASSERTV(NameOf<KEY>(), ii, testValuesSpec[ii] ==
                                                      valueOf(testValues[ii]));
    }
    ASSERT(0 < numIters);

    const TestHashFunctor<KEY> hf(5);
    const TestEqualityComparator<KEY> ec(5);

    bool done = false;
    int totalThrows = 0;
    for (char ctor = 'a'; ctor <= 'h' ; ++ctor) {
        bool ibPassed = false;
        bool hfPassed = false;
        bool ecPassed = false;
        bool oaPassed = false;

        Obj *p = 0;

#define u_INIT_LIST { testValues[1], testValues[0], testValues[2] }

#if defined(BDE_BUILD_TARGET_EXC)
        bslma::TestAllocator& usedAlloc = ctor < 'e' ? da : oa;
#endif
        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(usedAlloc) {
            ++numThrows;

#if defined(BSLS_COMPILERFEATURES_INITIALIZER_LIST_LEAKS_ON_EXCEPTIONS)
            // There's a bug in 'std::initializer_list' in the GNU g++ compiler
            // which, if a throw occurs while the initializer list is being
            // constructed, it isn't destroyed properly and memory is leaked.
            // To avoid that, do this test without the exceptions if
            // '&usedAlloc == &da'.

            // Therefore, default allocator should never throw on allocations.

            da.setAllocationLimit(-1);
#endif

            switch (ctor) {
              case 'a': {
                  p = new (fa) Obj(u_INIT_LIST);
              } break;
              case 'b': {
                  p = new (fa) Obj(u_INIT_LIST, 1000);
                  ibPassed = true;
              } break;
              case 'c': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf);
                  ibPassed = true;
                  hfPassed = true;
              } break;
              case 'd': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf, ec);
                  ibPassed = true;
                  hfPassed = true;
                  ecPassed = true;
              } break;
              case 'e': {
                  p = new (fa) Obj(u_INIT_LIST, &oa);
                  oaPassed = true;
              } break;
              case 'f': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, &oa);
                  ibPassed = true;
                  oaPassed = true;
              } break;
              case 'g': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf, &oa);
                  ibPassed = true;
                  hfPassed = true;
                  oaPassed = true;
              } break;
              case 'h': {
                  p = new (fa) Obj(u_INIT_LIST, 1000, hf, ec, &oa);
                  ibPassed = true;
                  hfPassed = true;
                  ecPassed = true;
                  oaPassed = true;

                  done = true;
              } break;
              default: {
                  ASSERTV(ctor, 0 && "invalid ctor choice");
              } return;                                               // RETURN
            }

            ASSERT(p);
            ASSERT(!p->empty() && 3 == p->size());
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        const Obj& X = *p;

#if defined(BDE_BUILD_TARGET_EXC)
        ASSERTV(NameOf<KEY>(), numThrows, ctor, !oaPassed || 0 < numThrows);
#endif
        totalThrows += numThrows;

        // Make sure parameters either got passed or were default constructed
        // appropriately.

        const size_t bc = X.bucket_count();
        ASSERTV(bc, ibPassed, ibPassed ? (bc > 1000) : (bc < 1000));

        const int hfId = X.hash_function().id();
        ASSERTV(hfId, hfPassed, hfId == (hfPassed ? 5 : 0));

        const int ecId = X.key_eq().id();
        ASSERTV(ecId, ecPassed, ecId == (ecPassed ? 5 : 0));

        ASSERTV(X.get_allocator().mechanism() == (oaPassed ? &oa : &da));
        ASSERTV(oaPassed || 0 == oa.numAllocations());

        // Make sure the container works OK.

        ASSERTV(NameOf<KEY>(), X.size(), testValues.size() == X.size());
        ASSERTV(NameOf<KEY>(), 0 == verifyContainer(X,
                                                    testValues,
                                                    testValues.size()));

        ASSERTV(!oa.numBlocksInUse() != !da.numBlocksInUse());

        fa.deleteObjectRaw(p);

        ASSERTV(ctor, 0 == fa.numBlocksInUse());
        ASSERTV(ctor, 0 == oa.numBlocksInUse());
        ASSERTV(ctor, 0 == da.numBlocksInUse());
    }

    ASSERTV(fa.numBlocksTotal() == 'h' + 1 - 'a' + totalThrows);

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");

        TestValues  expectValues("ABCDE", &ta);

        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            mX.insert(u_INIT_LIST);

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERTV(!k_TYPE_ALLOC || !PLAT_EXC || 0 < numThrows);

        ASSERTV(0 == verifyContainer(X, expectValues, expectValues.size()));
    }

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");
        Obj Y(X, &oa);

        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            CompareProctor<Obj> proctor(Y, X);

            Obj *ret = &(mX = u_INIT_LIST);
            ASSERT(&X == ret);

            proctor.release();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERTV(!k_TYPE_ALLOC || !PLAT_EXC || 0 < numThrows);
        ASSERTV(X, 0 == verifyContainer(X, testValues, testValues.size()));
    }

    {
        struct MyPair {
            int d_first;
            Obj d_second;

            MyPair(int first, const Obj& second)
            : d_first(first)
            , d_second(second)
            {}
        };

        bslma::TestAllocator         ta("ta", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&ta);

        const MyPair& mp = MyPair(5, { u_INIT_LIST });

        ASSERT(3 == mp.d_second.size());
    }

#undef u_INIT_LIST

    ASSERT(done);
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase31b()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note tha only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase31'.
    //:
    //: 2 'emplace_hint' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase31b_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as arguments a pointer to a
    //:   modifiable container and a hint.
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on (first) template parameter indicating the number of args
    //:     to pass in, call 'emplace_hint' with the hint passed in as an
    //:     argument and the corresponding constructor argument values,
    //:     performing an explicit move of the argument if so indicated by
    //:     the template parameter corresponding to the argument, all of this
    //:     in the presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.  Otherwise, verify the return value is as expected.
    //:
    //:   7 Return the iterator returned by the call to 'emplace_hint'.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase31b_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //:
    // Testing:
    // iterator emplace_hint(const_iterator hint, Args&&... args);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    Iter hint;
#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=1\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31b_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, hint, true);
        hint = testCase31b_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint, true);
    }

    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31b_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint, true);
        hint = testCase31b_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint, true);
    }

    if (verbose) printf("\tTesting emplace_hint with 0 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
    }

    if (verbose) printf("\tTesting emplace_hint with 1 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31b_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, hint, false);
    }

    if (verbose) printf("\tTesting emplace_hint with 2 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31b_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31b_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31b_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, hint, false);
    }

    if (verbose) printf("\tTesting emplace_hint with 3 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31b_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31b_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31b_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31b_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, hint, false);
    }

    if (verbose) printf("\tTesting emplace_hint with 10 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end(), true);
        hint = testCase31b_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, hint, false);
        hint = testCase31b_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, hint, false);
        hint = testCase31b_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint, false);
    }
#else
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31b_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31b_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint, true);
        hint = testCase31b_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint, true);
        hint = testCase31b_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint, true);
    }
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase31a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note tha only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase31'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase31a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument a pointer
    //:   to a modifiable container.
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on (first) template parameter indicating the number of args
    //:     to pass in, call 'emplace' with the corresponding argument values,
    //:     performing an explicit move of the argument if so indicated by
    //:     the template parameter corresponding to the argument, all in the
    //:     presence of injected exceptions.
    //:
    //:   3 Verify that the argument values were passed correctly.
    //:
    //:   4 Verify that the allocator was forwarded correctly.
    //:
    //:   5 Verify that the move-state for each argument is as expected.
    //:
    //:   6 If the object did not contain the emplaced value, verify it now
    //:     exists.  Otherwise, verify the return value is as expected.
    //:
    //: 2 Create a container with it's own object-specific allocator.
    //:
    //: 3 Call 'testCase31a_RunTest' in various configurations:
    //:   1 For 1..10 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3, and 10 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //:
    //
    // Testing:
    // pair<iterator, bool> emplace(Args&&... args);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\tTesting emplace 1..10 args, move=1\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, true);
        testCase31a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, true);
        testCase31a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, true);
        testCase31a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, true);
    }

    if (verbose) printf("\tTesting emplace 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, true);
        testCase31a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, true);
        testCase31a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, true);
        testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, true);
    }

    if (verbose) printf("\tTesting emplace with 0 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
    }

    if (verbose) printf("\tTesting emplace with 1 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX,  true);
        testCase31a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, false);
    }

    if (verbose) printf("\tTesting emplace with 2 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX,  true);
        testCase31a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, false);
        testCase31a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, false);
        testCase31a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, false);
    }

    if (verbose) printf("\tTesting emplace with 3 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX,  true);
        testCase31a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, false);
        testCase31a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, false);
        testCase31a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, false);
        testCase31a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, false);
    }

    if (verbose) printf("\tTesting emplace with 10 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX,  true);
        testCase31a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, false);
        testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, false);
        testCase31a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, false);
    }
#else
    if (verbose) printf("\tTesting emplace 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, true);
        testCase31a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, true);
        testCase31a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, true);
        testCase31a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, true);
        testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, true);
    }
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    //: 1 'emplace' returns a pair containing an iterator and a 'bool';
    //    'emplace_hint' returns an iterator.
    //:
    //: 3 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and the existing element if it did.
    //:
    //: 4 The 'bool' returned by 'emplace' is 'true' if a new element is
    //:   inserted, and 'false' otherwise.
    //:
    //: 5 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' and
    //:   'emplace_hint' functions and will test proper forwarding of
    //:    constructor arguments in test 'testCase31a' and 'testCase31b',
    //:    respectively.
    //:
    //: 2 For emplace, we will create objects of varying sizes and capacities
    //:   containing default values, and emplace a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..5)
    //:
    //:   3 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 3 Repeat P-2, but this time calling 'emplace_hint' with a 'hint'
    //:   iterator that has been initialized to garbage and observe that it
    //:   makes no difference.
    //:
    //: 4 Repeat P-2 under the presence of exception  (C-7)
    //
    // Testing:
    //   bsl::pair<iterator, bool> emplace(Args&&... arguments);
    //   iterator emplace(const_iterator hint, Args&&... arguments);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
        const char *d_allocs;  // expected pool resizes
    } DATA[] = {
        //line  spec           isUnique       poolAlloc
        //----  -------------  -------------  -------------
        { L_,   "A",           "Y",           "+"           },
        { L_,   "AAA",         "YNN",         "++-"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY",    "++-+---+"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY", "++-+-------" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN",  "++---+----"  }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'emplace' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const char *const ALLOCS = DATA[ti].d_allocs;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj& X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ T_ P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                bsl::pair<Iter, bool> RESULT = mX.emplace(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    if ('+' == ALLOCS[tj]) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC <= AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 1              <=  A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC <= AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 0              <=  A);
                    }
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'emplace' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const char *const ALLOCS = DATA[ti].d_allocs;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj& X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ T_ P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                Iter hint;        // Give it a garbage value.
                memset((void *)&hint, 0xaf ^ (tj * 97), sizeof(hint));
                Iter RESULT = mX.emplace_hint(hint, VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    if ('+' == ALLOCS[tj]) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC <= AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 1              <=  A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC <= AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 0              <=  A);
                    }
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'emplace' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj& X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ T_ P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                bsl::pair<Iter, bool> RESULT;
                int                   numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    Iter hint;                  // Give it a garbage value.
                    memset((void *)&hint, 0xaf ^ (tj * 97), sizeof(hint));
                    if (tj & 1) {
                        RESULT.first  = mX.emplace_hint(hint, VALUES[tj]);
                        RESULT.second = IS_UNIQ;
                    }
                    else {
                        RESULT = mX.emplace(VALUES[tj]);
                    }
                proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                ASSERTV(LINE, tj, SIZE,
                            VALUES[tj] == *(RESULT.first));
                // The following line confirms that there is no exception
                // thrown when trying to 'insert' a value that's already there,
                // which is not true for 'emplace' because you have to create
                // the object first, which may throw.
                // ASSERTV(LINE, tj, IS_UNIQ, numPasses,
                //                                  IS_UNIQ || 1 == numPasses);

                if (IS_UNIQ) {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase30()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION ON MOVABLE VALUES
    //
    // Concerns:
    //: 1 'insert' returns a pair containing an iterator and a 'bool'.
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and the existing element if it did.
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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  -------------  -------------
        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj& X = mX;

                bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { T_ T_ T_ P(EXPECTED); }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));

                    bsl::pair<Iter, bool> RESULT = mX.insert(
                                                      MoveUtil::move(*valptr));

                    MoveState::Enum mState = TstFacility::getMovedFromState(
                                                                      *valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    const MoveState::Enum expMove =
                           k_KEY_IS_MOVE_ENABLED
                           ? ((k_KEY_IS_WELL_BEHAVED && &sa != &oa) || !IS_UNIQ
                              ? MoveState::e_NOT_MOVED
                              : MoveState::e_MOVED)
                           : MoveState::e_UNKNOWN;
                    ASSERTV(expMove, mState, NameOf<KEY>(), expMove == mState);

                    if (IS_UNIQ) {
                        ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                        ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        if (&sa == &oa) {
                            ASSERTV(LINE, CONFIG, tj, AA, BB,
                                    BB + TYPE_ALLOC == AA);
                        }
                        else {
                            ASSERTV(LINE, tj, AA, BB, BB == AA);
                        }
                        ASSERTV(LINE, tj, A,  B,  B  ==  A);
                        ASSERTV(LINE, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
    if (verbose) printf("\tTesting 'insert' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { T_ T_ T_ P(EXPECTED); }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter hint;        // Give it a garbage value.
                    memset((void *)&hint, 0xaf ^ (tj * 97), sizeof(hint));

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));

                    Iter RESULT = mX.insert(hint, MoveUtil::move(*valptr));

                    MoveState::Enum mState = TstFacility::getMovedFromState(
                                                                      *valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    const MoveState::Enum expMove =
                           k_KEY_IS_MOVE_ENABLED
                           ? ((k_KEY_IS_WELL_BEHAVED && &sa != &oa) || !IS_UNIQ
                              ? MoveState::e_NOT_MOVED
                              : MoveState::e_MOVED)
                           : MoveState::e_UNKNOWN;
                    ASSERTV(expMove, mState, NameOf<KEY>(), expMove == mState);

                    if (IS_UNIQ) {
                        ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                        ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        if (&sa == &oa) {
                            ASSERTV(LINE, CONFIG, tj, AA, BB,
                                    BB + TYPE_ALLOC == AA);
                        }
                        else {
                            ASSERTV(LINE, tj, AA, BB, BB == AA);
                        }
                        ASSERTV(LINE, tj, A,  B,  B  ==  A);
                        ASSERTV(LINE, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj& X = mX;

                bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                bslma::TestAllocator *valAllocator = 0;
                switch (cfg) {
                  case 'a': {
                    // inserted value has same allocator
                    valAllocator = &oa;
                  } break;
                  case 'b': {
                    // inserted value has different allocator
                    valAllocator = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { T_ T_ T_ P(EXPECTED); }
                    }

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));
                    bslma::DestructorGuard<ValueType> guard(valptr);

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);

                    bsl::pair<Iter, bool> RESULT;
                    int                   numPasses = 0;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;
                        Obj mZ(xscratch);   const Obj& Z = mZ;
                        for (int tk = 0; tk < tj; ++tk) {
                            primaryManipulator(
                                        &mZ,
                                        TstFacility::getIdentifier(VALUES[tk]),
                                        xscratch);
                        }
                        ASSERTV(Z, X, Z == X);
                        ExceptionProctor<Obj, ALLOC> proctor(
                                                           &X,
                                                           L_,
                                                           MoveUtil::move(mZ));

                        Iter hint;                  // Give it a garbage value.
                        memset((void *)&hint, 0xaf ^ (tj * 97), sizeof(hint));
                        if (tj & 1) {
                            RESULT.first =  mX.insert(hint,
                                                      MoveUtil::move(*valptr));
                            RESULT.second = IS_UNIQ;
                        }
                        else {
                            RESULT = mX.insert(MoveUtil::move(*valptr));
                        }
                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));
                    ASSERTV(LINE, tj, IS_UNIQ, numPasses,
                            IS_UNIQ || 1 == numPasses);

                    if (IS_UNIQ) {
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X, exp, SIZE + 1));
                    }
                    else {
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                    }
                }
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::
                   testCase29_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                                           StdAlloc;

    typedef bsl::unordered_set<KEY, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

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

        const Obj W(IVALUES.begin(), IVALUES.end(),
                    1, HASH(), EQUAL(), scratch);    // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj mY(IVALUES.begin(), IVALUES.end(),
                       1, HASH(), EQUAL(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj mX(JVALUES.begin(), JVALUES.end(),
                       1, HASH(), EQUAL(), mat);
                const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = MoveUtil::move(mY));

                ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
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
                    ASSERTV(ISPEC, JSPEC, !oasm.isInUseUp());
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
                            testCase29_propagate_on_container_move_assignment()
{
    // ------------------------------------------------------------------------
    // MOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    //: 1 If the 'propagate_on_container_move_assignment' trait is 'false', the
    //:   allocator used by the target object remains unchanged (i.e., the
    //:   source object's allocator is *not* propagated).
    //:
    //: 2 If the 'propagate_on_container_move_assignment' trait is 'true', the
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
    //: 5 The effect of the 'propagate_on_container_move_assignment' trait is
    //:   independent of the other three allocator propagation traits.
    //
    // Plan:
    //: 1 Specify a set S of object values with varied differences, ordered by
    //:   increasing length, to be used in the following tests.
    //:
    //: 2 Create two 'bsltf::StdStatefulAllocator' objects with their
    //:   'propagate_on_container_move_assignment' property configured to
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
    //:   3 Move-assign 'Y' to 'X' and use 'operator==' to verify that 'X'
    //:     subsequently has the same value as 'W'.
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
    //   propagate_on_container_move_assignment
    // ------------------------------------------------------------------------

    if (verbose) printf("\nMOVE-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION"
                        "\n===============================================\n");

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == false'\n");

    testCase29_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase29_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == true'\n");

    testCase29_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase29_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //  TBD: the test does not yet cover the case where allocator propagation
    //       is enabled for move assignment (hard-coded to 'false') -- i.e.,
    //       parts of C-5..6 are currently not addressed.
    //
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 If allocator propagation is not enabled for move-assignment, the
    //:   allocator address held by the target object is unchanged; otherwise,
    //:   the allocator address held by the target object is changed to that of
    //:   the source.
    //:
    //: 6 If allocator propagation is enabled for move-assignment, any memory
    //:   allocation from the original target allocator is released after the
    //:   operation has completed.
    //:
    //: 7 All elements in the target object are either move-assigned to or
    //:   destroyed.
    //:
    //: 8 The source object is left in a valid state but unspecified state; the
    //:   allocator address held by the original object is unchanged.
    //:
    //: 9 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Assigning a source object having the default-constructed value
    //:   allocates no memory; assigning a value to a target object in the
    //:   default state does not allocate or deallocate any memory.
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 Any memory allocation is exception neutral.
    //:
    //:13 Assigning an object to itself behaves as expected (alias-safety).
    //:
    // Plan:
    //
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment, with its
    //:   own scratch allocator.
    //:
    //: 3 Iterate again over the same set of object values and create a
    //:   object representing the target of the assignment, with its own unique
    //:   object allocator.
    //:
    //: 4 In a loop consisting of two iterations, create a source object (a
    //:   copy of the control object in P-1) with 1) a different allocator than
    //:   that of target and 2) the same allocator as that of the target,
    //:
    //: 5 Call the move-assignment operator in the presence of exceptions
    //:   during memory allocations (using a 'bslma::Allocator' and varying
    //:   its allocation limit) and verify the following:                (C-12)
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 CONTAINER SPECIFIC NOTE: Ensure that the comparator was assigned.
    //:
    //:   4 If the source and target objects use the same allocator, ensure
    //:     that there is no net increase in memory use from the common
    //:     allocator.  Also consider the following cases:
    //:
    //:     1 If the source object is empty, confirm that there are no bytes
    //:       currently in use from the common allocator.                (C-10)
    //:
    //:     2 If the target object is empty, confirm that there was no memory
    //:       change in memory usage.                                    (C-10)
    //:
    //:   5 If the source and target objects use different allocators, ensure
    //:     that each element in the source object is move-inserted into the
    //:     target object.                                                (C-7)
    //:
    //:   6 Ensure that the source, target, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                        (C-4)
    //:
    //:   7 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.                 (C-8)
    //:
    //:   8 Verify all memory is released when the source and target objects
    //:     are destroyed.                                               (C-11)
    //:
    //: 6 Use a test allocator installed as the default allocator to verify
    //:   that no memory is ever allocated from the default allocator.
    //
    // Testing:
    //   unordered_set& operator=(bslmf::MovableRef<unordered_set> rhs);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues VALUES;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    // Create first object
    if (verbose)
        printf("\tTesting move assignment.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const char *const RESULT1 = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(RESULT1);

            if (5 < LENGTH1 && NUM_DATA-1 != ti) {
                continue;
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj  mZZ(xscratch); const Obj&  ZZ = gg(&mZZ, SPEC1);

            const bsls::Types::Int64 BEFORE = scratch.numBytesInUse();

            Obj mZZZ(xscratch); const Obj& ZZZ = gg(&mZZZ, SPEC1);

            const bsls::Types::Int64 BIU = scratch.numBytesInUse() - BEFORE;

            if (veryVerbose) { T_ T_ P_(LINE1) P_(ZZZ) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.
            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), ZZZ, Obj() == ZZZ);
                firstFlag = false;
            }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const char *const RESULT2 = DATA[tj].d_results;
                const size_t      LENGTH2 = strlen(RESULT2);

                if (5 < LENGTH2 && NUM_DATA-1 != tj) {
                    continue;
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);

                    KeyAllocator        xoa(&oa);
                    KeyAllocator        xda(&da);

                    Obj        *objPtr = new (fa) Obj(xoa);
                    Obj&        mX = *objPtr;
                    const Obj&  X = gg(&mX, SPEC2);

                    Obj                  *srcPtr = 0;
                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (fa) Obj(xda); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        srcPtr = new (fa) Obj(xoa); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }

                    Obj& mZ = *srcPtr;      const Obj& Z = mZ;

                    bslma::TestAllocator& sa = *srcAllocatorPtr;
                    KeyAllocator          xsa(&sa);

                    if (veryVerbose) { T_ T_ P_(LINE2) P(Z) }
                    if (veryVerbose) { T_ T_ P_(LINE2) P(X) }

                    ASSERTV(SPEC1, SPEC2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bool empty = 0 == ZZ.size();

                    typename Obj::const_pointer pointers[2];
                    storeFirstNElemAddr(pointers,
                                        Z,
                                        sizeof pointers / sizeof *pointers);

                    bslma::TestAllocatorMonitor oam(&oa);
                    bslma::TestAllocatorMonitor dam(&da);

                    Obj *mR = &(mX = MoveUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    if (0 == LENGTH1) {
                        // assigned an empty set
                        ASSERTV(SPEC1, SPEC2, oa.numBytesInUse(),
                                0 == oa.numBytesInUse());
                    }

                    // CONTAINER SPECIFIC NOTE: For 'set', the original object
                    // is left in the default state even when the source and
                    // target objects use different allocators because
                    // move-insertion changes the value of the source key and
                    // violates the uniqueness requirements for keys contained
                    // in the 'set'.

                    if (&sa == &oa) {
                        // same allocator
                        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                        if (0 == LENGTH2) {
                            // assigning to an empty set
                            ASSERTV(SPEC1, SPEC2, oam.isInUseSame());
                            ASSERTV(SPEC1, SPEC2, BIU, oa.numBytesInUse(),
                                    BIU == oa.numBytesInUse());
                        }

                        // 2. unchanged address of contained element(s)
                        ASSERT(0 == checkFirstNElemAddr(
                                          pointers,
                                          X,
                                          sizeof pointers / sizeof *pointers));

                        // 3. original object left empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 4. nothing from the other allocator
                        ASSERTV(SPEC1, SPEC2, 0 == da.numBlocksTotal());
                    }
                    else {
                        // 1. each element in original move-inserted
                        ASSERTV(SPEC1,
                                SPEC2,
                                X.end() == TstMovUtil::findFirstNotMovedInto(
                                                                     X.begin(),
                                                                     X.end()));

                        // 2. CONTAINER SPECIFIC NOTE: original object left
                        // empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 3. additional memory checks
                        ASSERTV(SPEC1, SPEC2, &sa == &oa,
                                empty || oam.isTotalUp());
                        ASSERTV(SPEC1, SPEC2, 0 == dam.isInUseUp());

                    }
                    // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                    ASSERTV(SPEC1, SPEC2, &scratch == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      &oa ==  X.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      &sa ==  Z.get_allocator());

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.
                    pair<Iter, bool> RESULT =
                                             primaryManipulator(&mZ, 'Z', xsa);
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    fa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            empty ||
                                  ((&sa == &oa) == (0 < sa.numBlocksInUse())));

                    // Verify subsequent manipulation of target object 'X'.
                    RESULT = primaryManipulator(&mX, 'Z', xoa);
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                            LENGTH1 + 1 == X.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    fa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            0 == oa.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                Obj mX(xoa);  const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(xscratch);  const Obj& ZZ  = gg(&mZZ,  SPEC1);

                const Obj& Z = mX;

                ASSERTV(SPEC1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = MoveUtil::move(mX));
                    ASSERTV(SPEC1, ZZ,   Z, ZZ == Z);
                    ASSERTV(SPEC1, mR,  &X, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(SPEC1, &oa == Z.get_allocator());

                ASSERTV(SPEC1, sam.isTotalSame());
                ASSERTV(SPEC1, oam.isTotalSame());

                ASSERTV(SPEC1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(SPEC1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
    if (verbose)
        printf("\tTesting move assignment with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const char *const RESULT1 = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(RESULT1);

            if (4 < LENGTH1 && NUM_DATA-1 != ti) {
                continue;
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj  mZZ(xscratch); const Obj&  ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ T_ P_(LINE1) P(ZZ) }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const char *const RESULT2 = DATA[tj].d_results;
                const size_t      LENGTH2 = strlen(RESULT2);

                if (4 < LENGTH2 && NUM_DATA-1 != tj) {
                    continue;
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ T_ Q(ExceptionTestBody) }

                        const bsls::Types::Int64 AL = oa.allocationLimit();
                        oa.setAllocationLimit(-1);
                        Obj mX(xoa); const Obj& X = gg(&mX, SPEC2);

                        bslma::TestAllocator *srcAllocatorPtr;
                        switch (CONFIG) {
                          case 'a': {
                            srcAllocatorPtr = &da;
                          } break;
                          case 'b': {
                            srcAllocatorPtr = &oa;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"Bad allocator config.");
                          } return;                                   // RETURN
                        }
                        bslma::TestAllocator& sa = *srcAllocatorPtr;
                        KeyAllocator          xsa(&sa);

                        Obj mZ(xsa); const Obj& Z = gg(&mZ, SPEC1);

                        if (veryVerbose) { T_ T_ P_(LINE2) P(Z) }
                        if (veryVerbose) { T_ T_ P_(LINE2) P(X) }

                        ASSERTV(SPEC1, SPEC2, Z, X,
                                (Z == X) == (INDEX1 == INDEX2));

                        typename Obj::const_pointer pointers[2];
                        storeFirstNElemAddr(pointers,
                                            Z,
                                            sizeof pointers /sizeof *pointers);

                        Obj mE(xscratch);
                        ExceptionProctor<Obj, ALLOC> proctor(
                                                   &Z, L_, MoveUtil::move(mE));

                        oa.setAllocationLimit(AL);

                        Obj *mR = &(mX = MoveUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                        proctor.release();

                        // Manipulate source object 'Z' to ensure it is in a
                        // valid state and is independent of 'X'.
                        pair<Iter, bool> RESULT = primaryManipulator(&mZ,
                                                                     'Z',
                                                                     xsa);
                        ASSERTV(true == RESULT.second);
                        ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                        ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                        // Verify subsequent manipulation of target object 'X'.
                        RESULT = primaryManipulator(&mX, 'Z', xoa);
                        ASSERTV(true == RESULT.second);
                        ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                                LENGTH1 + 1 == X.size());
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERTV(0 == da.numAllocations());
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase28()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR:
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The allocator is propagated to the newly created object if (and only
    //:   if) no allocator is specified in the call to the move constructor.
    //:
    //: 4 A constant-time move, with no additional memory allocations or
    //:   deallocations, is performed when no allocator or the same allocator
    //:   as that of the original object is passed to the move constructor.
    //:
    //: 5 A linear operation, where each element is move-inserted into the
    //:   newly created object, is performed when a '0' or an allocator that is
    //:   different than that of the original object is explicitly passed to
    //:   the move constructor.
    //:
    //: 6 The original object is always left in a valid state; the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 7 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 8 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    //:10 Any memory allocation is exception neutral.
    //:
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator: 1) no allocator passed in, 2) a '0' is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //:   as that of the original object is explicitly passed in, and 4) a
    //:   different allocator than that of the original object is passed in.
    //:
    //: 3 For each of the object values (P-1) and for each configuration (P-2),
    //:   verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 CONTAINER SPECIFIC NOTE: Ensure that the comparator was copied.
    //:
    //:   3 Where a constant-time move is expected, ensure that no memory was
    //:     allocated, that element addresses did not change, and that the
    //:     original object is left in the default state.         (C-3..5, C-7)
    //:
    //:   4 Where a linear-time move is expected, ensure that the move
    //:     constructor was called for each element.                   (C-6..7)
    //:
    //:   5 CONTAINER SPECIFIC:
    //:     Where a linear-time move is expected, the value of the original
    //:     object is also left in the default state because move insertion
    //:     changes the value of the original key object and violates the class
    //:     invariant enforcing uniqueness of contained keys.             (C-7)
    //:
    //:   6 Ensure that the new original, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                    (C-3,C-9)
    //:
    //:   7 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state, destroy it, and then manipulate the
    //:     newly created object to ensure that it is in a valid state.   (C-8)
    //:
    //:   8 Verify all memory is released when the object is destroyed.  (C-11)
    //;
    //: 4 Perform tests in the presence of exceptions during memory allocations
    //:   using a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:                                                                  (C-10)
    //
    // Testing:
    //   unordered_set(bslmf::MovableRef<unordered_set> original);
    //   unordered_set(bslmf::MovableRef<unordered_set> original, const A&);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

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

    if (verbose)
        printf("\tTesting both versions of move constructor.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tFor an object of length " ZU ":\n", LENGTH);
                T_ T_ P(SPEC);
            }

            // Create control object ZZ with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mZZ(xscratch);    const Obj& ZZ = gg(&mZZ, SPEC);

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(ti, Obj(), ZZ, Obj() == ZZ);
                firstFlag = false;
            }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                KeyAllocator        xda(&da);
                KeyAllocator        xsa(&sa);
                KeyAllocator        xza(&za);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Z'.
                Obj        *srcPtr = new (fa) Obj(xsa);
                Obj&        mZ = *srcPtr;
                const Obj&  Z = gg(&mZ, SPEC);

                typename Obj::const_pointer pointers[2];
                storeFirstNElemAddr(pointers,
                                    Z,
                                    sizeof pointers / sizeof *pointers);

                bslma::TestAllocatorMonitor oam(&da);
                bslma::TestAllocatorMonitor sam(&sa);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                bslma::TestAllocator *othAllocatorPtr;

                bool empty = 0 == ZZ.size();

                switch (CONFIG) {
                  case 'a': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ));
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    oam.reset(&da);
                    KeyAllocator        sdc;
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), sdc);
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), xsa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), xza);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator& oa  = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;
                KeyAllocator          xoa(&oa);

                Obj& mX = *objPtr;      const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // Verify that the hash function and the key equivalence
                // predicate were copied.
                ASSERTV(SPEC, CONFIG,
                        X.hash_function().id() == Z.hash_function().id());
                ASSERTV(SPEC, CONFIG, X.key_eq().id() == Z.key_eq().id());

                // CONTAINER SPECIFIC NOTE: For 'set', the original object is
                // left in the default state even when the source and target
                // objects use different allocators because move-insertion
                // changes the value of the source key and violates the
                // uniqueness requirements for keys contained in the 'set'.

                if (&sa == &oa) {
                    // 1. no memory allocation
                    ASSERTV(SPEC, CONFIG, &sa == &oa, oam.isTotalSame());
                    // 2. unchanged address of contained element(s)
                    ASSERT(0 == checkFirstNElemAddr(pointers, X,
                                          sizeof pointers / sizeof *pointers));
                    // 3. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());
                }
                else {
                    // 1. each element in original move-inserted
                    ASSERTV(SPEC, X.end() ==
                        TstMovUtil::findFirstNotMovedInto(X.begin(), X.end()));
                    // 2. original object left empty
                    ASSERTV(SPEC, CONFIG, &sa == &oa, Z, 0 == Z.size());
                    // 3. additional memory checks
                    ASSERTV(SPEC, CONFIG, &sa == &oa, oam.isTotalUp() ||empty);
                }

                // Verify that 'X', 'Z', and 'ZZ' have the correct allocator.
                ASSERTV(SPEC, CONFIG, &scratch == ZZ.get_allocator());
                ASSERTV(SPEC, CONFIG,      &sa ==  Z.get_allocator());
                ASSERTV(SPEC, CONFIG,      &oa ==  X.get_allocator());

                // Verify no allocation from the non-object allocator and that
                // object allocator is hooked up.
                ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || empty);

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.
                bsl::pair<Iter, bool> RESULT =
                               primaryManipulator(&mZ, 'Z', xsa);
                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.
                RESULT = primaryManipulator(&mX, 'Z', xoa);
                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, LENGTH + 1 == X.size());
                ASSERTV(SPEC, X != ZZ);

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.
                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }
    if (verbose)
        printf("\tTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mZZ(xscratch);    const Obj& ZZ = gg(&mZZ, SPEC);

            if (verbose) {
                printf("\t\tFor an object of length " ZU ":\n", LENGTH);
                T_ T_ P(SPEC);
            }

            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator da("different", veryVeryVeryVerbose);

            KeyAllocator        xoa(&oa);
            KeyAllocator        xda(&da);

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mE(xscratch);
                Obj mZ(xda);  const Obj& Z = gg(&mZ, SPEC);
                ExceptionProctor<Obj, ALLOC> proctor(&Z,
                                                     L_,
                                                     MoveUtil::move(mE));

                const Obj X(MoveUtil::move(mZ), xoa);
                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }
                ASSERTV(SPEC, X, ZZ, ZZ == X);
                ASSERTV(SPEC, 0 == Z.size());
                ASSERTV(SPEC, Z.get_allocator() != X.get_allocator());

                proctor.release();

                // Manipulate source object 'Z' to ensure it is in a
                // valid state and is independent of 'X'.
                pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'Z', xda);

                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, Z, 1 == Z.size());
                ASSERTV(SPEC, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, X, ZZ, X == ZZ);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const bsls::Types::Int64 AA = oa.numBlocksTotal();
            const bsls::Types::Int64 A  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
            }

            if (LENGTH == 0) {
                ASSERTV(SPEC, BB + 0 == AA);
                ASSERTV(SPEC,  B + 0 ==  A);
            }
            else {
                // TBD: calculating the expected # of total allocations is
                // non-trivial (as compared to say a 'set'); let's think about
                // a simpler test.
                ASSERTV(SPEC, B + 0 == A);
            }
        }
    }
#endif // BDE_BUILD_TARGET_EXC
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase27()
{
    // ------------------------------------------------------------------------
    // TESTING SPREAD
    //
    // Concerns:
    //   That the hash table isn't unintentionally funneling all nodes into
    //   a few buckets.
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    TestValues VALUES;

    if (verbose) printf("\tTest spread\n");
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

    if (verbose) printf("\tTest non-spread with delinquent hash function\n");
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    typedef typename Obj::local_iterator       LIter;
    typedef typename Obj::const_local_iterator CLIter;

    BSLMF_ASSERT((bslmf::IsSame<LIter, CLIter>::value));

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    TestValues VALUES;

    const KEY kA(VALUES[0]);

    Obj mX;     const Obj& X = mX;

    ASSERTV(X.begin(0) ==  X.end(0));
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
            size_t      b = X.bucket(kA);
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
            LIter  end = X.end(ti);
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
            const  LIter end = X.end(b);
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
        size_t emptyCount = 0;
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
            LIter  end = X.end(ti);
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
            const  LIter end = X.end(b);
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
        size_t emptyCount = 0;
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
            LIter  end = X.end(ti);
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
            const  LIter end = X.end(b);
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
        size_t emptyCount = 0;
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
            LIter  end = X.end(ti);
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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    HASH  defaultHash;
    EQUAL defaultEqual;

    const char *SPEC, *lastSpec = "+";
    for (int ti = 0; ti < NUM_DATA; ++ti, lastSpec = SPEC) {
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
            Obj        mX(values.begin(),
                          values.end(),
                          0,
                          defaultHash,
                          defaultEqual,
                          &oa);
            const Obj& X = mX;
            values.resetIterators();
            Obj        mY(X);
            const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &sc);
                ASSERTV(COUNT == X.bucket_count());

                mX.reserve(my_max<size_t>(X.size(), 1) * 3);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        {
            Obj        mX(values.begin(),
                          values.end(),
                          0,
                          defaultHash,
                          defaultEqual,
                          &oa);
            const Obj& X = mX;
            values.resetIterators();
            Obj        mY(X);
            const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &sc);

                mX.rehash(X.bucket_count() + 1);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV(!PLAT_EXC || numPasses > 1);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        if (values.size() > 0) {
            Obj        mX(values.begin(),
                          values.end(),
                          0,
                          defaultHash,
                          defaultEqual,
                          &oa);
            const Obj& X = mX;
            values.resetIterators();
            Obj        mY(X);
            const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();
            const float  LOAD  = X.load_factor();

            double actualLoad = static_cast<double>(X.size())
                                       / static_cast<double>(X.bucket_count());

            ASSERTV(LOAD, actualLoad, nearlyEqual<double>(LOAD, actualLoad));
            ASSERTV(1.0f == X.max_load_factor());

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                ExceptionProctor<Obj, ALLOC> proctor(&X, L_, &sc);

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
    // TESTING STANDARD INTERFACE COVERAGE
    //
    // Concern:
    //   That all functions defined by the standard are at least defined.
    //
    // Plan:
    //   Take pointers to functions and verify they're non-zero.
    // ------------------------------------------------------------------------

    // TBD: For C++11, fix up all places where 'C++11' appears in comments
    // below.
    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose) printf("\tEstablish types exist\n");
    {
        typedef typename Obj::key_type             Kt;
        typedef typename Obj::value_type           Vt;
        typedef typename Obj::hasher               Ht;
        typedef typename Obj::key_equal            Eq;
        typedef typename Obj::allocator_type       Al;
        typedef typename Obj::pointer              Pt;
        typedef typename Obj::const_pointer        Cpt;
        typedef typename Obj::reference            Ref;
        typedef typename Obj::const_reference      Cref;
        typedef typename Obj::size_type            St;
        typedef typename Obj::iterator             It;
        typedef typename Obj::const_iterator       CIt;
        typedef typename Obj::local_iterator       Lit;
        typedef typename Obj::const_local_iterator CLit;

        BSLMF_ASSERT((bslmf::IsSame< KEY,        Kt   >::value));
        BSLMF_ASSERT((bslmf::IsSame< Kt,         Vt   >::value));
        BSLMF_ASSERT((bslmf::IsSame< HASH,       Ht   >::value));
        BSLMF_ASSERT((bslmf::IsSame< EQUAL,      Eq   >::value));
        BSLMF_ASSERT((bslmf::IsSame< ALLOC,      Al   >::value));
        BSLMF_ASSERT((bslmf::IsSame< Kt *,       Pt   >::value));
        BSLMF_ASSERT((bslmf::IsSame< const Kt *, Cpt  >::value));
        BSLMF_ASSERT((bslmf::IsSame< Kt&,        Ref  >::value));
        BSLMF_ASSERT((bslmf::IsSame< const Kt&,  Cref >::value));
        BSLMF_ASSERT((bslmf::IsSame< size_t,     St   >::value));
        BSLMF_ASSERT((bslmf::IsSame< It,         CIt  >::value));
        BSLMF_ASSERT((bslmf::IsSame< Lit,        CLit >::value));

    }

    if (verbose) printf("\tEstablishing constructors existence\n");

    const typename Obj::size_type zero = 0;

    {
        Obj mX;
        (void) mX;
    }

    {
        Obj mX(zero);
        (void) mX;
    }

    {
        Obj mX(zero, HASH());
        (void) mX;
    }

    {
        Obj mX(zero, HASH(), EQUAL());
        (void) mX;
    }

    {
        Obj mX(zero, HASH(),EQUAL(), ALLOC());
        (void) mX;
    }

    TestValues VALUES;

    {
        Obj mX(VALUES.begin(), VALUES.end());
        (void) mX;
    }

    VALUES.resetIterators();

    {
        Obj mX(VALUES.begin(), VALUES.end(), zero);
        (void) mX;
    }

    VALUES.resetIterators();

    {
        Obj mX(VALUES.begin(), VALUES.end(), zero, HASH());
        (void) mX;
    }

    VALUES.resetIterators();

    {
        Obj mX(VALUES.begin(), VALUES.end(), zero, HASH(), EQUAL());
        (void) mX;
    }

    VALUES.resetIterators();

    {
        Obj mX(VALUES.begin(), VALUES.end(), zero, HASH(), EQUAL(), ALLOC());
        (void) mX;
    }

    {
        Obj        mX;
        const Obj& X = mX;
        Obj        mY(X);
        (void) mY;
    }

    // unordered_set(unordered_set&&);    // <- C++11

    {
        typename Obj::allocator_type a;
        Obj mX(a);
        (void) mX;
    }

    // ~unordered_set();                  // Tested by ALL the above

    if (verbose) printf("\tEstablishing member functions existence\n");

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
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
            (void) [](const Obj& lhs, const Obj& rhs) -> bool {
                return lhs != rhs;
            };
#else
            fp = &operator!=;
#endif
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::unordered_set<KEY, HASH, EQUAL, StlAlloc> ObjStlAlloc;

    StlAlloc scratch;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_results;
        const ptrdiff_t   LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results, scratch);
        ASSERT(0 <= LENGTH);

        TestValues CONT(SPEC, scratch);

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
    //: 7 Functor with a non-'const' function call operator can be used -- any
    //:   non-'const' operation on the set that utilizes the comparator can be
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
    //:     passing in the comparator created in P-5.1.  Verify that the
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
    //: 6 Repeat P-5 except with a comparator having non-'const' function call
    //:   operator.  (P-7
    //
    // Testing:
    //   key_compare key_comp() const;
    //   value_compare value_comp() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    {
        bsl::unordered_set<int> X;
        bsl::hash<int>          hsh = X.hash_function();
        bsl::equal_to<int>      eq  = X.key_eq();

        (void) hsh;    // quash potential compiler warning
        (void) eq;     // quash potential compiler warning
    }

    static const int ID[] = { 0, 1, 2 };
    enum { NUM_ID = sizeof ID / sizeof *ID };

    for (int ti = 0; ti < NUM_ID; ++ti) {
        const int   hId = ti;
        const int   eId = ti * 7 + 5;
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
        int         d_line;  // source line number
        const char *d_spec;  // spec
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ACBD"      },
        { L_,  "BCDAE"     },
        { L_,  "GFEDCBA"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "BCDEFGHIA" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const HASH DH(0, false);    // delinquent hash, always returns 0
    const HASH H;               // normal hash

    const EQUAL E;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);
        const TestValues  EXP(DATA[ti].d_spec, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
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

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);
        const TestValues  EXP(DATA[ti].d_spec, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TestNonConstHashFunctor<KEY>        NCH;
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    bslma::TestAllocator  oa(veryVeryVerbose);

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj   X;
        ALLOC a;
        ASSERTV(~(size_t)0 / sizeof(KEY) >= X.max_size());
        ASSERTV(a.max_size(), X.max_size(),
                a.max_size() ==  X.max_size());
    }

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ABCD"      },
        { L_,  "ABCDE"     },
        { L_,  "ABCDEFG"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "ABCDEFGHI" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());
            ASSERTV(LINE, SPEC, (0 == ti) == bsl::empty(X));

            mX.clear();

            ASSERTV(LINE, SPEC, true == X.empty());
            ASSERTV(LINE, SPEC, true == bsl::empty(X));
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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose)printf("\tTesting 'erase(pos)' on non-empty unordered_set.\n");
    {
        for (int ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_results;
            const size_t      LENGTH = strlen(DATA[ti].d_results);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, xoa); const Obj& Y = mY;

                Iter pos    = getIterForIndex(X, tj);
                Iter before = 0 == tj ? mX.end()
                                      : getIterForIndex(X, tj - 1);
                Iter after = pos;
                ++after;

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

    if (verbose)printf("\tTesting 'erase(key)' on non-empty unordered_set.\n");
    {
        for (int ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_results;
            const size_t      LENGTH = strlen(DATA[ti].d_results);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, xoa); const Obj& Y = mY;

                Iter      pos    = getIterForIndex(X, tj);
                Iter      before = 0 == tj ? mX.end()
                                           : getIterForIndex(X, tj - 1);
                Iter      after = pos;
                const KEY k(*pos);

                while (mX.end() != after && k == *after) {
                    ++after;
                }

                const size_t COUNT = X.count(k);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&oa);

                size_t RESULT;
                int    numPasses = 0;
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

    if (verbose) printf("\tTesting 'erase(first, last)'.\n");
    {
        for (int ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_results;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0;  tj <= LENGTH; ++tj) {
                for (size_t tk = tj; tk <= LENGTH; ++tk) {
                    bslma::TestAllocator oa("object", veryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    Obj mX(xoa);    const Obj& X = gg(&mX, SPEC);
                    Obj mY(X, xoa); const Obj& Y = mY;

                    CIter start  = getIterForIndex(X, tj);
                    CIter finish = getIterForIndex(X, tk);

                    Obj        mZ(start, finish, 2, HASH(), EQUAL(), xoa);
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
                    int   numPasses = 0;
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

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        {
            const TestValues VALUES;

            Obj  mX;
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
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

            bslma::TestAllocator oa("object", veryVeryVerbose);
            KeyAllocator         xoa(&oa);

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, 2, HASH(), EQUAL(), xoa);  const Obj& X = mX;

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
    //:   not already exists, and the existing element if it did.
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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  -------------  -------------
        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVerbose) {T_ T_ P(EXPECTED); }
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
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB == AA);
                    ASSERTV(LINE, tj, A,  B,  B  ==  A);
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'insert' with hint.\n");
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
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

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
                memset((void *)&hint, 0xaf ^ (tj * 97), sizeof(hint));
                Iter RESULT = mX.insert(hint, VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB == AA);
                    ASSERTV(LINE, tj, A,  B,  B  ==  A);
                    ASSERTV(LINE, tj, SIZE == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int) strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVerbose) { T_ T_ P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                bsl::pair<Iter, bool> RESULT;
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    Iter hint;                  // Give it a garbage value.
                    memset((void *)&hint, 0xaf ^ (tj * 97), sizeof(hint));
                    if (tj & 1) {
                        RESULT.first  = mX.insert(hint, VALUES[tj]);
                        RESULT.second = IS_UNIQ;
                    }
                    else {
                        RESULT = mX.insert(VALUES[tj]);
                    }
                proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));
                ASSERTV(LINE, tj, IS_UNIQ, numPasses,
                        IS_UNIQ || 1 == numPasses);

                if (IS_UNIQ) {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
                else {
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE));
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    BSLMF_ASSERT((bsl::is_same<Iter, CIter>::value));

    const TestValues VALUES;

    bslma::TestAllocator oa(veryVeryVerbose);

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial spec
    } DATA[] = {
        //line spec
        //---- -----------
        { L_,  ""          },
        { L_,  "A"         },
        { L_,  "ABC"       },
        { L_,  "ACBD"      },
        { L_,  "BCDAE"     },
        { L_,  "GFEDCBA"   },
        { L_,  "ABCDEFGH"  },
        { L_,  "BCDEFGHIA" }
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

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
                Obj    mY(&oa);
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
    // TESTING FIND, CONTAINS, EQUAL_RANGE, COUNT
    //
    // Concern:
    //: 1 If the key being searched exists in the container, 'find' and
    //:   'lower_bound' returns the iterator referring the existing element,
    //:   'contains' returns 'true', and 'upper_bound' returns the iterator to
    //:   the element after the searched element.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator, 'contains' returns 'false', 'lower_bound'
    //:   and 'upper_bound' returns the iterator to the smallest element
    //:   greater than searched element.
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
    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

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
            int  numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                it = mX.find(k);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERT(it == X.find(k));
            ASSERT(1 == numPasses);

            const bool expectContains = (mX.end() != it);
            ASSERT(expectContains == mX.contains(k));

            if (std::strchr(SPEC, c)) {
                ASSERT(isConstValue(*it));

                ASSERT(*it  ==  k);
                ASSERT(BSLS_UTIL_ADDRESSOF(*it) != BSLS_UTIL_ADDRESSOF(k));

                ASSERT(1 == mX.count(k));
                ASSERT(1 ==  X.count(k));
            }
            else {
                ASSERT(X.end() == it);

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

            if (std::strchr(SPEC, c)) {
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
    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    TestValues VALUES;

    if (verbose) printf("\tTesting 'count'\n");
    {
        for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
            const int   LINE = DEFAULT_DATA[i].d_line;
            const char *SPEC = DEFAULT_DATA[i].d_spec;

            Obj        mX;
            const Obj& X = gg(&mX, SPEC);

            for (char c = 'A'; c <= 'Z'; ++c) {
                KEY k = VALUES[c - 'A'];

                const size_t EXP = std::strchr(SPEC, c) ? 1 : 0;

                ASSERTV(LINE, SPEC, c, EXP, EXP == X.count(k));
            }
        }
    }

    if (verbose) printf("\tTesting range creation\n");
    {
        HASH  defaultHash(7);
        EQUAL defaultEqual(9);

        ASSERTV(!(HASH()  == defaultHash));
        ASSERTV(!(EQUAL() == defaultEqual));

        for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
            const int     LINE    = DEFAULT_DATA[i].d_line;
            const char   *SPEC    = DEFAULT_DATA[i].d_spec;
            const char   *RESULTS = DEFAULT_DATA[i].d_results;
            const size_t  LENGTH  = strlen(SPEC);

            if (veryVerbose) { T_ T_ P(SPEC) }

            for (char cfg = 'a'; cfg <= 'n' ; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                bsltf::TestValuesArray<KEY> tv(SPEC, xscratch);

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                KeyAllocator         xsa(&sa);

                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator& oa = strchr("efghlmn", CONFIG) ? sa : da;
                bslma::TestAllocator& noa = (&oa == &da) ? sa : da;

                int  numPasses = 0;
                Obj *pmX;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;
                    tv.resetIterators();

                    switch (CONFIG) {
                      case 'a': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end());
                      } break;
                      case 'b': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0);
                      } break;
                      case 'c': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           defaultHash);
                      } break;
                      case 'd': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           defaultHash,
                                           defaultEqual);
                      } break;
                      case 'e': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           defaultHash,
                                           defaultEqual,
                                           xsa);
                      } break;
                      case 'f': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           defaultHash,
                                           xsa);
                      } break;
                      case 'g': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           0,
                                           xsa);
                      } break;
                      case 'h': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           xsa);
                      } break;
                      case 'i': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100);
                      } break;
                      case 'j': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           defaultHash);
                      } break;
                      case 'k': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           defaultHash,
                                           defaultEqual);
                      } break;
                      case 'l': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           defaultHash,
                                           defaultEqual,
                                           xsa);
                      } break;
                      case 'm': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           defaultHash,
                                           xsa);
                      } break;
                      case 'n': {
                        pmX = new (fa) Obj(tv.begin(),
                                           tv.end(),
                                           100,
                                           xsa);
                      } break;
                      default: {
                        ASSERTV(0);
                        return;                                       // RETURN
                      } break;
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(CONFIG, LENGTH,
                        (PLAT_EXC && (LENGTH > 0 || CONFIG >= 'i')) ==
                                                              (numPasses > 1));

                Obj& mX = *pmX;    const Obj& X = mX;

                ASSERTV(CONFIG, LENGTH, noa.numBlocksTotal(), &oa == &da,
                        0 == noa.numBlocksTotal());

                TestValues EXP(RESULTS, xscratch);
                ASSERT(0 == verifyContainer(X, EXP, strlen(RESULTS)));

                ASSERTV((strchr("cdefjklm", CONFIG) ?
                                 defaultHash  : HASH())  == X.hash_function());
                ASSERTV((strchr("dekl",   CONFIG) ?
                                        defaultEqual : EQUAL()) == X.key_eq());

                ASSERTV(CONFIG, SPEC, X.bucket_count(),
                        CONFIG < 'i' || 100 <= X.bucket_count());

                for (char c = 'A'; c <= 'Z'; ++c) {
                    int        idx = c - 'A';
                    const KEY& k = VALUES[idx];

                    std::size_t EXP = !!std::strchr(SPEC, c);

                    ASSERTV(LINE, SPEC, c, EXP, EXP == X.count(k));
                }

                fa.deleteObject(pmX);
            }
        }
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::unordered_set' cannot be deduced from the constructor parameters.
    //..
    // unordered_set()
    // explicit unordered_set(size_t, HASH=HASH(), EQUAL=EQUAL(),
    //                                                  ALLOCATOR=ALLOCATOR());
    // unordered_set(size_t, HASH, EQUAL);
    // unordered_set(size_t, ALLOCATOR);
    // explicit unordered_set(ALLOCATOR);
    //..

    template <class KEY_TYPE>
    struct StupidEqual {
        bool operator()(const KEY_TYPE&, const KEY_TYPE&) const
            // Always return true
        {
            return true;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidEqualFn(const KEY_TYPE&, const KEY_TYPE&)
        // Always return true
    {
        return true;
    }

    template <class KEY_TYPE>
    struct StupidHash {
        size_t operator()(const KEY_TYPE&) const
            // Always hash to bucket #0
        {
            return 0U;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidHashFn(const KEY_TYPE&)
        // Always hash to bucket #0
    {
        return 0U;
    }

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    void TestConstructors ()
        // Test that constructing a 'bsl::unordered_set' from various
        // combinations of arguments deduces the correct type.
        //..
        // unordered_set(const unordered_set&  s)            -> decltype(s)
        // unordered_set(const unordered_set&  s, ALLOCATOR) -> decltype(s)
        // unordered_set(      unordered_set&& s)            -> decltype(s)
        // unordered_set(      unordered_set&& s, ALLOCATOR) -> decltype(s)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::unordered_set<T1> us1;
        bsl::unordered_set     us1a(us1);
        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_set<T1>);

        typedef float T2;
        bsl::unordered_set<T2> us2;
        bsl::unordered_set     us2a(us2, bsl::allocator<T2>());
        bsl::unordered_set     us2b(us2, a1);
        bsl::unordered_set     us2c(us2, a2);
        bsl::unordered_set     us2d(us2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_set<T2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_set<T2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_set<T2>);
        ASSERT_SAME_TYPE(decltype(us2d), bsl::unordered_set<T2>);

        typedef short T3;
        bsl::unordered_set<T3> us3;
        bsl::unordered_set     us3a(std::move(us3));
        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_set<T3>);

        typedef long double T4;
        bsl::unordered_set<T4> us4;
        bsl::unordered_set     us4a(std::move(us4), bsl::allocator<T4>{});
        bsl::unordered_set     us4b(std::move(us4), a1);
        bsl::unordered_set     us4c(std::move(us4), a2);
        bsl::unordered_set     us4d(std::move(us4), bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4d), bsl::unordered_set<T4>);
    }

    void TestIteratorConstructors ()
        // Test that constructing a 'bsl::unordered_set' from a pair of
        // iterators and various combinations of other arguments deduces the
        // correct type.
        //..
        // unordered_set(Iter, Iter, size_type = N, HASH=HASH(), EQUAL=EQUAL(),
        //                                              ALLOCATOR=ALLOCATOR());
        // unordered_set(Iter, Iter, size_type, HASH, ALLOCATOR);
        // unordered_set(Iter, Iter, size_type, ALLOCATOR);
        // unordered_set(Iter, Iter, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef long                        T1;
        typedef StupidHash<T1>              HashT1;
        typedef StupidEqual<T1>             EqualT1;
        typedef decltype(StupidHashFn<T1>)  HashFnT1;
        typedef decltype(StupidEqualFn<T1>) EqualFnT1;
        typedef bsl::allocator<T1>          BA1;
        typedef std::allocator<T1>          SA1;

        T1                               *p1b = nullptr;
        T1                               *p1e = nullptr;
        bsl::unordered_set<T1>::iterator  i1b;
        bsl::unordered_set<T1>::iterator  i1e;

        bsl::unordered_set us1a(p1b, p1e);
        bsl::unordered_set us1b(i1b, i1e);
        bsl::unordered_set us1c(p1b, p1e, 3);
        bsl::unordered_set us1d(i1b, i1e, 3);

        bsl::unordered_set us1e(p1b, p1e, 3, HashT1{});
        bsl::unordered_set us1f(p1b, p1e, 3, StupidHashFn<T1>);
        bsl::unordered_set us1g(i1b, i1e, 3, HashT1{});
        bsl::unordered_set us1h(i1b, i1e, 3, StupidHashFn<T1>);

        bsl::unordered_set us1i(p1b, p1e, 3, HashT1{}, EqualT1{});
        bsl::unordered_set us1j(i1b, i1e, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_set us1k(p1b, p1e, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_set us1l(i1b, i1e, 3,
                                          StupidHashFn<T1>, StupidEqualFn<T1>);

        bsl::unordered_set us1m(p1b, p1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_set us1n(p1b, p1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_set us1o(p1b, p1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_set us1p(p1b, p1e, 3, HashT1{}, EqualT1{}, SA1{});
        bsl::unordered_set us1q(i1b, i1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_set us1r(i1b, i1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_set us1s(i1b, i1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_set us1t(i1b, i1e, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_set<T1>);
        ASSERT_SAME_TYPE(decltype(us1b), bsl::unordered_set<T1>);
        ASSERT_SAME_TYPE(decltype(us1c), bsl::unordered_set<T1>);
        ASSERT_SAME_TYPE(decltype(us1d), bsl::unordered_set<T1>);
        ASSERT_SAME_TYPE(decltype(us1e), bsl::unordered_set<T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1f), bsl::unordered_set<T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1g), bsl::unordered_set<T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1h), bsl::unordered_set<T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1i),
                         bsl::unordered_set<T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1j),
                         bsl::unordered_set<T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1k),
                         bsl::unordered_set<T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1l),
                         bsl::unordered_set<T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(decltype(us1m),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1n),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1o),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1p),
                         bsl::unordered_set<T1, HashT1, EqualT1, SA1>);
        ASSERT_SAME_TYPE(decltype(us1q),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1r),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1s),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1t),
                         bsl::unordered_set<T1, HashT1, EqualT1, SA1>);

        typedef double                      T2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        typedef bsl::allocator<T2>          BA2;
        typedef std::allocator<T2>          SA2;
        T2                               *p2b = nullptr;
        T2                               *p2e = nullptr;
        bsl::unordered_set<T2>::iterator  i2b;
        bsl::unordered_set<T2>::iterator  i2e;

        bsl::unordered_set us2a(p2b, p2e, 3, HashT2{}, BA2{});
        bsl::unordered_set us2b(p2b, p2e, 3, HashT2{}, a1);
        bsl::unordered_set us2c(p2b, p2e, 3, HashT2{}, a2);
        bsl::unordered_set us2d(p2b, p2e, 3, HashT2{}, SA2{});
        bsl::unordered_set us2e(p2b, p2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_set us2f(p2b, p2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_set us2g(p2b, p2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_set us2h(p2b, p2e, 3, StupidHashFn<T2>, SA2{});
        bsl::unordered_set us2i(i2b, i2e, 3, HashT2{}, BA2{});
        bsl::unordered_set us2j(i2b, i2e, 3, HashT2{}, a1);
        bsl::unordered_set us2k(i2b, i2e, 3, HashT2{}, a2);
        bsl::unordered_set us2l(i2b, i2e, 3, HashT2{}, SA2{});
        bsl::unordered_set us2m(i2b, i2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_set us2n(i2b, i2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_set us2o(i2b, i2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_set us2p(i2b, i2e, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(
                       decltype(us2d),
                       bsl::unordered_set<T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2e), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2f), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2g), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
                   decltype(us2h),
                   bsl::unordered_set<T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2i), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2j), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2k), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(
                       decltype(us2l),
                       bsl::unordered_set<T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2m), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2n), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2o), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
                   decltype(us2p),
                   bsl::unordered_set<T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                T3;
        typedef bsl::allocator<T3> BA3;
        typedef std::allocator<T3> SA3;
        T3                               *p3b = nullptr;
        T3                               *p3e = nullptr;
        bsl::unordered_set<T3>::iterator  i3b;
        bsl::unordered_set<T3>::iterator  i3e;

        bsl::unordered_set us3a(p3b, p3e, 3, BA3{});
        bsl::unordered_set us3b(p3b, p3e, 3, a1);
        bsl::unordered_set us3c(p3b, p3e, 3, a2);
        bsl::unordered_set us3d(p3b, p3e, 3, SA3{});
        bsl::unordered_set us3e(i3b, i3e, 3, BA3{});
        bsl::unordered_set us3f(i3b, i3e, 3, a1);
        bsl::unordered_set us3g(i3b, i3e, 3, a2);
        bsl::unordered_set us3h(i3b, i3e, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(decltype(us3b), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(decltype(us3c), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(
                decltype(us3d),
                bsl::unordered_set<T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);
        ASSERT_SAME_TYPE(decltype(us3e), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(decltype(us3f), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(decltype(us3g), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(
                decltype(us3h),
                bsl::unordered_set<T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);

        typedef char               T4;
        typedef bsl::allocator<T4> BA4;
        typedef std::allocator<T4> SA4;
        T4                               *p4b = nullptr;
        T4                               *p4e = nullptr;
        bsl::unordered_set<T4>::iterator  i4b;
        bsl::unordered_set<T4>::iterator  i4e;

        bsl::unordered_set us4a(p4b, p4e, BA4{});
        bsl::unordered_set us4b(p4b, p4e, a1);
        bsl::unordered_set us4c(p4b, p4e, a2);
        bsl::unordered_set us4d(p4b, p4e, SA4{});
        bsl::unordered_set us4e(i4b, i4e, BA4{});
        bsl::unordered_set us4f(i4b, i4e, a1);
        bsl::unordered_set us4g(i4b, i4e, a2);
        bsl::unordered_set us4h(i4b, i4e, SA4{});

        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(
                decltype(us4d),
                bsl::unordered_set<T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);

        ASSERT_SAME_TYPE(decltype(us4e), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4f), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4g), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(
                decltype(us4h),
                bsl::unordered_set<T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);
    }

    void TestStdInitializerListConstructors ()
        // Test that constructing a 'bsl::unordered_set' from an
        // initializer_list and various combinations of other arguments deduces
        // the correct type.
        //..
        // unordered_set(initializer_list, size_type = N, HASH=HASH(),
        //                               EQUAL=EQUAL(), ALLOCATOR=ALLOCATOR());
        // unordered_set(initializer_list, size_type, HASH, ALLOCATOR);
        // unordered_set(initializer_list, size_type, ALLOCATOR);
        // unordered_set(initializer_list, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef long                        T1;
        typedef bsl::allocator<T1>          BA1;
        typedef std::allocator<T1>          SA1;
        typedef StupidHash<T1>              HashT1;
        typedef StupidEqual<T1>             EqualT1;
        typedef decltype(StupidHashFn<T1>)  HashFnT1;
        typedef decltype(StupidEqualFn<T1>) EqualFnT1;
        std::initializer_list<T1> il1 = {1L, 2L, 3L, 4L};

        bsl::unordered_set us1a(il1);
        bsl::unordered_set us1b(il1, 3);
        bsl::unordered_set us1c(il1, 3, HashT1{});
        bsl::unordered_set us1d(il1, 3, StupidHashFn<T1>);
        bsl::unordered_set us1e(il1, 3, HashT1{}, EqualT1{});
        bsl::unordered_set us1f(il1, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_set us1g(il1, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_set us1h(il1, 3, StupidHashFn<T1>, StupidEqualFn<T1>);

        bsl::unordered_set us1i(il1, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_set us1j(il1, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_set us1k(il1, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_set us1l(il1, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_set<T1>);
        ASSERT_SAME_TYPE(decltype(us1b), bsl::unordered_set<T1>);
        ASSERT_SAME_TYPE(decltype(us1c), bsl::unordered_set<T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1d), bsl::unordered_set<T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1e),
                         bsl::unordered_set<T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1f),
                         bsl::unordered_set<T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1g),
                         bsl::unordered_set<T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1h),
                         bsl::unordered_set<T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(decltype(us1i),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1j),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1k),
                         bsl::unordered_set<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1l),
                         bsl::unordered_set<T1, HashT1, EqualT1, SA1>);

        typedef double                      T2;
        typedef bsl::allocator<T2>          BA2;
        typedef std::allocator<T2>          SA2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        std::initializer_list<T2> il2 = {1.0, 2.0, 3.0, 4.0};

        bsl::unordered_set us2a(il2, 3, HashT2{}, BA2{});
        bsl::unordered_set us2b(il2, 3, HashT2{}, a1);
        bsl::unordered_set us2c(il2, 3, HashT2{}, a2);
        bsl::unordered_set us2d(il2, 3, HashT2{}, SA2{});
        bsl::unordered_set us2e(il2, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_set us2f(il2, 3, StupidHashFn<T2>, a1);
        bsl::unordered_set us2g(il2, 3, StupidHashFn<T2>, a2);
        bsl::unordered_set us2h(il2, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_set<T2, HashT2>);
        ASSERT_SAME_TYPE(
                       decltype(us2d),
                       bsl::unordered_set<T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2e), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2f), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2g), bsl::unordered_set<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
                   decltype(us2h),
                   bsl::unordered_set<T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                T3;
        typedef bsl::allocator<T3> BA3;
        typedef std::allocator<T3> SA3;
        std::initializer_list<T3> il3 = {1, 2, 3, 4};

        bsl::unordered_set us3a(il3, 3, BA3{});
        bsl::unordered_set us3b(il3, 3, a1);
        bsl::unordered_set us3c(il3, 3, a2);
        bsl::unordered_set us3d(il3, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(decltype(us3b), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(decltype(us3c), bsl::unordered_set<T3>);
        ASSERT_SAME_TYPE(
                decltype(us3d),
                bsl::unordered_set<T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);

        typedef char               T4;
        typedef bsl::allocator<T4> BA4;
        typedef std::allocator<T4> SA4;
        std::initializer_list<T4> il4 = {'1', '2', '3', '4'};

        bsl::unordered_set us4a(il4, BA4{});
        bsl::unordered_set us4b(il4, a1);
        bsl::unordered_set us4c(il4, a2);
        bsl::unordered_set us4d(il4, SA4{});

        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_set<T4>);
        ASSERT_SAME_TYPE(
                decltype(us4d),
                bsl::unordered_set<T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

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
      case 36: {
        // --------------------------------------------------------------------
        // TESTING ERASE_IF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTION 'BSL::ERASE_IF'"
                            "\n=====================================\n");

        TestDriver<char>::testCase36();
        TestDriver<int>::testCase36();
        TestDriver<long>::testCase36();
      } break;
      case 35: {
        //---------------------------------------------------------------------
        // TESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)
        //   Ensure that the deduction guides are properly specified to deduce
        //   the template arguments from the arguments supplied to the
        //   constructors.
        //
        // Concerns:
        //: 1 Construction from iterators deduces the value type from the value
        //:   type of the iterator.
        //
        //: 2 Construction with a 'bslma::Allocator *' deduces the correct
        //:   specialization of 'bsl::allocator' for the type of the allocator.
        //
        // Plan:
        //: 1 Create an unordered_set by invoking the constructor without
        //:   supplying the template arguments explicitly.
        //:
        //: 2 Verify that the deduced type is correct.
        //
        // Testing:
        //   CLASS TEMPLATE DEDUCTION GUIDES
        //---------------------------------------------------------------------
        if (verbose)
            printf(
              "\nTESTING CLASS TEMPLATE DEDUCTION GUIDES (AT COMPILE TIME)"
              "\n=========================================================\n");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
        // This is a compile-time only test case.
        TestDeductionGuides test; (void) test;
#endif
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        //: 1 'unordered_set' has does not have a transparent set of lookup
        //:   functions if the comparator is not transparent.
        //: 2 'unordered_set' has a transparent set of lookup functions if the
        //:   comparator is transparent.
        //
        // Plan:
        //: 1 Construct a non-transparent set and call the lookup functions
        //:   with a type that is convertible to the 'value_type'.  There
        //:   should be exactly one conversion per call to a lookup function.
        //:   (C-1)
        //: 2 Construct a transparent set and call the lookup functions with a
        //:   type that is convertible to the 'value_type'.  There should be
        //:   no conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: 'find'        properly handles transparent comparators
        //   CONCERN: 'count'       properly handles transparent comparators
        //   CONCERN: 'equal_range' properly handles transparent comparators
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::unordered_set<int>                      NonTransparentSet;
        typedef bsl::unordered_set<int,
                   TransparentHasher, TransparentComparator> TransparentSet;

        const int DATA[] = { 0, 1, 2, 3, 4 };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        NonTransparentSet        mXNT;
        const NonTransparentSet& XNT = mXNT;

        for (int i = 0; i < NUM_DATA; ++i) {
            if (veryVeryVeryVerbose) {
                printf("Constructing test data.\n");
            }
            mXNT.insert(DATA[i]);
        }

        TransparentSet        mXT(mXNT.begin(), mXNT.end());
        const TransparentSet& XT = mXT;

        ASSERT(NUM_DATA == XNT.size());
        ASSERT(NUM_DATA == XT.size() );

        for (int i = 0; i < NUM_DATA; ++i) {
            const int KEY = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a key of %d\n",
                       KEY);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent set.\n");
            }
            testTransparentComparator( XNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent set.\n");
            }
            testTransparentComparator(mXNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting const transparent set.\n");
            }
            testTransparentComparator( XT,  true,  KEY);

            if (veryVerbose) {
                printf("\tTesting mutable transparent set.\n");
            }
            testTransparentComparator(mXT,  true,  KEY);
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n'noexcept' SPECIFICATION"
                            "\n========================");

        typedef bsltf::StdStatefulAllocator
                           <int,    // TYPE
                            false,  // PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION
                            false,  // PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT
                            false,  // PROPAGATE_ON_CONTAINER_SWAP
                            false,  // PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT
                            false   // IS_ALWAYS_EQUAL
                           > AFA;   // AllFalseAllocator

        typedef bsltf::StdStatefulAllocator
                           <int,    // TYPE
                            false,  // PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION
                            false,  // PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT
                            false,  // PROPAGATE_ON_CONTAINER_SWAP
                            false,  // PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT
                            true    // IS_ALWAYS_EQUAL
                           > AEA;   // AlwaysEqualAllocator

        typedef TestHashFunctor<int>             NTH;  // NonThrowingHash
        typedef ThrowingHash<int>                TH;   // ThrowingHash
        typedef TestEqualityComparator<int>      NTP;  // NonThrowingPredicate
        typedef ThrowingAssignmentPredicate<int> TP;   // ThrowingPredicate

        TestDriver<int>::testCase33();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        ASSERT( std::is_nothrow_move_assignable<NTH >::value);
        ASSERT(!std::is_nothrow_move_assignable< TH >::value);
        ASSERT( std::is_nothrow_move_assignable<NTP>::value);
        ASSERT(!std::is_nothrow_move_assignable< TP>::value);
#endif
#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
        ASSERT( bsl::is_nothrow_swappable<NTH >::value);
        ASSERT( bsl::is_nothrow_swappable<NTP>::value);
#endif
        ASSERT(!bsl::is_nothrow_swappable< TH >::value);
        ASSERT(!bsl::is_nothrow_swappable< TP>::value);
#endif

        TestDriver<int, TH,  TP,  AFA>::testCase33();
        TestDriver<int, TH,  TP,  AEA>::testCase33();
        TestDriver<int, TH,  NTP, AFA>::testCase33();
        TestDriver<int, TH,  NTP, AEA>::testCase33();
        TestDriver<int, NTH, TP,  AFA>::testCase33();
        TestDriver<int, NTH, TP,  AEA>::testCase33();
        TestDriver<int, NTH, NTP, AFA>::testCase33();
        TestDriver<int, NTH, NTP, AEA>::testCase33();

      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INITIALIZER LIST FUNCTIONS"
                            "\n==================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase32_outOfLine,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase32_outOfLine,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        RUN_EACH_TYPE(TestDriver,
                      testCase32_inline,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE (WITH AND WITHOUT HINT)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EMPLACE (WITH AND WITHOUT HINT)"
                            "\n=======================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase31,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(TestDriver,
                      testCase31a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);

        RUN_EACH_TYPE(TestDriver,
                      testCase31b,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase31,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING INSERTION OF MOVABLE VALUES"
                            "\n===================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase30,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING MOVE ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE ASSIGNMENT"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        // 'propagate_on_container_move_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase29_propagate_on_container_move_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        // TBD test 'bsltf::MoveOnlyAllocTestType' here

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase29,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTION
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTION"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase28,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING SPREAD
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING SPREAD\n"
                            "==============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING BUCKET INTERFACE AND BUCKET ITERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING BUCKET INTERFACE AND BUCKET ITERATORS\n"
                            "=============================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'reserve', 'rehash', AND 'max_load_factor'
        // --------------------------------------------------------------------

        if (verbose) printf(
                "TESTING 'reserve', 'rehash', AND 'max_load_factor'\n"
                "==================================================\n");

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

        if (verbose) printf("\nTESTING COMPARATOR"
                            "\n==================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' AND 'empty'
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'max_size' AND 'empty'\n"
                            "==============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------

        // N/A

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'erase'\n"
                            "===============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase18,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE 'insert'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING RANGE 'insert'"
                            "\n======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase17,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'insert' WITH HINT
        // --------------------------------------------------------------------

        // N/A

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'insert'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'insert'\n"
                            "================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase15,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ITERATORS\n"
                            "=================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find', 'contains', 'equal_range', 'count'
        // --------------------------------------------------------------------

        if (verbose)
            printf("TESTING 'find', 'contains', 'equal_range', 'count'\n"
                   "==================================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING RANGE (TEMPLATE) CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING RANGE (TEMPLATE) CONSTRUCTORS"
                            "\n=====================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase12,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 11: // falls through
      case 10: // falls through
      case  9: // falls through
      case  8: // falls through
      case  7: // falls through
      case  6: // falls through
      case  5: // falls through
      case  4: // falls through
      case  3: // falls through
      case  2: // falls through
      case  1: {
        if (verbose)
            printf("\nTEST CASE %d IS HANDLED BY PRIMARY TEST DRIVER"
                   "\n==============================================\n",
                   test);
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
