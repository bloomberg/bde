// bslstl_unorderedmultiset.t.cpp                                     -*-C++-*-

#include <bslstl_unorderedmultiset.h>

#include <bslstl_iterator.h>  // for testing only

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
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_util.h>

#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdalloctesttype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdio.h>
#include <stdlib.h>


#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND) \
 && (defined(BSLS_PLATFORM_CMP_IBM)   \
  || defined(BSLS_PLATFORM_CMP_CLANG) \
  || defined(BSLS_PLATFORM_CMP_MSVC)  \
  ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION == 0x5130) \
     )
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

enum {
#if defined(BDE_BUILD_TARGET_EXC)
    PLAT_EXC = 1
#else
    PLAT_EXC = 0
#endif
};

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
using bsls::NameOf;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
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
// [35] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.

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
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::bool_constant<EXPRESSION> NAME{}
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
    template <class OTHER_TYPE>
    struct rebind
    {
        // This nested 'struct' template, parameterized by some 'OTHER_TYPE',
        // provides a namespace for an 'other' type alias, which is an
        // allocator type following the same template as this one but that
        // allocates elements of 'OTHER_TYPE'.  Note that this allocator type
        // is convertible to and from 'other' for any 'OTHER_TYPE' including
        // 'void'.

        typedef StatefulStlAllocator<OTHER_TYPE> other;
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

    template <class OTHER_TYPE>
    StatefulStlAllocator(const StatefulStlAllocator<OTHER_TYPE>& original)
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
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj);
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

    static void testCase11();
        // Test generator functions 'g'.

    static void testCase10();
        // Reserved for BSLX.

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
    bsl::allocator<KEY>  xscratch(&scratch);

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

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.get_allocator()));
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

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX.begin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.begin()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(mX.end()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.end()));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR( X.cend()));
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

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.max_size()));
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

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
           == BSLS_CPP11_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 897
    //..
    // // bucket interface
    // size_type bucket_count() const noexcept;
    // size_type max_bucket_count() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.bucket_count()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.max_bucket_count()));
    }

    // page 897
    //..
    // // hash policy:
    // float load_factor() const noexcept;
    // float max_load_factor() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.load_factor()));
        ASSERT(BSLS_CPP11_NOEXCEPT_AVAILABLE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(X.max_load_factor()));
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

        ASSERT(BSLS_CPP11_PROVISIONALLY_FALSE
            == BSLS_CPP11_NOEXCEPT_OPERATOR(swap(mX, mY)));
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
        bsl::allocator<KEY>         xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);
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
        bsl::allocator<KEY>         xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);
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
        bsl::allocator<KEY>         xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);
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
        bsl::allocator<KEY>         xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);
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
        bsl::allocator<KEY>         xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);
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
        bsl::allocator<KEY>         xoa(&oa);
        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);
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

        bslma::TestAllocator& usedAlloc = ctor < 'e' ? da : oa;
        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(usedAlloc) {
            ++numThrows;

#if defined(BSLS_PLATFORM_CMP_GNU)
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

        ASSERTV(NameOf<KEY>(), numThrows, ctor, !oaPassed || 0 < numThrows);
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
                bsl::allocator<KEY>  xoa(&oa);
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
                bsl::allocator<KEY>  xoa(&oa);
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
                    bsl::allocator<KEY>  xscratch(&scratch);
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
            bsl::allocator<KEY>  xoa(&oa);
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
            bsl::allocator<KEY>  xoa(&oa);
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
                bsl::allocator<KEY>  xscratch(&scratch);

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
                    bsl::allocator<KEY>  xoa(&oa);
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
                    bsl::allocator<KEY>   xsa(&sa);

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

                        ASSERTV(mState, MoveState::e_UNKNOWN == mState ||
                                        MoveState::e_MOVED   == mState);

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
                    bsl::allocator<KEY>  xoa(&oa);
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
                    bsl::allocator<KEY>   xsa(&sa);

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
                        bsl::allocator<KEY>  xscratch(&scratch);

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
                bsl::allocator<KEY>  xoa(&oa);
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
                bsl::allocator<KEY>   xsa(&sa);

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

                    ASSERTV(mState, MoveState::e_UNKNOWN == mState
                                    || MoveState::e_MOVED == mState);

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
                bsl::allocator<KEY>  xoa(&oa);
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
                    bsl::allocator<KEY>  xscratch(&scratch);

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

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);

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

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bsl::allocator<KEY>  xza(&za);
                    bsl::allocator<KEY>  xoa(&oa);

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
                    bsl::allocator<KEY>   xsa(&sa);

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
            bsl::allocator<KEY>  xoa(&oa);
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bsl::allocator<KEY>  xscratch(&scratch);

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

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bsl::allocator<KEY>  xscratch(&scratch);

            Obj  mZZ(xscratch); const Obj&  ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ T_ P_(LINE1) P(ZZ) }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                    bsl::allocator<KEY>  xoa(&oa);

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
                        bsl::allocator<KEY>   xsa(&sa);

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
            bsl::allocator<KEY>  xscratch(&scratch);
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
                bsl::allocator<KEY>  xsa(&sa);
                bsl::allocator<KEY>  xza(&za);

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
                bsl::allocator<KEY>   xoa(&oa);

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
            bsl::allocator<KEY>  xscratch(&scratch);
            Obj                  mZZ(xscratch);
            const Obj&           ZZ = gg(&mZZ, SPEC);

            if (veryVerbose) {
                printf("\t\tFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",      veryVeryVeryVerbose);
            bslma::TestAllocator za("different",   veryVeryVeryVerbose);
            bsl::allocator<KEY>  xoa(&oa);
            bsl::allocator<KEY>  xza(&za);

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
#endif
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

        ASSERT(0 <= LENGTH);

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
                    0 <= da.numBlocksInUse() - TYPE_ALLOC * LENGTH);

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, -1 == verifySpec(Y, EXP));
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 == da.numBlocksInUse() - 2 * TYPE_ALLOC * LENGTH);

            ObjStlAlloc mZ; const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, -1 == verifySpec(Z, EXP));
            ASSERTV(LINE, da.numBlocksInUse(),
                    0 <= da.numBlocksInUse() - 2 * TYPE_ALLOC * LENGTH);
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
                    0 <= da.numBlocksInUse() - TYPE_ALLOC * LENGTH);
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

#if 1 // TBD non-const comparator
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
                bsl::allocator<KEY>  xoa(&oa);

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
                bsl::allocator<KEY>  xoa(&oa);

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
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        {
            const TestValues VALUES;

            Obj  mX;
            Iter it = mX.insert(VALUES[0]);

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
            bsl::allocator<KEY>  xoa(&oa);

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
                bsl::allocator<KEY>  xoa(&oa);
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
                bsl::allocator<KEY>  xoa(&oa);
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
                    bsl::allocator<KEY>  xscratch(&scratch);
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
            bsl::allocator<KEY>  xoa(&oa);
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
            bsl::allocator<KEY>  xoa(&oa);
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
                bsl::allocator<KEY>  xscratch(&scratch);
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
    // TESTING FIND, EQUAL_RANGE, COUNT
    //
    // Concern:
    //: 1 If the key being searched exists in the container, 'find' returns the
    //:   iterator referring the existing element.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator.
    //:
    //: 3 'equal_range(key)' returns all elements equivalent to 'key'.
    //:
    //: 2 'count' returns the number of elements with the same value as defined
    //:   by the comparator.
    //:
    //: 3 Both the 'const' and non-'const' versions returns the same value.
    //:
    //: 4 No memory is allocated, from either the object allocator nor from
    //:   the default allocator.
    //
    // Plan:
    //:  TDB
    //
    // Testing:
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
                bsl::allocator<KEY>  xscratch(&scratch);

                bsltf::TestValuesArray<KEY> tv(SPEC, xscratch);

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bsl::allocator<KEY>  xsa(&sa);

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
            bsl::allocator<KEY>  xscratch(&scratch);

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
                bsl::allocator<KEY>  xoa(&oa);

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
        bsl::allocator<KEY>  xoa(&oa);
        bsl::allocator<KEY>  xscratch(&scratch);

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
        bsl::allocator<KEY>  xoa(&oa);
        bsl::allocator<KEY>  xscratch(&scratch);

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
        bsl::allocator<KEY>  xoa(&oa);

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
        operatorPtr operatorNe = bsl::operator!=;

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
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
                bsl::allocator<KEY>  xscratch(&scratch);

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
                    bsl::allocator<KEY>   xxa(&xa);
                    bsl::allocator<KEY>   xya(&ya);

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
    bsl::allocator<KEY>  xoa(&oa);

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
            bsl::allocator<KEY>  xsa(&sa);

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
            bsl::allocator<KEY>  xscratch(&scratch);

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
                bsl::allocator<KEY>  xsa(&sa);
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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator testAlloc("A");
    bslma::Default::setDefaultAllocator(&testAlloc);

    switch (test) { case 0:
      case 36: {
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
      case 35: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------
        if (verbose) printf("'noexcept' SPECIFICATION\n"
                            "========================\n");

        TestDriver<int>::testCase35();

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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

        // 'propagate_on_container_move_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase29_propagate_on_container_move_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase29_propagate_on_container_move_assignment,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find', 'equal_range' AND 'count'
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'find', 'equal_range' AND 'count'\n"
                            "=========================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING RANGE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RANGE CONSTRUCTORS\n"
                            "==========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase12,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        // 'propagate_on_container_swap' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

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
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        // 'select_on_container_copy_construction' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);


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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);
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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

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
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType,
                      bsltf::MoveOnlyAllocTestType);

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
