// bslstl_unorderedmultiset_test.t.cpp                                -*-C++-*-

#include <bslstl_unorderedmultiset_test.h>

#include <bslstl_iterator.h>
#include <bslstl_pair.h>
#include <bslstl_unorderedmultiset.h>

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
#include <bsls_keyword.h>
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

#include <utility> // move

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
// [38] erase_if(unordered_multiset<K, H, E, A>& a, PREDICATE);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] default construction (only)
// [36] CLASS TEMPLATE DEDUCTION GUIDES
// [37] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(unordered_multiset *object, const char *s, int verbose);
// [ 3] unordered_multiset& gg(unordered_multiset *object, const char *s);
//
// [22] CONCERN: The object is compatible with STL allocators.
// [23] CONCERN: The object has the necessary type traits
// [27] CONCERN: The values are spread into different buckets.
// [35] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
// [36] CONCERN: 'find'        properly handles transparent comparators.
// [36] CONCERN: 'count'       properly handles transparent comparators.
// [36] CONCERN: 'equal_range' properly handles transparent comparators.

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

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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

    const Count EXPECTED_C = initKeyValue ? initKeyValue : 1;
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

    enum { k_IS_KEY_WELL_BEHAVED = bsl::is_same<KEY,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,
           k_IS_KEY_MOVE_AWARE =
                    bsl::is_same<KEY, bsltf::MovableTestType>::value ||
                    bsl::is_same<KEY, bsltf::MovableAllocTestType>::value ||
                    bsl::is_same<KEY, bsltf::MoveOnlyAllocTestType>::value ||
                                                       k_IS_KEY_WELL_BEHAVED };

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
    static void testCase32a_RunTest(Obj *target);
        // Call 'emplace' on the specified 'target' container and verify that a
        // value was inserted.  Forward (template parameter) 'N_ARGS' arguments
        // to the 'emplace' method and ensure 1) that values are properly
        // passed to the constructor of 'value_type', 2) that the allocator is
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
    static Iter testCase33a_RunTest(Obj *target, CIter hint);
        // Call 'emplace_hint' on the specified 'target' container and verify
        // that a value was inserted.  Forward (template parameter) 'N_ARGS'
        // arguments to the 'emplace' method and ensure 1) that values are
        // properly passed to the constructor of 'value_type', 2) that the
        // allocator is correctly configured for each argument in the newly
        // inserted element in 'target', and 3) that the arguments are
        // forwarded using copy or move semantics based on integer template
        // parameters 'N01' ... 'N10'.

  public:
    // TEST CASES
    static void testCase38();
        // Test free function 'bsl::erase_if'

    static void testCase35();
        // Test 'noexcept' specifications.

    static void testCase34_outOfLine();
    static void testCase34_inline();
        // Test initializer lists.

    static void testCase33a();
        // Test forwarding of arguments in 'emplace_hint' method.

    static void testCase33();
        // Test emplace with hint.

    static void testCase32a();
        // Test forwarding of arguments in 'emplace' method.

    static void testCase32();
        // Test emplace.

    static void testCase31();
        // Test insertion with hint on movable values.

    static void testCase30();
        // Test insertion on movable values.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase29_propagate_on_container_move_assignment_dispatch();
    static void testCase29_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase29();
        // Test move assignment.

    static void testCase28();
        // Test move construction.

    static void testCase27();
        // Test spread.

    static void testCase26();
        // Test 'reserve', 'rehash', 'max_load_factor' and bucket iterators.

    static void testCase25();
        // Test reserve, rehash, max_load_factor.

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

    static void testCase16();
        // Test 'insert' with hint.

    static void testCase15();
        // Test 'insert'.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test 'find', 'equal_range' and 'count'.

    static void testCase12();
        // Test value and range constructors.
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
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase38()
{
    // --------------------------------------------------------------------
    // TESTING FREE FUNCTION 'BSL::ERASE_IF'
    //
    // Concerns:
    //: 1 The free function exists, and is callable with an unordered_multiset.
    //
    // Plan:
    //: 1 Fill a set with known values, then attempt to erase some of
    //:   the values using 'bsl::erase_if'.  Verify that the resultant set
    //:   is the right size, contains the correct values, and that the
    //:   value returned from the functions is correct.
    //
    // Testing:
    //   size_t erase_if(unordered_multiset&, PREDICATE);
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
        { L_,   "CBADEGHIJKL",       'B',     "CADEGHIJKL"        },
        { L_,   "CBADEABCDAB",       'B',     "CADEACDA"          },
        { L_,   "CBADEABCDABCDEA",   'E',     "CBADABCDABCDA"     },
        { L_,   "ZZZZZZZZZZZZZZZ",   'Z',     ""                  }
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
TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase32a_RunTest(Obj *target)
{

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
        return;                                                       // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj&                  mX = *target;
    const Obj&            X = mX;

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
TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase33a_RunTest(Obj  *target,
                                                         CIter hint)
{
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
        ASSERT(!"Allocator in test case 33 is not a test allocator!");
        return hint;                                                  // RETURN
    }
    bslma::TestAllocator& oa = *testAlloc;
    Obj&                  mX = *target;
    const Obj&            X = mX;

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

        if (mX.end() != hint) {
            ASSERTV(true == (&(*result) != &(*hint)));
        }

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
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase35()
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

    // N4594: page 895: Class template unordered_multiset

    // page 895: 23.5.7.2, construct/copy/destroy:
    //..
    //  unordered_multiset& operator=(unordered_multiset&&)
    //          noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //                          is_nothrow_move_assignable<Hash>::value &&
    //                          is_nothrow_move_assignable<Pred>::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;  const Obj& X = mX;    (void) X;
        Obj mY;                        (void) mY;

        bool expected = false;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        expected = bsl::allocator_traits<ALLOC> ::is_always_equal::value
                && std::is_nothrow_move_assignable<HASH>::value
                && std::is_nothrow_move_assignable<EQUAL>::value;
#endif
        ASSERT(expected
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.get_allocator()));
    }

    // page 896
    //..
    //  // iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cend()));
    }

    // page 896
    //..
    //  // capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_size()));
    }

    // page 897
    //..
    // // modifiers:
    //  void swap(unordered_multiset&)
    //          noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //          is_nothrow_swappable_v<Hash> &&
    //          is_nothrow_swappable_v<Pred>);
    //  void clear() noexcept;
    //..

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        bool expected = false;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        expected = bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                   bsl::is_nothrow_swappable<HASH>::value &&
                   bsl::is_nothrow_swappable<EQUAL>::value;
#endif

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 897
    //..
    // // bucket interface
    // size_type bucket_count() const noexcept;
    // size_type max_bucket_count() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.bucket_count()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_bucket_count()));
    }

    // page 897
    //..
    // // hash policy:
    // float load_factor() const noexcept;
    // float max_load_factor() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.load_factor()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_load_factor()));
    }

    // page 897
    //..
    //  // 23.5.7.3, swap:
    //  template <class Key, class Hash, class Pred, class Alloc>
    //  void swap(unordered_multiset<Key, Hash, Pred, Alloc>& x,
    //  unordered_multiset<Key, Hash, Pred, Alloc>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj mX;    (void) mX;
        Obj mY;    (void) mY;

        bool expected = BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.swap(mY));

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(mX, mY)));
    }

}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase34_outOfLine()
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
    //   unordered_multiset(initializer_list, const A&);
    //   unordered_multiset(initializer_list, size_type, const A&);
    //   unordered_multiset(initializer_list, size_type, hash, const A&);
    //   unordered_multiset(initializer_list, size_type, hash, pred, const A&);
    //   unordered_multiset& operator=(initializer_list<value_type>);
    //   iterator insert(initializer_list<value_type>);
    // -----------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    const TestValues V;

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose)
        printf("\tTesting constructor with initializer lists\n");

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocatorRaw(&da);
    {
        const struct {
            int                         d_line;    // source line number
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  ---------------------  ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[0], V[1]       },  "AB"   },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[0] },  "AAB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[2], V[1], V[1] },  "BBC"  },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa     == X.get_allocator());
                ASSERT(HASH()  == X.hash_function());
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
            //----  ---------------------  ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[0], V[1]       },  "AB"   },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[0] },  "AAB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[2], V[1], V[1] },  "BBC"  },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, 100, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa     == X.get_allocator());
                ASSERT(HASH()  == X.hash_function());
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
            //----  ---------------------  ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[0], V[1]       },  "AB"   },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[0] },  "AAB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[2], V[1], V[1] },  "BBC"  },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, 100, HASH(9), xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa     == X.get_allocator());
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
            //----  ---------------------  ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "AA"   },
            { L_,   { V[0], V[1]       },  "AB"   },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[0] },  "AAB"  },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[2], V[1], V[1] },  "BBC"  },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, 100, HASH(9), EQUAL(6), xoa);
                const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa      == X.get_allocator());
                ASSERT(HASH(9)  == X.hash_function());
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
            //----  -----  ---------------------  ------
            { L_,   "",    {                  },  ""     },
            { L_,   "",    { V[0]             },  "A"    },
            { L_,   "A",   {                  },  ""     },
            { L_,   "A",   { V[1]             },  "B"    },
            { L_,   "A",   { V[0], V[1]       },  "AB"   },
            { L_,   "A",   { V[1], V[2]       },  "BC"   },
            { L_,   "AB",  {                  },  ""     },
            { L_,   "AB",  { V[0], V[1], V[2] },  "ABC"  },
            { L_,   "AB",  { V[2], V[1], V[0] },  "ABC"  },
            { L_,   "AB",  { V[2], V[3], V[4] },  "CDE"  },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mY(xscratch);
            const Obj&           Y = gg(&mY, DATA[ti].d_result);

            Obj mX(xoa); const Obj& X = gg(&mX, DATA[ti].d_spec);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = DATA[ti].d_list);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(Y,    X,  Y == X);

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
            //line  source  list                   result
            //----  ------  ---------------------  -------
            { L_,   "",     {                  },  ""      },
            { L_,   "",     { V[0]             },  "A"     },
            { L_,   "A",    {                  },  "A"     },
            { L_,   "A",    { V[0]             },  "AA"    },
            { L_,   "A",    { V[1]             },  "AB"    },
            { L_,   "AB",   { V[0], V[1]       },  "AABB"  },
            { L_,   "AB",   { V[1], V[0]       },  "AABB"  },
            { L_,   "AB",   { V[1], V[2]       },  "ABBC"  },
            { L_,   "AB",   { V[2], V[1]       },  "ABBC"  },
            { L_,   "AB",   { V[0], V[1], V[2] },  "AABBC" },
            { L_,   "AB",   { V[2], V[3], V[4] },  "ABCDE" },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        KeyAllocator                xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

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
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase34_inline()
{
    // ------------------------------------------------------------------------
    // TESTING INITIALIZER LIST C'TORS INLINE
    //
    // Concerns:
    //: 1 That the initializer list constructors can work with an inline
    //:   initializer list.
    //
    // Plan:
    //: 1 Repeat the tests done in 'testCase33_outOfLine', except test only
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
    //   void insert(initializer_list<Pair>);
    //   unordered_set& operator=(initializer_list<Pair>);
    // ------------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    if (verbose) printf("INITIALIZER_LIST not supported -- no test: %s\n",
                        NameOf<KEY>().name());
#else
    if (verbose) printf(
                "TESTING INITIALIZER LIST C'TORS AND MANIPULATORS INLINE: %s\n"
                "=======================================================\n",
                NameOf<KEY>().name());

    bslma::TestAllocator ta("ta", veryVeryVeryVerbose);    // testValues
    bslma::TestAllocator fa("fa", veryVeryVeryVerbose);    // footprint
    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);    // other
    bslma::TestAllocator da("da", veryVeryVeryVerbose);    // default
    bslma::DefaultAllocatorGuard dag(&da);

    const char   testValuesSpec[] = { "ABC" };
    const size_t testValuesSpecLen = sizeof(testValuesSpec) - 1;

    TestValues   testValues(testValuesSpec, &ta);
    ASSERT(testValues.size() == testValuesSpecLen);

#define u_INIT_LIST                                                           \
              { testValues[1], testValues[0], testValues[2], testValues[0] }

    TestValues   expectedValues("AABC", &ta);

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
            ASSERT(!p->empty() && 4 == p->size());
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

        ASSERTV(NameOf<KEY>(), X.size(), expectedValues.size() == X.size());
        ASSERTV(NameOf<KEY>(), verifySpec(X, "AABC"));

        ASSERTV(!oa.numBlocksInUse() != !da.numBlocksInUse());

        fa.deleteObjectRaw(p);

        ASSERTV(ctor, 0 == fa.numBlocksInUse());
        ASSERTV(ctor, 0 == oa.numBlocksInUse());
        ASSERTV(ctor, 0 == da.numBlocksInUse());
    }

    ASSERTV(fa.numBlocksTotal() == 'h' + 1 - 'a' + totalThrows);

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");

        mX.insert(u_INIT_LIST);

        ASSERTV(verifySpec(X, "AABCCDE"));
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
        ASSERTV(verifySpec(X, "AABC"));
        ASSERTV(verifySpec(Y, "CDE"));
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

        ASSERT(4 == mp.d_second.size());
    }

#undef u_INIT_LIST

    ASSERT(done);
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase33a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note tha only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase33'.
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

    Iter hint;

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=1\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase33a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, hint);
        hint = testCase33a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint);
    }
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase33a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint);
    }
    if (verbose) printf("\tTesting emplace_hint with 0 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
    }
    if (verbose) printf("\tTesting emplace_hint with 1 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase33a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, hint);
    }
    if (verbose) printf("\tTesting emplace_hint with 2 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase33a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, hint);
    }
    if (verbose) printf("\tTesting emplace_hint with 3 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase33a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, hint);
    }

    if (verbose) printf("\tTesting emplace_hint with 10 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end());
        hint = testCase33a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, hint);
        hint = testCase33a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint);
    }
#else
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase33a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase33a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint);
        hint = testCase33a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint);
        hint = testCase33a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint);
    }
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase32a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note tha only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase32'.
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
    // iterator emplace(Args&&... args);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING

    if (verbose) printf("\tTesting emplace 1..10 args, move=1\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX);
        testCase32a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX);
        testCase32a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX);
        testCase32a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX);
        testCase32a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
    if (verbose) printf("\tTesting emplace 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase32a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase32a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase32a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
    if (verbose) printf("\tTesting emplace with 0 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX);
    }
    if (verbose) printf("\tTesting emplace with 1 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX);
    }
    if (verbose) printf("\tTesting emplace with 2 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX);
    }
    if (verbose) printf("\tTesting emplace with 3 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\tTesting emplace with 10 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
        testCase32a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX);
        testCase32a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX);
        testCase32a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX);
        testCase32a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX);
        testCase32a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX);
        testCase32a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
#else
    if (verbose) printf("\tTesting emplace 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase32a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase32a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase32a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase32a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase32a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase32a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase33()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' returns an iterator referring to the newly inserted
    //:   element.
    //:
    //: 2 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 3 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 4 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace_hint'
    //:   function and will test proper forwarding of constructor arguments in
    //:   test 'testCase33a'.
    //:
    //: 2 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'begin', 'begin' + 1,
    //:     'end', find(key).
    //:
    //:     1 For each value in the set, 'emplace' the value with hint.
    //:
    //:       1 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       2 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify all allocations are from the object's allocator.  (C-4)
    //:
    //: 3 Repeat P-2 under the presence of exception  (C-5)
    //
    // Testing:
    //   iterator emplace_hint(const_iterator position, Args&&... args);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    const int MAX_LENGTH = 16;

    if (verbose) printf("\tTesting 'emplace_hint' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);
            char             EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj]);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(CONFIG, !"Unexpected configuration");
                      }
                    }

                    Iter RESULT = mX.emplace_hint(hint, VALUES[tj]);

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
                }
            }
        }
    }
    if (verbose) printf("\tTesting 'emplace_hint' with exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj]);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(CONFIG, !"Unexpected configuration");
                      }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);
                    Iter                 RESULT;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                        RESULT = mX.emplace_hint(hint, VALUES[tj]);
                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
                }
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase32()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    //: 1 The iterator returned refers to the newly inserted element.
    //:
    //: 2 A new element is added to the container.
    //:
    //: 3 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' function
    //:   and will test proper forwarding of constructor arguments in test
    //:   'testCase32a'.
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
    //: 3 Repeat P-2 under the presence of exception  (C-7)
    //
    // Testing:
    //   iterator emplace(Args&&... arguments);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    static const struct {
        int         d_line;  // source line number
        const char *d_spec;  // specification string
    } DATA[] = {
        //line  spec
        //----  --------------
        { L_,   "A"            },
        { L_,   "AAA"          },
        { L_,   "ABCDEFGH"     },
        { L_,   "AABBCCDDEE"   },
        { L_,   "EEDDCCBBAA"   },
        { L_,   "ABCDEABCDEF"  },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    const int MAX_LENGTH = 16;

    if (verbose) printf("\tTesting 'emplace' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const size_t SIZE = X.size();

                if (veryVerbose) { T_ T_ P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                Iter RESULT = mX.emplace(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
            }
        }
    }

    if (verbose) printf("\tTesting 'emplace' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const size_t SIZE = X.size();

                if (veryVerbose) { T_ T_ P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    Iter RESULT = mX.emplace(VALUES[tj]);

                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT ON MOVABLE VALUES
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element.
    //:
    //: 2 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 3 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 4 The new element is move-inserted into the container.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'begin', 'begin' + 1,
    //:     'end', find(key).
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Verify the return value and the resulting data in the container
    //:         is as expected.  (C-1)
    //:
    //:       2 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-2,3)
    //:
    //:       3 Ensure that the move constructor was called on the value type
    //:         (if that type has a mechanism to detect such) where the value
    //:         has the same allocator as that of the container and a different
    //:         allocator than that of the container.  (C-4)
    //:
    //:       4 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-6)
    //
    // Testing:
    //   iterator insert(const_iterator hint, value_type&& value);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    const int MAX_LENGTH = 16;

    if (verbose) printf("\tTesting 'insert' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);
                    Obj                  mX(xoa);
                    const Obj&           X = mX;

                    bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator *valAllocator = 0;
                    switch (cfg2) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(cfg2, !"Bad allocator config.");
                      } break;
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    KeyAllocator          xsa(&sa);

                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                        const size_t SIZE    = X.size();

                        if (veryVeryVerbose) { T_ T_ P(SIZE); }

                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                        CIter hint;
                        switch(CONFIG) {
                          case 'a': {
                              hint = X.begin();
                          } break;
                          case 'b': {
                              hint = X.begin();
                              if (hint != X.end()) {
                                  ++hint;
                              }
                          } break;
                          case 'c': {
                              hint = X.end();
                          } break;
                          case 'd': {
                              hint = X.find(VALUES[tj]);
                              ASSERTV(IS_UNIQ == (hint == X.end()));
                          } break;
                          default: {
                              ASSERTV(CONFIG, !"Unexpected configuration");
                          }
                        }

                        bsls::ObjectBuffer<ValueType> buffer;
                        ValueType *valptr = buffer.address();
                        TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        xsa);

                        Iter            RESULT = mX.insert(
                                                      hint,
                                                      MoveUtil::move(*valptr));
                        MoveState::Enum mState =TstFacility::getMovedFromState(
                                                                      *valptr);
                        bslma::DestructionUtil::destroy(valptr);

                        ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                        const MoveState::Enum exp =
                                         k_IS_KEY_MOVE_AWARE
                                         ? (k_IS_KEY_WELL_BEHAVED && &oa != &sa
                                                       ? MoveState::e_NOT_MOVED
                                                       : MoveState::e_MOVED)
                                                    : MoveState::e_UNKNOWN;

                        ASSERTV(mState, exp, NameOf<KEY>(), exp == mState);

                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        if (CONFIG == 'd' && hint != X.end()) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }

                        ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
                    }
                }
            }
        }
    }
    if (verbose) printf("\tTesting 'insert' with exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);
                    Obj                  mX(xoa);
                    const Obj&           X = mX;

                    bslma::TestAllocator  za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator *valAllocator = 0;
                    switch (cfg2) {
                      case 'a': {
                        // inserted value has same allocator
                        valAllocator = &oa;
                      } break;
                      case 'b': {
                        // inserted value has different allocator
                        valAllocator = &za;
                      } break;
                      default: {
                        ASSERTV(cfg2, !"Bad allocator config.");
                      } break;
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    KeyAllocator          xsa(&sa);

                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                        const size_t SIZE    = X.size();

                        if (veryVeryVerbose) { T_ T_ P(SIZE); }

                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                        CIter hint;
                        switch(CONFIG) {
                          case 'a': {
                              hint = X.begin();
                          } break;
                          case 'b': {
                              hint = X.begin();
                              if (hint != X.end()) {
                                  ++hint;
                              }
                          } break;
                          case 'c': {
                              hint = X.end();
                          } break;
                          case 'd': {
                              hint = X.find(VALUES[tj]);
                              ASSERTV(IS_UNIQ == (hint == X.end()));
                          } break;
                          default: {
                              ASSERTV(CONFIG, !"Unexpected configuration");
                          }
                        }

                        bslma::TestAllocator scratch("scratch",
                                                     veryVeryVeryVerbose);
                        KeyAllocator         xscratch(&scratch);

                        Iter RESULT;
                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            Obj mZ(xscratch); const Obj& Z = mZ;
                            for (size_t tk = 0; tk < tj; ++tk) {
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

                            bsls::ObjectBuffer<ValueType>  buffer;
                            ValueType                     *valptr =
                                                              buffer.address();
                            TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        xsa);
                            bslma::DestructorGuard<ValueType> guard(valptr);
                            RESULT = mX.insert(hint, MoveUtil::move(*valptr));

                            proctor.release();

                            ASSERTV(LINE, CONFIG, tj, SIZE,
                                    VALUES[tj] == *RESULT);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        if (CONFIG == 'd' && hint != X.end()) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }

                        ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
                    }
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
    //: 1 The iterator returned refers to the newly inserted element.
    //:
    //: 2 The new element is move-inserted into the container.
    //:
    //: 3 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 4 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..2)
    //:
    //:   3 Ensure that the move constructor was called on the value type (if
    //:     that type has a mechanism to detect such) where the value has the
    //:     same allocator as that of the container and a different allocator
    //:     than that of the container.  (C-2)
    //:
    //:   4 Verify all allocations are from the object's allocator.  (C-3)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-4)
    //
    // Testing:
    //   iterator insert(value_type&& value);
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
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    const int MAX_LENGTH = 16;

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

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
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        xsa);

                    Iter RESULT = mX.insert(MoveUtil::move(*valptr));

                    MoveState::Enum mState = TstFacility::getMovedFromState(
                                                                      *valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    const MoveState::Enum exp =
                                         k_IS_KEY_MOVE_AWARE
                                         ? (k_IS_KEY_WELL_BEHAVED && &oa != &sa
                                                       ? MoveState::e_NOT_MOVED
                                                       : MoveState::e_MOVED)
                                                    : MoveState::e_UNKNOWN;

                    ASSERTV(mState, exp, NameOf<KEY>(), exp == mState);

                    ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                    ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <=  A);
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
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
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

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
                  } break;
                }
                bslma::TestAllocator& sa = *valAllocator;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        &sa);
                    bslma::DestructorGuard<ValueType> guard(valptr);

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);

                    Iter RESULT;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(xscratch); const Obj& Z = mZ;
                        for (size_t tk = 0; tk < tj; ++tk) {
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

                        RESULT = mX.insert(MoveUtil::move(*valptr));

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                    ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
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

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };

    enum { NUM_SPECS = sizeof SPECS / sizeof *SPECS };

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose)
        printf("\t'propagate_on_container_move_assignment::value == false'\n");

    testCase29_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase29_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_move_assignment::value == true'\n");

    testCase29_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase29_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR
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
    //   unordered_multiset& operator=(unordered_multiset&& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\tTesting signature.\n");

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\tTesting move assignment.\n");
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
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    KeyAllocator         xza(&za);
                    KeyAllocator         xoa(&oa);

                    Obj        *objPtr = new (fa) Obj(xoa);
                    Obj&        mX = *objPtr;
                    const Obj&  X = gg(&mX, SPEC2);

                    Obj                  *srcPtr = 0;
                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (fa) Obj(xza); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &za;
                      } break;
                      case 'b': {
                        srcPtr = new (fa) Obj(xoa); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }

                    Obj&                  mZ = *srcPtr;
                    const Obj&            Z = mZ;
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
                    bslma::TestAllocatorMonitor zam(&za);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    if (0 == LENGTH1) {
                        // assigned an empty unordered_multiset
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
                            // assigning to an empty unordered_multiset
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
                        ASSERTV(SPEC1, SPEC2, 0 == za.numBlocksTotal());
                    }
                    else {
                        // 1. each element in original move-inserted
                        ASSERTV(SPEC1,
                                SPEC2,
                                X.end() == TstMoveUtil::findFirstNotMovedInto(
                                               X.begin(), X.end()));

                        // 2. CONTAINER SPECIFIC NOTE: original object left
                        // empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 3. additional memory checks
                        ASSERTV(SPEC1, SPEC2, &sa == &oa,
                                empty || oam.isTotalUp());
                        ASSERTV(SPEC1, SPEC2, 0 == zam.isInUseUp());

                    }
                    // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                    ASSERTV(SPEC1, SPEC2, &scratch == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2, &oa      ==  X.get_allocator());
                    ASSERTV(SPEC1, SPEC2, &sa      ==  Z.get_allocator());

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.
                    Iter RESULT = primaryManipulator(&mZ, 'Z', xsa);
                    ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    fa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            empty ||
                                ((&sa == &oa) == (0 < sa.numBlocksInUse())));

                    // Verify subsequent manipulation of target object 'X'.
                    RESULT = primaryManipulator(&mX, 'Z', xoa);
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

                Obj        mX(xoa);
                const Obj& X  = gg(&mX,  SPEC1);
                Obj        mZZ(xscratch);
                const Obj& ZZ  = gg(&mZZ,  SPEC1);

                const Obj& Z = mX;

                ASSERTV(SPEC1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
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

                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        const bsls::Types::Int64 AL = oa.allocationLimit();
                        oa.setAllocationLimit(-1);
                        Obj mX(xoa); const Obj& X = gg(&mX, SPEC2);

                        bslma::TestAllocator *srcAllocatorPtr;
                        switch (CONFIG) {
                          case 'a': {
                            srcAllocatorPtr = &za;
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
                                              &Z,
                                              L_,
                                              bslmf::MovableRefUtil::move(mE));

                        oa.setAllocationLimit(AL);

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                        proctor.release();

                        // Manipulate source object 'Z' to ensure it is in a
                        // valid state and is independent of 'X'.
                        Iter RESULT = primaryManipulator(&mZ, 'Z', xsa);
                        ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                        ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                        // Verify subsequent manipulation of target object 'X'.
                        RESULT = primaryManipulator(&mX, 'Z', xoa);
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
    // TESTING MOVE CONSTRUCTOR
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
    //   unordered_multiset(unordered_multiset&& original);
    //   unordered_multiset(unordered_multiset&& original, const A&);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    static const char *DATA[] = {
        "",
        "A",
        "AA",
        "BC",
        "CB",
        "BBC",
        "BCB",
        "BCC",
        "CBB",
        "CBC",
        "CCB",
        "CCC",
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

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose)
        printf("\tTesting both versions of move constructor.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (veryVerbose) {
                printf("\t\tFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object ZZ with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mZZ(xscratch);
            const Obj&           ZZ = gg(&mZZ, SPEC);

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
                KeyAllocator         xsa(&sa);
                KeyAllocator         xza(&za);

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
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), ALLOC(0));
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

                // CONTAINER SPECIFIC NOTE: For 'unordered_multiset', the
                // original object is left in the default state even when the
                // source and target objects use different allocators because
                // move-insertion changes the value of the source key and
                // violates the requirements for keys contained in the
                // 'unordered_multiset'.

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
                       TstMoveUtil::findFirstNotMovedInto(X.begin(), X.end()));
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
                Iter RESULT = primaryManipulator(&mZ, 'Z', xsa);
                ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.
                RESULT = primaryManipulator(&mX, 'Z', xoa);
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
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            Obj                  mZZ(xscratch);
            const Obj&           ZZ = gg(&mZZ, SPEC);

            if (veryVerbose) {
                printf("\t\tFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",      veryVeryVeryVerbose);
            bslma::TestAllocator za("different",   veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            KeyAllocator         xza(&za);

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64 B  = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj        mE(xscratch);
                Obj        mZ(xza);
                const Obj& Z = gg(&mZ, SPEC);

                ExceptionProctor<Obj, ALLOC> proctor(&Z,
                                                     L_,
                                                     MoveUtil::move(mE));

                const Obj X(bslmf::MovableRefUtil::move(mZ), xoa);

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
                Iter RESULT = primaryManipulator(&mZ, 'Z', xza);
                ASSERTV(RESULT != Z.end());

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
    // Testing
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

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    TestValues VALUES;
    const KEY  kA(VALUES[0]);

    Obj mX; const Obj& X = mX;

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
            size_t      b   = X.bucket(kA);
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

    Obj mX10; const Obj& X10 = mX10;
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const int    NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da     ("default", veryVeryVeryVerbose);
    bslma::TestAllocator         scratch("scratch", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int    LINE   = DATA[ti].d_line;
        const char  *SPEC   = DATA[ti].d_spec;
        const size_t LENGTH = strlen(SPEC);

        TestValues values(SPEC, &scratch);

        if (veryVeryVerbose) printf("\t\t\tTest 'reserve'\n");
        {
            Obj mX(values.begin(), values.end());  const Obj& X = mX;
            values.resetIterators();
            Obj mY(X);                             const Obj& Y = mY;

            const size_t COUNT = X.bucket_count();

            mX.reserve(my_max<size_t>(X.size(), 1) * 3);

            ASSERTV(X == Y);

            ASSERTV(X.size(), X.bucket_count() > COUNT);
        }

        if (veryVeryVerbose) printf("\t\t\tTest 'reserve' before insert\n");
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

    // Verify unordered_multiset defines the expected traits.

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
    //  CONCERN: 'unordered_multiset' is compatible with a standard allocator.
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const int    NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::unordered_multiset<KEY, HASH, EQUAL, StlAlloc> ObjStlAlloc;

    StlAlloc scratch;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const char *const EXP    = DATA[ti].d_results;
        const size_t      LENGTH = strlen(SPEC);

        TestValues VALUES(SPEC, scratch);

        typename TestValues::iterator BEGIN = VALUES.begin();
        typename TestValues::iterator END   = VALUES.end();

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            ObjStlAlloc mX(BEGIN, END); const ObjStlAlloc& X = mX;

            const ptrdiff_t v = verifySpec(X, EXP);
            ASSERTV(LINE, v, EXP, X.size(), -1 == v);
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 <= da.numBlocksInUse()
                       - TYPE_ALLOC * static_cast<int>(LENGTH));

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, -1 == verifySpec(Y, EXP));
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 == da.numBlocksInUse()
                       - 2 * TYPE_ALLOC * static_cast<int>(LENGTH));

            ObjStlAlloc mZ; const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, -1 == verifySpec(Z, EXP));
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 <= da.numBlocksInUse()
                       - 2 * TYPE_ALLOC * static_cast<int>(LENGTH));
        }

        VALUES.resetIterators();

        {
            ObjStlAlloc mX; const ObjStlAlloc& X = mX;

            mX.insert(BEGIN, END);

            ASSERTV(LINE, -1 == verifySpec(X, EXP));
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 == da.numBlocksInUse() - TYPE_ALLOC * LENGTH);
        }

        VALUES.resetIterators();

        {
            ObjStlAlloc mX; const ObjStlAlloc& X = mX;

            for (size_t tj = 0; tj < VALUES.size(); ++tj) {
                Iter RESULT = mX.insert(VALUES[tj]);

                ASSERTV(LINE, tj, LENGTH, VALUES[tj] == *RESULT);
            }
            ASSERTV(LINE, -1 == verifySpec(X, EXP));
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 <= da.numBlocksInUse()
                       - TYPE_ALLOC * static_cast<int>(LENGTH));
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
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING 'hash_function' AND 'key_eq'
    //
    // Concern:
    //: 1 The hash function and key equal comparator is set correctly.
    //:
    //: 2 'hash_function' and 'key_eq' return the functions that were passed in
    //:   on construction.
    //:
    //: 3 Functions are properly propagated on copy construction, copy
    //:   assignment, and swap.
    //
    // Plan:
    //:  TDB
    //
    // Testing:
    //   hasher hash_function() const;
    //   key_equal key_eq() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

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

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

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

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mW(BEGIN, END, 0, DH, E); const Obj& W = mW;

            ASSERTV(LINE, -1 == verifySpec(W, SPEC));
            ASSERTV(LINE,  H == W.hash_function());
            ASSERTV(LINE,  E == W.key_eq());

            Obj mX(W); const Obj& X = mX;

            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
            ASSERTV(LINE,  H == X.hash_function());
            ASSERTV(LINE,  E == X.key_eq());

            Obj mY;  const Obj& Y = mY;
            mY = mW;
            ASSERTV(LINE, -1 == verifySpec(Y, SPEC));
            ASSERTV(LINE,  H == Y.hash_function());
            ASSERTV(LINE,  E == Y.key_eq());

            Obj mZ;  const Obj& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, -1 == verifySpec(Z, SPEC));
            ASSERTV(LINE,  H == Z.hash_function());
            ASSERTV(LINE,  E == Z.key_eq());
            ASSERTV(LINE,  H == W.hash_function());
            ASSERTV(LINE,  E == W.key_eq());
        }

        CONT.resetIterators();

        {
            Obj mX(0, DH, E); const Obj& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            Obj mX(0, DH, E); const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

#if 1 // TBD non-'const' comparator
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        TestNonConstHashFunctor<KEY>           NCH;
        TestNonConstEqualityComparator<KEY>    NCE;
        typedef bsl::unordered_multiset<KEY,
                                        TestNonConstHashFunctor<KEY>,
                                        TestNonConstEqualityComparator<KEY>,
                                        ALLOC> ObjNCH;

        {
            ObjNCH mW(BEGIN, END); const ObjNCH& W = mW;

            ASSERTV(LINE, -1  == verifySpec(W, SPEC));
            ASSERTV(LINE, NCH == W.hash_function());
            ASSERTV(LINE, NCE == W.key_eq());

            ObjNCH mX(W); const ObjNCH& X = mX;

            ASSERTV(LINE, -1  == verifySpec(X, SPEC));
            ASSERTV(LINE, NCH == X.hash_function());
            ASSERTV(LINE, NCE == X.key_eq());

            ObjNCH mY; const ObjNCH& Y = mY;
            mY = mW;
            ASSERTV(LINE, -1  == verifySpec(Y, SPEC));
            ASSERTV(LINE, NCH == Y.hash_function());
            ASSERTV(LINE, NCE == Y.key_eq());

            ObjNCH mZ; const ObjNCH& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, -1  == verifySpec(Z, SPEC));
            ASSERTV(LINE, NCH == Z.hash_function());
            ASSERTV(LINE, NCH == W.hash_function());
            ASSERTV(LINE, NCE == Z.key_eq());
            ASSERTV(LINE, NCE == W.key_eq());
            ASSERTV(LINE, TestNonConstHashFunctor<KEY>() == W.hash_function());
            ASSERTV(LINE, TestNonConstEqualityComparator<KEY>() == W.key_eq());
        }

        CONT.resetIterators();

        {
            ObjNCH mX; const ObjNCH& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            ObjNCH mX; const ObjNCH& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, -1 == verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }
#endif
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING 'max_size' AND 'empty'
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

    bslma::TestAllocator  oa("object", veryVeryVerbose);

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
        const char *d_spec;     // initial spec
    } DATA[] = {
        //line  spec
        //----  -----------
        { L_,   ""          },
        { L_,   "A"         },
        { L_,   "AA"        },
        { L_,   "ABC"       },
        { L_,   "ABCD"      },
        { L_,   "ABCA"      },
        { L_,   "ABCDE"     },
        { L_,   "ABCDEFG"   },
        { L_,   "ABCDEFGH"  },
        { L_,   "ABCDEFGHI" }
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa); const Obj& X = gg(&mX, SPEC);

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
    //: 2 'erase' with 'key' returns the number of elements removed (0 if
    //:    there are no elements equivalent to the 'key'
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

    const int    NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose)
        printf("\tTesting 'erase(pos)' on non-empty unordered_multiset.\n");
    {
        for (int ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_spec);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, xoa); const Obj& Y = mY;

                Iter pos    = getIterForIndex(X, tj);
                Iter before = 0 == tj ? mX.end()
                                      : getIterForIndex(X, tj - 1);
                Iter after = pos; ++after;

                const KEY k(*pos);

                const size_t COUNT = X.count(k);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor dam(&oa);

                {
                    Iter RESULT;
                    int  numPasses = 0;
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
                ASSERTV(X.size()  == LENGTH - 1);

                for (CIter it = Y.begin(); Y.end() != it; ++it) {
                    if (k != *it) {
                        ASSERTV(*it, k, X.count(*it), X.count(*it) > 0);
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

    if (verbose)
        printf("\tTesting 'erase(key)' on non-empty unordered_multiset.\n");
    {
        for (int ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_spec);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);    const Obj& X = gg(&mX, SPEC);
                Obj mY(X, xoa); const Obj& Y = mY;

                Iter pos = getIterForIndex(X, tj);

                const KEY k(*pos);

                bsl::pair<Iter, Iter> range = mX.equal_range(k);

                Iter before = mX.begin();
                if (mX.begin() == range.first) {
                    before = mX.end();
                }
                else {
                    Iter afterBefore = before;
                    while (mX.end() != afterBefore &&
                           range.first != ++afterBefore) {
                        ++before;
                    }
                }

                Iter after = range.second;

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

                if (before == X.end()) {
                    ASSERTV(X.begin() == after);
                }
                else {
                    pos = before; ++pos;
                    ASSERTV(X, pos == after);
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'erase(first, last)'.\n");
    {
//#warning "TDB: non-trivial testing"
        if (verbose) printf("\t\tTBD: Testing 'erase(first, last)'.\n");
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        {
            const TestValues VALUES;

            Obj  mX;
            Iter it = mX.insert(VALUES[0]);

            ASSERT_FAIL(mX.erase(mX.end()));
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
    //: 3 Repeated values are also inserted.
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

    const int    NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE     = DATA[ti].d_line;
        const char *const SPEC     = DATA[ti].d_spec;
        const char *const EXP_SPEC = DATA[ti].d_results;

        TestValues CONT(SPEC);

        for (size_t tj = 0; tj <= CONT.size(); ++tj) {

            CONT.resetIterators();
            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator MID   = CONT.index(tj);
            typename TestValues::iterator END   = CONT.end();

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator oa("object", veryVeryVerbose);
            KeyAllocator         xoa(&oa);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, xoa); const Obj& X = mX;

            bslma::TestAllocatorMonitor oam(&oa);

            mX.insert(MID, END);

            if (tj == CONT.size()) {
                ASSERTV(LINE, oam.isTotalSame());
            }
            ASSERTV(LINE, EXP_SPEC, X, -1 == verifySpec(X, EXP_SPEC));

            ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element.
    //:
    //: 2 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 3 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 4 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'begin', 'begin' + 1,
    //:     'end', find(key).
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Verify the return value and the resulting data in the container
    //:         is as expected.  (C-1)
    //:
    //:       2 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-2,3)
    //:
    //:       3 Verify all allocations are from the object's allocator.  (C-4)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-5)
    //
    // Testing:
    //   iterator insert(const_iterator position, const value_type& value);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    const int MAX_LENGTH = 16;

    if (verbose) printf("\tTesting 'insert' with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);
            char             EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ P_(SIZE); P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj]);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    Iter RESULT = mX.insert(hint, VALUES[tj]);

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'insert' with exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);
            char             EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.begin();
                      } break;
                      case 'b': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'c': {
                          hint = X.end();
                      } break;
                      case 'd': {
                          hint = X.find(VALUES[tj]);
                          ASSERTV(IS_UNIQ == (hint == X.end()));
                      } break;
                      default: {
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);
                    Iter                 RESULT;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                        RESULT = mX.insert(hint, VALUES[tj]);
                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (CONFIG == 'd' && hint != X.end()) {
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
                }
            }
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
    //: 1 'insert' adds an additional element to the object and return the
    //:   iterator to the newly added element.
    //:
    //: 2 Inserting duplicated value do not overwrite previous ones.
    //:
    //: 3 Duplicated values are inserted at the end of its range.
    //:
    //: 4 A new element is added to the container.
    //:
    //: 5 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
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
    //: 2 Repeat P-1 under the presence of exception  (C-6)
    //
    // Testing:
    //   iterator insert(const value_type& value);
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
        //line  spec           unique
        //----  -------------- --------------
        { L_,   "A",           "Y"            },
        { L_,   "AAA",         "YNN"          },
        { L_,   "ABCDEFGH",    "YYYYYYYY"     },
        { L_,   "AABBCCDDEE",  "YNYNYNYNYN"   },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"   },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY"  },
    };

    enum { NUM_DATA = sizeof DATA / sizeof *DATA };
    const int MAX_LENGTH = 16;

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);
            char             EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                Iter RESULT = mX.insert(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LINE, tj, AA, BB, BB + TYPE_ALLOC <= AA);
                ASSERTV(LINE, tj,  A,  B,  B + TYPE_ALLOC <= A);
                ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
            }
        }
    }

    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);
            char             EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);
            Obj                  mX(xoa);
            const Obj&           X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const size_t SIZE    = X.size();

                if (veryVerbose) { P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { T_ T_ P(EXPECTED); }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);
                Iter                 RESULT;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    RESULT = mX.insert(VALUES[tj]);
                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                ASSERTV(LINE, tj, -1 == verifySpec(X, EXPECTED));
            }
        }
    }
}

template <class KEY, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, HASH, EQUAL, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 The range '[begin(), end())' contains all values inserted into the
    //:   container.
    //:
    //: 2 'iterator' is a pointer to 'const KEY'.
    //:
    //: 3 'const_iterator' is a pointer to 'const KEY'.
    //:
    //
    // Plan:
    //: 1 For each value given by variety of specifications of different
    //:   lengths:
    //:
    //:   1 Create an object this value, and access each element in sequence,
    //:     as a modifiable reference (setting it to a default value, then back
    //:     to its original value, and as a non-modifiable reference.  (C-1)
    //:
    //: 2 Use 'bsl::is_same' to assert the identity of iterator types.
    //:   (C-2..3)
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_iterator cbegin() const;
    //   const_iterator cend() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    BSLMF_ASSERT((bsl::is_same<Iter, CIter>::value));

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

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
            const int     LINE   = DATA[ti].d_line;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            bslma::TestAllocator oa(veryVeryVerbose);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { T_ T_ P_(LINE); P(SPEC); }

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
                    Iter ret = mY.insert(*iter);
                    ASSERTV(*ret == *iter);
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
    //: 1 If the key being searched exists in the container, 'find' returns the
    //:   iterator referring the existing element and 'contains' returns
    //:   'true'.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator and 'contains' returns 'false'.
    //:
    //: 3 'equal_range(key)' returns all elements equivalent to 'key'.
    //:
    //: 4 'count' returns the number of elements with the same value as defined
    //:   by the comparator.
    //:
    //: 5 Both the 'const' and non-'const' versions returns the same value.
    //:
    //: 6 No memory is allocated, from either the object allocator nor from
    //:   the default allocator.
    //
    // Plan:
    //:  TDB
    //
    // Testing:
    //   bool contains(const key_type& key);
    //   bool contains(const LOOKUP_KEY& key);
    //   size_type count(const key_type& key) const;
    //   bsl::pair<iterator, iterator> equal_range(const key_type& key);
    //   bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
    //   iterator find(const key_type& key);
    //   const_iterator find(const key_type& key) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    BSLMF_ASSERT((bslmf::IsSame<Iter, CIter>::VALUE));

    typedef bsl::pair<Iter, Iter> Range;

    if (verbose) printf("\tTesting Signatures\n");
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
        typedef bool (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::contains;
        (void) mp;
    }
    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::count;
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

    if (verbose) printf("\tTesting methods\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocator         oa("object",  veryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    TestValues VALUES(&oa);

    for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
        const char *SPEC = DEFAULT_DATA[i].d_spec;

        if (veryVeryVerbose) { T_ T_ P_(SPEC) }

        TestValues tv(SPEC, &oa);

        Obj mX(tv.begin(), tv.end()); const Obj& X = mX;

        for (char c = 'A'; c < 'Z'; ++c) {
            KEY key = KEY(VALUES[c - 'A']);

            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            Iter it;
            int  numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                it = mX.find(key);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERT(it == X.find(key));
            ASSERT(1 == numPasses);

            const bool expectContains = (mX.end() != it);
            ASSERT(expectContains == mX.contains(key));

            const size_t expectedCount = numCharInstances(SPEC, c);

            if (expectedCount) {
                ASSERT(isConstValue(*it));

                ASSERT(key == *it);
                ASSERT(BSLS_UTIL_ADDRESSOF(key) != BSLS_UTIL_ADDRESSOF(*it));

                ASSERT(expectedCount == mX.count(key));
                ASSERT(expectedCount ==  X.count(key));
            }
            else {
                ASSERT(X.end() == it);

                ASSERT(0 == mX.count(key));
                ASSERT(0 ==  X.count(key));
            }

            Range range;
            {
                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numPasses;

                    range = mX.equal_range(key);
                    ASSERT(X.equal_range(key) == range);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERT(1 == numPasses);
            }

            ASSERTV(it == range.first);

            if (expectedCount) {
                Iter after = range.first;
                for (size_t count = expectedCount; count > 0; --count) {
                    ASSERT(key == *after);
                    ++after;
                }
                ASSERT(after == range.second);
            }
            else {
                ASSERT(range.first == range.second);
                ASSERT(X.end()     == range.first);
            }

            ASSERT(oam.isTotalSame());
            ASSERT(dam.isTotalSame());
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
    //: 1 That all c'tors taking a range of objects of type 'KEY' function
    //:   correctly.
    //
    // Plan:
    //: TBD
    //
    // Testing:
    //   unordered_multiset(ITER, ITER, allocator);
    //   unordered_multiset(ITER, ITER, size_type, allocator);
    //   unordered_multiset(ITER, ITER, size_type, hasher, allocator);
    //   unordered_multiset(ITER, ITER, size_type, hasher, key_equal, alloc);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    TestValues VALUES;

    if (verbose) printf("\tTest 'count'\n");
    {
        for (int i = 0; i < DEFAULT_NUM_DATA; ++i) {
            const int   LINE = DEFAULT_DATA[i].d_line;
            const char *SPEC = DEFAULT_DATA[i].d_spec;

            Obj mX;    const Obj& X = gg(&mX, SPEC);

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

    if (verbose) printf("\tTest range creation\n");
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

            if (veryVerbose) { T_ T_ P(SPEC); }

            int done = 0;
            for (char cfg = 'a'; cfg <= 'n'; ++cfg) {
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
                bslma::TestAllocator& noa = &oa == &da ? sa : da;

                Obj *pmX;
                int  numPasses = 0;
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
                        ++done;
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

                Obj& mX = *pmX; const Obj& X = mX;

                ASSERTV(CONFIG, LENGTH, noa.numBlocksTotal(), &oa == &da,
                                                    0 == noa.numBlocksTotal());

                ASSERT(-1 == verifySpec(X, RESULTS));

                ASSERTV((strchr("cdefjklm", CONFIG) ?
                                 defaultHash  : HASH())  == X.hash_function());
                ASSERTV((strchr("dekl",   CONFIG) ?
                                 defaultEqual : EQUAL()) == X.key_eq());

                ASSERTV(CONFIG, SPEC, X.bucket_count(),
                                      CONFIG < 'i' || 100 <= X.bucket_count());

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

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::unordered_multiset' cannot be deduced from the constructor
    // parameters.
    //..
    // unordered_multiset()
    // explicit unordered_multiset(size_t, HASH=HASH(), EQUAL=EQUAL(),
    //                                                  ALLOCATOR=ALLOCATOR());
    // unordered_multiset(size_t, HASH, EQUAL);
    // unordered_multiset(size_t, ALLOCATOR);
    // explicit unordered_multiset(ALLOCATOR);
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
        // Test that constructing a 'bsl::unordered_multiset' from various
        // combinations of arguments deduces the correct type.
        //..
        // unordered_multiset(const unordered_multiset&  s) -> decltype(s)
        // unordered_multiset(const unordered_multiset&  s, ALLOCATOR)
        //                                                  -> decltype(s)
        // unordered_multiset(      unordered_multiset&& s) -> decltype(s)
        // unordered_multiset(      unordered_multiset&& s, ALLOCATOR)
        //                                                  -> decltype(s)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::unordered_multiset<T1> ums1;
        bsl::unordered_multiset     ums1a(ums1);
        ASSERT_SAME_TYPE(decltype(ums1a), bsl::unordered_multiset<T1>);

        typedef float T2;
        bsl::unordered_multiset<T2> ums2;
        bsl::unordered_multiset     ums2a(ums2, bsl::allocator<T2>());
        bsl::unordered_multiset     ums2b(ums2, a1);
        bsl::unordered_multiset     ums2c(ums2, a2);
        bsl::unordered_multiset     ums2d(ums2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(ums2a), bsl::unordered_multiset<T2>);
        ASSERT_SAME_TYPE(decltype(ums2b), bsl::unordered_multiset<T2>);
        ASSERT_SAME_TYPE(decltype(ums2c), bsl::unordered_multiset<T2>);
        ASSERT_SAME_TYPE(decltype(ums2d), bsl::unordered_multiset<T2>);

        typedef short T3;
        bsl::unordered_multiset<T3> ums3;
        bsl::unordered_multiset     ums3a(std::move(ums3));
        ASSERT_SAME_TYPE(decltype(ums3a), bsl::unordered_multiset<T3>);

        typedef long double T4;
        typedef bsl::allocator<T4> BA4;
        bsl::unordered_multiset<T4> ums4;
        bsl::unordered_multiset     ums4a(std::move(ums4), BA4{});
        bsl::unordered_multiset     ums4b(std::move(ums4), a1);
        bsl::unordered_multiset     ums4c(std::move(ums4), a2);
        bsl::unordered_multiset     ums4d(std::move(ums4),
                                          bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(ums4a), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4b), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4c), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4d), bsl::unordered_multiset<T4>);
    }

    void TestIteratorConstructors ()
        // Test that constructing a 'bsl::unordered_multiset' from a pair of
        // iterators and various combinations of other arguments deduces the
        // correct type.
        //..
        // unordered_multiset(Iter, Iter, size_type = N, HASH=HASH(),
        //                               EQUAL=EQUAL(), ALLOCATOR=ALLOCATOR());
        // unordered_multiset(Iter, Iter, size_type, HASH, ALLOCATOR);
        // unordered_multiset(Iter, Iter, size_type, ALLOCATOR);
        // unordered_multiset(Iter, Iter, ALLOCATOR)
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

        T1                                    *p1b = nullptr;
        T1                                    *p1e = nullptr;
        bsl::unordered_multiset<T1>::iterator  i1b;
        bsl::unordered_multiset<T1>::iterator  i1e;

        bsl::unordered_multiset ums1a(p1b, p1e);
        bsl::unordered_multiset ums1b(i1b, i1e);
        bsl::unordered_multiset ums1c(p1b, p1e, 3);
        bsl::unordered_multiset ums1d(i1b, i1e, 3);

        bsl::unordered_multiset ums1e(p1b, p1e, 3, HashT1{});
        bsl::unordered_multiset ums1f(p1b, p1e, 3, StupidHashFn<T1>);
        bsl::unordered_multiset ums1g(i1b, i1e, 3, HashT1{});
        bsl::unordered_multiset ums1h(i1b, i1e, 3, StupidHashFn<T1>);

        bsl::unordered_multiset ums1i(p1b, p1e, 3, HashT1{}, EqualT1{});
        bsl::unordered_multiset ums1j(i1b, i1e, 3, HashT1{},
                                                            StupidEqualFn<T1>);
        bsl::unordered_multiset ums1k(p1b, p1e, 3, StupidHashFn<T1>,
                                                                    EqualT1{});
        bsl::unordered_multiset ums1l(i1b, i1e, 3,
                                          StupidHashFn<T1>, StupidEqualFn<T1>);

        bsl::unordered_multiset ums1m(p1b, p1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_multiset ums1n(p1b, p1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_multiset ums1o(p1b, p1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_multiset ums1p(p1b, p1e, 3, HashT1{}, EqualT1{}, SA1{});
        bsl::unordered_multiset ums1q(i1b, i1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_multiset ums1r(i1b, i1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_multiset ums1s(i1b, i1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_multiset ums1t(i1b, i1e, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(ums1a), bsl::unordered_multiset<T1>);
        ASSERT_SAME_TYPE(decltype(ums1b), bsl::unordered_multiset<T1>);
        ASSERT_SAME_TYPE(decltype(ums1c), bsl::unordered_multiset<T1>);
        ASSERT_SAME_TYPE(decltype(ums1d), bsl::unordered_multiset<T1>);
        ASSERT_SAME_TYPE(decltype(ums1e), bsl::unordered_multiset<T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(ums1f),
                         bsl::unordered_multiset<T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(ums1g), bsl::unordered_multiset<T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(ums1h),
                         bsl::unordered_multiset<T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(ums1i),
                         bsl::unordered_multiset<T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(ums1j),
                         bsl::unordered_multiset<T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(ums1k),
                         bsl::unordered_multiset<T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(decltype(ums1l),
                         bsl::unordered_multiset<T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(decltype(ums1m),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1n),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1o),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1p),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, SA1>);
        ASSERT_SAME_TYPE(decltype(ums1q),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1r),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1s),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1t),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, SA1>);

        typedef double                      T2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        typedef bsl::allocator<T2>          BA2;
        typedef std::allocator<T2>          SA2;
        T2                                    *p2b = nullptr;
        T2                                    *p2e = nullptr;
        bsl::unordered_multiset<T2>::iterator  i2b;
        bsl::unordered_multiset<T2>::iterator  i2e;

        bsl::unordered_multiset ums2a(p2b, p2e, 3, HashT2{}, BA2{});
        bsl::unordered_multiset ums2b(p2b, p2e, 3, HashT2{}, a1);
        bsl::unordered_multiset ums2c(p2b, p2e, 3, HashT2{}, a2);
        bsl::unordered_multiset ums2d(p2b, p2e, 3, HashT2{}, SA2{});
        bsl::unordered_multiset ums2e(p2b, p2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_multiset ums2f(p2b, p2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_multiset ums2g(p2b, p2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_multiset ums2h(p2b, p2e, 3, StupidHashFn<T2>, SA2{});
        bsl::unordered_multiset ums2i(i2b, i2e, 3, HashT2{}, BA2{});
        bsl::unordered_multiset ums2j(i2b, i2e, 3, HashT2{}, a1);
        bsl::unordered_multiset ums2k(i2b, i2e, 3, HashT2{}, a2);
        bsl::unordered_multiset ums2l(i2b, i2e, 3, HashT2{}, SA2{});
        bsl::unordered_multiset ums2m(i2b, i2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_multiset ums2n(i2b, i2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_multiset ums2o(i2b, i2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_multiset ums2p(i2b, i2e, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(ums2a), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(ums2b), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(ums2c), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(
                  decltype(ums2d),
                  bsl::unordered_multiset<T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(ums2e),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(ums2f),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(ums2g),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
              decltype(ums2h),
              bsl::unordered_multiset<T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(ums2i), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(ums2j), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(ums2k), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(
                  decltype(ums2l),
                  bsl::unordered_multiset<T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(ums2m),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(ums2n),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(ums2o),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
              decltype(ums2p),
              bsl::unordered_multiset<T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                T3;
        typedef bsl::allocator<T3> BA3;
        typedef std::allocator<T3> SA3;
        T3                                    *p3b = nullptr;
        T3                                    *p3e = nullptr;
        bsl::unordered_multiset<T3>::iterator  i3b;
        bsl::unordered_multiset<T3>::iterator  i3e;

        bsl::unordered_multiset ums3a(p3b, p3e, 3, BA3{});
        bsl::unordered_multiset ums3b(p3b, p3e, 3, a1);
        bsl::unordered_multiset ums3c(p3b, p3e, 3, a2);
        bsl::unordered_multiset ums3d(p3b, p3e, 3, SA3{});
        bsl::unordered_multiset ums3e(i3b, i3e, 3, BA3{});
        bsl::unordered_multiset ums3f(i3b, i3e, 3, a1);
        bsl::unordered_multiset ums3g(i3b, i3e, 3, a2);
        bsl::unordered_multiset ums3h(i3b, i3e, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(ums3a), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(decltype(ums3b), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(decltype(ums3c), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(
           decltype(ums3d),
           bsl::unordered_multiset<T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);
        ASSERT_SAME_TYPE(decltype(ums3e), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(decltype(ums3f), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(decltype(ums3g), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(
           decltype(ums3h),
           bsl::unordered_multiset<T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);

        typedef char               T4;
        typedef bsl::allocator<T4> BA4;
        typedef std::allocator<T4> SA4;
        T4                                    *p4b = nullptr;
        T4                                    *p4e = nullptr;
        bsl::unordered_multiset<T4>::iterator  i4b;
        bsl::unordered_multiset<T4>::iterator  i4e;

        bsl::unordered_multiset ums4a(p4b, p4e, BA4{});
        bsl::unordered_multiset ums4b(p4b, p4e, a1);
        bsl::unordered_multiset ums4c(p4b, p4e, a2);
        bsl::unordered_multiset ums4d(p4b, p4e, SA4{});
        bsl::unordered_multiset ums4e(i4b, i4e, BA4{});
        bsl::unordered_multiset ums4f(i4b, i4e, a1);
        bsl::unordered_multiset ums4g(i4b, i4e, a2);
        bsl::unordered_multiset ums4h(i4b, i4e, SA4{});

        ASSERT_SAME_TYPE(decltype(ums4a), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4b), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4c), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(
           decltype(ums4d),
           bsl::unordered_multiset<T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);

        ASSERT_SAME_TYPE(decltype(ums4e), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4f), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4g), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(
           decltype(ums4h),
           bsl::unordered_multiset<T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);
    }

    void TestStdInitializerListConstructors ()
        // Test that constructing a 'bsl::unordered_multiset' from an
        // initializer_list and various combinations of other arguments deduces
        // the correct type.
        //..
        // unordered_multiset(initializer_list, size_type = N, HASH=HASH(),
        //                               EQUAL=EQUAL(), ALLOCATOR=ALLOCATOR());
        // unordered_multiset(initializer_list, size_type, HASH, ALLOCATOR);
        // unordered_multiset(initializer_list, size_type, ALLOCATOR);
        // unordered_multiset(initializer_list, ALLOCATOR)
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

        bsl::unordered_multiset ums1a(il1);
        bsl::unordered_multiset ums1b(il1, 3);
        bsl::unordered_multiset ums1c(il1, 3, HashT1{});
        bsl::unordered_multiset ums1d(il1, 3, StupidHashFn<T1>);
        bsl::unordered_multiset ums1e(il1, 3, HashT1{}, EqualT1{});
        bsl::unordered_multiset ums1f(il1, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_multiset ums1g(il1, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_multiset ums1h(il1,
                                      3,
                                      StupidHashFn<T1>,
                                      StupidEqualFn<T1>);

        bsl::unordered_multiset ums1i(il1, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_multiset ums1j(il1, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_multiset ums1k(il1, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_multiset ums1l(il1, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(ums1a), bsl::unordered_multiset<T1>);
        ASSERT_SAME_TYPE(decltype(ums1b), bsl::unordered_multiset<T1>);
        ASSERT_SAME_TYPE(decltype(ums1c), bsl::unordered_multiset<T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(ums1d),
                         bsl::unordered_multiset<T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(ums1e),
                         bsl::unordered_multiset<T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(ums1f),
                         bsl::unordered_multiset<T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(ums1g),
                         bsl::unordered_multiset<T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(decltype(ums1h),
                         bsl::unordered_multiset<T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(decltype(ums1i),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1j),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1k),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(ums1l),
                         bsl::unordered_multiset<T1, HashT1, EqualT1, SA1>);

        typedef double                      T2;
        typedef bsl::allocator<T2>          BA2;
        typedef std::allocator<T2>          SA2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        std::initializer_list<T2> il2 = {1.0, 2.0, 3.0, 4.0};

        bsl::unordered_multiset ums2a(il2, 3, HashT2{}, BA2{});
        bsl::unordered_multiset ums2b(il2, 3, HashT2{}, a1);
        bsl::unordered_multiset ums2c(il2, 3, HashT2{}, a2);
        bsl::unordered_multiset ums2d(il2, 3, HashT2{}, SA2{});
        bsl::unordered_multiset ums2e(il2, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_multiset ums2f(il2, 3, StupidHashFn<T2>, a1);
        bsl::unordered_multiset ums2g(il2, 3, StupidHashFn<T2>, a2);
        bsl::unordered_multiset ums2h(il2, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(ums2a), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(ums2b), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(ums2c), bsl::unordered_multiset<T2, HashT2>);
        ASSERT_SAME_TYPE(
                  decltype(ums2d),
                  bsl::unordered_multiset<T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(ums2e),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(ums2f),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(ums2g),
                         bsl::unordered_multiset<T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
              decltype(ums2h),
              bsl::unordered_multiset<T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                T3;
        typedef bsl::allocator<T3> BA3;
        typedef std::allocator<T3> SA3;
        std::initializer_list<T3> il3 = {1, 2, 3, 4};

        bsl::unordered_multiset ums3a(il3, 3, BA3{});
        bsl::unordered_multiset ums3b(il3, 3, a1);
        bsl::unordered_multiset ums3c(il3, 3, a2);
        bsl::unordered_multiset ums3d(il3, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(ums3a), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(decltype(ums3b), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(decltype(ums3c), bsl::unordered_multiset<T3>);
        ASSERT_SAME_TYPE(
           decltype(ums3d),
           bsl::unordered_multiset<T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);

        typedef char               T4;
        typedef bsl::allocator<T4> BA4;
        typedef std::allocator<T4> SA4;
        std::initializer_list<T4> il4 = {'1', '2', '3', '4'};

        bsl::unordered_multiset ums4a(il4, BA4{});
        bsl::unordered_multiset ums4b(il4, a1);
        bsl::unordered_multiset ums4c(il4, a2);
        bsl::unordered_multiset ums4d(il4, SA4{});

        ASSERT_SAME_TYPE(decltype(ums4a), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4b), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ums4c), bsl::unordered_multiset<T4>);
        ASSERT_SAME_TYPE(
           decltype(ums4d),
           bsl::unordered_multiset<T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);
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
      case 38: {
        // --------------------------------------------------------------------
        // TESTING ERASE_IF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTION 'BSL::ERASE_IF'"
                            "\n=====================================\n");

        TestDriver<char>::testCase38();
        TestDriver<int>::testCase38();
        TestDriver<long>::testCase38();
      } break;
      case 37: {
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
      case 36: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        //: 1 'unordered_multiset' has does not have a transparent set of
        //:   lookup functions if the comparator is not transparent.
        //:
        //: 2 'unordered_multiset' has a transparent set of lookup functions if
        //:   the comparator is transparent.
        //
        // Plan:
        //: 1 Construct a non-transparent multiset and call the lookup
        //:   functions with a type that is convertible to the 'value_type'.
        //:   There should be exactly one conversion per call to a lookup
        //:   function.  (C-1)
        //:
        //: 2 Construct a transparent multiset and call the lookup functions
        //:   with a type that is convertible to the 'value_type'.  There
        //:   should be no conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: 'find'        properly handles transparent comparators.
        //   CONCERN: 'count'       properly handles transparent comparators.
        //   CONCERN: 'equal_range' properly handles transparent comparators.
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::unordered_multiset<int>            NonTransparentMultiset;
        typedef bsl::unordered_multiset<int,
            TransparentHasher, TransparentComparator>   TransparentMultiset;

        const int DATA[] = { 0, 1, 2, 3, 4 };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        NonTransparentMultiset        mXNT;
        const NonTransparentMultiset& XNT = mXNT;

        mXNT.insert(0);
        for (int i = 0; i < NUM_DATA; ++i) {
            for (int j = 0; j < i; ++j) {
                if (veryVeryVeryVerbose) {
                    printf("Constructing test data.\n");
                }
                mXNT.insert(i);
            }
        }

        TransparentMultiset        mXT(mXNT.begin(), mXNT.end());
        const TransparentMultiset& XT = mXT;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int KEY = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a key of %d\n",
                       KEY);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent multiset.\n");
            }
            testTransparentComparator( XNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent multiset.\n");
            }
            testTransparentComparator(mXNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting const transparent multiset.\n");
            }
            testTransparentComparator( XT,  true,  KEY);

            if (veryVerbose) {
                printf("\tTesting mutable transparent multiset.\n");
            }
            testTransparentComparator(mXT,  true,  KEY);
        }
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------
        if (verbose) printf("'noexcept' SPECIFICATION\n"
                            "========================\n");

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

        TestDriver<int>::testCase35();

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

        TestDriver<int, TH,  TP,  AFA>::testCase35();
        TestDriver<int, TH,  TP,  AEA>::testCase35();
        TestDriver<int, TH,  NTP, AFA>::testCase35();
        TestDriver<int, TH,  NTP, AEA>::testCase35();
        TestDriver<int, NTH, TP,  AFA>::testCase35();
        TestDriver<int, NTH, TP,  AEA>::testCase35();
        TestDriver<int, NTH, NTP, AFA>::testCase35();
        TestDriver<int, NTH, NTP, AEA>::testCase35();

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST FUNCTIONS
        // --------------------------------------------------------------------
        if (verbose) printf("TESTING INITIALIZER LIST FUNCTIONS\n"
                            "==================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase34_outOfLine,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase34_inline,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase34_outOfLine,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING 'emplace' WITH HINT
        // --------------------------------------------------------------------
        if (verbose) printf("TESTING 'emplace' WITH HINT\n"
                            "===========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase33,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase33a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase33,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING 'emplace'
        // --------------------------------------------------------------------
        if (verbose) printf("TESTING 'emplace'\n"
                            "=================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase32a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase32,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING INSERTION WITH HINT OF MOVABLE VALUES
        // --------------------------------------------------------------------
        if (verbose) printf("TESTING INSERTION WITH HINT OF MOVABLE VALUES\n"
                            "=============================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase31,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase31,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES
        // --------------------------------------------------------------------
        if (verbose) printf("TESTING INSERTION OF MOVABLE VALUES\n"
                            "===================================\n");

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
        if (verbose) printf("TESTING TESTING MOVE ASSIGNMENT\n"
                            "===============================\n");

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
        if (verbose) printf("TESTING MOVE CONSTRUCTION\n"
                            "=========================\n");

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

        if (verbose)
             printf("TESTING 'reserve', 'rehash', AND 'max_load_factor'\n"
                    "==================================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase25,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 24: {
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
        // TESTING 'HASH' and 'EQUAL'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'HASH' and 'EQUAL'\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' AND 'empty'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'max_size' AND 'empty'\n"
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

        if (verbose) printf("TESTING RANGE 'insert'\n"
                            "======================\n");

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

        if (verbose) printf("TESTING 'insert' WITH HINT\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase16,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
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
        // TESTING 'find', 'contains', 'equal_range' AND 'count'
        // --------------------------------------------------------------------

        if (verbose)
            printf("TESTING 'find', 'contains', 'equal_range' AND 'count'\n"
                   "=====================================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING RANGE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RANGE CONSTRUCTORS\n"
                            "==========================\n");

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
