// bslstl_unorderedmap_test.t.cpp                                     -*-C++-*-
#include <bslstl_unorderedmap_test.h>

#include <bslstl_hash.h>
#include <bslstl_iterator.h>
#include <bslstl_pair.h>
#include <bslstl_string.h>
#include <bslstl_unorderedmap.h>
#include <bslstl_vector.h>

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
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_removeconst.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_movablealloctesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_nonoptionalalloctesttype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <algorithm>
#include <functional>
#include <stdexcept>   // to confirm that the contractual exceptions are thrown
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
# include <tuple>      // for 'std::forward_as_tuple'
# include <utility>    // for 'std::piecewise_construct'
#endif

#include <stddef.h> // for 'NULL'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>  // for 'strtok'
#include <ctype.h>  // for 'toupper'

using namespace BloombergLP;
using bsls::NameOf;
using bsl::pair;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, 'bslstl_unorderedmap.t.cpp' (cases 1-11, plus the usage
// example), and 'bslstl_unorderedmap_test.cpp' (cases 12 and higher).
//
//                                  Overview
//                                  --------
// The object under test is a container whose interface and contract is
// dictated by the C++ standard.  The general concerns are compliance,
// exception safety, and proper dispatching (for member function templates such
// as insert).  This container is implemented in the form of a class template,
// and thus its proper instantiation for several types is a concern.  Regarding
// the allocator template argument, we use mostly a 'bsl::allocator' together
// with a 'bslma::TestAllocator' mechanism, but we also verify the C++
// standard.
//
// Primary Manipulators:
//: o 'insert'  (via helper function 'primaryManipulator')
//: o 'clear'
//
// Basic Accessors:
//: o 'cbegin'
//: o 'cend'
//: o 'size'
//: o 'get_allocator'
//
// There is a lot of redundancy in this test driver.  The first pass was a sort
// of ad-hoc, incomplete test, then tests were propagated by copying from
// 'bslstl_map.t.cpp' and customizing them to be applicable to
// 'bslstl_unorderedmap', the result is that quite a bit of functionality winds
// up being tested in two places.  It was not felt that the effort needed to
// remove this redundancy would be justified.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'insert' and 'clear' methods to be used by the generator function 'gg'.
// Note that some manipulators must support aliasing, and those that perform
// memory allocation must be tested for exception neutrality via the
// 'bslma_testallocator' component.  After the mandatory sequence of cases
// (1-10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is no output or BDEX streaming below 'bslstl'), we test each
// individual constructor, manipulator, and accessor in subsequent cases.
//
// Shorthand in function signatures:
//
// typedef unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC> Obj;
//
// typedef Obj::value_type Pair;    // == pair<const KEY, VALUE>
//
// ----------------------------------------------------------------------------
// [ 2] Obj();
// [ 2] Obj(const ALLOC&);
// [ 2] Obj(size_t);
// [ 2] Obj(size_t, const ALLOC&);
// [ 2] Obj(size_t, HASH);
// [ 2] Obj(size_t, HASH, const ALLOC&);
// [ 2] Obj(size_t, HASH, EQUAL);
// [ 2] Obj(size_t, HASH, EQUAL, const ALLOC&);
// [ 7] Obj(const Obj& original);
// [ 7] Obj(const Obj& original, const A& allocator);
// [12] Obj(ITER, ITER);
// [12] Obj(ITER, ITER, const ALLOC&);
// [12] Obj(ITER, ITER, size_t);
// [12] Obj(ITER, ITER, size_t, const ALLOC&);
// [12] Obj(ITER, ITER, size_t, HASH);
// [12] Obj(ITER, ITER, size_t, HASH, const ALLOC&);
// [12] Obj(ITER, ITER, size_t, HASH, EQUAL);
// [12] Obj(ITER, ITER, size_t, HASH, EQUAL, const ALLOC&);
// [27] Obj(Obj&&);
// [27] Obj(Obj&&, const ALLOC&);
// [33] void Obj(initializer_list<Pair>);
// [33] void Obj(initializer_list<Pair>, size_t);
// [33] void Obj(initializer_list<Pair>, size_t, HASH);
// [33] void Obj(initializer_list<Pair>, size_t, HASH, EQUAL);
// [33] void Obj(initializer_list<Pair>, const ALLOC&);
// [33] void Obj(initializer_list<Pair>, size_t, const ALLOC&);
// [33] void Obj(initializer_list<Pair>, size_t, HASH, const ALLOC&);
// [33] void Obj(initializer_list<Pair>, size_t, HASH, EQUAL, const ALLOC&);
// [ 2] ~Obj();
//
// modifiers:
// [ 9] Obj& operator=(const Obj&);
// [28] Obj& operator=(Obj&&);
// [33] Obj& operator=(initializer_list<Pair>);
// [ 2] pair<Iter, bool> primaryManipulator(Obj *, int);
// [ 2] void clear();
// [31] pair<iterator, bool> emplace(Args&&...);
// [32] pair<iterator, bool> emplace_hint(CIter, Args&&...);
// [18] Iter erase(CIter);
// [18] Iter erase(Iter);
// [18] size_type erase(const KEY&);
// [18] Iter erase(CIter, CIter);
// [15] pair<Iter, bool> insert(const Pair&);
// [15] Iter insert(CIter, const Pair&);
// [17] void insert(ITER, ITER);
// [29] pair<iterator, bool> insert(Pair&&);
// [29] pair<iterator, bool> insert(ALT_PAIR&&);
// [29] iterator insert(CIter, Pair&&);
// [29] iterator insert(CIter, ALT_PAIR&&);
// [33] void insert(initializer_list<Pair>);
// [ 8] void swap(Obj&);
// [43] pair<iterator, bool> try_emplace(const key&, Args&&...);
// [43] iterator try_emplace(const_iterator, const key&, Args&&...);
// [43] pair<iterator, bool> try_emplace(key&&, Args&&...);
// [43] iterator try_emplace(const_iterator, key&&, Args&&...);
// [43] pair<iterator, bool> insert_or_assign(const key&, OTHER&&);
// [43] iterator insert_or_assign(const_iterator, const key&, OTHER&&);
// [43] pair<iterator, bool> insert_or_assign(key&&, OTHER&&);
// [43] iterator insert_or_assign(const_iterator, key&&, OTHER&&);
//
// element access:
// [24] VALUE& operator[](const KEY&);
// [34] VALUE& operator[](KEY&&);
// [24] VALUE& at(const KEY&);
// [24] const VALUE& at(const KEY&) const;
//
// search:
// [13] size_type count(const KEY& key) const;
// [13] pair<iterator, iterator> equal_range(const KEY& key);
// [13] pair<const_iter, const_iter> equal_range(const KEY&) const;
// [ 4] iterator find(const KEY& key);
// [ 4] const_iterator find(const KEY& key) const;
//
// non-local iterators:
// [14] iterator begin();
// [14] iterator end();
// [14] const_iterator begin() const;
// [14] const_iterator end() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
//
// local iterators
// [14] local_iterator begin(size_t);
// [14] local_iterator end(size_t);
// [14] const_local_iterator  begin(size_t) const;
// [14] const_local_iterator cbegin(size_t) const;
// [14] const_local_iterator  end(size_t) const;
// [14] const_local_iterator cend(size_t) const;
// [14] size_t bucket(const KEY&) const;
// [14] size_t bucket_count() const;
// [14] size_t bucket_size(size_t) const;
//
// container comparisons:
// [ 6] bool operator==(const Obj&, const Obj&);
// [ 6] bool operator!=(const Obj&, const Obj&);
//
/// misc:
// [ 8] void swap(Obj&, Obj&);
// [ 4] allocator_type get_allocator() const;
//
// capacity:
// [ 4] size_type size() const;
// [20] size_type max_size() const;
// [20] bool empty() const;
//
/// bucket growth:
// [35] size_t bucket_count() const;
// [35] void max_load_factor(float);
// [35] float max_load_factor() const;
// [35] void rehash(size_type);
// [35] void reserve(size_type);
//
// functor access:
// [ 2] HASH hash_function() const;
// [ 2] EQUAL key_equal() const;
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [41] CLASS TEMPLATE DEDUCTION GUIDES
// [42] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int  ggg(Obj *, const char *, bool verbose = true);
// [ 3] Obj& gg(Obj *, const char *);
// [11] Obj  g(const char *);
// [ 3] bool verifySpec(const Obj&, const char *, bool = false);
// [37] CONCERN: Methods qualifed 'noexcept' in standard are so implemented.
//
// [22] CONCERN: 'unordered_map' is compatible with standard allocators.
// [23] CONCERN: 'unordered_map' has the necessary type traits.
// [25] CONCERN: Constructor of a template wrapper class compiles.
// [26] CONCERN: The type provides the full interface defined by the standard.
// [36] CONCERN: 'unordered_map' supports incomplete types.
// [38] CONCERN: 'erase' overload is deduced correctly.
// [39] CONCERN: Simple test case fails to compile on MSVC.
// [40] CONCERN: 'find'        properly handles transparent comparators.
// [40] CONCERN: 'count'       properly handles transparent comparators.
// [40] CONCERN: 'equal_range' properly handles transparent comparators.

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

#define EXCEPTION_TEST_BEGIN(CONTAINER)                                       \
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(                             \
          (* (bslma::TestAllocator *) (CONTAINER).get_allocator().mechanism()))

#define EXCEPTION_TEST_END  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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
//                      TEST CONFIGURATION MACRO
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#if (defined(BSLS_PLATFORM_CMP_SUN) && defined(BDE_BUILD_TARGET_OPT)) \
 || (defined(BSLS_PLATFORM_CMP_IBM) && defined(BSLS_ASSERT_SAFE_IS_ACTIVE))
    // The Sun compiler segfaults when trying to compile the usage example in
    // an optimized build.  It was initially thought specific to no-exception
    // builds as well, but it now appears to affect all optimized builds.

    // The IBM compiler produces an odd optimization that causes the usage
    // example to segfault when built in safe mode.  However, this segfault
    // appears to be triggered by an odd optimization bug, rather than caused
    // by one of the assertions added by a safe build.
# define BSLSTL_UNORDEREDMAP_DO_NOT_TEST_USAGE
#endif

#if defined(BDE_BUILD_TARGET_EXC)
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
#endif

//=============================================================================
//                              TEST SUPPORT
//-----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// Define DEFAULT DATA used in multiple test cases.

//=============================================================================
//                            GLOBAL TYPEDEFS
//-----------------------------------------------------------------------------

typedef bslma::ConstructionUtil     ConstrUtil;
typedef bsltf::TemplateTestFacility TTF;
typedef bsls::Types::IntPtr         IntPtr;
typedef bsls::Types::Int64          Int64;

typedef bsltf::NonDefaultConstructibleTestType TestKeyType;
typedef bsltf::NonTypicalOverloadsTestType     TestValueType;

//=============================================================================
//                             GLOBAL DEFAULT DATA
//-----------------------------------------------------------------------------

static struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec_p;     // specification string, for input to 'gg' func
    const char *d_results_p;  // expected element values
} DEFAULT_DATA[] = {
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
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };
static const size_t DEFAULT_MAX_LENGTH = 17;        // length of longest spec
static const size_t DEFAULT_NUM_MAX_LENGTH = 2;     // # of specs that length

// Define values used to initialize positional arguments for
// 'bsltf::EmplacableTestType' and 'bsltf::AllocEmplacableTestType'
// constructors.  Note, that you cannot change those values as they are used by
// 'TemplateTestFacility::getIdentifier' to map the constructed emplacable
// objects to their integer identifiers.
static const int K01 = 1;
static const int K02 = 20;
static const int K03 = 23;
static const int V01 = 44;
static const int V02 = 68;
static const int V03 = 912;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl {

template <class FIRST, class SECOND>
inline
void debugprint(const bsl::pair<FIRST, SECOND>& p)
{
    printf("(");
    bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                                TTF::getIdentifier(p.first)));
    printf(",");
    bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                                TTF::getIdentifier(p.second)));
    printf(")");
}

// unordered_map-specific print function.

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void debugprint(const bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef typename bsl::unordered_map<KEY,
                                            VALUE,
                                            HASH,
                                            EQUAL,
                                            ALLOC>::const_iterator CIter;
        for (CIter it = s.begin(); it != s.end(); ++it) {
            bsls::BslTestUtil::callDebugprint(static_cast<char>(
                                               TTF::getIdentifier(it->first)));
        }
    }
    fflush(stdout);
}

}  // close namespace bsl

namespace {
namespace u {

enum { k_CHAR_SHIFT = '0' - 'A' };

class TestAllocatorUtil {
    // Namespace for the 'test' function.

  public:
    template <class TYPE>
    static void test(const TYPE&, const bslma::Allocator&)
        // Do nothing.
    {
    }

    static void test(const bsltf::AllocEmplacableTestType& value,
                     const bslma::Allocator&               allocator)
        // If the specified 'value' is an 'AllocEmplacableTestType', check that
        // all of the fields of 'value' were created with the specified
        // 'allocator'.
    {
        ASSERTV(&allocator == value.arg01().allocator());
        ASSERTV(&allocator == value.arg02().allocator());
        ASSERTV(&allocator == value.arg03().allocator());
        ASSERTV(&allocator == value.arg04().allocator());
        ASSERTV(&allocator == value.arg05().allocator());
        ASSERTV(&allocator == value.arg06().allocator());
        ASSERTV(&allocator == value.arg07().allocator());
        ASSERTV(&allocator == value.arg08().allocator());
        ASSERTV(&allocator == value.arg09().allocator());
        ASSERTV(&allocator == value.arg10().allocator());
    }
};

template <class TYPE, int N>
int arrayLength(const TYPE (&)[N])
{
    return N;
}

template <class TYPE>
TYPE& copyAssignTo(TYPE *dst, const TYPE& src)
    // Set the value of the specified '*dst' to that of the specified 'src' and
    // return a reference for modifiable access to '*dst'.
{
    BSLMF_ASSERT((!bsl::is_same<TYPE, bsltf::MoveOnlyAllocTestType>::value));
    BSLMF_ASSERT((!bsl::is_same<TYPE,
                             bsltf::WellBehavedMoveOnlyAllocTestType>::value));

    return *dst = src;
}

template <>
bsltf::MoveOnlyAllocTestType&
copyAssignTo(bsltf::MoveOnlyAllocTestType        *dst,
             const bsltf::MoveOnlyAllocTestType&  src)
{
    dst->setData(src.data());

    return *dst;
}

template <>
bsltf::WellBehavedMoveOnlyAllocTestType&
copyAssignTo(bsltf::WellBehavedMoveOnlyAllocTestType        *dst,
             const bsltf::WellBehavedMoveOnlyAllocTestType&  src)
{
    dst->setData(src.data());

    return *dst;
}

void deleteFromSpec(char *spec, char toRemove)
    // Remove all instances of the specified character 'toRemove' from the
    // specified 'SPEC'.
{
    char *el;
    while ((el = strchr(spec, toRemove))) {
        char *end = spec + strlen(spec);
        memmove(el, el + 1, end + 1 - (el + 1));
    }
}

template <class KEY, class VALUE>
inline
bool eq(const bsl::pair<KEY, VALUE>& a,
        const bsl::pair<KEY, VALUE>& b)
    // Compare the specified 'a' to the specified 'b', regardless of the
    // 'const'-status of the 'KEY' fields of the pairs.
{
    return a.first == b.first && a.second == b.second;
}

template <class KEY, class VALUE>
inline
bool eq(const bsl::pair<const KEY, VALUE>& a,
        const bsl::pair<KEY,       VALUE>& b)
{
    return a.first == b.first && a.second == b.second;
}

template <class KEY, class VALUE>
inline
bool eq(const bsl::pair<KEY,       VALUE>& a,
        const bsl::pair<const KEY, VALUE>& b)
{
    return a.first == b.first && a.second == b.second;
}

template <class U, class V>
inline
int idOf(const pair<U, V>& value)
    // Return the 'id' with which the 'first' field of the specified 'value'
    // was created.  Note that the 'second' field is ignored.
{
    return TTF::getIdentifier(value.first);
}

template <class TYPE>
TYPE myAbs(const TYPE& x)
{
    return x < 0 ? -x : x;
}

template <class TYPE>
bool nearlyEqual(const TYPE& x, const TYPE& y)
{
    TYPE tolerance = std::max(u::myAbs(x), u::myAbs(y)) * 0.0001;
    return u::myAbs(x - y) <= tolerance;
}

template <class TYPE>
inline
int valueOf(const TYPE& value)
    // Return the 'id' with which the specified 'value' was created.
{
    return TTF::getIdentifier(value);
}
}  // close namespace u

void testCase39(const bsl::unordered_map<int, int>& m)
    // Iterate through the specified map 'm'.  Note that the real test is
    // seeing whether this function compiles, not what it does.
{
    for (bsl::unordered_map<int, int>::const_iterator
         it = m.begin(); it != m.end(); ++it) {
    }
}

                       // =============================
                       // struct EraseAmbiguityTestType
                       // =============================

struct EraseAmbiguityTestType
    // This test type has a template constructor that can accept iterator.
{
    // CREATORS
    template <class T>
    EraseAmbiguityTestType(T&)
        // Construct an object.
    {}
};

bool operator==(const EraseAmbiguityTestType&,
                const EraseAmbiguityTestType&)
    // This operator is no-op and written only to satisfy requirements for
    // 'key_type' class.
{
    return true;
}

template <class HASHALG>
inline
void hashAppend(HASHALG& hashAlg, const EraseAmbiguityTestType& object)
    // This function is no-op and written only to satisfy requirements for
    // 'key_type' class.
{
    using ::BloombergLP::bslh::hashAppend;
    (void) object;  // suppress 'unused variable' warning
    hashAppend(hashAlg, 0);
}

void runErasure(bsl::unordered_map<EraseAmbiguityTestType, int>& container,
                EraseAmbiguityTestType                           element)
    // Look for the specified 'element' in the specified 'container' and delete
    // it if found.  Code is written in such a way as to reveal the ambiguity
    // of the 'erase' method call.
{
    bsl::unordered_map<EraseAmbiguityTestType, int>::iterator it =
                                                       container.find(element);
    if (it != container.end()) {
        container.erase(it);
    }
}

}  // close unnamed namespace

                       // ================
                       // struct EqKeyPred
                       // ================

template <class KEY, class VALUE>
struct EqKeyPred {
    // A predicate for testing 'erase_if'; it takes a value at construction
    // and uses it for comparisons later.

    KEY d_key;
    EqKeyPred(KEY val) : d_key(val) {}

    bool operator() (const bsl::pair<const KEY, VALUE> &p) const
        // return 'true' if the second member of the specified pair 'p' is
        // equal to the stored value, and 'false' otherwise.
    {
        return d_key == p.first;
    }
};

//=============================================================================
//                      GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

#if 0
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

}  // close unnamed namespace

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
namespace u {

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
    , d_id(0)
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

template <class VALUE>
inline
bool operator==(const StatefulStlAllocator<VALUE>& lhs,
                const StatefulStlAllocator<VALUE>& rhs)
{
    return lhs.id() == rhs.id();
}

template <class VALUE>
inline
bool operator!=(const StatefulStlAllocator<VALUE>& lhs,
                const StatefulStlAllocator<VALUE>& rhs)
{
    return lhs.id() != rhs.id();
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

                       // ====================
                       // class TestComparator
                       // ====================

bool g_enableAllFunctorsFlag = true;

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
        g_enableAllFunctorsFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableAllFunctorsFlag = true;
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
        if (!g_enableAllFunctorsFlag) {
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
        g_enableAllFunctorsFlag = false;
    }

    static void enableFunctor()
        // Enable all objects of 'TestComparator' such that 'operator()' may
        // be invoked
    {
        g_enableAllFunctorsFlag = true;
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
        if (!g_enableAllFunctorsFlag) {
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

                            // =======================
                            // struct ThrowingMoveHash
                            // =======================

template <class TYPE>
struct ThrowingMoveHash : public bsl::hash<TYPE> {
    // Hash functor with throwing move operations.

    // CREATORS
    ThrowingMoveHash()
        // Create a 'ThrowingMoveHash' object.
    {
    }

    ThrowingMoveHash(const ThrowingMoveHash &other)
        // Create a 'ThrowingMoveHash' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    ThrowingMoveHash(bslmf::MovableRef<ThrowingMoveHash> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingMoveHash' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    // MANIPULATORS
    ThrowingMoveHash &operator=(const ThrowingMoveHash &other)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }

    ThrowingMoveHash &operator=(bslmf::MovableRef<ThrowingMoveHash> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }
};
                            // ========================
                            // struct ThrowingMoveEqual
                            // ========================

template <class TYPE>
struct ThrowingMoveEqual : public bsl::equal_to<TYPE> {
    // Equal functor with throwing move operations.

    // CREATORS
    ThrowingMoveEqual()
        // Create a 'ThrowingMoveEqual' object.
    {
    }

    ThrowingMoveEqual(const ThrowingMoveEqual &other)
        // Create a 'ThrowingMoveEqual' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    ThrowingMoveEqual(bslmf::MovableRef<ThrowingMoveEqual> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingMoveEqual' object having the same value as that of
        // the specified 'other'.
    {
        (void)other;
    }

    // MANIPULATORS
    ThrowingMoveEqual &operator=(const ThrowingMoveEqual &other)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }

    ThrowingMoveEqual &operator=(bslmf::MovableRef<ThrowingMoveEqual> other)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Assign to this object the value of the specified 'other'.
    {
        (void)other;
        return *this;
    }
};

                       // =====================
                       // class TemplateWrapper
                       // =====================

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
class TemplateWrapper {
    // This class contains a container and does nothing else, checking for an
    // Aix compiler bug.

    // DATA
    bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC> d_member;

  public:
    // CREATORS
    TemplateWrapper()
    : d_member()
    {
    }

    //! TemplateWrapper(const TemplateWrapper&) = default;

    template <class INPUT_ITERATOR>
    TemplateWrapper(INPUT_ITERATOR begin, INPUT_ITERATOR end)
    : d_member(begin, end)
    {
    }
};

                       // ===============
                       // class DummyHash
                       // ===============

class DummyHash {
    // A dummy hasher class.  Must be defined after 'TemplateWrapper' to
    // reproduce the AIX bug.

  public:
    size_t operator() (int)
    {
        return true;
    }
};

                       // ================
                       // class DummyEqual
                       // ================

class DummyEqual {
    // A dummy hasher class.  Must be defined after 'TemplateWrapper' to
    // reproduce the AIX bug.

  public:
    bool operator() (int, int)
    {
        return true;
    }
};

                       // ====================
                       // class DummyAllocator
                       // ====================

template <class TYPE>
class DummyAllocator {
    // A dummy allocator class.  Must be defined after 'TemplateWrapper' to
    // reproduce an AIX bug.  Every method is a no-op.

  public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
    typedef TYPE&           reference;
    typedef const TYPE&     const_reference;
    typedef TYPE            value_type;

    template <class BDE_OTHER_TYPE>
    struct rebind
    {
        typedef DummyAllocator<BDE_OTHER_TYPE> other;
    };

    // CREATORS
    DummyAllocator()
        // Default construct.
    {
    }

    // DummyAllocator(const DummyAllocator& original) = default;

    template <class BDE_OTHER_TYPE>
    DummyAllocator(const DummyAllocator<BDE_OTHER_TYPE>& original)
        // Copy the specified 'original'.
    {
        (void)original;
    }

    //! ~DummyAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! DummyAllocator& operator=(const DummyAllocator& rhs) = default;

    pointer allocate(size_type,           // numElements
                     const void * = 0)    // hint
        // Allocate.
    {
        return 0;
    }

    void deallocate(pointer,          // address
                    size_type = 1)    // numElements
        // Deallocate.
    {
    }

    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *) {}
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *,
                   const ELEMENT_TYPE) { }

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *) {}

    // ACCESSORS
    pointer address(reference) const { return 0; }    // Take an address.

    const_pointer address(const_reference) const
        // Take an address.
    {
        return 0;
    }

    size_type max_size() const { return 0; }    // Return maximum size.
};

                       // =========================
                       // class CharToPairConverter
                       // =========================

template <class PAIR, class ALLOC>
struct CharToPairConverter {
    // Convert a 'char' value to a 'bsl::pair' of the parameterized 'KEY' and
    // 'VALUE' type.

    // CLASS METHODS
    static void createInplace(PAIR *address, char value, ALLOC allocator)
        // Construct a pair of type 'PAIR' at the specified 'address',
        // translating from the specified 'value', using the specified
        // 'allocator', without requiring either of the members of 'PAIR' to
        // have copy constructors.
    {
        BSLS_ASSERT(address);
        BSLS_ASSERT(0 < value);

        typedef typename bsl::remove_const<typename PAIR::first_type>::type
                                                                           Key;
        bsls::ObjectBuffer<Key> tmpKey;
        TTF::emplace(tmpKey.address(), value, allocator);
        bslma::DestructorGuard<Key> keyGuard(tmpKey.address());

        typedef typename PAIR::second_type Mapped;
        bsls::ObjectBuffer<Mapped> tmpMapped;
        TTF::emplace(tmpMapped.address(), value + u::k_CHAR_SHIFT, allocator);
        bslma::DestructorGuard<Mapped> mappedGuard(tmpMapped.address());

        bsl::allocator_traits<ALLOC>::construct(
                              allocator,
                              address,
                              bslmf::MovableRefUtil::move(tmpKey.object()),
                              bslmf::MovableRefUtil::move(tmpMapped.object()));

        Key *keyPtr = const_cast<Key *>(bsls::Util::addressOf(address->first));
        bsltf::setMovedInto(keyPtr,    bsltf::MoveState::e_NOT_MOVED);
        Mapped *mappedPtr =             bsls::Util::addressOf(address->second);
        bsltf::setMovedInto(mappedPtr, bsltf::MoveState::e_NOT_MOVED);
    }
};

template <class OBJECT>
bool verifySpec(const OBJECT&     object,
                const char       *spec,
                bool              keysOnly  = false)
    // Return 'true' if the specified 'object' exactly matches the specified
    // 'spec'.  If the spec is invalid (contains redundant chars or chars
    // outside the range "[ 'A' - 'Z' ]") 'false' will be returned.  If the
    // optionally specified 'keysOnly' is 'true', only key values in the map
    // are verified, otherwise both key values and mapped values are verified.
{
    typedef typename OBJECT::const_iterator CIter;

    // Create, in 'buf', a copy of 'spec' with redundant elements removed.

    char deducedSpec[1000], *pds = deducedSpec;
    const CIter end = object.end();
    for  (CIter it  = object.begin(); end != it; ++it) {
        const char keyVal = static_cast<char>(TTF::getIdentifier(it->first));
        if (!keysOnly) {
            const int mappedVal = TTF::getIdentifier(it->second);
            if (mappedVal != keyVal + u::k_CHAR_SHIFT) {
                return false;                                         // RETURN
            }
        }
        *pds++ = keyVal;
    }
    *pds = 0;
    const IntPtr deducedSize = pds - deducedSpec;

    char buf[1000], *bufEnd = buf;
    while ((*bufEnd = *spec++)) {
        ++bufEnd;
    }
    ASSERT(!*bufEnd);
    std::sort(           buf + 0, bufEnd);
    bufEnd = std::unique(buf + 0, bufEnd);
    *bufEnd = 0;

    if (bufEnd - buf != deducedSize) {
        return false;                                                 // RETURN
    }
    ASSERT(object.size() == static_cast<size_t>(deducedSize));

    std::sort(deducedSpec + 0, pds);

    return !strcmp(buf, deducedSpec);
}

}  // close namespace u

}  // close unnamed namespace

namespace {

                       // =========================
                       // struct TestIncompleteType
                       // =========================

struct IncompleteType;
struct TestIncompleteType {
    // This 'struct' provides a simple compile-time test to verify that
    // incomplete types can be used in container definitions.  Currently,
    // definitions of 'bsl::unordered_map' can contain incomplete types on all
    // supported platforms.
    //
    // See 'TestIncompleteType' in bslstl_map.t.cpp for the rationale behind
    // this test type.

    // PUBLIC TYPES
    typedef bsl::unordered_map<int, IncompleteType>::iterator            Iter1;
    typedef bsl::unordered_map<IncompleteType, int>::iterator            Iter2;
    typedef bsl::unordered_map<IncompleteType, IncompleteType>::iterator Iter3;

    // PUBLIC DATA
    bsl::unordered_map<int, IncompleteType>            d_data1;
    bsl::unordered_map<IncompleteType, int>            d_data2;
    bsl::unordered_map<IncompleteType, IncompleteType> d_data3;

#if defined(BDE_BUILD_TARGET_SAFE_2)
    ~TestIncompleteType();
        // If building in 'SAFE_2' mode, the invariants-check in the destructor
        // for 'bslstl::HashTable' requires the key type to be complete, and
        // hashable.  Deferring the definition of this class's destructor until
        // after the incomplete key type has been fully defined, with hashing
        // support provided, resolves these issues.
#endif
};

struct IncompleteType {
    int d_data;
};

#if defined(BDE_BUILD_TARGET_SAFE_2)
template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM &hashAlg, const IncompleteType &object)
    // Apply the specified 'hashAlg' to the specified 'object'
{
    using bslh::hashAppend;
    hashAppend(hashAlg, object.d_data);
}

inline
TestIncompleteType::~TestIncompleteType()
{
}
#endif

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
    ASSERT(existingKey.value()           == EXISTING_F->first);
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
        ASSERT(existingKey.value() == it->first);
    }

    const bsl::pair<Iterator, Iterator> NON_EXISTING_ER =
                                         container.equal_range(nonExistingKey);
    ASSERT(NON_EXISTING_ER.first   == NON_EXISTING_ER.second);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
}

                         // ================
                         // class IntValue
                         // ================

struct IntValue {
    // A struct that holds an integer value, but has multiple constructors
    // that add the different arguments.

    IntValue ()                    : d_value(0) {}
        // Construct an IntValue, setting d_value to zero.

    IntValue (int a)               : d_value(a) {}
        // Construct an IntValue from the specified 'a'

    IntValue (int a, int b)        : d_value(a + b) {}
        // Construct an IntValue from the specified 'a' and 'b'.

    IntValue (int a, int b, int c) : d_value(a + b + c) {}
        // Construct an IntValue from the specified 'a', 'b', and 'c'.

    int d_value;
};
                         // ================
                         // class MoveHolder
                         // ================

template <class TYPE>
struct MoveHolder {
    // A simple class that holds a value, but records if the value has ever
    // been moved from.

    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
        // This typedef is a convenient alias for the utility associated with
        // movable references.

    MoveHolder()
    : d_value()
    , d_moved(false)
        // Default initialize the held value.
    {}

    explicit MoveHolder(const TYPE &value)
    : d_value(value)
    , d_moved(false)
        // Construct from the specified 'value'
    {}

    MoveHolder(const MoveHolder& rhs)
    : d_value(rhs.d_value)
    , d_moved(false)
        // Copy-construct from the specified 'rhs'
    {}

    MoveHolder(BloombergLP::bslmf::MovableRef<MoveHolder> rhs)
    : d_value(MoveUtil::move(MoveUtil::access(rhs).d_value))
    , d_moved(false)
        // Move-construct from the specified 'rhs'. Set the 'rhs' moved-from
        // flag to 'true'.
    {
        MoveHolder& lvalue = rhs;
        lvalue.d_moved = true;
    }

    MoveHolder& operator=(const MoveHolder& rhs)
        // Copy-assign from the specified 'rhs'.
    {
        d_value = rhs.d_value;
        d_moved = rhs.d_moved;
        return *this;
    }

    MoveHolder& operator=(BloombergLP::bslmf::MovableRef<MoveHolder> rhs)
        // Move-assign from the specified 'rhs'. Set the 'rhs' moved-from flag
        // to 'true'.
    {
        MoveHolder& lvalue = rhs;

        d_value = MoveUtil::move(MoveUtil::access(rhs).d_value);
        d_moved = lvalue.d_moved;
        lvalue.d_moved = true;
        return *this;
    }

    bool hasBeenMoved() const
        // Return 'true' if this object has been moved from, and 'false'
        // otherwise.
    {
        return d_moved;
    }

    TYPE d_value;
    bool d_moved;
};


template <class TYPE>
struct MoveHolderHash {
    // Provide a hash operation for all MoveHolder objects.  Return the
    // 'bsl::hash' of the underlying object held in 'value'.

    size_t operator() (const MoveHolder<TYPE>& value) const
        // return the hash of the underlying object held in the specified
        // 'value'.
    {
        return bsl::hash<TYPE>()(value.d_value);
    }
};

template <class TYPE>
bool operator==(const MoveHolder<TYPE>& lhs, const MoveHolder<TYPE>& rhs)
    // Return 'true' if both the held value and the moved-from flag of the
    // specified 'lhs' are equal to the corresponding fields in the specified
    // 'rhs', and 'false' otherwise.
{
    return lhs.hasBeenMoved() == rhs.hasBeenMoved()
                                                 && lhs.d_value == rhs.d_value;
}

template <class TYPE>
bool operator==(const MoveHolder<TYPE>& lhs, const TYPE& rhs)
    // Return 'true' if the held value in the specified 'lhs' is equal to the
    // specified 'rhs', and 'false' otherwise.  If 'lhs' has been moved from,
    // then return 'false'.
{
    return lhs.hasBeenMoved() ? false : lhs.d_value == rhs;
}

template <class TYPE>
bool operator==(const TYPE& lhs, const MoveHolder<TYPE>& rhs)
    // Return 'true' if the held value in the specified 'rhs' is equal to the
    // specified 'lhs', and 'false' otherwise.  If 'rhs' has been moved from,
    // then return 'false'.
{
    return rhs.hasBeenMoved() ? false : lhs == rhs.d_value;
}

}  // close unnamed namespace

//=============================================================================
//                              TestDriver
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class KEY,
          class VALUE = KEY,
          class HASH  = u::TestHashFunctor<KEY>,
          class EQUAL = u::TestEqualityComparator<KEY>,
          class ALLOC = bsl::allocator<pair<const KEY, VALUE> > >
class TestDriver {
    // This templatized struct provide a namespace for testing the
    // 'unordered_map' container.  The parameterized 'KEY', 'VALUE', 'HASH',
    // 'EQUAL' and 'ALLOC' specifies the key type, the mapped type, the hash
    // functor, the equality comparator type and allocator type respectively.
    // Each "testCase*" method test a specific aspect of
    // 'unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC>'.  Every test cases
    // should be invoked with various parameterized type to fully test the
    // container.

  private:
    // TYPES
    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC>  Obj;
        // Type under testing.

    typedef typename Obj::key_type                Key;
    typedef typename Obj::iterator                Iter;
    typedef typename Obj::const_iterator          CIter;
    typedef typename Obj::local_iterator          LIter;
    typedef typename Obj::const_local_iterator    CLIter;
    typedef typename Obj::size_type               SizeType;
    typedef typename Obj::value_type              Pair;

    // Shorthands
    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::MoveState                      MoveState;

    typedef typename bsl::remove_const<KEY>::type NoConstKey;
    typedef pair<NoConstKey, VALUE>               TValueType;

    typedef bsltf::TestValuesArray<
                        TValueType,
                        ALLOC,
                        u::CharToPairConverter<TValueType, ALLOC> > TestValues;

    typedef bsltf::TestValuesArray<
                       Pair,
                       ALLOC,
                       u::CharToPairConverter<Pair, ALLOC> >       CTestValues;

    BSLMF_ASSERT((!bslmf::IsSame<Iter,  CIter>::value));
    BSLMF_ASSERT((!bslmf::IsSame<LIter, CLIter>::value));
    BSLMF_ASSERT((!bslmf::IsSame<Pair,  TValueType>::value));

    enum { k_TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                          bslma::UsesBslmaAllocator<VALUE>::value,

           k_IS_KEY_WELL_BEHAVED =
                   bsl::is_same<KEY,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,

           k_IS_KEY_MOVE_AWARE =
                   bsl::is_same<KEY, bsltf::MovableTestType>::value ||
                   bsl::is_same<KEY, bsltf::MovableAllocTestType>::value ||
                   bsl::is_same<KEY, bsltf::MoveOnlyAllocTestType>::value ||
                   k_IS_KEY_WELL_BEHAVED,

           k_IS_VALUE_WELL_BEHAVED =
                   bsl::is_same<VALUE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,
           k_IS_VALUE_MOVE_AWARE =
                   bsl::is_same<VALUE, bsltf::MovableTestType>::value ||
                   bsl::is_same<VALUE, bsltf::MovableAllocTestType>::value ||
                   bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value ||
                   k_IS_VALUE_WELL_BEHAVED,

#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLS_PLATFORM_OS_WINDOWS)
           // Aix has a compiler bug where method pointers do not default
           // construct to 0.  Windows has the same problem.

           k_IS_VALUE_DEFAULT_CONSTRUCTIBLE =
                !bsl::is_same<VALUE,
                              bsltf::TemplateTestFacility::MethodPtr>::value };
#else
           k_IS_VALUE_DEFAULT_CONSTRUCTIBLE = true };
#endif

  public:
    typedef bsltf::StdTestAllocator<KEY> StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'unordered_map<KEY, VALUE, EQUAL, ALLOC>'
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
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static void matchFirstValues(const int         LINE,
                                 const Obj&        object,
                                 const TestValues& values,
                                 const size_t      count,
                                 bool              keysOnly = false);
        // Match that exactly the first specified 'count' values of the
        // specified array 'values' match all of the objects in the specified
        // 'object'.

    static pair<Iter, bool> primaryManipulator(Obj              *container,
                                               int               identifier)
        // Insert into the specified 'container' the value object indicated by
        // the specified 'identifier', ensuring that the overload of the
        // primary manipulator taking a modifiable rvalue is invoked (rather
        // than the one taking an lvalue).  Return the result of invoking the
        // primary manipulator.
    {
        bsls::ObjectBuffer<TValueType> buffer;
        u::CharToPairConverter<TValueType, ALLOC>::createInplace(
                                       buffer.address(),
                                       static_cast<char>(identifier),
                                       container->get_allocator().mechanism());
        bslma::DestructorGuard<TValueType> guard(buffer.address());

        return container->insert(MoveUtil::move(buffer.object()));
    }

    template <class T>
    static bslmf::MovableRef<T> testArg(T *t, bsl::true_type)
        // If the second arg is of type 'true_type', return the specified '*t',
        // moved.
    {
        return MoveUtil::move(*t);
    }

    template <class T>
    static const T&             testArg(T *t, bsl::false_type)
        // If the second arg is of type 'false_type', return a const ref to the
        // specified '*t'.
    {
        return *t;
    }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    template <int NUM_KEY_ARGS,
              int NK1,
              int NK2,
              int NK3,
              int NUM_VALUE_ARGS,
              int NV1,
              int NV2,
              int NV3>
    static Iter testCase32a_RunTest(Obj *target, CIter hint, bool inserted);
        // Call 'emplace_hint' on the specified 'target' container and verify
        // that a value was newly inserted if and only if the specified
        // 'inserted' flag is 'true'.  Forward (template parameters)
        // 'NUM_KEY_ARGS' and 'NUM_VALUE_ARGS' arguments to the 'emplace_hint'
        // method and ensure 1) that values are properly passed to the
        // piecewise constructor of 'value_type', 2) that the allocator is
        // correctly configured for each argument in the newly inserted element
        // in 'target', and 3) that the arguments are forwarded using copy or
        // move semantics based on integer template parameters '[NK1 .. NK3]'
        // and '[NV1 .. NV3]'.

    template <int NUM_KEY_ARGS,
              int NK1,
              int NK2,
              int NK3,
              int NUM_VALUE_ARGS,
              int NV1,
              int NV2,
              int NV3>
    static void testCase31a_RunTest(Obj *target, bool inserted);
        // Call 'emplace' on the specified 'target' container and verify that a
        // value was newly inserted if and only if the specified 'inserted'
        // flag is 'true'.  Forward (template parameters) 'NUM_KEY_ARGS' and
        // 'NUM_VALUE_ARGS' arguments to the 'emplace' method and ensure 1)
        // that values are properly passed to the piecewise constructor of
        // 'value_type', 2) that the allocator is correctly configured for each
        // argument in the newly inserted element in 'target', and 3) that the
        // arguments are forwarded using copy or move semantics based on
        // integer template parameters '[NK1 .. NK3]' and '[NV1 .. NV3]'.
#endif

  public:
    // TEST CASES
    static void testCase43();
        // Test 'try_emplace' and 'insert_or_assign'

    static void testCase42();
        // Test free function 'bsl::erase_if'.

    static void testCase38();
        // Test absence of 'erase' method ambiguity.

    static void testCase37();
        // Test 'noexcept' specifications

    static void testCase35();
        // Bucket Growth

    static void testCase34();
        // Test element access with movable key.  'VALUE' must be default
        // constructible.

    static void testCase33_inline();
    static void testCase33_outOfLine();
        // Test initializer lists.

    static void testCase32a();
        // Test forwarding of arguments in 'emplace_hint' method.

    static void testCase31a();
        // Test forwarding of arguments in 'emplace' method.

    static void testCase31();
        // Test 'emplace' method.

    static void testCase29();
        // Test insert on movable value.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase28_propagate_on_container_move_assignment_dispatch();
    static void testCase28_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase28_noexcept();
        // Test move assignment operator noexcept.

    static void testCase28();
        // Test move-assignment operator.

    static void testCase27();
        // Test move construction.

    static void testCase26();
        // Test standard interface coverage.

    static void testCase25();
        // Test constructors of a template wrapper class.

    static void testCase24();
        // Test element access via non-move 'operator[]' and 'at'.  'VALUE'
        // must be default constructible.

    static void testCase23();
        // Type Traits and Typedefs

    static void testCase22();
        // Object with STL allocator.

    static void testCase20();
        // Test 'max_size' and 'empty'.

    static void testCase18();
        // Test 'erase'.

    static void testCase17();
        // Test range 'insert'.

//  static void testCase16();
//      // Testing 'insert' with hint -- already tested in TC 15.

    static void testCase15();
        // Test insert, with & without hint, with and without move, with and
        // without matching allocators.  '..._copy' requires 'KEY' and 'VALUE'
        // to have copy c'tors.

    static void testCase14();
        // Test iterators.

    static void testCase13();
        // Test 'find', 'equal_range'

    static void testCase12();
        // Range C'tor
};

template <class KEY>
class StdAllocTestDriver :
    public TestDriver<KEY,
                      KEY,
                      u::TestHashFunctor<KEY>,
                      u::TestEqualityComparator<KEY>,
                      bsltf::StdTestAllocator<pair<const KEY, KEY> > > {
};

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
int TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::ggg(Obj        *object,
                                                     const char *spec,
                                                     int         verbose)
{
    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            pair<typename Obj::iterator, bool> pr =
                                           primaryManipulator(object, spec[i]);

            if (pr.second) {
                typename Obj::iterator it = pr.first;

                typedef typename bsl::remove_const<KEY>::type Key;

                Key *keyPtr =
                           const_cast<Key *>(bsls::Util::addressOf(it->first));
                bsltf::setMovedInto(keyPtr,    MoveState::e_NOT_MOVED);
                VALUE *mappedPtr =           bsls::Util::addressOf(it->second);
                bsltf::setMovedInto(mappedPtr, MoveState::e_NOT_MOVED);
            }
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC>&
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::gg(Obj        *object,
                                               const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
bsl::unordered_map<KEY, VALUE, HASH, EQUAL, ALLOC>
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::matchFirstValues(
                                                    const int         LINE,
                                                    const Obj&        object,
                                                    const TestValues& values,
                                                    const size_t      count,
                                                    bool              keysOnly)
{
    if (!count) {
        return;                                                       // RETURN
    }

    ASSERTV(LINE, object.size(), count, object.size() == count);

    bool *foundValues = new bool[count];
    memset(foundValues, 0, count);

    for (size_t ii = 0; ii < count; ++ii) {
        const TValueType& v = values[ii];
        CIter it = object.find(v.first);
        bool found;
        ASSERTV(v.first, (found = (object.end() != it)));
        ASSERTV(v.first, it->second, v.second,
                                 !found || keysOnly || it->second == v.second);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase43()
{
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;

    static const struct {
        int         d_line;      // source line number
        const char *d_key_p;     // list of keys string
        const char *d_value_p;   // list of values string
        const char *d_results_p; // expected element values
    } DATA[] = {
        //line  key            value          results
        //----  ---            -----          -------

        { L_,   "A",           "0",           "Y"           },
        { L_,   "AAA",         "012",         "YNN"         },
        { L_,   "ABCDEFGH",    "01234567",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "0123456789A", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "0123456789",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    typedef bsl::unordered_map<MoveHolder<KEY>, VALUE, MoveHolderHash<KEY> >
                                                              MH_unordered_map;

    // Sanity checks
    for (size_t i = 0; i < NUM_DATA; ++i) {
        ASSERT(strlen(DATA[i].d_key_p) == strlen(DATA[i].d_value_p));
        ASSERT(strlen(DATA[i].d_key_p) == strlen(DATA[i].d_results_p));
    }

    // try_emplace (const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename Obj::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.try_emplace(keys[j], values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, KEY(keys[j]) == res.first->first);
            if (res.second) {
                ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // try_emplace (KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename MH_unordered_map::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_unordered_map mX;
        size_t           map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));
            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.try_emplace(MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            ASSERTV(LINE, j, KEY(keys[j]) == res.first->first);
            if (res.second) {
                ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // try_emplace (hint, const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            typename Obj::iterator it =
                                  mX.try_emplace(mX.end(), keys[j], values[j]);
            ASSERTV(LINE, j, KEY(keys[j]) == it->first);
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, VALUE(values[j]) == it->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // try_emplace (hint, KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_unordered_map mX;
        size_t           map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));
            ASSERTV(LINE, j, map_size == mX.size());
            typename MH_unordered_map::iterator it =
                        mX.try_emplace(mX.end(), MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, KEY(keys[j]) == it->first);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, VALUE(values[j]) == it->second);
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename Obj::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.insert_or_assign(keys[j], values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, KEY(keys[j])     == res.first->first);
            ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
            if (res.second) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        typedef bsl::pair<typename MH_unordered_map::iterator, bool> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_unordered_map mX;
        size_t           map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));
            ASSERTV(LINE, j, map_size == mX.size());
            PAIR res = mX.insert_or_assign(MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, ('Y' == results[j]) == res.second);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            ASSERTV(LINE, j, KEY(keys[j])     == res.first->first);
            ASSERTV(LINE, j, VALUE(values[j]) == res.first->second);
            if (res.second) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (hint, const KEY &)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        Obj    mX;
        size_t map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            ASSERTV(LINE, j, map_size == mX.size());
            typename Obj::iterator it =
                            mX.insert_or_assign(mX.end(), keys[j], values[j]);
            ASSERTV(LINE, j, KEY(keys[j])     == it->first);
            ASSERTV(LINE, j, VALUE(values[j]) == it->second);
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }

    // insert_or_assign (hint, KEY&&)
    for (size_t i = 0; i < NUM_DATA; ++i)
    {
        int             LINE = DATA[i].d_line;
        const char     *keys = DATA[i].d_key_p;
        const size_t    keysLen = strlen(keys);
        const char     *values = DATA[i].d_value_p;
        const char     *results = DATA[i].d_results_p;

        MH_unordered_map mX;
        size_t           map_size = 0;

        for (size_t j = 0; j < keysLen; ++j)
        {
            MoveHolder<KEY> k(static_cast<KEY>(keys[j]));
            ASSERTV(LINE, j, map_size == mX.size());
            typename MH_unordered_map::iterator it =
                   mX.insert_or_assign(mX.end(), MoveUtil::move(k), values[j]);
            ASSERTV(LINE, j, KEY(keys[j])     == it->first);
            ASSERTV(LINE, j, VALUE(values[j]) == it->second);
            ASSERTV(LINE, j, ('Y' == results[j]) == k.hasBeenMoved());
            if ('Y' == results[j]) {
                ASSERTV(LINE, j, ++map_size == mX.size());
            }
        }
    }
}


template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase42()
{
    // --------------------------------------------------------------------
    // TESTING FREE FUNCTION 'BSL::ERASE_IF'
    //
    // Concerns:
    //: 1 The free function exists, and is callable with a map.
    //
    // Plan:
    //: 1 Fill a map with known values, then attempt to erase some of
    //:   the values using 'bsl::erase_if'.  Verify that the resultant map
    //:   is the right size, contains the correct values, and that the
    //:   value returned from the functions is correct.
    //
    // Testing:
    //   size_t erase_if(map&, PREDICATE);
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
        typedef bsl::pair<const KEY, VALUE> PAIR;
        int             LINE = DATA[i].d_line;
        const char     *initial = DATA[i].d_initial_p;
        size_t          initialLen = strlen(initial);
        const char     *results = DATA[i].d_results_p;
        size_t          resultsLen = strlen(results);

        Obj mX;
        Obj mRes;

        for (size_t j = 0; j < initialLen; ++j)
        {
            mX.insert(PAIR(initial[j], 0));
        }

        for (size_t j = 0; j < resultsLen; ++j)
        {
            mRes.insert(PAIR(results[j], 0));
        }

        EqKeyPred<KEY, VALUE> pred(DATA[i].d_element);
        size_t                ret   = bsl::erase_if(mX, pred);

        // Is the modified container correct?
        ASSERTV(LINE, mX == mRes);

        // Is the return value correct?
        ASSERTV(LINE, ret == initialLen - resultsLen);
        }
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <int NUM_KEY_ARGS,
          int NK1,
          int NK2,
          int NK3,
          int NUM_VALUE_ARGS,
          int NV1,
          int NV2,
          int NV3>
typename TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::Iter
TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase32a_RunTest(
                                                               Obj   *target,
                                                               CIter  hint,
                                                               bool   inserted)
{
    if (veryVeryVerbose) printf("32a_Runtest<%d,%d,%d,%d,%d,%d,%d,%d>\n",
                                NUM_KEY_ARGS, NK1, NK2, NK3,
                                NUM_VALUE_ARGS, NV1, NV2, NV3);

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NK1 == 1> MOVE_K1 = {};
    static const bsl::integral_constant<bool, NK2 == 1> MOVE_K2 = {};
    static const bsl::integral_constant<bool, NK3 == 1> MOVE_K3 = {};
    static const bsl::integral_constant<bool, NV1 == 1> MOVE_V1 = {};
    static const bsl::integral_constant<bool, NV2 == 1> MOVE_V2 = {};
    static const bsl::integral_constant<bool, NV3 == 1> MOVE_V3 = {};

    bslma::TestAllocator  scratch("scratch", veryVeryVeryVerbose);
    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 32 is not a test allocator!");
        return target->end();                                         // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;
    const Obj Y(X, &scratch);

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);

    Iter result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUFK1;
        ConstrUtil::construct(BUFK1.address(), &aa, K01);
        typename KEY::ArgType01& AK1 = BUFK1.object();
        bslma::DestructorGuard<typename KEY::ArgType01> GK1(&AK1);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUFK2;
        ConstrUtil::construct(BUFK2.address(), &aa, K02);
        typename KEY::ArgType02& AK2 = BUFK2.object();
        bslma::DestructorGuard<typename KEY::ArgType02> GK2(&AK2);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUFK3;
        ConstrUtil::construct(BUFK3.address(), &aa, K03);
        typename KEY::ArgType03& AK3 = BUFK3.object();
        bslma::DestructorGuard<typename KEY::ArgType03> GK3(&AK3);

        bsls::ObjectBuffer<typename VALUE::ArgType01> BUFV1;
        ConstrUtil::construct(BUFV1.address(), &aa, V01);
        typename VALUE::ArgType01& AV1 = BUFV1.object();
        bslma::DestructorGuard<typename VALUE::ArgType01> GV1(&AV1);

        bsls::ObjectBuffer<typename VALUE::ArgType02> BUFV2;
        ConstrUtil::construct(BUFV2.address(), &aa, V02);
        typename VALUE::ArgType02& AV2 = BUFV2.object();
        bslma::DestructorGuard<typename VALUE::ArgType02> GV2(&AV2);

        bsls::ObjectBuffer<typename VALUE::ArgType03> BUFV3;
        ConstrUtil::construct(BUFV3.address(), &aa, V03);
        typename VALUE::ArgType03& AV3 = BUFV3.object();
        bslma::DestructorGuard<typename VALUE::ArgType03> GV3(&AV3);

        u::CompareProctor<Obj> proctor(Y, X);

        switch (NUM_KEY_ARGS) {
          case 0: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(hint,
                                         std::piecewise_construct,
                                         std::forward_as_tuple(),
                                         std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 1: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 2: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                       testArg(&AK2, MOVE_K2)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 3: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace_hint(
                                 hint,
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                       testArg(&AK2, MOVE_K2),
                                                       testArg(&AK3, MOVE_K3)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2),
                                                      testArg(&AK3, MOVE_K3)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2),
                                                      testArg(&AK3, MOVE_K3)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace_hint(
                                hint,
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2),
                                                      testArg(&AK3, MOVE_K3)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }

        proctor.release();

        ASSERTV(inserted, mX.end() == hint ||
                                         inserted == (&(*result) != &(*hint)));

        ASSERTV(MOVE_K1, AK1.movedFrom(),
               MOVE_K1 == (MoveState::e_MOVED == AK1.movedFrom()) || 2 == NK1);
        ASSERTV(MOVE_K2, AK2.movedFrom(),
               MOVE_K2 == (MoveState::e_MOVED == AK2.movedFrom()) || 2 == NK2);
        ASSERTV(MOVE_K3, AK3.movedFrom(),
               MOVE_K3 == (MoveState::e_MOVED == AK3.movedFrom()) || 2 == NK3);

        ASSERTV(MOVE_V1, AV1.movedFrom(),
               MOVE_V1 == (MoveState::e_MOVED == AV1.movedFrom()) || 2 == NV1);
        ASSERTV(MOVE_V2, AV2.movedFrom(),
               MOVE_V2 == (MoveState::e_MOVED == AV2.movedFrom()) || 2 == NV2);
        ASSERTV(MOVE_V3, AV3.movedFrom(),
               MOVE_V3 == (MoveState::e_MOVED == AV3.movedFrom()) || 2 == NV3);

        const KEY&   K = result->first;
        const VALUE& V = result->second;

        ASSERTV(K01, K.arg01(), K01 == K.arg01() || 2 == NK1);
        ASSERTV(K02, K.arg02(), K02 == K.arg02() || 2 == NK2);
        ASSERTV(K03, K.arg03(), K03 == K.arg03() || 2 == NK3);

        ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == NV1);
        ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == NV2);
        ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == NV3);

        u::TestAllocatorUtil::test(K, oa);
        u::TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

    return result;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <int NUM_KEY_ARGS,
          int NK1,
          int NK2,
          int NK3,
          int NUM_VALUE_ARGS,
          int NV1,
          int NV2,
          int NV3>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase31a_RunTest(
                                                                Obj  *target,
                                                                bool  inserted)
{
    if (veryVeryVerbose) printf("31a_Runtest<%d,%d,%d,%d,%d,%d,%d,%d>\n",
                                NUM_KEY_ARGS, NK1, NK2, NK3,
                                NUM_VALUE_ARGS, NV1, NV2, NV3);

    // In C++17, these become the simpler-to-name 'bool_constant'.

    static const bsl::integral_constant<bool, NK1 == 1> MOVE_K1 = {};
    static const bsl::integral_constant<bool, NK2 == 1> MOVE_K2 = {};
    static const bsl::integral_constant<bool, NK3 == 1> MOVE_K3 = {};
    static const bsl::integral_constant<bool, NV1 == 1> MOVE_V1 = {};
    static const bsl::integral_constant<bool, NV2 == 1> MOVE_V2 = {};
    static const bsl::integral_constant<bool, NV3 == 1> MOVE_V3 = {};

    bslma::TestAllocator  scratch("scratch", veryVeryVeryVerbose);
    bslma::TestAllocator *testAlloc = dynamic_cast<bslma::TestAllocator *>(
                                          target->get_allocator().mechanism());
    if (!testAlloc) {
        ASSERT(!"Allocator in test case 31 is not a test allocator!");
        return;                                                       // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target;  const Obj& X = mX;
    const Obj Y(X, &scratch);

    bslma::TestAllocator aa("args", veryVeryVeryVerbose);

    pair<Iter, bool> result;

    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {

        // Construct all arguments inside the exception test loop as the
        // exception thrown after moving only a portion of arguments leave the
        // moved arguments in a valid, but unspecified state.
        bsls::ObjectBuffer<typename KEY::ArgType01> BUFK1;
        ConstrUtil::construct(BUFK1.address(), &aa, K01);
        typename KEY::ArgType01& AK1 = BUFK1.object();
        bslma::DestructorGuard<typename KEY::ArgType01> GK1(&AK1);

        bsls::ObjectBuffer<typename KEY::ArgType02> BUFK2;
        ConstrUtil::construct(BUFK2.address(), &aa, K02);
        typename KEY::ArgType02& AK2 = BUFK2.object();
        bslma::DestructorGuard<typename KEY::ArgType02> GK2(&AK2);

        bsls::ObjectBuffer<typename KEY::ArgType03> BUFK3;
        ConstrUtil::construct(BUFK3.address(), &aa, K03);
        typename KEY::ArgType03& AK3 = BUFK3.object();
        bslma::DestructorGuard<typename KEY::ArgType03> GK3(&AK3);

        bsls::ObjectBuffer<typename VALUE::ArgType01> BUFV1;
        ConstrUtil::construct(BUFV1.address(), &aa, V01);
        typename VALUE::ArgType01& AV1 = BUFV1.object();
        bslma::DestructorGuard<typename VALUE::ArgType01> GV1(&AV1);

        bsls::ObjectBuffer<typename VALUE::ArgType02> BUFV2;
        ConstrUtil::construct(BUFV2.address(), &aa, V02);
        typename VALUE::ArgType02& AV2 = BUFV2.object();
        bslma::DestructorGuard<typename VALUE::ArgType02> GV2(&AV2);

        bsls::ObjectBuffer<typename VALUE::ArgType03> BUFV3;
        ConstrUtil::construct(BUFV3.address(), &aa, V03);
        typename VALUE::ArgType03& AV3 = BUFV3.object();
        bslma::DestructorGuard<typename VALUE::ArgType03> GV3(&AV3);

        u::CompareProctor<Obj> proctor(Y, X);

        switch (NUM_KEY_ARGS) {
          case 0: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(),
                                    std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 1: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 2: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                       testArg(&AK2, MOVE_K2)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          case 3: {
            switch (NUM_VALUE_ARGS) {
              case 0: {
                result = mX.emplace(
                                 std::piecewise_construct,
                                 std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                       testArg(&AK2, MOVE_K2),
                                                       testArg(&AK3, MOVE_K3)),
                                 std::forward_as_tuple());
              } break;
              case 1: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2),
                                                      testArg(&AK3, MOVE_K3)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1)));
              } break;
              case 2: {
                result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2),
                                                      testArg(&AK3, MOVE_K3)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2)));
              } break;
              case 3: {
               result = mX.emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(testArg(&AK1, MOVE_K1),
                                                      testArg(&AK2, MOVE_K2),
                                                      testArg(&AK3, MOVE_K3)),
                                std::forward_as_tuple(testArg(&AV1, MOVE_V1),
                                                      testArg(&AV2, MOVE_V2),
                                                      testArg(&AV3, MOVE_V3)));
              } break;
              default: {
                ASSERTV(!"Invalid # of args!");
              } break;
            }
          } break;
          default: {
            ASSERTV(!"Invalid # of args!");
          } break;
        }

        proctor.release();

        ASSERTV(inserted, inserted == result.second);

        ASSERTV(MOVE_K1, AK1.movedFrom(),
               MOVE_K1 == (MoveState::e_MOVED == AK1.movedFrom()) || 2 == NK1);
        ASSERTV(MOVE_K2, AK2.movedFrom(),
               MOVE_K2 == (MoveState::e_MOVED == AK2.movedFrom()) || 2 == NK2);
        ASSERTV(MOVE_K3, AK3.movedFrom(),
               MOVE_K3 == (MoveState::e_MOVED == AK3.movedFrom()) || 2 == NK3);

        ASSERTV(MOVE_V1, AV1.movedFrom(),
               MOVE_V1 == (MoveState::e_MOVED == AV1.movedFrom()) || 2 == NV1);
        ASSERTV(MOVE_V2, AV2.movedFrom(),
               MOVE_V2 == (MoveState::e_MOVED == AV2.movedFrom()) || 2 == NV2);
        ASSERTV(MOVE_V3, AV3.movedFrom(),
               MOVE_V3 == (MoveState::e_MOVED == AV3.movedFrom()) || 2 == NV3);

        const KEY& K = result.first->first;

        ASSERTV(K01, K.arg01(), K01 == K.arg01() || 2 == NK1);
        ASSERTV(K02, K.arg02(), K02 == K.arg02() || 2 == NK2);
        ASSERTV(K03, K.arg03(), K03 == K.arg03() || 2 == NK3);

        const VALUE& V = result.first->second;

        if (inserted) {
            ASSERTV(V01, V.arg01(), V01 == V.arg01() || 2 == NV1);
            ASSERTV(V02, V.arg02(), V02 == V.arg02() || 2 == NV2);
            ASSERTV(V03, V.arg03(), V03 == V.arg03() || 2 == NV3);
        }

        u::TestAllocatorUtil::test(K, oa);
        u::TestAllocatorUtil::test(V, oa);
    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase38()
{
    // ------------------------------------------------------------------------
    // TESTING ABSENCE OF ERASE AMBIGUITY
    //  'std::unordered_map::erase' takes an iterator in C++03, but a
    //  const_iterator in C++0x.  This breaks code where the unordered_map's
    //  'key_type' has a constructor which accepts an iterator (for example a
    //  template constructor), as the compiler cannot choose between
    //  'erase(const key_type&)' and 'erase(const_iterator)'. As BDE library
    //  duplicates 'std' interfaces, it has the same problem. The solution is
    //  to restore the iterator overload in addition to the const_iterator
    //  overload.
    //  See https://cplusplus.github.io/LWG/lwg-defects.html#2059
    //
    // Concerns:
    //: 1 Adding the iterator overload takes away the ambiguity of 'erase'
    //:   method.
    //
    // Plan:
    //: 1 Using brute force and a specially tailored test type,
    //:   'EraseAmbiguityTestType', we verify that appropriate overload is
    //:   deduced successfully.  Note that this is a compile-only test; runtime
    //:   values are not checked.  (C-1)
    //
    // Testing:
    //   CONCERN: 'erase' overload is deduced correctly.
    // ------------------------------------------------------------------------

    VALUE                                 value(5);
    KEY                                   key(value);
    bsl::unordered_map<KEY, VALUE>        mX;
    const bsl::unordered_map<KEY, VALUE>& X = mX;

    mX.insert(bsl::pair<KEY, VALUE>(key, value));
    ASSERTV(X.size(), 1 == X.size());
    runErasure(mX, key);
    ASSERTV(X.size(), 0 == X.size());
}

template <class KEY, class MAPPED, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, MAPPED, HASH, EQUAL, ALLOC>::testCase37()
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
        P(bsls::NameOf<MAPPED>())
        P(bsls::NameOf<HASH>())
        P(bsls::NameOf<EQUAL>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: 23.5.4 Class template 'unordered_map' [unord.map]

    // page 882 - 883:
    //..
    //  // 23.5.4.2, construct/copy/destroy:
    //  unordered_map& operator=(unordered_map&&)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_move_assignable<Hash>::value &&
    //               is_nothrow_move_assignable<Pred>::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;  const Obj& X = mX;    (void) X;
        Obj mY;  const Obj& Y = mY;    (void) Y;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.get_allocator()));
    }

    // page 883:
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

    // page 883:
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

    // page 883:
    //..
    //  // 23.5.4.4, modifiers:
    //  void swap(unordered_map&)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //              is_nothrow_swappable_v<Hash> &&
    //              is_nothrow_swappable_v<Pred>);
    //  void clear() noexcept;
    //..

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        ASSERT(false
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 884:
    //..
    //  // bucket interface:
    //  size_type bucket_count() const noexcept;
    //  size_type max_bucket_count() const noexcept;
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.bucket_count()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_bucket_count()));
    }

    // page 884:
    //..
    //  // hash policy:
    //  float load_factor() const noexcept;
    //  float max_load_factor() const noexcept;
    //  void max_load_factor(float z);
    //  void rehash(size_type n);
    //  void reserve(size_type n);
    //..

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.load_factor()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_load_factor()));
    }

    // page 884:
    //..
    //  // 23.5.4.5, swap:
    //  template <class Key, class T, class Hash, class Pred, class Alloc>
    //  void swap(unordered_map<Key, T, Hash, Pred, Alloc>& x,
    //            unordered_map<Key, T, Hash, Pred, Alloc>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj mX;    (void) mX;
        Obj mY;    (void) mY;

        ASSERT(false == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(mX, mY)));
    }
}

template <class KEY, class MAPPED, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, MAPPED, HASH, EQUAL, ALLOC>::testCase35()
{
    // ------------------------------------------------------------------------
    // BUCKET GROWTH
    //
    // Concerns:
    //: 1 That 'rehash' will increase the bucket count of the container to at
    //:   least the specified amount.
    //: 2 That 'load_factor' correctly reflects the ratio of the number of
    //:   elements in the container to the number of buckets.
    //: 3 That 'load_factor' never exceeds 'max_load_factor'.
    //: 4 That setting 'max_load_factor' before populating the container
    //:   affects the bucket count.
    //: 5 That reducing 'max_load_factor' below the current load factor will
    //:   result in an increase in the bucket count.
    //: 6 That a call to 'reserve' will allow the container to be populated up
    //:   to the capacity specified by the 'reserve' call without any increase
    //:   in the bucket count after the 'reserve' call returns.
    //: 7 That 'max_load_factor(float)', 'rehash', and 'reserve' are all
    //:   exception neutral.
    //
    // Plan:
    //: 1 Populate a container from a spec, then call 'rehash' with a doubling
    //:   of the bucket count, and observe the bucket count increases to
    //:   greater than that value.
    //: 2 Iterate through different load factors, decreasing by a factor of
    //:   three each time.  Create a container and set the load factor before
    //:   populating it from a spec.  Observe that, once populated, the bucket
    //:   count is higher than with the previous, higher load factor.  Then
    //:   call 'max_load_factor' to force the load factor to at least halve,
    //:   and observe that the bucket count more than doubles.
    //: 3 Iterate 'len2' from 0 to 'LENGTH' ('LENGTH' being the length of the
    //:   spec).  Each pass, create a container and populate it with 'len2'
    //:   elements.  Then call 'reserve(LENGTH)'.  Then insert the remaining
    //:   'LENGTH - len2' elements, and observe that the bucket count doesn't
    //:   change.
    //: 4 Enclose the calls to 'max_load_factor', 'rehash', and 'reserve' in
    //:   exception test blocks with 'CompareProctor's to ensure they are
    //:   exception neutral, and check that the bucket count doesn't change
    //:   between subsequent passes through the beginning of the exception test
    //:   blocks.
    //
    // Testing
    //   size_t bucket_count() const;
    //   void max_load_factor(float);
    //   float max_load_factor() const;
    //   void rehash(size_type);
    //   void reserve(size_type);
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING BUCKET GROWTH: %s\n"
                        "=====================\n",
                        NameOf<KEY>().name());

    {
        typedef float (Obj::*MP)() const;
        MP mp = &Obj::load_factor;
        (void) mp;
        mp    = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(float);
        MP mp = &Obj::max_load_factor;
        (void) mp;
    }
    {
        typedef void (Obj::*MP)(size_t);
        MP mp = &Obj::reserve;
        (void) mp;
        mp    = &Obj::rehash;
        (void) mp;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bool done = false;
    const char *prevSpec = "+";
    const char *SPEC;
    for (int ti = 0; ti < NUM_DATA; ++ti, prevSpec = SPEC) {
        const int         LINE   = DATA[ti].d_line;
                          SPEC   = DATA[ti].d_results_p;
        const size_t      LENGTH = strlen(DATA[ti].d_results_p);

        if (ti && strlen(prevSpec) == LENGTH) {
            continue;
        }
        ASSERT(!done);
        done |= DEFAULT_MAX_LENGTH == LENGTH;

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        if (veryVeryVerbose) Q(Test 'rehash');
        {
            Obj mX(&sa);         const Obj& X = gg(&mX, SPEC);
            Obj mY(X, &scratch); const Obj& Y = mY;

            const size_t BC = X.bucket_count();

            ASSERTV((LENGTH > 0) == (BC > 1));

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                u::CompareProctor<Obj> proctor(Y, X);
                ASSERTV(BC == X.bucket_count());

                mX.rehash(2 * BC);

                // proctor.release(); -- 'rehash' doesn't change salient
                //                       attributes.
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV((LENGTH > 0) == (X.bucket_count() > 2 * BC));
        }

        if (veryVeryVerbose) Q(Test 'load_factor' and 'max_load_factor');
        {
            const float loadFactors[] = { 3.0f, 1.0f, 1.0f/3 };
            enum { NUM_LOAD_FACTORS =
                                    sizeof loadFactors / sizeof *loadFactors };

            size_t prevBucketCount = 0;
            for (size_t tj = 0; tj < NUM_LOAD_FACTORS; ++tj) {
                const float MAX_LOAD_FACTOR = loadFactors[tj];

                Obj        mX(&sa);
                const Obj& X = mX;

                mX.max_load_factor(MAX_LOAD_FACTOR);

                ASSERTV(X.max_load_factor() == MAX_LOAD_FACTOR);

                gg(&mX, SPEC);

                const size_t BC = X.bucket_count();

                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(u::nearlyEqual<double>(
                                          X.load_factor(),
                                          static_cast<double>(X.size()) /
                                          static_cast<double>(BC)));

                if (LENGTH > 0) {
                    if (LENGTH > 3) {
                        ASSERTV(NameOf<KEY>(), SPEC, BC, prevBucketCount,
                                    X.max_load_factor(), BC > prevBucketCount);
                        prevBucketCount = BC;
                    }

                    Obj mY(X, &scratch);    const Obj& Y = mY;

                    const float newLoadFactor = X.load_factor() / 2;
                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        ++numPasses;

                        u::CompareProctor<Obj> proctor(Y, X);
                        ASSERTV(BC == X.bucket_count());

                        mX.max_load_factor(newLoadFactor);

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(!PLAT_EXC || numPasses > 1);

                    ASSERTV(newLoadFactor == X.max_load_factor());

                    ASSERTV(X.bucket_count() > 2 * BC);
                    ASSERTV(X.load_factor() <= X.max_load_factor());
                }
            }
        }

        if (veryVeryVerbose) Q(Test 'reserve');
        {
            for (size_t len2 = 0; len2 <= LENGTH; ++len2) {
                TestValues values(SPEC, &sa);

                Obj mX(values.begin(), values.index(len2), &sa);
                const Obj& X = mX;
                Obj mY(X, &scratch);    const Obj& Y = mY;

                const size_t PRE_BC = X.bucket_count();

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    u::CompareProctor<Obj> proctor(Y, X);
                    ASSERT(X.bucket_count() == PRE_BC);

                    ++numPasses;

                    mX.reserve(LENGTH);

                    // proctor.release(); -- 'reserve' doesn't change salient
                    //                       attributes.
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || len2 || 0 == LENGTH || numPasses > 1);

                const size_t BC = X.bucket_count();
                ASSERTV(X.load_factor() <= X.max_load_factor());
                ASSERTV(0.9999 * static_cast<double>(LENGTH)
                                      / static_cast<double>(X.bucket_count()) <
                        X.max_load_factor());

                mX.insert(values.index(len2), values.end());

                ASSERTV(LINE, SPEC, LENGTH == X.size());
                ASSERTV(u::verifySpec(X, SPEC));

                ASSERTV(BC == X.bucket_count());
            }
        }
    }

    ASSERT(done);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase34()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENTAL ACCESS WITH MOVABLE KEY
    //
    // Concerns:
    //: 1 'operator[]' returns the value associated with the key.
    //:
    //: 2 'operator[]' can be used to set the value.
    //:
    //: 3 'operator[]' creates a default constructed value if the key does not
    //:   exist in the object.
    //:
    //: 4 New keys are move-inserted into the container.
    //:
    //: 5 Any memory allocations come from the object's allocator.
    //:
    //: 6 'operator[]' is exception neutral.
    //
    // Plan:
    //: 1 Using the table-driven technique, for each set of key-value pairs:
    //:
    //:   1 For each key-value pair in the object:
    //:
    //:     1 Verify 'operator[]' returns the expected 'VALUE'.
    //:
    //:     2 Verify no memory is allocated.
    //:
    //:     3 Set the value to something different with 'operator[]' and verify
    //:       the value is changed.  Set the value back to its original value.
    //:
    //:     4 Verify memory use did not increase.
    //:
    //:   2 Verify no memory is allocated from the default allocator.
    //:
    //:   3 Invoke 'operator[]' using an out-of-range key under the presence of
    //:     exceptions.
    //:
    //:   4 Verify that a default 'VALUE' is created.
    //:
    //:   5 Verify memory usage is as expected.
    //
    // Testing:
    //   VALUE& operator[](KEY&&);
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING ELEMENTAL ACCESS WITH MOVABLE KEY: (%s, %s)\n"
                        "-----------------------------------------\n",
                        NameOf<KEY>().name(), NameOf<VALUE>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues yz("yz");

    const KEY&   ZK = yz[0].first;   // A value not in any spec.
    const VALUE& ZM = yz[1].second;  // A value not in any spec.

    int iterations = 0;
    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE    = DATA[ti].d_line;
        const char *const SPEC    = DATA[ti].d_spec_p;
        const size_t      LENGTH  = strlen(DATA[ti].d_results_p);

        const TestValues  VALUES(DATA[ti].d_results_p);

        for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
            const char CONFIG = cfg;

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            bslma::TestAllocator *valAllocator = 0;
            switch (cfg) {
              case 'a': {
                // inserted key has same allocator
                valAllocator = &oa;
              } break;
              case 'b': {
                // inserted key has different allocator
                valAllocator = &za;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }
            bslma::TestAllocator& sa = *valAllocator;

            if (veryVeryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            bsltf::MoveState::Enum mFromState, mIntoState;

            // Observe that 'operator[]' finding an already existing element
            // does not move the key.

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const KEY&   K = VALUES[tj].first;
                const VALUE& M = VALUES[tj].second;

                ASSERT(1 == X.count(K));

                bsls::ObjectBuffer<KEY> buffer;
                KEY *keyPtr = buffer.address();
                TTF::emplace(keyPtr, TTF::getIdentifier(K), &sa);

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, K, mX.end() != mX.find(K));
                ASSERTV(LINE, M, M == mX[MoveUtil::move(*keyPtr)]);

                mFromState = TTF::getMovedFromState(*keyPtr);

                ASSERTV(!k_IS_KEY_MOVE_AWARE
                                      || MoveState::e_NOT_MOVED == mFromState);
                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(LINE, tj, oam.isInUseSame());

                // Verify 'operator[]' can be used to set the value.

                u::copyAssignTo(
                       bsls::Util::addressOf(mX[MoveUtil::move(*keyPtr)]), ZM);

                bsltf::MoveState::Enum mFromState, mIntoState;

                mFromState = TTF::getMovedFromState(K);
                ASSERTV(NameOf<KEY>(),
                 !k_IS_KEY_MOVE_AWARE || MoveState::e_NOT_MOVED == mFromState);

                ASSERTV(ZM == X.find(K)->second);

                u::copyAssignTo(
                        bsls::Util::addressOf(mX[MoveUtil::move(*keyPtr)]), M);
                mFromState = TTF::getMovedFromState(*keyPtr);

                ASSERTV(M == X.find(K)->second);
                ASSERTV(NameOf<KEY>(),
                 !k_IS_KEY_MOVE_AWARE || MoveState::e_NOT_MOVED == mFromState);

                mIntoState = TTF::getMovedIntoState(mX.find(K)->first);
                ASSERTV(!k_IS_KEY_MOVE_AWARE
                                      || MoveState::e_NOT_MOVED == mIntoState);

                ASSERTV(NameOf<Pair>(), k_TYPE_ALLOC || oam.isTotalSame());
                ASSERTV(NameOf<Pair>(),                 oam.isInUseSame());

                bslma::DestructionUtil::destroy(keyPtr);
            }

            // Now observe that 'operator[]' creating a new node, and moving
            // the key.

            {
                const size_t SIZE = X.size();

                const bsls::Types::Int64 B = oa.numBlocksInUse();

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                // Constructing default 'VALUE' to compare with.  Note, that we
                // construct default value this way to support some types that
                // do not meet C++ requirement of 'default-insertable'.
                bslma::TestAllocator value("value", veryVeryVeryVerbose);
                ALLOC                xvalue(&value);

                bsls::ObjectBuffer<VALUE> d;
                bsl::allocator_traits<ALLOC>::construct(xvalue, d.address());
                bslma::DestructorGuard<VALUE> defaultValueGuard(d.address());

                const VALUE& D = d.object();

                Obj        mW(&scratch);
                const Obj& W = gg(&mW, SPEC);

                VALUE *ret;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    u::CompareProctor<Obj> compProctor(X, W);

                    bsls::ObjectBuffer<KEY> buffer;
                    KEY *keyPtr = buffer.address();
                    TTF::emplace(keyPtr, TTF::getIdentifier(ZK), &sa);
                    bslma::DestructorGuard<KEY> guard(keyPtr);

                    ret = bsls::Util::addressOf(mX[MoveUtil::move(*keyPtr)]);
                    mFromState = TTF::getMovedFromState(*keyPtr);

                    compProctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 A = oa.numBlocksInUse();

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
                (void) iterations;    // suppress 'unused'

                MoveState::Enum exp = k_IS_KEY_MOVE_AWARE
                                      ? (k_IS_KEY_WELL_BEHAVED && &oa != &sa
                                         ? MoveState::e_NOT_MOVED
                                         : MoveState::e_MOVED)
                                      : MoveState::e_UNKNOWN;

                ASSERTV(NameOf<KEY>(), SPEC, mFromState, exp,
                                                            exp == mFromState);
#else
                // TBD: See the TBD in the .h file in 'operator[key&&]' where
                // it has been temporarily hobbled in C++03 (and this test
                // similarly had to be hobbled to not expose the fact that
                // the imp is broken.  Once 'operator[key&&]' is restored,
                // restore this test.

                (void) mFromState;    // suppress 'unused'
                if (0 == iterations++) {
                    printf("'From' test suppressed on C++03, type: %s\n",
                           NameOf<KEY>().name());
                }
#endif

                ASSERTV(LINE, SIZE, SIZE + 1 == X.size());
                ASSERTV(LINE, !k_IS_VALUE_DEFAULT_CONSTRUCTIBLE || D == *ret);
                ASSERTV(!k_TYPE_ALLOC || B < A);

                mIntoState = TTF::getMovedIntoState(mX.find(ZK)->first);
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
                ASSERTV(NameOf<KEY>(), SPEC, mIntoState, exp,
                                                            exp == mIntoState);
#else
                (void) mIntoState;    // suppress 'unused'
                if (1 == iterations++) {
                    printf("'Into' test suppressed on C++03, type: %s\n",
                           NameOf<KEY>().name());
                }
#endif
            }

            ASSERTV(LINE, 0 == da.numAllocations());
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase33_outOfLine()
{
    // ------------------------------------------------------------------------
    // TESTING INITIALIZER LIST C'TORS AND MANIPULATORS OUT OF LINE
    //
    // Concerns:
    //: 1 That constructors from an initializer list function correctly.
    //:   o The set of elements in the container is correct.
    //:   o If a hash functor is passed, it is used, otherwise the default
    //:     constructed hash functor is used.
    //:   o If a key equality compare functor is passed, it is used, otherwise
    //:     the default key equality compare functor is used.
    //:   o If an allocator is passed, it is used, otherwise the default
    //:     allocator is used.
    //: 2 That 'insert' called with an initializer list functions correctly.
    //: 3 That 'operator=' called with an initializer list functions correctly.
    //: 4 That the above 3 operations are all exception neutral.
    //
    // Plan:
    //: 1 Iterate through a set of initializer lists, with an expected
    //:   container size and an expected spec for each one.
    //:   o Nested within that, iterate though all possible c'tors that take
    //:     an initializer list.
    //:   o Use boolean variables to keep track of which arguments in addition
    //:     to the initializer list are passed to the c'tor.
    //:   o Do this in exception block, using test allocators, to detect if
    //:     any memory is leaked by any of the c'tors on a throw.
    //:   o Use accessors 'hash_function' and 'key_eq' to verify that these
    //:     fields are as expected.
    //:   o Verify that the state of the container corresponds to the
    //:     initializer_list passed.
    //: 2 Iterate again through the set of initializer lists.
    //:   o Iterate through a series of 'OTHER_SPECS', and initialize a
    //:     container to 'OTHER_SPEC' each iteration.
    //:   o Created a 'mergeSpec', which is a spec describing the expected
    //:     result of inserting the initializer list to a container already
    //:     initialized with 'OTHER_SPEC'.
    //:   o Call insert, in an exception block, of the initializer list.
    //:   o Observe that the result corresponds to 'mergedSpec'.
    //: 3 Iterate again through the set of initializer lists, with an expected
    //:   container size and an expected spec for each one.
    //:   o Iterate through a series of 'OTHER_SPECS', and initialize a
    //:     container to 'OTHER_SPEC' each iteration.
    //:   o Assign, within an exception block to the initializer list.
    //:   o Have a compare proctor in the exception block to establish that the
    //:     assignment is through a copy-swap operation.
    //:   o Observe that the state of the container matches the expected spec
    //:     associated with the initializer list (meaning that it is unaffected
    //:     by the initial state.
    //
    // Testing:
    //   void Obj(initializer_list<Pair>);
    //   void Obj(initializer_list<Pair>, size_t);
    //   void Obj(initializer_list<Pair>, size_t, HASH);
    //   void Obj(initializer_list<Pair>, size_t, HASH, EQUAL);
    //   void Obj(initializer_list<Pair>, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, HASH, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, HASH, EQUAL, const ALLOC&);
    //   void insert(initializer_list<Pair>);
    //   Obj& operator=(initializer_list<Pair>);
    // ------------------------------------------------------------------------

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    if (verbose) printf("INITIALIZER_LIST not supported -- no test: %s\n",
                        NameOf<KEY>().name());
#else
    if (verbose) printf(
           "TESTING INITIALIZER LIST C'TORS AND MANIPULATORS OUT OF LINE: %s\n"
           "============================================================\n",
           NameOf<KEY>().name());

    int numIters = 0;

    typedef std::initializer_list<Pair> InitList;
    typedef typename InitList::iterator InitListIt;

    bslma::TestAllocator ia("ia", veryVeryVeryVerbose);    // initializer list
    bslma::TestAllocator ta("ta", veryVeryVeryVerbose);    // testValues
    bslma::TestAllocator fa("fa", veryVeryVeryVerbose);    // footprint
    bslma::TestAllocator oa("oa", veryVeryVeryVerbose);    // other

    const char   testValuesSpec[] = { "ABCDE" };
    const size_t testValuesSpecLen = sizeof(testValuesSpec) - 1;

    TestValues testValues(testValuesSpec, &ia);
    ASSERT(testValues.size() == testValuesSpecLen);
    for (size_t ii = 0; ii < testValuesSpecLen; ++ii, ++numIters) {
        ASSERTV(NameOf<KEY>(), ii,
                          u::valueOf(testValues[ii].first) + u::k_CHAR_SHIFT ==
                                            u::valueOf(testValues[ii].second));
    }
    ASSERT(0 < numIters);
    numIters = 0;

    const Pair& VA = testValues[0];
    const Pair& VB = testValues[1];
    const Pair& VC = testValues[2];
    const Pair& VD = testValues[3];
    const Pair& VE = testValues[4];

    const u::TestHashFunctor<KEY> hf(5);
    const u::TestEqualityComparator<KEY> ec(5);

    const struct {
        int         d_line;
        size_t      d_expectedSize;
        InitList    d_list;
        const char *d_expSpec_p;
    } DATA[] = {
        { L_, 0, { },                            ""      },
        { L_, 1, { VA },                         "A"     },
        { L_, 1, { VA, VA, VA },                 "A"     },
        { L_, 2, { VC, VE, VC },                 "CE"    },
        { L_, 5, { VE, VD, VC, VA, VB },         "ABCDE" },
        { L_, 5, { VE, VD, VC, VE, VA, VD, VB }, "ABCDE" } };
    const int NUM_DATA = u::arrayLength(DATA);
    ASSERT(NUM_DATA > 4);
    const int MAX_LIST_LEN = 7;

    if (veryVerbose) printf("    C'tors with initializer list\n");

    bool done = false;
    int totalThrows = 0;
    for (char ctor = 'a'; ctor <= 'h'; ++ctor) {
        if (veryVeryVerbose) printf("    c'tor '%c'\n", ctor);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            int              LINE          = DATA[ti].d_line;
            size_t           EXPECTED_SIZE = DATA[ti].d_expectedSize;
            const InitList&  LIST          = DATA[ti].d_list;
            const char      *EXP_SPEC      = DATA[ti].d_expSpec_p;

            size_t listLen = bsl::distance(LIST.begin(), LIST.end());

            ASSERTV(LINE, listLen >= EXPECTED_SIZE);
            ASSERT(listLen <= MAX_LIST_LEN);

            if (veryVeryVerbose) {
                printf("    Line: %d, dist: " ZU ", esz: " ZU "\n",
                       LINE, listLen, EXPECTED_SIZE);
            }

            bslma::TestAllocator da("da", veryVeryVeryVerbose);    // default
            bslma::DefaultAllocatorGuard dag(&da);
#if defined(BDE_BUILD_TARGET_EXC)
            bslma::TestAllocator& usedAlloc = ctor < 'e' ? da : oa;
#endif

            Obj *p = 0;
            bool ibPassed = false;
            bool hfPassed = false;
            bool ecPassed = false;
            bool oaPassed = false;

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(usedAlloc) {
                ++numThrows;

                switch (ctor) {
                  case 'a': {
                      p = new (fa) Obj(LIST);
                  } break;
                  case 'b': {
                      p = new (fa) Obj(LIST, 1000);
                      ibPassed = true;
                  } break;
                  case 'c': {
                      p = new (fa) Obj(LIST, 1000, hf);
                      ibPassed = true;
                      hfPassed = true;
                  } break;
                  case 'd': {
                      p = new (fa) Obj(LIST, 1000, hf, ec);
                      ibPassed = true;
                      hfPassed = true;
                      ecPassed = true;
                  } break;
                  case 'e': {
                      p = new (fa) Obj(LIST, &oa);
                      oaPassed = true;
                  } break;
                  case 'f': {
                      p = new (fa) Obj(LIST, 1000, &oa);
                      ibPassed = true;
                      oaPassed = true;
                  } break;
                  case 'g': {
                      p = new (fa) Obj(LIST, 1000, hf, &oa);
                      ibPassed = true;
                      hfPassed = true;
                      oaPassed = true;
                  } break;
                  case 'h': {
                      p = new (fa) Obj(LIST, 1000, hf, ec, &oa);
                      ibPassed = true;
                      hfPassed = true;
                      ecPassed = true;
                      oaPassed = true;

                      done |= MAX_LIST_LEN == listLen;
                  } break;
                  default: {
                      ASSERTV(ctor, 0 && "invalid ctor choice");
                  } return;                                           // RETURN
                }

                ASSERT(p);
                ASSERT(static_cast<unsigned>(EXPECTED_SIZE) == p->size());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(u::verifySpec(*p, EXP_SPEC));

            ASSERTV(NameOf<KEY>(), numThrows, ctor, !PLAT_EXC ||
                           (0 == EXPECTED_SIZE && !ibPassed) || 0 < numThrows);
            totalThrows += numThrows;

            // Make sure parameters either got passed or were default
            // constructed appropriately.

            const size_t bc = p->bucket_count();
            ASSERTV(bc, ibPassed, ibPassed ? (bc > 1000) : (bc < 1000));

            const int hfId = p->hash_function().id();
            ASSERTV(hfId, hfPassed, hfId == (hfPassed ? 5 : 0));

            const int ecId = p->key_eq().id();
            ASSERTV(ecId, ecPassed, ecId == (ecPassed ? 5 : 0));

            ASSERTV(p->get_allocator().mechanism() == (oaPassed ? &oa : &da));

            // Make sure the container works OK.

            size_t ii = 0;
            for (InitListIt it = LIST.begin(); LIST.end() != it; ++ii, ++it) {
                const KEY& k   = it->first;

                ASSERTV(NameOf<KEY>(), 1 == p->count(k));

                const Pair& pr = *p->find(k);

                ASSERTV(NameOf<KEY>(), u::idOf(pr), u::valueOf(pr.second),
                       u::idOf(pr) + u::k_CHAR_SHIFT == u::valueOf(pr.second));
                ++numIters;
            }
            ASSERTV(ii, listLen == ii && p->size() <= ii);
            ASSERTV(listLen, p->empty(), !p->empty() || 0 == listLen);

            ASSERTV(p->empty() || oa.numBlocksInUse() || da.numBlocksInUse());
            ASSERTV(0 == (oaPassed ? da : oa).numBlocksTotal());

            fa.deleteObjectRaw(p);

            ASSERTV(ctor, 0 == fa.numBlocksInUse());
            ASSERTV(ctor, 0 == oa.numBlocksInUse());
            ASSERTV(ctor, 0 == da.numBlocksInUse());
        }
    }

    ASSERTV(fa.numBlocksTotal(), totalThrows,
              fa.numBlocksTotal() == ('h' + 1 - 'a') * NUM_DATA + totalThrows);

    ASSERT(NUM_DATA < numIters);
    ASSERT(done);

    if (veryVerbose) printf("    'insert' with initializer list\n");

    done = false; numIters = 0;

    const char *OTHER_SPECS[]      = { "", "G", "JIH", "ABG" };
    const int   NUM_OTHER_SPECS    = u::arrayLength(OTHER_SPECS);
    const int   MAX_OTHER_SPEC_LEN = 3;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const InitList&  LIST     = DATA[ti].d_list;
        const char      *EXP_SPEC = DATA[ti].d_expSpec_p;

        for (int tj = 0; tj < NUM_OTHER_SPECS; ++tj) {
            const char *OTHER_SPEC = OTHER_SPECS[tj];

            char mergedSpec[MAX_OTHER_SPEC_LEN + MAX_LIST_LEN + 1];
            strcpy(mergedSpec, OTHER_SPEC);
            strcat(mergedSpec, EXP_SPEC);
            char *pEnd = mergedSpec + strlen(mergedSpec);
            std::sort(         mergedSpec + 0, pEnd);
            pEnd = std::unique(mergedSpec + 0, pEnd);
            *pEnd = 0;
            const size_t EXP_SIZE = static_cast<size_t>(pEnd - mergedSpec);

            Obj mX(&oa);    const Obj& X = gg(&mX, OTHER_SPEC);

            ASSERTV(u::verifySpec(X, OTHER_SPEC));

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                mX.insert(LIST);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(X.size() == EXP_SIZE);
            ASSERTV(u::verifySpec(X, mergedSpec));
        }
    }

    if (veryVerbose) printf("    Assign from initializer list, no throw\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        size_t           EXPECTED_SIZE = DATA[ti].d_expectedSize;
        const InitList&  LIST          = DATA[ti].d_list;
        const char      *EXP_SPEC      = DATA[ti].d_expSpec_p;

        for (int tj = 0; tj < NUM_OTHER_SPECS; ++tj) {
            const char *OTHER_SPEC = OTHER_SPECS[tj];

            Obj mX(&oa);    const Obj& X = gg(&mX, OTHER_SPEC);

            ASSERTV(u::verifySpec(X, OTHER_SPEC));

            mX = LIST;

            ASSERTV(EXPECTED_SIZE == X.size());
            ASSERTV(u::verifySpec(X, EXP_SPEC));
        }
    }

    if (veryVerbose) printf("    Assign from initializer list, throw\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        size_t           EXPECTED_SIZE = DATA[ti].d_expectedSize;
        const InitList&  LIST          = DATA[ti].d_list;
        const char      *EXP_SPEC      = DATA[ti].d_expSpec_p;

        for (int tj = 0; tj < NUM_OTHER_SPECS; ++tj) {
            const char *OTHER_SPEC = OTHER_SPECS[tj];

            Obj        mX(&oa);
            const Obj& X = gg(&mX, OTHER_SPEC);

            Obj Y(X);

            ASSERTV(u::verifySpec(X, OTHER_SPEC));
            ASSERTV(u::verifySpec(Y, OTHER_SPEC));

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numThrows;

                u::CompareProctor<Obj> proctor(Y, X);

                Obj *ret = &(mX = LIST);
                ASSERT(&X == ret);

                proctor.release();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(!PLAT_EXC || !k_TYPE_ALLOC || 0 == EXPECTED_SIZE ||
                                                                0 < numThrows);
            ASSERT(EXPECTED_SIZE == X.size());
            ASSERTV(u::verifySpec(X, EXP_SPEC));
            ASSERTV(u::verifySpec(Y, OTHER_SPEC));
        }
    }
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase33_inline()
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
    //   void Obj(initializer_list<Pair>);
    //   void Obj(initializer_list<Pair>, size_t);
    //   void Obj(initializer_list<Pair>, size_t, HASH);
    //   void Obj(initializer_list<Pair>, size_t, HASH, EQUAL);
    //   void Obj(initializer_list<Pair>, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, HASH, const ALLOC&);
    //   void Obj(initializer_list<Pair>, size_t, HASH, EQUAL, const ALLOC&);
    //   void insert(initializer_list<Pair>);
    //   Obj& operator=(initializer_list<Pair>);
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

    CTestValues  testValues(testValuesSpec, &ta);
    ASSERT(testValues.size() == testValuesSpecLen);
    for (size_t ii = 0; ii < testValuesSpecLen; ++ii, ++numIters) {
        ASSERTV(NameOf<KEY>(), ii,
                          u::valueOf(testValues[ii].first) + u::k_CHAR_SHIFT ==
                                            u::valueOf(testValues[ii].second));
    }
    ASSERT(0 < numIters);

    const u::TestHashFunctor<KEY> hf(5);
    const u::TestEqualityComparator<KEY> ec(5);

    bool done = false;
    int totalThrows = 0;
    for (char ctor = 'a'; ctor <= 'h' ; ++ctor) {
        bool ibPassed = false;
        bool hfPassed = false;
        bool ecPassed = false;
        bool oaPassed = false;

        Obj *p = 0;

#define u_INIT_LIST                                                           \
              { testValues[1], testValues[0], testValues[2], testValues[0] }

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
        ASSERTV(NameOf<KEY>(), u::verifySpec(X, testValuesSpec));

        ASSERTV(!oa.numBlocksInUse() != !da.numBlocksInUse());

        fa.deleteObjectRaw(p);

        ASSERTV(ctor, 0 == fa.numBlocksInUse());
        ASSERTV(ctor, 0 == oa.numBlocksInUse());
        ASSERTV(ctor, 0 == da.numBlocksInUse());
    }

    ASSERTV(fa.numBlocksTotal() == 'h' + 1 - 'a' + totalThrows);

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");

        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            mX.insert(u_INIT_LIST);

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERTV(!k_TYPE_ALLOC || !PLAT_EXC || 0 < numThrows);
        ASSERTV(u::verifySpec(X, "ABCDE"));
    }

    {
        Obj mX(&oa);    const Obj& X = gg(&mX, "CDE");
        Obj Y(X, &oa);

        int numThrows = -1;
        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            ++numThrows;

            u::CompareProctor<Obj> proctor(Y, X);

            Obj *ret = &(mX = u_INIT_LIST);
            ASSERT(&X == ret);

            proctor.release();
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERTV(!k_TYPE_ALLOC || !PLAT_EXC || 0 < numThrows);
        ASSERTV(u::verifySpec(X, "ABC"));
        ASSERTV(u::verifySpec(Y, "CDE"));
    }

    {
        typedef bsl::pair<int, Obj> MyPair;

        bslma::DefaultAllocatorGuard dag(&ta);

        const MyPair& mp = MyPair(5, u_INIT_LIST);

        ASSERT(3 == mp.second.size());
    }

#undef u_INIT_LIST

    ASSERT(done);
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase32a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase31', where garbage hints are provided (based on the white-box
    //:   knowledge that the 'hint' argument is ignored).
    //:
    //: 2 'emplace_hint' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase32a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 10 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as arguments a pointer to a
    //:   modifiable container and a hint.
    //:   1 Create 10 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass in, call 'emplace_hint' with the hint passed in
    //:     as an argument and the corresponding constructor argument values,
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
    //: 3 Call 'testCase32a_RunTest' in various configurations:
    //:   1 For 1..3 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1, 2, 3 arguments, call with move flags set to '0',
    //:     '1', and each move flag set independently.
    //
    // Testing:
    //   iterator emplace_hint(const_iterator hint, Args&&... args);
    // ------------------------------------------------------------------------

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    if (verbose) printf("EMPLACE WITH HINT: not tested\n");
#else
    if (verbose) printf(
                 "TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT: %s\n"
                 "------------------------------------------------------\n",
                 NameOf<KEY>().name());

    Iter hint;

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (veryVerbose) printf("\nTesting emplace 1..3 args, move=1"
                            "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,0,2,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,1,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,1,1,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,1,1,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,1,1,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,1,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,2,1,1,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,2,1,1,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,2,1,1,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,3,1,1,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,3,1,1,1>(&mX, hint, true);
            hint = testCase32a_RunTest<2,1,1,2,3,1,1,1>(&mX, hint, true);
            hint = testCase32a_RunTest<3,1,1,1,3,1,1,1>(&mX, hint, true);
        }
    }

    if (veryVerbose) printf("\nTesting emplace 1..3 args, move=0"
                            "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,0,2,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,1,0,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,2,0,0,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,3,0,0,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,3,0,0,0>(&mX, hint, true);
        }
    }

    if (veryVerbose) printf("\nTesting emplace with 0 args"
                            "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);  const Obj& X = mX;

        hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
    }

    if (veryVerbose) printf("\nTesting emplace with 1 arg"
                            "\n--------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<0,2,2,2,1,1,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<1,0,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,0,2,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<1,0,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,1,0,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<1,0,2,2,1,1,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,1,2,2,1,1,2,2>(&mX, hint, false);
        }
    }

    if (veryVerbose) printf("\nTesting emplace with 2 args"
                            "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<0,2,2,2,2,0,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,1,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<0,2,2,2,2,1,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,0,2,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,0,2,2,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,0,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,0,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,0,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,0,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,0,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,0,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,1,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,1,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,1,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,1,0,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,0,0,2,2,1,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,0,1,2,2,1,1,2>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<2,1,0,2,2,1,1,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<2,1,1,2,2,1,1,2>(&mX, hint, false);
        }
    }

    if (veryVerbose) printf("\nTesting emplace with 3 args"
                            "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,1,0,0,3,1,0,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,0,1,1,3,0,1,1>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,0,1,0,3,0,1,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,1,0,1,3,1,0,1>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,0,0,1,3,0,0,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,1,1,0,3,0,1,1>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,0,1,1,3,0,1,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,1,0,0,3,1,0,0>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,1,0,1,3,1,0,1>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,0,1,0,3,0,1,0>(&mX, hint, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<3,1,1,0,3,1,1,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<3,0,0,1,3,1,0,0>(&mX, hint, false);
        }
    }
#else
    if (veryVerbose) printf("\nTesting emplace 0..3 args, move=0"
                            "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,0,2,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,0,2,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,0,2,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,1,0,2,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,1,0,2,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,1,0,2,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,2,0,0,2>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,2,0,0,2>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,2,0,0,2>(&mX, hint, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj& X = mX;

            hint = testCase32a_RunTest<0,2,2,2,3,0,0,0>(&mX, X.end(), true);
            hint = testCase32a_RunTest<1,0,2,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<2,0,0,2,3,0,0,0>(&mX, hint, true);
            hint = testCase32a_RunTest<3,0,0,0,3,0,0,0>(&mX, hint, true);
        }
    }
#endif
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase31a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the piecewise constructor
    //:   of the value type, up to 3 arguments.  Note that only the forwarding
    //:   of arguments is tested in this function; all other functionality is
    //:   tested in 'testCase31'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase31a_RunTest'
    //:   with first integer template parameter indicating the number of
    //:   arguments to use, the next 3 integer template parameters indicating
    //:   '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //:   the number of arguments), and taking as the only argument a pointer
    //:   to a modifiable container.
    //:   1 Create 3 argument values with their own argument-specific
    //:     allocator.
    //:
    //:   2 Based on the (first) template parameter indicating the number of
    //:     arguments to pass in, call 'emplace' with the corresponding
    //:     argument values, performing an explicit move of the argument if so
    //:     indicated by the template parameter corresponding to the argument,
    //:     all in the presence of injected exceptions.
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
    //:   1 For 1..3 arguments, call with the move flag set to '1' and then
    //:     with the move flag set to '0'.
    //:
    //:   2 For 1..3, call with move flags set to '0', '1', and each move flag
    //:     set independently.
    //
    // Testing:
    //   iterator emplace(Args&&...);
    // ------------------------------------------------------------------------

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    if (verbose) printf("Testcase31a -- no test\n");
#else
    if (verbose) printf(
                    "TESTING FORWARDING OF ARGUMENTS WITH EMPLACE: (%s, %s)\n"
                    "--------------------------------------------\n",
                    NameOf<KEY>().name(), NameOf<VALUE>().name());

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (veryVerbose) printf("\nTesting emplace 1..3 args, move=1"
                            "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<3,1,1,1,0,2,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<3,1,1,1,1,1,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<3,1,1,1,2,1,1,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,3,1,1,1>(&mX, true);
            testCase31a_RunTest<1,1,2,2,3,1,1,1>(&mX, true);
            testCase31a_RunTest<2,1,1,2,3,1,1,1>(&mX, true);
            testCase31a_RunTest<3,1,1,1,3,1,1,1>(&mX, true);
        }
    }

    if (veryVerbose) printf("\nTesting emplace 1..3 args, move=0"
                             "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,0,2,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,1,0,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,2,0,0,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<1,0,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<2,0,0,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<3,0,0,0,3,0,0,0>(&mX, true);
        }
    }

    if (veryVerbose) printf("\nTesting emplace with 0 args"
                            "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
    }

    if (veryVerbose) printf("\nTesting emplace with 1 arg"
                            "\n--------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<0,2,2,2,1,1,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,0,2,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,1,0,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<1,0,2,2,1,1,2,2>(&mX, true);
            testCase31a_RunTest<1,1,2,2,1,1,2,2>(&mX, false);
        }
    }

    if (veryVerbose) printf("\nTesting emplace with 2 args"
                            "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<0,2,2,2,2,0,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,1,0,2>(&mX, true);
            testCase31a_RunTest<0,2,2,2,2,1,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,0,2,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,0,2,2,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,0,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,0,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,0,1,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,0,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,0,1,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,0,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,1,0,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,1,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,1,0,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,1,0,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,0,0,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<2,0,1,2,2,1,1,2>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<2,1,0,2,2,1,1,2>(&mX, true);
            testCase31a_RunTest<2,1,1,2,2,1,1,2>(&mX, false);
        }
    }

    if (veryVerbose) printf("\nTesting emplace with 3 args"
                            "\n---------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,1,0,0,3,1,0,0>(&mX, true);
            testCase31a_RunTest<3,0,1,1,3,0,1,1>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,0,1,0,3,0,1,0>(&mX, true);
            testCase31a_RunTest<3,1,0,1,3,1,0,1>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,0,0,1,3,0,0,1>(&mX, true);
            testCase31a_RunTest<3,1,1,0,3,0,1,1>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,0,1,1,3,0,1,1>(&mX, true);
            testCase31a_RunTest<3,1,0,0,3,1,0,0>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,1,0,1,3,1,0,1>(&mX, true);
            testCase31a_RunTest<3,0,1,0,3,0,1,0>(&mX, false);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<3,1,1,0,3,1,1,0>(&mX, true);
            testCase31a_RunTest<3,0,0,1,3,1,0,0>(&mX, false);
        }
    }
#else
    if (veryVerbose) printf("\nTesting emplace 0..3 args, move=0"
                            "\n---------------------------------\n");
    {
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,0,2,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,0,2,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,1,0,2,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,1,0,2,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<1,0,2,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<2,0,0,2,2,0,0,2>(&mX, true);
            testCase31a_RunTest<3,0,0,0,2,0,0,2>(&mX, true);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);

            testCase31a_RunTest<0,2,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<1,0,2,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<2,0,0,2,3,0,0,0>(&mX, true);
            testCase31a_RunTest<3,0,0,0,3,0,0,0>(&mX, true);
        }
    }
#endif
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING SINGLE-ARG EMPLACE AND EMPLACE_HINT
    //
    // Concerns:
    //: 1 'emplace' returns a pair containing an iterator and a 'bool', and
    //:   'emplace_hint' just returns an iterator.
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and to the existing element if it did.
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
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' function
    //:   and will test proper forwarding of constructor arguments in test
    //:   'testCase31a'.
    //:
    //: 2 For emplace, we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   2 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //:   3 Assign the return value to a pair<iterator, bool>.
    //:
    //: 3 For emplace_hint, we will create objects of varying sizes and
    //:   capacities containing default values, and insert a 'value'.
    //:
    //:   1 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   2 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //:   3 Assign the return value to a pair<iterator, bool>.first, and set
    //:     the 'second' part of the pair to whether an addition to the
    //:     container is expected.
    //:
    //: 3 Repeat P-2 under the presence of injected exceptions.  (C-6)
    //
    // Testing:
    //   pair<iterator, bool> emplace(Args&&...);
    //   pair<iterator, bool> emplace_hint(CIter, Args&&...);
    // -----------------------------------------------------------------------

    if (verbose) printf(
                     "TESTING SINGLE-ARG EMPLACE AND EMPLACE_HINT: (%s, %s)\n"
                     "-------------------------------------------\n",
                     NameOf<KEY>().name(),
                     NameOf<VALUE>().name());

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  ----           --------

        { L_,   "A",           "Y",           },
        { L_,   "AAA",         "YNN",         },
        { L_,   "ABCDEFGH",    "YYYYYYYY",    },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN",  },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY", }
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 11;

    int doneA = 0, doneB = 0;
    if (veryVerbose) printf("\nTesting 'emplace' without exceptions.\n");
    {
        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int ii             = ti % NUM_DATA;
            const int         LINE   = DATA[ii].d_line;
            const char *const SPEC   = DATA[ii].d_spec_p;
            const char *const UNIQUE = DATA[ii].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);
            const bool        HINT   = ti / NUM_DATA;

            ASSERTV(LENGTH <= MAX_LENGTH);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH + 1] = { 0 };

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                pair<Iter, bool> RESULT;
                if (HINT) {
                    RESULT.second = IS_UNIQ;
                    RESULT.first  = mX.emplace_hint(hint, VALUES[tj]);
                }
                else {
                    RESULT = mX.emplace(VALUES[tj]);
                }

                ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                ASSERTV(LINE, tj, SIZE, u::eq(VALUES[tj], *(RESULT.first)));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(AA, BB, A, B,
                              !k_TYPE_ALLOC || !IS_UNIQ || (AA > BB && A > B));

                ASSERTV(LINE, tj, SIZE, SIZE + IS_UNIQ == X.size());

                ASSERTV(LINE, tj, X, EXPECTED, u::verifySpec(X, EXPECTED));

                doneA += HINT && tj + 1 == MAX_LENGTH;
            }
        }
    }

    if (veryVerbose) printf("\nTesting 'emplace' with injected exceptions.\n");
    {
        for (int ti = 0; ti < 2 * NUM_DATA; ++ti) {
            const int ii             = ti % NUM_DATA;
            const int         LINE   = DATA[ii].d_line;
            const char *const SPEC   = DATA[ii].d_spec_p;
            const char *const UNIQUE = DATA[ii].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);
            const bool        HINT   = ti / NUM_DATA;

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH + 1] = { 0 };

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mY(&scratch);    const Obj& Y = gg(&mY, EXPECTED);

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    u::CompareProctor<Obj> proctor(Y, X);

                    pair<Iter, bool> RESULT;
                    if (HINT) {
                        RESULT.second = IS_UNIQ;
                        RESULT.first  = mX.emplace_hint(hint, VALUES[tj]);
                    }
                    else {
                        RESULT = mX.emplace(VALUES[tj]);
                    }

                    proctor.release();

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                    ASSERTV(LINE, tj, SIZE, u::eq(VALUES[tj],
                                                  *(RESULT.first)));
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, X, EXPECTED, u::verifySpec(X, EXPECTED));

                doneB += HINT && tj + 1 == MAX_LENGTH;
            }
        }
    }

    ASSERTV(doneA, doneB, 1 == doneA && 1 == doneB);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING 'insert' SINGLE VALUE MOVE
    //
    // Concerns:
    //: 1 'insert' returns a pair containing an iterator and a 'bool'
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and to the existing element if it did.
    //:
    //: 3 The 'bool' returned is 'true' if a new element is inserted, and
    //:   'false' otherwise.
    //:
    //: 4 A new element is added to the container if the element did not
    //:   already exist.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 7 If a hint is supplied, it is ignored.
    //:
    //: 8 'insert' will work with a pair of type 'Obj::value_type', or from
    //:   another pair type from which 'Obj::value_type' is constructible.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   2 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 2 Repeat P-1 under the presence of injected exceptions.  (C-6)
    //:
    //: 3 Repeat P-1 and P-2 inserting object of type 'TValueType', which is
    //:   different from 'Obj::value_type' yet from which 'Obj::value_type' can
    //:   be constructed.
    //
    // Testing:
    //   pair<iterator, bool> insert(value_type&&);
    //   pair<iterator, bool> insert(ALT_VALUE_TYPE&&);
    //   iterator insert(CIter, value_type&&);
    //   iterator insert(CIter, ALT_VALUE_TYPE&&);
    // -----------------------------------------------------------------------

    if (verbose) printf("TESTING 'insert' SINGLE VALUE MOVE: %s\n"
                        "----------------------------------\n",
                        NameOf<Pair>().name());

    bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  ----           --------

        { L_,   "A",           "Y"           },    // All specs consist of
        { L_,   "AAA",         "YNN"         },    // sequential (but possibly
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },    // scrambled and/or
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  },    // redundant) letters
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" }     // beginning with 'A'.
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int MAX_LENGTH = 11;
    char      EXPECTED[MAX_LENGTH + 1];

    enum Mode {
        e_BEGIN,
        e_MOVE_MATCH = e_BEGIN,
        e_MOVE_MATCH_HINT,
        e_MOVE_NOMATCH,
        e_MOVE_NOMATCH_HINT,
        e_END };

    bool doneA = false, doneB = false, doneC = false, doneD = false;
    for (int rawMode = e_BEGIN; rawMode < e_END; ++rawMode) {
        const Mode mode = static_cast<Mode>(rawMode);

        if (veryVerbose) printf(
                    "Testing 'insert(Pair&&)' without injected exceptions.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);

            ASSERTV(LENGTH <= MAX_LENGTH);

            const TestValues VALUES(SPEC, &sa);
            EXPECTED[0] = '\0';

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            Obj mX(&oa); const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();
                ASSERT((strchr(SPEC, SPEC[tj]) == &SPEC[tj]) == IS_UNIQ);

                // 'hint' should be ignored, so maliciously make it garbage.

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';
                }

                if (veryVerbose) { P_(IS_UNIQ) P(EXPECTED); }

                bslma::Allocator& ba = e_MOVE_MATCH      == mode ||
                                       e_MOVE_MATCH_HINT == mode ? oa : sa;
                bsls::ObjectBuffer<Pair> buffer;
                u::CharToPairConverter<Pair, ALLOC>::createInplace(
                                        buffer.address(), char(SPEC[tj]), &ba);
                bslma::DestructorGuard<Pair> guard(buffer.address());

                pair<Iter, bool> RESULT;
                switch (mode) {
                  case e_MOVE_MATCH:
                  case e_MOVE_NOMATCH: {
                    RESULT = mX.insert(MoveUtil::move(buffer.object()));
                  } break;
                  case e_MOVE_MATCH_HINT:
                  case e_MOVE_NOMATCH_HINT: {
                    RESULT.first = mX.insert(hint,
                                             MoveUtil::move(buffer.object()));
                    RESULT.second = IS_UNIQ;

                    doneA |= MAX_LENGTH == tj + 1;
                  } break;
                  default: {
                    ASSERTV(rawMode, 0 && "invalid mode");
                  }
                }

                if (IS_UNIQ && k_IS_KEY_MOVE_AWARE) {
                    // 'KEY' is a const type, so cannot be moved.

                    ASSERTV(NameOf<KEY>(), MoveState::e_NOT_MOVED ==
                                  bsltf::getMovedFrom(buffer.object().first));
                    ASSERTV(NameOf<KEY>(), MoveState::e_NOT_MOVED ==
                                  bsltf::getMovedInto(RESULT.first->first));
                }

                if (IS_UNIQ && k_IS_VALUE_MOVE_AWARE) {
                    const MoveState::Enum exp =
                                          k_IS_VALUE_WELL_BEHAVED && &oa != &ba
                                          ? MoveState::e_NOT_MOVED
                                          : MoveState::e_MOVED;

                    MoveState::Enum mState =
                                   bsltf::getMovedFrom(buffer.object().second);
                    ASSERTV(NameOf<VALUE>(), exp == mState);
                    mState = bsltf::getMovedInto(RESULT.first->second);
                    ASSERTV(NameOf<VALUE>(), exp == mState);
                }

                ASSERTV(SIZE + IS_UNIQ  == X.size());
                ASSERTV(IS_UNIQ         == RESULT.second);
                ASSERTV(u::eq(VALUES[tj], *RESULT.first));
                ASSERTV(LINE, tj, u::verifySpec(X, EXPECTED));
            }
        }

        if (veryVerbose) printf(
                  "Testing 'insert(TValue&&)' without injected exceptions.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC, &sa);
            EXPECTED[0] = '\0';

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            Obj mX(&oa);  const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();
                ASSERT((strchr(SPEC, SPEC[tj]) == &SPEC[tj]) == IS_UNIQ);

                // 'hint' should be ignored, so maliciously make it garbage.

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';
                }

                if (veryVerbose) { P_(IS_UNIQ) P(EXPECTED); }

                bslma::Allocator& ba = e_MOVE_MATCH      == mode ||
                                       e_MOVE_MATCH_HINT == mode ? oa : sa;
                bsls::ObjectBuffer<TValueType> buffer;
                u::CharToPairConverter<TValueType, ALLOC>::createInplace(
                                        buffer.address(), char(SPEC[tj]), &ba);
                bslma::DestructorGuard<TValueType> guard(buffer.address());

                pair<Iter, bool> RESULT;
                switch (mode) {
                  case e_MOVE_MATCH:
                  case e_MOVE_NOMATCH: {
                    RESULT = mX.insert(MoveUtil::move(buffer.object()));
                  } break;
                  case e_MOVE_MATCH_HINT:
                  case e_MOVE_NOMATCH_HINT: {
                    RESULT.first = mX.insert(hint,
                                             MoveUtil::move(buffer.object()));
                    RESULT.second = IS_UNIQ;

                    doneB |= MAX_LENGTH == tj + 1;
                  } break;
                  default: {
                    ASSERTV(rawMode, 0 && "invalid mode");
                  }
                }

                if (IS_UNIQ && k_IS_KEY_MOVE_AWARE) {
                    // 'KEY' of 'TValueType' is a non-'const' type, so can be
                    // moved.

                    ASSERTV(NameOf<KEY>(), MoveState::e_MOVED ==
                                  bsltf::getMovedFrom(buffer.object().first));
                    ASSERTV(NameOf<KEY>(), MoveState::e_MOVED ==
                                  bsltf::getMovedInto(RESULT.first->first));
                }

                if (IS_UNIQ && k_IS_VALUE_MOVE_AWARE) {
                    const MoveState::Enum exp =
                                          k_IS_VALUE_WELL_BEHAVED && &oa != &ba
                                          ? MoveState::e_NOT_MOVED
                                          : MoveState::e_MOVED;

                    MoveState::Enum mState =
                                   bsltf::getMovedFrom(buffer.object().second);
                    ASSERTV(NameOf<VALUE>(), exp, mState,
                                                      exp == mState && "from");
                    mState = bsltf::getMovedInto(RESULT.first->second);
                    ASSERTV(NameOf<VALUE>(), exp, mState,
                                                      exp == mState && "into");
                }

                ASSERTV(SIZE + IS_UNIQ  == X.size());
                ASSERTV(IS_UNIQ         == RESULT.second);
                ASSERTV(u::eq(VALUES[tj], *RESULT.first));
                ASSERTV(LINE, tj, u::verifySpec(X, EXPECTED));
            }
        }

        if (veryVerbose) printf(
                       "Testing 'insert(Pair&&)' with injected exceptions.\n");
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC, &sa);
            EXPECTED[0] = '\0';

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                // 'hint' should be ignored, so maliciously make it garbage.

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                Obj mY(&sa);    const Obj& Y = gg(&mY, EXPECTED);

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';
                }

                if (veryVerbose) { P_(IS_UNIQ) P(EXPECTED); }

                bslma::Allocator& ba = e_MOVE_MATCH      == mode ||
                                       e_MOVE_MATCH_HINT == mode ? oa : sa;
                bsls::ObjectBuffer<Pair> buffer;
                u::CharToPairConverter<Pair, ALLOC>::createInplace(
                                        buffer.address(), char(SPEC[tj]), &ba);
                bslma::DestructorGuard<Pair> guard(buffer.address());

                // Note that 'bslstl::HashTable::emplaceIfMissing' actually
                // allocates a new node BEFORE it searches to see if the key
                // is already in the table.  Between this and the fact that
                // memory is pooled, it is very difficult to predict anything
                // about the behavior of memory allocation.

                pair<Iter, bool> RESULT;
                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numThrows;
                    ASSERTV(numThrows, Y == X);

                    switch (mode) {
                      case e_MOVE_MATCH:
                      case e_MOVE_NOMATCH: {
                        RESULT = mX.insert(MoveUtil::move(buffer.object()));
                      } break;
                      case e_MOVE_MATCH_HINT:
                      case e_MOVE_NOMATCH_HINT: {
                        RESULT.first = mX.insert(
                                             hint,
                                             MoveUtil::move(buffer.object()));
                        RESULT.second = IS_UNIQ;

                        doneC |= MAX_LENGTH == tj + 1;
                      } break;
                      default: {
                        ASSERTV(rawMode, 0 && "invalid mode");
                      }
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (IS_UNIQ && k_IS_KEY_MOVE_AWARE) {
                    // 'KEY' is a const type, so cannot be moved.

                    ASSERTV(NameOf<KEY>(), MoveState::e_NOT_MOVED ==
                                  bsltf::getMovedFrom(buffer.object().first));
                    ASSERTV(NameOf<KEY>(), MoveState::e_NOT_MOVED ==
                                  bsltf::getMovedInto(RESULT.first->first));
                }

                if (IS_UNIQ && k_IS_VALUE_MOVE_AWARE) {
                    const MoveState::Enum exp =
                                          k_IS_VALUE_WELL_BEHAVED && &oa != &ba
                                          ? MoveState::e_NOT_MOVED
                                          : MoveState::e_MOVED;

                    MoveState::Enum mState =
                                   bsltf::getMovedFrom(buffer.object().second);
                    ASSERTV(NameOf<VALUE>(), exp, mState,
                                                      exp == mState && "from");
                    mState = bsltf::getMovedInto(RESULT.first->second);
                    ASSERTV(NameOf<VALUE>(), exp, mState,
                                                      exp == mState && "into");
                }

                ASSERTV(SPEC, tj, SIZE, !IS_UNIQ         == (X == Y));
                ASSERTV(LINE, tj, SIZE, IS_UNIQ          == RESULT.second);
                ASSERTV(LINE, tj, SIZE, u::eq(VALUES[tj], *(RESULT.first)));
                ASSERTV(LINE, tj, SIZE, u::verifySpec(X, EXPECTED));
            }
        }

        if (veryVerbose) printf(
                     "Testing 'insert(TValue&&)' with injected exceptions.\n");
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC, &sa);
            EXPECTED[0] = '\0';

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa);  const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                // 'hint' should be ignored, so maliciously make it garbage.

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                Obj mY(&sa);    const Obj& Y = gg(&mY, EXPECTED);

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';
                }

                if (veryVerbose) { P_(IS_UNIQ) P(EXPECTED); }

                bslma::Allocator& ba = e_MOVE_MATCH      == mode ||
                                       e_MOVE_MATCH_HINT == mode ? oa : sa;

                // Note that 'bslstl::HashTable::emplaceIfMissing' actually
                // allocates a new node BEFORE it searches to see if the key
                // is already in the table.  Between this and the fact that
                // memory is pooled, it is very difficult to predict anything
                // about the behavior of memory allocation.

                pair<Iter, bool> RESULT;
                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numThrows;

                    bsls::ObjectBuffer<TValueType> buffer;
                    u::CharToPairConverter<TValueType, ALLOC>::createInplace(
                                        buffer.address(), char(SPEC[tj]), &ba);
                    bslma::DestructorGuard<TValueType> guard(buffer.address());

                    switch (mode) {
                      case e_MOVE_MATCH:
                      case e_MOVE_NOMATCH: {
                        RESULT = mX.insert(MoveUtil::move(buffer.object()));
                      } break;
                      case e_MOVE_MATCH_HINT:
                      case e_MOVE_NOMATCH_HINT: {
                        RESULT.first = mX.insert(
                                             hint,
                                             MoveUtil::move(buffer.object()));
                        RESULT.second = IS_UNIQ;

                        doneD |= MAX_LENGTH == tj + 1;
                      } break;
                      default: {
                        ASSERTV(rawMode, 0 && "invalid mode");
                      }
                    }

                    if (IS_UNIQ && k_IS_KEY_MOVE_AWARE) {
                        ASSERTV(NameOf<KEY>(),
                                bsltf::getMovedFrom(buffer.object().first),
                                MoveState::e_MOVED ==
                                   bsltf::getMovedFrom(buffer.object().first));
                        ASSERTV(NameOf<KEY>(),
                                bsltf::getMovedInto(RESULT.first->first),
                                MoveState::e_MOVED ==
                                     bsltf::getMovedInto(RESULT.first->first));
                    }

                    if (IS_UNIQ && k_IS_VALUE_MOVE_AWARE) {
                        const MoveState::Enum exp =
                                          k_IS_VALUE_WELL_BEHAVED && &oa != &ba
                                          ? MoveState::e_NOT_MOVED
                                          : MoveState::e_MOVED;

                        MoveState::Enum mState =
                                   bsltf::getMovedFrom(buffer.object().second);
                        ASSERTV(NameOf<VALUE>(), exp, mState,
                                                      exp == mState && "from");
                        mState = bsltf::getMovedInto(RESULT.first->second);
                        ASSERTV(NameOf<VALUE>(), exp, mState,
                                                      exp == mState && "into");
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(SPEC, tj, SIZE, !IS_UNIQ         == (X == Y));
                ASSERTV(LINE, tj, SIZE, IS_UNIQ          == RESULT.second);
                ASSERTV(LINE, tj, SIZE, u::eq(VALUES[tj], *(RESULT.first)));
                ASSERTV(LINE, tj, SIZE, u::verifySpec(X, EXPECTED));
            }
        }
    }

    ASSERT(0 == da.numAllocations());
    ASSERTV(doneA && doneB && doneC && doneD);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                   testCase28_propagate_on_container_move_assignment_dispatch()
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

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                            testCase28_propagate_on_container_move_assignment()
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

    testCase28_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase28_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == true'\n");

    testCase28_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase28_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase28_noexcept()
    // Verify that noexcept specification of the move assignment operator is
    // correct.
{
    Obj a;
    Obj b;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isNoexcept =
                        bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                        std::is_nothrow_move_assignable<HASH>::value &&
                        std::is_nothrow_move_assignable<EQUAL>::value;
    ASSERT(isNoexcept ==
           BSLS_KEYWORD_NOEXCEPT_OPERATOR(a = MoveUtil::move(b)));
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase28()
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
    //:   that of target and 2) the same allocator as that of the target.
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
    //   Obj& operator=(Obj&&);
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING MOVE-ASSIGNMENT OPERATOR: %s\n"
                        "--------------------------------\n",
                        NameOf<Pair>().name());

    const int              NUM_DATA        = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void)operatorMAg;  // quash potential compiler warning

    const TestValues VALUES;

    int doneA = 0, doneB = 0, doneC = 0;

    // Create first object.
    if (veryVerbose)
        printf("\nTesting move assignment.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec_p;
            const char *const RESULT1 = DATA[ti].d_results_p;
            const size_t      LENGTH1 = strlen(RESULT1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj  mZZ(&scratch);  const Obj&  ZZ = gg(&mZZ, SPEC1);

            const bsls::Types::Int64 BIU = scratch.numBytesInUse();

            if (veryVeryVerbose) { T_ P_(LINE1) P(ZZ) }

            // Create second object.

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec_p;
                const char *const RESULT2 = DATA[tj].d_results_p;
                const size_t      LENGTH2 = strlen(RESULT2);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);

                    Obj *objPtr = new (fa) Obj(&oa);
                    Obj& mX = *objPtr;      const Obj& X  = gg(&mX, SPEC2);

                    Obj *srcPtr = 0;
                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (fa) Obj(&za); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &za;
                      } break;
                      case 'b': {
                        srcPtr = new (fa) Obj(&oa); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }

                    Obj& mZ = *srcPtr;  const Obj& Z = mZ;
                    bslma::TestAllocator& sa = *srcAllocatorPtr;

                    ASSERTV(SPEC1, SPEC2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    const VALUE *firstPtr = Z.empty()
                                    ? 0
                                    : bsls::Util::addressOf(Z.begin()->second);

                    bslma::TestAllocatorMonitor oam(&oa), zam(&za);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.

                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    const bool match =
                       firstPtr == (X.empty()
                                   ? 0
                                   : bsls::Util::addressOf(X.begin()->second));

                    if (&sa == &oa) {
                        ASSERT(Z.empty());

                        // same allocator

                        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                        if (0 == LENGTH2) {
                            // assigning to an empty container

                            ASSERTV(SPEC1, SPEC2, oam.isInUseSame());
                            ASSERTV(SPEC1, SPEC2, BIU, oa.numBytesInUse(),
                                    BIU == oa.numBytesInUse());
                        }

                        // unchanged address of first contained element,
                        // swapped into dest container

                        ASSERTV(match);

                        // nothing from the other allocator

                        ASSERTV(SPEC1, SPEC2, 0 == za.numBlocksTotal());
                    }
                    else {
                        ASSERTV(Z.empty());

                        // first element not in dst container

                        ASSERTV(!firstPtr || !match);

                        // additional memory checks

                        ASSERTV(SPEC1, SPEC2, &sa == &oa,
                                !*SPEC1 || oam.isTotalUp());
                        ASSERTV(SPEC1, SPEC2, 0 == zam.isInUseUp());

                    }

                    if (k_IS_KEY_MOVE_AWARE) {
                        // 'it->first' is 'const', so it can't have been moved
                        // from, so the key must be copy constructed and not
                        // moved.

                        for (CIter it = Z.cbegin(); Z.cend() != it; ++it) {
                            ASSERTV(NameOf<KEY>(), MoveState::e_NOT_MOVED ==
                                               bsltf::getMovedFrom(it->first));
                        }

                        for (CIter it = X.cbegin(); X.cend() != it; ++it) {
                            const bsltf::MoveState::Enum movedInto =
                                                bsltf::getMovedInto(it->first);

                            ASSERTV(NameOf<KEY>(), CONFIG, movedInto,
                                    MoveState::e_NOT_MOVED == movedInto);
                        }
                    }

                    if (k_IS_VALUE_MOVE_AWARE) {
                        const MoveState::Enum exp =
                                          &oa == &sa || k_IS_VALUE_WELL_BEHAVED
                                          ? MoveState::e_NOT_MOVED
                                          : MoveState::e_MOVED;

                        for (CIter it = Z.cbegin(); Z.cend() != it; ++it) {
                            const MoveState::Enum movedFrom =
                                               bsltf::getMovedFrom(it->second);

                            ASSERTV(NameOf<VALUE>(), movedFrom, CONFIG, exp,
                                                             exp == movedFrom);
                        }

                        for (Iter it = mX.begin(); mX.end() != it; ++it) {
                            const MoveState::Enum movedInto =
                                               bsltf::getMovedInto(it->second);

                            ASSERTV(NameOf<VALUE>(), movedInto, CONFIG, exp,
                                                             exp == movedInto);
                        }
                    }

                    // Verify that 'X', 'Z', and 'ZZ' have correct allocator.

                    ASSERTV(SPEC1, SPEC2, &scratch == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      &oa ==  X.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      &sa ==  Z.get_allocator());

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.

                    pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'Z');
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, &oa == &sa, Z.size(), 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    fa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            &sa == &oa || 0 == sa.numBlocksInUse());

                    // Verify subsequent manipulation of target object 'X'.

                    RESULT = primaryManipulator(&mX, 'Z');
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                            LENGTH1 + 1 == X.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    fa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            0 == oa.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());

                    doneA += DEFAULT_MAX_LENGTH == LENGTH1 &&
                                DEFAULT_MAX_LENGTH == LENGTH2 && 'b' == CONFIG;
                }
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

                ASSERTV(SPEC1, ZZ, X, ZZ == X);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
                ASSERTV(SPEC1, ZZ,  X, ZZ == X);
                ASSERTV(SPEC1, mR, &X, mR == &X);

                ASSERTV(SPEC1, &oa == X.get_allocator().mechanism());

                ASSERTV(SPEC1, sam.isTotalSame());
                ASSERTV(SPEC1, oam.isTotalSame());

                doneB += DEFAULT_MAX_LENGTH == LENGTH1;
            }

            // Verify all object memory is released on destruction.

            ASSERTV(SPEC1, 0 == da.numBlocksTotal());
        }
    }

    if (veryVerbose)
        printf("\nTesting move assignment with injected exceptions.\n");
    {
        // Create first object.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec_p;
            const char *const RESULT1 = DATA[ti].d_results_p;
            const size_t      LENGTH1 = strlen(RESULT1);

            if (4 < LENGTH1 && NUM_DATA-1 != ti) {
                continue;
            }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVeryVerbose) { T_ P_(SPEC1) }

            // Create second object.
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec_p;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results_p);

                if (4 < LENGTH2 && NUM_DATA-1 != tj) {
                    continue;
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVeryVerbose) { T_ T_ P_(SPEC2); P(CONFIG); }

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);

                    Obj mX(&oa);          const Obj& X  = gg(&mX,  SPEC2);
                    Obj mXX(&scratch);    const Obj& XX = gg(&mXX, SPEC2);

                    ASSERTV(XX == X);

                    bslma::TestAllocator *srcAllocatorPtr = NULL;
                    switch (CONFIG) {
                      case 'a': {
                        srcAllocatorPtr = &za;
                      } break;
                      case 'b': {
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    bslma::TestAllocator& sa = *srcAllocatorPtr;

                    // By the time any exceptions are throw, 'Z' will be empty.
                    // Note that if &oa == &sa, the assignment will just be a
                    // non-throwing swap.

                    Obj mE(&scratch);    const Obj& E = mE;

                    Obj *pmZ = 0;

                    int numThrows = -1;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numThrows;

                        if (pmZ) {
                            ASSERT(0 < numThrows);

                            fa.deleteObjectRaw(pmZ);
                        }

                        pmZ = new (fa) Obj(&sa);
                        Obj& mZ = *pmZ;    gg(&mZ, SPEC1);

                        ASSERT(ZZ == mZ);
                        ASSERTV(SPEC1, SPEC2, X, mZ,
                                              (X == mZ) == (INDEX1 == INDEX2));

                        u::CompareProctor<Obj> proctorA(mZ, E);
                        u::CompareProctor<Obj> proctorB(XX, X);

                        Obj *mR = &(mX = MoveUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        proctorA.release();
                        proctorB.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    Obj& mZ = *pmZ;    const Obj& Z = mZ;

                    ASSERTV((&oa == &sa), Z, Z.empty());

                    // Verify the value of the object.

                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.

                    pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'z');
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    fa.deleteObject(pmZ);

                    // Verify subsequent manipulation of target object 'X'.

                    RESULT = primaryManipulator(&mX, 'z');
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(), X,
                                                      LENGTH1 + 1 == X.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    doneC += DEFAULT_MAX_LENGTH == LENGTH1 &&
                                DEFAULT_MAX_LENGTH == LENGTH2 && 'b' == CONFIG;
                }
            }
        }
    }

    ASSERTV(0 == da.numAllocations());

    BSLMF_ASSERT(2 == DEFAULT_NUM_MAX_LENGTH);
    ASSERTV(doneA, doneB, doneC, 4 == doneA && 2 == doneB && 1 == doneC);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase27()
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
    //:   newly created object, is performed when a 0 or an allocator that is
    //:   different than that of the original object is explicitly passed to
    //:   the move constructor.
    //:
    //: 6 The original object is always left in a valid state; the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 7 Subsequent changes to, or destruction of, the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 8 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //
    //:10 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator 1) no allocator passed in, 2) a 0 is
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
    //:     changes the value of the original pair object and violates the
    //:     class invariant enforcing uniqueness of keys.                 (C-7)
    //:
    //:   6 Ensure that the new original, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                   (C-3, C-9)
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
    //   Obj(Obj&&);
    //   Obj(Obj&&, const ALLOC&);
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING MOVE CONSTRUCTOR: (%s, %s)\n"
                        "========================\n",
                        NameOf<KEY>().name(), NameOf<VALUE>().name());

    const TestValues VALUES;

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
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);
    const size_t MAX_SPEC_LEN = 17;

    bool doneA = false, doneB = false;
    if (veryVerbose)
        printf("\nTesting both versions of move constructor.\n");
    {
        const HASH  tstHash(7);
        const EQUAL tstEqual(9);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            ASSERTV(LENGTH <= MAX_SPEC_LEN);

            if (veryVerbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object, 'ZZ', with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZZ(0, tstHash, tstEqual, &scratch);
            const Obj& ZZ = gg(&mZZ, SPEC);

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

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Z'.

                Obj mZ(0, tstHash, tstEqual, &sa);
                const Obj& Z = gg(&mZ, SPEC);

                {
                    const HASH& hf   = Z.hash_function();
                    ASSERTV(tstHash.id()  == hf.id()  &&
                                                     HASH().id()  != hf.id());
                    const EQUAL& equ = Z.key_eq();
                    ASSERTV(tstEqual.id() == equ.id() &&
                                                     EQUAL().id() != equ.id());
                }

                const bool empty = 0 == ZZ.size();

                VALUE *firstPtr = Z.empty()
                                ? 0
                                : bsls::Util::addressOf(mZ.begin()->second);

                bslma::TestAllocator& oa  = 'a' == CONFIG || 'c' == CONFIG
                                          ? sa
                                          : 'b' == CONFIG
                                          ? da
                                          : za;

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64  B = oa.numBlocksInUse();

                Obj                  *objPtr;

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(MoveUtil::move(mZ));
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), 0);
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), &sa);
                  } break;
                  case 'd': {
                    objPtr = new (fa) Obj(MoveUtil::move(mZ), &za);
                    doneA |= LENGTH == MAX_SPEC_LEN;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                Obj& mX = *objPtr;      const Obj& X = mX;

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64  A = oa.numBlocksInUse();

                // Verify the value of the object.

                ASSERTV(SPEC, CONFIG, X == ZZ);

                // Verify that the comparator was copied.

                ASSERTV(X.get_allocator().mechanism() == &oa);

                {
                    const HASH& hf   = X.hash_function();
                    ASSERTV(tstHash.id()  == hf.id()  &&
                                                     HASH().id()  != hf.id());
                    const EQUAL& equ = X.key_eq();
                    ASSERTV(tstEqual.id() == equ.id() &&
                                                     EQUAL().id() != equ.id());
                }

                VALUE *resultFirstPtr = X.empty()
                                   ? 0
                                   : bsls::Util::addressOf(mX.begin()->second);

                ASSERTV(SPEC, CONFIG, &sa == &oa, Z, Z.empty());
                if (&sa == &oa) {
                    ASSERTV(SPEC, CONFIG, &sa == &oa, AA == BB && A == B);

                    ASSERT(resultFirstPtr == firstPtr);
                }

                ASSERTV(Z.empty());

                if (k_IS_KEY_MOVE_AWARE) {
                    // 'it->first' is 'const', so it can't have been moved
                    // from, so the key must be copy constructed and not moved.

                    for (CIter it = X.cbegin(); X.cend() != it; ++it) {
                        const bsltf::MoveState::Enum movedInto =
                                            bsltf::getMovedInto(it->first);

                        ASSERTV(NameOf<KEY>(), CONFIG, movedInto,
                                MoveState::e_NOT_MOVED == movedInto);
                    }
                }

                if (k_IS_VALUE_MOVE_AWARE) {
                    const MoveState::Enum exp = &oa == &sa ||
                                                        k_IS_VALUE_WELL_BEHAVED
                                                ? MoveState::e_NOT_MOVED
                                                : MoveState::e_MOVED;

                    for (Iter it = mX.begin(); mX.end() != it; ++it) {
                        const MoveState::Enum movedInto =
                                               bsltf::getMovedInto(it->second);

                        ASSERTV(NameOf<VALUE>(), movedInto, CONFIG, exp,
                                                             exp == movedInto);
                    }
                }

                ASSERTV(u::verifySpec(X, SPEC));

                // Verify that 'X', 'Z', and 'ZZ' have the correct allocator.

                ASSERTV(SPEC, CONFIG, &scratch == ZZ.get_allocator());
                ASSERTV(SPEC, CONFIG,      &sa ==  Z.get_allocator());
                ASSERTV(SPEC, CONFIG,      &oa ==  X.get_allocator());

                // Verify no allocation from the non-object allocator and that
                // object allocator is hooked up.

                ASSERTV(SPEC, CONFIG, &oa == &da || 0 == da.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || empty);

                // Manipulate source object 'Z' to ensure it is in a valid
                // state and is independent of 'X'.

                pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'Z');
                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object 'X'.

                RESULT = primaryManipulator(&mX, 'Z');
                ASSERTV(true == RESULT.second);
                ASSERTV(SPEC, LENGTH + 1 == X.size());
                ASSERTV(SPEC, X != ZZ);

                fa.deleteObject(objPtr);

                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }

    if (veryVerbose)
        printf("\nTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            const bool empty = 0 == LENGTH;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator za("different",    veryVeryVeryVerbose);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);
            Obj mE( &scratch);  const Obj& E  = mE;

            int numThrows = -1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numThrows;

                Obj mZ(&za);         const Obj& Z = gg(&mZ, SPEC);

                // 'Z' will be empty before any throws happen.

                u::CompareProctor<Obj> proctor(E, Z);

                Obj mX(MoveUtil::move(mZ), &oa);  const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }

                ASSERTV(SPEC, X, ZZ, ZZ == X);
                ASSERTV(SPEC, X, ZZ, Z.empty());
                ASSERTV(SPEC, Z.get_allocator() != X.get_allocator());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(SPEC, LENGTH, !PLAT_EXC || !empty == (0 < numThrows));
            doneB |= LENGTH == MAX_SPEC_LEN;
        }
    }
#else
    doneB = true;
#endif  // BDE_BUILD_TARGET_EXC

    ASSERTV(doneA && doneB);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING STANDARD INTERFACE COVERAGE
    //
    // Concerns:
    //: 1 The type provides the full interface defined by the section
    //:   '[map.overview]' in the C++11 standard, with exceptions for methods
    //:   that require C+11 compiler support.
    //
    // Plan:
    //: 1 Invoke each constructor defined by the standard.  (C-1)
    //:
    //: 2 For each method and free function defined in the standard, use the
    //:   respective address of the method to initialize function pointers
    //:   having the appropriate signatures and return type for that method
    //:   according to the standard.  (C-1)
    //
    // Testing:
    //   CONCERN: The type provides the full interface defined by the standard.
    // ------------------------------------------------------------------------

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StlAlloc> SUMap;

    SUMap A((StlAlloc()));

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    TestValues values("ABC", &scratch);
    SUMap B(values.begin(), values.end(), StlAlloc());

    {
        TestValues values("DEF", &scratch);
        SUMap B(values.begin(), values.end(), StlAlloc());
    }

    SUMap C(B);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    SUMap D(MoveUtil::move(SUMap()));
#else
    SUMap dummyD;
    SUMap D(MoveUtil::move(dummyD));
#endif

    SUMap E((StlAlloc()));

    SUMap F(B, StlAlloc());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    SUMap G(MoveUtil::move(SUMap()), StlAlloc());
#else
    SUMap dummyG;
    SUMap G(MoveUtil::move(dummyG), StlAlloc());
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#if !defined(BSLS_PLATFORM_CMP_MSVC) || BSLS_PLATFORM_CMP_VERSION != 1800
    // MSVC cl 18.00 fails to compile for KEY/VALUE int/int or char/char.
# define u_INIT_BRACES {}
    SUMap H(u_INIT_BRACES);
# undef  u_INIT_BRACES
#endif
#endif

    Obj& (Obj::*operatorAg)(const Obj&) = &Obj::operator=;
    (void)operatorAg;  // quash potential compiler warning

    Obj& (Obj::*operatorMAg)(bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void)operatorMAg;  // quash potential compiler warning

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    Obj& (Obj::*operatorILAg)(std::initializer_list<Pair>) = &Obj::operator=;
    (void)operatorILAg;  // quash potential compiler warning
#endif

    ALLOC (Obj::*methodGetAllocator)() const = &Obj::get_allocator;
    (void)methodGetAllocator;

    Iter (Obj::*methodBegin)() = &Obj::begin;
    (void)methodBegin;

    // const_iterator begin() const noexcept;
    CIter (Obj::*methodBeginConst)() const = &Obj::begin;
    (void)methodBeginConst;

    // iterator end() noexcept;
    Iter (Obj::*methodEnd)() = &Obj::end;
    (void)methodEnd;

    // const_iterator end() const noexcept;
    CIter (Obj::*methodEndConst)() const = &Obj::end;
    (void)methodEndConst;

    // const_iterator cbegin() const noexcept;
    CIter (Obj::*methodCbegin)() const = &Obj::cbegin;
    (void)methodCbegin;

    // const_iterator cend() const noexcept;
    CIter (Obj::*methodCend)() const = &Obj::cend;
    (void)methodCend;

    LIter (Obj::*methodLBeginEnd)(size_t) = &Obj::begin;
    (void)methodLBeginEnd;
    methodLBeginEnd = &Obj::end;
    (void)methodLBeginEnd;

    CLIter (Obj::*methodCLBeginEnd)(size_t) const = &Obj::begin;
    (void)methodCLBeginEnd;
    methodCLBeginEnd = &Obj::end;
    (void)methodCLBeginEnd;
    methodCLBeginEnd = &Obj::cbegin;
    (void)methodCLBeginEnd;
    methodCLBeginEnd = &Obj::cend;
    (void)methodCLBeginEnd;

    size_t (Obj::*methodBucket)(const KEY&) const = &Obj::bucket;
    (void)methodBucket;

    size_t (Obj::*methodBucketCount)() const = &Obj::bucket_count;
    (void)methodBucketCount;
    methodBucketCount = &Obj::max_bucket_count;
    (void)methodBucketCount;

    size_t (Obj::*methodBucketSize)(size_t) const = &Obj::bucket_size;
    (void)methodBucketSize;

    bool (Obj::*methodEmpty)() const = &Obj::empty;
    (void)methodEmpty;

    size_t (Obj::*methodSize)() const = &Obj::size;
    (void)methodSize;

    typename Obj::size_type (Obj::*methodMaxSize)() const = &Obj::max_size;
    (void)methodMaxSize;

    VALUE& (Obj::*operatorIdx)(const KEY&) = &Obj::operator[];
    (void)operatorIdx;

    VALUE& (Obj::*operatorMIdx)(bslmf::MovableRef<KEY>) = &Obj::operator[];
    (void)operatorMIdx;

    VALUE& (Obj::*methodAt)(const KEY&) = &Obj::at;
    (void)methodAt;

    const VALUE& (Obj::*methodAtConst)(const KEY&) const = &Obj::at;
    (void)methodAtConst;

    pair<Iter, bool> (Obj::*methodInsert)(const Pair&) = &Obj::insert;
    (void)methodInsert;

#if !defined(BSLS_PLATFORM_CMP_MSVC)
    pair<Iter, bool> (Obj::*methodInsert2)(
                       BSLS_COMPILERFEATURES_FORWARD_REF(Pair)) = &Obj::insert;
    (void)methodInsert2;
#endif

    Iter (Obj::*methodInsert3)(CIter, const Pair&) = &Obj::insert;
    (void)methodInsert3;

#if !defined(BSLS_PLATFORM_CMP_MSVC)
    Iter (Obj::*methodInsert4)(
                CIter, BSLS_COMPILERFEATURES_FORWARD_REF(Pair)) = &Obj::insert;
    (void)methodInsert4;
#endif

    void (Obj::*methodInsert5)(Iter, Iter) = &Obj::insert;
    (void)methodInsert5;

    // C++11 only:
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void (Obj::*methodInsert6)(std::initializer_list<Pair>) = &Obj::insert;
    (void)methodInsert6;
#endif

    Iter (Obj::*methodErase)(CIter) = &Obj::erase;
    (void)methodErase;

    size_t (Obj::*methodErase2)(const KEY&) = &Obj::erase;
    (void)methodErase2;

    Iter (Obj::*methodErase3)(CIter, CIter) = &Obj::erase;
    (void)methodErase3;

    void (Obj::*methodSwap)(Obj&) = &Obj::swap;
    (void)methodSwap;

    void (Obj::*methodClear)() = &Obj::clear;
    (void)methodClear;

    HASH (Obj::*methodHash)() const = &Obj::hash_function;
    (void)methodHash;

    EQUAL (Obj::*methodKeyEq)() const = &Obj::key_eq;
    (void)methodKeyEq;

    float (Obj::*loadFactor)() const = &Obj::load_factor;
    (void)loadFactor;

    void (Obj::*maxLoadFactor)(float) = &Obj::max_load_factor;
    (void)maxLoadFactor;

    float (Obj::*maxLoadFactorConst)() const = &Obj::max_load_factor;
    (void)maxLoadFactorConst;

    void (Obj::*reHash)(size_t) = &Obj::rehash;
    (void)reHash;

    void (Obj::*reserveFn)(size_t) = &Obj::reserve;
    (void)reserveFn;

    Iter (Obj::*methodFind)(const KEY&) = &Obj::find;
    (void)methodFind;

    CIter (Obj::*methodFindConst)(const KEY&) const = &Obj::find;
    (void)methodFindConst;

    typename Obj::size_type (Obj::*methodCount)(const KEY&) const =
                                                                   &Obj::count;
    (void)methodCount;

    pair<Iter, Iter> ( Obj::*methodEqualRange)(const KEY&) = &Obj::equal_range;
    (void)methodEqualRange;

    pair<CIter, CIter> (
            Obj::*methodEqualRangeConst)(const KEY&) const = &Obj::equal_range;
    (void)methodEqualRangeConst;

    {
        using namespace bsl;

        bool (*operatorEq)(const Obj&, const Obj&) = &operator==;
        (void)operatorEq;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        (void) [](const Obj& lhs, const Obj& rhs) -> bool {
            return lhs != rhs;
        };
#else
        bool (*operatorNe)(const Obj&, const Obj&) = &operator!=;
        (void)operatorNe;
#endif

        void (*functionSwap)(Obj&, Obj&) = &swap;
        (void)functionSwap;
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase25()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTOR OF A TEMPLATE WRAPPER CLASS
    //
    // Concerns:
    //: 1 The constructor of a parameterized wrapper around the container
    //:   compiles.
    //
    // Plan:
    //: 1 Invoke each constructor of a class that inherits from the container.
    //:   (C-1)
    //
    // Testing:
    //   CONCERN: Constructor of a template wrapper class compiles.
    // ------------------------------------------------------------------------

    // The following may fail to compile on AIX.

    u::TemplateWrapper<KEY,
                       VALUE,
                       u::DummyHash,
                       u::DummyEqual,
                       u::DummyAllocator<Pair> > obj1;
    (void) obj1;

    // This compiles because the copy constructor doesn't use a default
    // argument.

    u::TemplateWrapper<KEY,
                       VALUE,
                       u::DummyHash,
                       u::DummyEqual,
                       u::DummyAllocator<Pair> > obj2(obj1);
    (void) obj2;

    // This also compiles, most likely because the constructor is
    // parameterized.

    typename Obj::value_type array[1];
    u::TemplateWrapper<KEY,
                       VALUE,
                       u::DummyHash,
                       u::DummyEqual,
                       u::DummyAllocator<Pair> > obj3(array, array);
    (void) obj3;
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase24()
{
    // ------------------------------------------------------------------------
    // TESTING ELEMENTAL ACCESS -- NON-MOVABLE KEY
    //
    // Concerns:
    //: 1 All elemental access methods return the value associated with the
    //:   key.
    //:
    //: 2 'operator[]' and 'at' can be used to set the value.
    //:
    //: 3 'at' throws 'std::out_of_range' exception if the key does not exist.
    //:
    //: 4 'operator[]' creates a default constructed value if the key does not
    //:   exist in the object.
    //:
    //: 5 Any memory allocations come from the object's allocator.
    //:
    //: 6 'operator[]' is exception neutral.
    //
    // Plan:
    //: 1 Using the table-driven technique, for each set of key-value pairs:
    //:
    //:   1 For each key-value pair in the object:
    //:
    //:     1 Verify 'operator[]' and 'at' returns the expected 'VALUE'.
    //:
    //:     2 Verify no memory is allocated.
    //:
    //:     3 Set the value to something different with 'operator[]' and verify
    //:       the value is changed.  Set the value back to its original value.
    //:
    //:     4 Repeat P-1.1.3 with the 'at' method.
    //:
    //:     5 Verify memory use did not increase.
    //:
    //:   2 Verify no memory is allocated from the default allocator.
    //:
    //:   3 Invoke the 'const' version of 'at' method with a key that does not
    //:     exist in the container.
    //:
    //:   4 Verify 'std::out_of_range' is thrown.
    //:
    //:   5 Repeat P-1.3-4 with the non-'const' version of 'at' method.
    //:
    //:   6 Invoke 'operator[]' using the out-of-range key under the presence
    //:     of exception.
    //:
    //:   7 Verify that a default 'VALUE' is created.
    //:
    //:   8 Verify memory usage is as expected.
    //
    // Testing:
    //   VALUE& operator[](const KEY&);
    //   VALUE& operator[](KEY&&);
    //   VALUE& at(const KEY&);
    //   const VALUE& at(const KEY&) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING ELEMENTAL ACCESS -- NON-MOVABLE KEY: %s\n"
                        "-------------------------------------------\n",
                        NameOf<Pair>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    TestValues tv("z");

    const KEY&   ZK = tv[0].first;       // A value not in any spec.
    const VALUE& ZM = tv[0].second;      // A value not in any spec.

    int done = 0, numTests = 0;

    if (veryVerbose) printf("Testing elemental access.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char *const SPEC    = DATA[ti].d_spec_p;
            const size_t      LENGTH  = strlen(DATA[ti].d_results_p);

            const TestValues  VALUES(DATA[ti].d_results_p);

            if (ti && DATA[ti].d_index == DATA[ti - 1].d_index) {
                continue;
            }
            ++numTests;

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj        mX(&oa);
            const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const KEY&   K = VALUES[tj].first;
                const VALUE& M = VALUES[tj].second;

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, M, mX[K],    M == mX[K]);
                ASSERTV(LINE, M, mX.at(K), M == mX.at(K));
                ASSERTV(LINE, M, X.at(K),  M == X.at(K));

                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(LINE, tj, oam.isInUseSame());

                // Verify 'operator[]' and 'at' can be used to set the value.

                u::copyAssignTo(bsls::Util::addressOf(mX[K]), ZM);
                ASSERTV(LINE, ZM == X.find(K)->second);

                u::copyAssignTo(bsls::Util::addressOf(mX[K]), M);
                ASSERTV(LINE,  M == X.find(K)->second);

                u::copyAssignTo(bsls::Util::addressOf(mX.at(K)), ZM);
                ASSERTV(LINE, ZM == X.find(K)->second);

                u::copyAssignTo(bsls::Util::addressOf(mX.at(K)), M);
                ASSERTV(LINE,  M == X.find(K)->second);

                ASSERTV(LINE, tj, oam.isInUseSame());

                done += DEFAULT_MAX_LENGTH == tj + 1;
            }

            ASSERTV(LINE, 0 == da.numBlocksTotal());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj        mY(&scratch);
            const Obj& Y = gg(&mY, SPEC);

            ASSERT(X == Y);

#if defined(BDE_BUILD_TARGET_EXC)
            if (veryVerbose) printf("Test correct exception is thrown.\n");

            // on 'const Obj'
            {
                bool exceptionCaught = false;
                try {
                    u::CompareProctor<Obj> proctor(X, Y);

                    X.at(ZK);
                }
                catch (const std::out_of_range&) {
                    exceptionCaught = true;
                }
                ASSERTV(LINE, true == exceptionCaught);
            }

            // on 'Obj'
            {
                bool exceptionCaught = false;
                try {
                    u::CompareProctor<Obj> proctor(X, Y);

                    mX.at(ZK);
                }
                catch (const std::out_of_range&) {
                    exceptionCaught = true;
                }
                ASSERTV(LINE, true == exceptionCaught);
            }
#endif

            // Constructing default 'VALUE' to compare with.  Note, that we
            // construct default value this way to support some types that do
            // not meet C++ requirement of 'default-insertable'.
            bslma::TestAllocator value("default value", veryVeryVeryVerbose);
            ALLOC                xvalue(&value);

            bsls::ObjectBuffer<VALUE> d;
            bsl::allocator_traits<ALLOC>::construct(xvalue, d.address());
            bslma::DestructorGuard<VALUE> dGuard(d.address());

            const VALUE& D = d.object();

            if (veryVerbose) printf(
                                   "Test 'operator[]' on out-of-range key.\n");
            {
                const size_t SIZE = X.size();

                const bsls::Types::Int64 B = oa.numBlocksInUse();

                VALUE *ret;
                int numThrows = -1;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numThrows;

                    u::CompareProctor<Obj> proctor(X, Y);

                    ret = bsls::Util::addressOf(mX[ZK]);

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 A = oa.numBlocksInUse();

                ASSERTV(&X.find(ZK)->second == ret);
                ASSERTV(LINE, !k_IS_VALUE_DEFAULT_CONSTRUCTIBLE || D == *ret);

                ASSERTV(!k_TYPE_ALLOC || B < A);
                ASSERTV(!PLAT_EXC || !k_TYPE_ALLOC || 0 < numThrows);
                ASSERTV(LINE, SIZE, SIZE + 1 == X.size());
            }

            ASSERTV(LINE, 0 == da.numBlocksInUse());
        }
    }

    ASSERTV(1 == done && 6 <= numTests);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS AND TYPEDEFS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exist.  (C-1)
    //
    // Testing:
    //   CONCERN: 'unordered_map' has the necessary type traits.
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING TYPE TRAITS: %s\n"
                        "===================\n",
                        NameOf<KEY>().name());

    if (veryVerbose) printf("    Type Traits\n");

    // Verify unordered map defines the expected traits.

    typedef bsl::unordered_map<KEY, VALUE> UMKV;

    BSLMF_ASSERT((1 == bslalg::HasStlIterators<UMKV>::value));

    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<UMKV>::value));

    BSLMF_ASSERT((1 == bslmf::IsBitwiseMoveable<UMKV>::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StlAlloc> ObjStlAlloc;
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<ObjStlAlloc>::value));

    // Verify unordered_map does not define other common traits.

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<UMKV>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<UMKV>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<UMKV>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<UMKV>::value));

    if (veryVerbose) printf("    Typedefs\n");

    typedef pair<const KEY, VALUE> VT;

    BSLMF_ASSERT((bslmf::IsSame<KEY,        typename Obj::key_type>::value));
    BSLMF_ASSERT((bslmf::IsSame<VT,         typename Obj::value_type>::value));
    BSLMF_ASSERT((bslmf::IsSame<VALUE,      typename Obj::mapped_type>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<HASH,       typename Obj::hasher>::value));
    BSLMF_ASSERT((bslmf::IsSame<EQUAL,      typename Obj::key_equal>::value));
    BSLMF_ASSERT((bslmf::IsSame<ALLOC,      typename Obj::allocator_type>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<VT *,       typename Obj::pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<const VT *, typename Obj::const_pointer>::
                                                                       value));
    BSLMF_ASSERT((bslmf::IsSame<VT &,       typename Obj::reference>::value));
    BSLMF_ASSERT((bslmf::IsSame<const VT &, typename Obj::const_reference>::
                                                                       value));

    BSLMF_ASSERT((bslmf::IsSame<typename Obj::pointer,
                                typename ALLOC::pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::const_pointer,
                                typename ALLOC::const_pointer>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::reference,
                                typename ALLOC::reference>::value));
    BSLMF_ASSERT((bslmf::IsSame<typename Obj::const_reference,
                                typename ALLOC::const_reference>::value));
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase22()
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
    //  CONCERN: 'unordered_map' is compatible with standard allocators.
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING STL ALLOCATOR: %s\n"
                        "=====================\n",
                        NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StlAlloc> ObjStlAlloc;

    StlAlloc scratch;

    const char *prevSpec = "woof";
    const char *SPEC;
    for (size_t ti = 0; ti < NUM_DATA; ++ti, prevSpec = SPEC) {
        const int         LINE   = DATA[ti].d_line;
                          SPEC   = DATA[ti].d_results_p;
        const size_t      LENGTH = strlen(DATA[ti].d_results_p);

        if (!strcmp(prevSpec, SPEC)) {
            continue;
        }

        TestValues CONT(SPEC, scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            ObjStlAlloc mX(BEGIN, END);  const ObjStlAlloc& X = mX;

            ASSERTV(LINE, u::verifySpec(X, SPEC));

            ObjStlAlloc mY(X);  const ObjStlAlloc& Y = mY;

            ASSERTV(LINE, u::verifySpec(Y, SPEC));

            ObjStlAlloc mZ;  const ObjStlAlloc& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, u::verifySpec(Z, SPEC));
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, u::verifySpec(X, SPEC));
        }

        CONT.resetIterators();

        {
            ObjStlAlloc mX;  const ObjStlAlloc& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, RESULT.second);
                ASSERTV(LINE, tj, LENGTH, u::eq(CONT[tj], *(RESULT.first)));
            }
            ASSERTV(LINE, u::verifySpec(X, SPEC));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::unordered_map<int,
                                   int,
                                   u::TestHashFunctor<int>,
                                   u::TestEqualityComparator<int>,
                                   StlAlloc> TestObjIntStlAlloc;

        TestObjIntStlAlloc mX;
        mX.insert(pair<int, int>(1, 1));
        TestObjIntStlAlloc mY;
        mY = mX;
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING MAX_SIZE AND EMPTY
    //
    // Concerns:
    //: 1 'max_size' returns the 'max_size' of the supplied allocator.
    //:
    //: 2 'empty' returns 'true' only when the object is empty.
    //
    // Plan:
    //: 1 Run each function and verify the result.  (C-1..2)
    //
    // Testing:
    //   bool empty() const;
    //   size_type max_size() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING MAX_SIZE AND EMPTY: %s\n"
                        "--------------------------\n",
                        NameOf<VALUE>().name());

    bslma::TestAllocator  oa(veryVeryVeryVerbose);

    if (veryVerbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj X;
        ASSERTV(~(size_t)0 / sizeof(typename Obj::value_type) >= X.max_size());
    }

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec_p;     // initial
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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (veryVerbose) printf("\tTesting 'empty'.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE    = DATA[ti].d_lineNum;
            const char   *SPEC    = DATA[ti].d_spec_p;
            const size_t  LENGTH  = strlen(SPEC);

            if (veryVeryVerbose) { T_ P(SPEC); }

            TestValues tv(SPEC);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            ASSERTV(LENGTH == X.size());
            ASSERTV(LINE, SPEC, (0 == LENGTH) == X.empty());
            ASSERTV(LINE, SPEC, (0 == LENGTH) == bsl::empty(X));

            mX.clear();

            ASSERTV(LINE, SPEC, true == X.empty());
            ASSERTV(LINE, SPEC, true == bsl::empty(X));

            gg(&mX, SPEC);

            ASSERTV(X.size() == LENGTH);
            ASSERTV(!LENGTH == X.empty());
            ASSERTV(!LENGTH == bsl::empty(X));

            for (size_t tj = LENGTH; tj > 0; ) {
                ASSERTV(!X.empty());
                ASSERTV(!bsl::empty(X));
                ASSERTV(X.size() == tj);

                ASSERTV(1 == mX.erase(tv[--tj].first));

                ASSERTV(X.size() == tj);
                ASSERTV(!tj == X.empty());
                ASSERTV(!tj == bsl::empty(X));
            }

            ASSERTV(X.empty());
            ASSERTV(bsl::empty(X));
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING SINGLE AND RANGE ERASE
    //
    // Concerns:
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
    //: 2 Repeat P-1 with 'erase(iterator position)'.
    //:
    //: 3 Repeat P-1 with 'erase(const key_type& key)' (C-2).
    //:
    //: 4 For range erase, call erase on all possible range of for each length,
    //:   'l' and verify result is as expected.
    //:
    //: 5 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid values, but not triggered for adjacent valid
    //:   ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //   Iter erase(CIter);
    //   Iter erase(Iter);
    //   size_type erase(const KEY&);
    //   Iter erase(CIter, CIter);
    // -----------------------------------------------------------------------

    if (verbose) printf("TESTING SINGLE AND RANGE ERASE: %s\n"
                        "==============================\n",
                        NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bool doneA = false, doneB = false, doneC = false, doneD = false;

    if (verbose) printf("    Testing 'erase(pos)' on non-empty map.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const VSPEC  = DATA[ti].d_results_p;
            const size_t      LENGTH = strlen(VSPEC);

            const TestValues  VALUES(VSPEC);

            if (ti && !strcmp(DATA[ti-1].d_results_p, VSPEC)) {
                continue;
            }

            if (veryVerbose) { T_ P_(LINE) P_(VSPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                char expSpec[DEFAULT_MAX_LENGTH + 1];
                memcpy(expSpec,      VSPEC,          tj);
                memcpy(expSpec + tj, VSPEC + tj + 1, LENGTH + 1 - (tj + 1));
                ASSERTV(LENGTH - 1 == strlen(expSpec));
                ASSERTV(0 == strchr(expSpec, VSPEC[tj]));

                Obj mXC(&oa);  const Obj& XC = gg(&mXC, VSPEC);
                Obj mX(&oa);   const Obj& X  = gg(&mX,  VSPEC);

                CIter posC   = XC.find(VALUES[tj].first);
                Iter  pos    = mX.find(VALUES[tj].first);

                CIter afterC = posC;
                CIter after  = pos;
                ++afterC;
                ++after;

                CIter beforeC;
                for (CIter next = XC.begin(); posC != next; ++next) {
                    beforeC = next;
                }
                CIter before;
                for (CIter next = X.begin(); pos != next; ++next) {
                    before = next;
                }

                ASSERTV(LINE, tj, posC    != XC.end());
                ASSERTV(LINE, tj, pos     != X.end() );
                ASSERTV(LINE, tj, afterC  != posC    );
                ASSERTV(LINE, tj, after   != pos     );
                ASSERTV(LINE, tj, beforeC != posC    );
                ASSERTV(LINE, tj, before  != pos     );

                if (veryVeryVerbose) { P_(expSpec); P_(*posC); P(*pos); }

                bslma::TestAllocatorMonitor oam(&oa);

                const Iter retC = mXC.erase(posC);
                const Iter ret  = mX.erase(pos);

                if (veryVeryVerbose) { T_ T_ P_(XC); P(X);}

                // Check return value

                ASSERTV(LINE, tj, afterC == retC);
                ASSERTV(LINE, tj, after  == ret );

                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(!k_TYPE_ALLOC || oam.isInUseDown());

                // Check the element does not exist

                ASSERTV(LINE, tj, XC.end() == XC.find(VALUES[tj].first));
                ASSERTV(LINE, tj, X.end()  == X.find(VALUES[tj].first));
                if (XC.end() != beforeC) {
                    ASSERTV(++beforeC == afterC);
                }
                if (X.end() != before) {
                    ASSERTV(++before == after);
                }

                ASSERTV(LENGTH - 1 == XC.size());
                ASSERTV(LENGTH - 1 == X.size());

                ASSERTV(VSPEC, expSpec, u::verifySpec(XC, expSpec));
                ASSERTV(VSPEC, expSpec, u::verifySpec(X,  expSpec));

                doneA |= DEFAULT_MAX_LENGTH == tj + 1;
            }
        }
    }
    if (verbose) printf("    Testing 'erase(key)' on non-empty map.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const VSPEC  = DATA[ti].d_results_p;
            const size_t      LENGTH = strlen(VSPEC);

            const TestValues  VALUES(VSPEC);

            if (ti && !strcmp(DATA[ti-1].d_results_p, VSPEC)) {
                continue;
            }

            if (veryVerbose) { T_ P_(LINE) P_(VSPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, VSPEC);

                char expSpec[DEFAULT_MAX_LENGTH + 1];
                memcpy(expSpec,      VSPEC,          tj);
                memcpy(expSpec + tj, VSPEC + tj + 1, LENGTH + 1 - (tj + 1));
                ASSERTV(LENGTH - 1 == strlen(expSpec));
                ASSERTV(0 == strchr(expSpec, VSPEC[tj]));

                CIter pos   = X.find(VALUES[tj].first);
                CIter after = pos;
                ++after;
                CIter before = X.end();
                for (CIter next = X.begin(); pos != next; ++next) {
                    before = next;
                }

                ASSERTV(LINE, tj, pos != X.end());
                ASSERTV(LINE, tj, after  != pos);
                ASSERTV(LINE, tj, before != pos);

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, tj, 1 == mX.erase(VALUES[tj].first));

                // Check the element does not exist

                ASSERTV(LINE, tj, X.end() == X.find(VALUES[tj].first));
                if (X.end() == before) {
                    ASSERTV(LINE, tj, X.begin() == after);
                }
                else {
                    ASSERTV(LINE, tj, ++before == after);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(!k_TYPE_ALLOC || oam.isInUseDown());
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());
                ASSERTV(VSPEC, expSpec, u::verifySpec(X, expSpec));

                if (veryVeryVerbose) {
                   T_ P(X);
                }

                // Erase a non-existing element.

                ASSERTV(LINE, tj, 0 == mX.erase(VALUES[tj].first));
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());
                ASSERTV(VSPEC, expSpec, u::verifySpec(X, expSpec));

                doneB |= DEFAULT_MAX_LENGTH == tj + 1;
            }
        }
    }

    if (verbose) printf("    Testing 'erase(first, last)'.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const VSPEC  = DATA[ti].d_results_p;
            const size_t      LENGTH = strlen(VSPEC);

            const TestValues  VALUES(VSPEC);

            if (ti && !strcmp(DATA[ti-1].d_results_p, VSPEC)) {
                continue;
            }

            if (veryVerbose) { T_ P_(LINE) P_(VSPEC) P(LENGTH); }

            for (size_t tj = 0; tj <= LENGTH; ++tj) {
                for (size_t tk = tj; tk <= LENGTH; ++tk) {
                    const size_t NUM_ELEMENTS = tk - tj;

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    char expSpec[DEFAULT_MAX_LENGTH + 1];
                    strcpy(expSpec, VSPEC);

                    Obj mX(&oa);  const Obj& X = gg(&mX, VSPEC);

                    const CIter end = X.end();
                    CIter first  = X.begin();
                    CIter before = end;                 // one before 'last'
                    for (size_t ii = 0; ii < tj; ++ii) {
                        ASSERTV(end != first);
                        before = first;
                        ++first;
                    }
                    CIter last = first;                 // after last element
                                                        // in range
                    for (size_t ii = 0; ii < NUM_ELEMENTS; ++ii) {
                        ASSERTV(end != last);
                        u::deleteFromSpec(expSpec,
                                          static_cast<char>(u::idOf(*last)));
                        ++last;
                    }

                    const CIter after = last;

                    bslma::TestAllocatorMonitor oam(&oa);

                    const Iter ret = mX.erase(first, last);
                    ASSERTV(LINE, tj, after == ret);  // Check return value.

                    if (veryVeryVerbose) {
                        T_ P_(LENGTH); P_(tj); P_(tk); P(X);
                    }

                    // Check the element does not exist.

                    ASSERTV(LINE, tj, tk,
                             (end == before ? X.cbegin() : ++before) == after);
                    ASSERTV(LINE, tj, oam.isTotalSame());
                    ASSERTV(0 < NUM_ELEMENTS || oam.isInUseSame());
                    ASSERTV(!k_TYPE_ALLOC || 0 == NUM_ELEMENTS ||
                                                            oam.isInUseDown());
                    ASSERTV(LINE, tj, tk, LENGTH,
                            LENGTH == X.size() + NUM_ELEMENTS);
                    ASSERTV(VSPEC, expSpec, u::verifySpec(X, expSpec));

                    doneC |= DEFAULT_MAX_LENGTH == tj &&
                             DEFAULT_MAX_LENGTH == tk;
                }
            }
        }
    }

    if (verbose) printf("    Negative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj mXC;
        Obj mX;
        pair<Iter, bool> RESULTC = primaryManipulator(&mXC, 'A');
        pair<Iter, bool> RESULT  = primaryManipulator(&mX,  'A');

        ASSERTV(RESULTC.second);
        ASSERTV(RESULT.second);

        CIter iterC = mXC.end();
        Iter  iter  = mX.end();

        ASSERT_SAFE_FAIL(mXC.erase(iterC));
        ASSERT_SAFE_FAIL(mX.erase(iter));
        ASSERT_SAFE_PASS(mXC.erase(RESULTC.first));
        ASSERT_SAFE_PASS(mX.erase(RESULT.first));

        doneD = true;
    }

    ASSERTV(doneA && doneB && doneC && doneD);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE 'insert'
    //
    // Concerns:
    //: 1 All values within the range [first, last) are inserted.
    //:
    //: 2 Each iterator is dereferenced only once.
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
    //:   1 Specify a map of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value map, 'V') in the
    //:   table described in P-1:
    //:
    //:   1 Use the range constructor to create a object with part of the
    //:     elements in 'V'.
    //:
    //:   2 Insert the rest of 'V' under the presence of exceptions.  (C-7)
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
    //   void insert(ITER, ITER);
    // ------------------------------------------------------------------------
    if (verbose) printf("TESTING RANGE 'insert': %s\n"
                        "----------------------\n",
                        NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec_p;
        const char *const EXP    = DATA[ti].d_results_p;
        const size_t      LENGTH = strlen(EXP);

        TestValues CONT(SPEC);

        for (size_t tj = 0; tj <= CONT.size(); ++tj) {
            CONT.resetIterators();

            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator MID   = CONT.index(tj);
            typename TestValues::iterator END   = CONT.end();

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, &oa);  const Obj& X = mX;

            const Int64 A = oa.numAllocations();

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                CONT.resetIterators();

                mX.insert(MID, END);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = oa.numAllocations();

            ASSERTV(LINE, tj < CONT.size() || AA == A);
            ASSERTV(LINE, tj, X.size(), LENGTH, u::verifySpec(X, EXP));
            ASSERTV(LINE, tj, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING 'insert' SINGLE VALUE WITH & WITHOUT HINT
    //
    // Concerns:
    //: 1 'insert' returns a pair containing an iterator and a 'bool'
    //:
    //: 2 The iterator returned refers to the newly inserted element if it did
    //:   not already exists, and to the existing element if it did.
    //:
    //: 3 The 'bool' returned is 'true' if a new element is inserted, and
    //:   'false' otherwise.
    //:
    //: 4 A new element is added to the container if the element did not
    //:   already exist.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 7 If a hint is supplied, it is ignored.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   2 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 2 Repeat P-1 under the presence of injected exceptions.  (C-6)
    //
    // Testing:
    //   pair<Iter, bool> insert(const Pair&);
    //   Iter insert(CIter, const Pair&);
    // -----------------------------------------------------------------------

    if (verbose) printf(
                     "TESTING 'insert' SINGLE VALUE COPY C'TOR: %s\n"
                     "----------------------------------------\n",
                     NameOf<KEY>().name());

    bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    static const struct {
        int         d_line;      // source line number
        const char *d_spec_p;    // specification string
        const char *d_unique_p;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  ----           --------

        { L_,   "A",           "Y"           },    // All specs consist of
        { L_,   "AAA",         "YNN"         },    // sequential (but possibly
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },    // scrambled and/or
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  },    // redundant) letters
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" }     // beginning with 'A'.
    };
    enum { NUM_DATA = sizeof DATA / sizeof *DATA };

    const int MAX_LENGTH = 10;
    char EXPECTED[MAX_LENGTH + 1];

    enum Mode {
        e_BEGIN,
        e_DEFAULT = e_BEGIN,
        e_HINT,
        e_END };

    bool doneA = false, doneB = false;
    for (int rawMode = e_BEGIN; rawMode < e_END; ++rawMode) {
        const Mode mode = static_cast<Mode>(rawMode);

        if (veryVerbose) printf(
                            "Testing 'insert' without injected exceptions.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            // Note that we are using objects of Obj::value_type to invoke
            // correct 'insert' overload.
            const CTestValues VALUES(SPEC, &sa);
            EXPECTED[0] = '\0';

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            Obj mX(&oa); const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                ASSERT((strchr(SPEC, SPEC[tj]) == &SPEC[tj]) == IS_UNIQ);

                // 'hint' should be ignored, so maliciously make it garbage.
                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';
                }

                if (veryVerbose) { P_(IS_UNIQ) P(EXPECTED); }

                pair<Iter, bool> RESULT;
                switch (mode) {
                  case e_DEFAULT: {
                    RESULT = mX.insert(VALUES[tj]);
                  } break;
                  case e_HINT: {
                    RESULT.first = mX.insert(hint, VALUES[tj]);
                    RESULT.second = IS_UNIQ;

                    doneA |= MAX_LENGTH == tj + 1;
                  } break;
                  default: {
                    ASSERTV(rawMode, 0 && "invalid mode");
                  } return;                                           // RETURN
                }

                ASSERTV(SIZE + IS_UNIQ  == X.size());
                ASSERTV(IS_UNIQ         == RESULT.second);
                ASSERTV(u::eq(VALUES[tj], *RESULT.first));
                ASSERTV(LINE, tj, u::verifySpec(X, EXPECTED));
            }
        }

        if (veryVerbose) printf(
                               "Testing 'insert' with injected exceptions.\n");
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const char *const UNIQUE = DATA[ti].d_unique_p;
            const size_t      LENGTH = strlen(SPEC);

            const CTestValues VALUES(SPEC, &sa);
            EXPECTED[0] = '\0';

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            Obj mX(&oa); const Obj &X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = 'Y' == UNIQUE[tj];
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                // 'hint' should be ignored, so maliciously make it garbage.

                CIter hint;
                memset(static_cast<void *>(&hint),
                       static_cast<char>('a' + ti + tj),
                       sizeof(hint));

                Obj mY(&sa); const Obj& Y = gg(&mY, EXPECTED);

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';
                }

                if (veryVerbose) { P_(IS_UNIQ) P(EXPECTED); }

                // Note that 'bslstl::HashTable::emplaceIfMissing' actually
                // allocates a new node BEFORE it searches to see if the key
                // is already in the table.  Between this and the fact that
                // memory is pooled, it is very difficult to predict anything
                // about the behavior of memory allocation.

                pair<Iter, bool> RESULT;
                int numThrows = -1;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ++numThrows;
                    ASSERTV(numThrows, Y == X);

                    switch (mode) {
                      case e_DEFAULT: {
                        RESULT = mX.insert(VALUES[tj]);
                      } break;
                      case e_HINT: {
                        RESULT.first = mX.insert(hint, VALUES[tj]);
                        RESULT.second = IS_UNIQ;

                        doneB |= MAX_LENGTH == tj + 1;
                      } break;
                      default: {
                        ASSERTV(rawMode, 0 && "invalid mode");
                      }
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(SPEC, tj, SIZE, !IS_UNIQ         == (X == Y));
                ASSERTV(LINE, tj, SIZE, IS_UNIQ          == RESULT.second);
                ASSERTV(LINE, tj, SIZE, u::eq(VALUES[tj], *(RESULT.first)));
                ASSERTV(LINE, tj, SIZE, u::verifySpec(X, EXPECTED));
            }
        }
    }

    ASSERT(0 == da.numAllocations());
    ASSERTV(doneA && doneB);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 'begin' and 'end' return non-mutable iterators.
    //:
    //: 2 The range '[begin(), end())' contains all values inserted into the
    //:   container.
    //:
    //: 3 The range '[rbegin(), rend())' contains all values inserted into the
    //:   container.
    //:
    //: 4 'iterator' is a pointer to 'const KEY'.
    //:
    //: 5 'const_iterator' is a pointer to 'const KEY'.
    //:
    //: 6 'reverse_iterator' and 'const_reverse_iterator' are implemented by
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
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_iterator cbegin() const;
    //   const_iterator cend() const;
    //   local_iterator begin(index);
    //   local_iterator end(index);
    //   const_local_iterator cbegin(index) const;
    //   const_local_iterator cend(index) const;
    //   const_local_iterator begin(index) const;
    //   const_local_iterator end(index) const;
    //   size_t bucket(const KEY&) const;
    //   size_t bucket_count() const;
    //   size_t bucket_size(size_t) const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator sa("scratch", veryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVerbose);

    const char *DATA[] = {
        "",                             // All specs consist of sequential (but
        "A",                            // possibly scrambled) letters starting
        "ABC",                          // from 'A'.
        "ACBD",
        "BCDAE",
        "GFEDCBA",
        "ABCDEFGH",
        "BCDEFGHIA",
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    const int MAX_SPEC_LEN = 9;

    bool done = false;

    BSLMF_ASSERT(1 == (bsl::is_same<typename Iter::pointer,
                                             pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename Iter::reference,
                                             pair<const KEY, VALUE>&>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CIter::pointer,
                                       const pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CIter::reference,
                                       const pair<const KEY, VALUE>&>::value));

    BSLMF_ASSERT(1 == (bsl::is_same<typename LIter::pointer,
                                             pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename LIter::reference,
                                             pair<const KEY, VALUE>&>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CLIter::pointer,
                                       const pair<const KEY, VALUE>*>::value));
    BSLMF_ASSERT(1 == (bsl::is_same<typename CLIter::reference,
                                       const pair<const KEY, VALUE>&>::value));

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char   *SPEC   = DATA[ti];
        const size_t  LENGTH = strlen(SPEC);
        ASSERT(LENGTH <= MAX_SPEC_LEN);

        const size_t DIM = MAX_SPEC_LEN + 1;

        CIter  CITERS[DIM];
        Iter   ITERS[ DIM];
        size_t VSEQ[  DIM];
        size_t ISEQ[  DIM];

        const TestValues VALUES(SPEC);

        Obj mX(&oa);  const Obj& X = mX;

        for (size_t tj = 0; tj < LENGTH; ++tj) {
            pair<Iter, bool> RESULT = primaryManipulator(
                                                     &mX, u::idOf(VALUES[tj]));

            ASSERTV(ti, tj, true == RESULT.second);
            ASSERTV(ti, tj, u::eq(VALUES[tj], *RESULT.first));

            CITERS[tj] = RESULT.first;
            ITERS[ tj] = RESULT.first;
        }
        ASSERTV(ti, LENGTH == X.size());

        // Initialize 'VSEQ'.

        std::fill(VSEQ + 0, VSEQ + DIM, -1);
        size_t ii = 0;
        for (; ii < LENGTH; ++ii) {
            const CIter it = X.find(VALUES[ii].first);

            const CIter *pit = std::find(CITERS + 0, CITERS + LENGTH, it);
            const IntPtr jj = pit - CITERS;
            ASSERTV(static_cast<size_t>(jj) < LENGTH);    // found

            ASSERTV(-1 == (int)VSEQ[jj]);                 // not found before
            VSEQ[jj] = ii;

            // Verify unique

            ASSERTV(std::find(VSEQ + 0, VSEQ + jj, VSEQ[jj]) == VSEQ + jj);
        }

        // Now, for the iterators in 'CITERS' and 'ITERS', the 'i'th iterator
        // in the array is referring to the 'VSEQ[i]'th entry in 'VALUES'.

        // Use 'VSEQ' to initialize 'ISEQ'.

        ii = 0;
        for (CIter it = X.cbegin(); X.cend() != it; ++it, ++ii) {
            const CIter *pit = std::find(CITERS + 0, CITERS + LENGTH, it);
            const IntPtr jj = pit - CITERS;
            ASSERTV(static_cast<size_t>(jj) < LENGTH);    // found

            ISEQ[ii] = VSEQ[jj];

            // Verify unique

            ASSERTV(std::find(ISEQ + 0, ISEQ + ii, ISEQ[ii]) == ISEQ + ii);
        }

        // Now, when traversing iterators from front to back, 'ISEQ[x]' is the
        // index in 'VALUES' of the 'x'th node encountered.

        ASSERTV(mX. begin() ==  X. begin());
        ASSERTV(mX. begin() ==  X.cbegin());
        ASSERTV(mX. begin() == mX.cbegin());
        ASSERTV(mX.cbegin() ==  X. begin());
        ASSERTV(mX.cbegin() ==  X.cbegin());
        ASSERTV( X. begin() ==  X.cbegin());

        ASSERTV(mX. end() ==  X. end());
        ASSERTV(mX. end() ==  X.cend());
        ASSERTV(mX. end() == mX.cend());
        ASSERTV(mX.cend() ==  X. end());
        ASSERTV(mX.cend() ==  X.cend());
        ASSERTV( X. end() ==  X.cend());

        ii = 0;
        for (CIter it = X.cbegin(); X.cend() != it; ++it, ++ii) {
            ASSERTV(u::eq(*it, VALUES[ISEQ[ii]]));
            ASSERTV(it->first == VALUES[ISEQ[ii]].first);
        }
        ASSERTV(LENGTH == ii);
        ii = 0;
        for (CIter it = X.begin(); X.end() != it; ++it, ++ii) {
            ASSERTV(u::eq(*it, VALUES[ISEQ[ii]]));
            ASSERTV(it->first == VALUES[ISEQ[ii]].first);
        }
        ASSERTV(LENGTH == ii);
        ii = 0;
        for (Iter it = mX.begin(); mX.end() != it; ++it, ++ii) {
            ASSERTV(u::eq(*it, VALUES[ISEQ[ii]]));
            ASSERTV(it->first == VALUES[ISEQ[ii]].first);
        }
        ASSERTV(LENGTH == ii);

        ASSERT(mX.cbegin() == X.begin());
        ASSERT(mX.cend()   == X.end());

        // Now traverse all the nodes, and establish that within a bucket,
        // normal iterators and local iterators go in the same direction and
        // order.

        const size_t BC = X.bucket_count();

        bool *visited = static_cast<bool *>(sa.allocate(sizeof(bool) * BC));
        memset(visited, 0, sizeof(bool) * BC);

        size_t iB = BC;
        ii = 0;
        size_t numPop = 0;           // # of populated buckets
        size_t jj = 0, endJJ = 0;    // index within a bucket
        LIter  lit;
        CLIter clit, lend;
        CIter cit = X.cbegin(), cend = X.cend();
        for (Iter it = mX.begin(), end = mX.end(); end != it;
                                      ++it, ++cit, ++lit, ++clit, ++ii, ++jj) {
            ASSERTV(cend != cit);
            ASSERTV(&*cit == &*it);

            const KEY& k = it->first;
            size_t newB = X.bucket(k);
            if (iB != newB) {
                ASSERTV(newB < BC);

                ++numPop;

                ASSERTV(0 == ii || (lend == lit && lend == clit));
                ASSERTV(endJJ == jj);

                ASSERTV(!visited[newB]);
                visited[newB] = true;

                clit  = X.cbegin(     newB);
                lit   = mX.begin(     newB);
                lend  = X.cend(       newB);
                endJJ = X.bucket_size(newB);
                jj    = 0;

                ASSERTV( X.begin(newB) == clit);
                ASSERTV( X.end(  newB) == lend);
                ASSERTV(mX.end(  newB) == lend);

                iB = newB;

                done |= LENGTH == MAX_SPEC_LEN;
            }
            else {
                ASSERTV(0 < ii && ii < LENGTH);
                ASSERTV(0 < jj && jj < endJJ);
            }

            ASSERTV(lend  != lit);

            ASSERTV(u::eq(*it, VALUES[ISEQ[ii]]));

            // Test 'operator*' on local iterators

            ASSERTV(&*it  == &*lit);
            ASSERTV(&*cit == &*clit);

            // Test 'operator->' on local iterators

            ASSERTV(it->first == lit->first);
            ASSERTV(it->first == clit->first);
        }
        ASSERTV(X.cend() == cit);
        ASSERTV(X.empty() || lend == lit);
        ASSERTV(endJJ == jj);
        ASSERTV(LENGTH == ii);

        // Now traverse with local iterators

        ii = 0;
        for (size_t idx = 0; idx < BC; ++idx) {
            ASSERTV(mX. begin(idx) ==  X. begin(idx));
            ASSERTV(mX. begin(idx) ==  X.cbegin(idx));
            ASSERTV(mX. begin(idx) == mX.cbegin(idx));
            ASSERTV(mX.cbegin(idx) ==  X. begin(idx));
            ASSERTV(mX.cbegin(idx) ==  X.cbegin(idx));
            ASSERTV( X. begin(idx) ==  X.cbegin(idx));

            ASSERTV(mX. end(idx) ==  X. end(idx));
            ASSERTV(mX. end(idx) ==  X.cend(idx));
            ASSERTV(mX. end(idx) == mX.cend(idx));
            ASSERTV(mX.cend(idx) ==  X. end(idx));
            ASSERTV(mX.cend(idx) ==  X.cend(idx));
            ASSERTV( X. end(idx) ==  X.cend(idx));

            ASSERTV((0 == X.bucket_size(idx)) == (X.begin(idx) == X.end(idx)));

            const CLIter end = X.cend(idx);

            const size_t bs = X.bucket_size(idx);;
            const size_t fin = ii + bs;
            numPop -= 0 < bs;
            ASSERT((0 < bs) == visited[idx]);
            ASSERT((0 == bs) == (X.cbegin(idx) == end));

            size_t jj = ii;
            for (LIter it = mX.begin(idx); end != it; ++it) {
                ASSERT(jj < LENGTH);
                ++jj;
            }
            ASSERTV(fin == jj);

            jj = ii;
            for (CLIter it = X.cbegin(idx); end != it; ++it) {
                ASSERT(jj < LENGTH);
                ++jj;
            }
            ASSERTV(fin == jj);

            ii = fin;
        }
        ASSERTV(LENGTH == ii);
        ASSERTV(0 == numPop);

        sa.deleteObject(visited);
    }

    ASSERT(done);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING SEARCH FUNCTIONS
    //
    // Concerns:
    //: 1 If the key being searched exists in the container, 'find' returns the
    //:   iterator referring to the existing element and 'contains' returns
    //:   'true'.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator and 'contains' returns 'false'.
    //:
    //: 3 'equal_range(key)' returns 'std::make_pair(fird(key), ++find(key))'.
    //:
    //: 2 'count' returns the number of elements with the same value as defined
    //:   by the comparator.
    //:
    //: 3 Both the 'const' and non-'const' versions returns the same value.
    //:
    //: 4 No memory is allocated.
    //
    // Plan:
    //: 1 Use a loop-based approach for different lengths:
    //:
    //:   1 Create an object for each length using values where every
    //:     consecutive values have at least 1 value that is between those two
    //:     values.
    //:
    //:   2 Use run all search functions on all values in the container and
    //:     values between each consecutive values in the container.
    //:
    //:   3 Verify expected result is returned.  (C-1..3)
    //:
    //:   4 Verify no memory is allocated from any allocators.  (C-4)
    //
    // Testing:
    //   bool contains(const key_type& key);
    //   bool contains(const LOOKUP_KEY& key);
    //   iterator find(const key_type& key);
    //   const_iterator find(const key_type& key) const;
    //   size_type count(const key_type& key) const;
    //   pair<iterator, iterator> equal_range(const key_type& key);
    //   pair<const_iter, const_iter> equal_range(const key_type&) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("TESTING SEARCH FUNCTIONS: %s\n"
                        "------------------------\n", NameOf<KEY>().name());

    const TestValues VALUES;  // contains 52 distinct increasing values

    const int MAX_LENGTH = 16;

    bool doneA = false, doneB = false, doneC = false;

    {
        for (size_t ti = 0; ti <= MAX_LENGTH; ++ti) {
            const size_t LENGTH = ti;

            CIter CITER[MAX_LENGTH + 1];
            Iter  ITER[ MAX_LENGTH + 1];

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);  const Obj& X = mX;

            for (size_t i = 0; i < LENGTH; ++i) {
                size_t idx = 2 * i + 1;
                pair<Iter, bool> RESULT = primaryManipulator(
                                                    &mX, u::idOf(VALUES[idx]));

                ASSERTV(ti, i, true == RESULT.second);
                ASSERTV(ti, i, u::eq(VALUES[idx], *RESULT.first));

                CITER[i] = RESULT.first;
                ITER[i]  = RESULT.first;

                doneA |= MAX_LENGTH - 1 == i;
            }
            ASSERTV(ti, LENGTH == X.size());

            bslma::TestAllocatorMonitor oam(&oa);

            for (size_t tj = 0; tj < 2 * LENGTH; ++tj) {
                if (1 == tj % 2) {
                    // expect to find

                    const size_t idx = tj / 2;
                    ASSERTV(ti, tj, CITER[idx] ==  X.find(VALUES[tj].first));
                    ASSERTV(ti, tj, ITER[idx]  == mX.find(VALUES[tj].first));

                    bool cShouldBeFound = CITER[idx] != X.end();
                    ASSERTV(
                           ti,
                           tj,
                           cShouldBeFound,
                           cShouldBeFound == X.contains(VALUES[tj].first));

                    bool shouldBeFound  = ITER[idx] != mX.end();
                    ASSERTV(cShouldBeFound,
                            shouldBeFound,
                            cShouldBeFound == shouldBeFound);

                    ASSERTV(
                           ti,
                           tj,
                           shouldBeFound,
                           shouldBeFound == mX.contains(VALUES[tj].first));

                    ASSERTV( X.end() != CITER[idx]);
                    ASSERTV(mX.end() != ITER[ idx]);

                    pair<CIter, CIter> R1 = X.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, CITER[idx] == R1.first);
                    CIter cAfter = R1.first;
                    ++cAfter;
                    ASSERTV(ti, tj, cAfter == R1.second);

                    pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, ITER[idx] == R2.first);
                    Iter mAfter = R2.first;
                    ++mAfter;
                    ASSERTV(ti, tj, mAfter == R2.second);

                    ASSERTV(R1.first  == R2.first);
                    ASSERTV(R1.second == R2.second);

                    ASSERTV(ti, tj, 1 == mX.count(VALUES[tj].first));

                    doneB |= tj >= 2 * MAX_LENGTH - 2;
                }
                else {
                    // don't expect to find

                    ASSERTV(ti, tj,  X.end() ==  X.find(VALUES[tj].first));
                    ASSERTV(ti, tj, mX.end() == mX.find(VALUES[tj].first));

                    pair<CIter, CIter> R1 = X.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, X.end() == R1.first);
                    ASSERTV(ti, tj, X.end() == R1.second);

                    pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj].first);
                    ASSERTV(ti, tj, mX.end() == R2.first);
                    ASSERTV(ti, tj, mX.end() == R2.second);

                    ASSERTV(ti, tj, 0 == mX.count(VALUES[tj].first));

                    doneC |= tj >= 2 * MAX_LENGTH - 2;
                }
            }
            ASSERTV(ti, oam.isTotalSame());
            ASSERTV(ti, da.numAllocations(), 0 == da.numAllocations());
        }
    }

    ASSERTV(doneA && doneB && doneC);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE C'TORS
    //
    // Concern:
    //: 1 That all c'tors taking a range of objects of type 'KEY' function
    //:   correctly.
    //: 2 The allocator, if passed, is correct, if not passed, is the default
    //:   allocator.
    //: 3 The hash function, if passed, is correct, if not passed, is the
    //:   default constructed hash function.
    //: 4 The key comparison operator, if passed, is correct, if not passed, is
    //:   the default constructed comparator.
    //: 5 The state of the 'unordered_map' created correctly represents the
    //:   range passed.
    //
    // Plan:
    //: 1 Create 'tstHash', a hash functor object with state distinguishable
    //:   from a default-constructed 'HASH' object.
    //: 2 Create 'tstEqual', an key equality comparator object with state
    //:   distinguishable from a default-constructed 'EQUAL' object.
    //: 3 Iterate through a list of 'SPEC's, with expected values for size and
    //:   state of the object to be created given the corresponding spec.
    //: 4 Create a 'TestValues' array object based on the given 'SPEC'.
    //: 5 Iterate through all possible constructors that take range iterators.
    //: 6 In an exception block, call the constructor with the full range of
    //:   the 'TestValues' object and the appropriate other args.
    //: 7 Observe that memory allocation was as expected.
    //: 8 Observe that the number of throws was as expected.
    //: 9 Observe that the state of the container is as expected.
    //: 10 Observe that the hash function of the container is as expected.
    //: 11 Observe that the key equality comparator of the container is as
    //:    expected.
    // ------------------------------------------------------------------------

    HASH  tstHash(7);
    EQUAL tstEqual(9);

    ASSERTV(!(HASH()  == tstHash));
    ASSERTV(!(EQUAL() == tstEqual));

    int done = 0;
    for (size_t i = 0; i < DEFAULT_NUM_DATA; ++i) {
        const int     LINE   = DEFAULT_DATA[i].d_line;
        const char   *SPEC   = DEFAULT_DATA[i].d_spec_p;
        const char   *RSPEC  = DEFAULT_DATA[i].d_results_p;
        const size_t  LENGTH = strlen(RSPEC);

        ASSERT(LENGTH <= 17);

        TestValues values(SPEC);

        for (char config = 'a'; config <= 'n'; ++config) {
            const char CONFIG = config;

            bslma::TestAllocator sa("scratch",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator&  oa = (1 == ((CONFIG - 'a') & 1)) ? sa : da;
            bslma::TestAllocator& noa = &oa == &da                  ? sa : da;

            Obj *objPtr;
            size_t numThrows = size_t(0) - 1;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numThrows;
                values.resetIterators();

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj(values.begin(), values.end());
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), &sa);
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0);
                  } break;
                  case 'd': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          &sa);
                  } break;
                  case 'e': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash);
                  } break;
                  case 'f': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, &sa);
                  } break;
                  case 'g': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, tstEqual);
                  } break;
                  case 'h': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 0,
                                          tstHash, tstEqual, &sa);
                  } break;
                  case 'i': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100);
                  } break;
                  case 'j': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          &sa);
                  } break;
                  case 'k': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash);
                  } break;
                  case 'l': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, &sa);
                  } break;
                  case 'm': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, tstEqual);
                  } break;
                  case 'n': {
                    objPtr = new (fa) Obj(values.begin(), values.end(), 100,
                                          tstHash, tstEqual, &sa);
                    done += DEFAULT_MAX_LENGTH == LENGTH;
                  } break;
                  default: {
                    ASSERTV(0 && "unrecognized CONFIG");
                  } return;                                           // RETURN
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Obj& X = *objPtr;

            ASSERTV(0 == noa.numBlocksTotal());
            ASSERTV(X.get_allocator().mechanism() == &oa);

            const bool EXP0 = CONFIG <= 'h' && 0 == LENGTH;
            ASSERTV(LINE, CONFIG, EXP0 == (0 == oa.numBlocksInUse()));
            ASSERTV(LINE, CONFIG, EXP0 == (0 == oa.numBlocksTotal()));

            ASSERTV((PLAT_EXC && !EXP0) == (numThrows > 0));
            ASSERTV(!PLAT_EXC || !k_TYPE_ALLOC || numThrows >= LENGTH);

            ASSERTV(LENGTH == X.size());
            ASSERTV(u::verifySpec(X, RSPEC));

            Obj mY;            const Obj& Y = gg(&mY, SPEC);
            ASSERTV(X == Y);

            ASSERTV((CONFIG >= 'i') == (X.bucket_count() > 100));

            const HASH& EXP_HASH = strchr("efghklmn", CONFIG) ? tstHash
                                                              : HASH();
            ASSERTV(CONFIG, EXP_HASH == X.hash_function());

            const EQUAL& EXP_EQUAL = strchr("ghmn", CONFIG) ? tstEqual
                                                            : EQUAL();
            ASSERTV(CONFIG, EXP_EQUAL == X.key_eq());

            fa.deleteObject(objPtr);
        }
    }

    ASSERTV(DEFAULT_NUM_MAX_LENGTH == done);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::unordered_map' cannot be deduced from the constructor parameters.
    //..
    // unordered_map()
    // explicit unordered_map(size_t, HASH=HASH(), EQUAL=EQUAL(),
    //                                                  ALLOCATOR=ALLOCATOR());
    // unordered_map(size_t, HASH, ALLOCATOR);
    // unordered_map(size_t, ALLOCATOR);
    // explicit unordered_map(ALLOCATOR);
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
        // Test that constructing a 'bsl::unordered_map' from various
        // combinations of arguments deduces the correct type.
        //..
        // unordered_map(const unordered_map&  s)            -> decltype(s)
        // unordered_map(const unordered_map&  s, ALLOCATOR) -> decltype(s)
        // unordered_map(      unordered_map&& s)            -> decltype(s)
        // unordered_map(      unordered_map&& s, ALLOCATOR) -> decltype(s)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::unordered_map<T1, T1> us1;
        bsl::unordered_map         us1a(us1);
        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_map<T1, T1>);

        typedef float T2;
        typedef bsl::allocator<bsl::pair<const T2, T2>> BA2;

        bsl::unordered_map<T2, T2> us2;
        bsl::unordered_map         us2a(us2, BA2{});
        bsl::unordered_map         us2b(us2, a1);
        bsl::unordered_map         us2c(us2, a2);
        bsl::unordered_map         us2d(us2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_map<T2, T2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_map<T2, T2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_map<T2, T2>);
        ASSERT_SAME_TYPE(decltype(us2d), bsl::unordered_map<T2, T2>);

        typedef short T3;
        bsl::unordered_map<T3, T3> us3;
        bsl::unordered_map         us3a(std::move(us3));
        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_map<T3, T3>);

        typedef long double T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>> BA4;

        bsl::unordered_map<T4, T4> us4;
        bsl::unordered_map         us4a(std::move(us4), BA4{});
        bsl::unordered_map         us4b(std::move(us4), a1);
        bsl::unordered_map         us4c(std::move(us4), a2);
        bsl::unordered_map         us4d(std::move(us4), bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4d), bsl::unordered_map<T4, T4>);
    }

    void TestIteratorConstructors ()
        // Test that constructing a 'bsl::unordered_map' from a pair of
        // iterators and various combinations of other arguments deduces the
        // correct type.
        //..
        // unordered_map(Iter, Iter, size_type = N, HASH=HASH(), EQUAL=EQUAL(),
        //                                              ALLOCATOR=ALLOCATOR());
        // unordered_map(Iter, Iter, size_type, HASH, ALLOCATOR);
        // unordered_map(Iter, Iter, size_type, ALLOCATOR);
        // unordered_map(Iter, Iter, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef long                                    T1;
        typedef StupidHash<T1>                          HashT1;
        typedef StupidEqual<T1>                         EqualT1;
        typedef decltype(StupidHashFn<T1>)              HashFnT1;
        typedef decltype(StupidEqualFn<T1>)             EqualFnT1;
        typedef bsl::allocator<bsl::pair<const T1, T1>> BA1;
        typedef std::allocator<bsl::pair<const T1, T1>> SA1;

        bsl::pair<T1, T1>                    *p1b = nullptr;
        bsl::pair<T1, T1>                    *p1e = nullptr;
        bsl::unordered_map<T1, T1>::iterator  i1b;
        bsl::unordered_map<T1, T1>::iterator  i1e;

        bsl::unordered_map us1a(p1b, p1e);
        bsl::unordered_map us1b(i1b, i1e);
        bsl::unordered_map us1c(p1b, p1e, 3);
        bsl::unordered_map us1d(i1b, i1e, 3);

        bsl::unordered_map us1e(p1b, p1e, 3, HashT1{});
        bsl::unordered_map us1f(p1b, p1e, 3, StupidHashFn<T1>);
        bsl::unordered_map us1g(i1b, i1e, 3, HashT1{});
        bsl::unordered_map us1h(i1b, i1e, 3, StupidHashFn<T1>);

        bsl::unordered_map us1i(p1b, p1e, 3, HashT1{}, EqualT1{});
        bsl::unordered_map us1j(i1b, i1e, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_map us1k(p1b, p1e, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_map us1l(i1b, i1e, 3,
                                          StupidHashFn<T1>, StupidEqualFn<T1>);

        bsl::unordered_map us1m(p1b, p1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_map us1n(p1b, p1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_map us1o(p1b, p1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_map us1p(p1b, p1e, 3, HashT1{}, EqualT1{}, SA1{});
        bsl::unordered_map us1q(i1b, i1e, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_map us1r(i1b, i1e, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_map us1s(i1b, i1e, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_map us1t(i1b, i1e, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_map<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1b), bsl::unordered_map<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1c), bsl::unordered_map<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1d), bsl::unordered_map<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1e), bsl::unordered_map<T1, T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1f),
                         bsl::unordered_map<T1, T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1g), bsl::unordered_map<T1, T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1h),
                         bsl::unordered_map<T1, T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1i),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1j),
                         bsl::unordered_map<T1, T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1k),
                         bsl::unordered_map<T1, T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1l),
                         bsl::unordered_map<T1, T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(decltype(us1m),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1n),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1o),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1p),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, SA1>);
        ASSERT_SAME_TYPE(decltype(us1q),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1r),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1s),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1t),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, SA1>);

        typedef double                      T2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        typedef bsl::allocator<bsl::pair<const T2, T2>> BA2;
        typedef std::allocator<bsl::pair<const T2, T2>> SA2;
        bsl::pair<T2, T2>                    *p2b = nullptr;
        bsl::pair<T2, T2>                    *p2e = nullptr;
        bsl::unordered_map<T2, T2>::iterator  i2b;
        bsl::unordered_map<T2, T2>::iterator  i2e;

        bsl::unordered_map us2a(p2b, p2e, 3, HashT2{}, BA2{});
        bsl::unordered_map us2b(p2b, p2e, 3, HashT2{}, a1);
        bsl::unordered_map us2c(p2b, p2e, 3, HashT2{}, a2);
        bsl::unordered_map us2d(p2b, p2e, 3, HashT2{}, SA2{});
        bsl::unordered_map us2e(p2b, p2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_map us2f(p2b, p2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_map us2g(p2b, p2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_map us2h(p2b, p2e, 3, StupidHashFn<T2>, SA2{});
        bsl::unordered_map us2i(i2b, i2e, 3, HashT2{}, BA2{});
        bsl::unordered_map us2j(i2b, i2e, 3, HashT2{}, a1);
        bsl::unordered_map us2k(i2b, i2e, 3, HashT2{}, a2);
        bsl::unordered_map us2l(i2b, i2e, 3, HashT2{}, SA2{});
        bsl::unordered_map us2m(i2b, i2e, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_map us2n(i2b, i2e, 3, StupidHashFn<T2>, a1);
        bsl::unordered_map us2o(i2b, i2e, 3, StupidHashFn<T2>, a2);
        bsl::unordered_map us2p(i2b, i2e, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(
                   decltype(us2d),
                   bsl::unordered_map<T2, T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2e),
                                       bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2f),
                                       bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2g),
                                       bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
               decltype(us2h),
               bsl::unordered_map<T2, T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2i), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2j), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2k), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(
                   decltype(us2l),
                   bsl::unordered_map<T2, T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2m),
                         bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2n),
                         bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2o),
                         bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
               decltype(us2p),
               bsl::unordered_map<T2, T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                                     T3;
        typedef bsl::allocator<bsl::pair<const T3, T3>> BA3;
        typedef std::allocator<bsl::pair<const T3, T3>> SA3;
        bsl::pair<T3, T3>                    *p3b = nullptr;
        bsl::pair<T3, T3>                    *p3e = nullptr;
        bsl::unordered_map<T3, T3>::iterator  i3b;
        bsl::unordered_map<T3, T3>::iterator  i3e;

        bsl::unordered_map us3a(p3b, p3e, 3, BA3{});
        bsl::unordered_map us3b(p3b, p3e, 3, a1);
        bsl::unordered_map us3c(p3b, p3e, 3, a2);
        bsl::unordered_map us3d(p3b, p3e, 3, SA3{});
        bsl::unordered_map us3e(i3b, i3e, 3, BA3{});
        bsl::unordered_map us3f(i3b, i3e, 3, a1);
        bsl::unordered_map us3g(i3b, i3e, 3, a2);
        bsl::unordered_map us3h(i3b, i3e, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3b), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3c), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(
            decltype(us3d),
            bsl::unordered_map<T3, T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);
        ASSERT_SAME_TYPE(decltype(us3e), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3f), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3g), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(
            decltype(us3h),
            bsl::unordered_map<T3, T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);

        typedef char                                    T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>> BA4;
        typedef std::allocator<bsl::pair<const T4, T4>> SA4;
        bsl::pair<T4, T4>                    *p4b = nullptr;
        bsl::pair<T4, T4>                    *p4e = nullptr;
        bsl::unordered_map<T4, T4>::iterator  i4b;
        bsl::unordered_map<T4, T4>::iterator  i4e;

        bsl::unordered_map us4a(p4b, p4e, BA4{});
        bsl::unordered_map us4b(p4b, p4e, a1);
        bsl::unordered_map us4c(p4b, p4e, a2);
        bsl::unordered_map us4d(p4b, p4e, SA4{});
        bsl::unordered_map us4e(i4b, i4e, BA4{});
        bsl::unordered_map us4f(i4b, i4e, a1);
        bsl::unordered_map us4g(i4b, i4e, a2);
        bsl::unordered_map us4h(i4b, i4e, SA4{});

        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(
            decltype(us4d),
            bsl::unordered_map<T4, T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);

        ASSERT_SAME_TYPE(decltype(us4e), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4f), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4g), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(
            decltype(us4h),
            bsl::unordered_map<T4, T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);
    }

    void TestStdInitializerListConstructors ()
        // Test that constructing a 'bsl::unordered_map' from an
        // initializer_list and various combinations of other arguments deduces
        // the correct type.
        //..
        // unordered_map(initializer_list, size_type = N, HASH=HASH(),
        //                               EQUAL=EQUAL(), ALLOCATOR=ALLOCATOR());
        // unordered_map(initializer_list, size_type, HASH, ALLOCATOR);
        // unordered_map(initializer_list, size_type, ALLOCATOR);
        // unordered_map(initializer_list, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef long                                           T1;
        typedef bsl::allocator<bsl::pair<const T1, T1>>        BA1;
        typedef std::allocator<bsl::pair<const T1, T1>>        SA1;
        typedef std::initializer_list<bsl::pair<const T1, T1>> IL1;

        typedef StupidHash<T1>              HashT1;
        typedef StupidEqual<T1>             EqualT1;
        typedef decltype(StupidHashFn<T1>)  HashFnT1;
        typedef decltype(StupidEqualFn<T1>) EqualFnT1;
        IL1 il1 = {{1L, 2L}, {3L, 4L}};

        bsl::unordered_map us1a(il1);
        bsl::unordered_map us1b(il1, 3);
        bsl::unordered_map us1c(il1, 3, HashT1{});
        bsl::unordered_map us1d(il1, 3, StupidHashFn<T1>);
        bsl::unordered_map us1e(il1, 3, HashT1{}, EqualT1{});
        bsl::unordered_map us1f(il1, 3, HashT1{}, StupidEqualFn<T1>);
        bsl::unordered_map us1g(il1, 3, StupidHashFn<T1>, EqualT1{});
        bsl::unordered_map us1h(il1, 3, StupidHashFn<T1>, StupidEqualFn<T1>);

        bsl::unordered_map us1i(il1, 3, HashT1{}, EqualT1{}, BA1{});
        bsl::unordered_map us1j(il1, 3, HashT1{}, EqualT1{}, a1);
        bsl::unordered_map us1k(il1, 3, HashT1{}, EqualT1{}, a2);
        bsl::unordered_map us1l(il1, 3, HashT1{}, EqualT1{}, SA1{});

        ASSERT_SAME_TYPE(decltype(us1a), bsl::unordered_map<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1b), bsl::unordered_map<T1, T1>);
        ASSERT_SAME_TYPE(decltype(us1c), bsl::unordered_map<T1, T1, HashT1>);
        ASSERT_SAME_TYPE(decltype(us1d),
                         bsl::unordered_map<T1, T1, HashFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1e),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1f),
                         bsl::unordered_map<T1, T1, HashT1, EqualFnT1 *>);
        ASSERT_SAME_TYPE(decltype(us1g),
                         bsl::unordered_map<T1, T1, HashFnT1 *, EqualT1>);
        ASSERT_SAME_TYPE(decltype(us1h),
                         bsl::unordered_map<T1, T1, HashFnT1 *, EqualFnT1 *>);

        ASSERT_SAME_TYPE(decltype(us1i),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1j),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1k),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, BA1>);
        ASSERT_SAME_TYPE(decltype(us1l),
                         bsl::unordered_map<T1, T1, HashT1, EqualT1, SA1>);

        typedef double                                         T2;
        typedef bsl::allocator<bsl::pair<const T2, T2>>        BA2;
        typedef std::allocator<bsl::pair<const T2, T2>>        SA2;
        typedef std::initializer_list<bsl::pair<const T2, T2>> IL2;
        typedef StupidHash<T2>              HashT2;
        typedef decltype(StupidHashFn<T2>)  HashFnT2;
        IL2 il2 = {{1.0, 2.0}, {3.0, 4.0}};

        bsl::unordered_map us2a(il2, 3, HashT2{}, BA2{});
        bsl::unordered_map us2b(il2, 3, HashT2{}, a1);
        bsl::unordered_map us2c(il2, 3, HashT2{}, a2);
        bsl::unordered_map us2d(il2, 3, HashT2{}, SA2{});
        bsl::unordered_map us2e(il2, 3, StupidHashFn<T2>, BA2{});
        bsl::unordered_map us2f(il2, 3, StupidHashFn<T2>, a1);
        bsl::unordered_map us2g(il2, 3, StupidHashFn<T2>, a2);
        bsl::unordered_map us2h(il2, 3, StupidHashFn<T2>, SA2{});

        ASSERT_SAME_TYPE(decltype(us2a), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2b), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(decltype(us2c), bsl::unordered_map<T2, T2, HashT2>);
        ASSERT_SAME_TYPE(
                   decltype(us2d),
                   bsl::unordered_map<T2, T2, HashT2, bsl::equal_to<T2>, SA2>);
        ASSERT_SAME_TYPE(decltype(us2e),
                         bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2f),
                         bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(decltype(us2g),
                         bsl::unordered_map<T2, T2, HashFnT2 *>);
        ASSERT_SAME_TYPE(
               decltype(us2h),
               bsl::unordered_map<T2, T2, HashFnT2 *, bsl::equal_to<T2>, SA2>);

        typedef int                                            T3;
        typedef bsl::allocator<bsl::pair<const T3, T3>>        BA3;
        typedef std::allocator<bsl::pair<const T3, T3>>        SA3;
        typedef std::initializer_list<bsl::pair<const T3, T3>> IL3;
        IL3 il3 = {{1, 2}, {3, 4}};

        bsl::unordered_map us3a(il3, 3, BA3{});
        bsl::unordered_map us3b(il3, 3, a1);
        bsl::unordered_map us3c(il3, 3, a2);
        bsl::unordered_map us3d(il3, 3, SA3{});

        ASSERT_SAME_TYPE(decltype(us3a), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3b), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(decltype(us3c), bsl::unordered_map<T3, T3>);
        ASSERT_SAME_TYPE(
            decltype(us3d),
            bsl::unordered_map<T3, T3, bsl::hash<T3>, bsl::equal_to<T3>, SA3>);

        typedef char                                           T4;
        typedef bsl::allocator<bsl::pair<const T4, T4>>        BA4;
        typedef std::allocator<bsl::pair<const T4, T4>>        SA4;
        typedef std::initializer_list<bsl::pair<const T4, T4>> IL4;
        IL4 il4 = {{'1', '2'}, {'3', '4'}};

        bsl::unordered_map us4a(il4, BA4{});
        bsl::unordered_map us4b(il4, a1);
        bsl::unordered_map us4c(il4, a2);
        bsl::unordered_map us4d(il4, SA4{});

        ASSERT_SAME_TYPE(decltype(us4a), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4b), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(decltype(us4c), bsl::unordered_map<T4, T4>);
        ASSERT_SAME_TYPE(
            decltype(us4d),
            bsl::unordered_map<T4, T4, bsl::hash<T4>, bsl::equal_to<T4>, SA4>);
    }

#undef ASSERT_SAME_TYPE
};
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

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
      case 43: {
        // --------------------------------------------------------------------
        // TESTING 'TRY_EMPLACE' AND 'INSERT_OR_ASSIGN'
        //
        // Concerns:
        //: 1 'try_emplace' only adds an entry to the map if the key does not
        //:   already exist, otherwise it does nothing.  Specifically, it does
        //:   not consume the arguments if the key already exists.
        //:
        //: 2 'insert_or_assign' only adds an entry to the map if the key does
        //:   not already exist, and otherwise updates an existing entry.
        //
        // Plan:
        //: 1 Construct a map and call 'try_emplace' with keys that both do
        //:   and do not exist.  Examine the map and the parameters to confirm
        //:   correct behavior. (C-1)
        //:
        //: 2 Construct a map and call 'insert_or_assign' with keys that both
        //:   do and do not exist.  Examine the map to confirm correct
        //:   behavior. (C-2)
        //
        // Testing:
        //   pair<iterator, bool> try_emplace(const key&, Args&&...);
        //   iterator try_emplace(const_iterator, const key&, Args&&...);
        //   pair<iterator, bool> try_emplace(key&&, Args&&...);
        //   iterator try_emplace(const_iterator, key&&, Args&&...);
        //   pair<iterator, bool> insert_or_assign(const key&, OTHER&&);
        //   iterator insert_or_assign(const_iterator, const key&, OTHER&&);
        //   pair<iterator, bool> insert_or_assign(key&&, OTHER&&);
        //   iterator insert_or_assign(const_iterator, key&&, OTHER&&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING 'TRY_EMPLACE' AND 'INSERT_OR_ASSIGN'\n"
                     "============================================\n");
        TestDriver<char, size_t>::testCase43();
        TestDriver<int,  size_t>::testCase43();
        TestDriver<long, size_t>::testCase43();

        // test 'try_emplace' with different numbers of arguments
        {
            typedef bsl::unordered_map<int, IntValue> Map;
            typedef bsl::pair<Map::iterator, bool> PAIR;

            Map  m;
            PAIR p;

            p = m.try_emplace(1);
            ASSERT(p.second);
            ASSERT(1 == p.first->first);
            ASSERT(0 == p.first->second.d_value);

            p = m.try_emplace(2, 3);
            ASSERT(p.second);
            ASSERT(2 == p.first->first);
            ASSERT(3 == p.first->second.d_value);

            p = m.try_emplace(4, 5, 6);
            ASSERT(p.second);
            ASSERT(4 == p.first->first);
            ASSERT(11 == p.first->second.d_value);

            p = m.try_emplace(7, 8, 9, 10);
            ASSERT(p.second);
            ASSERT(7 == p.first->first);
            ASSERT(27 == p.first->second.d_value);
        }
      } break;
      case 42: {
        // --------------------------------------------------------------------
        // TESTING ERASE_IF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTION 'BSL::ERASE_IF'"
                            "\n=====================================\n");

        TestDriver<char, size_t>::testCase42();
        TestDriver<int,  size_t>::testCase42();
        TestDriver<long, size_t>::testCase42();
      } break;
      case 41: {
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
        //: 1 Create an unordered_map by invoking the constructor without
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
        TestDeductionGuides test;
        (void) test; // This variable only exits for ease of IDE navigation
#endif
      } break;
      case 40: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        //: 1 'unordered_map' does not have a transparent set of lookup
        //:   functions if the comparator is not transparent.
        //:
        //: 2 'unordered_map' has a transparent set of lookup functions if the
        //:   comparator is transparent.
        //
        // Plan:
        //: 1 Construct a non-transparent map and call the lookup functions
        //:   with a type that is convertible to the 'value_type'.  There
        //:   should be exactly one conversion per call to a lookup function.
        //:   (C-1)
        //:
        //: 2 Construct a transparent map and call the lookup functions with a
        //:   type that is convertible to the 'value_type'.  There should be no
        //:   conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: 'find'        properly handles transparent comparators.
        //   CONCERN: 'count'       properly handles transparent comparators.
        //   CONCERN: 'equal_range' properly handles transparent comparators.
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::unordered_map<int, int>              NonTransparentMap;
        typedef bsl::unordered_map<int, int,
                                   TransparentHasher, TransparentComparator>
                                                          TransparentMap;
        typedef NonTransparentMap::value_type             Value;

        const int DATA[] = { 0, 1, 2, 3, 4 };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        NonTransparentMap        mXNT;
        const NonTransparentMap& XNT = mXNT;

        for (int i = 0; i < NUM_DATA; ++i) {
            if (veryVeryVeryVerbose) {
                printf("Constructing test data\n");
            }
            const Value VALUE(DATA[i], DATA[i]);
            mXNT.insert(VALUE);
        }

        TransparentMap        mXT(mXNT.begin(), mXNT.end());
        const TransparentMap& XT = mXT;

        ASSERT(NUM_DATA == XNT.size());
        ASSERT(NUM_DATA == XT.size() );

        for (int i = 0; i < NUM_DATA; ++i) {
            const int KEY = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a key of %d\n",
                       KEY);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent map.\n");
            }
            testTransparentComparator( XNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent map.\n");
            }
            testTransparentComparator(mXNT, false, KEY);

            if (veryVerbose) {
                printf("\tTesting const transparent map.\n");
            }
            testTransparentComparator( XT,  true,  KEY);

            if (veryVerbose) {
                printf("\tTesting mutable transparent map.\n");
            }
            testTransparentComparator(mXT,  true,  KEY);
        }
      } break;
      case 39: {
        // --------------------------------------------------------------------
        // SIMPLE MSVC COMPILATION FAILURE
        //
        // Concerns:
        //: 1 Per '{DRQS 126926371}', try building a simple 'int -> int' map.
        //
        // Plan:
        //: 1 Include a test case as reported in that 'DRQS'.
        //
        // Testing:
        //   CONCERN: Simple test case fails to compile on MSVC.
        // --------------------------------------------------------------------

        if (verbose) printf("\nSIMPLE MSVC COMPILATION FAILURE"
                            "\n===============================\n");

        bsl::unordered_map<int, int> mX;
        testCase39(mX);
      } break;
      case 38: {
        // --------------------------------------------------------------------
        // 'erase' overload is deduced correctly
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING ABSENCE OF ERASE AMBIGUITY\n"
                            "==================================\n");

        TestDriver<EraseAmbiguityTestType, int>::testCase38();
      } break;
      case 37: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

        TestDriver<int>::testCase37();

      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING SUPPORT FOR INCOMPLETE TYPES
        //
        // Concerns:
        //: 1 The type can be declared with incomplete types.
        //
        // Plan:
        //: 1 Instantiate a test object that uses incomplete types in the class
        //:   declaration.  (C-1)
        //
        // Testing:
        //   CONCERN: 'unordered_map' supports incomplete types.
        // --------------------------------------------------------------------
        TestIncompleteType x;
        (void) x;
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // GROWING FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Growing Functions\n"
                            "=========================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase35,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType no copy

        TestDriver<TestKeyType, TestValueType>::testCase35();
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING ELEMENTAL ACCESS WITH MOVABLE KEY
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase34,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase34,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase34();
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST FUNCTIONS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase33_inline,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType no copy

        TestDriver<TestKeyType, TestValueType>::testCase33_inline();

        RUN_EACH_TYPE(TestDriver,
                      testCase33_outOfLine,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType no copy

        TestDriver<TestKeyType, TestValueType>::testCase33_outOfLine();
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE WITH HINT
        // --------------------------------------------------------------------

#if 0
        // TC 31 already test single-arg emplace with garbage hint

        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        // Multiple arg with hint.

        RUN_EACH_TYPE(TestDriver,
                      testCase32a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE
        // --------------------------------------------------------------------

        // Single arg, and single arg with garbage hint.

        RUN_EACH_TYPE(TestDriver,
                      testCase31,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType,
                      bsltf::NonOptionalAllocTestType);

        // Multiple arg, no hint.

        RUN_EACH_TYPE(TestDriver,
                      testCase31a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES WITH HINT
        // --------------------------------------------------------------------

        // Testing with a garbage hint (which was ignored) was done in TC 29.

#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
#endif
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES WITH & WITHOUT HINT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase29();

        // Note that since we're calling 'move(value_type)', the 'KEY' field
        // is const, so it must have a copy c'tor for this to work.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TestDriver<signed char,
                   bsltf::MoveOnlyAllocTestType>::testCase29();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase29();
        TestDriver<signed char,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase29();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase29();
#endif
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase28,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // Since 'KEY' is 'const', copy c'tor of 'KEY' must be used to insert
        // elements, so cannot have move-only 'KEY'.

        TestDriver<signed char,
                   bsltf::MoveOnlyAllocTestType>::testCase28();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase28();
        TestDriver<signed char,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase28();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase28();

        TestDriver<TestKeyType, TestValueType>::testCase28();

        // 'propagate_on_container_move_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase28_propagate_on_container_move_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

// TBD get this working?
#if 0
        TestDriver<signed char, bsltf::MoveOnlyAllocTestType>::
                           testCase28_propagate_on_container_move_assignment();

        TestDriver<signed char, bsltf::WellBehavedMoveOnlyAllocTestType)>::
                           testCase28_propagate_on_container_move_assignment();

        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::MoveOnlyAllocTestType>::
                           testCase28_propagate_on_container_move_assignment();

        TestDriver<bsltf::MovableAllocTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::
                           testCase28_propagate_on_container_move_assignment();
#endif

        TestDriver<TestKeyType, TestValueType>::
                           testCase28_propagate_on_container_move_assignment();

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        // Test noexcept
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef u::ThrowingMoveHash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int> Hash;
            typedef u::ThrowingMoveEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef u::ThrowingMoveHash<int> Hash;
            typedef u::ThrowingMoveEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef u::ThrowingMoveHash<int> Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT( std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int> Hash;
            typedef u::ThrowingMoveEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef u::ThrowingMoveHash<int> Hash;
            typedef u::ThrowingMoveEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!std::is_nothrow_move_assignable<Hash>::value);
            ASSERT(!std::is_nothrow_move_assignable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase28_noexcept();
        }
#endif
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTION
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // Since 'KEY' is 'const', copy c'tor of 'KEY' must be used to insert
        // elements, so cannot have move-only 'KEY'.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TestDriver<signed char,
                   bsltf::MoveOnlyAllocTestType>::testCase27();
        TestDriver<bsltf::MovableTestType,
                   bsltf::MoveOnlyAllocTestType>::testCase27();

        TestDriver<signed char,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase27();
        TestDriver<bsltf::MovableTestType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase27();
#endif
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING STANDARD INTERFACE COVERAGE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // 'value_type' must be copy constructible.
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR OF TEMPLATE WRAPPER
        // --------------------------------------------------------------------
        // KEY/VALUE doesn't affect the test.  So run test only for 'int'.

        TestDriver<int, int>::testCase25();
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING ELEMENTAL ACCESS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase24,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // Key type must be copy constructible.
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        TestDriver<signed char,
                   bsltf::MoveOnlyAllocTestType>::testCase24();
        TestDriver<TestKeyType,
                   bsltf::MoveOnlyAllocTestType>::testCase24();

        TestDriver<signed char,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase24();
        TestDriver<TestKeyType,
                   bsltf::WellBehavedMoveOnlyAllocTestType>::testCase24();
#endif

        TestDriver<TestKeyType, TestValueType>::testCase24();
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS AND TYPEDEFS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        //            bsltf::MoveOnlyAllocTestType no copy

        TestDriver<TestKeyType, TestValueType>::testCase23();
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(StdAllocTestDriver,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        //            bsltf::MoveOnlyAllocTestType no copy

        // TestDriver<TestKeyType, TestValueType>::testCase22();
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'EQUAL' and 'HASH'
        // --------------------------------------------------------------------

#if 0
        // The accessors 'key_equal' and 'hash_function' were tested in test
        // case 2.

        RUN_EACH_TYPE(TestDriver,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        TestDriver<TestKeyType, TestValueType>::testCase21();
#endif
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'max_size' and 'empty'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase20();
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING ORDERING CONTAINER COMPARISONS
        // --------------------------------------------------------------------

#if 0
        // Operators such as '<', '<=', '>', etc. exist for 'bsl::map', not for
        // 'bsl::unordered_map'.

        RUN_EACH_TYPE(TestDriver, testCase19, int, char);

        TestDriver<char, int>::testCase19();
#endif
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING ERASE
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase18,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase18();
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RANGE INSERT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType no copy

        TestDriver<TestKeyType, TestValueType>::testCase17();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // -- covered in 15 --
        // --------------------------------------------------------------------
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'insert' SINGLE VALUE AND WITH HINT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        //            bsltf::MoveOnlyAllocTestType no copy

        TestDriver<TestKeyType, TestValueType>::testCase15();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase14();
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'find', 'contains'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase13();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RANGE C'TOR
        // --------------------------------------------------------------------

        if (verbose) printf("Testing RANGE C'TOR\n"
                            "===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType);

        // 'bsltf::MoveOnlyAllocTestType' no copy construct.

        TestDriver<TestKeyType, TestValueType>::testCase12();
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
}  // Breathing test driver

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
