// bslstl_unorderedmap.t.cpp                                          -*-C++-*-
#include <bslstl_unorderedmap.h>

#include <bslstl_hash.h>
#include <bslstl_pair.h>
#include <bslstl_string.h>
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
#include <bslmf_iscopyconstructible.h>
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
//
// element access:
// [24] VALUE& operator[](const KEY&);
// [34] VALUE& operator[](KEY&&);
// [24] VALUE& at(const KEY&);
// [24] const VALUE& at(const KEY&) const;
//
// search:
// [13] bool contains(const key_type& key);
// [13] bool contains(const LOOKUP_KEY& key);
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
// [42] size_t erase_if(Obj&, PREDICATE);
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
// [41] CLASS TEMPLATE ARGUMENT DEDUCTION
// [43] USAGE EXAMPLE
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
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

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

// Define DEFAULT DATA used in multiple test cases.

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

static const char TV_SPEC[] = {
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };

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

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

#if !defined(BSLSTL_UNORDEREDMAP_DO_NOT_TEST_USAGE)
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
    // Work around per internal ticket D36282765
static int my_count_if_equal(bsl::vector<int>::const_iterator begin,
                             bsl::vector<int>::const_iterator end,
                             const int                        value)
{
    int count = 0;
    for (bsl::vector<int>::const_iterator cur = begin; end != cur; ++cur) {
        if (value == *cur) {
            ++count;
        }
    }

    return count;
}
#endif
#endif

///Example 3: Inverse Concordance
///------------------------------
// If one has a concordance for a set of documents (an index of the position of
// every unique word in those documents), then words of interest can be
// efficiently located.  Suppose after locating a word of interest one also
// needs the surrounding words (for context).  Searching in the original
// document requires re-tokenization (time consuming).  Alternatively, one can
// use the concordance to create an inverse concordance to provide a fast
// lookup of the words at given locations in a document and then examine words
// near the word of interest.
//
// First, we define the types required (and convenient aliases) to create an
// unordered map from a word location to the corresponding word.  The "key"
// value will be 'WordLocation', a pair of 'int' values: the first being the
// document code number (arbitrarily assigned), and second the word offset in
// that document (the first word of the document is at offset 0).  The "value"
// of each entry is a 'bsl::string' containing the word at that location.
//..
    typedef bsl::pair<int, int> WordLocation;
        // Document code number ('first') and word offset ('second') in that
        // document specify a word location.  The first word in the document
        // is at word offset 0.
//..
// Notice that the 'WordLocation', the type of the key value, has no natural
// ordering.  The assignment of document codes is arbitrary so there is no
// reason to consider the words on one document to sort below those in any
// another.
//
// Then, since there is no default hash function for the 'WordLocation' type,
// we define one.  The document code and the word offset are individually
// hashed using the default hasher for the 'int' type and those results bitwise
// exclusive OR-ed a combined result.  This trivial combination formula
// suffices for this problem, but is *not* a general solution for combining
// hashes; see {Practical Requirements on 'HASH'}.
//..
    class WordLocationHash
    {
      private:
        WordLocationHash& operator=(const WordLocationHash& rhs);

      public:
        // CREATORS
        //! WordLocationHash() = default;
            // Create a 'WordLocationHash' object.

        //! WordLocationHash(const WordLocationHash& original) = default;
            // Create a 'WordLocationHash' object.  Note that as
            // 'WordLocationHash' is an empty (stateless) type, this operation
            // will have no observable effect.

        //! ~WordLocationHash() = default;
            // Destroy this object.

        // ACCESSORS
        std::size_t operator()(WordLocation x) const
            // Return a hash value computed using the specified 'x'.
        {
            bsl::hash<int> hasher;
            return hasher(x.first) ^ hasher(x.second);
        }
    };
//..
// Notice that many of the required methods of the hash type are compiler
// generated.  (The declaration of those methods are commented out and suffixed
// by an '= default' comment.)
//
// In addition to a hash functor, the unordered map requires an equality
// comparison functor.  In this example, the unordered map uses 'operator=='
// method of 'std::pair' by default.  If the mapped type has no such method, a
// equality comparison functor must be provided explicitly.
//
// Next, we define the type of the unordered map and associated convenience
// aliases:
//..
    typedef bsl::unordered_map<WordLocation, bsl::string, WordLocationHash>
                                                 InverseConcordance;

    typedef InverseConcordance::const_iterator   InverseConcordanceConstItr;
//..

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Gathering Document Statistics
/// - - - - - - - - - - - - - - - - - - - -
// Unordered maps are useful in situations when there is no meaningful way to
// order the key values, when the order of the keys is irrelevant to the
// problem domain (see {Example 3}), and (even if there is a meaningful
// ordering) the value of ordering the results is outweighed by the higher
// performance provided by unordered maps (compared to ordered maps).
//
// Suppose one wished to gather statistics on the words appearing in a large
// set of documents on disk or in a data base.  Gathering those statistics is
// intrusive (as one is competing for access to the documents with the regular
// users) and must be done as quickly as possible.  Moreover, the set of unique
// words appearing in those documents may be high.  The English language has in
// excess of a million words (albeit many appear infrequently), and, if the
// documents contain serial numbers, or Social Security numbers, or chemical
// formulas, etc then the O[log(n)] insertion time of ordered maps may well be
// inadequate.  The unordered map, having an O[1] typical insertion cost, is a
// viable alternative.  In many problem domains, sorting, if needed, can be
// done after the data is gathered.
//
// This example illustrates the use of 'bsl::unordered_map' to gather one
// simple statistic (counts of unique words) on a single document.  To avoid
// irrelevant details of acquiring the data, several modestly sized documents
// are stored in static arrays:
//..
void usage()
{
#if !defined(BSLSTL_UNORDEREDMAP_DO_NOT_TEST_USAGE)
    static char document0[] =
    " IN CONGRESS, July 4, 1776.\n"
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
    " Honor.\n";

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

    static char document2[] =
    "/CHARTER OF FUNDAMENTAL RIGHTS OF THE EUROPEAN UNION\n"
    "/---------------------------------------------------\n"
    " PREAMBLE\n"
    "\n"
    " The peoples of Europe, in creating an ever closer union among them,\n"
    " are resolved to share a peaceful future based on common values.\n"
    "\n"
    " Conscious of its spiritual and moral heritage, the Union is founded\n"
    " on the indivisible, universal values of human dignity, freedom,\n"
    " equality and solidarity; it is based on the principles of democracy\n"
    " and the rule of law.  It places the individual at the heart of its\n"
    " activities, by establishing the citizenship of the Union and by\n"
    " creating an area of freedom, security and justice.\n"
    "\n"
    " The Union contributes to the preservation and to the development of\n"
    " these common values while respecting the diversity of the cultures\n"
    " and traditions of the peoples of Europe as well as the national\n"
    " identities of the Member States and the organisation of their public\n"
    " authorities at national, regional and local levels; it seeks to\n"
    " promote balanced and sustainable development and ensures free\n"
    " movement of persons, goods, services and capital, and the freedom of\n"
    " establishment.\n"
    "\n"
    " To this end, it is necessary to strengthen the protection of\n"
    " fundamental rights in the light of changes in society, social\n"
    " progress and scientific and technological developments by making\n"
    " those rights more visible in a Charter.\n"
    "\n"
    " This Charter reaffirms, with due regard for the powers and tasks of\n"
    " the Community and the Union and the principle of subsidiarity, the\n"
    " rights as they result, in particular, from the constitutional\n"
    " traditions and international obligations common to the Member States,\n"
    " the Treaty on European Union, the Community Treaties, the European\n"
    " Convention for the Protection of Human Rights and Fundamental\n"
    " Freedoms, the Social Charters adopted by the Community and by the\n"
    " Council of Europe and the case-law of the Court of Justice of the\n"
    " European Communities and of the European Court of Human Rights.\n"
    "\n"
    " Enjoyment of these rights entails responsibilities and duties with\n"
    " regard to other persons, to the human community and to future\n"
    " generations.\n"
    "\n"
    " The Union therefore recognises the rights, freedoms and principles\n"
    " set out hereafter.\n"
    "\n"
    "/CHAPTER I\n"
    "/- - - - -\n"
    " DIGNITY\n"
    "\n"
    "/Article 1\n"
    "/  -  -  -\n"
    " Human dignity\n"
    "\n"
    " Human dignity is inviolable.  It must be respected and protected.\n"
    "\n"
    "/Article 2\n"
    "/  -  -  -\n"
    " Right to life\n"
    "\n"
    ": 1 Everyone has the right to life.\n"
    ": 2 No one shall be condemned to the death penalty, or executed.\n"
    "\n"
    "/Article 3\n"
    "/  -  -  -\n"
    " Right to the integrity of the person\n"
    "\n"
    ": 1 Everyone has the right to respect for his or her physical and\n"
    ":   mental integrity.\n"
    ":\n"
    ": 2 In the fields of medicine and biology, the following must be\n"
    ":   respected in particular:\n"
    ":\n"
    ":   o the free and informed consent of the person concerned, according\n"
    ":     to the procedures laid down by law,\n"
    ":\n"
    ":   o the prohibition of eugenic practices, in particular those aiming\n"
    ":     at the selection of persons,\n"
    ":\n"
    ":   o the prohibition on making the human body and its parts as such a\n"
    ":     source of financial gain,\n"
    ":\n"
    ":   o the prohibition of the reproductive cloning of human beings.\n"
    "\n"
    "/Article 4\n"
    "/  -  -  -\n"
    " Prohibition of torture and inhuman or degrading treatment or\n"
    " punishment\n"
    "\n"
    " No one shall be subjected to torture or to inhuman or degrading\n"
    " treatment or punishment.\n"
    "\n"
    "/Article 5\n"
    "/  -  -  -\n"
    " Prohibition of slavery and forced labour\n"
    "\n"
    ": 1 No one shall be held in slavery or servitude.\n"
    ": 2 No one shall be required to perform forced or compulsory labour.\n"
    ": 3 Trafficking in human beings is prohibited.\n"
    "\n"
    "/CHAPTER II\n"
    "/ - - - - -\n"
    " FREEDOMS\n"
    "\n"
    "/Article 6\n"
    "/  -  -  -\n"
    " Right to liberty and security\n"
    "\n"
    " Everyone has the right to liberty and security of person.\n"
    "\n"
    "/Article 7\n"
    "/  -  -  -\n"
    " Respect for private and family life\n"
    "\n"
    " Everyone has the right to respect for his or her private and family\n"
    " life, home and communications.\n"
    "\n"
    "/Article 8\n"
    "/  -  -  -\n"
    " Protection of personal data\n"
    "\n"
    ": 1 Everyone has the right to the protection of personal data\n"
    ":   concerning him or her.\n"
    ":\n"
    ": 2 Such data must be processed fairly for specified purposes and on\n"
    ":   the basis of the consent of the person concerned or some other\n"
    ":   legitimate basis laid down by law.  Everyone has the right of\n"
    ":   access to data which has been collected concerning him or her, and\n"
    ":   the right to have it rectified.\n"
    ":\n"
    ": 3 Compliance with these rules shall be subject to control by an\n"
    ":   independent authority.\n"
    "\n"
    "/Article 9\n"
    "/  -  -  -\n"
    " Right to marry and right to found a family\n"
    "\n"
    " The right to marry and the right to found a family shall be\n"
    " guaranteed in accordance with the national laws governing the\n"
    " exercise of these rights.\n"
    "\n"
    "/Article 10\n"
    "/-  -  -  -\n"
    " Freedom of thought, conscience and religion\n"
    "\n"
    ": 1 Everyone has the right to freedom of thought, conscience and\n"
    ":   religion.  This right includes freedom to change religion or\n"
    ":   belief and freedom, either alone or in community with others and\n"
    ":   in public or in private, to manifest religion or belief, in\n"
    ":   worship, teaching, practice and observance.\n"
    ":\n"
    ": 2 The right to conscientious objection is recognised, in accordance\n"
    ":   with the national laws governing the exercise of this right.\n"
    "\n"
    "/Article 11\n"
    "/-  -  -  -\n"
    " Freedom of expression and information\n"
    "\n"
    ": 1 Everyone has the right to freedom of expression.  This right shall\n"
    ":   include freedom to hold opinions and to receive and impart\n"
    ":   information and ideas without interference by public authority and\n"
    ":   regardless of frontiers.\n"
    ":\n"
    ": 2 The freedom and pluralism of the media shall be respected.\n"
    "\n"
    "/Article 12\n"
    "/-  -  -  -\n"
    " Freedom of assembly and of association\n"
    "\n"
    ": 1 Everyone has the right to freedom of peaceful assembly and to\n"
    ":   freedom of association at all levels, in particular in political,\n"
    ":   trade union and civic matters, which implies the right of everyone\n"
    ":   to form and to join trade unions for the protection of his or her\n"
    ":   interests.\n"
    ":\n"
    ": 2 Political parties at Union level contribute to expressing the\n"
    ":   political will of the citizens of the Union.\n"
    "\n"
    "/Article 13\n"
    "/-  -  -  -\n"
    " Freedom of the arts and sciences\n"
    "\n"
    " The arts and scientific research shall be free of constraint.\n"
    " Academic freedom shall be respected.\n"
    "\n"
    "/Article 14\n"
    "/-  -  -  -\n"
    " Right to education\n"
    "\n"
    ": 1 Everyone has the right to education and to have access to\n"
    ":   vocational and continuing training.\n"
    ":\n"
    ": 2 This right includes the possibility to receive free compulsory\n"
    ":   education.\n"
    ":\n"
    ": 3 The freedom to found educational establishments with due respect\n"
    ":   for democratic principles and the right of parents to ensure the\n"
    ":   education and teaching of their children in conformity with their\n"
    ":   religious, philosophical and pedagogical convictions shall be\n"
    ":   respected, in accordance with the national laws governing the\n"
    ":   exercise of such freedom and right.\n"
    "\n"
    "/Article 15\n"
    "/-  -  -  -\n"
    " Freedom to choose an occupation and right to engage in work\n"
    "\n"
    ": 1 Everyone has the right to engage in work and to pursue a freely\n"
    ":   chosen or accepted occupation.\n"
    ":\n"
    ": 2 Every citizen of the Union has the freedom to seek employment, to\n"
    ":   work, to exercise the right of establishment and to provide\n"
    ":   services in any Member State.\n"
    ":\n"
    ": 3 Nationals of third countries who are authorised to work in the\n"
    ":   territories of the Member States are entitled to working\n"
    ":   conditions equivalent to those of citizens of the Union.\n"
    "\n"
    "/Article 16\n"
    "/-  -  -  -\n"
    " Freedom to conduct a business\n"
    "\n"
    " The freedom to conduct a business in accordance with Community law\n"
    " and national laws and practices is recognised.\n"
    "\n"
    "/Article 17\n"
    "/-  -  -  -\n"
    " Right to property\n"
    "\n"
    ": 1 Everyone has the right to own, use, dispose of and bequeath his or\n"
    ":   her lawfully acquired possessions.  No one may be deprived of his\n"
    ":   or her possessions, except in the public interest and in the cases\n"
    ":   and under the conditions provided for by law, subject to fair\n"
    ":   compensation being paid in good time for their loss.  The use of\n"
    ":   property may be regulated by law in so far as is necessary for the\n"
    ":   general interest.\n"
    ":\n"
    ": 2 Intellectual property shall be protected.\n"
    "\n"
    "/Article 18\n"
    "/-  -  -  -\n"
    " Right to asylum\n"
    "\n"
    " The right to asylum shall be guaranteed with due respect for the\n"
    " rules of the Geneva Convention of 28 July 1951 and the Protocol of 31\n"
    " January 1967 relating to the status of refugees and in accordance\n"
    " with the Treaty establishing the European Community.\n"
    "\n"
    "/Article 19\n"
    "/-  -  -  -\n"
    " Protection in the event of removal, expulsion or extradition\n"
    "\n"
    ": 1 Collective expulsions are prohibited.\n"
    ":\n"
    ": 2 No one may be removed, expelled or extradited to a State where\n"
    ":   there is a serious risk that he or she would be subjected to the\n"
    ":   death penalty, torture or other inhuman or degrading treatment or\n"
    ":   punishment.\n"
    "\n"
    "/CHAPTER III\n"
    "/- - - - - -\n"
    " EQUALITY\n"
    "\n"
    "/Article 20\n"
    "/-  -  -  -\n"
    " Equality before the law\n"
    "\n"
    " Everyone is equal before the law.\n"
    "\n"
    "/Article 21\n"
    "/-  -  -  -\n"
    " Non-discrimination\n"
    "\n"
    ": 1 Any discrimination based on any ground such as sex, race, colour,\n"
    ":   ethnic or social origin, genetic features, language, religion or\n"
    ":   belief, political or any other opinion, membership of a national\n"
    ":   minority, property, birth, disability, age or sexual orientation\n"
    ":   shall be prohibited.\n"
    ":\n"
    ": 2 Within the scope of application of the Treaty establishing the\n"
    ":   European Community and of the Treaty on European Union, and\n"
    ":   without prejudice to the special provisions of those Treaties, any\n"
    ":   discrimination on grounds of nationality shall be prohibited.\n"
    "\n"
    "/Article 22\n"
    "/-  -  -  -\n"
    " Cultural, religious and linguistic diversity\n"
    "\n"
    " The Union shall respect cultural, religious and linguistic diversity.\n"
    "\n"
    "/Article 23\n"
    "/-  -  -  -\n"
    " Equality between men and women\n"
    "\n"
    " Equality between men and women must be ensured in all areas,\n"
    " including employment, work and pay.  The principle of equality shall\n"
    " not prevent the maintenance or adoption of measures providing for\n"
    " specific advantages in favour of the under-represented sex.\n"
    "\n"
    "/Article 24\n"
    "/-  -  -  -\n"
    " The rights of the child\n"
    "\n"
    ": 1 Children shall have the right to such protection and care as is\n"
    ":   necessary for their well-being.  They may express their views\n"
    ":   freely.  Such views shall be taken into consideration on matters\n"
    ":   which concern them in accordance with their age and maturity.\n"
    ":\n"
    ": 2 In all actions relating to children, whether taken by public\n"
    ":   authorities or private institutions, the child's best interests\n"
    ":   must be a primary consideration.\n"
    ":\n"
    ": 3 Every child shall have the right to maintain on a regular basis a\n"
    ":   personal relationship and direct contact with both his or her\n"
    ":   parents, unless that is contrary to his or her interests.\n"
    "\n"
    "/Article 25\n"
    "/-  -  -  -\n"
    " The rights of the elderly\n"
    "\n"
    " The Union recognises and respects the rights of the elderly to lead a\n"
    " life of dignity and independence and to participate in social and\n"
    " cultural life.\n"
    "\n"
    "/Article 26\n"
    "/-  -  -  -\n"
    " Integration of persons with disabilities\n"
    "\n"
    " The Union recognises and respects the right of persons with\n"
    " disabilities to benefit from measures designed to ensure their\n"
    " independence, social and occupational integration and participation\n"
    " in the life of the community.\n"
    "\n"
    "/CHAPTER IV\n"
    "/ - - - - -\n"
    " SOLIDARITY\n"
    "\n"
    "/Article 27\n"
    "/-  -  -  -\n"
    " Workers' right to information and consultation within the undertaking\n"
    "\n"
    " Workers or their representatives must, at the appropriate levels, be\n"
    " guaranteed information and consultation in good time in the cases and\n"
    " under the conditions provided for by Community law and national laws\n"
    " and practices.\n"
    "\n"
    "/Article 28\n"
    "/-  -  -  -\n"
    " Right of collective bargaining and action\n"
    "\n"
    " Workers and employers, or their respective organisations, have, in\n"
    " accordance with Community law and national laws and practices, the\n"
    " right to negotiate and conclude collective agreements at the\n"
    " appropriate levels and, in cases of conflicts of interest, to take\n"
    " collective action to defend their interests, including strike action.\n"
    "\n"
    "/Article 29\n"
    "/-  -  -  -\n"
    " Right of access to placement services\n"
    "\n"
    " Everyone has the right of access to a free placement service.\n"
    "\n"
    "/Article 30\n"
    "/-  -  -  -\n"
    " Protection in the event of unjustified dismissal\n"
    "\n"
    " Every worker has the right to protection against unjustified\n"
    " dismissal, in accordance with Community law and national laws and\n"
    " practices.\n"
    "\n"
    "/Article 31\n"
    "/-  -  -  -\n"
    " Fair and just working conditions\n"
    "\n"
    ": 1 Every worker has the right to working conditions which respect his\n"
    ":   or her health, safety and dignity.\n"
    ":\n"
    ": 2 Every worker has the right to limitation of maximum working hours,\n"
    ":   to daily and weekly rest periods and to an annual period of paid\n"
    ":   leave.\n"
    "\n"
    "/Article 32\n"
    "/-  -  -  -\n"
    " Prohibition of child labour and protection of young people at work\n"
    "\n"
    " The employment of children is prohibited.  The minimum age of\n"
    " admission to employment may not be lower than the minimum\n"
    " school-leaving age, without prejudice to such rules as may be more\n"
    " favourable to young people and except for limited derogations.  Young\n"
    " people admitted to work must have working conditions appropriate to\n"
    " their age and be protected against economic exploitation and any work\n"
    " likely to harm their safety, health or physical, mental, moral or\n"
    " social development or to interfere with their education.\n"
    "\n"
    "/Article 33\n"
    "/-  -  -  -\n"
    " Family and professional life\n"
    "\n"
    ": 1 The family shall enjoy legal, economic and social protection.\n"
    ":\n"
    ": 2 To reconcile family and professional life, everyone shall have the\n"
    ":   right to protection from dismissal for a reason connected with\n"
    ":   maternity and the right to paid maternity leave and to parental\n"
    ":   leave following the birth or adoption of a child.\n"
    "\n"
    "/Article 34\n"
    "/-  -  -  -\n"
    " Social security and social assistance\n"
    "\n"
    ": 1 The Union recognises and respects the entitlement to social\n"
    ":   security benefits and social services providing protection in\n"
    ":   cases such as maternity, illness, industrial accidents, dependency\n"
    ":   or old age, and in the case of loss of employment, in accordance\n"
    ":   with the rules laid down by Community law and national laws and\n"
    ":   practices.\n"
    ":\n"
    ": 2 Everyone residing and moving legally within the European Union is\n"
    ":   entitled to social security benefits and social advantages in\n"
    ":   accordance with Community law and national laws and practices.\n"
    ":\n"
    ": 3 In order to combat social exclusion and poverty, the Union\n"
    ":   recognises and respects the right to social and housing assistance\n"
    ":   so as to ensure a decent existence for all those who lack\n"
    ":   sufficient resources, in accordance with the rules laid down by\n"
    ":   Community law and national laws and practices.\n"
    "\n"
    "/Article 35\n"
    "/-  -  -  -\n"
    " Health care\n"
    "\n"
    " Everyone has the right of access to preventive health care and the\n"
    " right to benefit from medical treatment under the conditions\n"
    " established by national laws and practices.  A high level of human\n"
    " health protection shall be ensured in the definition and\n"
    " implementation of all Union policies and activities.\n"
    "\n"
    "/Article 36\n"
    "/-  -  -  -\n"
    " Access to services of general economic interest\n"
    "\n"
    " The Union recognises and respects access to services of general\n"
    " economic interest as provided for in national laws and practices, in\n"
    " accordance with the Treaty establishing the European Community, in\n"
    " order to promote the social and territorial cohesion of the Union.\n"
    "\n"
    "/Article 37\n"
    "/-  -  -  -\n"
    " Environmental protection\n"
    "\n"
    " A high level of environmental protection and the improvement of the\n"
    " quality of the environment must be integrated into the policies of\n"
    " the Union and ensured in accordance with the principle of sustainable\n"
    " development.\n"
    "\n"
    "/Article 38\n"
    "/-  -  -  -\n"
    " Consumer protection\n"
    "\n"
    " Union policies shall ensure a high level of consumer protection.\n"
    "\n"
    "/CHAPTER V\n"
    "/- - - - -\n"
    " CITIZENS' RIGHTS\n"
    "\n"
    "/Article 39\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at elections to the\n"
    " European Parliament\n"
    "\n"
    ": 1 Every citizen of the Union has the right to vote and to stand as a\n"
    ":   candidate at elections to the European Parliament in the Member\n"
    ":   State in which he or she resides, under the same conditions as\n"
    ":   nationals of that State.\n"
    ":\n"
    ": 2 Members of the European Parliament shall be elected by direct\n"
    ":   universal suffrage in a free and secret ballot.\n"
    "\n"
    "/Article 40\n"
    "/-  -  -  -\n"
    " Right to vote and to stand as a candidate at municipal elections\n"
    "\n"
    " Every citizen of the Union has the right to vote and to stand as a\n"
    " candidate at municipal elections in the Member State in which he or\n"
    " she resides under the same conditions as nationals of that State.\n"
    "\n"
    "/Article 41\n"
    "/-  -  -  -\n"
    " Right to good administration\n"
    "\n"
    ": 1 Every person has the right to have his or her affairs handled\n"
    ":   impartially, fairly and within a reasonable time by the\n"
    ":   institutions and bodies of the Union.\n"
    ":\n"
    ": 2 This right includes:\n"
    ":\n"
    ":   o the right of every person to be heard, before any individual\n"
    ":     measure which would affect him or her adversely is taken;\n"
    ":\n"
    ":   o the right of every person to have access to his or her file,\n"
    ":     while respecting the legitimate interests of confidentiality and\n"
    ":     of professional and business secrecy;\n"
    ":\n"
    ":   o the obligation of the administration to give reasons for its\n"
    ":     decisions.\n"
    ":\n"
    ": 3 Every person has the right to have the Community make good any\n"
    ":   damage caused by its institutions or by its servants in the\n"
    ":   performance of their duties, in accordance with the general\n"
    ":   principles common to the laws of the Member States.\n"
    ":\n"
    ": 4 Every person may write to the institutions of the Union in one of\n"
    ":   the languages of the Treaties and must have an answer in the same\n"
    ":   language.\n"
    "\n"
    "/Article 42\n"
    "/-  -  -  -\n"
    " Right of access to documents\n"
    "\n"
    " Any citizen of the Union, and any natural or legal person residing or\n"
    " having its registered office in a Member State, has a right of access\n"
    " to European Parliament, Council and Commission documents.\n"
    "\n"
    "/Article 43\n"
    "/-  -  -  -\n"
    " Ombudsman\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to refer\n"
    " to the Ombudsman of the Union cases of maladministration in the\n"
    " activities of the Community institutions or bodies, with the\n"
    " exception of the Court of Justice and the Court of First Instance\n"
    " acting in their judicial role.\n"
    "\n"
    "/Article 44\n"
    "/-  -  -  -\n"
    " Right to petition\n"
    "\n"
    " Any citizen of the Union and any natural or legal person residing or\n"
    " having its registered office in a Member State has the right to\n"
    " petition the European Parliament.\n"
    "\n"
    "/Article 45\n"
    "/-  -  -  -\n"
    " Freedom of movement and of residence\n"
    "\n"
    ": 1 Every citizen of the Union has the right to move and reside freely\n"
    ":   within the territory of the Member States.\n"
    ":\n"
    ": 2 Freedom of movement and residence may be granted, in accordance\n"
    ":   with the Treaty establishing the European Community, to nationals\n"
    ":   of third countries legally resident in the territory of a Member\n"
    ":   State.\n"
    "\n"
    "/Article 46\n"
    "/-  -  -  -\n"
    " Diplomatic and consular protection\n"
    "\n"
    " Every citizen of the Union shall, in the territory of a third country\n"
    " in which the Member State of which he or she is a national is not\n"
    " represented, be entitled to protection by the diplomatic or consular\n"
    " authorities of any Member State, on the same conditions as the\n"
    " nationals of that Member State.\n"
    "\n"
    "/CHAPTER VI\n"
    "/ - - - - -\n"
    " JUSTICE\n"
    "\n"
    "/Article 47\n"
    "/-  -  -  -\n"
    " Right to an effective remedy and to a fair trial\n"
    "\n"
    " Everyone whose rights and freedoms guaranteed by the law of the Union\n"
    " are violated has the right to an effective remedy before a tribunal\n"
    " in compliance with the conditions laid down in this Article.\n"
    " Everyone is entitled to a fair and public hearing within a reasonable\n"
    " time by an independent and impartial tribunal previously established\n"
    " by law.  Everyone shall have the possibility of being advised,\n"
    " defended and represented.  Legal aid shall be made available to those\n"
    " who lack sufficient resources in so far as such aid is necessary to\n"
    " ensure effective access to justice.\n"
    "\n"
    "/Article 48\n"
    "/-  -  -  -\n"
    " Presumption of innocence and right of defence\n"
    "\n"
    ": 1 Everyone who has been charged shall be presumed innocent until\n"
    ":   proved guilty according to law.\n"
    ":\n"
    ": 2 Respect for the rights of the defence of anyone who has been\n"
    ":   charged shall be guaranteed.\n"
    "\n"
    "/Article 49\n"
    "/-  -  -  -\n"
    " Principles of legality and proportionality of criminal offences and\n"
    " penalties\n"
    "\n"
    ": 1 No one shall be held guilty of any criminal offence on account of\n"
    ":   any act or omission which did not constitute a criminal offence\n"
    ":   under national law or international law at the time when it was\n"
    ":   committed.  Nor shall a heavier penalty be imposed than that which\n"
    ":   was applicable at the time the criminal offence was committed.\n"
    ":   If, subsequent to the commission of a criminal offence, the law\n"
    ":   provides for a lighter penalty, that penalty shall be applicable.\n"
    ":\n"
    ": 2 This Article shall not prejudice the trial and punishment of any\n"
    ":   person for any act or omission which, at the time when it was\n"
    ":   committed, was criminal according to the general principles\n"
    ":   recognised by the community of nations.\n"
    ":\n"
    ": 3 The severity of penalties must not be disproportionate to the\n"
    ":   criminal offence.\n"
    "\n"
    "/Article 50\n"
    "/-  -  -  -\n"
    " Right not to be tried or punished twice in criminal proceedings for\n"
    " the same criminal offence\n"
    "\n"
    " No one shall be liable to be tried or punished again in criminal\n"
    " proceedings for an offence for which he or she has already been\n"
    " finally acquitted or convicted within the Union in accordance with\n"
    " the law.\n"
    "\n"
    "/CHAPTER VII\n"
    "/- - - - - -\n"
    " GENERAL PROVISIONS\n"
    "\n"
    "/Article 51\n"
    "/-  -  -  -\n"
    " Scope\n"
    "\n"
    ": 1 The provisions of this Charter are addressed to the institutions\n"
    ":   and bodies of the Union with due regard for the principle of\n"
    ":   subsidiarity and to the Member States only when they are\n"
    ":   implementing Union law.  They shall therefore respect the rights,\n"
    ":   observe the principles and promote the application thereof in\n"
    ":   accordance with their respective powers.\n"
    ":\n"
    ": 2 This Charter does not establish any new power or task for the\n"
    ":   Community or the Union, or modify powers and tasks defined by the\n"
    ":   Treaties.\n"
    "\n"
    "/Article 52\n"
    "/-  -  -  -\n"
    " Scope of guaranteed rights\n"
    "\n"
    ": 1 Any limitation on the exercise of the rights and freedoms\n"
    ":   recognised by this Charter must be provided for by law and respect\n"
    ":   the essence of those rights and freedoms.  Subject to the\n"
    ":   principle of proportionality, limitations may be made only if they\n"
    ":   are necessary and genuinely meet objectives of general interest\n"
    ":   recognised by the Union or the need to protect the rights and\n"
    ":   freedoms of others.\n"
    ":\n"
    ": 2 Rights recognised by this Charter which are based on the Community\n"
    ":   Treaties or the Treaty on European Union shall be exercised under\n"
    ":   the conditions and within the limits defined by those Treaties.\n"
    ":\n"
    ": 3 In so far as this Charter contains rights which correspond to\n"
    ":   rights guaranteed by the Convention for the Protection of Human\n"
    ":   Rights and Fundamental Freedoms, the meaning and scope of those\n"
    ":   rights shall be the same as those laid down by the said\n"
    ":   Convention.  This provision shall not prevent Union law providing\n"
    ":   more extensive protection.\n"
    "\n"
    "/Article 53\n"
    "/-  -  -  -\n"
    " Level of protection\n"
    "\n"
    " Nothing in this Charter shall be interpreted as restricting or\n"
    " adversely affecting human rights and fundamental freedoms as\n"
    " recognised, in their respective fields of application, by Union law\n"
    " and international law and by international agreements to which the\n"
    " Union, the Community or all the Member States are party, including\n"
    " the European Convention for the Protection of Human Rights and\n"
    " Fundamental Freedoms, and by the Member States' constitutions.\n"
    "\n"
    "/Article 54\n"
    "/-  -  -  -\n"
    " Prohibition of abuse of rights\n"
    "\n"
    " Nothing in this Charter shall be interpreted as implying any right to\n"
    " engage in any activity or to perform any act aimed at the destruction\n"
    " of any of the rights and freedoms recognised in this Charter or at\n"
    " their limitation to a greater extent than is provided for herein.\n";

    static char * const documents[] = { document0,
                                        document1,
                                        document2
                                      };
    enum { NUM_DOCUMENTS = sizeof documents / sizeof *documents };
//..
// First, we define an alias to make our code more comprehensible.
//..
    typedef bsl::unordered_map<bsl::string, int> WordTally;
//..
// Next, we create an (empty) unordered map to hold our word tallies.  The
// output from the 'printf' statements will be discussed in {Example 2}.
//..
    WordTally wordTally;

if (verbose) {
    printf("size             %4d initial\n",
           static_cast<int>(wordTally.size()));
    printf("bucket_count     %4d initial\n",
           static_cast<int>(wordTally.bucket_count()));
    printf("load_factor      %f  initial\n", wordTally.load_factor());
    printf("max_load_factor  %f  initial\n", wordTally.max_load_factor());
};
//..
// Then, we define the set of characters that define word boundaries:
//..
    const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our documents.  Note that 'strtok' modifies
// the document arrays (which were not made 'const').
//
// For each iteration of the inner loop, that method looks for a map entry
// matchingv the given key value.  On the first occurence of a word, the map
// has no such entry, so one is created with a default value of the mapped
// value (0, just what we want in this case) and inserted into the map where is
// is found on any subsequent occurrences of the word.  The 'operator[]' method
// returns a reference providing modifiable access to the mapped value.  Here,
// we  the '++' operator to that reference to maintain a tally for the word.
//..
    for (int idx = 0; idx < NUM_DOCUMENTS; ++idx) {
        for (char *cur = strtok(documents[idx], delimiters);
                   cur;
                   cur = strtok(NULL,     delimiters)) {
            ++wordTally[bsl::string(cur)];
        }
    }
//..
// Now that the data has been (quickly) gathered, we can indulge in analysis
// that is more time consuming.  For example, we can define a comparison
// function, copy the data to another container (e.g., 'bsl::vector'), sort the
// entries, and determine the 20 most commonly used words in the given
// documents:
//..
    typedef bsl::pair<bsl::string, int> WordTallyEntry;
        // Assignable equivalent to 'WordTally::value_type'.  Note that
        // 'bsl::vector' requires assignable types.

    struct WordTallyEntryCompare {
        static bool lessValue(const WordTallyEntry& a,
                              const WordTallyEntry& b)
        {
            return a.second < b.second;
        }
        static bool moreValue(const WordTallyEntry& a,
                              const WordTallyEntry& b)
        {
            return lessValue(b, a);
        }
    };

    bsl::vector<WordTallyEntry> array(wordTally.cbegin(), wordTally.cend());

    ASSERT(20 <= array.size());

    std::partial_sort(array.begin(),
                      array.begin() + 20,
                      array.end(),
                      WordTallyEntryCompare::moreValue);
//..
// Notice that 'partial_sort' suffices here since we seek only the 20 most used
// words, not a complete distribution of word counts.
//
// Finally, we print the sorted portion of 'array':
//..
    for (bsl::vector<WordTallyEntry>::const_iterator cur  = array.begin(),
                                                     end  = cur + 20;
                                                     end != cur; ++cur) {
if (verbose) {
        printf("%-10s %4d\n", cur->first.c_str(), cur->second);
}
    }
//..
// and standard output shows:
//..
//  the         463
//  -           398
//  of          361
//  and         349
//  to          306
//  in          141
//  or          106
//  right        93
//  be           90
//  Article      86
//  has          79
//  a            76
//  shall        69
//  for          69
//  by           62
//  with         50
//  Everyone     49
//  rights       44
//  their        44
//  is           43
//..
// Notice that "-" (used as an header underscore in our markup) appears in the
// word count.  That could be elimiated by adding '-' to the set of delimiters;
// however, that would partition hyphenated words into separate words.  In
// practice, one defines a "stop list" of common words (e.g., "the", "of",
// "and", "is") that one does not wish to tally.  We could easily add "-" to
// the stop list.
//
///Example 2: Examining and Setting Unordered Map Configuration
///------------------------------------------------------------
// Suppose we wish to examine (and possibly influence) the performance of an
// unordered map.  The unordered map provides several interfaces that allow us
// to do so.  Several of these were used in {Example 1} (code repeated below):
//..
//  WordTally wordTally;
//
//  printf("size             %4d initial\n", wordTally.size());
//  printf("bucket_count     %4d initial\n", wordTally.bucket_count());
//  printf("load_factor      %f  initial\n", wordTally.load_factor());
//  printf("max_load_factor  %f  initial\n", wordTally.max_load_factor());
//..
// First, we examine the metrics of this newly created (empty) unordered map:
//..
//  size                0 initial
//  bucket_count        1 initial
//  load_factor      0.000000  initial
//  max_load_factor  1.000000  initial
//..
// Notice that even when there are no elements ('size' is 0) there is one
// bucket.  Since there are no elements, the average number of elements per
// bucket (the 'load_factor' above) must be 0.
//
// Next, after 'wordTally' has been loaded, we examine its metrics:
//..
if (verbose) {
    printf("size             %4d\n", static_cast<int>(wordTally.size()));
    printf("bucket_count     %4d\n",
        static_cast<int>(wordTally.bucket_count()));
    printf("load_factor      %f\n",  wordTally.load_factor());
    printf("max_load_factor  %f\n",  wordTally.max_load_factor());
}
//..
// and find at standard output:
//..
//  size             1504
//  bucket_count     2099
//  load_factor      0.716532
//  max_load_factor  1.000000
//..
// Notice how the number of buckets has increased.  (Sampling this metric as
// the map was loaded would show that the increase was done in several stages.)
//
// Then, we see that the load factor is indeed below the specified maximum;
// however we obtain further details of how the buckets are used.
//
// Using the 'bucket_count' method, the unordered map's interface for the
// number of elements in each bucket, we can easily determine the bucket with
// the greatest number of elements (i.e., the greatest number of collisions):
//..
    bsl::vector<int> bucketSizes;
    bucketSizes.reserve(wordTally.bucket_count());

    for (size_t idx = 0; idx < wordTally.bucket_count(); ++idx) {
       bucketSizes.push_back(static_cast<int>(wordTally.bucket_size(idx)));
    }

    ASSERT(0 < bucketSizes.size());
    int maxBucketSize = *std::max_element(bucketSizes.begin(),
                                          bucketSizes.end());
if (verbose) {
    printf("maxBucketSize    %4d\n", maxBucketSize);
}
//..
// and find on standard output:
//..
//  maxBucketSize       5
//..
// We can also count the number of empty buckets, and the number of buckets at
// 'maxBucketSize'.
//..
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
    // Work around per internal ticket D36282765
    int numEmptyBuckets = my_count_if_equal(bucketSizes.begin(),
                                            bucketSizes.end(),
                                            0);
#else
    int numEmptyBuckets = static_cast<int>(std::count(bucketSizes.begin(),
                                           bucketSizes.end(),
                                           0));
#endif
if (verbose) {
    printf("numEmptyBuckets  %4d\n", numEmptyBuckets);
}
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
    // Work around per internal ticket D36282765
    int numMaxBuckets = my_count_if_equal(bucketSizes.begin(),
                                          bucketSizes.end(),
                                          maxBucketSize);
#else
    int numMaxBuckets = static_cast<int>(std::count(bucketSizes.begin(),
                                         bucketSizes.end(),
                                         maxBucketSize));
#endif
if (verbose) {
    printf("numMaxBuckets    %4d\n", numMaxBuckets);
}
//..
// which shows on standard output:
//..
//  numEmptyBuckets  1031
//  numMaxBuckets       3
//..
// Suppose we are not satisfied with this distribution.  (Perhaps the load
// factor is too high.)  We can create a second, differently configured table.
//
// Next, create a new table 'wordTally2' with twice the bucket count shown by
// the first table ('wordTally'), and examine its initial metrics.
//..
    WordTally wordTally2(wordTally.bucket_count() * 2);

if (verbose) {
    printf("size2            %4d initial\n",
        static_cast<int>(wordTally2.size()));
    printf("bucket_count2    %4d initial\n",
        static_cast<int>(wordTally2.bucket_count()));
    printf("load_factor2     %f  initial\n", wordTally2.load_factor());
    printf("max_load_factor2 %f  initial\n", wordTally2.max_load_factor());
}
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
    wordTally2 = wordTally;

if (verbose) {
    printf("size2            %4d\n", static_cast<int>(wordTally2.size()));
    printf("bucket_count2    %4d\n",
        static_cast<int>(wordTally2.bucket_count()));
    printf("load_factor2     %f\n",  wordTally2.load_factor());
    printf("max_load_factor2 %f\n",  wordTally2.max_load_factor());
}

    bsl::vector<int> bucketSizes2;
    bucketSizes2.reserve(wordTally2.bucket_count());

    for (size_t idx = 0; idx < wordTally2.bucket_count(); ++idx) {
       bucketSizes2.push_back(static_cast<int>(wordTally2.bucket_size(idx)));
    }

    ASSERT(0 < bucketSizes2.size());
    int maxBucketSize2 = *std::max_element(bucketSizes2.begin(),
                                           bucketSizes2.end());
if (verbose) {
    printf("maxBucketSize2   %4d\n", maxBucketSize2);
}

#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
    // Work around per internal ticket D36282765
    int numEmptyBuckets2 = my_count_if_equal(bucketSizes2.begin(),
                                             bucketSizes2.end(),
                                             0);
#else
    int numEmptyBuckets2 = static_cast<int>(std::count(bucketSizes2.begin(),
                                            bucketSizes2.end(),
                                            0));
#endif
if (verbose) {
    printf("numEmptyBuckets2 %4d\n", numEmptyBuckets2);
}
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
    // Work around per internal ticket D36282765
    int numMaxBuckets2 = my_count_if_equal(bucketSizes2.begin(),
                                           bucketSizes2.end(),
                                           maxBucketSize2);
#else
    int numMaxBuckets2 = static_cast<int>(std::count(bucketSizes2.begin(),
                                          bucketSizes2.end(),
                                          maxBucketSize2));
#endif
if (verbose) {
    printf("numMaxBuckets2   %4d\n", numMaxBuckets2);
}
//..
// Finally, we see on standard output:
//..
//  size2            1504
//  bucket_count2    4201
//  load_factor2     0.358010
//  max_load_factor2 1.000000
//  maxBucketSize2      4
//  numEmptyBuckets2 2971
//  numMaxBuckets2      5
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
//
///Example 3: Inverse Concordance
///------------------------------
// continued...
//
// Next, we obtain a concordance for the document set (see
// {'bslstl_unorderedmultimap'|Example 1}).  Here, the concordance is provided
// as a statically initialized array:
//..
    const static struct {
//        bsl::string d_word;
        const char *d_word_p;
        int         d_documentCode;
        int         d_wordOffset;
    } concordance[] = {
        { "extent",             2,  3597 }, { "to",                 2,  1225 },
        { "greater",            2,  3596 }, { "to",                 2,  1221 },
        { "abuse",              2,  3551 }, { "to",                 2,  1182 },
        { "constitutions",      2,  3546 }, { "to",                 2,  1141 },
        { "affecting",          2,  3491 }, { "to",                 2,  1134 },
        { "Level",              2,  3477 }, { "to",                 2,  1115 },
        { "provision",          2,  3465 }, { "to",                 2,  1109 },
        { "said",               2,  3462 }, { "to",                 2,  1099 },
        { "correspond",         2,  3429 }, { "to",                 2,  1095 },
        { "contains",           2,  3426 }, { "to",                 2,  1084 },
        { "limits",             2,  3414 }, { "to",                 2,  1069 },
        { "protect",            2,  3379 }, { "to",                 2,  1062 },
        { "need",               2,  3377 }, { "to",                 2,  1060 },
        { "objectives",         2,  3367 }, { "to",                 2,  1057 },
        { "meet",               2,  3366 }, { "to",                 2,  1040 },
        { "Subject",            2,  3349 }, { "to",                 2,  1035 },
        { "essence",            2,  3343 }, { "to",                 2,  1026 },
        { "defined",            2,  3415 }, { "to",                 2,  1020 },
        { "defined",            2,  3309 }, { "to",                 2,   982 },
        { "modify",             2,  3305 }, { "to",                 2,   967 },
        { "task",               2,  3297 }, { "to",                 2,   959 },
        { "does",               2,  3290 }, { "to",                 2,   948 },
        { "thereof",            2,  3280 }, { "to",                 2,   945 },
        { "addressed",          2,  3238 }, { "to",                 2,   942 },
        { "Scope",              2,  3315 }, { "to",                 2,   935 },
        { "Scope",              2,  3230 }, { "to",                 2,   898 },
        { "VII",                2,  3225 }, { "to",                 2,   879 },
        { "convicted",          2,  3215 }, { "to",                 2,   876 },
        { "acquitted",          2,  3213 }, { "to",                 2,   854 },
        { "finally",            2,  3212 }, { "to",                 2,   848 },
        { "again",              2,  3197 }, { "to",                 2,   808 },
        { "liable",             2,  3191 }, { "to",                 2,   804 },
        { "twice",              2,  3178 }, { "to",                 2,   795 },
        { "punished",           2,  3196 }, { "to",                 2,   766 },
        { "punished",           2,  3177 }, { "to",                 2,   752 },
        { "disproportionate",   2,  3164 }, { "to",                 2,   732 },
        { "severity",           2,  3158 }, { "to",                 2,   721 },
        { "lighter",            2,  3110 }, { "to",                 2,   689 },
        { "provides",           2,  3107 }, { "to",                 2,   684 },
        { "subsequent",         2,  3097 }, { "to",                 2,   678 },
        { "If",                 2,  3096 }, { "to",                 2,   674 },
        { "penalties",          2,  3160 }, { "to",                 2,   665 },
        { "penalties",          2,  3037 }, { "to",                 2,   653 },
        { "legality",           2,  3030 }, { "to",                 2,   640 },
        { "Principles",         2,  3028 }, { "to",                 2,   594 },
        { "54",                 2,  3548 }, { "to",                 2,   570 },
        { "49",                 2,  3027 }, { "to",                 2,   552 },
        { "anyone",             2,  3018 }, { "to",                 2,   544 },
        { "53",                 2,  3476 }, { "to",                 2,   525 },
        { "48",                 2,  2985 }, { "to",                 2,   494 },
        { "aid",                2,  2975 }, { "to",                 2,   491 },
        { "aid",                2,  2959 }, { "to",                 2,   424 },
        { "Legal",              2,  2958 }, { "to",                 2,   388 },
        { "defended",           2,  2955 }, { "to",                 2,   377 },
        { "advised",            2,  2954 }, { "to",                 2,   368 },
        { "previously",         2,  2943 }, { "to",                 2,   360 },
        { "violated",           2,  2903 }, { "to",                 2,   353 },
        { "52",                 2,  3314 }, { "to",                 2,   318 },
        { "47",                 2,  2879 }, { "to",                 2,   313 },
        { "JUSTICE",            2,  2877 }, { "to",                 2,   310 },
        { "VI",                 2,  2876 }, { "to",                 2,   242 },
        { "diplomatic",         2,  2856 }, { "to",                 2,   175 },
        { "consular",           2,  2858 }, { "to",                 2,   148 },
        { "consular",           2,  2819 }, { "to",                 2,   102 },
        { "Diplomatic",         2,  2817 }, { "to",                 2,    98 },
        { "51",                 2,  3229 }, { "to",                 2,    23 },
        { "46",                 2,  2816 }, { "to",                 1,  1767 },
        { "resident",           2,  2807 }, { "to",                 1,  1761 },
        { "reside",             2,  2774 }, { "to",                 1,  1733 },
        { "50",                 2,  3170 }, { "to",                 1,  1678 },
        { "45",                 2,  2755 }, { "to",                 1,  1648 },
        { "petition",           2,  2750 }, { "to",                 1,  1621 },
        { "petition",           2,  2724 }, { "to",                 1,  1593 },
        { "role",               2,  2719 }, { "to",                 1,  1580 },
        { "judicial",           2,  2718 }, { "to",                 1,  1575 },
        { "acting",             2,  2715 }, { "to",                 1,  1566 },
        { "Instance",           2,  2714 }, { "to",                 1,  1555 },
        { "First",              2,  2713 }, { "to",                 1,  1545 },
        { "maladministration",  2,  2691 }, { "to",                 1,  1500 },
        { "refer",              2,  2682 }, { "to",                 1,  1491 },
        { "Ombudsman",          2,  2685 }, { "to",                 1,  1479 },
        { "Ombudsman",          2,  2656 }, { "to",                 1,  1444 },
        { "Commission",         2,  2652 }, { "to",                 1,  1417 },
        { "registered",         2,  2740 }, { "to",                 1,  1389 },
        { "registered",         2,  2672 }, { "to",                 1,  1357 },
        { "registered",         2,  2636 }, { "to",                 1,  1335 },
        { "documents",          2,  2653 }, { "to",                 1,  1319 },
        { "documents",          2,  2620 }, { "to",                 1,  1316 },
        { "answer",             2,  2609 }, { "to",                 1,  1284 },
        { "languages",          2,  2601 }, { "to",                 1,  1271 },
        { "write",              2,  2590 }, { "to",                 1,  1259 },
        { "performance",        2,  2568 }, { "to",                 1,  1251 },
        { "servants",           2,  2565 }, { "to",                 1,  1246 },
        { "caused",             2,  2558 }, { "to",                 1,  1244 },
        { "damage",             2,  2557 }, { "to",                 1,  1199 },
        { "make",               2,  2554 }, { "to",                 1,  1193 },
        { "decisions",          2,  2543 }, { "to",                 1,  1130 },
        { "reasons",            2,  2540 }, { "to",                 1,  1107 },
        { "give",               2,  2539 }, { "to",                 1,  1097 },
        { "obligation",         2,  2534 }, { "to",                 1,  1095 },
        { "secrecy",            2,  2531 }, { "to",                 1,  1082 },
        { "confidentiality",    2,  2525 }, { "to",                 1,  1060 },
        { "file",               2,  2518 }, { "to",                 1,  1054 },
        { "adversely",          2,  3490 }, { "to",                 1,  1044 },
        { "adversely",          2,  2502 }, { "to",                 1,  1026 },
        { "affect",             2,  2498 }, { "to",                 1,  1006 },
        { "measure",            2,  2495 }, { "to",                 1,   995 },
        { "heard",              2,  2491 }, { "to",                 1,   968 },
        { "impartially",        2,  2464 }, { "to",                 1,   954 },
        { "handled",            2,  2463 }, { "to",                 1,   914 },
        { "affairs",            2,  2462 }, { "to",                 1,   910 },
        { "administration",     2,  2537 }, { "to",                 1,   903 },
        { "administration",     2,  2450 }, { "to",                 1,   900 },
        { "municipal",          2,  2424 }, { "to",                 1,   892 },
        { "municipal",          2,  2405 }, { "to",                 1,   875 },
        { "ballot",             2,  2392 }, { "to",                 1,   858 },
        { "Members",            2,  2375 }, { "to",                 1,   842 },
        { "Parliament",         2,  2753 }, { "to",                 1,   812 },
        { "Parliament",         2,  2649 }, { "to",                 1,   809 },
        { "Parliament",         2,  2379 }, { "to",                 1,   799 },
        { "Parliament",         2,  2354 }, { "to",                 1,   797 },
        { "Parliament",         2,  2331 }, { "to",                 1,   789 },
        { "candidate",          2,  2422 }, { "to",                 1,   772 },
        { "candidate",          2,  2403 }, { "to",                 1,   754 },
        { "candidate",          2,  2348 }, { "to",                 1,   743 },
        { "candidate",          2,  2325 }, { "to",                 1,   732 },
        { "44",                 2,  2721 }, { "to",                 1,   649 },
        { "39",                 2,  2316 }, { "to",                 1,   641 },
        { "CITIZENS'",          2,  2313 }, { "to",                 1,   602 },
        { "V",                  2,  2312 }, { "to",                 1,   588 },
        { "consumer",           2,  2309 }, { "to",                 1,   558 },
        { "Consumer",           2,  2299 }, { "to",                 1,   549 },
        { "43",                 2,  2655 }, { "to",                 1,   534 },
        { "38",                 2,  2298 }, { "to",                 1,   525 },
        { "integrated",         2,  2280 }, { "to",                 1,   502 },
        { "environment",        2,  2277 }, { "to",                 1,   488 },
        { "quality",            2,  2274 }, { "to",                 1,   485 },
        { "improvement",        2,  2271 }, { "to",                 1,   448 },
        { "environmental",      2,  2267 }, { "to",                 1,   422 },
        { "Environmental",      2,  2261 }, { "to",                 1,   365 },
        { "42",                 2,  2615 }, { "to",                 1,   302 },
        { "37",                 2,  2260 }, { "to",                 1,   287 },
        { "cohesion",           2,  2255 }, { "to",                 1,   264 },
        { "territorial",        2,  2254 }, { "to",                 1,   196 },
        { "41",                 2,  2446 }, { "to",                 1,   178 },
        { "36",                 2,  2210 }, { "to",                 1,   130 },
        { "policies",           2,  2302 }, { "to",                 1,   109 },
        { "policies",           2,  2283 }, { "to",                 1,   102 },
        { "policies",           2,  2206 }, { "to",                 1,    99 },
        { "implementation",     2,  2202 }, { "to",                 0,  1351 },
        { "definition",         2,  2200 }, { "to",                 0,  1317 },
        { "preventive",         2,  2168 }, { "to",                 0,  1307 },
        { "Health",             2,  2159 }, { "to",                 0,  1292 },
        { "40",                 2,  2394 }, { "to",                 0,  1272 },
        { "35",                 2,  2158 }, { "to",                 0,  1259 },
        { "sufficient",         2,  2968 }, { "to",                 0,  1219 },
        { "sufficient",         2,  2140 }, { "to",                 0,  1169 },
        { "poverty",            2,  2115 }, { "to",                 0,  1152 },
        { "exclusion",          2,  2113 }, { "to",                 0,  1134 },
        { "combat",             2,  2111 }, { "to",                 0,  1111 },
        { "legally",            2,  2806 }, { "to",                 0,  1104 },
        { "legally",            2,  2083 }, { "to",                 0,  1094 },
        { "observe",            2,  3273 }, { "to",                 0,  1079 },
        { "moving",             2,  2082 }, { "to",                 0,  1009 },
        { "residing",           2,  2736 }, { "to",                 0,   992 },
        { "residing",           2,  2668 }, { "to",                 0,   982 },
        { "residing",           2,  2632 }, { "to",                 0,   976 },
        { "residing",           2,  2080 }, { "to",                 0,   930 },
        { "dependency",         2,  2051 }, { "to",                 0,   874 },
        { "accidents",          2,  2050 }, { "to",                 0,   824 },
        { "industrial",         2,  2049 }, { "to",                 0,   794 },
        { "entitlement",        2,  2033 }, { "to",                 0,   716 },
        { "parental",           2,  2009 }, { "to",                 0,   705 },
        { "reconcile",          2,  1980 }, { "to",                 0,   701 },
        { "legal",              2,  2734 }, { "to",                 0,   698 },
        { "legal",              2,  2666 }, { "to",                 0,   688 },
        { "legal",              2,  2630 }, { "to",                 0,   680 },
        { "legal",              2,  1973 }, { "to",                 0,   649 },
        { "Family",             2,  1964 }, { "to",                 0,   604 },
        { "harm",               2,  1945 }, { "to",                 0,   578 },
        { "admitted",           2,  1923 }, { "to",                 0,   575 },
        { "Young",              2,  1921 }, { "to",                 0,   557 },
        { "limited",            2,  1919 }, { "to",                 0,   542 },
        { "leaving",            2,  1901 }, { "to",                 0,   526 },
        { "school",             2,  1900 }, { "to",                 0,   514 },
        { "lower",              2,  1896 }, { "to",                 0,   511 },
        { "admission",          2,  1890 }, { "to",                 0,   445 },
        { "minimum",            2,  1899 }, { "to",                 0,   441 },
        { "minimum",            2,  1887 }, { "to",                 0,   414 },
        { "young",              2,  1914 }, { "to",                 0,   408 },
        { "young",              2,  1876 }, { "to",                 0,   406 },
        { "period",             2,  1863 }, { "to",                 0,   379 },
        { "annual",             2,  1862 }, { "to",                 0,   362 },
        { "periods",            2,  1858 }, { "to",                 0,   352 },
        { "weekly",             2,  1856 }, { "to",                 0,   307 },
        { "daily",              2,  1854 }, { "to",                 0,   281 },
        { "maximum",            2,  1850 }, { "to",                 0,   275 },
        { "safety",             2,  1947 }, { "to",                 0,   261 },
        { "safety",             2,  1838 }, { "to",                 0,   239 },
        { "Fair",               2,  1818 }, { "to",                 0,   232 },
        { "worker",             2,  1843 }, { "to",                 0,   225 },
        { "worker",             2,  1825 }, { "to",                 0,   191 },
        { "worker",             2,  1797 }, { "to",                 0,   185 },
        { "dismissal",          2,  1993 }, { "to",                 0,   167 },
        { "dismissal",          2,  1805 }, { "to",                 0,   163 },
        { "dismissal",          2,  1795 }, { "to",                 0,   160 },
        { "unjustified",        2,  1804 }, { "to",                 0,   123 },
        { "unjustified",        2,  1794 }, { "to",                 0,    90 },
        { "strike",             2,  1766 }, { "to",                 0,    83 },
        { "defend",             2,  1762 }, { "to",                 0,    68 },
        { "conflicts",          2,  1754 }, { "to",                 0,    53 },
        { "agreements",         2,  3514 }, { "to",                 0,    40 },
        { "agreements",         2,  1745 }, { "to",                 0,    28 },
        { "organisations",      2,  1726 }, { "that",               2,  3112 },
        { "respective",         2,  3501 }, { "that",               2,  3084 },
        { "respective",         2,  3285 }, { "that",               2,  2872 },
        { "respective",         2,  1725 }, { "that",               2,  2443 },
        { "employers",          2,  1722 }, { "that",               2,  2372 },
        { "action",             2,  1767 }, { "that",               2,  1584 },
        { "action",             2,  1760 }, { "that",               2,  1301 },
        { "action",             2,  1719 }, { "that",               1,  1551 },
        { "bargaining",         2,  1717 }, { "that",               1,   714 },
        { "collective",         2,  1759 }, { "that",               1,   267 },
        { "collective",         2,  1744 }, { "that",               1,   115 },
        { "collective",         2,  1716 }, { "that",               0,  1297 },
        { "appropriate",        2,  1930 }, { "that",               0,  1277 },
        { "appropriate",        2,  1748 }, { "that",               0,  1265 },
        { "appropriate",        2,  1685 }, { "that",               0,   565 },
        { "Workers",            2,  1720 }, { "that",               0,   220 },
        { "Workers",            2,  1678 }, { "that",               0,   201 },
        { "undertaking",        2,  1677 }, { "that",               0,   111 },
        { "Workers'",           2,  1669 }, { "that",               0,   100 },
        { "IV",                 2,  1665 }, { "that",               0,    94 },
        { "participation",      2,  1657 }, { "that",               0,    74 },
        { "integration",        2,  1655 }, { "Seas",               0,   975 },
        { "occupational",       2,  1654 }, { "Seas",               0,   793 },
        { "designed",           2,  1647 }, { "while",              2,  2519 },
        { "benefit",            2,  2175 }, { "while",              2,   109 },
        { "benefit",            2,  1644 }, { "while",              0,   227 },
        { "disabilities",       2,  1642 }, { "proportionality",    2,  3354 },
        { "disabilities",       2,  1631 }, { "proportionality",    2,  3032 },
        { "Integration",        2,  1627 }, { "*4*",                0,     3 },
        { "lead",               2,  1610 }, { "secure",             1,   303 },
        { "respects",           2,  2222 }, { "secure",             0,   124 },
        { "respects",           2,  2120 }, { "shewn",              0,   219 },
        { "respects",           2,  2031 }, { "dissolve",           0,    29 },
        { "respects",           2,  1636 }, { "Protection",         2,  3534 },
        { "respects",           2,  1603 }, { "Protection",         2,  3438 },
        { "elderly",            2,  1608 }, { "Protection",         2,  1789 },
        { "elderly",            2,  1598 }, { "Protection",         2,  1269 },
        { "contact",            2,  1576 }, { "Protection",         2,   585 },
        { "relationship",       2,  1573 }, { "Protection",         2,   259 },
        { "regular",            2,  1569 }, { "Protection",         0,   894 },
        { "maintain",           2,  1566 }, { "The",                2,  3232 },
        { "primary",            2,  1556 }, { "The",                2,  3157 },
        { "best",               2,  1551 }, { "The",                2,  2218 },
        { "child's",            2,  1550 }, { "The",                2,  2027 },
        { "institutions",       2,  3241 }, { "The",                2,  1969 },
        { "institutions",       2,  2698 }, { "The",                2,  1886 },
        { "institutions",       2,  2593 }, { "The",                2,  1880 },
        { "institutions",       2,  2561 }, { "The",                2,  1632 },
        { "institutions",       2,  2473 }, { "The",                2,  1599 },
        { "institutions",       2,  1548 }, { "The",                2,  1594 },
        { "actions",            2,  1537 }, { "The",                2,  1486 },
        { "maturity",           2,  1533 }, { "The",                2,  1460 },
        { "concern",            2,  1525 }, { "The",                2,  1428 },
        { "consideration",      2,  1557 }, { "The",                2,  1223 },
        { "consideration",      2,  1521 }, { "The",                2,  1193 },
        { "views",              2,  1516 }, { "The",                2,  1113 },
        { "views",              2,  1513 }, { "The",                2,   965 },
        { "express",            2,  1511 }, { "The",                2,   917 },
        { "Children",           2,  1492 }, { "The",                2,   825 },
        { "child",              2,  2018 }, { "The",                2,   764 },
        { "child",              2,  1871 }, { "The",                2,   682 },
        { "child",              2,  1560 }, { "The",                2,   321 },
        { "child",              2,  1490 }, { "The",                2,    95 },
        { "represented",        2,  2957 }, { "The",                2,     9 },
        { "represented",        2,  2849 }, { "The",                1,  1137 },
        { "represented",        2,  1482 }, { "The",                1,   940 },
        { "favour",             2,  1478 }, { "The",                1,     0 },
        { "advantages",         2,  2096 }, { "The",                0,   314 },
        { "advantages",         2,  1476 }, { "The",                0,     5 },
        { "specific",           2,  1475 }, { "on",                 2,  3402 },
        { "providing",          2,  3471 }, { "on",                 2,  3395 },
        { "providing",          2,  2041 }, { "on",                 2,  3322 },
        { "providing",          2,  1473 }, { "on",                 2,  3049 },
        { "adoption",           2,  2015 }, { "on",                 2,  2864 },
        { "adoption",           2,  1470 }, { "on",                 2,  1567 },
        { "areas",              2,  1454 }, { "on",                 2,  1522 },
        { "linguistic",         2,  1435 }, { "on",                 2,  1414 },
        { "linguistic",         2,  1426 }, { "on",                 2,  1399 },
        { "Cultural",           2,  1423 }, { "on",                 2,  1344 },
        { "grounds",            2,  1415 }, { "on",                 2,   615 },
        { "provisions",         2,  3233 }, { "on",                 2,   449 },
        { "provisions",         2,  1408 }, { "on",                 2,   248 },
        { "prejudice",          2,  3122 }, { "on",                 2,    58 },
        { "prejudice",          2,  1904 }, { "on",                 2,    43 },
        { "prejudice",          2,  1404 }, { "on",                 2,    29 },
        { "application",        2,  3504 }, { "on",                 1,  1481 },
        { "application",        2,  3279 }, { "on",                 1,   678 },
        { "application",        2,  1387 }, { "on",                 1,   407 },
        { "Within",             2,  1383 }, { "on",                 0,  1342 },
        { "orientation",        2,  1378 }, { "on",                 0,  1011 },
        { "minority",           2,  1371 }, { "on",                 0,   972 },
        { "membership",         2,  1367 }, { "on",                 0,   770 },
        { "features",           2,  1357 }, { "on",                 0,   748 },
        { "ethnic",             2,  1352 }, { "on",                 0,   616 },
        { "ground",             2,  1346 }, { "on",                 0,   494 },
        { "Any",                2,  3320 }, { "on",                 0,   174 },
        { "Any",                2,  2725 }, { "separate",           0,    49 },
        { "Any",                2,  2657 }, { "*CONGRESS*",         0,     1 },
        { "Any",                2,  2621 }, { "taking",             0,   845 },
        { "Any",                2,  1341 }, { "human",              2,  3492 },
        { "Non",                2,  1338 }, { "human",              2,  2192 },
        { "EQUALITY",           2,  1323 }, { "human",              2,   534 },
        { "III",                2,  1322 }, { "human",              2,   472 },
        { "risk",               2,  1300 }, { "human",              2,   452 },
        { "serious",            2,  1299 }, { "human",              2,   315 },
        { "there",              2,  1296 }, { "human",              2,    49 },
        { "where",              2,  1295 }, { "human",              1,  1506 },
        { "extradited",         2,  1291 }, { "human",              1,  1497 },
        { "expelled",           2,  1289 }, { "human",              1,  1299 },
        { "removed",            2,  1288 }, { "human",              1,   331 },
        { "expulsions",         2,  1280 }, { "human",              1,   214 },
        { "independence",       2,  1651 }, { "human",              1,   164 },
        { "independence",       2,  1616 }, { "human",              1,   155 },
        { "Collective",         2,  1279 }, { "human",              1,   116 },
        { "expulsion",          2,  1275 }, { "human",              1,    65 },
        { "refugees",           2,  1256 }, { "human",              1,    43 },
        { "relating",           2,  1538 }, { "human",              1,    25 },
        { "relating",           2,  1251 }, { "human",              0,    20 },
        { "extradition",        2,  1277 }, { "eat",                0,   654 },
        { "1967",               2,  1250 }, { "we",                 0,  1348 },
        { "January",            2,  1249 }, { "we",                 0,  1192 },
        { "Protocol",           2,  1246 }, { "we",                 0,  1141 },
        { "1951",               2,  1243 }, { "seas",               0,   905 },
        { "July",               2,  1242 }, { "equal",              2,  1332 },
        { "Geneva",             2,  1238 }, { "equal",              1,  1275 },
        { "Intellectual",       2,  1213 }, { "equal",              1,  1272 },
        { "far",                2,  3422 }, { "equal",              1,  1167 },
        { "far",                2,  2972 }, { "equal",              1,  1128 },
        { "far",                2,  1204 }, { "equal",              1,   911 },
        { "regulated",          2,  1199 }, { "equal",              1,   535 },
        { "loss",               2,  2060 }, { "equal",              1,   526 },
        { "loss",               2,  1192 }, { "equal",              1,   515 },
        { "paid",               2,  2004 }, { "equal",              1,   337 },
        { "paid",               2,  1865 }, { "equal",              1,   169 },
        { "paid",               2,  1186 }, { "equal",              1,    16 },
        { "compensation",       2,  1184 }, { "equal",              0,    99 },
        { "provided",           2,  3600 }, { "equal",              0,    51 },
        { "provided",           2,  3336 }, { "a",                  2,  3595 },
        { "provided",           2,  2231 }, { "a",                  2,  3109 },
        { "provided",           2,  1702 }, { "a",                  2,  3102 },
        { "provided",           2,  1177 }, { "a",                  2,  3078 },
        { "interest",           2,  3370 }, { "a",                  2,  3060 },
        { "interest",           2,  2229 }, { "a",                  2,  2934 },
        { "interest",           2,  2217 }, { "a",                  2,  2928 },
        { "interest",           2,  1756 }, { "a",                  2,  2912 },
        { "interest",           2,  1211 }, { "a",                  2,  2887 },
        { "interest",           2,  1168 }, { "a",                  2,  2845 },
        { "except",             2,  1917 }, { "a",                  2,  2831 },
        { "except",             2,  1164 }, { "a",                  2,  2812 },
        { "possessions",        2,  1163 }, { "a",                  2,  2743 },
        { "possessions",        2,  1153 }, { "a",                  2,  2675 },
        { "lawfully",           2,  1151 }, { "a",                  2,  2643 },
        { "bequeath",           2,  1147 }, { "a",                  2,  2639 },
        { "party",              2,  3527 }, { "a",                  2,  2468 },
        { "dispose",            2,  1144 }, { "a",                  2,  2421 },
        { "business",           2,  2530 }, { "a",                  2,  2402 },
        { "business",           2,  1118 }, { "a",                  2,  2388 },
        { "business",           2,  1112 }, { "a",                  2,  2347 },
        { "conduct",            2,  1116 }, { "a",                  2,  2324 },
        { "conduct",            2,  1110 }, { "a",                  2,  2305 },
        { "authorised",         2,  1083 }, { "a",                  2,  2132 },
        { "third",              2,  2832 }, { "a",                  2,  2017 },
        { "third",              2,  2804 }, { "a",                  2,  1995 },
        { "third",              2,  1079 }, { "a",                  2,  1783 },
        { "Nationals",          2,  1077 }, { "a",                  2,  1611 },
        { "applicable",         2,  3116 }, { "a",                  2,  1571 },
        { "applicable",         2,  3087 }, { "a",                  2,  1568 },
        { "applicable",         1,   716 }, { "a",                  2,  1555 },
        { "imposed",            2,  3082 }, { "a",                  2,  1369 },
        { "imposed",            1,   710 }, { "a",                  2,  1298 },
        { "usurpations",        0,  1155 }, { "a",                  2,  1293 },
        { "usurpations",        0,   330 }, { "a",                  2,  1117 },
        { "usurpations",        0,   252 }, { "a",                  2,  1111 },
        { "committed",          2,  3142 }, { "a",                  2,  1042 },
        { "committed",          2,  3095 }, { "a",                  2,   691 },
        { "committed",          2,  3075 }, { "a",                  2,   680 },
        { "committed",          1,   724 }, { "a",                  2,   459 },
        { "committed",          1,   703 }, { "a",                  2,   203 },
        { "was",                2,  3143 }, { "a",                  2,    25 },
        { "was",                2,  3141 }, { "a",                  1,  1718 },
        { "was",                2,  3094 }, { "a",                  1,  1622 },
        { "was",                2,  3086 }, { "a",                  1,  1542 },
        { "was",                2,  3074 }, { "a",                  1,  1358 },
        { "was",                1,   723 }, { "a",                  1,  1186 },
        { "was",                1,   715 }, { "a",                  1,   905 },
        { "was",                1,   702 }, { "a",                  1,   859 },
        { "constitute",         2,  3059 }, { "a",                  1,   706 },
        { "constitute",         1,   688 }, { "a",                  1,   689 },
        { "did",                2,  3057 }, { "a",                  1,   652 },
        { "did",                1,   686 }, { "a",                  1,   635 },
        { "13",                 2,   910 }, { "a",                  1,   603 },
        { "13",                 1,   766 }, { "a",                  1,   506 },
        { "account",            2,  3050 }, { "a",                  1,   424 },
        { "account",            1,   679 }, { "a",                  1,   356 },
        { "defence",            2,  3016 }, { "a",                  1,   253 },
        { "defence",            2,  2992 }, { "a",                  1,   220 },
        { "defence",            1,   666 }, { "a",                  1,   106 },
        { "guarantees",         1,   662 }, { "a",                  1,    61 },
        { "trial",              2,  3124 }, { "a",                  0,  1339 },
        { "trial",              2,  2889 }, { "a",                  0,  1084 },
        { "trial",              1,   654 }, { "a",                  0,  1075 },
        { "EUROPEAN",           2,     6 }, { "a",                  0,   960 },
        { "visible",            2,   201 }, { "a",                  0,   810 },
        { "proved",             2,  3004 }, { "a",                  0,   736 },
        { "proved",             1,   646 }, { "a",                  0,   702 },
        { "jurisdictional",     1,   413 }, { "a",                  0,   638 },
        { "until",              2,  3003 }, { "a",                  0,   505 },
        { "until",              1,   645 }, { "a",                  0,   438 },
        { "innocent",           2,  3002 }, { "a",                  0,   353 },
        { "innocent",           1,   644 }, { "a",                  0,   324 },
        { "offence",            2,  3203 }, { "a",                  0,   259 },
        { "offence",            2,  3186 }, { "a",                  0,   246 },
        { "offence",            2,  3168 }, { "a",                  0,    65 },
        { "offence",            2,  3104 }, { "implementing",       2,  3264 },
        { "offence",            2,  3093 }, { "*July*",             0,     2 },
        { "offence",            2,  3062 }, { "Facts",              0,   349 },
        { "offence",            2,  3048 }, { "consultation",       2,  1691 },
        { "offence",            1,   722 }, { "consultation",       2,  1674 },
        { "offence",            1,   691 }, { "Savages",            0,  1020 },
        { "offence",            1,   677 }, { "the",                2,  3582 },
        { "offence",            1,   637 }, { "the",                2,  3577 },
        { "penal",              1,   721 }, { "the",                2,  3543 },
        { "penal",              1,   690 }, { "the",                2,  3533 },
        { "penal",              1,   676 }, { "the",                2,  3529 },
        { "penal",              1,   636 }, { "the",                2,  3523 },
        { "charged",            2,  3022 }, { "the",                2,  3519 },
        { "charged",            2,  2998 }, { "the",                2,  3517 },
        { "charged",            1,   633 }, { "the",                2,  3461 },
        { "11",                 2,   784 }, { "the",                2,  3454 },
        { "11",                 1,   630 }, { "the",                2,  3445 },
        { "charge",             1,   626 }, { "the",                2,  3437 },
        { "our",                0,  1359 }, { "the",                2,  3434 },
        { "our",                0,  1356 }, { "the",                2,  3413 },
        { "our",                0,  1354 }, { "the",                2,  3409 },
        { "our",                0,  1230 }, { "the",                2,  3400 },
        { "our",                0,  1186 }, { "the",                2,  3396 },
        { "our",                0,  1160 }, { "the",                2,  3380 },
        { "our",                0,  1149 }, { "the",                2,  3376 },
        { "our",                0,  1126 }, { "the",                2,  3373 },
        { "our",                0,  1095 }, { "the",                2,  3351 },
        { "our",                0,  1015 }, { "the",                2,  3342 },
        { "our",                0,   967 }, { "the",                2,  3326 },
        { "our",                0,   917 }, { "the",                2,  3323 },
        { "our",                0,   910 }, { "the",                2,  3311 },
        { "our",                0,   907 }, { "the",                2,  3302 },
        { "our",                0,   904 }, { "the",                2,  3299 },
        { "our",                0,   865 }, { "the",                2,  3278 },
        { "our",                0,   860 }, { "the",                2,  3274 },
        { "our",                0,   850 }, { "the",                2,  3271 },
        { "our",                0,   847 }, { "the",                2,  3257 },
        { "our",                0,   773 }, { "the",                2,  3251 },
        { "our",                0,   758 }, { "the",                2,  3245 },
        { "our",                0,   711 }, { "the",                2,  3240 },
        { "our",                0,   706 }, { "the",                2,  3222 },
        { "our",                0,   674 }, { "the",                2,  3217 },
        { "our",                0,   651 }, { "the",                2,  3183 },
        { "tolerance",          1,  1515 }, { "the",                2,  3166 },
        { "obligations",        2,   240 }, { "the",                2,  3152 },
        { "obligations",        1,   621 }, { "the",                2,  3147 },
        { "combined",           0,   695 }, { "the",                2,  3137 },
        { "determination",      1,   616 }, { "the",                2,  3123 },
        { "tribunal",           2,  2942 }, { "the",                2,  3105 },
        { "tribunal",           2,  2913 }, { "the",                2,  3099 },
        { "tribunal",           1,   613 }, { "the",                2,  3091 },
        { "hearing",            2,  2932 }, { "the",                2,  3089 },
        { "hearing",            1,   607 }, { "the",                2,  3070 },
        { "commission",         2,  3100 }, { "the",                2,  3015 },
        { "equality",           2,  1463 }, { "the",                2,  3012 },
        { "equality",           2,    52 }, { "the",                2,  2950 },
        { "equality",           1,   601 }, { "the",                2,  2917 },
        { "10",                 2,   709 }, { "the",                2,  2905 },
        { "10",                 1,   595 }, { "the",                2,  2900 },
        { "interference",       2,   816 }, { "the",                2,  2897 },
        { "interference",       1,  1058 }, { "the",                2,  2869 },
        { "interference",       1,   762 }, { "the",                2,  2865 },
        { "interference",       1,   734 }, { "the",                2,  2855 },
        { "cause",              0,   512 }, { "the",                2,  2836 },
        { "arrest",             1,   590 }, { "the",                2,  2828 },
        { "arbitrary",          1,   733 }, { "the",                2,  2824 },
        { "arbitrary",          1,   589 }, { "the",                2,  2809 },
        { "9",                  2,   672 }, { "the",                2,  2798 },
        { "9",                  1,   582 }, { "the",                2,  2795 },
        { "remedy",             2,  2910 }, { "the",                2,  2780 },
        { "remedy",             2,  2884 }, { "the",                2,  2777 },
        { "remedy",             1,   561 }, { "the",                2,  2769 },
        { "eugenic",            2,   435 }, { "the",                2,  2766 },
        { "8",                  2,   584 }, { "the",                2,  2751 },
        { "8",                  1,   553 }, { "the",                2,  2747 },
        { "incitement",         1,   548 }, { "the",                2,  2728 },
        { "violation",          1,   541 }, { "the",                2,  2710 },
        { "THE",                2,     5 }, { "the",                2,  2705 },
        { "THE",                1,   242 }, { "the",                2,  2702 },
        { "discrimination",     2,  1413 }, { "the",                2,  2696 },
        { "discrimination",     2,  1342 }, { "the",                2,  2693 },
        { "discrimination",     2,  1339 }, { "the",                2,  2687 },
        { "discrimination",     1,  1267 }, { "the",                2,  2684 },
        { "discrimination",     1,   551 }, { "the",                2,  2679 },
        { "discrimination",     1,   539 }, { "the",                2,  2660 },
        { "discrimination",     1,   524 }, { "the",                2,  2624 },
        { "everywhere",         1,   504 }, { "the",                2,  2611 },
        { "12",                 2,   836 }, { "the",                2,  2603 },
        { "12",                 1,   726 }, { "the",                2,  2600 },
        { "degrading",          2,  1317 }, { "the",                2,  2595 },
        { "degrading",          2,   497 }, { "the",                2,  2592 },
        { "degrading",          2,   482 }, { "the",                2,  2583 },
        { "degrading",          1,   492 }, { "the",                2,  2580 },
        { "torture",            2,  1312 }, { "the",                2,  2575 },
        { "torture",            2,   492 }, { "the",                2,  2567 },
        { "torture",            2,   478 }, { "the",                2,  2552 },
        { "torture",            1,   486 }, { "the",                2,  2548 },
        { "subjected",          2,  1307 }, { "the",                2,  2536 },
        { "subjected",          2,   490 }, { "the",                2,  2533 },
        { "subjected",          1,   731 }, { "the",                2,  2521 },
        { "subjected",          1,   587 }, { "the",                2,  2506 },
        { "subjected",          1,   484 }, { "the",                2,  2484 },
        { "5",                  2,   502 }, { "the",                2,  2477 },
        { "5",                  1,   479 }, { "the",                2,  2472 },
        { "prohibited",         2,  1885 }, { "the",                2,  2455 },
        { "prohibited",         2,  1420 }, { "the",                2,  2437 },
        { "prohibited",         2,  1381 }, { "the",                2,  2427 },
        { "prohibited",         2,  1282 }, { "the",                2,  2413 },
        { "prohibited",         2,   537 }, { "the",                2,  2410 },
        { "prohibited",         1,   473 }, { "the",                2,  2377 },
        { "financial",          2,   462 }, { "the",                2,  2366 },
        { "slave",              1,   469 }, { "the",                2,  2356 },
        { "invested",           0,   871 }, { "the",                2,  2352 },
        { "No",                 2,  3187 }, { "the",                2,  2339 },
        { "No",                 2,  3039 }, { "the",                2,  2336 },
        { "No",                 2,  1284 }, { "the",                2,  2329 },
        { "No",                 2,  1154 }, { "the",                2,  2292 },
        { "No",                 2,   520 }, { "the",                2,  2285 },
        { "No",                 2,   510 }, { "the",                2,  2282 },
        { "No",                 2,   486 }, { "the",                2,  2276 },
        { "No",                 2,   363 }, { "the",                2,  2273 },
        { "No",                 1,  1090 }, { "the",                2,  2270 },
        { "No",                 1,   980 }, { "the",                2,  2257 },
        { "No",                 1,   862 }, { "the",                2,  2251 },
        { "No",                 1,   727 }, { "the",                2,  2244 },
        { "No",                 1,   668 }, { "the",                2,  2241 },
        { "No",                 1,   583 }, { "the",                2,  2199 },
        { "No",                 1,   480 }, { "the",                2,  2180 },
        { "No",                 1,   457 }, { "the",                2,  2172 },
        { "4",                  2,  2586 }, { "the",                2,  2163 },
        { "4",                  2,   475 }, { "the",                2,  2145 },
        { "4",                  1,  1311 }, { "the",                2,  2121 },
        { "4",                  1,   456 }, { "the",                2,  2116 },
        { "liberty",            2,   553 }, { "the",                2,  2085 },
        { "liberty",            2,   545 }, { "the",                2,  2066 },
        { "liberty",            1,   450 }, { "the",                2,  2057 },
        { "limitation",         2,  3593 }, { "the",                2,  2032 },
        { "limitation",         2,  3321 }, { "the",                2,  2012 },
        { "limitation",         2,  1848 }, { "the",                2,  2001 },
        { "limitation",         1,  1341 }, { "the",                2,  1988 },
        { "limitation",         1,   890 }, { "the",                2,  1898 },
        { "limitation",         1,   439 }, { "the",                2,  1845 },
        { "governing",          2,  1009 }, { "the",                2,  1827 },
        { "governing",          2,   777 }, { "the",                2,  1799 },
        { "governing",          2,   702 }, { "the",                2,  1791 },
        { "governing",          1,   434 }, { "the",                2,  1778 },
        { "trust",              1,   431 }, { "the",                2,  1747 },
        { "belongs",            1,   426 }, { "the",                2,  1738 },
        { "territory",          2,  2829 }, { "the",                2,  1700 },
        { "territory",          2,  2810 }, { "the",                2,  1696 },
        { "territory",          2,  2778 }, { "the",                2,  1684 },
        { "territory",          1,   421 }, { "the",                2,  1676 },
        { "interfere",          2,  1958 }, { "the",                2,  1662 },
        { "country",            2,  2833 }, { "the",                2,  1659 },
        { "country",            1,  1135 }, { "the",                2,  1637 },
        { "country",            1,  1115 }, { "the",                2,  1607 },
        { "country",            1,   801 }, { "the",                2,  1604 },
        { "country",            1,   792 }, { "the",                2,  1597 },
        { "country",            1,   419 }, { "the",                2,  1563 },
        { "no",                 1,  1728 }, { "the",                2,  1549 },
        { "no",                 1,   402 }, { "the",                2,  1495 },
        { "Furthermore",        1,   401 }, { "the",                2,  1489 },
        { "birth",              2,  2013 }, { "the",                2,  1480 },
        { "birth",              2,  1373 }, { "the",                2,  1467 },
        { "birth",              1,   397 }, { "the",                2,  1406 },
        { "Access",             2,  2211 }, { "the",                2,  1397 },
        { "property",           2,  1372 }, { "the",                2,  1392 },
        { "property",           2,  1214 }, { "the",                2,  1389 },
        { "property",           2,  1196 }, { "the",                2,  1384 },
        { "property",           2,  1135 }, { "the",                2,  1334 },
        { "property",           1,   988 }, { "the",                2,  1328 },
        { "property",           1,   970 }, { "the",                2,  1309 },
        { "property",           1,   396 }, { "the",                2,  1271 },
        { "religion",           2,  1359 }, { "the",                2,  1264 },
        { "religion",           2,   754 }, { "the",                2,  1261 },
        { "religion",           2,   734 }, { "the",                2,  1253 },
        { "religion",           2,   727 }, { "the",                2,  1245 },
        { "religion",           2,   715 }, { "the",                2,  1237 },
        { "religion",           1,  1029 }, { "the",                2,  1234 },
        { "religion",           1,  1009 }, { "the",                2,  1209 },
        { "religion",           1,  1001 }, { "the",                2,  1175 },
        { "religion",           1,   896 }, { "the",                2,  1171 },
        { "religion",           1,   387 }, { "the",                2,  1166 },
        { "language",           2,  2613 }, { "the",                2,  1139 },
        { "language",           2,  1358 }, { "the",                2,  1104 },
        { "language",           1,   386 }, { "the",                2,  1090 },
        { "sex",                2,  1483 }, { "the",                2,  1087 },
        { "sex",                2,  1349 }, { "the",                2,  1064 },
        { "sex",                1,   385 }, { "the",                2,  1055 },
        { "colour",             2,  1351 }, { "the",                2,  1052 },
        { "colour",             1,   384 }, { "the",                2,  1033 },
        { "race",               2,  1350 }, { "the",                2,  1010 },
        { "race",               1,   893 }, { "the",                2,  1006 },
        { "race",               1,   383 }, { "the",                2,   984 },
        { "kind",               1,  1548 }, { "the",                2,   978 },
        { "kind",               1,   380 }, { "the",                2,   957 },
        { "distinction",        1,   403 }, { "the",                2,   940 },
        { "distinction",        1,   377 }, { "the",                2,   913 },
        { "Equality",           2,  1444 }, { "the",                2,   907 },
        { "Equality",           2,  1439 }, { "the",                2,   904 },
        { "Equality",           2,  1326 }, { "the",                2,   900 },
        { "forth",              1,  1783 }, { "the",                2,   884 },
        { "forth",              1,  1634 }, { "the",                2,   872 },
        { "forth",              1,   372 }, { "the",                2,   846 },
        { "scope",              2,  3448 }, { "the",                2,   830 },
        { "scope",              2,  1385 }, { "the",                2,   793 },
        { "she",                2,  3208 }, { "the",                2,   778 },
        { "she",                2,  2843 }, { "the",                2,   774 },
        { "she",                2,  2434 }, { "the",                2,   719 },
        { "she",                2,  2363 }, { "the",                2,   703 },
        { "she",                2,  1304 }, { "the",                2,   699 },
        { "set",                2,   330 }, { "the",                2,   687 },
        { "set",                1,  1782 }, { "the",                2,   651 },
        { "set",                1,  1633 }, { "the",                2,   636 },
        { "set",                1,   371 }, { "the",                2,   622 },
        { "entitled",           2,  2926 }, { "the",                2,   619 },
        { "entitled",           2,  2851 }, { "the",                2,   616 },
        { "entitled",           2,  2089 }, { "the",                2,   595 },
        { "entitled",           2,  1094 }, { "the",                2,   592 },
        { "entitled",           1,  1620 }, { "the",                2,   568 },
        { "entitled",           1,  1416 }, { "the",                2,   550 },
        { "entitled",           1,  1198 }, { "the",                2,   468 },
        { "entitled",           1,   953 }, { "the",                2,   465 },
        { "entitled",           1,   909 }, { "the",                2,   451 },
        { "entitled",           1,   598 }, { "the",                2,   447 },
        { "entitled",           1,   533 }, { "the",                2,   442 },
        { "entitled",           1,   521 }, { "the",                2,   432 },
        { "entitled",           1,   364 }, { "the",                2,   425 },
        { "Everyone",           2,  2994 }, { "the",                2,   420 },
        { "Everyone",           2,  2947 }, { "the",                2,   414 },
        { "Everyone",           2,  2924 }, { "the",                2,   406 },
        { "Everyone",           2,  2890 }, { "the",                2,   400 },
        { "Everyone",           2,  2161 }, { "the",                2,   386 },
        { "Everyone",           2,  2079 }, { "the",                2,   381 },
        { "Everyone",           2,  1776 }, { "the",                2,   378 },
        { "Everyone",           2,  1330 }, { "the",                2,   369 },
        { "Everyone",           2,  1137 }, { "the",                2,   358 },
        { "Everyone",           2,  1031 }, { "the",                2,   325 },
        { "Everyone",           2,   938 }, { "the",                2,   314 },
        { "Everyone",           2,   844 }, { "the",                2,   294 },
        { "Everyone",           2,   791 }, { "the",                2,   289 },
        { "Everyone",           2,   717 }, { "the",                2,   284 },
        { "Everyone",           2,   634 }, { "the",                2,   280 },
        { "Everyone",           2,   590 }, { "the",                2,   275 },
        { "Everyone",           2,   566 }, { "the",                2,   271 },
        { "Everyone",           2,   548 }, { "the",                2,   266 },
        { "Everyone",           2,   384 }, { "the",                2,   258 },
        { "Everyone",           2,   356 }, { "the",                2,   254 },
        { "Everyone",           1,  1645 }, { "the",                2,   251 },
        { "Everyone",           1,  1618 }, { "the",                2,   246 },
        { "Everyone",           1,  1589 }, { "the",                2,   243 },
        { "Everyone",           1,  1561 }, { "the",                2,   235 },
        { "Everyone",           1,  1440 }, { "the",                2,   227 },
        { "Everyone",           1,  1353 }, { "the",                2,   223 },
        { "Everyone",           1,  1331 }, { "the",                2,   220 },
        { "Everyone",           1,  1312 }, { "the",                2,   217 },
        { "Everyone",           1,  1278 }, { "the",                2,   212 },
        { "Everyone",           1,  1264 }, { "the",                2,   183 },
        { "Everyone",           1,  1240 }, { "the",                2,   177 },
        { "Everyone",           1,  1184 }, { "the",                2,   165 },
        { "Everyone",           1,  1123 }, { "the",                2,   134 },
        { "Everyone",           1,  1103 }, { "the",                2,   130 },
        { "Everyone",           1,  1078 }, { "the",                2,   126 },
        { "Everyone",           1,  1040 }, { "the",                2,   119 },
        { "Everyone",           1,   991 }, { "the",                2,   114 },
        { "Everyone",           1,   964 }, { "the",                2,   111 },
        { "Everyone",           1,   854 }, { "the",                2,   103 },
        { "Everyone",           1,   805 }, { "the",                2,    99 },
        { "Everyone",           1,   785 }, { "the",                2,    83 },
        { "Everyone",           1,   768 }, { "the",                2,    80 },
        { "Everyone",           1,   750 }, { "the",                2,    73 },
        { "Everyone",           1,   632 }, { "the",                2,    70 },
        { "Everyone",           1,   596 }, { "the",                2,    64 },
        { "Everyone",           1,   554 }, { "the",                2,    59 },
        { "Everyone",           1,   498 }, { "the",                2,    44 },
        { "Everyone",           1,   444 }, { "the",                2,    39 },
        { "Everyone",           1,   362 }, { "the",                1,  1778 },
        { "employment",         2,  2062 }, { "the",                1,  1773 },
        { "employment",         2,  1892 }, { "the",                1,  1739 },
        { "employment",         2,  1881 }, { "the",                1,  1734 },
        { "employment",         2,  1456 }, { "the",                1,  1714 },
        { "employment",         2,  1059 }, { "the",                1,  1706 },
        { "employment",         1,  1250 }, { "the",                1,  1697 },
        { "towards",            1,   352 }, { "the",                1,  1688 },
        { "born",               1,  1425 }, { "the",                1,  1666 },
        { "born",               1,   334 }, { "the",                1,  1654 },
        { "cutting",            0,   756 }, { "the",                1,  1649 },
        { "All",                1,  1422 }, { "the",                1,  1629 },
        { "All",                1,   531 }, { "the",                1,  1614 },
        { "All",                1,   513 }, { "the",                1,  1597 },
        { "All",                1,   330 }, { "the",                1,  1594 },
        { "1",                  2,  3319 }, { "the",                1,  1591 },
        { "1",                  2,  3231 }, { "the",                1,  1577 },
        { "1",                  2,  3038 }, { "the",                1,  1573 },
        { "1",                  2,  2993 }, { "the",                1,  1569 },
        { "1",                  2,  2762 }, { "the",                1,  1563 },
        { "1",                  2,  2451 }, { "the",                1,  1547 },
        { "1",                  2,  2332 }, { "the",                1,  1535 },
        { "1",                  2,  2026 }, { "the",                1,  1531 },
        { "1",                  2,  1968 }, { "the",                1,  1528 },
        { "1",                  2,  1823 }, { "the",                1,  1501 },
        { "1",                  2,  1491 }, { "the",                1,  1496 },
        { "1",                  2,  1340 }, { "the",                1,  1492 },
        { "1",                  2,  1278 }, { "the",                1,  1482 },
        { "1",                  2,  1136 }, { "the",                1,  1453 },
        { "1",                  2,  1030 }, { "the",                1,  1442 },
        { "1",                  2,   937 }, { "the",                1,  1433 },
        { "1",                  2,   843 }, { "the",                1,  1392 },
        { "1",                  2,   790 }, { "the",                1,  1387 },
        { "1",                  2,   716 }, { "the",                1,  1364 },
        { "1",                  2,   589 }, { "the",                1,  1355 },
        { "1",                  2,   509 }, { "the",                1,  1333 },
        { "1",                  2,   383 }, { "the",                1,  1324 },
        { "1",                  2,   355 }, { "the",                1,  1314 },
        { "1",                  2,   337 }, { "the",                1,  1282 },
        { "1",                  1,  1644 }, { "the",                1,  1269 },
        { "1",                  1,  1560 }, { "the",                1,  1242 },
        { "1",                  1,  1439 }, { "the",                1,  1231 },
        { "1",                  1,  1352 }, { "the",                1,  1220 },
        { "1",                  1,  1239 }, { "the",                1,  1212 },
        { "1",                  1,  1102 }, { "the",                1,  1191 },
        { "1",                  1,  1077 }, { "the",                1,  1147 },
        { "1",                  1,   963 }, { "the",                1,  1144 },
        { "1",                  1,   881 }, { "the",                1,  1140 },
        { "1",                  1,   853 }, { "the",                1,  1125 },
        { "1",                  1,   804 }, { "the",                1,  1111 },
        { "1",                  1,   767 }, { "the",                1,  1105 },
        { "1",                  1,   631 }, { "the",                1,  1080 },
        { "1",                  1,   329 }, { "the",                1,  1042 },
        { "Article",            2,  3547 }, { "the",                1,   993 },
        { "Article",            2,  3475 }, { "the",                1,   966 },
        { "Article",            2,  3313 }, { "the",                1,   959 },
        { "Article",            2,  3228 }, { "the",                1,   943 },
        { "Article",            2,  3169 }, { "the",                1,   936 },
        { "Article",            2,  3119 }, { "the",                1,   930 },
        { "Article",            2,  3026 }, { "the",                1,   898 },
        { "Article",            2,  2984 }, { "the",                1,   873 },
        { "Article",            2,  2923 }, { "the",                1,   856 },
        { "Article",            2,  2878 }, { "the",                1,   848 },
        { "Article",            2,  2815 }, { "the",                1,   843 },
        { "Article",            2,  2754 }, { "the",                1,   828 },
        { "Article",            2,  2720 }, { "the",                1,   807 },
        { "Article",            2,  2654 }, { "the",                1,   787 },
        { "Article",            2,  2614 }, { "the",                1,   779 },
        { "Article",            2,  2445 }, { "the",                1,   770 },
        { "Article",            2,  2393 }, { "the",                1,   758 },
        { "Article",            2,  2315 }, { "the",                1,   755 },
        { "Article",            2,  2297 }, { "the",                1,   752 },
        { "Article",            2,  2259 }, { "the",                1,   720 },
        { "Article",            2,  2209 }, { "the",                1,   718 },
        { "Article",            2,  2157 }, { "the",                1,   712 },
        { "Article",            2,  2019 }, { "the",                1,   698 },
        { "Article",            2,  1962 }, { "the",                1,   661 },
        { "Article",            2,  1867 }, { "the",                1,   639 },
        { "Article",            2,  1816 }, { "the",                1,   615 },
        { "Article",            2,  1787 }, { "the",                1,   576 },
        { "Article",            2,  1768 }, { "the",                1,   570 },
        { "Article",            2,  1712 }, { "the",                1,   563 },
        { "Article",            2,  1667 }, { "the",                1,   556 },
        { "Article",            2,  1625 }, { "the",                1,   529 },
        { "Article",            2,  1592 }, { "the",                1,   517 },
        { "Article",            2,  1484 }, { "the",                1,   509 },
        { "Article",            2,  1437 }, { "the",                1,   500 },
        { "Article",            2,  1421 }, { "the",                1,   468 },
        { "Article",            2,  1336 }, { "the",                1,   446 },
        { "Article",            2,  1324 }, { "the",                1,   418 },
        { "Article",            2,  1267 }, { "the",                1,   411 },
        { "Article",            2,  1218 }, { "the",                1,   408 },
        { "Article",            2,  1131 }, { "the",                1,   367 },
        { "Article",            2,  1106 }, { "the",                1,   321 },
        { "Article",            2,  1017 }, { "the",                1,   313 },
        { "Article",            2,   932 }, { "the",                1,   265 },
        { "Article",            2,   909 }, { "the",                1,   234 },
        { "Article",            2,   835 }, { "the",                1,   230 },
        { "Article",            2,   783 }, { "the",                1,   205 },
        { "Article",            2,   708 }, { "the",                1,   202 },
        { "Article",            2,   671 }, { "the",                1,   168 },
        { "Article",            2,   583 }, { "the",                1,   163 },
        { "Article",            2,   558 }, { "the",                1,   158 },
        { "Article",            2,   541 }, { "the",                1,   148 },
        { "Article",            2,   501 }, { "the",                1,   143 },
        { "Article",            2,   474 }, { "the",                1,   140 },
        { "Article",            2,   374 }, { "the",                1,   132 },
        { "Article",            2,   350 }, { "the",                1,   122 },
        { "Article",            2,   336 }, { "the",                1,    88 },
        { "Article",            1,  1742 }, { "the",                1,    84 },
        { "Article",            1,  1642 }, { "the",                1,    58 },
        { "Article",            1,  1616 }, { "the",                1,    53 },
        { "Article",            1,  1558 }, { "the",                1,    36 },
        { "Article",            1,  1437 }, { "the",                1,    28 },
        { "Article",            1,  1350 }, { "the",                1,    24 },
        { "Article",            1,  1329 }, { "the",                1,    15 },
        { "Article",            1,  1237 }, { "the",                1,    10 },
        { "Article",            1,  1182 }, { "the",                0,  1343 },
        { "Article",            1,  1100 }, { "the",                0,  1333 },
        { "Article",            1,  1075 }, { "the",                0,  1284 },
        { "Article",            1,  1038 }, { "the",                0,  1273 },
        { "Article",            1,   989 }, { "the",                0,  1240 },
        { "Article",            1,   961 }, { "the",                0,  1234 },
        { "Article",            1,   879 }, { "the",                0,  1227 },
        { "Article",            1,   851 }, { "the",                0,  1224 },
        { "Article",            1,   802 }, { "the",                0,  1220 },
        { "Article",            1,   765 }, { "the",                0,  1209 },
        { "Article",            1,   725 }, { "the",                0,  1206 },
        { "Article",            1,   629 }, { "the",                0,  1194 },
        { "Article",            1,   594 }, { "the",                0,  1182 },
        { "Article",            1,   581 }, { "the",                0,  1170 },
        { "Article",            1,   552 }, { "the",                0,  1146 },
        { "Article",            1,   511 }, { "the",                0,  1123 },
        { "Article",            1,   496 }, { "the",                0,  1081 },
        { "Article",            1,   478 }, { "the",                0,  1048 },
        { "Article",            1,   455 }, { "the",                0,  1017 },
        { "Article",            1,   442 }, { "the",                0,  1012 },
        { "Article",            1,   360 }, { "the",                0,   984 },
        { "Article",            1,   328 }, { "the",                0,   973 },
        { "territories",        2,  1088 }, { "the",                0,   957 },
        { "territories",        1,   324 }, { "the",                0,   950 },
        { "both",               2,  1578 }, { "the",                0,   932 },
        { "both",               1,   311 }, { "the",                0,   914 },
        { "effective",          2,  2980 }, { "the",                0,   857 },
        { "effective",          2,  2909 }, { "the",                0,   836 },
        { "effective",          2,  2883 }, { "the",                0,   803 },
        { "effective",          1,   560 }, { "the",                0,   782 },
        { "effective",          1,   307 }, { "the",                0,   764 },
        { "international",      2,  3513 }, { "the",                0,   749 },
        { "international",      2,  3509 }, { "the",                0,   689 },
        { "international",      2,  3067 }, { "the",                0,   682 },
        { "international",      2,   239 }, { "the",                0,   671 },
        { "international",      1,  1625 }, { "the",                0,   627 },
        { "international",      1,  1205 }, { "the",                0,   621 },
        { "international",      1,   695 }, { "the",                0,   596 },
        { "international",      1,   415 }, { "the",                0,   585 },
        { "international",      1,   301 }, { "the",                0,   568 },
        { "national",           2,  3064 }, { "the",                0,   559 },
        { "national",           2,  2846 }, { "the",                0,   544 },
        { "national",           2,  2234 }, { "the",                0,   538 },
        { "national",           2,  2184 }, { "the",                0,   534 },
        { "national",           2,  2153 }, { "the",                0,   527 },
        { "national",           2,  2103 }, { "the",                0,   518 },
        { "national",           2,  2074 }, { "the",                0,   498 },
        { "national",           2,  1812 }, { "the",                0,   495 },
        { "national",           2,  1734 }, { "the",                0,   469 },
        { "national",           2,  1708 }, { "the",                0,   462 },
        { "national",           2,  1370 }, { "the",                0,   436 },
        { "national",           2,  1125 }, { "the",                0,   431 },
        { "national",           2,  1007 }, { "the",                0,   419 },
        { "national",           2,   775 }, { "the",                0,   370 },
        { "national",           2,   700 }, { "the",                0,   364 },
        { "national",           2,   141 }, { "the",                0,   336 },
        { "national",           2,   127 }, { "the",                0,   317 },
        { "national",           1,  1202 }, { "the",                0,   302 },
        { "national",           1,   693 }, { "the",                0,   292 },
        { "national",           1,   565 }, { "the",                0,   255 },
        { "national",           1,   392 }, { "the",                0,   237 },
        { "national",           1,   299 }, { "the",                0,   158 },
        { "progressive",        1,   297 }, { "the",                0,   155 },
        { "education",          2,  1961 }, { "the",                0,   140 },
        { "education",          2,   985 }, { "the",                0,   137 },
        { "education",          2,   963 }, { "the",                0,   118 },
        { "education",          2,   943 }, { "the",                0,    84 },
        { "education",          2,   936 }, { "the",                0,    78 },
        { "education",          1,  1550 }, { "the",                0,    69 },
        { "education",          1,  1474 }, { "the",                0,    55 },
        { "education",          1,  1466 }, { "the",                0,    48 },
        { "education",          1,  1459 }, { "the",                0,    46 },
        { "education",          1,  1445 }, { "the",                0,    43 },
        { "education",          1,   286 }, { "the",                0,    30 },
        { "6",                  2,   542 }, { "the",                0,    17 },
        { "6",                  1,   497 }, { "the",                0,     9 },
        { "continuing",         2,   951 }, { "hath",               0,   218 },
        { "teaching",           2,   987 }, { "forms",              1,   477 },
        { "teaching",           2,   759 }, { "forms",              0,   238 },
        { "teaching",           1,  1033 }, { "status",             2,  1254 },
        { "teaching",           1,   284 }, { "status",             1,   416 },
        { "him",                2,  2499 }, { "status",             1,   400 },
        { "him",                2,   647 }, { "object",             0,   335 },
        { "him",                2,   601 }, { "political",          2,  1362 },
        { "him",                1,   628 }, { "political",          2,   901 },
        { "him",                1,   574 }, { "political",          2,   864 },
        { "strive",             1,   282 }, { "political",          1,   836 },
        { "adequate",           1,  1362 }, { "political",          1,   412 },
        { "constantly",         1,   278 }, { "political",          1,   388 },
        { "organ",              1,   272 }, { "political",          0,  1279 },
        { "including",          2,  3528 }, { "political",          0,    31 },
        { "including",          2,  1765 }, { "endeavoured",        0,  1008 },
        { "including",          2,  1455 }, { "endeavoured",        0,   556 },
        { "including",          1,  1375 }, { "hold",               2,   805 },
        { "including",          1,  1339 }, { "hold",               1,  1055 },
        { "including",          1,   793 }, { "hold",               0,  1193 },
        { "individual",         2,  2494 }, { "hold",               0,  1189 },
        { "individual",         2,    71 }, { "hold",               0,    87 },
        { "individual",         1,   269 }, { "bands",              0,    32 },
        { "be",                 2,  3559 }, { "Honor",              0,  1361 },
        { "be",                 2,  3485 }, { "own",                2,  1142 },
        { "be",                 2,  3453 }, { "own",                1,   969 },
        { "be",                 2,  3406 }, { "own",                1,   795 },
        { "be",                 2,  3357 }, { "own",                0,   866 },
        { "be",                 2,  3335 }, { "have",               2,  2949 },
        { "be",                 2,  3193 }, { "have",               2,  2607 },
        { "be",                 2,  3190 }, { "have",               2,  2551 },
        { "be",                 2,  3174 }, { "have",               2,  2512 },
        { "be",                 2,  3163 }, { "have",               2,  2458 },
        { "be",                 2,  3115 }, { "have",               2,  1987 },
        { "be",                 2,  3081 }, { "have",               2,  1927 },
        { "be",                 2,  3042 }, { "have",               2,  1727 },
        { "be",                 2,  3024 }, { "have",               2,  1562 },
        { "be",                 2,  3000 }, { "have",               2,  1494 },
        { "be",                 2,  2961 }, { "have",               2,   946 },
        { "be",                 2,  2850 }, { "have",               2,   654 },
        { "be",                 2,  2790 }, { "have",               1,  1541 },
        { "be",                 2,  2490 }, { "have",               1,   897 },
        { "be",                 2,  2381 }, { "have",               1,   193 },
        { "be",                 2,  2279 }, { "have",               1,   176 },
        { "be",                 2,  2196 }, { "have",               1,   146 },
        { "be",                 2,  1935 }, { "have",               1,   103 },
        { "be",                 2,  1910 }, { "have",               1,    51 },
        { "be",                 2,  1895 }, { "have",               1,    45 },
        { "be",                 2,  1687 }, { "have",               0,  1304 },
        { "be",                 2,  1554 }, { "have",               0,  1166 },
        { "be",                 2,  1518 }, { "have",               0,  1142 },
        { "be",                 2,  1450 }, { "have",               0,  1132 },
        { "be",                 2,  1419 }, { "have",               0,  1119 },
        { "be",                 2,  1380 }, { "have",               0,  1099 },
        { "be",                 2,  1306 }, { "have",               0,  1088 },
        { "be",                 2,  1287 }, { "have",               0,  1055 },
        { "be",                 2,  1228 }, { "have",               0,  1043 },
        { "be",                 2,  1216 }, { "have",               0,   524 },
        { "be",                 2,  1198 }, { "have",               0,    34 },
        { "be",                 2,  1157 }, { "good",               2,  2555 },
        { "be",                 2,  1001 }, { "good",               2,  2449 },
        { "be",                 2,   930 }, { "good",               2,  1693 },
        { "be",                 2,   923 }, { "good",               2,  1188 },
        { "be",                 2,   833 }, { "good",               0,  1241 },
        { "be",                 2,   694 }, { "good",               0,   372 },
        { "be",                 2,   663 }, { "after",              0,   508 },
        { "be",                 2,   608 }, { "connected",          2,  1997 },
        { "be",                 2,   523 }, { "connected",          0,    35 },
        { "be",                 2,   513 }, { "Hands",              0,   997 },
        { "be",                 2,   489 }, { "them",               2,  1526 },
        { "be",                 2,   409 }, { "them",               2,    20 },
        { "be",                 2,   366 }, { "them",               0,  1282 },
        { "be",                 2,   346 }, { "them",               0,  1190 },
        { "be",                 1,  1749 }, { "them",               0,  1144 },
        { "be",                 1,  1730 }, { "them",               0,  1121 },
        { "be",                 1,  1675 }, { "them",               0,  1101 },
        { "be",                 1,  1639 }, { "them",               0,   734 },
        { "be",                 1,  1553 }, { "them",               0,   474 },
        { "be",                 1,  1489 }, { "them",               0,   442 },
        { "be",                 1,  1476 }, { "them",               0,   409 },
        { "be",                 1,  1468 }, { "them",               0,   306 },
        { "be",                 1,  1461 }, { "them",               0,   263 },
        { "be",                 1,  1448 }, { "them",               0,   186 },
        { "be",                 1,  1171 }, { "them",               0,    82 },
        { "be",                 1,  1163 }, { "them",               0,    64 },
        { "be",                 1,  1154 }, { "them",               0,    36 },
        { "be",                 1,  1143 }, { "conditions",         2,  3410 },
        { "be",                 1,  1093 }, { "conditions",         2,  2918 },
        { "be",                 1,   983 }, { "conditions",         2,  2867 },
        { "be",                 1,   925 }, { "conditions",         2,  2439 },
        { "be",                 1,   865 }, { "conditions",         2,  2368 },
        { "be",                 1,   825 }, { "conditions",         2,  2181 },
        { "be",                 1,   730 }, { "conditions",         2,  1929 },
        { "be",                 1,   709 }, { "conditions",         2,  1831 },
        { "be",                 1,   671 }, { "conditions",         2,  1822 },
        { "be",                 1,   642 }, { "conditions",         2,  1701 },
        { "be",                 1,   586 }, { "conditions",         2,  1176 },
        { "be",                 1,   483 }, { "conditions",         2,  1097 },
        { "be",                 1,   472 }, { "conditions",         1,  1255 },
        { "be",                 1,   460 }, { "conditions",         0,  1035 },
        { "be",                 1,   429 }, { "conditions",         0,   586 },
        { "be",                 1,   405 }, { "Country",            0,   981 },
        { "be",                 1,   119 }, { "and",                2,  3584 },
        { "be",                 1,   100 }, { "and",                2,  3541 },
        { "be",                 0,  1293 }, { "and",                2,  3538 },
        { "be",                 0,  1260 }, { "and",                2,  3511 },
        { "be",                 0,  1080 }, { "and",                2,  3508 },
        { "be",                 0,   795 }, { "and",                2,  3494 },
        { "be",                 0,   515 }, { "and",                2,  3447 },
        { "be",                 0,   396 }, { "and",                2,  3442 },
        { "be",                 0,   350 }, { "and",                2,  3411 },
        { "be",                 0,   207 }, { "and",                2,  3382 },
        { "be",                 0,    91 }, { "and",                2,  3364 },
        { "achievement",        1,   257 }, { "and",                2,  3347 },
        { "standard",           1,  1359 }, { "and",                2,  3340 },
        { "standard",           1,   255 }, { "and",                2,  3328 },
        { "RIGHTS",             2,  2314 }, { "and",                2,  3307 },
        { "RIGHTS",             2,     3 }, { "and",                2,  3276 },
        { "RIGHTS",             1,   251 }, { "and",                2,  3255 },
        { "vocational",         2,   949 }, { "and",                2,  3242 },
        { "OF",                 2,     4 }, { "and",                2,  3125 },
        { "OF",                 2,     1 }, { "and",                2,  3036 },
        { "OF",                 1,   249 }, { "and",                2,  3031 },
        { "UNIVERSAL",          1,   247 }, { "and",                2,  2989 },
        { "THIS",               1,   246 }, { "and",                2,  2956 },
        { "proclaims",          1,   245 }, { "and",                2,  2940 },
        { "ASSEMBLY",           1,   244 }, { "and",                2,  2930 },
        { "GENERAL",            2,  3226 }, { "and",                2,  2893 },
        { "GENERAL",            1,   243 }, { "and",                2,  2885 },
        { "Assent",             0,   715 }, { "and",                2,  2818 },
        { "Assent",             0,   603 }, { "and",                2,  2787 },
        { "Assent",             0,   394 }, { "and",                2,  2773 },
        { "Assent",             0,   361 }, { "and",                2,  2759 },
        { "Therefore",          1,   241 }, { "and",                2,  2730 },
        { "Now",                1,   240 }, { "and",                2,  2709 },
        { "greatest",           1,   231 }, { "and",                2,  2662 },
        { "understanding",      1,  1514 }, { "and",                2,  2651 },
        { "understanding",      1,   222 }, { "and",                2,  2626 },
        { "freedoms",           2,  3585 }, { "and",                2,  2605 },
        { "freedoms",           2,  3496 }, { "and",                2,  2529 },
        { "freedoms",           2,  3383 }, { "and",                2,  2526 },
        { "freedoms",           2,  3348 }, { "and",                2,  2474 },
        { "freedoms",           2,  3329 }, { "and",                2,  2466 },
        { "freedoms",           2,  2894 }, { "and",                2,  2417 },
        { "freedoms",           2,   327 }, { "and",                2,  2398 },
        { "freedoms",           1,  1781 }, { "and",                2,  2390 },
        { "freedoms",           1,  1725 }, { "and",                2,  2343 },
        { "freedoms",           1,  1700 }, { "and",                2,  2320 },
        { "freedoms",           1,  1672 }, { "and",                2,  2287 },
        { "freedoms",           1,  1632 }, { "and",                2,  2269 },
        { "freedoms",           1,  1510 }, { "and",                2,  2253 },
        { "freedoms",           1,   370 }, { "and",                2,  2236 },
        { "freedoms",           1,   294 }, { "and",                2,  2221 },
        { "freedoms",           1,   227 }, { "and",                2,  2207 },
        { "freedoms",           1,   218 }, { "and",                2,  2201 },
        { "use",                2,  1194 }, { "and",                2,  2186 },
        { "use",                2,  1143 }, { "and",                2,  2171 },
        { "observance",         2,   762 }, { "and",                2,  2155 },
        { "observance",         1,  1037 }, { "and",                2,  2152 },
        { "observance",         1,   310 }, { "and",                2,  2125 },
        { "observance",         1,   212 }, { "and",                2,  2119 },
        { "universal",          2,  2385 }, { "and",                2,  2114 },
        { "universal",          2,    46 }, { "and",                2,  2105 },
        { "universal",          1,  1165 }, { "and",                2,  2102 },
        { "universal",          1,   305 }, { "and",                2,  2094 },
        { "universal",          1,   208 }, { "and",                2,  2081 },
        { "promotion",          1,   206 }, { "and",                2,  2076 },
        { "ensures",            2,   155 }, { "and",                2,  2073 },
        { "population",         0,   560 }, { "and",                2,  2055 },
        { "pledged",            1,   194 }, { "and",                2,  2038 },
        { "origin",             2,  1355 }, { "and",                2,  2030 },
        { "origin",             1,   395 }, { "and",                2,  2023 },
        { "larger",             1,   188 }, { "and",                2,  2007 },
        { "granted",            2,  2791 }, { "and",                2,  2000 },
        { "granted",            1,   573 }, { "and",                2,  1982 },
        { "better",             1,   183 }, { "and",                2,  1975 },
        { "had",                1,   659 }, { "and",                2,  1965 },
        { "merciless",          0,  1018 }, { "and",                2,  1940 },
        { "progress",           2,   190 }, { "and",                2,  1934 },
        { "progress",           1,   181 }, { "and",                2,  1916 },
        { "determined",         1,  1683 }, { "and",                2,  1873 },
        { "determined",         1,   177 }, { "and",                2,  1859 },
        { "fundamental",        2,  3495 }, { "and",                2,  1855 },
        { "fundamental",        2,   180 }, { "and",                2,  1839 },
        { "fundamental",        1,  1509 }, { "and",                2,  1819 },
        { "fundamental",        1,  1456 }, { "and",                2,  1814 },
        { "fundamental",        1,   946 }, { "and",                2,  1811 },
        { "fundamental",        1,   571 }, { "and",                2,  1750 },
        { "fundamental",        1,   217 }, { "and",                2,  1742 },
        { "fundamental",        1,   154 }, { "and",                2,  1736 },
        { "cultural",           2,  1623 }, { "and",                2,  1733 },
        { "cultural",           2,  1432 }, { "and",                2,  1721 },
        { "cultural",           1,  1570 }, { "and",                2,  1718 },
        { "cultural",           1,  1224 }, { "and",                2,  1710 },
        { "nationals",          2,  2870 }, { "and",                2,  1707 },
        { "nationals",          2,  2802 }, { "and",                2,  1698 },
        { "nationals",          2,  2441 }, { "and",                2,  1690 },
        { "nationals",          2,  2370 }, { "and",                2,  1673 },
        { "faith",              1,   152 }, { "and",                2,  1656 },
        { "too",                0,  1165 }, { "and",                2,  1653 },
        { "right",              2,  3564 }, { "and",                2,  1635 },
        { "right",              2,  2990 }, { "and",                2,  1622 },
        { "right",              2,  2906 }, { "and",                2,  1617 },
        { "right",              2,  2770 }, { "and",                2,  1615 },
        { "right",              2,  2748 }, { "and",                2,  1602 },
        { "right",              2,  2680 }, { "and",                2,  1574 },
        { "right",              2,  2644 }, { "and",                2,  1532 },
        { "right",              2,  2549 }, { "and",                2,  1500 },
        { "right",              2,  2507 }, { "and",                2,  1458 },
        { "right",              2,  2485 }, { "and",                2,  1447 },
        { "right",              2,  2481 }, { "and",                2,  1442 },
        { "right",              2,  2456 }, { "and",                2,  1434 },
        { "right",              2,  2414 }, { "and",                2,  1425 },
        { "right",              2,  2340 }, { "and",                2,  1402 },
        { "right",              2,  2173 }, { "and",                2,  1395 },
        { "right",              2,  2164 }, { "and",                2,  1257 },
        { "right",              2,  2122 }, { "and",                2,  1244 },
        { "right",              2,  2002 }, { "and",                2,  1173 },
        { "right",              2,  1989 }, { "and",                2,  1169 },
        { "right",              2,  1846 }, { "and",                2,  1146 },
        { "right",              2,  1828 }, { "and",                2,  1127 },
        { "right",              2,  1800 }, { "and",                2,  1124 },
        { "right",              2,  1779 }, { "and",                2,  1068 },
        { "right",              2,  1739 }, { "and",                2,  1039 },
        { "right",              2,  1670 }, { "and",                2,  1024 },
        { "right",              2,  1638 }, { "and",                2,  1015 },
        { "right",              2,  1564 }, { "and",                2,   997 },
        { "right",              2,  1496 }, { "and",                2,   986 },
        { "right",              2,  1224 }, { "and",                2,   977 },
        { "right",              2,  1140 }, { "and",                2,   950 },
        { "right",              2,  1065 }, { "and",                2,   944 },
        { "right",              2,  1034 }, { "and",                2,   919 },
        { "right",              2,  1025 }, { "and",                2,   915 },
        { "right",              2,  1016 }, { "and",                2,   878 },
        { "right",              2,   979 }, { "and",                2,   867 },
        { "right",              2,   955 }, { "and",                2,   853 },
        { "right",              2,   941 }, { "and",                2,   840 },
        { "right",              2,   873 }, { "and",                2,   827 },
        { "right",              2,   847 }, { "and",                2,   820 },
        { "right",              2,   800 }, { "and",                2,   813 },
        { "right",              2,   794 }, { "and",                2,   810 },
        { "right",              2,   782 }, { "and",                2,   807 },
        { "right",              2,   765 }, { "and",                2,   788 },
        { "right",              2,   729 }, { "and",                2,   761 },
        { "right",              2,   720 }, { "and",                2,   746 },
        { "right",              2,   688 }, { "and",                2,   737 },
        { "right",              2,   683 }, { "and",                2,   726 },
        { "right",              2,   677 }, { "and",                2,   714 },
        { "right",              2,   652 }, { "and",                2,   686 },
        { "right",              2,   637 }, { "and",                2,   676 },
        { "right",              2,   593 }, { "and",                2,   650 },
        { "right",              2,   569 }, { "and",                2,   614 },
        { "right",              2,   551 }, { "and",                2,   581 },
        { "right",              2,   387 }, { "and",                2,   577 },
        { "right",              2,   359 }, { "and",                2,   563 },
        { "right",              1,  1760 }, { "and",                2,   554 },
        { "right",              1,  1592 }, { "and",                2,   546 },
        { "right",              1,  1564 }, { "and",                2,   506 },
        { "right",              1,  1544 }, { "and",                2,   479 },
        { "right",              1,  1443 }, { "and",                2,   454 },
        { "right",              1,  1388 }, { "and",                2,   416 },
        { "right",              1,  1356 }, { "and",                2,   404 },
        { "right",              1,  1334 }, { "and",                2,   395 },
        { "right",              1,  1315 }, { "and",                2,   348 },
        { "right",              1,  1283 }, { "and",                2,   328 },
        { "right",              1,  1270 }, { "and",                2,   317 },
        { "right",              1,  1243 }, { "and",                2,   306 },
        { "right",              1,  1192 }, { "and",                2,   292 },
        { "right",              1,  1126 }, { "and",                2,   279 },
        { "right",              1,  1106 }, { "and",                2,   273 },
        { "right",              1,  1081 }, { "and",                2,   263 },
        { "right",              1,  1051 }, { "and",                2,   238 },
        { "right",              1,  1043 }, { "and",                2,   222 },
        { "right",              1,  1003 }, { "and",                2,   219 },
        { "right",              1,   994 }, { "and",                2,   214 },
        { "right",              1,   967 }, { "and",                2,   193 },
        { "right",              1,   899 }, { "and",                2,   191 },
        { "right",              1,   874 }, { "and",                2,   164 },
        { "right",              1,   857 }, { "and",                2,   162 },
        { "right",              1,   822 }, { "and",                2,   154 },
        { "right",              1,   808 }, { "and",                2,   151 },
        { "right",              1,   788 }, { "and",                2,   143 },
        { "right",              1,   771 }, { "and",                2,   133 },
        { "right",              1,   753 }, { "and",                2,   116 },
        { "right",              1,   640 }, { "and",                2,   101 },
        { "right",              1,   557 }, { "and",                2,    93 },
        { "right",              1,   501 }, { "and",                2,    85 },
        { "right",              1,   447 }, { "and",                2,    63 },
        { "right",              0,  1329 }, { "and",                2,    53 },
        { "right",              0,   439 }, { "and",                2,    36 },
        { "right",              0,   432 }, { "and",                1,  1780 },
        { "right",              0,   270 }, { "and",                1,  1736 },
        { "right",              0,   233 }, { "and",                1,  1724 },
        { "reaffirmed",         1,   150 }, { "and",                1,  1713 },
        { "Charter",            2,  3589 }, { "and",                1,  1703 },
        { "Charter",            2,  3557 }, { "and",                1,  1699 },
        { "Charter",            2,  3483 }, { "and",                1,  1694 },
        { "Charter",            2,  3425 }, { "and",                1,  1671 },
        { "Charter",            2,  3391 }, { "and",                1,  1656 },
        { "Charter",            2,  3333 }, { "and",                1,  1631 },
        { "Charter",            2,  3289 }, { "and",                1,  1624 },
        { "Charter",            2,  3236 }, { "and",                1,  1599 },
        { "Charter",            2,   206 }, { "and",                1,  1585 },
        { "Charter",            2,   204 }, { "and",                1,  1579 },
        { "Charter",            1,   149 }, { "and",                1,  1525 },
        { "genetic",            2,  1356 }, { "and",                1,  1516 },
        { "violating",          1,   569 }, { "and",                1,  1508 },
        { "peoples",            2,   120 }, { "and",                1,  1499 },
        { "peoples",            2,    10 }, { "and",                1,  1472 },
        { "peoples",            1,   322 }, { "and",                1,  1464 },
        { "peoples",            1,   314 }, { "and",                1,  1455 },
        { "peoples",            1,   260 }, { "and",                1,  1420 },
        { "peoples",            1,   141 }, { "and",                1,  1413 },
        { "nations",            2,  3155 }, { "and",                1,  1386 },
        { "nations",            1,  1520 }, { "and",                1,  1382 },
        { "nations",            1,   263 }, { "and",                1,  1379 },
        { "nations",            1,   138 }, { "and",                1,  1371 },
        { "relations",          1,   136 }, { "and",                1,  1366 },
        { "friendly",           1,   135 }, { "and",                1,  1345 },
        { "development",        2,  2296 }, { "and",                1,  1337 },
        { "development",        2,  1955 }, { "and",                1,  1318 },
        { "development",        2,   153 }, { "and",                1,  1301 },
        { "development",        2,   104 }, { "and",                1,  1292 },
        { "development",        1,  1658 }, { "and",                1,  1286 },
        { "development",        1,  1494 }, { "and",                1,  1258 },
        { "development",        1,  1233 }, { "and",                1,  1253 },
        { "development",        1,   133 }, { "and",                1,  1230 },
        { "promote",            2,  3277 }, { "and",                1,  1223 },
        { "promote",            2,  2250 }, { "and",                1,  1214 },
        { "promote",            2,   149 }, { "and",                1,  1208 },
        { "promote",            1,  1513 }, { "and",                1,  1204 },
        { "promote",            1,   288 }, { "and",                1,  1196 },
        { "promote",            1,   179 }, { "and",                1,  1169 },
        { "promote",            1,   131 }, { "and",                1,  1166 },
        { "law",                2,  3510 }, { "and",                1,  1158 },
        { "law",                2,  3507 }, { "and",                1,  1087 },
        { "law",                2,  3470 }, { "and",                1,  1071 },
        { "law",                2,  3339 }, { "and",                1,  1066 },
        { "law",                2,  3266 }, { "and",                1,  1063 },
        { "law",                2,  3223 }, { "and",                1,  1059 },
        { "law",                2,  3106 }, { "and",                1,  1048 },
        { "law",                2,  3068 }, { "and",                1,  1036 },
        { "law",                2,  3065 }, { "and",                1,  1021 },
        { "law",                2,  3008 }, { "and",                1,  1012 },
        { "law",                2,  2946 }, { "and",                1,  1000 },
        { "law",                2,  2898 }, { "and",                1,   958 },
        { "law",                2,  2151 }, { "and",                1,   951 },
        { "law",                2,  2101 }, { "and",                1,   945 },
        { "law",                2,  2072 }, { "and",                1,   932 },
        { "law",                2,  1810 }, { "and",                1,   918 },
        { "law",                2,  1732 }, { "and",                1,   902 },
        { "law",                2,  1706 }, { "and",                1,   883 },
        { "law",                2,  1335 }, { "and",                1,   845 },
        { "law",                2,  1329 }, { "and",                1,   811 },
        { "law",                2,  1201 }, { "and",                1,   796 },
        { "law",                2,  1180 }, { "and",                1,   776 },
        { "law",                2,  1123 }, { "and",                1,   748 },
        { "law",                2,   633 }, { "and",                1,   622 },
        { "law",                2,   430 }, { "and",                1,   620 },
        { "law",                2,   282 }, { "and",                1,   611 },
        { "law",                2,    67 }, { "and",                1,   605 },
        { "law",                1,  1685 }, { "and",                1,   545 },
        { "law",                1,   759 }, { "and",                1,   519 },
        { "law",                1,   696 }, { "and",                1,   467 },
        { "law",                1,   650 }, { "and",                1,   451 },
        { "law",                1,   580 }, { "and",                1,   369 },
        { "law",                1,   530 }, { "and",                1,   349 },
        { "law",                1,   518 }, { "and",                1,   347 },
        { "law",                1,   510 }, { "and",                1,   340 },
        { "law",                1,   125 }, { "and",                1,   336 },
        { "legislate",          0,   875 }, { "and",                1,   319 },
        { "rebellion",          1,   110 }, { "and",                1,   309 },
        { "resort",             1,   108 }, { "and",                1,   306 },
        { "part",               1,  1109 }, { "and",                1,   300 },
        { "34",                 2,  2020 }, { "and",                1,   295 },
        { "29",                 2,  1769 }, { "and",                1,   293 },
        { "29",                 1,  1643 }, { "and",                1,   285 },
        { "last",               1,   107 }, { "and",                1,   270 },
        { "recourse",           1,   104 }, { "and",                1,   261 },
        { "so",                 2,  3421 }, { "and",                1,   226 },
        { "so",                 2,  2971 }, { "and",                1,   216 },
        { "so",                 2,  2128 }, { "and",                1,   211 },
        { "so",                 2,  1203 }, { "and",                1,   182 },
        { "so",                 0,   822 }, { "and",                1,   175 },
        { "so",                 0,   400 }, { "and",                1,   173 },
        { "compelled",          1,  1094 }, { "and",                1,   166 },
        { "compelled",          1,   101 }, { "and",                1,   160 },
        { "aspiration",         1,    86 }, { "and",                1,   113 },
        { "highest",            1,    85 }, { "and",                1,    78 },
        { "want",               1,    79 }, { "and",                1,    74 },
        { "fear",               1,    77 }, { "and",                1,    72 },
        { "belief",             2,  1361 }, { "and",                1,    57 },
        { "belief",             2,   756 }, { "and",                1,    40 },
        { "belief",             2,   736 }, { "and",                1,    33 },
        { "belief",             1,  1031 }, { "and",                1,    17 },
        { "belief",             1,  1011 }, { "and",                1,    13 },
        { "belief",             1,    73 }, { "and",                0,  1358 },
        { "beings",             2,   535 }, { "and",                0,  1322 },
        { "beings",             2,   473 }, { "and",                0,  1316 },
        { "beings",             1,   332 }, { "and",                0,  1300 },
        { "beings",             1,    66 }, { "and",                0,  1296 },
        { "advent",             1,    59 }, { "and",                0,  1290 },
        { "conscience",         2,   725 }, { "and",                0,  1283 },
        { "conscience",         2,   713 }, { "and",                0,  1276 },
        { "conscience",         1,   999 }, { "and",                0,  1262 },
        { "conscience",         1,   348 }, { "and",                0,  1255 },
        { "conscience",         1,    54 }, { "and",                0,  1248 },
        { "outraged",           1,    52 }, { "and",                0,  1236 },
        { "acts",               1,   840 }, { "and",                0,  1188 },
        { "acts",               1,   568 }, { "and",                0,  1174 },
        { "acts",               1,    49 }, { "and",                0,  1162 },
        { "resulted",           1,    46 }, { "and",                0,  1140 },
        { "contempt",           1,    41 }, { "and",                0,  1138 },
        { "basis",              2,  1570 }, { "and",                0,  1128 },
        { "basis",              2,   629 }, { "and",                0,  1034 },
        { "basis",              2,   617 }, { "and",                0,  1006 },
        { "basis",              1,  1483 }, { "and",                0,   989 },
        { "basis",              1,  1145 }, { "and",                0,   954 },
        { "basis",              1,   409 }, { "and",                0,   937 },
        { "every",              2,  2509 }, { "and",                0,   912 },
        { "every",              2,  2487 }, { "and",                0,   895 },
        { "every",              1,   271 }, { "and",                0,   868 },
        { "every",              1,   268 }, { "and",                0,   854 },
        { "every",              0,  1070 }, { "and",                0,   831 },
        { "every",              0,  1037 }, { "and",                0,   818 },
        { "thirteen",           0,    10 }, { "and",                0,   708 },
        { "disregard",          1,    39 }, { "and",                0,   686 },
        { "detention",          1,   591 }, { "and",                0,   653 },
        { "impart",             2,   811 }, { "and",                0,   643 },
        { "impart",             1,  1064 }, { "and",                0,   629 },
        { "presumed",           2,  3001 }, { "and",                0,   626 },
        { "presumed",           1,   643 }, { "and",                0,   583 },
        { "personality",        1,  1661 }, { "and",                0,   550 },
        { "personality",        1,  1498 }, { "and",                0,   459 },
        { "personality",        1,  1236 }, { "and",                0,   443 },
        { "HUMAN",              1,   250 }, { "and",                0,   398 },
        { "men",                2,  1446 }, { "and",                0,   384 },
        { "men",                2,  1441 }, { "and",                0,   367 },
        { "men",                1,   172 }, { "and",                0,   329 },
        { "men",                0,    96 }, { "and",                0,   298 },
        { "illness",            2,  2048 }, { "and",                0,   280 },
        { "freedom",            2,  1114 }, { "and",                0,   251 },
        { "freedom",            2,  1056 }, { "and",                0,   214 },
        { "freedom",            2,  1014 }, { "and",                0,   211 },
        { "freedom",            2,   966 }, { "and",                0,   195 },
        { "freedom",            2,   928 }, { "and",                0,   177 },
        { "freedom",            2,   855 }, { "and",                0,   166 },
        { "freedom",            2,   849 }, { "and",                0,   117 },
        { "freedom",            2,   826 }, { "and",                0,    59 },
        { "freedom",            2,   803 }, { "and",                0,    50 },
        { "freedom",            2,   796 }, { "and",                0,    39 },
        { "freedom",            2,   738 }, { "with",               2,  3283 },
        { "freedom",            2,   731 }, { "with",               2,  3247 },
        { "freedom",            2,   722 }, { "with",               2,  3221 },
        { "freedom",            2,   166 }, { "with",               2,  2916 },
        { "freedom",            2,    91 }, { "with",               2,  2794 },
        { "freedom",            2,    51 }, { "with",               2,  2701 },
        { "freedom",            1,  1083 }, { "with",               2,  2574 },
        { "freedom",            1,  1053 }, { "with",               2,  2291 },
        { "freedom",            1,  1045 }, { "with",               2,  2240 },
        { "freedom",            1,  1013 }, { "with",               2,  2144 },
        { "freedom",            1,  1005 }, { "with",               2,  2099 },
        { "freedom",            1,   996 }, { "with",               2,  2065 },
        { "freedom",            1,   773 }, { "with",               2,  1998 },
        { "freedom",            1,   189 }, { "with",               2,  1959 },
        { "freedom",            1,    75 }, { "with",               2,  1808 },
        { "freedom",            1,    69 }, { "with",               2,  1730 },
        { "freedom",            1,    31 }, { "with",               2,  1641 },
        { "laws",               2,  2581 }, { "with",               2,  1630 },
        { "laws",               2,  2235 }, { "with",               2,  1577 },
        { "laws",               2,  2185 }, { "with",               2,  1529 },
        { "laws",               2,  2154 }, { "with",               2,  1260 },
        { "laws",               2,  2104 }, { "with",               2,  1230 },
        { "laws",               2,  2075 }, { "with",               2,  1121 },
        { "laws",               2,  1813 }, { "with",               2,  1005 },
        { "laws",               2,  1735 }, { "with",               2,   993 },
        { "laws",               2,  1709 }, { "with",               2,   971 },
        { "laws",               2,  1126 }, { "with",               2,   773 },
        { "laws",               2,  1008 }, { "with",               2,   744 },
        { "laws",               2,   776 }, { "with",               2,   698 },
        { "laws",               2,   701 }, { "with",               2,   659 },
        { "laws",               0,   712 }, { "with",               2,   308 },
        { "pursue",             2,  1041 }, { "with",               2,   208 },
        { "family",             2,  1981 }, { "with",               1,  1348 },
        { "family",             2,  1970 }, { "with",               1,  1211 },
        { "family",             2,   692 }, { "with",               1,  1019 },
        { "family",             2,   681 }, { "with",               1,   977 },
        { "family",             2,   578 }, { "with",               1,   929 },
        { "family",             2,   564 }, { "with",               1,   735 },
        { "family",             1,  1374 }, { "with",               1,   634 },
        { "family",             1,  1294 }, { "with",               1,   345 },
        { "family",             1,   941 }, { "with",               1,   201 },
        { "family",             1,   906 }, { "with",               0,  1338 },
        { "family",             1,   738 }, { "with",               0,   941 },
        { "family",             1,    26 }, { "with",               0,   872 },
        { "members",            1,    22 }, { "with",               0,   760 },
        { "Oppressions",        0,  1041 }, { "with",               0,   696 },
        { "guilty",             2,  3044 }, { "with",               0,   489 },
        { "guilty",             2,  3005 }, { "with",               0,   477 },
        { "guilty",             1,   673 }, { "with",               0,   107 },
        { "guilty",             1,   647 }, { "with",               0,    37 },
        { "inalienable",        1,    18 }, { "criminal",           2,  3199 },
        { "British",            0,  1274 }, { "criminal",           2,  3185 },
        { "will",               2,   902 }, { "criminal",           2,  3180 },
        { "will",               1,  1152 }, { "criminal",           2,  3167 },
        { "will",               1,  1138 }, { "criminal",           2,  3144 },
        { "will",               0,   199 }, { "criminal",           2,  3103 },
        { "disability",         2,  1374 }, { "criminal",           2,  3092 },
        { "disability",         1,  1397 }, { "criminal",           2,  3061 },
        { "inherent",           1,    11 }, { "criminal",           2,  3047 },
        { "Whereas",            1,   219 }, { "criminal",           2,  3034 },
        { "Whereas",            1,   190 }, { "criminal",           1,   625 },
        { "Whereas",            1,   139 }, { "provide",            2,  1070 },
        { "Whereas",            1,   126 }, { "provide",            0,   282 },
        { "Whereas",            1,    91 }, { "people",             2,  1922 },
        { "Whereas",            1,    38 }, { "people",             2,  1915 },
        { "Whereas",            1,     7 }, { "people",             2,  1877 },
        { "Preamble",           1,     6 }, { "people",             1,  1141 },
        { "Universal",          1,     1 }, { "people",             1,    90 },
        { "sacred",             0,  1360 }, { "people",             0,  1086 },
        { "Lives",              0,  1355 }, { "people",             0,   918 },
        { "pledge",             1,   239 }, { "people",             0,   652 },
        { "pledge",             0,  1350 }, { "people",             0,   499 },
        { "mutually",           0,  1349 }, { "people",             0,   428 },
        { "Providence",         0,  1347 }, { "people",             0,   425 },
        { "divine",             0,  1346 }, { "people",             0,    27 },
        { "Armies",             0,   926 }, { "wholesome",          0,   366 },
        { "Armies",             0,   669 }, { "away",               0,   846 },
        { "And",                0,  1331 }, { "groups",             1,  1524 },
        { "Things",             0,  1323 }, { "purpose",            1,  1689 },
        { "Commerce",           0,  1315 }, { "purpose",            0,   566 },
        { "belong",             1,  1096 }, { "purpose",            0,   471 },
        { "establish",          2,  3292 }, { "duty",               0,   274 },
        { "establish",          0,  1314 }, { "unless",             2,  1583 },
        { "contract",           0,  1312 }, { "unless",             0,   426 },
        { "conclude",           2,  1743 }, { "unless",             0,   387 },
        { "conclude",           0,  1310 }, { "truths",             0,    89 },
        { "life",               2,  1984 }, { "fair",               2,  2929 },
        { "life",               2,  1967 }, { "fair",               2,  2888 },
        { "life",               2,  1660 }, { "fair",               2,  1183 },
        { "life",               2,  1624 }, { "fair",               1,   604 },
        { "life",               2,  1612 }, { "earth",              0,    47 },
        { "life",               2,   579 }, { "Absolved",           0,  1268 },
        { "life",               2,   565 }, { "Men",                1,   882 },
        { "life",               2,   361 }, { "Men",                0,   131 },
        { "life",               2,   354 }, { "disposed",           0,   224 },
        { "life",               1,  1571 }, { "same",               2,  3455 },
        { "life",               1,   449 }, { "same",               2,  3184 },
        { "life",               1,   186 }, { "same",               2,  2866 },
        { "levy",               0,  1308 }, { "same",               2,  2612 },
        { "Power",              0,  1306 }, { "same",               2,  2438 },
        { "full",               1,  1657 }, { "same",               2,  2367 },
        { "full",               1,  1493 }, { "same",               1,  1434 },
        { "full",               1,   933 }, { "same",               0,   837 },
        { "full",               1,   886 }, { "same",               0,   256 },
        { "full",               1,   600 }, { "compulsory",         2,   962 },
        { "full",               1,   235 }, { "compulsory",         2,   529 },
        { "full",               0,  1305 }, { "compulsory",         1,  1462 },
        { "between",            2,  1445 }, { "station",            0,    52 },
        { "between",            2,  1440 }, { "foundation",         1,    29 },
        { "between",            1,   137 }, { "foundation",         0,   173 },
        { "between",            0,  1281 }, { "information",        2,  1689 },
        { "pursuing",           0,   253 }, { "information",        2,  1672 },
        { "connection",         0,  1280 }, { "information",        2,   812 },
        { "Crown",              0,  1275 }, { "information",        2,   789 },
        { "Independent",        0,  1325 }, { "information",        1,  1065 },
        { "Independent",        0,  1301 }, { "these",              2,   706 },
        { "Independent",        0,  1263 }, { "these",              2,   660 },
        { "ought",              0,  1291 }, { "these",              2,   302 },
        { "ought",              0,  1258 }, { "these",              2,   106 },
        { "fit",                0,   832 }, { "these",              1,   291 },
        { "publish",            0,  1247 }, { "these",              1,   224 },
        { "take",               2,  1758 }, { "these",              0,  1251 },
        { "take",               1,  1108 }, { "these",              0,  1244 },
        { "solemnly",           0,  1246 }, { "these",              0,  1154 },
        { "Authority",          0,  1238 }, { "these",              0,  1040 },
        { "Name",               0,  1235 }, { "these",              0,   841 },
        { "Judge",              0,  1222 }, { "these",              0,   752 },
        { "before",             2,  2911 }, { "these",              0,   562 },
        { "before",             2,  2492 }, { "these",              0,   343 },
        { "before",             2,  1333 }, { "these",              0,   296 },
        { "before",             2,  1327 }, { "these",              0,   151 },
        { "before",             1,   516 }, { "these",              0,   125 },
        { "before",             1,   508 }, { "these",              0,   113 },
        { "social",             2,  2252 }, { "these",              0,    88 },
        { "social",             2,  2124 }, { "consanguinity",      0,  1176 },
        { "social",             2,  2112 }, { "abuses",             0,   250 },
        { "social",             2,  2095 }, { "here",               0,  1130 },
        { "social",             2,  2091 }, { "here",               0,   887 },
        { "social",             2,  2039 }, { "requires",           0,    73 },
        { "social",             2,  2035 }, { "Nature's",           0,    61 },
        { "social",             2,  2024 }, { "oppression",         1,   114 },
        { "social",             2,  1976 }, { "Laws",               0,   853 },
        { "social",             2,  1954 }, { "Laws",               0,   808 },
        { "social",             2,  1652 }, { "Laws",               0,   605 },
        { "social",             2,  1621 }, { "Laws",               0,   569 },
        { "social",             2,  1354 }, { "Laws",               0,   417 },
        { "social",             2,   189 }, { "Laws",               0,   381 },
        { "social",             1,  1623 }, { "Laws",               0,   363 },
        { "social",             1,  1435 }, { "Laws",               0,    56 },
        { "social",             1,  1384 }, { "scarcely",           0,   947 },
        { "social",             1,  1309 }, { "been",               2,  3211 },
        { "social",             1,  1222 }, { "been",               2,  3021 },
        { "social",             1,  1194 }, { "been",               2,  2997 },
        { "social",             1,   394 }, { "been",               2,   644 },
        { "social",             1,   180 }, { "been",               1,    81 },
        { "appealing",          0,  1218 }, { "been",               0,  1167 },
        { "Prince",             0,  1063 }, { "been",               0,  1090 },
        { "Assembled",          0,  1217 }, { "been",               0,  1056 },
        { "Congress",           0,  1216 }, { "been",               0,   291 },
        { "General",            0,  1215 }, { "sufferable",         0,   230 },
        { "Representatives",    0,  1207 }, { "Governments",        0,   861 },
        { "Friends",            0,  1203 }, { "Governments",        0,   202 },
        { "extensive",          2,  3473 }, { "Governments",        0,   127 },
        { "Peace",              0,  1311 }, { "instituted",         0,   129 },
        { "Peace",              0,  1202 }, { "exception",          2,  2703 },
        { "Enemies",            0,  1198 }, { "one",                2,  3188 },
        { "rest",               2,  1857 }, { "one",                2,  3040 },
        { "rest",               1,  1336 }, { "one",                2,  2598 },
        { "rest",               0,  1195 }, { "one",                2,  1285 },
        { "Separation",         0,  1187 }, { "one",                2,  1155 },
        { "denounces",          0,  1185 }, { "one",                2,   521 },
        { "acquiesce",          0,  1180 }, { "one",                2,   511 },
        { "emigration",         0,  1127 }, { "one",                2,   487 },
        { "therefore",          2,  3269 }, { "one",                2,   364 },
        { "therefore",          2,   323 }, { "one",                1,  1091 },
        { "therefore",          0,  1205 }, { "one",                1,   981 },
        { "therefore",          0,  1179 }, { "one",                1,   863 },
        { "achieve",            1,   197 }, { "one",                1,   728 },
        { "must",               2,  3334 }, { "one",                1,   713 },
        { "must",               2,  3161 }, { "one",                1,   669 },
        { "must",               2,  2606 }, { "one",                1,   584 },
        { "must",               2,  2278 }, { "one",                1,   481 },
        { "must",               2,  1926 }, { "one",                1,   458 },
        { "must",               2,  1682 }, { "one",                1,   353 },
        { "must",               2,  1553 }, { "one",                0,    26 },
        { "must",               2,  1449 }, { "enjoy",              2,  1972 },
        { "must",               2,   607 }, { "enjoy",              1,  1576 },
        { "must",               2,   408 }, { "enjoy",              1,  1432 },
        { "must",               2,   345 }, { "enjoy",              1,   813 },
        { "must",               0,  1178 }, { "enjoy",              1,    68 },
        { "They",               2,  3267 }, { "erected",            0,   637 },
        { "They",               2,  1509 }, { "institute",          0,   168 },
        { "They",               1,   907 }, { "consent",            2,   620 },
        { "They",               1,   342 }, { "consent",            2,   418 },
        { "They",               0,  1164 }, { "consent",            1,   934 },
        { "opinion",            2,  1366 }, { "consent",            0,   138 },
        { "opinion",            1,  1047 }, { "governed",           0,   141 },
        { "opinion",            1,   391 }, { "&",                  0,   945 },
        { "correspondence",     1,   741 }, { "whenever",           0,   143 },
        { "correspondence",     0,  1163 }, { "times",              0,   665 },
        { "connections",        0,  1161 }, { "opposing",           0,   488 },
        { "spirit",             1,   357 }, { "world",              1,    62 },
        { "disavow",            0,  1153 }, { "world",              1,    37 },
        { "whether",            2,  1541 }, { "world",              0,  1225 },
        { "whether",            1,  1424 }, { "world",              0,   765 },
        { "whether",            1,   427 }, { "world",              0,   355 },
        { "kindred",            0,  1151 }, { "relinquish",         0,   430 },
        { "meaning",            2,  3446 }, { "Government",         0,   886 },
        { "common",             2,  2578 }, { "Government",         0,   313 },
        { "common",             2,   241 }, { "Government",         0,   279 },
        { "common",             2,   107 }, { "Government",         0,   170 },
        { "common",             2,    30 }, { "Government",         0,   147 },
        { "common",             1,   254 }, { "We",                 0,  1204 },
        { "common",             1,   221 }, { "We",                 0,  1177 },
        { "common",             1,    89 }, { "We",                 0,  1131 },
        { "common",             0,  1150 }, { "We",                 0,  1118 },
        { "ties",               0,  1147 }, { "We",                 0,  1098 },
        { "treatment",          2,  2178 }, { "We",                 0,  1089 },
        { "treatment",          2,  1318 }, { "We",                 0,  1042 },
        { "treatment",          2,   498 }, { "We",                 0,    86 },
        { "treatment",          2,   483 }, { "enlarging",          0,   819 },
        { "treatment",          1,   493 }, { "alter",              0,   308 },
        { "conjured",           0,  1143 }, { "alter",              0,   161 },
        { "magnanimity",        0,  1139 }, { "organizing",         0,   178 },
        { "justice",            2,  2983 }, { "medical",            2,  2177 },
        { "justice",            2,    94 }, { "medical",            1,  1380 },
        { "justice",            1,    32 }, { "competent",          1,   564 },
        { "justice",            0,  1173 }, { "abolish",            0,   164 },
        { "justice",            0,  1137 }, { "changed",            0,   208 },
        { "native",             0,  1136 }, { "Fortunes",           0,  1357 },
        { "appealed",           0,  1133 }, { "research",           2,   921 },
        { "settlement",         0,  1129 }, { "Declaration",        1,  1747 },
        { "end",                2,   171 }, { "Declaration",        1,  1637 },
        { "end",                1,   266 }, { "Declaration",        1,   544 },
        { "reminded",           0,  1120 }, { "Declaration",        1,   375 },
        { "unwarrantable",      0,  1114 }, { "Declaration",        1,   277 },
        { "interrupt",          0,  1159 }, { "Declaration",        1,     2 },
        { "extend",             0,  1112 }, { "Declaration",        0,  1337 },
        { "throw",              0,   276 }, { "Declaration",        0,     7 },
        { "attempts",           0,  1107 }, { "importance",         1,   232 },
        { "deaf",               0,  1168 }, { "importance",         0,   386 },
        { "warned",             0,  1100 }, { "endowed",            1,   344 },
        { "brethren",           0,  1097 }, { "endowed",            0,   103 },
        { "Alliances",          0,  1313 }, { "as",                 2,  3561 },
        { "Brittish",           0,  1096 }, { "as",                 2,  3497 },
        { "attentions",         0,  1093 }, { "as",                 2,  3487 },
        { "Nor",                2,  3076 }, { "as",                 2,  3456 },
        { "Nor",                1,   704 }, { "as",                 2,  3423 },
        { "Nor",                0,  1087 }, { "as",                 2,  2973 },
        { "unfit",              0,  1078 }, { "as",                 2,  2868 },
        { "Tyrant",             0,  1076 }, { "as",                 2,  2440 },
        { "define",             0,  1074 }, { "as",                 2,  2420 },
        { "sustainable",        2,  2295 }, { "as",                 2,  2401 },
        { "sustainable",        2,   152 }, { "as",                 2,  2369 },
        { "may",                2,  3356 }, { "as",                 2,  2346 },
        { "may",                2,  2789 }, { "as",                 2,  2323 },
        { "may",                2,  2589 }, { "as",                 2,  2230 },
        { "may",                2,  1909 }, { "as",                 2,  2129 },
        { "may",                2,  1893 }, { "as",                 2,  2046 },
        { "may",                2,  1510 }, { "as",                 2,  1908 },
        { "may",                2,  1286 }, { "as",                 2,  1502 },
        { "may",                2,  1197 }, { "as",                 2,  1348 },
        { "may",                2,  1156 }, { "as",                 2,  1205 },
        { "may",                1,  1748 }, { "as",                 2,   457 },
        { "may",                1,  1726 }, { "as",                 2,   229 },
        { "may",                1,  1092 }, { "as",                 2,   125 },
        { "may",                1,   823 }, { "as",                 2,   123 },
        { "may",                0,  1327 }, { "as",                 1,  1751 },
        { "may",                0,  1073 }, { "as",                 1,  1681 },
        { "act",                2,  3574 }, { "as",                 1,  1185 },
        { "act",                2,  3132 }, { "as",                 1,   974 },
        { "act",                2,  3053 }, { "as",                 1,   972 },
        { "act",                1,  1770 }, { "as",                 1,   913 },
        { "act",                1,   682 }, { "as",                 1,   505 },
        { "act",                1,   351 }, { "as",                 1,   382 },
        { "act",                0,  1071 }, { "as",                 1,   252 },
        { "marked",             0,  1068 }, { "as",                 1,   105 },
        { "thus",               0,  1067 }, { "as",                 1,    83 },
        { "essential",          1,   129 }, { "as",                 0,  1298 },
        { "essential",          1,    94 }, { "as",                 0,  1191 },
        { "character",          0,  1065 }, { "as",                 0,   823 },
        { "answered",           0,  1057 }, { "as",                 0,   184 },
        { "Petitions",          0,  1054 }, { "new",                2,  3294 },
        { "Presumption",        2,  2986 }, { "new",                0,   588 },
        { "Our",                0,  1052 }, { "new",                0,   283 },
        { "humble",             0,  1050 }, { "new",                0,   169 },
        { "held",               2,  3043 }, { "change",             2,   733 },
        { "held",               2,   514 }, { "change",             1,  1007 },
        { "held",               1,  1172 }, { "change",             1,   876 },
        { "held",               1,   672 }, { "laying",             0,   171 },
        { "held",               1,   461 }, { "just",               2,  1820 },
        { "elected",            2,  2382 }, { "just",               1,  1707 },
        { "elected",            0,   516 }, { "just",               1,  1285 },
        { "dissolutions",       0,   510 }, { "just",               1,  1252 },
        { "time",               2,  3138 }, { "just",               0,   134 },
        { "time",               2,  3090 }, { "man",                1,    96 },
        { "time",               2,  3071 }, { "its",                2,  2739 },
        { "time",               2,  2936 }, { "its",                2,  2671 },
        { "time",               2,  2470 }, { "its",                2,  2635 },
        { "time",               2,  1694 }, { "its",                2,  2564 },
        { "time",               2,  1189 }, { "its",                2,  2560 },
        { "time",               1,   719 }, { "its",                2,  2542 },
        { "time",               1,   699 }, { "its",                2,   455 },
        { "time",               0,  1105 }, { "its",                2,    76 },
        { "time",               0,  1103 }, { "its",                2,    34 },
        { "time",               0,   923 }, { "its",                1,  1586 },
        { "time",               0,   540 }, { "its",                1,   920 },
        { "time",               0,   507 }, { "its",                0,   820 },
        { "invasions",          0,   493 }, { "its",                0,   179 },
        { "firmness",           0,   491 }, { "its",                0,   172 },
        { "manly",              0,   490 }, { "waging",             0,   896 },
        { "dissolved",          0,  1295 }, { "move",               2,  2772 },
        { "dissolved",          0,   483 }, { "local",              2,   144 },
        { "measures",           2,  1646 }, { "most",               0,  1049 },
        { "measures",           2,  1472 }, { "most",               0,   951 },
        { "measures",           1,   298 }, { "most",               0,   851 },
        { "measures",           0,   479 }, { "most",               0,   365 },
        { "compliance",         2,  2915 }, { "most",               0,   189 },
        { "compliance",         0,   476 }, { "Prudence",           0,   197 },
        { "Records",            0,   467 }, { "has",                2,  3209 },
        { "effect",             0,   192 }, { "has",                2,  3020 },
        { "having",             2,  2738 }, { "has",                2,  2996 },
        { "having",             2,  2670 }, { "has",                2,  2904 },
        { "having",             2,  2634 }, { "has",                2,  2768 },
        { "having",             0,   332 }, { "has",                2,  2746 },
        { "depository",         0,   463 }, { "has",                2,  2678 },
        { "repeatedly",         0,   486 }, { "has",                2,  2642 },
        { "King",               0,   319 }, { "has",                2,  2547 },
        { "2",                  2,  3386 }, { "has",                2,  2454 },
        { "2",                  2,  3287 }, { "has",                2,  2412 },
        { "2",                  2,  3117 }, { "has",                2,  2338 },
        { "2",                  2,  3009 }, { "has",                2,  2162 },
        { "2",                  2,  2783 }, { "has",                2,  1844 },
        { "2",                  2,  2479 }, { "has",                2,  1826 },
        { "2",                  2,  2374 }, { "has",                2,  1798 },
        { "2",                  2,  2078 }, { "has",                2,  1777 },
        { "2",                  2,  1978 }, { "has",                2,  1138 },
        { "2",                  2,  1841 }, { "has",                2,  1054 },
        { "2",                  2,  1534 }, { "has",                2,  1032 },
        { "2",                  2,  1382 }, { "has",                2,   939 },
        { "2",                  2,  1283 }, { "has",                2,   845 },
        { "2",                  2,  1212 }, { "has",                2,   792 },
        { "2",                  2,  1048 }, { "has",                2,   718 },
        { "2",                  2,   953 }, { "has",                2,   643 },
        { "2",                  2,   891 }, { "has",                2,   635 },
        { "2",                  2,   824 }, { "has",                2,   591 },
        { "2",                  2,   763 }, { "has",                2,   567 },
        { "2",                  2,   604 }, { "has",                2,   549 },
        { "2",                  2,   519 }, { "has",                2,   385 },
        { "2",                  2,   398 }, { "has",                2,   357 },
        { "2",                  2,   362 }, { "has",                1,  1646 },
        { "2",                  2,   351 }, { "has",                1,  1590 },
        { "2",                  1,  1664 }, { "has",                1,  1562 },
        { "2",                  1,  1588 }, { "has",                1,  1441 },
        { "2",                  1,  1486 }, { "has",                1,  1354 },
        { "2",                  1,  1411 }, { "has",                1,  1332 },
        { "2",                  1,  1263 }, { "has",                1,  1313 },
        { "2",                  1,  1122 }, { "has",                1,  1281 },
        { "2",                  1,  1089 }, { "has",                1,  1268 },
        { "2",                  1,   979 }, { "has",                1,  1241 },
        { "2",                  1,   922 }, { "has",                1,  1190 },
        { "2",                  1,   861 }, { "has",                1,  1124 },
        { "2",                  1,   820 }, { "has",                1,  1104 },
        { "2",                  1,   784 }, { "has",                1,  1079 },
        { "2",                  1,   667 }, { "has",                1,  1041 },
        { "2",                  1,   361 }, { "has",                1,   992 },
        { "unusual",            0,   457 }, { "has",                1,   965 },
        { "at",                 2,  3591 }, { "has",                1,   855 },
        { "at",                 2,  3576 }, { "has",                1,   806 },
        { "at",                 2,  3136 }, { "has",                1,   786 },
        { "at",                 2,  3088 }, { "has",                1,   769 },
        { "at",                 2,  3069 }, { "has",                1,   751 },
        { "at",                 2,  2423 }, { "has",                1,   658 },
        { "at",                 2,  2404 }, { "has",                1,   638 },
        { "at",                 2,  2349 }, { "has",                1,   555 },
        { "at",                 2,  2326 }, { "has",                1,   499 },
        { "at",                 2,  1878 }, { "has",                1,   445 },
        { "at",                 2,  1746 }, { "has",                1,    80 },
        { "at",                 2,  1683 }, { "has",                0,  1007 },
        { "at",                 2,   894 }, { "has",                0,  1000 },
        { "at",                 2,   858 }, { "has",                0,   965 },
        { "at",                 2,   441 }, { "has",                0,   902 },
        { "at",                 2,   140 }, { "has",                0,   884 },
        { "at",                 2,    72 }, { "has",                0,   694 },
        { "at",                 1,  1772 }, { "has",                0,   678 },
        { "at",                 1,  1450 }, { "has",                0,   660 },
        { "at",                 1,   919 }, { "has",                0,   636 },
        { "at",                 1,   717 }, { "has",                0,   612 },
        { "at",                 1,   697 }, { "has",                0,   594 },
        { "at",                 1,   655 }, { "has",                0,   555 },
        { "at",                 0,   921 }, { "has",                0,   502 },
        { "at",                 0,   827 }, { "has",                0,   482 },
        { "at",                 0,   529 }, { "has",                0,   450 },
        { "at",                 0,   455 }, { "has",                0,   412 },
        { "A",                  2,  2263 }, { "has",                0,   403 },
        { "A",                  2,  2188 }, { "has",                0,   375 },
        { "A",                  0,  1062 }, { "has",                0,   358 },
        { "whereby",            0,   517 }, { "has",                0,   290 },
        { "each",               1,  1217 }, { "indeed",             0,   198 },
        { "each",               1,   782 }, { "brotherhood",        1,   359 },
        { "each",               0,  1352 }, { "dictate",            0,   200 },
        { "bodies",             2,  3243 }, { "accordingly",        0,   215 },
        { "bodies",             2,  2700 }, { "advancement",        1,  1584 },
        { "bodies",             2,  2475 }, { "suffer",             0,   226 },
        { "bodies",             0,   726 }, { "altering",           0,   855 },
        { "bodies",             0,   454 }, { "speech",             1,    71 },
        { "his",                2,  2515 }, { "long",               0,   506 },
        { "his",                2,  2459 }, { "long",               0,   247 },
        { "his",                2,  1834 }, { "long",               0,   203 },
        { "his",                2,  1588 }, { "firm",               0,  1340 },
        { "his",                2,  1579 }, { "Safety",             0,   194 },
        { "his",                2,  1160 }, { "united",             0,  1210 },
        { "his",                2,  1148 }, { "united",             0,    11 },
        { "his",                2,   887 }, { "worth",              1,   161 },
        { "his",                2,   573 }, { "abolishing",         0,   849 },
        { "his",                2,   391 }, { "abolishing",         0,   802 },
        { "his",                1,  1669 }, { "abolishing",         0,   236 },
        { "his",                1,  1660 }, { "accustomed",         0,   243 },
        { "his",                1,  1409 }, { "upon",               1,   745 },
        { "his",                1,  1373 }, { "when",               2,  3261 },
        { "his",                1,  1327 }, { "when",               2,  3139 },
        { "his",                1,  1293 }, { "when",               2,  3072 },
        { "his",                1,  1235 }, { "when",               1,   700 },
        { "his",                1,  1228 }, { "when",               0,   399 },
        { "his",                1,  1134 }, { "when",               0,   245 },
        { "his",                1,  1114 }, { "terms",              0,  1051 },
        { "his",                1,  1028 }, { "incapable",          0,   521 },
        { "his",                1,  1008 }, { "purposes",           2,   613 },
        { "his",                1,   987 }, { "purposes",           1,  1735 },
        { "his",                1,   877 }, { "purposes",           1,   844 },
        { "his",                1,   869 }, { "penalty",            2,  3113 },
        { "his",                1,   800 }, { "penalty",            2,  3111 },
        { "his",                1,   794 }, { "penalty",            2,  3080 },
        { "his",                1,   746 }, { "penalty",            2,  1311 },
        { "his",                1,   736 }, { "penalty",            2,   371 },
        { "his",                1,   665 }, { "penalty",            1,   708 },
        { "his",                1,   618 }, { "fellow",             0,   968 },
        { "his",                0,   893 }, { "returned",           0,   525 },
        { "his",                0,   714 }, { "person",             2,  3129 },
        { "his",                0,   617 }, { "person",             2,  2735 },
        { "his",                0,   602 }, { "person",             2,  2667 },
        { "his",                0,   492 }, { "person",             2,  2631 },
        { "his",                0,   478 }, { "person",             2,  2588 },
        { "his",                0,   393 }, { "person",             2,  2546 },
        { "his",                0,   377 }, { "person",             2,  2510 },
        { "his",                0,   360 }, { "person",             2,  2488 },
        { "Supreme",            0,  1221 }, { "person",             2,  2453 },
        { "slavery",            2,   516 }, { "person",             2,   623 },
        { "slavery",            2,   505 }, { "person",             2,   557 },
        { "slavery",            1,   466 }, { "person",             2,   421 },
        { "slavery",            1,   463 }, { "person",             2,   382 },
        { "legislative",        0,   453 }, { "person",             1,  1758 },
        { "together",           0,   452 }, { "person",             1,   507 },
        { "voice",              0,  1171 }, { "person",             1,   454 },
        { "called",             0,   451 }, { "person",             1,   425 },
        { "participate",        2,  1619 }, { "person",             1,   165 },
        { "participate",        1,  1567 }, { "exercise",           2,  3324 },
        { "women",              2,  1448 }, { "exercise",           2,  1063 },
        { "women",              2,  1443 }, { "exercise",           2,  1011 },
        { "women",              1,   884 }, { "exercise",           2,   779 },
        { "women",              1,   174 }, { "exercise",           2,   704 },
        { "districts",          0,   423 }, { "exercise",           1,  1667 },
        { "traditions",         2,   237 }, { "exercise",           0,   533 },
        { "traditions",         2,   117 }, { "remaining",          0,   536 },
        { "large",              0,   925 }, { "Trade",              0,   759 },
        { "large",              0,   725 }, { "protected",          2,  1936 },
        { "large",              0,   530 }, { "protected",          2,  1217 },
        { "large",              0,   422 }, { "protected",          2,   349 },
        { "Representative",     0,   484 }, { "protected",          1,   120 },
        { "accommodation",      0,   420 }, { "mean",               0,   539 },
        { "giving",             0,   713 }, { "sciences",           2,   916 },
        { "rectitude",          0,  1228 }, { "But",                0,   244 },
        { "other",              2,  1365 }, { "exposed",            0,   541 },
        { "other",              2,  1314 }, { "reason",             2,  1996 },
        { "other",              2,   627 }, { "reason",             1,   346 },
        { "other",              2,   311 }, { "When",               0,    15 },
        { "other",              1,  1402 }, { "dangers",            0,   545 },
        { "other",              1,  1306 }, { "engage",             2,  3566 },
        { "other",              1,   815 }, { "engage",             2,  1036 },
        { "other",              1,   438 }, { "engage",             2,  1027 },
        { "other",              1,   399 }, { "engage",             1,  1762 },
        { "other",              1,   390 }, { "invasion",           0,   547 },
        { "other",              0,  1353 }, { "substance",          0,   657 },
        { "other",              0,  1320 }, { "hither",             0,   645 },
        { "other",              0,   416 }, { "hither",             0,   582 },
        { "powers",             2,  3306 }, { "commit",             0,   747 },
        { "powers",             2,  3286 }, { "without",            2,  1903 },
        { "powers",             2,   213 }, { "without",            2,  1403 },
        { "powers",             0,   609 }, { "without",            2,   815 },
        { "powers",             0,   520 }, { "without",            1,  1265 },
        { "powers",             0,   180 }, { "without",            1,  1057 },
        { "powers",             0,   135 }, { "without",            1,   888 },
        { "powers",             0,    44 }, { "without",            1,   522 },
        { "neglected",          0,   405 }, { "without",            1,   376 },
        { "utterly",            0,   404 }, { "without",            0,   772 },
        { "till",               0,   392 }, { "without",            0,   670 },
        { "operation",          1,  1207 }, { "without",            0,   549 },
        { "operation",          1,   200 }, { "ruler",              0,  1082 },
        { "operation",          0,   391 }, { "convulsions",        0,   551 },
        { "United",             1,  1740 }, { "within",             2,  3412 },
        { "United",             1,  1532 }, { "within",             2,  3216 },
        { "United",             1,   849 }, { "within",             2,  2933 },
        { "United",             1,   203 }, { "within",             2,  2776 },
        { "United",             1,   144 }, { "within",             2,  2467 },
        { "United",             0,  1252 }, { "within",             2,  2084 },
        { "suspended",          0,   401 }, { "within",             2,  1675 },
        { "suspended",          0,   388 }, { "within",             1,   778 },
        { "among",              2,    19 }, { "within",             0,   552 },
        { "among",              1,  1518 }, { "begun",              0,   940 },
        { "among",              1,   320 }, { "paralleled",         0,   948 },
        { "among",              1,   312 }, { "prevent",            2,  3468 },
        { "among",              0,   730 }, { "prevent",            2,  1466 },
        { "among",              0,   662 }, { "prevent",            0,   558 },
        { "among",              0,   130 }, { "obstructing",        0,   567 },
        { "among",              0,   112 }, { "Naturalization",     0,   571 },
        { "among",              0,    42 }, { "encourage",          0,   579 },
        { "insurrections",      0,  1003 }, { "wanting",            0,  1091 },
        { "pressing",           0,   385 }, { "sole",               0,   470 },
        { "immediate",          0,   383 }, { "reduce",             0,   262 },
        { "pass",               0,   576 }, { "intending",          1,   937 },
        { "pass",               0,   415 }, { "than",               2,  3598 },
        { "pass",               0,   380 }, { "than",               2,  3083 },
        { "forbidden",          0,   376 }, { "than",               2,  1897 },
        { "public",             2,  2931 }, { "than",               1,   711 },
        { "public",             2,  1544 }, { "than",               0,   231 },
        { "public",             2,  1167 }, { "raising",            0,   584 },
        { "public",             2,   818 }, { "maternity",          2,  2047 },
        { "public",             2,   748 }, { "maternity",          2,  2005 },
        { "public",             2,   138 }, { "maternity",          2,  1999 },
        { "public",             1,  1711 }, { "certain",            0,   108 },
        { "public",             1,  1131 }, { "Appropriations",     0,   589 },
        { "public",             1,  1023 }, { "which",              2,  3516 },
        { "public",             1,   653 }, { "which",              2,  3428 },
        { "public",             1,   606 }, { "which",              2,  3392 },
        { "public",             0,   466 }, { "which",              2,  3205 },
        { "public",             0,   371 }, { "which",              2,  3135 },
        { "refused",            0,   503 }, { "which",              2,  3085 },
        { "refused",            0,   413 }, { "which",              2,  3056 },
        { "refused",            0,   359 }, { "which",              2,  2840 },
        { "expressed",          1,  1155 }, { "which",              2,  2835 },
        { "He",                 0,   999 }, { "which",              2,  2496 },
        { "He",                 0,   964 }, { "which",              2,  2431 },
        { "He",                 0,   919 }, { "which",              2,  2360 },
        { "He",                 0,   901 }, { "which",              2,  1832 },
        { "He",                 0,   883 }, { "which",              2,  1524 },
        { "He",                 0,   693 }, { "which",              2,   870 },
        { "He",                 0,   677 }, { "which",              2,   642 },
        { "He",                 0,   659 }, { "which",              1,  1652 },
        { "He",                 0,   635 }, { "which",              1,  1628 },
        { "He",                 0,   611 }, { "which",              1,  1611 },
        { "He",                 0,   593 }, { "which",              1,  1161 },
        { "He",                 0,   554 }, { "which",              1,   685 },
        { "He",                 0,   501 }, { "which",              1,   656 },
        { "He",                 0,   481 }, { "which",              1,   423 },
        { "He",                 0,   449 }, { "which",              1,    64 },
        { "He",                 0,   411 }, { "which",              1,    50 },
        { "He",                 0,   374 }, { "which",              0,  1324 },
        { "He",                 0,   357 }, { "which",              0,  1184 },
        { "friendship",         1,  1517 }, { "which",              0,  1156 },
        { "Foreigners",         0,   573 }, { "which",              0,  1072 },
        { "constitution",       1,   577 }, { "which",              0,   744 },
        { "constitution",       0,   707 }, { "which",              0,   304 },
        { "o",                  2,  2532 }, { "which",              0,   240 },
        { "o",                  2,  2505 }, { "which",              0,    80 },
        { "o",                  2,  2483 }, { "which",              0,    54 },
        { "o",                  2,   464 }, { "which",              0,    33 },
        { "o",                  2,   446 }, { "Lands",              0,   591 },
        { "o",                  2,   431 }, { "negotiate",          2,  1741 },
        { "o",                  2,   413 }, { "society",            2,   188 },
        { "o",                  0,   998 }, { "society",            1,  1720 },
        { "o",                  0,   963 }, { "society",            1,  1189 },
        { "o",                  0,   900 }, { "society",            1,   957 },
        { "o",                  0,   882 }, { "society",            1,   950 },
        { "o",                  0,   862 }, { "society",            1,   274 },
        { "o",                  0,   843 }, { "obstructed",         0,   595 },
        { "o",                  0,   800 }, { "warfare",            0,  1025 },
        { "o",                  0,   788 }, { "direct",             2,  2384 },
        { "o",                  0,   766 }, { "direct",             2,  1575 },
        { "o",                  0,   754 }, { "direct",             0,   334 },
        { "o",                  0,   722 }, { "cultures",           2,   115 },
        { "o",                  0,   692 }, { "causes",             0,   213 },
        { "o",                  0,   676 }, { "causes",             0,    79 },
        { "o",                  0,   658 }, { "Administration",     0,   597 },
        { "o",                  0,   634 }, { "subject",            2,  1181 },
        { "o",                  0,   610 }, { "subject",            2,   664 },
        { "o",                  0,   592 }, { "subject",            1,  1676 },
        { "o",                  0,   553 }, { "subject",            0,   699 },
        { "o",                  0,   500 }, { "Justice",            2,  2708 },
        { "o",                  0,   480 }, { "Justice",            2,   287 },
        { "o",                  0,   448 }, { "Justice",            0,   599 },
        { "o",                  0,   410 }, { "rule",               2,    65 },
        { "o",                  0,   373 }, { "rule",               1,   123 },
        { "o",                  0,   356 }, { "rule",               0,  1023 },
        { "submitted",          0,   351 }, { "rule",               0,   839 },
        { "States",             2,  3525 }, { "Judiciary",          0,   608 },
        { "States",             2,  3259 }, { "made",               2,  3358 },
        { "States",             2,  2782 }, { "made",               2,  2962 },
        { "States",             2,  2585 }, { "made",               1,  1469 },
        { "States",             2,  1092 }, { "made",               1,   406 },
        { "States",             2,   245 }, { "made",               0,   613 },
        { "States",             2,   132 }, { "Judges",             0,   614 },
        { "States",             1,   317 }, { "uncomfortable",      0,   458 },
        { "States",             1,   192 }, { "dependent",          0,   615 },
        { "States",             0,  1326 }, { "Colonies",           0,  1253 },
        { "States",             0,  1302 }, { "Colonies",           0,  1245 },
        { "States",             0,  1264 }, { "Colonies",           0,   842 },
        { "States",             0,  1211 }, { "Colonies",           0,   297 },
        { "States",             0,   753 }, { "abdicated",          0,   885 },
        { "States",             0,   563 }, { "Will",               0,   618 },
        { "States",             0,   344 }, { "tenure",             0,   622 },
        { "States",             0,    12 }, { "Forms",              0,   858 },
        { "let",                0,   348 }, { "offices",            0,   625 },
        { "this",               2,  3588 }, { "necessary",          2,  3363 },
        { "this",               2,  3556 }, { "necessary",          2,  2977 },
        { "this",               2,  3482 }, { "necessary",          2,  1504 },
        { "this",               2,  3424 }, { "necessary",          2,  1207 },
        { "this",               2,  3390 }, { "necessary",          2,   174 },
        { "this",               2,  3332 }, { "necessary",          1,  1383 },
        { "this",               2,  3235 }, { "necessary",          1,  1304 },
        { "this",               2,  2922 }, { "necessary",          1,   663 },
        { "this",               2,   781 }, { "necessary",          0,   368 },
        { "this",               2,   170 }, { "necessary",          0,    24 },
        { "this",               1,  1746 }, { "amount",             0,   628 },
        { "this",               1,  1636 }, { "derogations",        2,  1920 },
        { "this",               1,  1151 }, { "indispensable",      1,  1226 },
        { "this",               1,  1050 }, { "keeping",            1,   275 },
        { "this",               1,  1002 }, { "payment",            0,   630 },
        { "this",               1,   543 }, { "salaries",           0,   633 },
        { "this",               1,   374 }, { "multitude",          0,   639 },
        { "this",               1,   276 }, { "specified",          2,   612 },
        { "this",               1,   238 }, { "Offices",            0,   642 },
        { "this",               0,  1336 }, { "Inhabitants",        0,   750 },
        { "this",               0,   922 }, { "sent",               0,   644 },
        { "this",               0,   347 }, { "entitle",            0,    63 },
        { "barbarous",          1,    48 }, { "swarms",             0,   646 },
        { "barbarous",          0,   952 }, { "Creator",            0,   106 },
        { "would",              2,  2497 }, { "Boundaries",         0,   821 },
        { "would",              2,  1305 }, { "In",                 2,  3420 },
        { "would",              0,  1157 }, { "In",                 2,  2108 },
        { "would",              0,   429 }, { "In",                 2,  1535 },
        { "prove",              0,   346 }, { "In",                 2,   399 },
        { "To",                 2,  1979 }, { "In",                 1,  1665 },
        { "To",                 2,   169 }, { "In",                 0,  1036 },
        { "To",                 0,   345 }, { "New",                0,   641 },
        { "organization",       1,  1213 }, { "Officers",           0,   648 },
        { "events",             0,    21 }, { "protection",         2,  3479 },
        { "Legislative",        0,   519 }, { "protection",         2,  3474 },
        { "an",                 2,  3202 }, { "protection",         2,  2853 },
        { "an",                 2,  2938 }, { "protection",         2,  2820 },
        { "an",                 2,  2908 }, { "protection",         2,  2310 },
        { "an",                 2,  2882 }, { "protection",         2,  2300 },
        { "an",                 2,  2608 }, { "protection",         2,  2268 },
        { "an",                 2,  1861 }, { "protection",         2,  2262 },
        { "an",                 2,  1022 }, { "protection",         2,  2194 },
        { "an",                 2,   668 }, { "protection",         2,  2042 },
        { "an",                 2,    88 }, { "protection",         2,  1991 },
        { "an",                 2,    15 }, { "protection",         2,  1977 },
        { "an",                 1,  1295 }, { "protection",         2,  1874 },
        { "an",                 1,  1098 }, { "protection",         2,  1802 },
        { "an",                 1,   609 }, { "protection",         2,  1499 },
        { "an",                 1,   559 }, { "protection",         2,   885 },
        { "an",                 0,  1113 }, { "protection",         2,   596 },
        { "an",                 0,  1027 }, { "protection",         2,   178 },
        { "an",                 0,   829 }, { "protection",         1,  1595 },
        { "an",                 0,   815 }, { "protection",         1,  1436 },
        { "an",                 0,   339 }, { "protection",         1,  1325 },
        { "co",                 1,  1206 }, { "protection",         1,  1310 },
        { "co",                 1,   199 }, { "protection",         1,  1260 },
        { "over",               0,  1116 }, { "protection",         1,   955 },
        { "over",               0,   342 }, { "protection",         1,   756 },
        { "is",                 2,  3599 }, { "protection",         1,   536 },
        { "is",                 2,  2976 }, { "protection",         1,   527 },
        { "is",                 2,  2925 }, { "protection",         0,  1344 },
        { "is",                 2,  2847 }, { "harrass",            0,   650 },
        { "is",                 2,  2844 }, { "proclaimed",         1,    82 },
        { "is",                 2,  2503 }, { "out",                2,   331 },
        { "is",                 2,  2088 }, { "out",                1,  1428 },
        { "is",                 2,  1884 }, { "out",                0,   891 },
        { "is",                 2,  1585 }, { "out",                0,   655 },
        { "is",                 2,  1503 }, { "us",                 0,  1117 },
        { "is",                 2,  1331 }, { "us",                 0,  1005 },
        { "is",                 2,  1297 }, { "us",                 0,   899 },
        { "is",                 2,  1206 }, { "us",                 0,   890 },
        { "is",                 2,  1129 }, { "us",                 0,   877 },
        { "is",                 2,   769 }, { "us",                 0,   791 },
        { "is",                 2,   536 }, { "us",                 0,   777 },
        { "is",                 2,   342 }, { "us",                 0,   771 },
        { "is",                 2,   173 }, { "us",                 0,   731 },
        { "is",                 2,    56 }, { "us",                 0,   700 },
        { "is",                 2,    41 }, { "us",                 0,   663 },
        { "is",                 1,  1662 }, { "assembly",           2,   852 },
        { "is",                 1,  1619 }, { "assembly",           2,   839 },
        { "is",                 1,  1613 }, { "assembly",           1,  1086 },
        { "is",                 1,  1197 }, { "Annihilation",       0,   523 },
        { "is",                 1,   952 }, { "peace",              1,  1538 },
        { "is",                 1,   942 }, { "peace",              1,    34 },
        { "is",                 1,   597 }, { "peace",              0,   667 },
        { "is",                 1,   363 }, { "Consent",            0,   774 },
        { "is",                 1,   228 }, { "Consent",            0,   672 },
        { "is",                 1,   128 }, { "legislatures",       0,   675 },
        { "is",                 1,    97 }, { "office",             2,  2741 },
        { "is",                 1,    93 }, { "office",             2,  2673 },
        { "is",                 1,    27 }, { "office",             2,  2637 },
        { "is",                 0,  1289 }, { "affected",           0,   679 },
        { "is",                 0,  1077 }, { "Military",           0,   683 },
        { "is",                 0,  1066 }, { "independent",        2,  2939 },
        { "is",                 0,  1026 }, { "independent",        2,   669 },
        { "is",                 0,   920 }, { "independent",        1,   610 },
        { "is",                 0,   323 }, { "independent",        1,   430 },
        { "is",                 0,   300 }, { "independent",        0,   684 },
        { "is",                 0,   272 }, { "power",              2,  3295 },
        { "is",                 0,   268 }, { "power",              0,   873 },
        { "is",                 0,   154 }, { "power",              0,   691 },
        { "places",             2,    69 }, { "jurisdiction",       1,   327 },
        { "places",             0,   456 }, { "jurisdiction",       0,  1115 },
        { "Tyranny",            0,   341 }, { "jurisdiction",       0,   703 },
        { "establishment",      2,  1067 }, { "foreign",            0,   928 },
        { "establishment",      2,   168 }, { "foreign",            0,   704 },
        { "establishment",      0,   337 }, { "care",               2,  2170 },
        { "principles",         2,  3275 }, { "care",               2,  2160 },
        { "principles",         2,  3149 }, { "care",               2,  1501 },
        { "principles",         2,  2577 }, { "care",               1,  1419 },
        { "principles",         2,   976 }, { "care",               1,  1381 },
        { "principles",         2,   329 }, { "lives",              0,   915 },
        { "principles",         2,    60 }, { "unacknowledged",     0,   709 },
        { "principles",         1,  1737 }, { "legislature",        0,  1110 },
        { "principles",         1,   846 }, { "pretended",          0,   798 },
        { "principles",         0,   176 }, { "pretended",          0,   720 },
        { "injuries",           0,   328 }, { "frontiers",          2,   823 },
        { "Britain",            0,  1288 }, { "frontiers",          1,  1074 },
        { "Britain",            0,   322 }, { "frontiers",          0,  1016 },
        { "7",                  2,   559 }, { "For",                0,   863 },
        { "7",                  1,   512 }, { "For",                0,   844 },
        { "unanimous",          0,     6 }, { "For",                0,   801 },
        { "deriving",           0,   132 }, { "For",                0,   789 },
        { "superior",           0,   687 }, { "For",                0,   775 },
        { "migrations",         0,   581 }, { "For",                0,   767 },
        { "Great",              0,  1287 }, { "For",                0,   755 },
        { "Great",              0,   321 }, { "For",                0,   732 },
        { "render",             0,   825 }, { "For",                0,   723 },
        { "render",             0,   681 }, { "Quartering",         0,   724 },
        { "natural",            2,  2732 }, { "whatsoever",         0,   881 },
        { "natural",            2,  2664 }, { "Legislation",        0,   721 },
        { "natural",            2,  2628 }, { "shall",              2,  3558 },
        { "natural",            1,   944 }, { "shall",              2,  3484 },
        { "history",            0,   325 }, { "shall",              2,  3466 },
        { "history",            0,   315 }, { "shall",              2,  3452 },
        { "those",              2,  3457 }, { "shall",              2,  3405 },
        { "those",              2,  3450 }, { "shall",              2,  3268 },
        { "those",              2,  3417 }, { "shall",              2,  3189 },
        { "those",              2,  3345 }, { "shall",              2,  3120 },
        { "those",              2,  2965 }, { "shall",              2,  3114 },
        { "those",              2,  2137 }, { "shall",              2,  3077 },
        { "those",              2,  1410 }, { "shall",              2,  3041 },
        { "those",              2,  1100 }, { "shall",              2,  3023 },
        { "those",              2,   439 }, { "shall",              2,  2999 },
        { "those",              2,   198 }, { "shall",              2,  2960 },
        { "those",              0,   427 }, { "shall",              2,  2948 },
        { "former",             0,   310 }, { "shall",              2,  2826 },
        { "citizen",            2,  2822 }, { "shall",              2,  2380 },
        { "citizen",            2,  2764 }, { "shall",              2,  2303 },
        { "citizen",            2,  2726 }, { "shall",              2,  2195 },
        { "citizen",            2,  2658 }, { "shall",              2,  1986 },
        { "citizen",            2,  2622 }, { "shall",              2,  1971 },
        { "citizen",            2,  2408 }, { "shall",              2,  1561 },
        { "citizen",            2,  2334 }, { "shall",              2,  1517 },
        { "citizen",            2,  1050 }, { "shall",              2,  1493 },
        { "obtained",           0,   397 }, { "shall",              2,  1464 },
        { "Acts",               0,  1321 }, { "shall",              2,  1430 },
        { "Acts",               0,   718 }, { "shall",              2,  1418 },
        { "constrains",         0,   305 }, { "shall",              2,  1379 },
        { "now",                0,   301 }, { "shall",              2,  1227 },
        { "Human",              2,  3536 }, { "shall",              2,  1215 },
        { "Human",              2,  3440 }, { "shall",              2,  1000 },
        { "Human",              2,   340 }, { "shall",              2,   929 },
        { "Human",              2,   338 }, { "shall",              2,   922 },
        { "Human",              2,   298 }, { "shall",              2,   832 },
        { "Human",              2,   261 }, { "shall",              2,   801 },
        { "Human",              1,     4 }, { "shall",              2,   693 },
        { "sufferance",         0,   294 }, { "shall",              2,   662 },
        { "Such",               2,  1515 }, { "shall",              2,   522 },
        { "Such",               2,   605 }, { "shall",              2,   512 },
        { "Such",               0,   289 }, { "shall",              2,   488 },
        { "security",           2,  2092 }, { "shall",              2,   365 },
        { "security",           2,  2036 }, { "shall",              1,  1674 },
        { "security",           2,  2022 }, { "shall",              1,  1552 },
        { "security",           2,   555 }, { "shall",              1,  1526 },
        { "security",           2,   547 }, { "shall",              1,  1512 },
        { "security",           2,    92 }, { "shall",              1,  1488 },
        { "security",           1,  1390 }, { "shall",              1,  1475 },
        { "security",           1,  1195 }, { "shall",              1,  1467 },
        { "security",           1,   452 }, { "shall",              1,  1460 },
        { "security",           0,   288 }, { "shall",              1,  1447 },
        { "perform",            2,  3572 }, { "shall",              1,  1431 },
        { "perform",            2,   526 }, { "shall",              1,  1170 },
        { "perform",            1,  1768 }, { "shall",              1,  1162 },
        { "legitimate",         2,  2522 }, { "shall",              1,  1153 },
        { "legitimate",         2,   628 }, { "shall",              1,  1142 },
        { "likely",             2,  1943 }, { "shall",              1,   982 },
        { "likely",             0,   190 }, { "shall",              1,   924 },
        { "Systems",            0,   311 }, { "shall",              1,   864 },
        { "off",                0,   757 }, { "shall",              1,   729 },
        { "off",                0,   277 }, { "shall",              1,   705 },
        { "ends",               0,   152 }, { "shall",              1,   670 },
        { "Despotism",          0,   266 }, { "shall",              1,   585 },
        { "absolute",           0,   838 }, { "shall",              1,   482 },
        { "absolute",           0,   340 }, { "shall",              1,   471 },
        { "absolute",           0,   265 }, { "shall",              1,   459 },
        { "under",              2,  3408 }, { "shall",              1,   404 },
        { "under",              2,  3063 }, { "shall",              1,   281 },
        { "under",              2,  2436 }, { "shall",              1,    67 },
        { "under",              2,  2365 }, { "shall",              0,   187 },
        { "under",              2,  2179 }, { "troops",             0,   729 },
        { "under",              2,  1699 }, { "protecting",         0,   733 },
        { "under",              2,  1481 }, { "mock",               0,   737 },
        { "under",              2,  1174 }, { "Trial",              0,   785 },
        { "under",              1,   692 }, { "Trial",              0,   738 },
        { "under",              1,   436 }, { "subsidiarity",       2,  3254 },
        { "under",              1,   325 }, { "subsidiarity",       2,   226 },
        { "under",              0,   264 }, { "punishment",         2,  3126 },
        { "Houses",             0,   485 }, { "punishment",         2,  1320 },
        { "they",               2,  3361 }, { "punishment",         2,   500 },
        { "they",               2,  3262 }, { "punishment",         2,   485 },
        { "they",               2,   230 }, { "punishment",         1,   495 },
        { "they",               0,  1303 }, { "punishment",         0,   740 },
        { "they",               0,  1266 }, { "parts",              2,   456 },
        { "they",               0,   745 }, { "parts",              0,   762 },
        { "they",               0,   241 }, { "depriving",          0,   776 },
        { "they",               0,   101 }, { "many",               0,   779 },
        { "they",               0,    75 }, { "cases",              2,  2689 },
        { "unworthy",           0,   956 }, { "cases",              2,  2044 },
        { "spiritual",          2,    35 }, { "cases",              2,  1752 },
        { "amongst",            0,  1004 }, { "cases",              2,  1697 },
        { "regard",             2,  3249 }, { "cases",              2,  1172 },
        { "regard",             2,   309 }, { "cases",              0,   880 },
        { "regard",             2,   210 }, { "cases",              0,   780 },
        { "design",             0,   260 }, { "benefits",           2,  2093 },
        { "Free",               0,  1299 }, { "benefits",           2,  2037 },
        { "Free",               0,  1261 }, { "benefits",           1,  1587 },
        { "placement",          2,  1785 }, { "benefits",           0,   783 },
        { "placement",          2,  1774 }, { "least",              1,  1451 },
        { "reliance",           0,  1341 }, { "leave",              2,  2010 },
        { "evinces",            0,   258 }, { "leave",              2,  2006 },
        { "unalienable",        0,   109 }, { "leave",              2,  1866 },
        { "armed",              0,   728 }, { "leave",              1,   790 },
        { "Object",             0,   257 }, { "Jury",               0,   787 },
        { "not",                2,  3467 }, { "imposing",           0,   768 },
        { "not",                2,  3291 }, { "transporting",       0,   924 },
        { "not",                2,  3172 }, { "transporting",       0,   790 },
        { "not",                2,  3162 }, { "Form",               0,   145 },
        { "not",                2,  3121 }, { "beyond",             1,  1408 },
        { "not",                2,  3058 }, { "beyond",             0,   792 },
        { "not",                2,  2848 }, { "unions",             2,   882 },
        { "not",                2,  1894 }, { "unions",             1,  1322 },
        { "not",                2,  1465 }, { "tried",              2,  3194 },
        { "not",                1,   824 }, { "tried",              2,  3175 },
        { "not",                1,   687 }, { "tried",              0,   796 },
        { "not",                1,    98 }, { "free",               2,  2389 },
        { "not",                0,   206 }, { "free",               2,  1784 },
        { "future",             2,   319 }, { "free",               2,   961 },
        { "future",             2,    27 }, { "free",               2,   924 },
        { "future",             0,   287 }, { "free",               2,   415 },
        { "pursuit",            0,   119 }, { "free",               2,   156 },
        { "implying",           2,  3562 }, { "free",               1,  1655 },
        { "implying",           1,  1752 }, { "free",               1,  1449 },
        { "Governors",          0,   378 }, { "free",               1,  1247 },
        { "form",               2,   877 }, { "free",               1,  1232 },
        { "form",               1,  1317 }, { "free",               1,  1179 },
        { "form",               0,   183 }, { "free",               1,   931 },
        { "Life",               0,   115 }, { "free",               1,   335 },
        { "objection",          2,   768 }, { "free",               0,  1085 },
        { "of",                 2,  3581 }, { "free",               0,   804 },
        { "of",                 2,  3579 }, { "invariably",         0,   254 },
        { "of",                 2,  3552 }, { "during",             1,   916 },
        { "of",                 2,  3550 }, { "System",             0,   805 },
        { "of",                 2,  3535 }, { "English",            0,   807 },
        { "of",                 2,  3503 }, { "neighbouring",       0,   811 },
        { "of",                 2,  3478 }, { "fairly",             2,  2465 },
        { "of",                 2,  3449 }, { "fairly",             2,   610 },
        { "of",                 2,  3439 }, { "therein",            0,   814 },
        { "of",                 2,  3384 }, { "Arbitrary",          0,   816 },
        { "of",                 2,  3368 }, { "only",               2,  3359 },
        { "of",                 2,  3353 }, { "only",               2,  3260 },
        { "of",                 2,  3344 }, { "only",               1,  1677 },
        { "of",                 2,  3325 }, { "only",               1,   928 },
        { "of",                 2,  3316 }, { "only",               0,  1058 },
        { "of",                 2,  3253 }, { "only",               0,   447 },
        { "of",                 2,  3244 }, { "government",         1,  1150 },
        { "of",                 2,  3234 }, { "government",         1,  1112 },
        { "of",                 2,  3159 }, { "government",         0,   817 },
        { "of",                 2,  3154 }, { "repeated",           0,  1060 },
        { "of",                 2,  3127 }, { "repeated",           0,  1053 },
        { "of",                 2,  3101 }, { "repeated",           0,   327 },
        { "of",                 2,  3051 }, { "example",            0,   830 },
        { "of",                 2,  3045 }, { "others",             2,  3385 },
        { "of",                 2,  3033 }, { "others",             2,   745 },
        { "of",                 2,  3029 }, { "others",             1,  1702 },
        { "of",                 2,  3017 }, { "others",             1,  1020 },
        { "of",                 2,  3014 }, { "others",             1,   978 },
        { "of",                 2,  2991 }, { "others",             0,   697 },
        { "of",                 2,  2987 }, { "others",             0,   577 },
        { "of",                 2,  2952 }, { "others",             0,   513 },
        { "of",                 2,  2899 }, { "introducing",        0,   835 },
        { "of",                 2,  2871 }, { "Charters",           2,   268 },
        { "of",                 2,  2860 }, { "Charters",           0,   848 },
        { "of",                 2,  2839 }, { "sovereignty",        1,   441 },
        { "of",                 2,  2830 }, { "support",            0,  1334 },
        { "of",                 2,  2823 }, { "valuable",           0,   852 },
        { "of",                 2,  2811 }, { "Province",           0,   812 },
        { "of",                 2,  2803 }, { "arts",               2,   918 },
        { "of",                 2,  2785 }, { "arts",               2,   914 },
        { "of",                 2,  2779 }, { "arts",               1,  1578 },
        { "of",                 2,  2765 }, { "fundamentally",      0,   856 },
        { "of",                 2,  2760 }, { "suspending",         0,   864 },
        { "of",                 2,  2757 }, { "Legislatures",       0,   867 },
        { "of",                 2,  2727 }, { "formidable",         0,   444 },
        { "of",                 2,  2712 }, { "evident",            0,    93 },
        { "of",                 2,  2707 }, { "declaring",          0,   889 },
        { "of",                 2,  2704 }, { "declaring",          0,   869 },
        { "of",                 2,  2695 }, { "Nothing",            2,  3554 },
        { "of",                 2,  2690 }, { "Nothing",            2,  3480 },
        { "of",                 2,  2686 }, { "Nothing",            1,  1744 },
        { "of",                 2,  2659 }, { "instrument",         0,   833 },
        { "of",                 2,  2645 }, { "State",              2,  2874 },
        { "of",                 2,  2623 }, { "State",              2,  2863 },
        { "of",                 2,  2617 }, { "State",              2,  2838 },
        { "of",                 2,  2602 }, { "State",              2,  2814 },
        { "of",                 2,  2599 }, { "State",              2,  2745 },
        { "of",                 2,  2594 }, { "State",              2,  2677 },
        { "of",                 2,  2582 }, { "State",              2,  2641 },
        { "of",                 2,  2569 }, { "State",              2,  2444 },
        { "of",                 2,  2535 }, { "State",              2,  2429 },
        { "of",                 2,  2527 }, { "State",              2,  2373 },
        { "of",                 2,  2524 }, { "State",              2,  2358 },
        { "of",                 2,  2508 }, { "State",              2,  1294 },
        { "of",                 2,  2486 }, { "State",              2,  1075 },
        { "of",                 2,  2476 }, { "State",              1,  1755 },
        { "of",                 2,  2442 }, { "State",              1,  1218 },
        { "of",                 2,  2409 }, { "State",              1,   960 },
        { "of",                 2,  2376 }, { "State",              0,  1285 },
        { "of",                 2,  2371 }, { "State",              0,   535 },
        { "of",                 2,  2335 }, { "War",                0,  1309 },
        { "of",                 2,  2308 }, { "War",                0,  1200 },
        { "of",                 2,  2294 }, { "War",                0,   897 },
        { "of",                 2,  2284 }, { "receive",            2,   960 },
        { "of",                 2,  2275 }, { "receive",            2,   809 },
        { "of",                 2,  2272 }, { "receive",            1,  1062 },
        { "of",                 2,  2266 }, { "against",            2,  1937 },
        { "of",                 2,  2256 }, { "against",            2,  1803 },
        { "of",                 2,  2226 }, { "against",            1,  1261 },
        { "of",                 2,  2214 }, { "against",            1,   760 },
        { "of",                 2,  2203 }, { "against",            1,   627 },
        { "of",                 2,  2191 }, { "against",            1,   546 },
        { "of",                 2,  2165 }, { "against",            1,   537 },
        { "of",                 2,  2061 }, { "against",            1,   111 },
        { "of",                 2,  2059 }, { "against",            0,   979 },
        { "of",                 2,  2016 }, { "against",            0,   898 },
        { "of",                 2,  1889 }, { "plundered",          0,   903 },
        { "of",                 2,  1882 }, { "burnt",              0,   909 },
        { "of",                 2,  1875 }, { "proceedings",        2,  3200 },
        { "of",                 2,  1870 }, { "proceedings",        2,  3181 },
        { "of",                 2,  1864 }, { "Standing",           0,   668 },
        { "of",                 2,  1849 }, { "ravaged",            0,   906 },
        { "of",                 2,  1793 }, { "Coasts",             0,   908 },
        { "of",                 2,  1780 }, { "destroyed",          0,   913 },
        { "of",                 2,  1771 }, { "Mercenaries",        0,   929 },
        { "of",                 2,  1755 }, { "works",              1,  1280 },
        { "of",                 2,  1753 }, { "works",              0,   933 },
        { "of",                 2,  1715 }, { "death",              2,  1310 },
        { "of",                 2,  1661 }, { "death",              2,   370 },
        { "of",                 2,  1639 }, { "death",              0,   935 },
        { "of",                 2,  1628 }, { "desolation",         0,   936 },
        { "of",                 2,  1613 }, { "constrained",        0,   966 },
        { "of",                 2,  1606 }, { "arising",            1,   833 },
        { "of",                 2,  1596 }, { "sexes",              0,  1033 },
        { "of",                 2,  1488 }, { "heavier",            2,  3079 },
        { "of",                 2,  1479 }, { "heavier",            1,   707 },
        { "of",                 2,  1471 }, { "DIGNITY",            2,   335 },
        { "of",                 2,  1462 }, { "tyranny",            1,   112 },
        { "of",                 2,  1416 }, { "tyranny",            0,   938 },
        { "of",                 2,  1409 }, { "compleat",           0,   931 },
        { "of",                 2,  1396 }, { "already",            2,  3210 },
        { "of",                 2,  1388 }, { "already",            0,   939 },
        { "of",                 2,  1386 }, { "circumstances",      1,  1407 },
        { "of",                 2,  1368 }, { "circumstances",      0,  1124 },
        { "of",                 2,  1273 }, { "circumstances",      0,   942 },
        { "of",                 2,  1255 }, { "Cruelty",            0,   944 },
        { "of",                 2,  1247 }, { "Member",             2,  3544 },
        { "of",                 2,  1240 }, { "Member",             2,  3524 },
        { "of",                 2,  1236 }, { "Member",             2,  3258 },
        { "of",                 2,  1195 }, { "Member",             2,  2873 },
        { "of",                 2,  1159 }, { "Member",             2,  2862 },
        { "of",                 2,  1145 }, { "Member",             2,  2837 },
        { "of",                 2,  1103 }, { "Member",             2,  2813 },
        { "of",                 2,  1101 }, { "Member",             2,  2781 },
        { "of",                 2,  1089 }, { "Member",             2,  2744 },
        { "of",                 2,  1078 }, { "Member",             2,  2676 },
        { "of",                 2,  1066 }, { "Member",             2,  2640 },
        { "of",                 2,  1051 }, { "Member",             2,  2584 },
        { "of",                 2,  1012 }, { "Member",             2,  2428 },
        { "of",                 2,   988 }, { "Member",             2,  2357 },
        { "of",                 2,   980 }, { "Member",             2,  1091 },
        { "of",                 2,   925 }, { "Member",             2,  1074 },
        { "of",                 2,   912 }, { "Member",             2,   244 },
        { "of",                 2,   906 }, { "Member",             2,   131 },
        { "of",                 2,   903 }, { "Member",             1,   316 },
        { "of",                 2,   886 }, { "Member",             1,   191 },
        { "of",                 2,   874 }, { "attend",             0,   407 },
        { "of",                 2,   856 }, { "perfidy",            0,   946 },
        { "of",                 2,   850 }, { "if",                 2,  3360 },
        { "of",                 2,   841 }, { "if",                 1,  1303 },
        { "of",                 2,   838 }, { "if",                 1,    95 },
        { "of",                 2,   829 }, { "ages",               0,  1032 },
        { "of",                 2,   822 }, { "ages",               0,   953 },
        { "of",                 2,   797 }, { "totally",            0,  1294 },
        { "of",                 2,   786 }, { "totally",            0,   955 },
        { "of",                 2,   780 }, { "authorities",        2,  2859 },
        { "of",                 2,   723 }, { "authorities",        2,  1545 },
        { "of",                 2,   711 }, { "authorities",        2,   139 },
        { "of",                 2,   705 }, { "Head",               0,   958 },
        { "of",                 2,   638 }, { "civilized",          0,   961 },
        { "of",                 2,   621 }, { "Allegiance",         0,  1271 },
        { "of",                 2,   618 }, { "nation",             0,   962 },
        { "of",                 2,   597 }, { "Citizens",           0,   969 },
        { "of",                 2,   586 }, { "higher",             1,  1473 },
        { "of",                 2,   556 }, { "taken",              2,  2504 },
        { "of",                 2,   504 }, { "taken",              2,  1542 },
        { "of",                 2,   477 }, { "taken",              2,  1519 },
        { "of",                 2,   471 }, { "taken",              0,   970 },
        { "of",                 2,   467 }, { "high",               2,  2306 },
        { "of",                 2,   461 }, { "high",               2,  2264 },
        { "of",                 2,   444 }, { "high",               2,  2189 },
        { "of",                 2,   434 }, { "high",               0,   974 },
        { "of",                 2,   419 }, { "bear",               0,   977 },
        { "of",                 2,   402 }, { "Arms",               0,   978 },
        { "of",                 2,   380 }, { "become",             0,   983 },
        { "of",                 2,   301 }, { "executioners",       0,   985 },
        { "of",                 2,   297 }, { "present",            0,   318 },
        { "of",                 2,   293 }, { "friends",            0,   988 },
        { "of",                 2,   288 }, { "Brethren",           0,   990 },
        { "of",                 2,   286 }, { "fall",               0,   993 },
        { "of",                 2,   283 }, { "excited",            0,  1001 },
        { "of",                 2,   277 }, { "Court",              2,  2711 },
        { "of",                 2,   260 }, { "Court",              2,  2706 },
        { "of",                 2,   225 }, { "Court",              2,   296 },
        { "of",                 2,   216 }, { "Court",              2,   285 },
        { "of",                 2,   185 }, { "domestic",           0,  1002 },
        { "of",                 2,   179 }, { "dignity",            2,  1840 },
        { "of",                 2,   167 }, { "dignity",            2,  1614 },
        { "of",                 2,   158 }, { "dignity",            2,   341 },
        { "of",                 2,   136 }, { "dignity",            2,   339 },
        { "of",                 2,   129 }, { "dignity",            2,    50 },
        { "of",                 2,   121 }, { "dignity",            1,  1300 },
        { "of",                 2,   118 }, { "dignity",            1,  1229 },
        { "of",                 2,   113 }, { "dignity",            1,   339 },
        { "of",                 2,   105 }, { "dignity",            1,   159 },
        { "of",                 2,    90 }, { "dignity",            1,    12 },
        { "of",                 2,    82 }, { "bring",              0,  1010 },
        { "of",                 2,    75 }, { "inhabitants",        0,  1013 },
        { "of",                 2,    66 }, { "known",              0,  1022 },
        { "of",                 2,    61 }, { "Legislature",        0,   437 },
        { "of",                 2,    48 }, { "spouses",            1,   938 },
        { "of",                 2,    33 }, { "undistinguished",    0,  1028 },
        { "of",                 2,    11 }, { "destruction",        2,  3578 },
        { "of",                 1,  1777 }, { "destruction",        1,  1774 },
        { "of",                 1,  1775 }, { "destruction",        0,  1029 },
        { "of",                 1,  1738 }, { "stage",              0,  1038 },
        { "of",                 1,  1709 }, { "Petitioned",         0,  1044 },
        { "of",                 1,  1704 }, { "privacy",            1,   737 },
        { "of",                 1,  1701 }, { "home",               2,   580 },
        { "of",                 1,  1690 }, { "home",               1,   739 },
        { "of",                 1,  1668 }, { "nor",                1,   871 },
        { "of",                 1,  1659 }, { "nor",                1,   742 },
        { "of",                 1,  1610 }, { "attacks",            1,   764 },
        { "of",                 1,  1596 }, { "attacks",            1,   744 },
        { "of",                 1,  1572 }, { "honour",             1,   747 },
        { "of",                 1,  1549 }, { "reputation",         1,   749 },
        { "of",                 1,  1537 }, { "movement",           2,  2786 },
        { "of",                 1,  1530 }, { "movement",           2,  2758 },
        { "of",                 1,  1503 }, { "movement",           2,   157 },
        { "of",                 1,  1495 }, { "movement",           1,   775 },
        { "of",                 1,  1484 }, { "residence",          2,  2788 },
        { "of",                 1,  1429 }, { "residence",          2,  2761 },
        { "of",                 1,  1404 }, { "residence",          1,   777 },
        { "of",                 1,  1394 }, { "borders",            1,   780 },
        { "of",                 1,  1372 }, { "state",              1,   783 },
        { "of",                 1,  1369 }, { "return",             1,   798 },
        { "of",                 1,  1360 }, { "14",                 2,   933 },
        { "of",                 1,  1342 }, { "14",                 1,   803 },
        { "of",                 1,  1326 }, { "seek",               2,  1058 },
        { "of",                 1,  1308 }, { "seek",               1,  1061 },
        { "of",                 1,  1298 }, { "seek",               1,   810 },
        { "of",                 1,  1256 }, { "biology",            2,   405 },
        { "of",                 1,  1249 }, { "countries",          2,  2805 },
        { "of",                 1,  1234 }, { "countries",          2,  1080 },
        { "of",                 1,  1219 }, { "countries",          1,   816 },
        { "of",                 1,  1216 }, { "asylum",             2,  1226 },
        { "of",                 1,  1188 }, { "asylum",             2,  1222 },
        { "of",                 1,  1149 }, { "asylum",             1,   817 },
        { "of",                 1,  1146 }, { "mind",               1,   280 },
        { "of",                 1,  1139 }, { "opinions",           2,   806 },
        { "of",                 1,  1127 }, { "opinions",           1,  1056 },
        { "of",                 1,  1113 }, { "opinions",           0,    70 },
        { "of",                 1,  1084 }, { "persecution",        1,   819 },
        { "of",                 1,  1073 }, { "This",               2,  3464 },
        { "of",                 1,  1046 }, { "This",               2,  3288 },
        { "of",                 1,   997 }, { "This",               2,  3118 },
        { "of",                 1,   986 }, { "This",               2,  2480 },
        { "of",                 1,   949 }, { "This",               2,   954 },
        { "of",                 1,   935 }, { "This",               2,   799 },
        { "of",                 1,   885 }, { "This",               2,   728 },
        { "of",                 1,   868 }, { "This",               2,   205 },
        { "of",                 1,   847 }, { "This",               1,   821 },
        { "of",                 1,   830 }, { "invoked",            1,   826 },
        { "of",                 1,   781 }, { "case",               2,  2058 },
        { "of",                 1,   774 }, { "case",               2,   281 },
        { "of",                 1,   757 }, { "case",               1,  1729 },
        { "of",                 1,   680 }, { "case",               1,   829 },
        { "of",                 1,   674 }, { "prosecutions",       1,   831 },
        { "of",                 1,   623 }, { "genuinely",          2,  3365 },
        { "of",                 1,   617 }, { "genuinely",          1,   832 },
        { "of",                 1,   542 }, { "crimes",             1,   837 },
        { "of",                 1,   528 }, { "contrary",           2,  1586 },
        { "of",                 1,   453 }, { "contrary",           1,  1732 },
        { "of",                 1,   440 }, { "contrary",           1,   841 },
        { "of",                 1,   417 }, { "20",                 2,  1325 },
        { "of",                 1,   410 }, { "20",                 1,  1076 },
        { "of",                 1,   378 }, { "15",                 2,  1018 },
        { "of",                 1,   358 }, { "15",                 1,   852 },
        { "of",                 1,   323 }, { "nationality",        2,  1417 },
        { "of",                 1,   315 }, { "nationality",        1,   894 },
        { "of",                 1,   273 }, { "nationality",        1,   878 },
        { "of",                 1,   256 }, { "nationality",        1,   870 },
        { "of",                 1,   237 }, { "nationality",        1,   860 },
        { "of",                 1,   229 }, { "Nations",            1,  1741 },
        { "of",                 1,   223 }, { "Nations",            1,  1533 },
        { "of",                 1,   213 }, { "Nations",            1,   850 },
        { "of",                 1,   207 }, { "Nations",            1,   204 },
        { "of",                 1,   185 }, { "Nations",            1,   145 },
        { "of",                 1,   171 }, { "arbitrarily",        1,   984 },
        { "of",                 1,   162 }, { "arbitrarily",        1,   866 },
        { "of",                 1,   142 }, { "deprived",           2,  1158 },
        { "of",                 1,   134 }, { "deprived",           1,   985 },
        { "of",                 1,   124 }, { "deprived",           1,   867 },
        { "of",                 1,    87 }, { "21",                 2,  1337 },
        { "of",                 1,    70 }, { "21",                 1,  1101 },
        { "of",                 1,    60 }, { "16",                 2,  1107 },
        { "of",                 1,    55 }, { "16",                 1,   880 },
        { "of",                 1,    30 }, { "age",                2,  2054 },
        { "of",                 1,    23 }, { "age",                2,  1933 },
        { "of",                 1,    20 }, { "age",                2,  1902 },
        { "of",                 1,    14 }, { "age",                2,  1888 },
        { "of",                 1,     9 }, { "age",                2,  1531 },
        { "of",                 1,     3 }, { "age",                2,  1375 },
        { "of",                 0,  1345 }, { "age",                1,  1400 },
        { "of",                 0,  1335 }, { "age",                1,   887 },
        { "of",                 0,  1328 }, { "due",                2,  3248 },
        { "of",                 0,  1286 }, { "due",                2,  1231 },
        { "of",                 0,  1256 }, { "due",                2,   972 },
        { "of",                 0,  1243 }, { "due",                2,   209 },
        { "of",                 0,  1239 }, { "due",                1,  1692 },
        { "of",                 0,  1229 }, { "due",                1,   891 },
        { "of",                 0,  1223 }, { "candid",             0,   354 },
        { "of",                 0,  1212 }, { "marry",              2,   685 },
        { "of",                 0,  1208 }, { "marry",              2,   675 },
        { "of",                 0,  1196 }, { "marry",              1,   901 },
        { "of",                 0,  1175 }, { "found",              2,   968 },
        { "of",                 0,  1172 }, { "found",              2,   690 },
        { "of",                 0,  1148 }, { "found",              2,   679 },
        { "of",                 0,  1125 }, { "found",              1,   904 },
        { "of",                 0,  1122 }, { "another",            1,   354 },
        { "of",                 0,  1106 }, { "another",            0,    38 },
        { "of",                 0,  1083 }, { "marriage",           1,   917 },
        { "of",                 0,  1039 }, { "marriage",           1,   915 },
        { "of",                 0,  1030 }, { "dissolution",        1,   921 },
        { "of",                 0,  1024 }, { "Marriage",           1,   923 },
        { "of",                 0,  1014 }, { "entered",            1,   926 },
        { "of",                 0,   986 }, { "group",              1,  1756 },
        { "of",                 0,   959 }, { "group",              1,   947 },
        { "of",                 0,   943 }, { "unit",               1,   948 },
        { "of",                 0,   934 }, { "22",                 2,  1422 },
        { "of",                 0,   927 }, { "22",                 1,  1183 },
        { "of",                 0,   916 }, { "17",                 2,  1132 },
        { "of",                 0,   892 }, { "17",                 1,   962 },
        { "of",                 0,   859 }, { "injury",             0,  1061 },
        { "of",                 0,   806 }, { "well",               2,  1507 },
        { "of",                 0,   784 }, { "well",               2,   124 },
        { "of",                 0,   781 }, { "well",               1,  1367 },
        { "of",                 0,   763 }, { "well",               1,   973 },
        { "of",                 0,   751 }, { "SOLIDARITY",         2,  1666 },
        { "of",                 0,   727 }, { "association",        2,   857 },
        { "of",                 0,   719 }, { "association",        2,   842 },
        { "of",                 0,   685 }, { "association",        1,  1099 },
        { "of",                 0,   673 }, { "association",        1,  1088 },
        { "of",                 0,   666 }, { "association",        1,   976 },
        { "of",                 0,   647 }, { "23",                 2,  1438 },
        { "of",                 0,   640 }, { "23",                 1,  1238 },
        { "of",                 0,   631 }, { "18",                 2,  1219 },
        { "of",                 0,   623 }, { "18",                 1,   990 },
        { "of",                 0,   598 }, { "thought",            2,   724 },
        { "of",                 0,   590 }, { "thought",            2,   712 },
        { "of",                 0,   587 }, { "thought",            1,   998 },
        { "of",                 0,   572 }, { "includes",           2,  2482 },
        { "of",                 0,   561 }, { "includes",           2,   956 },
        { "of",                 0,   546 }, { "includes",           2,   730 },
        { "of",                 0,   522 }, { "includes",           1,  1052 },
        { "of",                 0,   497 }, { "includes",           1,  1004 },
        { "of",                 0,   472 }, { "either",             2,   739 },
        { "of",                 0,   464 }, { "either",             1,  1014 },
        { "of",                 0,   433 }, { "community",          2,  3153 },
        { "of",                 0,   424 }, { "community",          2,  1663 },
        { "of",                 0,   421 }, { "community",          2,   743 },
        { "of",                 0,   382 }, { "community",          2,   316 },
        { "of",                 0,   338 }, { "community",          1,  1650 },
        { "of",                 0,   326 }, { "community",          1,  1574 },
        { "of",                 0,   320 }, { "community",          1,  1018 },
        { "of",                 0,   316 }, { "private",            2,  1547 },
        { "of",                 0,   312 }, { "private",            2,   751 },
        { "of",                 0,   295 }, { "private",            2,   576 },
        { "of",                 0,   249 }, { "private",            2,   562 },
        { "of",                 0,   157 }, { "private",            1,  1025 },
        { "of",                 0,   150 }, { "manifest",           2,   753 },
        { "of",                 0,   146 }, { "manifest",           1,  1027 },
        { "of",                 0,   139 }, { "24",                 2,  1485 },
        { "of",                 0,   120 }, { "24",                 1,  1330 },
        { "of",                 0,    71 }, { "19",                 2,  1268 },
        { "of",                 0,    60 }, { "19",                 1,  1039 },
        { "of",                 0,    57 }, { "practice",           2,   760 },
        { "of",                 0,    45 }, { "practice",           1,  1034 },
        { "of",                 0,    19 }, { "expression",         2,   798 },
        { "of",                 0,    13 }, { "expression",         2,   787 },
        { "of",                 0,     8 }, { "expression",         1,  1049 },
        { "Rights",             2,  3537 }, { "ideas",              2,   814 },
        { "Rights",             2,  3441 }, { "ideas",              1,  1067 },
        { "Rights",             2,  3387 }, { "through",            1,  1201 },
        { "Rights",             2,   299 }, { "through",            1,  1118 },
        { "Rights",             2,   262 }, { "through",            1,  1068 },
        { "Rights",             1,     5 }, { "media",              2,   831 },
        { "Rights",             0,   110 }, { "media",              1,  1070 },
        { "established",        2,  2944 }, { "stand",              2,  2419 },
        { "established",        2,  2182 }, { "stand",              2,  2400 },
        { "established",        0,   204 }, { "stand",              2,  2345 },
        { "Course",             0,    18 }, { "stand",              2,  2322 },
        { "by",                 2,  3542 }, { "regardless",         2,   821 },
        { "by",                 2,  3512 }, { "regardless",         1,  1072 },
        { "by",                 2,  3505 }, { "peaceful",           2,   851 },
        { "by",                 2,  3460 }, { "peaceful",           2,    26 },
        { "by",                 2,  3433 }, { "peaceful",           1,  1085 },
        { "by",                 2,  3416 }, { "directly",           1,  1116 },
        { "by",                 2,  3389 }, { "freely",             2,  2775 },
        { "by",                 2,  3372 }, { "freely",             2,  1514 },
        { "by",                 2,  3338 }, { "freely",             2,  1043 },
        { "by",                 2,  3331 }, { "freely",             1,  1565 },
        { "by",                 2,  3310 }, { "freely",             1,  1119 },
        { "by",                 2,  3151 }, { "chosen",             2,  1044 },
        { "by",                 2,  2945 }, { "chosen",             1,  1120 },
        { "by",                 2,  2937 }, { "representatives",    2,  1681 },
        { "by",                 2,  2896 }, { "representatives",    1,  1121 },
        { "by",                 2,  2854 }, { "access",             2,  2981 },
        { "by",                 2,  2563 }, { "access",             2,  2646 },
        { "by",                 2,  2559 }, { "access",             2,  2618 },
        { "by",                 2,  2471 }, { "access",             2,  2513 },
        { "by",                 2,  2383 }, { "access",             2,  2223 },
        { "by",                 2,  2183 }, { "access",             2,  2166 },
        { "by",                 2,  2149 }, { "access",             2,  1781 },
        { "by",                 2,  2070 }, { "access",             2,  1772 },
        { "by",                 2,  1704 }, { "access",             2,   947 },
        { "by",                 2,  1543 }, { "access",             2,   639 },
        { "by",                 2,  1200 }, { "access",             1,  1129 },
        { "by",                 2,  1179 }, { "service",            2,  1786 },
        { "by",                 2,   817 }, { "service",            1,  1132 },
        { "by",                 2,   667 }, { "authority",          2,   819 },
        { "by",                 2,   632 }, { "authority",          2,   670 },
        { "by",                 2,   429 }, { "authority",          1,  1148 },
        { "by",                 2,   274 }, { "periodic",           1,  1346 },
        { "by",                 2,   270 }, { "periodic",           1,  1157 },
        { "by",                 2,   196 }, { "genuine",            1,  1159 },
        { "by",                 2,    86 }, { "suffrage",           2,  2386 },
        { "by",                 2,    78 }, { "suffrage",           1,  1168 },
        { "by",                 1,  1684 }, { "FREEDOMS",           2,   540 },
        { "by",                 1,  1305 }, { "secret",             2,  2391 },
        { "by",                 1,  1177 }, { "secret",             1,  1174 },
        { "by",                 1,  1173 }, { "vote",               2,  2416 },
        { "by",                 1,  1164 }, { "vote",               2,  2397 },
        { "by",                 1,   956 }, { "vote",               2,  2342 },
        { "by",                 1,   608 }, { "vote",               2,  2319 },
        { "by",                 1,   579 }, { "vote",               1,  1175 },
        { "by",                 1,   575 }, { "equivalent",         2,  1098 },
        { "by",                 1,   562 }, { "equivalent",         1,  1178 },
        { "by",                 1,   296 }, { "voting",             1,  1180 },
        { "by",                 1,   283 }, { "Taxes",              0,   769 },
        { "by",                 1,   121 }, { "procedures",         2,   426 },
        { "by",                 0,  1237 }, { "procedures",         1,  1181 },
        { "by",                 0,  1145 }, { "member",             1,  1187 },
        { "by",                 0,  1108 }, { "effort",             1,  1203 },
        { "by",                 0,  1069 }, { "accordance",         2,  3282 },
        { "by",                 0,  1059 }, { "accordance",         2,  3220 },
        { "by",                 0,   995 }, { "accordance",         2,  2793 },
        { "by",                 0,   888 }, { "accordance",         2,  2573 },
        { "by",                 0,   786 }, { "accordance",         2,  2290 },
        { "by",                 0,   735 }, { "accordance",         2,  2239 },
        { "by",                 0,   710 }, { "accordance",         2,  2143 },
        { "by",                 0,   600 }, { "accordance",         2,  2098 },
        { "by",                 0,   235 }, { "accordance",         2,  2064 },
        { "by",                 0,   104 }, { "accordance",         2,  1807 },
        { "do",                 0,  1330 }, { "accordance",         2,  1729 },
        { "do",                 0,  1318 }, { "accordance",         2,  1528 },
        { "do",                 0,  1232 }, { "accordance",         2,  1259 },
        { "necessity",          0,  1183 }, { "accordance",         2,  1120 },
        { "necessity",          0,   303 }, { "accordance",         2,  1004 },
        { "old",                2,  2053 }, { "accordance",         2,   772 },
        { "old",                1,  1399 }, { "accordance",         2,   697 },
        { "non",                1,   835 }, { "accordance",         1,  1210 },
        { "non",                1,   432 }, { "resources",          2,  2969 },
        { "created",            0,    98 }, { "resources",          2,  2141 },
        { "all",                2,  3522 }, { "resources",          1,  1215 },
        { "all",                2,  2204 }, { "economic",           2,  2228 },
        { "all",                2,  2136 }, { "economic",           2,  2216 },
        { "all",                2,  1536 }, { "economic",           2,  1974 },
        { "all",                2,  1453 }, { "economic",           2,  1938 },
        { "all",                2,   859 }, { "economic",           1,  1221 },
        { "all",                1,  1519 }, { "work",               2,  1942 },
        { "all",                1,  1480 }, { "work",               2,  1925 },
        { "all",                1,   660 }, { "work",               2,  1879 },
        { "all",                1,   475 }, { "work",               2,  1457 },
        { "all",                1,   366 }, { "work",               2,  1085 },
        { "all",                1,   262 }, { "work",               2,  1061 },
        { "all",                1,   259 }, { "work",               2,  1038 },
        { "all",                1,    21 }, { "work",               2,  1029 },
        { "all",                0,  1319 }, { "work",               1,  1276 },
        { "all",                0,  1278 }, { "work",               1,  1257 },
        { "all",                0,  1270 }, { "work",               1,  1245 },
        { "all",                0,  1031 }, { "choice",             1,  1248 },
        { "all",                0,   879 }, { "Nature",             0,    58 },
        { "all",                0,   761 }, { "favourable",         2,  1912 },
        { "all",                0,   543 }, { "favourable",         1,  1287 },
        { "all",                0,   331 }, { "favourable",         1,  1254 },
        { "all",                0,   216 }, { "unemployment",       1,  1395 },
        { "all",                0,    95 }, { "unemployment",       1,  1262 },
        { "into",               2,  2281 }, { "pay",                2,  1459 },
        { "into",               2,  1520 }, { "pay",                1,  1349 },
        { "into",               1,   927 }, { "pay",                1,  1273 },
        { "into",               0,   840 }, { "who",                2,  3019 },
        { "into",               0,   475 }, { "who",                2,  2995 },
        { "fatiguing",          0,   473 }, { "who",                2,  2966 },
        { "childhood",          1,  1414 }, { "who",                2,  2138 },
        { "impel",              0,    81 }, { "who",                2,  1081 },
        { "kept",               0,   661 }, { "who",                1,  1279 },
        { "Captive",            0,   971 }, { "remuneration",       1,  1288 },
        { "That",               0,  1250 }, { "ensuring",           1,  1289 },
        { "That",               0,   142 }, { "himself",            1,  1370 },
        { "That",               0,   122 }, { "himself",            1,  1291 },
        { "such",               2,  2974 }, { "existence",          2,  2134 },
        { "such",               2,  2045 }, { "existence",          1,  1296 },
        { "such",               2,  1906 }, { "worthy",             1,  1297 },
        { "such",               2,  1498 }, { "Civil",              0,   690 },
        { "such",               2,  1347 }, { "means",              1,  1307 },
        { "such",               2,  1013 }, { "tribunals",          1,   566 },
        { "such",               2,   458 }, { "join",               2,   880 },
        { "such",               1,  1679 }, { "join",               1,  1320 },
        { "such",               1,   761 }, { "interests",          2,  2523 },
        { "such",               1,   550 }, { "interests",          2,  1764 },
        { "such",               1,   381 }, { "interests",          2,  1591 },
        { "such",               0,   509 }, { "interests",          2,  1552 },
        { "such",               0,   299 }, { "interests",          2,   890 },
        { "such",               0,   278 }, { "interests",          1,  1601 },
        { "such",               0,   182 }, { "interests",          1,  1328 },
        { "such",               0,   175 }, { "leisure",            1,  1338 },
        { "seeks",              2,   147 }, { "reasonable",         2,  2935 },
        { "tyrants",            0,   446 }, { "reasonable",         2,  2469 },
        { "declare",            0,  1249 }, { "reasonable",         1,  1340 },
        { "declare",            0,    77 }, { "working",            2,  1928 },
        { "offences",           2,  3035 }, { "working",            2,  1851 },
        { "offences",           0,   799 }, { "working",            2,  1830 },
        { "should",             1,   350 }, { "working",            2,  1821 },
        { "should",             1,   118 }, { "working",            2,  1096 },
        { "should",             0,   746 }, { "working",            1,  1343 },
        { "should",             0,   395 }, { "hours",              2,  1852 },
        { "should",             0,   205 }, { "hours",              1,  1344 },
        { "should",             0,    76 }, { "holidays",           1,  1347 },
        { "refusing",           0,   601 }, { "30",                 2,  1788 },
        { "refusing",           0,   574 }, { "30",                 1,  1743 },
        { "seem",               0,   188 }, { "25",                 2,  1593 },
        { "expressing",         2,   899 }, { "25",                 1,  1351 },
        { "mankind",            1,    56 }, { "living",             1,  1361 },
        { "mankind",            0,  1197 }, { "being",              2,  2953 },
        { "mankind",            0,   221 }, { "being",              2,  1508 },
        { "mankind",            0,    72 }, { "being",              2,  1185 },
        { "patient",            0,   293 }, { "being",              1,  1368 },
        { "respect",            2,  3341 }, { "food",               1,  1376 },
        { "respect",            2,  3270 }, { "clothing",           1,  1377 },
        { "respect",            2,  1833 }, { "housing",            2,  2126 },
        { "respect",            2,  1431 }, { "housing",            1,  1378 },
        { "respect",            2,  1232 }, { "services",           2,  2225 },
        { "respect",            2,   973 }, { "services",           2,  2213 },
        { "respect",            2,   571 }, { "services",           2,  2040 },
        { "respect",            2,   389 }, { "services",           2,  1775 },
        { "respect",            1,  1695 }, { "services",           2,  1071 },
        { "respect",            1,  1504 }, { "services",           2,   161 },
        { "respect",            1,   289 }, { "services",           1,  1385 },
        { "respect",            1,   209 }, { "event",              2,  1792 },
        { "respect",            0,    67 }, { "event",              2,  1272 },
        { "worship",            2,   758 }, { "event",              1,  1393 },
        { "worship",            1,  1035 }, { "sickness",           1,  1396 },
        { "decent",             2,  2133 }, { "DECLARATION",        1,   248 },
        { "decent",             0,    66 }, { "widowhood",          1,  1398 },
        { "God",                0,    62 }, { "ensured",            2,  2288 },
        { "rights",             2,  3583 }, { "ensured",            2,  2197 },
        { "rights",             2,  3553 }, { "ensured",            2,  1451 },
        { "rights",             2,  3493 }, { "lack",               2,  2967 },
        { "rights",             2,  3451 }, { "lack",               2,  2139 },
        { "rights",             2,  3431 }, { "lack",               1,  1403 },
        { "rights",             2,  3427 }, { "livelihood",         1,  1405 },
        { "rights",             2,  3381 }, { "control",            2,   666 },
        { "rights",             2,  3346 }, { "control",            1,  1410 },
        { "rights",             2,  3327 }, { "Motherhood",         1,  1412 },
        { "rights",             2,  3318 }, { "special",            2,  1407 },
        { "rights",             2,  3272 }, { "special",            1,  1418 },
        { "rights",             2,  3013 }, { "assistance",         2,  2127 },
        { "rights",             2,  2892 }, { "assistance",         2,  2025 },
        { "rights",             2,  1605 }, { "assistance",         1,  1421 },
        { "rights",             2,  1595 }, { "children",           2,  1883 },
        { "rights",             2,  1487 }, { "children",           2,  1540 },
        { "rights",             2,   707 }, { "children",           2,   990 },
        { "rights",             2,   326 }, { "children",           1,  1557 },
        { "rights",             2,   303 }, { "children",           1,  1423 },
        { "rights",             2,   228 }, { "wedlock",            1,  1430 },
        { "rights",             2,   199 }, { "31",                 2,  1817 },
        { "rights",             2,   181 }, { "31",                 2,  1248 },
        { "rights",             1,  1779 }, { "26",                 2,  1626 },
        { "rights",             1,  1723 }, { "26",                 1,  1438 },
        { "rights",             1,  1698 }, { "Education",          1,  1487 },
        { "rights",             1,  1670 }, { "Education",          1,  1446 },
        { "rights",             1,  1630 }, { "elementary",         1,  1454 },
        { "rights",             1,  1507 }, { "stages",             1,  1457 },
        { "rights",             1,  1225 }, { "Elementary",         1,  1458 },
        { "rights",             1,   912 }, { "Technical",          1,  1463 },
        { "rights",             1,   619 }, { "professional",       2,  2528 },
        { "rights",             1,   572 }, { "professional",       2,  1983 },
        { "rights",             1,   368 }, { "professional",       2,  1966 },
        { "rights",             1,   341 }, { "professional",       1,  1465 },
        { "rights",             1,   292 }, { "PROVISIONS",         2,  3227 },
        { "rights",             1,   225 }, { "generally",          1,  1470 },
        { "rights",             1,   215 }, { "labour",             2,  1872 },
        { "rights",             1,   170 }, { "labour",             2,   530 },
        { "rights",             1,   156 }, { "labour",             2,   508 },
        { "rights",             1,   117 }, { "available",          2,  2963 },
        { "rights",             1,    44 }, { "available",          1,  1471 },
        { "rights",             1,    19 }, { "equally",            1,  1477 },
        { "rights",             0,   496 }, { "accessible",         1,  1478 },
        { "rights",             0,   126 }, { "3",                  2,  3419 },
        { "whose",              2,  2891 }, { "3",                  2,  3156 },
        { "whose",              0,  1064 }, { "3",                  2,  2544 },
        { "whose",              0,  1021 }, { "3",                  2,  2107 },
        { "exile",              1,   593 }, { "3",                  2,  1558 },
        { "train",              0,   248 }, { "3",                  2,  1076 },
        { "experience",         0,   217 }, { "3",                  2,   964 },
        { "destructive",        0,   149 }, { "3",                  2,   657 },
        { "can",                1,  1638 }, { "3",                  2,   531 },
        { "assume",             0,    41 }, { "3",                  2,   375 },
        { "in",                 2,  3587 }, { "3",                  1,  1721 },
        { "in",                 2,  3567 }, { "3",                  1,  1539 },
        { "in",                 2,  3555 }, { "3",                  1,  1277 },
        { "in",                 2,  3499 }, { "3",                  1,  1136 },
        { "in",                 2,  3481 }, { "3",                  1,   939 },
        { "in",                 2,  3281 }, { "3",                  1,   443 },
        { "in",                 2,  3219 }, { "merit",              1,  1485 },
        { "in",                 2,  3198 }, { "resides",            2,  2435 },
        { "in",                 2,  3179 }, { "resides",            2,  2364 },
        { "in",                 2,  2970 }, { "their",              2,  3592 },
        { "in",                 2,  2921 }, { "their",              2,  3500 },
        { "in",                 2,  2914 }, { "their",              2,  3284 },
        { "in",                 2,  2834 }, { "their",              2,  2717 },
        { "in",                 2,  2827 }, { "their",              2,  2570 },
        { "in",                 2,  2808 }, { "their",              2,  1960 },
        { "in",                 2,  2792 }, { "their",              2,  1946 },
        { "in",                 2,  2742 }, { "their",              2,  1932 },
        { "in",                 2,  2716 }, { "their",              2,  1763 },
        { "in",                 2,  2692 }, { "their",              2,  1724 },
        { "in",                 2,  2674 }, { "their",              2,  1680 },
        { "in",                 2,  2638 }, { "their",              2,  1650 },
        { "in",                 2,  2610 }, { "their",              2,  1530 },
        { "in",                 2,  2597 }, { "their",              2,  1512 },
        { "in",                 2,  2572 }, { "their",              2,  1506 },
        { "in",                 2,  2566 }, { "their",              2,  1191 },
        { "in",                 2,  2430 }, { "their",              2,   994 },
        { "in",                 2,  2426 }, { "their",              2,   989 },
        { "in",                 2,  2387 }, { "their",              2,   137 },
        { "in",                 2,  2359 }, { "their",              1,  1556 },
        { "in",                 2,  2355 }, { "their",              1,   476 },
        { "in",                 2,  2289 }, { "their",              1,   326 },
        { "in",                 2,  2247 }, { "their",              1,   304 },
        { "in",                 2,  2238 }, { "their",              1,   151 },
        { "in",                 2,  2233 }, { "their",              0,  1135 },
        { "in",                 2,  2198 }, { "their",              0,  1109 },
        { "in",                 2,  2142 }, { "their",              0,   996 },
        { "in",                 2,  2097 }, { "their",              0,   987 },
        { "in",                 2,  2063 }, { "their",              0,   980 },
        { "in",                 2,  2056 }, { "their",              0,   717 },
        { "in",                 2,  2043 }, { "their",              0,   656 },
        { "in",                 2,  1806 }, { "their",              0,   632 },
        { "in",                 2,  1790 }, { "their",              0,   624 },
        { "in",                 2,  1751 }, { "their",              0,   580 },
        { "in",                 2,  1728 }, { "their",              0,   532 },
        { "in",                 2,  1695 }, { "their",              0,   465 },
        { "in",                 2,  1692 }, { "their",              0,   390 },
        { "in",                 2,  1658 }, { "their",              0,   309 },
        { "in",                 2,  1620 }, { "their",              0,   286 },
        { "in",                 2,  1527 }, { "their",              0,   273 },
        { "in",                 2,  1477 }, { "their",              0,   269 },
        { "in",                 2,  1452 }, { "their",              0,   193 },
        { "in",                 2,  1270 }, { "their",              0,   133 },
        { "in",                 2,  1258 }, { "their",              0,   105 },
        { "in",                 2,  1202 }, { "directed",           1,  1490 },
        { "in",                 2,  1187 }, { "It",                 2,   344 },
        { "in",                 2,  1170 }, { "It",                 2,    68 },
        { "in",                 2,  1165 }, { "It",                 1,  1511 },
        { "in",                 2,  1119 }, { "racial",             1,  1521 },
        { "in",                 2,  1086 }, { "religious",          2,  1433 },
        { "in",                 2,  1072 }, { "religious",          2,  1424 },
        { "in",                 2,  1037 }, { "religious",          2,   995 },
        { "in",                 2,  1028 }, { "religious",          1,  1523 },
        { "in",                 2,  1003 }, { "supplemented",       1,  1302 },
        { "in",                 2,   991 }, { "further",            1,  1527 },
        { "in",                 2,   863 }, { "acquired",           2,  1152 },
        { "in",                 2,   861 }, { "activities",         2,  2694 },
        { "in",                 2,   771 }, { "activities",         2,  2208 },
        { "in",                 2,   757 }, { "activities",         2,    77 },
        { "in",                 2,   750 }, { "activities",         1,  1529 },
        { "in",                 2,   747 }, { "maintenance",        2,  1468 },
        { "in",                 2,   742 }, { "maintenance",        1,  1536 },
        { "in",                 2,   696 }, { "Parents",            1,  1540 },
        { "in",                 2,   533 }, { "prior",              1,  1543 },
        { "in",                 2,   515 }, { "choose",             2,  1021 },
        { "in",                 2,   437 }, { "choose",             1,  1546 },
        { "in",                 2,   411 }, { "given",              1,  1554 },
        { "in",                 2,   232 }, { "32",                 2,  1868 },
        { "in",                 2,   202 }, { "27",                 2,  1668 },
        { "in",                 2,   187 }, { "27",                 1,  1559 },
        { "in",                 2,   182 }, { "share",              2,    24 },
        { "in",                 2,    13 }, { "share",              1,  1581 },
        { "in",                 1,  1763 }, { "restricting",        2,  3488 },
        { "in",                 1,  1745 }, { "Freedom",            2,  2784 },
        { "in",                 1,  1727 }, { "Freedom",            2,  2756 },
        { "in",                 1,  1717 }, { "Freedom",            2,  1108 },
        { "in",                 1,  1651 }, { "Freedom",            2,  1019 },
        { "in",                 1,  1635 }, { "Freedom",            2,   911 },
        { "in",                 1,  1627 }, { "Freedom",            2,   837 },
        { "in",                 1,  1582 }, { "Freedom",            2,   785 },
        { "in",                 1,  1568 }, { "Freedom",            2,   710 },
        { "in",                 1,  1452 }, { "scientific",         2,   920 },
        { "in",                 1,  1426 }, { "scientific",         2,   192 },
        { "in",                 1,  1406 }, { "scientific",         1,  1605 },
        { "in",                 1,  1391 }, { "scientific",         1,  1583 },
        { "in",                 1,  1209 }, { "innocence",          2,  2988 },
        { "in",                 1,  1156 }, { "moral",              2,  1952 },
        { "in",                 1,  1133 }, { "moral",              2,    37 },
        { "in",                 1,  1110 }, { "moral",              1,  1598 },
        { "in",                 1,  1032 }, { "material",           1,  1600 },
        { "in",                 1,  1022 }, { "resulting",          1,  1602 },
        { "in",                 1,  1017 }, { "literary",           1,  1606 },
        { "in",                 1,   975 }, { "artistic",           1,  1608 },
        { "in",                 1,   827 }, { "production",         1,  1609 },
        { "in",                 1,   814 }, { "author",             1,  1615 },
        { "in",                 1,   651 }, { "33",                 2,  1963 },
        { "in",                 1,   614 }, { "separation",         0,    85 },
        { "in",                 1,   599 }, { "28",                 2,  1713 },
        { "in",                 1,   540 }, { "28",                 2,  1241 },
        { "in",                 1,   474 }, { "28",                 1,  1617 },
        { "in",                 1,   462 }, { "order",              2,  2248 },
        { "in",                 1,   373 }, { "order",              2,  2109 },
        { "in",                 1,   355 }, { "order",              1,  1712 },
        { "in",                 1,   338 }, { "order",              1,  1626 },
        { "in",                 1,   279 }, { "fully",              1,  1640 },
        { "in",                 1,   198 }, { "realized",           1,  1641 },
        { "in",                 1,   187 }, { "duties",             2,  2571 },
        { "in",                 1,   167 }, { "duties",             2,   307 },
        { "in",                 1,   157 }, { "duties",             1,  1647 },
        { "in",                 1,   153 }, { "possible",           1,  1663 },
        { "in",                 1,   147 }, { "States'",            2,  3545 },
        { "in",                 1,    63 }, { "everyone",           2,  1985 },
        { "in",                 1,    47 }, { "everyone",           2,   875 },
        { "in",                 1,    35 }, { "everyone",           1,  1673 },
        { "in",                 0,  1233 }, { "inestimable",        0,   440 },
        { "in",                 0,  1214 }, { "limitations",        2,  3355 },
        { "in",                 0,  1201 }, { "limitations",        1,  1680 },
        { "in",                 0,  1199 }, { "solely",             1,  1686 },
        { "in",                 0,  1181 }, { "securing",           1,  1691 },
        { "in",                 0,  1092 }, { "meeting",            1,  1705 },
        { "in",                 0,  1047 }, { "requirements",       1,  1708 },
        { "in",                 0,   949 }, { "morality",           1,  1710 },
        { "in",                 0,   878 }, { "general",            2,  3369 },
        { "in",                 0,   809 }, { "general",            2,  3148 },
        { "in",                 0,   778 }, { "general",            2,  2576 },
        { "in",                 0,   664 }, { "general",            2,  2227 },
        { "in",                 0,   537 }, { "general",            2,  2215 },
        { "in",                 0,   435 }, { "general",            2,  1210 },
        { "in",                 0,   389 }, { "general",            1,  1715 },
        { "in",                 0,   333 }, { "welfare",            1,  1716 },
        { "in",                 0,   181 }, { "democratic",         2,   975 },
        { "in",                 0,    16 }, { "democratic",         1,  1719 },
        { "Guards",             0,   284 }, { "These",              1,  1722 },
        { "Liberty",            0,   116 }, { "exercised",          2,  3407 },
        { "or",                 2,  3590 }, { "exercised",          1,  1731 },
        { "or",                 2,  3570 }, { "interpreted",        2,  3560 },
        { "or",                 2,  3521 }, { "interpreted",        2,  3486 },
        { "or",                 2,  3489 }, { "interpreted",        1,  1750 },
        { "or",                 2,  3399 }, { "activity",           2,  3569 },
        { "or",                 2,  3375 }, { "activity",           1,  1765 },
        { "or",                 2,  3304 }, { "aimed",              2,  3575 },
        { "or",                 2,  3301 }, { "aimed",              1,  1771 },
        { "or",                 2,  3296 }, { "herein",             2,  3602 },
        { "or",                 2,  3214 }, { "herein",             1,  1784 },
        { "or",                 2,  3207 }, { "CHARTER",            2,     0 },
        { "or",                 2,  3195 }, { "according",          2,  3145 },
        { "or",                 2,  3176 }, { "according",          2,  3006 },
        { "or",                 2,  3133 }, { "according",          2,   423 },
        { "or",                 2,  3066 }, { "according",          1,   648 },
        { "or",                 2,  3054 }, { "FUNDAMENTAL",        2,     2 },
        { "or",                 2,  2857 }, { "UNION",              2,     7 },
        { "or",                 2,  2842 }, { "sexual",             2,  1377 },
        { "or",                 2,  2737 }, { "PREAMBLE",           2,     8 },
        { "or",                 2,  2733 }, { "Europe",             2,   278 },
        { "or",                 2,  2699 }, { "Europe",             2,   122 },
        { "or",                 2,  2669 }, { "Europe",             2,    12 },
        { "or",                 2,  2665 }, { "creating",           2,    87 },
        { "or",                 2,  2633 }, { "creating",           2,    14 },
        { "or",                 2,  2629 }, { "ever",               2,    16 },
        { "or",                 2,  2562 }, { "closer",             2,    17 },
        { "or",                 2,  2516 }, { "union",              2,   866 },
        { "or",                 2,  2500 }, { "union",              2,    18 },
        { "or",                 2,  2460 }, { "resolved",           2,    22 },
        { "or",                 2,  2433 }, { "based",              2,  3394 },
        { "or",                 2,  2362 }, { "based",              2,  1343 },
        { "or",                 2,  2052 }, { "based",              2,    57 },
        { "or",                 2,  2014 }, { "based",              2,    28 },
        { "or",                 2,  1956 }, { "values",             2,   108 },
        { "or",                 2,  1953 }, { "values",             2,    47 },
        { "or",                 2,  1949 }, { "values",             2,    31 },
        { "or",                 2,  1835 }, { "Conscious",          2,    32 },
        { "or",                 2,  1723 }, { "heritage",           2,    38 },
        { "or",                 2,  1679 }, { "Union",              2,  3518 },
        { "or",                 2,  1589 }, { "Union",              2,  3506 },
        { "or",                 2,  1580 }, { "Union",              2,  3469 },
        { "or",                 2,  1546 }, { "Union",              2,  3404 },
        { "or",                 2,  1469 }, { "Union",              2,  3374 },
        { "or",                 2,  1376 }, { "Union",              2,  3303 },
        { "or",                 2,  1363 }, { "Union",              2,  3265 },
        { "or",                 2,  1360 }, { "Union",              2,  3246 },
        { "or",                 2,  1353 }, { "Union",              2,  3218 },
        { "or",                 2,  1319 }, { "Union",              2,  2901 },
        { "or",                 2,  1316 }, { "Union",              2,  2825 },
        { "or",                 2,  1313 }, { "Union",              2,  2767 },
        { "or",                 2,  1303 }, { "Union",              2,  2729 },
        { "or",                 2,  1290 }, { "Union",              2,  2688 },
        { "or",                 2,  1276 }, { "Union",              2,  2661 },
        { "or",                 2,  1161 }, { "Union",              2,  2625 },
        { "or",                 2,  1149 }, { "Union",              2,  2596 },
        { "or",                 2,  1045 }, { "Union",              2,  2478 },
        { "or",                 2,   888 }, { "Union",              2,  2411 },
        { "or",                 2,   755 }, { "Union",              2,  2337 },
        { "or",                 2,   749 }, { "Union",              2,  2301 },
        { "or",                 2,   741 }, { "Union",              2,  2286 },
        { "or",                 2,   735 }, { "Union",              2,  2258 },
        { "or",                 2,   648 }, { "Union",              2,  2219 },
        { "or",                 2,   625 }, { "Union",              2,  2205 },
        { "or",                 2,   602 }, { "Union",              2,  2117 },
        { "or",                 2,   574 }, { "Union",              2,  2087 },
        { "or",                 2,   528 }, { "Union",              2,  2028 },
        { "or",                 2,   517 }, { "Union",              2,  1633 },
        { "or",                 2,   499 }, { "Union",              2,  1600 },
        { "or",                 2,   496 }, { "Union",              2,  1429 },
        { "or",                 2,   493 }, { "Union",              2,  1401 },
        { "or",                 2,   484 }, { "Union",              2,  1105 },
        { "or",                 2,   481 }, { "Union",              2,  1053 },
        { "or",                 2,   392 }, { "Union",              2,   908 },
        { "or",                 2,   372 }, { "Union",              2,   895 },
        { "or",                 1,  1766 }, { "Union",              2,   322 },
        { "or",                 1,  1757 }, { "Union",              2,   250 },
        { "or",                 1,  1607 }, { "Union",              2,   221 },
        { "or",                 1,  1522 }, { "Union",              2,    96 },
        { "or",                 1,  1427 }, { "Union",              2,    84 },
        { "or",                 1,  1401 }, { "Union",              2,    40 },
        { "or",                 1,  1176 }, { "distant",            0,   460 },
        { "or",                 1,  1117 }, { "founded",            2,    42 },
        { "or",                 1,  1030 }, { "standards",          1,   184 },
        { "or",                 1,  1024 }, { "indivisible",        2,    45 },
        { "or",                 1,  1016 }, { "solidarity",         2,    54 },
        { "or",                 1,  1010 }, { "democracy",          2,    62 },
        { "or",                 1,   895 }, { "heart",              2,    74 },
        { "or",                 1,   838 }, { "citizenship",        2,    81 },
        { "or",                 1,   763 }, { "area",               2,    89 },
        { "or",                 1,   740 }, { "contributes",        2,    97 },
        { "or",                 1,   694 }, { "preservation",       2,   100 },
        { "or",                 1,   683 }, { "once",               0,   828 },
        { "or",                 1,   592 }, { "respecting",         2,  2520 },
        { "or",                 1,   578 }, { "respecting",         2,   110 },
        { "or",                 1,   494 }, { "diversity",          2,  1436 },
        { "or",                 1,   491 }, { "diversity",          2,  1427 },
        { "or",                 1,   487 }, { "diversity",          2,   112 },
        { "or",                 1,   464 }, { "identities",         2,   128 },
        { "or",                 1,   435 }, { "organisation",       2,   135 },
        { "or",                 1,   420 }, { "regional",           2,   142 },
        { "or",                 1,   414 }, { "levels",             2,  1749 },
        { "or",                 1,   398 }, { "levels",             2,  1686 },
        { "or",                 1,   393 }, { "levels",             2,   860 },
        { "or",                 1,   389 }, { "levels",             2,   145 },
        { "or",                 0,   991 }, { "balanced",           2,   150 },
        { "or",                 0,   162 }, { "pluralism",          2,   828 },
        { "becomes",            0,   148 }, { "from",               2,  2176 },
        { "becomes",            0,    23 }, { "from",               2,  1992 },
        { "denied",             1,   872 }, { "from",               2,  1645 },
        { "People",             0,  1242 }, { "from",               2,   234 },
        { "People",             0,   528 }, { "from",               1,  1603 },
        { "People",             0,   159 }, { "from",               1,   839 },
        { "any",                2,  3580 }, { "from",               1,   834 },
        { "any",                2,  3573 }, { "from",               1,   818 },
        { "any",                2,  3568 }, { "from",               1,    76 },
        { "any",                2,  3563 }, { "from",               0,  1269 },
        { "any",                2,  3293 }, { "from",               0,  1102 },
        { "any",                2,  3131 }, { "from",               0,   739 },
        { "any",                2,  3128 }, { "from",               0,   548 },
        { "any",                2,  3052 }, { "from",               0,   461 },
        { "any",                2,  3046 }, { "from",               0,   136 },
        { "any",                2,  2861 }, { "persons",            2,  1640 },
        { "any",                2,  2731 }, { "persons",            2,  1629 },
        { "any",                2,  2663 }, { "persons",            2,   445 },
        { "any",                2,  2627 }, { "persons",            2,   312 },
        { "any",                2,  2556 }, { "persons",            2,   159 },
        { "any",                2,  2493 }, { "goods",              2,   160 },
        { "any",                2,  1941 }, { "capital",            2,   163 },
        { "any",                2,  1412 }, { "omission",           2,  3134 },
        { "any",                2,  1364 }, { "omission",           2,  3055 },
        { "any",                2,  1345 }, { "omission",           1,   684 },
        { "any",                2,  1073 }, { "strengthen",         2,   176 },
        { "any",                1,  1776 }, { "changes",            2,   186 },
        { "any",                1,  1769 }, { "inhuman",            2,  1315 },
        { "any",                1,  1764 }, { "inhuman",            2,   495 },
        { "any",                1,  1759 }, { "inhuman",            2,   480 },
        { "any",                1,  1754 }, { "inhuman",            1,   490 },
        { "any",                1,  1604 }, { "technological",      2,   194 },
        { "any",                1,  1266 }, { "developments",       2,   195 },
        { "any",                1,  1069 }, { "exploitation",       2,  1939 },
        { "any",                1,   889 }, { "making",             2,   450 },
        { "any",                1,   791 }, { "making",             2,   197 },
        { "any",                1,   681 }, { "reaffirms",          2,   207 },
        { "any",                1,   675 }, { "tasks",              2,  3308 },
        { "any",                1,   624 }, { "tasks",              2,   215 },
        { "any",                1,   547 }, { "elections",          2,  2425 },
        { "any",                1,   538 }, { "elections",          2,  2406 },
        { "any",                1,   523 }, { "elections",          2,  2350 },
        { "any",                1,   437 }, { "elections",          2,  2327 },
        { "any",                1,   379 }, { "elections",          1,  1160 },
        { "any",                0,   742 }, { "Community",          2,  3520 },
        { "any",                0,   144 }, { "Community",          2,  3397 },
        { "are",                2,  3526 }, { "Community",          2,  3300 },
        { "are",                2,  3393 }, { "Community",          2,  2800 },
        { "are",                2,  3362 }, { "Community",          2,  2697 },
        { "are",                2,  3263 }, { "Community",          2,  2553 },
        { "are",                2,  3237 }, { "Community",          2,  2246 },
        { "are",                2,  2902 }, { "Community",          2,  2150 },
        { "are",                2,  1281 }, { "Community",          2,  2100 },
        { "are",                2,  1093 }, { "Community",          2,  2071 },
        { "are",                2,  1082 }, { "Community",          2,  1809 },
        { "are",                2,    21 }, { "Community",          2,  1731 },
        { "are",                1,  1682 }, { "Community",          2,  1705 },
        { "are",                1,  1415 }, { "Community",          2,  1394 },
        { "are",                1,   908 }, { "Community",          2,  1266 },
        { "are",                1,   532 }, { "Community",          2,  1122 },
        { "are",                1,   520 }, { "Community",          2,   272 },
        { "are",                1,   514 }, { "Community",          2,   252 },
        { "are",                1,   343 }, { "Community",          2,   218 },
        { "are",                1,   333 }, { "establishing",       2,  2797 },
        { "are",                0,  1267 }, { "establishing",       2,  2243 },
        { "are",                0,  1254 }, { "establishing",       2,  1391 },
        { "are",                0,   242 }, { "establishing",       2,  1263 },
        { "are",                0,   229 }, { "establishing",       2,    79 },
        { "are",                0,   222 }, { "establishing",       0,   813 },
        { "are",                0,   128 }, { "establishing",       0,   607 },
        { "are",                0,   114 }, { "principle",          2,  3352 },
        { "are",                0,   102 }, { "principle",          2,  3252 },
        { "are",                0,    97 }, { "principle",          2,  2293 },
        { "it",                 2,  3140 }, { "principle",          2,  1461 },
        { "it",                 2,  3073 }, { "principle",          2,   224 },
        { "it",                 2,   655 }, { "result",             2,   231 },
        { "it",                 2,   172 }, { "particular",         2,   862 },
        { "it",                 2,   146 }, { "particular",         2,   438 },
        { "it",                 2,    55 }, { "particular",         2,   412 },
        { "it",                 1,   701 }, { "particular",         2,   233 },
        { "it",                 1,   428 }, { "constitutional",     2,   236 },
        { "it",                 1,   127 }, { "Treaty",             2,  3401 },
        { "it",                 1,    92 }, { "Treaty",             2,  2796 },
        { "it",                 0,   826 }, { "Treaty",             2,  2242 },
        { "it",                 0,   271 }, { "Treaty",             2,  1398 },
        { "it",                 0,   267 }, { "Treaty",             2,  1390 },
        { "it",                 0,   165 }, { "Treaty",             2,  1262 },
        { "it",                 0,   153 }, { "Treaty",             2,   247 },
        { "it",                 0,    22 }, { "European",           2,  3530 },
        { "Redress",            0,  1046 }, { "European",           2,  3403 },
        { "self",               1,   433 }, { "European",           2,  2799 },
        { "self",               0,    92 }, { "European",           2,  2752 },
        { "transient",          0,   212 }, { "European",           2,  2648 },
        { "recognition",        1,  1693 }, { "European",           2,  2378 },
        { "recognition",        1,   503 }, { "European",           2,  2353 },
        { "recognition",        1,   308 }, { "European",           2,  2330 },
        { "recognition",        1,     8 }, { "European",           2,  2245 },
        { "towns",              0,   911 }, { "European",           2,  2086 },
        { "strengthening",      1,  1502 }, { "European",           2,  1400 },
        { "*IN*",               0,     0 }, { "European",           2,  1393 },
        { "servitude",          2,   518 }, { "European",           2,  1265 },
        { "servitude",          1,   465 }, { "European",           2,   295 },
        { "light",              2,   184 }, { "European",           2,   290 },
        { "light",              0,   210 }, { "European",           2,   255 },
        { "Right",              2,  3171 }, { "European",           2,   249 },
        { "Right",              2,  2880 }, { "Treaties",           2,  3418 },
        { "Right",              2,  2722 }, { "Treaties",           2,  3398 },
        { "Right",              2,  2616 }, { "Treaties",           2,  3312 },
        { "Right",              2,  2447 }, { "Treaties",           2,  2604 },
        { "Right",              2,  2395 }, { "Treaties",           2,  1411 },
        { "Right",              2,  2317 }, { "Treaties",           2,   253 },
        { "Right",              2,  1770 }, { "Convention",         2,  3531 },
        { "Right",              2,  1714 }, { "Convention",         2,  3463 },
        { "Right",              2,  1220 }, { "Convention",         2,  3435 },
        { "Right",              2,  1133 }, { "Convention",         2,  1239 },
        { "Right",              2,   934 }, { "Convention",         2,   256 },
        { "Right",              2,   673 }, { "Fundamental",        2,  3539 },
        { "Right",              2,   543 }, { "Fundamental",        2,  3443 },
        { "Right",              2,   376 }, { "Fundamental",        2,   264 },
        { "Right",              2,   352 }, { "more",               2,  3472 },
        { "Right",              0,  1257 }, { "more",               2,  1911 },
        { "Right",              0,   156 }, { "more",               2,   200 },
        { "*1776*",             0,     4 }, { "more",               0,   223 },
        { "alone",              2,   740 }, { "Freedoms",           2,  3540 },
        { "alone",              1,  1653 }, { "Freedoms",           2,  3444 },
        { "alone",              1,  1015 }, { "Freedoms",           2,   265 },
        { "alone",              1,   971 }, { "Social",             2,  2021 },
        { "alone",              0,   619 }, { "Social",             2,   267 },
        { "inevitably",         0,  1158 }, { "cruel",              1,   489 },
        { "her",                2,  2517 }, { "adopted",            2,   269 },
        { "her",                2,  2501 }, { "Council",            2,  2650 },
        { "her",                2,  2461 }, { "Council",            2,   276 },
        { "her",                2,  1836 }, { "Communities",        2,   291 },
        { "her",                2,  1590 }, { "Indian",             0,  1019 },
        { "her",                2,  1581 }, { "Enjoyment",          2,   300 },
        { "her",                2,  1162 }, { "entails",            2,   304 },
        { "her",                2,  1150 }, { "responsibilities",   2,   305 },
        { "her",                2,   889 }, { "generations",        2,   320 },
        { "her",                2,   649 }, { "recognises",         2,  2220 },
        { "her",                2,   603 }, { "recognises",         2,  2118 },
        { "her",                2,   575 }, { "recognises",         2,  2029 },
        { "her",                2,   393 }, { "recognises",         2,  1634 },
        { "for",                2,  3601 }, { "recognises",         2,  1601 },
        { "for",                2,  3532 }, { "recognises",         2,   324 },
        { "for",                2,  3436 }, { "hereafter",          2,   332 },
        { "for",                2,  3337 }, { "CHAPTER",            2,  3224 },
        { "for",                2,  3298 }, { "CHAPTER",            2,  2875 },
        { "for",                2,  3250 }, { "CHAPTER",            2,  2311 },
        { "for",                2,  3204 }, { "CHAPTER",            2,  1664 },
        { "for",                2,  3201 }, { "CHAPTER",            2,  1321 },
        { "for",                2,  3182 }, { "CHAPTER",            2,   538 },
        { "for",                2,  3130 }, { "CHAPTER",            2,   333 },
        { "for",                2,  3108 }, { "I",                  2,   334 },
        { "for",                2,  3011 }, { "evils",              0,   228 },
        { "for",                2,  2541 }, { "inviolable",         2,   343 },
        { "for",                2,  2232 }, { "respected",          2,  1002 },
        { "for",                2,  2135 }, { "respected",          2,   931 },
        { "for",                2,  1994 }, { "respected",          2,   834 },
        { "for",                2,  1918 }, { "respected",          2,   410 },
        { "for",                2,  1703 }, { "respected",          2,   347 },
        { "for",                2,  1505 }, { "condemned",          2,   367 },
        { "for",                2,  1474 }, { "health",             2,  2193 },
        { "for",                2,  1233 }, { "health",             2,  2169 },
        { "for",                2,  1208 }, { "health",             2,  1948 },
        { "for",                2,  1190 }, { "health",             2,  1837 },
        { "for",                2,  1178 }, { "health",             1,  1365 },
        { "for",                2,   974 }, { "executed",           2,   373 },
        { "for",                2,   883 }, { "integrity",          2,   397 },
        { "for",                2,   611 }, { "integrity",          2,   379 },
        { "for",                2,   572 }, { "impartial",          2,  2941 },
        { "for",                2,   561 }, { "impartial",          1,   612 },
        { "for",                2,   390 }, { "physical",           2,  1950 },
        { "for",                2,   257 }, { "physical",           2,   394 },
        { "for",                2,   211 }, { "mental",             2,  1951 },
        { "for",                1,  1753 }, { "mental",             2,   396 },
        { "for",                1,  1696 }, { "fields",             2,  3502 },
        { "for",                1,  1687 }, { "fields",             2,   401 },
        { "for",                1,  1534 }, { "medicine",           2,   403 },
        { "for",                1,  1505 }, { "following",          2,  2011 },
        { "for",                1,  1363 }, { "following",          2,   407 },
        { "for",                1,  1323 }, { "concerned",          2,   624 },
        { "for",                1,  1290 }, { "concerned",          2,   422 },
        { "for",                1,  1274 }, { "laid",               2,  3458 },
        { "for",                1,  1227 }, { "laid",               2,  2919 },
        { "for",                1,   664 }, { "laid",               2,  2147 },
        { "for",                1,   567 }, { "laid",               2,  2068 },
        { "for",                1,   290 }, { "laid",               2,   630 },
        { "for",                1,   258 }, { "laid",               2,   427 },
        { "for",                1,   233 }, { "down",               2,  3459 },
        { "for",                1,   210 }, { "down",               2,  2920 },
        { "for",                1,    42 }, { "down",               2,  2148 },
        { "for",                0,  1332 }, { "down",               2,  2069 },
        { "for",                0,  1226 }, { "down",               2,   631 },
        { "for",                0,  1045 }, { "down",               2,   428 },
        { "for",                0,   876 }, { "prohibition",        2,   466 },
        { "for",                0,   834 }, { "prohibition",        2,   448 },
        { "for",                0,   797 }, { "prohibition",        2,   433 },
        { "for",                0,   741 }, { "practices",          2,  2237 },
        { "for",                0,   620 }, { "practices",          2,  2187 },
        { "for",                0,   606 }, { "practices",          2,  2156 },
        { "for",                0,   570 }, { "practices",          2,  2106 },
        { "for",                0,   564 }, { "practices",          2,  2077 },
        { "for",                0,   531 }, { "practices",          2,  1815 },
        { "for",                0,   504 }, { "practices",          2,  1737 },
        { "for",                0,   487 }, { "practices",          2,  1711 },
        { "for",                0,   468 }, { "practices",          2,  1128 },
        { "for",                0,   418 }, { "practices",          2,   436 },
        { "for",                0,   369 }, { "aiming",             2,   440 },
        { "for",                0,   285 }, { "selection",          2,   443 },
        { "for",                0,   209 }, { "body",               2,   453 },
        { "for",                0,    25 }, { "source",             2,   460 },
        { "intentions",         0,  1231 }, { "gain",               2,   463 },
        { "Happiness",          0,   196 }, { "reproductive",       2,   469 },
        { "Happiness",          0,   121 }, { "cloning",            2,   470 },
        { "themselves",         1,   318 }, { "Prohibition",        2,  3549 },
        { "themselves",         1,   195 }, { "Prohibition",        2,  1869 },
        { "themselves",         0,   994 }, { "Prohibition",        2,   503 },
        { "themselves",         0,   870 }, { "Prohibition",        2,   476 },
        { "themselves",         0,   234 }, { "forced",             2,   527 },
        { "America",            0,  1213 }, { "forced",             2,   507 },
        { "America",            0,    14 }, { "required",           2,   524 },
        { "Representation",     0,   434 }, { "Trafficking",        2,   532 },
        { "trade",              2,   881 }, { "II",                 2,   539 },
        { "trade",              2,   865 }, { "Respect",            2,  3010 },
        { "trade",              1,  1321 }, { "Respect",            2,   560 },
        { "trade",              1,   470 }, { "communications",     2,   582 },
        { "Murders",            0,   743 }, { "realization",        1,  1200 },
        { "to",                 2,  3594 }, { "realization",        1,   236 },
        { "to",                 2,  3571 }, { "personal",           2,  1572 },
        { "to",                 2,  3565 }, { "personal",           2,   598 },
        { "to",                 2,  3515 }, { "personal",           2,   587 },
        { "to",                 2,  3430 }, { "data",               2,   641 },
        { "to",                 2,  3378 }, { "data",               2,   606 },
        { "to",                 2,  3350 }, { "data",               2,   599 },
        { "to",                 2,  3256 }, { "data",               2,   588 },
        { "to",                 2,  3239 }, { "concerning",         2,   646 },
        { "to",                 2,  3192 }, { "concerning",         2,   600 },
        { "to",                 2,  3173 }, { "processed",          2,   609 },
        { "to",                 2,  3165 }, { "some",               2,   626 },
        { "to",                 2,  3146 }, { "collected",          2,   645 },
        { "to",                 2,  3098 }, { "rectified",          2,   656 },
        { "to",                 2,  3007 }, { "Compliance",         2,   658 },
        { "to",                 2,  2982 }, { "rules",              2,  2146 },
        { "to",                 2,  2978 }, { "rules",              2,  2067 },
        { "to",                 2,  2964 }, { "rules",              2,  1907 },
        { "to",                 2,  2927 }, { "rules",              2,  1235 },
        { "to",                 2,  2907 }, { "rules",              2,   661 },
        { "to",                 2,  2886 }, { "guaranteed",         2,  3432 },
        { "to",                 2,  2881 }, { "guaranteed",         2,  3317 },
        { "to",                 2,  2852 }, { "guaranteed",         2,  3025 },
        { "to",                 2,  2801 }, { "guaranteed",         2,  2895 },
        { "to",                 2,  2771 }, { "guaranteed",         2,  1688 },
        { "to",                 2,  2749 }, { "guaranteed",         2,  1229 },
        { "to",                 2,  2723 }, { "guaranteed",         2,   695 },
        { "to",                 2,  2683 }, { "informed",           2,   417 },
        { "to",                 2,  2681 }, { "conscientious",      2,   767 },
        { "to",                 2,  2647 }, { "recognised",         2,  3586 },
        { "to",                 2,  2619 }, { "recognised",         2,  3498 },
        { "to",                 2,  2591 }, { "recognised",         2,  3388 },
        { "to",                 2,  2579 }, { "recognised",         2,  3371 },
        { "to",                 2,  2550 }, { "recognised",         2,  3330 },
        { "to",                 2,  2538 }, { "recognised",         2,  3150 },
        { "to",                 2,  2514 }, { "recognised",         2,  1130 },
        { "to",                 2,  2511 }, { "recognised",         2,   770 },
        { "to",                 2,  2489 }, { "he",                 2,  3206 },
        { "to",                 2,  2457 }, { "he",                 2,  2841 },
        { "to",                 2,  2448 }, { "he",                 2,  2432 },
        { "to",                 2,  2418 }, { "he",                 2,  2361 },
        { "to",                 2,  2415 }, { "he",                 2,  1302 },
        { "to",                 2,  2399 }, { "he",                 1,  1612 },
        { "to",                 2,  2396 }, { "he",                 1,   657 },
        { "to",                 2,  2351 }, { "he",                 0,   402 },
        { "to",                 2,  2344 }, { "include",            2,   802 },
        { "to",                 2,  2341 }, { "civic",              2,   868 },
        { "to",                 2,  2328 }, { "matters",            2,  1523 },
        { "to",                 2,  2321 }, { "matters",            2,   869 },
        { "to",                 2,  2318 }, { "implies",            2,   871 },
        { "to",                 2,  2249 }, { "Political",          2,   892 },
        { "to",                 2,  2224 }, { "parties",            2,   893 },
        { "to",                 2,  2212 }, { "level",              2,  2307 },
        { "to",                 2,  2174 }, { "level",              2,  2265 },
        { "to",                 2,  2167 }, { "level",              2,  2190 },
        { "to",                 2,  2130 }, { "level",              2,   896 },
        { "to",                 2,  2123 }, { "contribute",         2,   897 },
        { "to",                 2,  2110 }, { "citizens",           2,  1102 },
        { "to",                 2,  2090 }, { "citizens",           2,   905 },
        { "to",                 2,  2034 }, { "constraint",         2,   926 },
        { "to",                 2,  2008 }, { "Academic",           2,   927 },
        { "to",                 2,  2003 }, { "training",           2,   952 },
        { "to",                 2,  1990 }, { "possibility",        2,  2951 },
        { "to",                 2,  1957 }, { "possibility",        2,   958 },
        { "to",                 2,  1944 }, { "educational",        2,   969 },
        { "to",                 2,  1931 }, { "removal",            2,  1274 },
        { "to",                 2,  1924 }, { "establishments",     2,   970 },
        { "to",                 2,  1913 }, { "parents",            2,  1582 },
        { "to",                 2,  1905 }, { "parents",            2,   981 },
        { "to",                 2,  1891 }, { "ensure",             2,  2979 },
        { "to",                 2,  1860 }, { "ensure",             2,  2304 },
        { "to",                 2,  1853 }, { "ensure",             2,  2131 },
        { "to",                 2,  1847 }, { "ensure",             2,  1649 },
        { "to",                 2,  1829 }, { "ensure",             2,   983 },
        { "to",                 2,  1801 }, { "conformity",         2,   992 },
        { "to",                 2,  1782 }, { "philosophical",      2,   996 },
        { "to",                 2,  1773 }, { "pedagogical",        2,   998 },
        { "to",                 2,  1761 }, { "convictions",        2,   999 },
        { "to",                 2,  1757 }, { "occupation",         2,  1047 },
        { "to",                 2,  1740 }, { "occupation",         2,  1023 },
        { "to",                 2,  1671 }, { "accepted",           2,  1046 },
        { "to",                 2,  1648 }, { "Every",              2,  2821 },
        { "to",                 2,  1643 }, { "Every",              2,  2763 },
        { "to",                 2,  1618 }, { "Every",              2,  2587 },
        { "to",                 2,  1609 }, { "Every",              2,  2545 },
        { "to",                 2,  1587 }, { "Every",              2,  2452 },
        { "to",                 2,  1565 }, { "Every",              2,  2407 },
        { "to",                 2,  1539 }, { "Every",              2,  2333 },
        { "to",                 2,  1497 }, { "Every",              2,  1842 },
        { "to",                 2,  1405 }, { "Every",              2,  1824 },
        { "to",                 2,  1308 }, { "Every",              2,  1796 },
        { "to",                 2,  1292 }, { "Every",              2,  1559 },
        { "to",                 2,  1252 }, { "Every",              2,  1049 }
    };
    enum { NUM_CONCORDANCE = sizeof concordance/sizeof *concordance };
//..
// Then, we create 'inverseConcordance', an unordered map, and initialize it
// with values obtained from 'concordance'.
//..
    InverseConcordance inverseConcordance;

    for (int idx = 0; idx < NUM_CONCORDANCE; ++idx) {
        bsl::string word         = concordance[idx].d_word_p;
        int         documentCode = concordance[idx].d_documentCode;
        int         wordOffset   = concordance[idx].d_wordOffset;

        WordLocation                   location(documentCode, wordOffset);
        InverseConcordance::value_type value(location, word);
        bool                           status =
                                       inverseConcordance.insert(value).second;
        ASSERT(status);
    }
//..
// Notice that we expect every 'insert' to be successful, as the concordance
// should not show more than one word at any location.
//
// Next, suppose we knew the location of the word "unalienable" in the document
// set (see {'bslstl_unorderedmultimap'|Example 1}) and want to know its
// context?
//..
//  "unalienable",  0,  109
//..
// We use the 'find' method of 'inverseConcordance' to determine the words
// within offset 'delta' of "unalienable".  Note that we must check the
// validity of the returned interator, in case we probe beyond the boundaries
// of the document.
//..
    const int docCode =   0;
    const int origin  = 109;
    const int delta   =  16;

    for (int offset = origin - delta; offset < origin + delta; ++offset) {
        WordLocation               location(docCode, offset);
        InverseConcordanceConstItr itr = inverseConcordance.find(location);

        if (inverseConcordance.end() != itr) {
if (verbose) {
            printf("%d %4d: %s\n",
                   itr->first.first,
                   itr->first.second,
                   itr->second.c_str());
}
            ASSERT(origin != offset
                || bsl::string("unalienable") == itr->second);
        }
    }
//..
// Notice that the assertion confirms that "unalienable" is found in our
// inverse location at the location we obtained from the concordance.
//
// Finally, we find on standard output:
//..
//  0   93: evident
//  0   94: that
//  0   95: all
//  0   96: men
//  0   97: are
//  0   98: created
//  0   99: equal
//  0  100: that
//  0  101: they
//  0  102: are
//  0  103: endowed
//  0  104: by
//  0  105: their
//  0  106: Creator
//  0  107: with
//  0  108: certain
//  0  109: unalienable
//  0  110: Rights
//  0  111: that
//  0  112: among
//  0  113: these
//  0  114: are
//  0  115: Life
//  0  116: Liberty
//  0  117: and
//  0  118: the
//  0  119: pursuit
//  0  120: of
//  0  121: Happiness
//  0  122: That
//  0  123: to
//  0  124: secure
//..
#else
        if (verbose) Q(Usage Skipped);
#endif
}

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
                            // struct ThrowingSwapHash
                            // =======================

template <class TYPE>
struct ThrowingSwapHash : public bsl::hash<TYPE> {
    // Hash functor with throwing 'swap'.

    // MANIPULATORS
    void swap(
            ThrowingSwapHash& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
         (void)other;
   }

    // FREE FUNCTIONS
    friend void swap(
                ThrowingSwapHash& a,
                ThrowingSwapHash& b) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        (void)a;
        (void)b;
    }
};
                            // ========================
                            // struct ThrowingSwapEqual
                            // ========================

template <class TYPE>
struct ThrowingSwapEqual : public bsl::equal_to<TYPE> {
    // Equal functor with throwing 'swap'.

    // MANIPULATORS
    void swap(
           ThrowingSwapEqual& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        (void)other;
    }

    // FREE FUNCTIONS
    friend void swap(
               ThrowingSwapEqual& a,
               ThrowingSwapEqual& b) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        (void)a;
        (void)b;
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

                            // =================
                            // struct NeqFunctor
                            // =================

template <class TYPE, bool = bsl::is_copy_constructible<TYPE>::value>
class NeqFunctor;
    // A class for storing a value away, and later comparing it to a second
    // value.   The 'operator()' returns the equivalent of "not equal to",
    // hence the name 'neqFunctor'.   If the template parameter class 'TYPE'
    // is not copy-constructible, then the copy is not made, and the functor
    // will return 'true' for all values passed.

template <class TYPE>
class NeqFunctor<TYPE, false> {
     // The specialization for non-copyable types.  Do not save a value, always
     // return true.
  public:
    NeqFunctor(const TYPE &)
        // Do nothing.
    {
    }

    bool operator()(const TYPE &) const
        // Return 'true'.
    {
        return true;
    }
};

template <class TYPE>
class NeqFunctor<TYPE, true> {
     // The specialization for copyable types.  Save a copy of the value passed
     // to the constructor, and use it for comparison in the 'operator()'.
  public:
    NeqFunctor(const TYPE &value)
        // Save off the specified 'value' for use later.
    : d_value(value)
    {
    }

    bool operator()(const TYPE &other) const
        // Return 'true' when the specified 'other' compares 'not equal' to the
        // saved value, and 'false' otherwise.
    {
        return d_value != other;
    }

  private:
    TYPE d_value;
};

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

           k_IS_KEY_MOVE_AWARE =
                   bsl::is_same<KEY, bsltf::MovableTestType>::value ||
                   bsl::is_same<KEY, bsltf::MovableAllocTestType>::value ||
                   bsl::is_same<KEY, bsltf::MoveOnlyAllocTestType>::value ||
                   bsl::is_same<KEY,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,

           k_IS_VALUE_MOVE_AWARE =
                   bsl::is_same<VALUE, bsltf::MovableTestType>::value ||
                   bsl::is_same<VALUE, bsltf::MovableAllocTestType>::value ||
                   bsl::is_same<VALUE, bsltf::MoveOnlyAllocTestType>::value ||
                   bsl::is_same<VALUE,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,

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

  public:
    // TEST CASES

    static void testCase11();
        // Test generator functions 'g'.

//   static void testCase10();
//      // bslx streaming -- N/A

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9();
        // operator=

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
              bool OTHER_FLAGS>
    static void testCase8_propagate_on_container_swap_dispatch();
    static void testCase8_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase8_noexcept();
        // Test 'swap' noexcept.

    static void testCase8();
        // Swap

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase7_select_on_container_copy_construction_dispatch();
    static void testCase7_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void testCase7();
        // Copy c'tor

    static void testCase6();
        // operator==, !=

//   static void testCase5();
//      // operator<< -- N/A

    static void testCase4();
        // Basic accessor tests.

    static void testCase3();
    static void testCase3_range();
    static void testCase3_verifySpec();
        // Range c'tor, 'ggg' and 'gg'.

    static void testCase2();
    static void testCase2_WithCopy();    // only types with copy c'tor
        // Basic manipulator test.
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
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g
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
        Obj& r2 = gg(&x, SPEC);
        const Obj& r3 = g(SPEC);
        const Obj& r4 = g(SPEC);
        ASSERT(&r2 == &r1);
        ASSERT(&x  == &r1);
        ASSERT(&r4 != &r3);
        ASSERT(&x  != &r3);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>                   StdAlloc;

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
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

    if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION"
                        "\n===============================================\n");

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == false'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR
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
    //   Obj& operator=(const Obj&);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("COPY-ASSIGNMENT OPERATOR: %s\n"
                        "------------------------\n",
                        NameOf<KEY>().name());
    {
        // Create first object

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec_p;

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
                const char *const SPEC2   = DATA[tj].d_spec_p;

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                               testCase8_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<KEY,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS>              StdAlloc;

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_SWAP_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
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

        const Obj ZZ(IVALUES.begin(), IVALUES.end(),
                     1, HASH(), EQUAL(), scratch);        // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const Obj WW(JVALUES.begin(), JVALUES.end(),
                         1, HASH(), EQUAL(), scratch);    // control

            {
                IVALUES.resetIterators();

                Obj mX(IVALUES.begin(), IVALUES.end(),
                        1, HASH(), EQUAL(), xma);
                const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(X) P(ZZ) }

                JVALUES.resetIterators();

                Obj mY(JVALUES.begin(), JVALUES.end(),
                       1, HASH(), EQUAL(), yma);
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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
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

    if (verbose)
        printf("\nSWAP MEMBER AND FREE FUNCTIONS: ALLOCATOR PROPAGATION"
               "\n=====================================================\n");

    if (verbose) printf("\n'propagate_on_container_swap::value == false'\n");

    testCase8_propagate_on_container_swap_dispatch<false, false>();
    testCase8_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\n'propagate_on_container_swap::value == true'\n");

    testCase8_propagate_on_container_swap_dispatch<true, false>();
    testCase8_propagate_on_container_swap_dispatch<true, true>();
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase8_noexcept()
    // Verify that noexcept specification of the member 'swap' function is
    // correct.
{
    Obj a;
    Obj b;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
    const bool isNoexcept =
                        bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                        bsl::is_nothrow_swappable<HASH>::value &&
                        bsl::is_nothrow_swappable<EQUAL>::value;
    ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(a.swap(b)));
#endif
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase8()
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
    //   void swap(Obj&);
    //   void swap(Obj&, Obj&);
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
        const char *const SPEC1   = DATA[ti].d_spec_p;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(&oa);          const Obj& W  = gg(&mW,  SPEC1);
        Obj mXX(&scratch);    const Obj& XX = gg(&mXX, SPEC1);

        ASSERT(W == XX);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // Ensure the first row of the table contains the default-constructed
        // value.

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
            const char *const SPEC2   = DATA[tj].d_spec_p;

            Obj mX( &oa);         const Obj& X  = gg(&mX,  SPEC1);
            Obj mY( &oa);         const Obj& Y  = gg(&mY,  SPEC2);
            Obj mYY(&scratch);    const Obj& YY = gg(&mYY, SPEC2);

            ASSERT(X == XX);
            ASSERT(Y == YY);

            mX.max_load_factor(2.0f);
            mY.max_load_factor(3.0f);

            ASSERT(2.0f == X.max_load_factor());
            ASSERT(3.0f == Y.max_load_factor());

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

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

                swap(mX, mY);

                ASSERT(2.0f == X.max_load_factor());
                ASSERT(3.0f == Y.max_load_factor());

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
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
        Obj XX(&scratch);

        Obj mY(&oa);          const Obj& Y  = gg(&mY,  "ABC");
        Obj mYY(&scratch);    const Obj& YY = gg(&mYY, "ABC");

        ASSERT(XX == X);
        ASSERT(YY == Y);

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

        ASSERT(3.0 == X.max_load_factor());
        ASSERT(2.0 == Y.max_load_factor());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
                     testCase7_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    KEY,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS>                  StdAlloc;

    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, StdAlloc> Obj;

    const bool PROPAGATE = SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

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
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::
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

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates *default* allocator.\n");

    testCase7_select_on_container_copy_construction_dispatch<false, false>();
    testCase7_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    testCase7_select_on_container_copy_construction_dispatch<true, false>();
    testCase7_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\nVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef u::StatefulStlAllocator<KEY>                           Allocator;
    typedef bsl::unordered_map<KEY, VALUE, HASH, EQUAL, Allocator> Obj;

    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
        };
        const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            TestValues VALUES(SPEC);

            const int ALLOC_ID = ti + 73;

            Allocator a;  a.setId(ALLOC_ID);

            const Obj W(VALUES.begin(), VALUES.end(), 1, HASH(), EQUAL(), a);
                                                                     // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

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

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase7()
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
    //   unordered_map(const unordered_map& original);
    //   unordered_map(const unordered_map& original, const A& allocator);
    // ------------------------------------------------------------------------

    typedef bsl::allocator_traits<ALLOC> Traits;
    bslma::TestAllocator sa("scratch", veryVeryVerbose);
    bslma::TestAllocator oa("test",    veryVeryVerbose);
    bslma::TestAllocator da("default", veryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    // This test only works if 'select_on_container_copy_construction' on
    // 'ALLOC' always returns the default allocator.

    ASSERT(&da == Traits::select_on_container_copy_construction(&oa));
    const TestValues VALUES(TV_SPEC);

    if (verbose)
        printf("\nTesting parameters: k_TYPE_ALLOC = %d.\n", k_TYPE_ALLOC);
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

            // Create control object 'W'.

            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(&sa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                const Obj Y0(X);

                ASSERTV(SPEC, X == Y0);
                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y0.get_allocator().mechanism() == &da);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                ASSERTV(SPEC, X == Y1);
                ASSERTV(SPEC, W == Y1);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y1.get_allocator().mechanism() == &da);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                pair<Iter, bool> RESULT = primaryManipulator(&Y1, 'z');

                ASSERTV(true == RESULT.second);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y1);
                }

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
                ASSERTV(SPEC, Y1.get_allocator().mechanism() == &da);
            }
            {   // Testing concern 5 with test allocator.
                if (veryVerbose) {
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");
                }

                const bsls::Types::Int64 A = oa.numBlocksTotal();

                Obj Y11(X, &oa);

                ASSERTV(SPEC, X == Y11);
                ASSERTV(SPEC, W == Y11);
                ASSERTV(SPEC, W == X);

                ASSERT(0 == LENGTH || oa.numBlocksTotal() > A);

                // Due of pooling of memory allocation, we can't predict
                // whether this insert will allocate or not.

                pair<Iter, bool> RESULT = primaryManipulator(&Y11, 'z');
                ASSERTV(true == RESULT.second);

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y11.get_allocator().mechanism() == &oa);
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
                    ASSERTV(SPEC, Y2.get_allocator() != X.get_allocator());
                    ASSERTV(SPEC, Y2.get_allocator().mechanism() == &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase6()
{
    // ------------------------------------------------------------------------
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
    //:       6 Verify that modifying the 'second' fields of the pair affects
    //:         the result -- when X == Y, go through the elements of X,
    //:         perterbing the 'second' field of every element, and verify
    //:         that even this small change causing an inequality result.
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const Obj&, const Obj&);
    //   bool operator!=(const Obj&, const Obj&);
    // ------------------------------------------------------------------------

    if (veryVerbose) printf("EQUALITY-COMPARISON OPERATORS\n"
                            "=============================\n");

    if (veryVerbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*OP)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        OP op = bsl::operator==;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        (void) [](const Obj& lhs, const Obj& rhs) -> bool {
            return lhs != rhs;
        };
#else
        op    = bsl::operator!=;
#endif
        (void) op;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const TestValues VALUES(TV_SPEC);

    // Constructing default 'VALUE' to compare with.  Note, that we construct
    // default value this way to support some types that do not meet C++
    // requirement of 'default-insertable'.
    bslma::TestAllocator value("default value", veryVeryVeryVerbose);
    ALLOC                xvalue(&value);

    bsls::ObjectBuffer<VALUE> d;
    bsl::allocator_traits<ALLOC>::construct(xvalue, d.address());
    bslma::DestructorGuard<VALUE> defaultValueGuard(d.address());

    const VALUE& D = d.object();

    bool done = false;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec_p;
            const size_t      LENGTH1 = strlen(DATA[ti].d_results_p);

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
                const char *const SPEC2   = DATA[tj].d_spec_p;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results_p);

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

                    if (EXP) {
                        unsigned count = 0;
                        const Iter end = mX.end();
                        for (Iter it = mX.begin(); end != it; ++it, ++count) {
                            // Constructing temp default 'VALUE' object.
                            bsls::ObjectBuffer<VALUE> m;
                            bsl::allocator_traits<ALLOC>::construct(xvalue,
                                                                  m.address());
                            bslma::DestructorGuard<VALUE> mGuard(m.address());

                            u::copyAssignTo(m.address(), it->second);

                            // Note this will even work for 'MethodPtr', which
                            // default constructs to a garbage value.

                            if (D != m.object()) {
                                u::copyAssignTo(
                                         bsls::Util::addressOf(it->second), D);

                                ASSERTV(!(X == Y));
                                ASSERTV(!(Y == X));
                                ASSERTV(  X != Y);
                                ASSERTV(  Y != X);

                                u::copyAssignTo(
                                             bsls::Util::addressOf(it->second),
                                             m.object());

                                ASSERTV(  X == Y);
                                ASSERTV(  Y == X);
                                ASSERTV(!(X != Y));
                                ASSERTV(!(Y != X));
                            }
                        }

                        ASSERT(X.size() == count);
                    }

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

                    if (EXP) {
                        unsigned count = 0;
                        const Iter end = mX.end();
                        for (Iter it = mX.begin(); end != it; ++it, ++count) {
                            // Constructing temp default 'VALUE' object.
                            bsls::ObjectBuffer<VALUE> m;
                            bsl::allocator_traits<ALLOC>::construct(xvalue,
                                                                  m.address());
                            bslma::DestructorGuard<VALUE> mGuard(m.address());

                            u::copyAssignTo(m.address(), it->second);

                            // Note this will even work for 'MethodPtr', which
                            // default constructs to a garbage value.

                            if (D != m.object()) {
                                u::copyAssignTo(
                                         bsls::Util::addressOf(it->second), D);
                                ASSERTV(!(X == Y));
                                ASSERTV(!(Y == X));
                                ASSERTV(  X != Y);
                                ASSERTV(  Y != X);
                                u::copyAssignTo(
                                             bsls::Util::addressOf(it->second),
                                             m.object());
                                ASSERTV(  X == Y);
                                ASSERTV(  Y == X);
                                ASSERTV(!(X != Y));
                                ASSERTV(!(Y != X));
                            }
                        }

                        ASSERT(X.size() == count);
                    }

                    ASSERT(!done);
                    done |=
                         NUM_DATA-1 == ti && NUM_DATA-1 == tj && 'b' == CONFIG;
                }
            }
        }
    }

    ASSERT(done);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //
    // Concerns:
    //: 1 That 'find' on a modifiable container:
    //:   o Returns an iterator for modifiable access.
    //:   o Finds the right pair in the container if it is to be found.
    //:   o Returns 'end' if the key is not to be found.
    //: 2 That 'find' on a const container:
    //:   o Returns an iterator for const access.
    //:   o Finds the right pair in the container if it is to be found.
    //:   o Returns 'end' if the key is not to be found.
    //: 3 That an iterator traversing the range '[ cbegin() .. cend() )' will
    //:   visit every node in the container exactly once.
    //
    // Plan:
    //: 1 Initialize a 'TestValues' array object a full set of possible values
    //:   of pairs with which nodes can be initialized via 'gg'.
    //: 2 Iterate through a set of specs.
    //: 3 Initialize a container according to the spec.
    //: 4 Iterate through all the elements in the 'TestValues' array and select
    //:   'K', the key element of the pair.
    //: 5 Call 'find' on a both modifiable and const versions of the container
    //:   and observe that the right types of iterators are returned, and they
    //:   compare equal.
    //: 6 Observe that if 'K' is not expected to be in the container, 'end()'
    //:   is returned.
    //: 7 Use a const iterator and 'cbegin()' 'cend()' to iterate through the
    //:   container, observe that the right number of nodes is encountered,
    //:   that 'K' is, or is not, the key of one of them, as expected.
    //
    // Testing:
    //   Iter  find(const KEY&);
    //   CIter find(const KEY&) const;
    //   CIter cbegin();
    //   CIter cend();
    // ------------------------------------------------------------------------

    if (veryVerbose) P(NameOf<KEY>());

    BSLMF_ASSERT((! bsl::is_same<Iter, CIter>::value));

    {
        typedef bool (Obj::*MP)() const;
        MP mp = &Obj::empty;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)() const;
        MP mp = &Obj::size;
        (void) mp;
        mp    = &Obj::max_size;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)();
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MP)() const;
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
        mp    = &Obj::cbegin;
        (void) mp;
        mp    = &Obj::cend;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(const KEY&);
        MP mp = &Obj::find;
        (void) mp;
    }

    {
        typedef CIter (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::find;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::count;
        (void) mp;
    }

    {
        typedef pair<Iter, Iter> (Obj::*MP)(const KEY&);
        MP mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef pair<CIter, CIter> (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)() const;
        MP mp = &Obj::bucket_count;
        (void) mp;
        mp    = &Obj::max_bucket_count;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(size_t) const;
        MP mp = &Obj::bucket_size;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::bucket;
        (void) mp;
    }

    {
        typedef LIter (Obj::*MP)(size_t);
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    {
        typedef CLIter (Obj::*MP)(size_t) const;
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
        mp    = &Obj::cbegin;
        (void) mp;
        mp    = &Obj::cend;
        (void) mp;
    }

    {
        typedef const VALUE& (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::at;
        (void) mp;
    }

    const int             NUM_DATA         = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    TestValues values(TV_SPEC);    const TestValues& VALUES = values;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char   *SPEC   = DATA[ti].d_results_p;
        const size_t  LENGTH = strlen(SPEC);

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&sa); const Obj& X = mX;

        gg(&mX, SPEC);

        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor sam(&sa);

        ASSERTV(u::verifySpec(X, SPEC));

        ASSERTV((0 == LENGTH) == X.empty());
        ASSERTV(LENGTH == X.size());
        ASSERTV((size_t) -1 >= X.max_size());   // TBD: have more
                                                // sophisticated value for
                                                // max_size.

        const Iter begin = mX.begin();
        CIter cBegin = X.begin();
        ASSERTV(begin == cBegin);
        cBegin = X.cbegin();
        ASSERTV(begin == cBegin);
        const Iter end = mX.end();
        CIter cend = X.end();
        ASSERTV(end == cend);
        cend = X.end();
        ASSERTV(end == cend);

        ASSERTV(!LENGTH == (begin == end));

        {
            size_t count = 0;
            for (Iter it = begin; end != it; ++it) {
                ++count;
            }
            ASSERTV(LENGTH == count);
        }

        ASSERTV(dam.isTotalSame());

        for (char c = 'A'; c <= 'Z'; ++c) {
            const char C = c;

            const bool EXP = strchr(SPEC, C);

            const TValueType& P = VALUES[C - 'A'];
            const KEY&        K = P.first;

            Iter it = mX.find(K);
            ASSERTV(EXP == (mX.end() != it));
            if (EXP) {
                ASSERTV(u::eq(*it, P));
            }

            CIter cit = X.find(K);
            ASSERTV(EXP == (mX.end() != cit));
            if (EXP) {
                ASSERTV(u::eq(*cit, P));
            }

            ASSERTV(cit == it);
            ASSERTV(EXP == (1 == X.count(K)));

            const pair<Iter, Iter>& EQR = mX.equal_range(K);
            ASSERTV(EQR.first == it);
            if (EXP) ++it;
            ASSERTV(EQR.second == it);

            const pair<CIter, CIter>& CEQR = X.equal_range(K);
            ASSERTV(CEQR.first == cit);
            if (EXP) ++cit;
            ASSERTV(CEQR.second == cit);

            if (EXP) {
                BSLS_TRY {
                    {
                        VALUE& v = mX.at(K);
                        ASSERTV(LENGTH == X.size());
                        ASSERT(P.second == v);
                    }

                    {
                        const VALUE& v = X.at(K);
                        ASSERTV(LENGTH == X.size());
                        ASSERT(P.second == v);
                    }
                }
                BSLS_CATCH(...) {
                    ASSERTV(0 && "at threw unexpectedly");
                }
            }

            size_t measuredLength = 0;
            CIter end = X.cend();
            bool found = false;
            for (cit = X.cbegin(); end != cit; ++cit, ++measuredLength) {
                if (K == cit->first) {
                    ASSERT(!found);
                    found = true;
                }
            }
            ASSERT(EXP == found);
            ASSERTV(SPEC, LENGTH, measuredLength, X.size(),
                                                     LENGTH == measuredLength);
        }

        ASSERTV((size_t) -1 >= X.max_bucket_count());   // TBD: make more
                                                        // sophisticated.
        const size_t BC = X.bucket_count();

        ASSERTV(BC >= 1);
        ASSERTV(X.empty() == (1 == BC));

        size_t sum = 0;
        for (size_t tj = 0; tj < BC; ++tj) {
            BSLMF_ASSERT((! bsl::is_same<LIter, CLIter>::value));

            LIter bIt   = mX.begin(tj);
            CLIter cbBegin =  X.begin(tj);
            ASSERTV(bIt  == cbBegin);
            cbBegin = X.cbegin(tj);
            ASSERTV(bIt  == cbBegin);
            LIter bEnd     = mX.end(tj);
            CLIter cbEnd   =  X.end(tj);
            ASSERTV(bEnd == cbEnd);
            cbEnd = X.cend(tj);
            ASSERTV(bEnd == cbEnd);

            {
                size_t count = 0;
                for (CLIter lit = cbBegin; cbEnd != lit; ++lit, ++bIt) {
                    ASSERTV(X.bucket(lit->first) == tj);
                    ASSERT(&*lit == &*bIt);

                    ++count;
                }
                ASSERTV(X.bucket_size(tj) == count);
                sum += count;
            }
        }

        ASSERT(X.size() == sum);
        ASSERTV(sam.isTotalSame());
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase3_verifySpec()
    // ------------------------------------------------------------------------
    // TESTING 'verifySpec'
    //
    // Concerns:
    //: 1 That 'verifySpec' correctly indicates whether a spec reflects the
    //:   state of a container.
    //
    // Plan:
    //: 1 Iterate through a sequence of 'SPEC's, with a 'LENGTH' and 'RESULT'
    //:   for each spec, where 'RESULT' has only one copy of every value, with
    //:   letters in sorted order.
    //: 2 Create a container X initialized with 'SPEC' by 'gg'.
    //: 3 Confirm that 'verifySpec' return 'true' pass the container and
    //:   'SPEC'.
    //: 4 Within that loop, iterate again through the specs, initializing
    //:   'JSPEC', 'JLENGTH', and 'JRESULT'.
    //: 5 Calculate 'EXP', which is 'true' iff the 'SPEC' & 'JSPEC' are
    //:   isomorphic.
    //: 6 Confirm 'verifySpec' returns EXP for 'JSPEC' and 'JRESULT'.
    //: 7 Permute 'JSPEC' through numerous permutations (limited at 100 to
    //:   prevent combinatoric explosion taking prohibitive time) and verify
    //:   this has no effect on the results of 'verifySpec'.
    //: 8 Perturb the 'mapped_type' fields of the container using 'Iter'
    //:   access, and observe that the change is reflected in the value of
    //:   'verifySpec'.
    //: 9 Perturb the 'mapped_type' values back to their original value using
    //:   'LIter' access, and observe that the change is reflected in the value
    //:   of 'verifySpec'.
    //: 10 If the container is not empty, delete the first element (after
    //:    storing its value) and observe that 'verifySpec' detects the change.
    //: 11 Add another element (not equal to the element just deleted) and
    //:    observe that, even though the size matches the original size,
    //:    'verifySpec' detects the difference.
    //: 12 Restore the container to its initial state and observe that
    //:    'verifySpec' returns 'true'.
    //: 13 clear the container and observe the result of 'verifySpec'.
    //
    // Note that 'VALUE' must be copy-assignable for this test.
    //
    // Testing:
    //   bool verifySpec(const Obj&, const char *, bool = false);
    // ------------------------------------------------------------------------
{
    TestValues values(TV_SPEC);    const TestValues& VALUES = values;
    const TValueType& Z = VALUES[VALUES.size() - 1];

    {
        const char vz = static_cast<char>(u::valueOf(Z.first));
        const char vs = static_cast<char>(u::valueOf(Z.second));
        const char vzs = static_cast<char>('z' + u::k_CHAR_SHIFT);

        ASSERTV(vz, 'z' == vz);
        ASSERTV(vzs, vs, vzs == vs);
    }

    bool doneA = false, doneB = false, doneC = false, doneD = false,
                                                  doneE = false, doneF = false;

    for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
        const DefaultDataRow *pd = DEFAULT_DATA + ti;
        const char   *SPEC   = pd->d_spec_p;
        const char   *RESULT = pd->d_results_p;
        const size_t  LENGTH = strlen(RESULT);

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&sa);    const Obj& X = gg(&mX, SPEC);

        bslma::TestAllocatorMonitor dam(&da);
        bslma::TestAllocatorMonitor sam(&sa);

        ASSERTV((0 == LENGTH) == X.empty());
        ASSERTV(LENGTH == X.size());

        ASSERTV(u::verifySpec(X, SPEC));
        ASSERTV(u::verifySpec(X, SPEC, true));
        ASSERTV(u::verifySpec(X, SPEC, false));

        ASSERTV(u::verifySpec(X, RESULT));
        ASSERTV(u::verifySpec(X, RESULT, true));
        ASSERTV(u::verifySpec(X, RESULT, false));

        const char *JSPEC, *PREV_JSPEC = "woof";
        size_t      JLENGTH, PREV_JLENGTH = 4;
        for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj,
                                  PREV_JSPEC = JSPEC, PREV_JLENGTH = JLENGTH) {
            const DefaultDataRow *pdj = DEFAULT_DATA + ti;
                          JSPEC   = pdj->d_spec_p;
            const char   *JRESULT = pdj->d_results_p;
                          JLENGTH = strlen(JRESULT);

            // Note that the specs in 'd_results_p' are always in canonical
            // form.

            const bool EXP = !strcmp(RESULT, JRESULT);

            ASSERTV(SPEC, JSPEC, EXP == u::verifySpec(X, JSPEC));
            ASSERTV(SPEC, JSPEC, EXP == u::verifySpec(X, JSPEC, false));
            ASSERTV(SPEC, JSPEC, EXP == u::verifySpec(X, JSPEC, true));
            ASSERTV(SPEC, JSPEC, EXP == u::verifySpec(X, JRESULT));
            ASSERTV(SPEC, JSPEC, EXP == u::verifySpec(X, JRESULT, false));
            ASSERTV(SPEC, JSPEC, EXP == u::verifySpec(X, JRESULT, true));

            doneA |= LENGTH == JLENGTH && DEFAULT_MAX_LENGTH == LENGTH;

            char PSPEC[DEFAULT_MAX_LENGTH + 1];    // permutable spec
            strcpy(PSPEC, JSPEC);
            std::sort(PSPEC + 0, PSPEC + JLENGTH);

            if (JLENGTH == PREV_JLENGTH) {
                // If 'JSPEC' is just a permutation of the previous spec, skip
                // the rest of this loop.

                char SORTED_PREV_SPEC[DEFAULT_MAX_LENGTH + 1];
                strcpy(SORTED_PREV_SPEC, PREV_JSPEC);
                std::sort(SORTED_PREV_SPEC + 0, SORTED_PREV_SPEC + JLENGTH);

                if (!strcmp(PSPEC, SORTED_PREV_SPEC)) {
                    continue;
                }
            }

            int count = 0;
            do {
                ASSERTV(RESULT, PSPEC, EXP == u::verifySpec(X, PSPEC));
                ASSERTV(RESULT, PSPEC, EXP == u::verifySpec(X, PSPEC, false));
                ASSERTV(RESULT, PSPEC, EXP == u::verifySpec(X, PSPEC, true));

                ++count;
            } while (count < 100 &&
                            std::next_permutation(PSPEC + 0, PSPEC + JLENGTH));

            doneB |= 100 == count;
        }

        // Perturb mapped values (using 'Iter') and observe that 'verifySpec'
        // detects this.

        const Iter end = mX.end();
        for (Iter it = mX.begin(); end != it; ++it) {
            // Constructing temp default 'VALUE' object.
            bslma::TestAllocator value("default value", veryVeryVeryVerbose);
            ALLOC                xvalue(&value);

            bsls::ObjectBuffer<VALUE> v;
            bsl::allocator_traits<ALLOC>::construct(xvalue, v.address());
            bslma::DestructorGuard<VALUE> mGuard(v.address());

            Pair  p = *it;

            u::copyAssignTo(v.address(), it->second);
            u::copyAssignTo(bsls::Util::addressOf(it->second), Z.second);

            const char vf = static_cast<char>(u::valueOf(it->first));
            const char vs = static_cast<char>(u::valueOf(it->second));
            ASSERTV(vf, vs, vf != vs);
            ASSERTV(it->second != v.object());
            ASSERTV(*it != p);

            ASSERTV(!u::verifySpec(X, SPEC));
            ASSERTV( u::verifySpec(X, SPEC, true));       // Keys only
            ASSERTV(!u::verifySpec(X, SPEC, false));      // mapped values too

            doneC |= DEFAULT_MAX_LENGTH == LENGTH;
        }

        // Now perturb it back (using 'LIter') and observe that this is
        // reflected in 'verifySpec'.

        const size_t BC = X.bucket_count();

        size_t count = 0;
        for (size_t tj = 0; tj < BC; ++tj) {
            LIter bEnd = mX.end(tj);
            for (LIter it = mX.begin(tj); bEnd != it; ++it, ++count) {
                ASSERT(Z.second == it->second);
                int id = u::idOf(*it);
                int idB = id + u::k_CHAR_SHIFT;
                it->second = TTF::create<VALUE>(idB);
                ASSERTV(VALUES[id - 'A'].second == it->second);

                ASSERTV( u::verifySpec(X, SPEC, true));           // Keys only
                ASSERTV((LENGTH == count + 1) == u::verifySpec(X, SPEC));

                doneD |= DEFAULT_MAX_LENGTH == LENGTH;
            }
        }
        ASSERTV(LENGTH == count);

        // Container should now be restored to match SPEC.

        ASSERTV(u::verifySpec(X, SPEC, true));    // Keys only
        ASSERTV(u::verifySpec(X, SPEC, false));   // Looking at mapped values
                                                  // too

        const bool empty = 0 == LENGTH;
        char deletedVal;
        if (!empty) {
            // Erase the first node and observe that spec no longer matches.

            deletedVal = static_cast<char>(u::valueOf(X.begin()->first));
            mX.erase(mX.begin());

            ASSERTV(!u::verifySpec(X, SPEC, true));    // Keys only
            ASSERTV(!u::verifySpec(X, SPEC, false));   // Looking at mapped
                                                       // values too

            doneE |= DEFAULT_MAX_LENGTH == LENGTH;
        }

        // Add a new node with perturb with 'primaryManipulator' and observe
        // that 'verifySpec' detects change.

        pair<Iter, bool> result = primaryManipulator(&mX, u::valueOf(Z.first));
        ASSERTV(result.second);
        ASSERTV(u::eq(*result.first, Z));

        ASSERTV(empty || LENGTH == X.size());

        // Observe that even when LENGTH is correct, verifySpec detects that
        // the spec doesn't match.

        ASSERTV(!u::verifySpec(X, SPEC, true));    // Keys only
        ASSERTV(!u::verifySpec(X, SPEC, false));   // Looking at mapped values
                                                   // toO

        // Restore to spec and observe verifySpec passes.

        // Erase the new node.

        mX.erase(Z.first);

        if (!empty) {
            // Restore the deleted node.

            result = primaryManipulator(&mX, deletedVal);
            ASSERTV(result.second);

            doneF |= DEFAULT_MAX_LENGTH == LENGTH;
        }

        ASSERTV(u::verifySpec(X, SPEC, true));    // Keys only
        ASSERTV(u::verifySpec(X, SPEC, false));   // Looking at mapped values
                                                  // too

        mX.clear();

        ASSERTV(empty == u::verifySpec(X, SPEC, true));    // Keys only
        ASSERTV(empty == u::verifySpec(X, SPEC, false));   // Looking at mapped
                                                           // values too
    }

    ASSERTV(doneA && doneB && doneC && doneD && doneE && doneF);
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase3_range()
{
    // ------------------------------------------------------------------------
    // Range c'tor, 'ggg', 'gg', and 'verifySpec' functions.
    // ------------------------------------------------------------------------

    for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
        const DefaultDataRow *pd     = DEFAULT_DATA + ti;
        const int             INDEX  = pd->d_index;
        const char           *SPEC   = pd->d_spec_p;
        const char           *RESULT = pd->d_results_p;
        const size_t          LENGTH = strlen(RESULT);

        // Verify indexes work.

        for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
            const DefaultDataRow *pdj = DEFAULT_DATA + ti;
            ASSERTV((INDEX == pdj->d_index) ==
                                            !strcmp(RESULT, pdj->d_results_p));
        }

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        TestValues src(SPEC, &sc);

        {
            Obj mX(src.begin(), src.end());    const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksTotal());
            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(u::verifySpec(X, SPEC));
            ASSERTV(u::verifySpec(X, RESULT));

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj = DEFAULT_DATA + ti;
                const bool MATCH = INDEX == pdj->d_index;

                ASSERTV(MATCH == u::verifySpec(X, pdj->d_spec_p));
                ASSERTV(MATCH == u::verifySpec(X, pdj->d_results_p));
            }
        }

        ASSERTV(0 == da.numBlocksInUse());

        {
            Obj mX(src.begin(),
                   src.end(),
                   0);
            const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(u::verifySpec(X, RESULT));
        }

        {
            Obj mX(src.begin(),
                   src.end(),
                   0,
                   HASH());
            const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(u::verifySpec(X, RESULT));
        }

        {
            Obj mX(src.begin(),
                   src.end(),
                   0,
                   HASH(),
                   EQUAL());
            const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(!!LENGTH == !!da.numBlocksInUse());

            ASSERTV(u::verifySpec(X, RESULT));
        }

        {
            Obj mX(src.begin(),
                   src.end(),
                   0,
                   HASH(),
                   EQUAL(),
                   &sa);
            const Obj& X = mX;
            src.resetIterators();

            ASSERTV(LENGTH == X.size());

            ASSERTV(0        ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH == !!sa.numBlocksTotal());
            ASSERTV(!!LENGTH == !!sa.numBlocksInUse());

            ASSERTV(u::verifySpec(X, RESULT));
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase3()
{
    // ------------------------------------------------------------------------
    // 'ggg' AND 'gg' functions.
    //
    // Concerns:
    //: 1 That the 'DEFAULT_DATA' table is correct.
    //: 1 That 'ggg' and 'gg' will correctly populate an empty container with
    //:   the specified state.
    //: 2 That 'ggg' correctly detects invalid specs.
    //
    // Plan:
    //: 1 (in main program) Iterate through the elements of DEFAULT_DATA.
    //:   o Calculate the minimum spec with its letter sorted.
    //:   o Verify that the string is the same aa the 'd_result' field.
    //:   o Verify that the length of the spec <= DEFAULT_MAX_LENGTH.
    //:   o Verify that all the letters in the spec are in the range "[ 'A',
    //:     'Z' ]".
    //:   o Traverse all the other specs and verify that matching indexes
    //:     indicate matching 'd_result' fields.
    //: 2 Iterate though a series of valid specs.
    //:   o Default construct a container, and use 'ggg' to initialize the
    //:     container to a spec.
    //:   o Observe that 'ggg' returns '-1'.
    //:   o Examine the state of the container and verify that it corresponds
    //:     to the spec.
    //:   o Clear the container, and repopulate it using 'gg'.
    //:   o Verify that 'gg' returned a const reference to the container it was
    //:     passed.
    //:   o Examine the state of the container and verify that it corresponds
    //:     to the spec.
    //: 3 Iterate through a sequence of bad specs.
    //:   o Default construct a container and call 'ggg' on it with the spec,
    //:     and observe 'ggg' does not return -1.
    // ------------------------------------------------------------------------

    for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
        const DefaultDataRow *pd     = DEFAULT_DATA + ti;
        const int             INDEX  = pd->d_index;
        const char           *SPEC   = pd->d_spec_p;
        const char           *RESULT = pd->d_results_p;
        const size_t          LENGTH = strlen(RESULT);

        // Verify indexes work.

        for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
            const DefaultDataRow *pdj = DEFAULT_DATA + ti;
            ASSERTV((INDEX == pdj->d_index) ==
                                            !strcmp(RESULT, pdj->d_results_p));
        }

        bslma::TestAllocator da("default",  veryVeryVeryVerbose);
        bslma::TestAllocator sc("scratch",  veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        ASSERTV(0 == sa.numBlocksInUse());

        {
            Obj mX(&sa);    const Obj& X = mX;

            ASSERTV(-1 == ggg(&mX, SPEC));

            // Make sure, by hand, that the value of the container corresponds
            // EXACTLY to 'RESULT', which is isomorphic to SPEC.

            char buf[100], *pChar;
            strcpy(buf, RESULT);
            for (CIter it = X.cbegin(); X.cend() != it; ++it) {
                const char val = static_cast<char>(u::idOf(*it));
                pChar = strchr(buf, val);
                ASSERT(pChar);
                ASSERT(val + u::k_CHAR_SHIFT == u::valueOf(it->second));
                *pChar = '_';
            }
            pChar = buf;
            while ('_' == *pChar) {
                ++pChar;
            }
            ASSERT(!*pChar);

            ASSERTV(0        ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH == !!sa.numBlocksInUse());

            ASSERTV(LENGTH == X.size());

            ASSERTV(u::verifySpec(X, RESULT));
            ASSERTV(u::verifySpec(X, SPEC));

            mX.clear();

            ASSERTV(0 == X.size());

            const Obj& XX = gg(&mX, SPEC);

            ASSERTV(&XX == &X);

            strcpy(buf, RESULT);
            for (CIter it = X.cbegin(); X.cend() != it; ++it) {
                const char val = static_cast<char>(u::idOf(*it));
                pChar = strchr(buf, val);
                ASSERT(pChar);
                ASSERT(val + u::k_CHAR_SHIFT == u::valueOf(it->second));
                *pChar = '_';
            }
            pChar = buf;
            while ('_' == *pChar) {
                ++pChar;
            }
            ASSERT(!*pChar);

            ASSERTV(0        ==   da.numBlocksInUse());
            ASSERTV(!!LENGTH == !!sa.numBlocksInUse());

            ASSERTV(LENGTH == X.size());

            ASSERTV(u::verifySpec(X, SPEC));
            ASSERTV(u::verifySpec(X, RESULT));
        }
    }

    struct {
        int d_line;
        const char *d_spec_p;
        const char *d_goodSpec_p;
    } BAD_SPECS[] = {
        { L_, " ",    "" },
        { L_, "ABCa", "ABC" },
        { L_, "ABC+", "ABC" },
        { L_, "A+BC", "ABC" },
        { L_, "A,+C", "AC" },
        { L_, "A1BC", "ABC" },
        { L_, "1234", "" } };
    enum { NUM_BAD_SPECS = sizeof BAD_SPECS / sizeof *BAD_SPECS };

    bslma::TestAllocator da("default",  veryVeryVeryVerbose);
    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    for (size_t ti = 0; ti < NUM_BAD_SPECS; ++ti) {
        const char *SPEC      = BAD_SPECS[ti].d_spec_p;
        const char *GOOD_SPEC = BAD_SPECS[ti].d_goodSpec_p;

        {
            Obj mX(&sa);    const Obj& X = mX;

            ASSERTV(-1 != ggg(&mX, SPEC, 0));
            ASSERTV(!u::verifySpec(X, SPEC));
        }

        {
            Obj mX(&sa);    const Obj& X = mX;
            gg(&mX, GOOD_SPEC);

            ASSERTV( u::verifySpec(X, GOOD_SPEC));
            ASSERTV(!u::verifySpec(X, SPEC));
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase2_WithCopy()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP) - WITH COPY C'TOR
    // ------------------------------------------------------------------------

    {
        typedef void (Obj::*MP)(const Pair *, const Pair *);
        MP mp = &Obj::insert;
        (void) mp;
    }

    {
        typedef Obj& (Obj::*MP)(bslmf::MovableRef<Obj>);
        MP mp = &Obj::operator=;
        (void) mp;
    }

    {
        typedef Obj& (Obj::*MP)(const Obj&);
        MP mp = &Obj::operator=;
        (void) mp;
    }

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    {
        typedef Obj& (Obj::*MP)(
                              std::initializer_list<typename Obj::value_type>);
        MP mp = &Obj::operator=;
        (void) mp;
    }
#endif

    {
        typedef VALUE& (Obj::*MP)(const KEY&);
        MP mp = &Obj::operator[];
        (void) mp;
    }

    {
        typedef pair<Iter, bool> (Obj::*MP)(const Pair&);
        MP mp = &Obj::insert;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(CIter, const Pair&);
        MP mp = &Obj::insert;
        (void) mp;
    }

    {
        typedef typename TestValues::iterator TVIter;
        typedef void (Obj::*MP)(TVIter, TVIter);
        MP mp = &Obj::insert;
        (void) mp;
    }

}

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOC>
void TestDriver<KEY, VALUE, HASH, EQUAL, ALLOC>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - primaryManipulator
    //      - insert
    //      - clear
    //      - erase
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
    //   Obj();
    //   Obj(const ALLOC&);
    //   Obj(size_t);
    //   Obj(size_t, const ALLOC&);
    //   Obj(size_t, HASH);
    //   Obj(size_t, HASH, const ALLOC&);
    //   Obj(size_t, HASH, EQUAL);
    //   Obj(size_t, HASH, EQUAL, const ALLOC&);
    //   ~Obj();
    //   pair<Iter, bool> primaryManipulator(Obj *, int);
    //   void clear();
    //   HASH hash_function() const;
    //   EQUAL key_equal() const;
    // ------------------------------------------------------------------------

    // 'pair<Iter, bool> emplace(Arg&& ... args);'    // N/A C++11
    // 'Iter emplace_hint(CIter, Arg&& ... args);'    // N/A C++11
    // 'template <class P> pair<Iter, bool> insert(P&&);'    // N/A C++11
    // 'pair<Iter, bool> emplace(Arg&& ... args);'    // N/A C++11
    // 'Iter emplace_hint(CIter, Arg&& ... args);'    // N/A C++11
    // 'template <class P> pair<Iter, bool> insert(P&&);'    // N/A C++11
    // 'template <class P> Iter insert(CIter, P&&);'    // N/A C++11

    {
        typedef void (Obj::*MP)();
        MP mp = &Obj::clear;
        (void) mp;
    }

    // MANIPULATORS

    {
        typedef VALUE& (Obj::*MP)(const KEY&);
        MP mp = &Obj::at;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)();
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    {
        typedef LIter (Obj::*MP)(size_t);
        MP mp = &Obj::begin;
        (void) mp;
        mp    = &Obj::end;
        (void) mp;
    }

    // 'emplace' & 'emplace_hint' tested in later test cases.

    {
        typedef Iter (Obj::*MP)(CIter);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef size_t (Obj::*MP)(const KEY&);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(CIter, CIter);
        MP mp = &Obj::erase;
        (void) mp;
    }

    {
        typedef Iter (Obj::*MP)(const KEY&);
        MP mp = &Obj::find;
        (void) mp;
    }

    {
        typedef pair<Iter, Iter> (Obj::*MP)(const KEY&);
        MP mp = &Obj::equal_range;
        (void) mp;
    }

    {
        typedef void (Obj::*MP)(float);
        MP mp = &Obj::max_load_factor;
        (void) mp;
    }

    {
        typedef void (Obj::*MP)(size_t);
        MP mp = &Obj::rehash;
        (void) mp;
    }

    {
        typedef void (Obj::*MP)(size_t);
        MP mp = &Obj::reserve;
        (void) mp;
    }

    {
        typedef void (Obj::*MP)(Obj&);
        MP mp = &Obj::swap;
        (void) mp;
    }

    {
        typedef EQUAL (Obj::*MP)() const;
        MP mp = &Obj::key_eq;
        (void) mp;
    }

    {
        typedef VALUE& (Obj::*MP)(bslmf::MovableRef<KEY>);
        MP mp = &Obj::operator[];
        (void) mp;
    }

    {
        typedef const VALUE& (Obj::*MP)(const KEY&) const;
        MP mp = &Obj::at;
        (void) mp;
    }

    const bool VALUE_TYPE_USES_ALLOC = bslma::UsesBslmaAllocator<Pair>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOC); }

    TestValues values(TV_SPEC);    const TestValues& VALUES = values;
                                      // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;
    ASSERT(MAX_LENGTH <= VALUES.size());

    HASH  tstHash(7);
    EQUAL tstEqual(9);

    ASSERTV(!(HASH()  == tstHash));
    ASSERTV(!(EQUAL() == tstEqual));

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        int done = 0;
        for (char cfg = 'a'; cfg <= 't'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            bslma::TestAllocator&  oa = strchr("behknqt", CONFIG) ? sa : da;
            bslma::TestAllocator& noa = &da == &oa                ? sa : da;

            Obj                  *objPtr;

            const bool EXP_ONE_BUCKET = CONFIG < 'l';

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;

                bslma::Allocator * const pNull = 0;
                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(&sa);
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj((size_t) 0);
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(0, pNull);
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(0, &sa);
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(0, tstHash);
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(0, tstHash, pNull);
                  } break;
                  case 'h': {
                      objPtr = new (fa) Obj(0, tstHash, &sa);
                  } break;
                  case 'i': {
                      objPtr = new (fa) Obj(0, tstHash, tstEqual);
                  } break;
                  case 'j': {
                      objPtr = new (fa) Obj(0, tstHash, tstEqual, pNull);
                  } break;
                  case 'k': {
                      objPtr = new (fa) Obj(0, tstHash, tstEqual, &sa);
                  } break;
                  case 'l': {
                      objPtr = new (fa) Obj(100);
                  } break;
                  case 'm': {
                      objPtr = new (fa) Obj(100, pNull);
                  } break;
                  case 'n': {
                      objPtr = new (fa) Obj(100, &sa);
                  } break;
                  case 'o': {
                      objPtr = new (fa) Obj(100, tstHash);
                  } break;
                  case 'p': {
                      objPtr = new (fa) Obj(100, tstHash, pNull);
                  } break;
                  case 'q': {
                      objPtr = new (fa) Obj(100, tstHash, &sa);
                  } break;
                  case 'r': {
                      objPtr = new (fa) Obj(100, tstHash, tstEqual);
                  } break;
                  case 's': {
                      objPtr = new (fa) Obj(100, tstHash, tstEqual, pNull);
                  } break;
                  case 't': {
                      objPtr = new (fa) Obj(100, tstHash, tstEqual, &sa);
                      ++done;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || EXP_ONE_BUCKET) == (1 == numPasses));

            Obj&                   mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.
            // NOTE THAT THIS QoI TEST IS STILL AN OPEN DESIGN ISSUE

            ASSERTV(CONFIG, oa.numBlocksTotal(),
                                       EXP_ONE_BUCKET == !oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
            ASSERTV(CONFIG, 0 == X.size());
            ASSERTV(CONFIG, X.cbegin() == X.cend());

            // If default constructed, only the static bucket is present.  Test
            // that exactly 1 bucket is present.

            ASSERTV(CONFIG, X.bucket_count(),
                                     EXP_ONE_BUCKET ? 1 == X.bucket_count()
                                                    : X.bucket_count() >= 100);

            ASSERTV(CONFIG, (('f' <= cfg && cfg <= 'k') || 'o' <= cfg ?
                                                              tstHash : HASH())
                                                         == X.hash_function());
            ASSERTV(CONFIG, (('i' <= cfg && cfg <= 'k') || 'r' <= cfg ?
                                                            tstEqual : EQUAL())
                                                         == X.key_eq());

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
                    pair<Iter, bool> result;
                    result = primaryManipulator(&mX, u::idOf(VALUES[tj]));
                    ASSERTV(LENGTH, tj, CONFIG, true == result.second);
                    ASSERTV(LENGTH, tj, CONFIG,
                                           u::eq(VALUES[tj], *(result.first)));
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

                Obj mG(&scratch); const Obj& G = mG;
                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    (void) primaryManipulator(&mG, u::idOf(VALUES[tj]));
                }
                ASSERT(X == G);

                const int MIN_PASSES = 1 + (CONFIG < 'h')
                                         + VALUE_TYPE_USES_ALLOC;
                numPasses = 0;
                EXCEPTION_TEST_BEGIN(mX) {
                    ++numPasses;

                    ASSERT(X == G);

                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[LENGTH - 1]);
                    }
                    pair<Iter, bool> RESULT =
                          primaryManipulator(&mX, u::idOf(VALUES[LENGTH - 1]));

                    ASSERTV(LENGTH, CONFIG, true == RESULT.second);
                    ASSERTV(LENGTH, CONFIG,
                                   u::eq(VALUES[LENGTH - 1], *(RESULT.first)));
                } EXCEPTION_TEST_END
                ASSERTV(!PLAT_EXC || !(1 == LENGTH) || numPasses > MIN_PASSES);

                matchFirstValues(L_, X, VALUES, LENGTH);

                // Verify behavior when element already exist in the object

                if (veryVerbose) {
                    printf("\t\t Verifying already inserted values\n");
                }
                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    if (veryVeryVeryVerbose) {
                       printf("\t\t\t\t Inserting: ");
                       P(VALUES[tj]);
                    }
                    pair<Iter, bool> RESULT =
                                  primaryManipulator(&mX, u::idOf(VALUES[tj]));
                    ASSERTV(LENGTH, tj, CONFIG, RESULT.second, false ==
                                                                RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, *(RESULT.first),
                                           u::eq(VALUES[tj], *(RESULT.first)));
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
//                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                if (veryVeryVeryVerbose) printf("mX.clear();\n");
                numPasses = 0;
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
//                        B - (int)LENGTH * k_TYPE_ALLOC == A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    pair<Iter, bool> RESULT =
                                    primaryManipulator(&mX,
                                                       u::idOf(VALUES[tj]));
                    ASSERTV(LENGTH, tj, CONFIG, true == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG,
                                           u::eq(VALUES[tj], *(RESULT.first)));
                }

                matchFirstValues(L_, X, VALUES, LENGTH);
            }

            // ----------------------------------------------------------------
            // We use 'NeqFunctor' to save a copy of the key for comparison
            // later.  If the key type is not copyable, then 'NeqFunctor' will
            // not copy it, and simply report that all existing keys are not
            // equal.

            if (veryVerbose) printf("\n\tTesting 'erase(it)'.\n");
            {
                size_t sz = X.size();
                for (Iter it = mX.begin(), nextIt; mX.end() != it;
                                                                 it = nextIt) {
                    NeqFunctor<KEY> neq(it->first);

                    numPasses = 0;
                    EXCEPTION_TEST_BEGIN(mX) {
                        ++numPasses;

                        nextIt = mX.erase(it);
                    } EXCEPTION_TEST_END
                    ASSERTV(1 == numPasses);

                    ASSERTV(X.size(), sz, X.size() == --sz);

                    // Ensure that the key is no longer in the map, and the
                    // iterator returned by 'erase' points to an element in the
                    // map.
                    bool found = mX.end() == nextIt;
                    for (Iter itB = mX.begin(); mX.end() != itB; ++itB) {
                        ASSERTV(it != itB);
                        ASSERTV(neq(itB->first));
                        if (nextIt == itB) {
                            found = true;
                        }
                    }
                    ASSERTV(found);
                }
                ASSERT(0 == sz);
                ASSERT(0 == X.size());
            }

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
        ASSERTV(1 == done);
    }
}

//=============================================================================
//                              BREATHING TEST
//-----------------------------------------------------------------------------

namespace BREATHING_TEST {

//-----------------------------------------------------------------------------
//    FREE FUNCTIONS FOR REVISED TESTING SCHEME
//    IMPLEMENTATION SHOULD BE (MOSTLY) CONTAINER INDEPENDENT

template<class CONTAINER>
const typename CONTAINER::key_type
keyForValue(const typename CONTAINER::value_type v)
{
//    return v;        // for 'set' containers
    return v.first;  // for 'unordered_map' containers
}

template <class CONTAINER>
void testConstEmptyContainer(const CONTAINER& x)
{
    typedef CONTAINER TestType;
    typedef typename TestType::size_type SizeType;

    ASSERT(x.empty());
    ASSERTV(x.size(), 0 == x.size());
    ASSERTV(x.load_factor(), 0.f == x.load_factor());

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

    const pair<typename TestType::const_iterator,
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

    const pair<typename TestType::iterator, typename TestType::iterator>
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

        pair<TestIterator, TestIterator> range =
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
void fillContainerWithData(CONTAINER&                            x,
                           const typename CONTAINER::value_type *data,
                           int                                   size)
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

template <class CONTAINER>
void validateIteration(CONTAINER &c)
{
    typedef typename CONTAINER::iterator       iterator;
    typedef typename CONTAINER::const_iterator const_iterator;
    typedef typename CONTAINER::size_type      size_type;

    const size_type size = c.size();

    size_type counter = 0;
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
    //: 1 number of buckets is returned by bucket_count
    //: 2 Number of buckets should reflect load_factor and max_load_factor
    //: 3 Each bucket hold a number of elements specified by bucket_size
    //: 4 bucket can be iterated from bucket_begin to bucket_end
    //:   o each element should match the bucket number via bucket(key)
    //:   o should have as many elements as reported by bucket_count
    //: 5 adding elements from all buckets should exactly equal 'size'
    //: 6 large buckets imply many hash collisions, which is undesirable
    //: 7 large buckets may be consequence of multicontainers

    typedef typename            CONTAINER::size_type             SizeType;
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
    pair<const_iterator, const_iterator> valRange = x.equal_range(key);
    ASSERT(valRange.second != x.end());  // or else container is *tiny*
    ASSERT(mX.erase(valRange.first, valRange.second) == valRange.second);
    ASSERT(x.size() + duplicates == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    //: o cIter is invalidated, so reset and start next sub-test
    //: o 10 range-erase all matching values for a given key value
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

    //: o cIter is invalidated, so reset and start next sub-test
    //: o range-erase all matching values for a given key value
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
    //: o cIter is invalidated, so reset and start next sub-test
    //: o range-erase all matching values for a given key value
    size = x.size();
    cIter = x.begin();
    key = keyForValue<CONTAINER>(*cIter);
    const_iterator next = cIter;
    while (key == keyForValue<CONTAINER>(*++next)) {
        cIter = next;
    }
    key = keyForValue<CONTAINER>(*next);
    while (key == keyForValue<CONTAINER>(*++next)) {}

    //: o cIter/next now point to elements either side of a key-range
    //: o confirm they are not in the same bucket:
    SizeType erasures = 0;
    while (cIter != next) {
        cIter = mX.erase(cIter);  // compile check for return type
        ++erasures;
    }
    ASSERT(x.size() + erasures == size);
    ASSERT(x.end() == x.find(key));
    validateIteration(mX);

    //: o erase elements straddling a bucket, in reverse order
    //: o cIter is invalidated, so reset and start next sub-test
    //: o range-erase all matching values for a given key value
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
    //: o cIter/next now point to elements either side of a key-range
    //: o confirm they are not in the same bucket:
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
    //
    // first truncate with a range-based erase
    cIter = mX.erase(cIter, mX.end());  // mixed const/mutable iterators
    ASSERT(cIter == x.end());
    validateIteration(mX);

    // then erase the rest of the container, one item at a time, from the front
    for (iterator it = mX.begin(); it != x.end(); it = mX.erase(it)) {}
    testEmptyContainer(mX);
}


template <class CONTAINER>
void testMapLookup(CONTAINER& mX)
{
    typedef typename CONTAINER::size_type    SizeType;
    typedef typename CONTAINER::iterator     iterator;

    typedef typename CONTAINER::   key_type  key_type;
    typedef typename CONTAINER::mapped_type  mapped_type;

    const CONTAINER& x = mX;
    // Assume there are no default key-values.  Enforce assumption with an
    // erase!
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

#ifndef BDE_BUILD_TARGET_NO_EXC
    try {
        mapped_type v = x.at(key_type());
        ASSERT(false); // prior line should throw
        (void) v;      // resolve unused variable warning
    }
    catch(const std::exception&) {
        // Expected code path.  Need to catch more specific exception though.
    }

    try {
        mapped_type v = mX.at(key_type());
        ASSERT(false); // prior line should throw
        (void) v;      // resolve unused variable warning
    }
    catch(const std::exception&) {
        // Expected code path.  Need to catch more specific exception though.
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
#endif
}


template <class CONTAINER>
void testImplicitInsert(CONTAINER& mX)
{
    typedef typename CONTAINER::   key_type    key_type;
    typedef typename CONTAINER::mapped_type mapped_type;

    const CONTAINER& x = mX;
    ASSERT(x.empty());  // assumption on entry

#ifndef BDE_BUILD_TARGET_NO_EXC
    try {
        mapped_type v = x.at(key_type());
        ASSERT(false); // prior line should throw
        (void) v;      // resolve unused variable warning
    }
    catch(const std::out_of_range&) {
        // Expected code path.  Need to catch more specific exception though.
    }

    try {
        mapped_type v = mX.at(key_type());
        ASSERT(false); // prior line should throw
        (void) v;      // resolve unused variable warning
    }
    catch(const std::out_of_range&) {
        // Expected code path.  Need to catch more specific exception though.
    }
#endif

    ASSERT(mX[key_type()] == mapped_type());
    ASSERT(x.size() == 1);  // want to see an error here before continuing

#ifndef BDE_BUILD_TARGET_NO_EXC
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
#endif
}

}  // close namespace BREATHING_TEST

//=============================================================================
//                            {DRQS 132030795}
//-----------------------------------------------------------------------------
namespace drqs132030795 {

// DRQS 132030795 deals with an error in Sun CC 12.3 (fixed in later versions)
// triggered by hash partial specializations for string.  The triggering code
// involves 'unordered_map' so we place the test here.  This code will not
// compile if the problem is present.

template <class TYPE>
void test1(bsl::unordered_map<bsl::string, TYPE> &)
    // Do nothing.
{
}

template <>
void test1(bsl::unordered_map<bsl::string, int> &)
    // Do nothing.
{
}

template <class TYPE>
void test2(bsl::unordered_map<bsl::wstring, TYPE> &)
    // Do nothing.
{
}

template <>
void test2(bsl::unordered_map<bsl::wstring, int> &)
    // Do nothing.
{
}

}

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
        // Testing: USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");
        usage();
      } break;
      case 42: // falls through
      case 41: // falls through
      case 40: // falls through
      case 39: // falls through
      case 38: // falls through
      case 37: // falls through
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
           "\nTEST CASE %d IS DELEGATED TO 'bslstl_unorderedmap_test.t.cpp'"
           "\n=============================================================\n",
           test);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'g' FUNCTION
        // --------------------------------------------------------------------

        if (verbose) printf("Testing 'g' FUNCTION\n"
                            "====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        // 'bsltf::MoveOnlyAllocTestType' no copy assign.

        TestDriver<TestKeyType, TestValueType>::testCase11();
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING -- N/A
        // --------------------------------------------------------------------

        if (verbose) printf("Testing BSLX STREAMING -- N/A\n"
                            "=============================\n");
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy Assignment\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonOptionalAllocTestType,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        // 'bsltf::MoveOnlyAllocTestType' no copy assign.

        TestDriver<TestKeyType, TestValueType>::testCase9();

        // 'propagate_on_container_copy_assignment' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::
                            testCase9_propagate_on_container_copy_assignment();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // Swap
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Swap\n"
                            "============\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase8();

        // 'propagate_on_container_swap' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase8_propagate_on_container_swap,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        // TBD test 'bsltf::MoveOnlyAllocTestType' here

        TestDriver<TestKeyType, TestValueType>::
                                       testCase8_propagate_on_container_swap();

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        // Test noexcept
#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int>     Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef u::ThrowingSwapHash<int> Hash;
            typedef bsl::equal_to<int>       Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef bsl::hash<int>            Hash;
            typedef u::ThrowingSwapEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int>     Hash;
            typedef bsl::equal_to<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef u::ThrowingSwapHash<int> Hash;
            typedef bsl::equal_to<int>       Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT( bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef bsl::hash<int>            Hash;
            typedef u::ThrowingSwapEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT( bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
#endif
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false> Alloc;
            typedef u::ThrowingSwapHash<int>  Hash;
            typedef u::ThrowingSwapEqual<int> Equal;

            ASSERT(!bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
        {
            typedef bsltf::StdStatefulAllocator<bsl::pair<const int, int>,
                                                false,
                                                false,
                                                false,
                                                false,
                                                true> Alloc;
            typedef u::ThrowingSwapHash<int>  Hash;
            typedef u::ThrowingSwapEqual<int> Equal;

            ASSERT( bsl::allocator_traits<Alloc>::is_always_equal::value);
            ASSERT(!bsl::is_nothrow_swappable<Hash>::value);
            ASSERT(!bsl::is_nothrow_swappable<Equal>::value);
            TestDriver<int, int, Hash, Equal, Alloc>::testCase8_noexcept();
        }
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Copy C'tor
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Copy C'tor\n"
                            "==================\n");

        // No copy c'tor for 'bsltf::MoveOnlyAllocTestType'.

        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase7();

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::
                             testCase7_select_on_container_copy_construction();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // operator==
        // --------------------------------------------------------------------

        if (verbose) printf("Testing operator==\n"
                            "==================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase6();
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // operator<< -- N/A
        // --------------------------------------------------------------------

        if (verbose) printf("Testing operator<< -- N/A\n"
                            "=========================\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Basic Accessors\n"
                            "=======================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST MECHANISMS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing DEFAULT_DATA table\n");

        int numMaxSpecs = 0;
        for (size_t ti = 0; ti < DEFAULT_NUM_DATA; ++ti) {
            const DefaultDataRow *pd     = DEFAULT_DATA + ti;
            const int             INDEX  = pd->d_index;
            const char           *SPEC   = pd->d_spec_p;
            const char           *RESULT = pd->d_results_p;
            const IntPtr          LENGTH = strlen(RESULT);

            char buf[100];
            strcpy(buf, SPEC);
            char *pEnd = buf + strlen(buf);
            std::sort(         buf + 0, pEnd);
            pEnd = std::unique(buf + 0, pEnd);
            *pEnd = 0;

            ASSERTV(pEnd - buf == LENGTH);
            ASSERTV(!strcmp(buf, RESULT));

            ASSERTV(strlen(SPEC) <= DEFAULT_MAX_LENGTH);
            numMaxSpecs += strlen(SPEC) == DEFAULT_MAX_LENGTH;

            for (const char *pc = SPEC; *pc; ++pc) {
                ASSERT('A' <= *pc && *pc <= 'Z');
            }

            for (size_t tj = 0; tj < DEFAULT_NUM_DATA; ++tj) {
                const DefaultDataRow *pdj     = DEFAULT_DATA + tj;
                const int             JINDEX  = pdj->d_index;
                const char           *JSPEC   = pdj->d_spec_p;
                const char           *JRESULT = pdj->d_results_p;

                ASSERTV((ti == tj)        == !strcmp(SPEC,   JSPEC));
                ASSERTV((INDEX == JINDEX) == !strcmp(RESULT, JRESULT));
            }
        }
        ASSERT(DEFAULT_NUM_MAX_LENGTH == numMaxSpecs);

        if (verbose) printf(
                       "Testing Range c'tor, 'ggg', 'gg', and 'verifySpec'\n"
                       "==================================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase3();

        RUN_EACH_TYPE(TestDriver,
                      testCase3_range,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase3_range,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase3_range();

        RUN_EACH_TYPE(TestDriver,
                      testCase3_verifySpec,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase3_verifySpec,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase3_verifySpec();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("Testing Primary Manipulators\n"
                            "============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
#endif

        TestDriver<TestKeyType, TestValueType>::testCase2();

        RUN_EACH_TYPE(TestDriver,
                      testCase2_WithCopy,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver,
                      testCase2_WithCopy,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);

        TestDriver<TestKeyType, TestValueType>::testCase2_WithCopy();
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
        {
            bsl::unordered_map<bsl::string,
                               bsl::pair<bsl::string, bsl::string> > mX;

            mX.insert({bsl::string{"banana"},
                      {bsl::string{"apple"}, bsl::string{"cherry"} } });
        }
#endif

        using namespace BREATHING_TEST;

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
        for (int i = 0; i != MAX_SAMPLE; ++i) {
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
            typedef pair<TestType::iterator, bool> InsertResult;
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
            typedef pair<TestType::iterator, bool> InsertResult;
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
            bsl::debugprint(mZ);
        }
        testErase(mZ);


        // quickly confirm assumptions on state of the containers
        ASSERT(z.empty());
        ASSERT(!x.empty());

        testMapLookup(mX);
        testImplicitInsert(mZ);  // double-check with an empty map

        if (veryVerbose) printf(
             "Call any remaining methods to be sure they at least compile.\n");

        mX[42] = 13;
        ASSERT(13 == x.at(42));

        const bsl::allocator<int> alloc   = x.get_allocator();
        const bsl::hash<int>      hasher  = x.hash_function();
        const bsl::equal_to<int>  compare = x.key_eq();

        (void) alloc;
        (void) hasher;
        (void) compare;

        const size_t maxSize    = x.max_size();
        const size_t buckets    = x.bucket_count();
        const float  loadFactor = x.load_factor();
        const float  maxLF      = x.max_load_factor();

        (void) maxSize;

        ASSERT(loadFactor < maxLF);

        mX.rehash(2 * buckets);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

        mX.reserve(0);
        ASSERTV(x.bucket_count(), 2 * buckets, x.bucket_count() > 2 * buckets);
        ASSERTV(x.load_factor(), loadFactor, x.load_factor() < loadFactor);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        if (verbose)
             printf("Test initializer lists.\n");
        {
            ASSERT((0 == []() -> bsl::unordered_map<char, int> {
                return {};
            }().size()));
            ASSERT((1 == []() -> bsl::unordered_map<char, int> {
                return {{'a', 1}};
            }().size()));
            ASSERT((2 == []() -> bsl::unordered_map<char, int> {
                return {{'a', 1}, {'b', 2}, {'a', 3}};
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
