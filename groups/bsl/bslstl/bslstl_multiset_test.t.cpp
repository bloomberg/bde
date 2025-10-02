// bslstl_multiset_test.t.cpp                                         -*-C++-*-

#include <bsls_platform.h>

// the following suppresses warnings from `#include` inlined functions

#if defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, SEC_NULL_PTR_DEREF)
#endif

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
# pragma GCC diagnostic ignored "-Warray-bounds"
# pragma GCC diagnostic ignored "-Wstringop-overflow="
#endif

#include <bslstl_multiset_test.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>
#include <bslstl_multiset.h>
#include <bslstl_pair.h>
#include <bslstl_randomaccessiterator.h>

#include <bslalg_rangecompare.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_bslallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>

#include <bsltf_allocargumenttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <algorithm>
#include <functional>
#include <utility>     // move

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
# include <initializer_list>
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
# include <ranges>
#endif

#include <stdlib.h>      // atoi

#ifdef BDE_BUILD_TARGET_ASAN
# ifdef BSLS_PLATFORM_CMP_GNU
#   pragma GCC diagnostic ignored "-Wlarger-than="
# endif
#endif

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, `bslstl_multiset.t.cpp` (cases 1-8, plus the usage example),
// and `bslstl_multiset_test.cpp` (cases 9 and higher).
//
//                             Overview
//                             --------
// The object under test is a container whose interface and contract is
// dictated by the C++ standard.  The general concerns are compliance,
// exception safety, and proper dispatching (for member function templates such
// as assign and insert).  This container is implemented in the form of a class
// template, and thus its proper instantiation for several types is a concern.
// Regarding the allocator template argument, we use mostly a `bsl::allocator`
// together with a `bslma::TestAllocator` mechanism, but we also verify the C++
// standard.
//
// Primary Manipulators:
//  - `insert`  (via helper function `primaryManipulator`)
//  - `clear`
//
// Basic Accessors:
//  - `cbegin`
//  - `cend`
//  - `size`
//  - `get_allocator`
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// `insert` and `clear` methods to be used by the generator functions `gg`.
// Note that some manipulators must support aliasing, and those that perform
// memory allocation must be tested for exception neutrality via the
// `bslma_testallocator` component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is no output or BDEX streaming below `bslstl`), we test each
// individual constructor, manipulator, and accessor in subsequent cases.
//
// ----------------------------------------------------------------------------
// 23.4.6.2, construct/copy/destroy:
// [19] multiset(const C& comparator, const A& allocator);
// [12] multiset(ITER first, ITER last, const C& comp, const A& alloc);
// [12] multiset(ITER first, ITER last, const A& alloc);
// [32] multiset(initializer_list<value_type>, const C& comp, const A& alloc);
// [32] multiset(initializer_list<value_type>, const A& allocator);
// [ 7] multiset(const multiset& original);
// [26] multiset(multiset&& original);
// [ 2] explicit multiset(const A& allocator);
// [ 7] multiset(const multiset& original, const A& allocator);
// [26] multiset(multiset&&, const ALLOCATOR&);
// [ 2] ~multiset();
// [ 9] multiset& operator=(const multiset& rhs);
// [27] multiset& operator=(multiset&& x);
// [32] multiset& operator=(initializer_list<value_type>);
// [ 4] allocator_type get_allocator() const;
//
// iterators:
// [14] iterator begin();
// [14] const_iterator begin() const;
// [14] iterator end();
// [14] const_iterator end() const;
// [14] reverse_iterator rbegin();
// [14] const_reverse_iterator rbegin() const;
// [14] reverse_iterator rend();
// [14] const_reverse_iterator rend() const;
// [ 4] const_iterator cbegin() const;
// [ 4] const_iterator cend() const;
// [14] const_reverse_iterator crbegin() const;
// [14] const_reverse_iterator crend() const;
//
// capacity:
// [20] bool empty() const;
// [ 4] size_type size() const;
// [20] size_type max_size() const;
//
// modifiers:
// [15] iterator insert(const value_type& value);
// [28] iterator insert(value_type&& value);
// [16] iterator insert(const_iterator position, const value_type& value);
// [29] iterator insert(const_iterator position, value_type&& value);
// [17] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [32] void insert(initializer_list<value_type>);
//
// [30] iterator emplace(Args&&... args);
// [31] iterator emplace_hint(const_iterator position, Args&&... args);
//
// [18] iterator erase(const_iterator position);
// [18] size_type erase(const key_type& key);
// [18] iterator erase(const_iterator first, const_iterator last);
// [ 8] void swap(multiset& other);
// [ 2] void clear();
//
// comparators:
// [21] key_compare key_comp() const;
// [21] value_compare value_comp() const;
//
// multiset operations:
// [13] iterator find(const key_type& key);
// [13] const_iterator find(const key_type& key) const;
// [13] size_type count(const key_type& key) const;
// [13] iterator lower_bound(const key_type& key);
// [13] const_iterator lower_bound(const key_type& key) const;
// [13] iterator upper_bound(const key_type& key);
// [13] const_iterator upper_bound(const key_type& key) const;
// [13] pair<iterator, iterator> equal_range(const key_type& key);
// [13] pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// [ 6] bool operator==(const multiset& lhs, const multiset& rhs);
// [19] bool operator< (const multiset& lhs, const multiset& rhs);
// [ 6] bool operator!=(const multiset& lhs, const multiset& rhs);
// [19] bool operator> (const multiset& lhs, const multiset& rhs);
// [19] bool operator>=(const multiset& lhs, const multiset& rhs);
// [19] bool operator<=(const multiset& lhs, const multiset& rhs);
//
//// specialized algorithms:
// [ 8] void swap(multiset& a, multiset& b);
// [36] erase_if(set<K, C, A>&s, PREDICATE);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [38] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(multiset *object, const char *spec, int verbose = 1);
// [ 3] multiset& gg(multiset *object, const char *spec);
//
// [22] CONCERN: The object is compatible with STL allocators.
// [23] CONCERN: The object has the necessary type traits
// [25] CONCERN: The type provides the full interface defined by the standard.
// [33] CONCERN: Methods qualifed `noexcept` in standard are so implemented.
// [34] CONCERN: `find`        properly handles transparent comparators.
// [34] CONCERN: `count`       properly handles transparent comparators.
// [34] CONCERN: `lower_bound` properly handles transparent comparators.
// [34] CONCERN: `upper_bound` properly handles transparent comparators.
// [34] CONCERN: `equal_range` properly handles transparent comparators.
// [35] CLASS TEMPLATE DEDUCTION GUIDES
// [37] CONCERN: `multiset` IS A C++20 RANGE

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
        fflush(stdout);
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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION >= 0x5130
// Some compilers struggle with the number of template instantiations in this
// test driver.  We define this macro to simplify the test driver for them,
// until such time as we can provide a more specific review of the type based
// concerns, and narrow the range of tests needed for confirmed coverage.
//
// Currently we are enabling the minimal set of test types on:
// Sun Studio 12.4            (CMP_SUN)
// (note: despite over-eager version check, we have not tested later compilers)
# define BSLSTL_MULTISET_TEST_LOW_MEMORY     1
#endif

#if defined(BSLSTL_MULTISET_TEST_LOW_MEMORY)
// For platforms that cannot sustain the full set of test concerns, reduce the
// number of elements in the most commonly use macro defining sets of test
// tyoes.
# undef  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR
# define BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR  \
        signed char,                                    \
        bsltf::TemplateTestFacility::MethodPtr,         \
        bsltf::AllocBitwiseMoveableTestType,            \
        bsltf::MovableAllocTestType
#endif

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for `emplace`-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BOOL_CONSTANT)
/// This leading branch is the preferred version for C++17, but the feature
/// test macro is (currently) for documentation purposes only, and never
/// defined.  This is the ideal (simplest) form for such declarations:
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    const BSLS_KEYWORD_CONSTEXPR bsl::bool_constant<EXPRESSION> NAME{}
#elif defined(BSLS_COMPILERFEATURES_SUPPORT_CONSTEXPR)
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    constexpr bsl::integral_constant<bool, EXPRESSION> NAME{}
    // This is the preferred C++11 form for the definition of integral constant
    // variables.  It assumes the presence of `constexpr` in the compiler as an
    // indication that brace-initialization and traits are available, as it has
    // historically been one of the last C++11 features to ship.
#else
# define DECLARE_BOOL_CONSTANT(NAME, EXPRESSION)                              \
    static const bsl::integral_constant<bool, EXPRESSION> NAME =              \
                 bsl::integral_constant<bool, EXPRESSION>()
    // `bsl::integral_constant` is not an aggregate prior to C++17 extending
    // the rules, so a C++03 compiler must explicitly initialize integral
    // constant variables in a way that is unambiguously not a vexing parse
    // that declares a function instead.
#endif

enum {
// The following enum is set to `1` when exceptions are enabled and to `0`
// otherwise.  It's here to avoid having preprocessor macros throughout.
#if defined(BDE_BUILD_TARGET_EXC)
    PLAT_EXC = 1
#else
    PLAT_EXC = 0
#endif
};

// ============================================================================
//                             SWAP TEST HELPERS
// ----------------------------------------------------------------------------

namespace incorrect {

/// Fail.  In a successful test, this `swap` should never be called.  It is
/// set up to be called (and fail) in the case where ADL fails to choose the
/// right `swap` in `invokeAdlSwap` below.
template <class TYPE>
void swap(TYPE&, TYPE&)
{
    ASSERT(0 && "incorrect swap called");
}

}  // close namespace incorrect

/// Exchange the values of the specified `*a` and `*b` objects using the
/// `swap` method found by ADL (Argument Dependent Lookup).
template <class TYPE>
void invokeAdlSwap(TYPE *a, TYPE *b)
{
    using incorrect::swap;

    // A correct ADL will key off the types of `*a` and `*b`, which will be of
    // our `bsl` container type, to find the right `bsl::swap` and not
    // `incorrect::swap`.

    swap(*a, *b);
}

/// Exchange the values of the specified `*a` and `*b` objects using the
/// `swap` method found by the recommended pattern for calling `swap`.
template <class TYPE>
void invokePatternSwap(TYPE *a, TYPE *b)
{
    // Invoke `swap` using the recommended pattern for `bsl` clients.

    using bsl::swap;

    swap(*a, *b);
}

// The following `using` directives must come *after* the definition of
// `invokeAdlSwap` and `invokePatternSwap` (above).

using namespace BloombergLP;
using bsl::pair;
using bsl::multiset;
using bsls::NameOf;

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

typedef bsls::Types::Int64 Int64;

// Define DEFAULT DATA used in multiple test cases.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to `gg` function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "A",                 "A" },
    { L_,    2, "AA",                "AA" },
    { L_,    3, "ABCABC",            "AABBCC" },
    { L_,    3, "AABBCC",            "AABBCC" },
    { L_,    4, "ABCA",              "AABC" },
    { L_,    5, "AB",                "AB" },
    { L_,    5, "BA",                "AB" },
    { L_,    6, "ABCB",              "ABBC" },
    { L_,    7, "ABC",               "ABC" },
    { L_,    7, "ACB",               "ABC" },
    { L_,    7, "BAC",               "ABC" },
    { L_,    7, "BCA",               "ABC" },
    { L_,    7, "CAB",               "ABC" },
    { L_,    7, "CBA",               "ABC" },
    { L_,    8, "ABCC",              "ABCC" },
    { L_,    9, "ABCD",              "ABCD" },
    { L_,    9, "ACBD",              "ABCD" },
    { L_,    9, "BDCA",              "ABCD" },
    { L_,    9, "DCBA",              "ABCD" },
    { L_,   10, "ABCDE",             "ABCDE" },
    { L_,   10, "ACBDE",             "ABCDE" },
    { L_,   10, "CEBDA",             "ABCDE" },
    { L_,   10, "EDCBA",             "ABCDE" },
    { L_,   11, "FEDCBA",            "ABCDEF" },
    { L_,   12, "ABCDEFG",           "ABCDEFG" },
    { L_,   13, "ABCDEFGH",          "ABCDEFGH" },
    { L_,   14, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,   15, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP" },
    { L_,   15, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP" },
    { L_,   16, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   16, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   17, "BAD",               "ABD" },
    { L_,   18, "BEAD",              "ABDE" },
    { L_,   19, "AC",                "AC" },
    { L_,   20, "B",                 "B" },
    { L_,   21, "BCDE",              "BCDE" },
    { L_,   22, "FEDCB",             "BCDEF" },
    { L_,   23, "CD",                "CD" },
};
static const size_t DEFAULT_NUM_DATA =
                                    sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// Define values used to initialize positional arguments for
// `bsltf::EmplacableTestType` and `bsltf::AllocEmplacableTestType`
// constructors.  Note, that you cannot change those values as they are used by
// `TemplateTestFacility::getIdentifier` to map the constructed emplacable
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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl {

// multiset-specific print function.
template <class KEY, class COMP, class ALLOC>
void debugprint(const bsl::multiset<KEY, COMP, ALLOC>& s)
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef typename bsl::multiset<KEY, COMP, ALLOC>::const_iterator CIter;

        putchar('"');
        for (CIter it = s.begin(); it != s.end(); ++it) {
            putchar(static_cast<char>(
                             bsltf::TemplateTestFacility::getIdentifier(*it)));
        }
        putchar('"');
    }
    fflush(stdout);
}

}  // close namespace bsl

namespace {

/// Return `true` if the container is expected to allocate memory on the
/// specified `n`th element, and `false` otherwise.
bool expectToAllocate(size_t n)
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when `n` is a power of 2
}

/// Verify the specified `container` has the specified `expectedSize` and
/// contains the same values as the array in the specified `expectedValues`.
/// Return 0 if `container` has the expected values, and a non-zero value
/// otherwise.
template<class CONTAINER, class VALUES>
size_t verifyContainer(const CONTAINER&  container,
                       const VALUES&     expectedValues,
                       size_t            expectedSize)
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if (expectedSize != container.size()) {
        return static_cast<size_t>(-1);                               // RETURN
    }

    typename CONTAINER::const_iterator it = container.cbegin();
    for (size_t i = 0; i < expectedSize; ++i) {
        ASSERTV(it != container.cend());
        ASSERTV(i, expectedValues[i], *it, expectedValues[i] == *it);

        if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[i])
            != bsltf::TemplateTestFacility::getIdentifier(*it)) {
            return i + 1;                                             // RETURN
        }
        ++it;
    }
    return 0;
}

                            // ==========================
                            // class StatefulStlAllocator
                            // ==========================

/// This class implements a standard compliant allocator that has an
/// attribute, `id`.
template <class VALUE>
class StatefulStlAllocator : public bsltf::StdTestAllocator<VALUE>
{
    // DATA
    int d_id;  // identifier

  private:
    // TYPES

    /// Alias for the base class.
    typedef bsltf::StdTestAllocator<VALUE> StlAlloc;

  public:
    /// This nested `struct` template, parameterized by some
    /// `BDE_OTHER_TYPE`, provides a namespace for an `other` type alias,
    /// which is an allocator type following the same template as this one
    /// but that allocates elements of `BDE_OTHER_TYPE`.  Note that this
    /// allocator type is convertible to and from `other` for any
    /// `BDE_OTHER_TYPE` including `void`.
    template <class BDE_OTHER_TYPE>
    struct rebind
    {

        typedef StatefulStlAllocator<BDE_OTHER_TYPE> other;
    };

    // CREATORS

    /// Create a `StatefulStlAllocator` object.
    StatefulStlAllocator()
    : StlAlloc()
    {
    }

    // StatefulStlAllocator(const StatefulStlAllocator& original) = default;
        // Create a `StatefulStlAllocator` object having the same id as the
        // specified `original`.

    /// Create a `StatefulStlAllocator` object having the same id as the
    /// specified `original` with a different template type.
    template <class BDE_OTHER_TYPE>
    StatefulStlAllocator(const StatefulStlAllocator<BDE_OTHER_TYPE>& original)
    : StlAlloc(original)
    , d_id(original.id())
    {
    }

    // MANIPULATORS

    /// Set the `id` attribute of this object to the specified `value`.
    void setId(int value)
    {
        d_id = value;
    }

    // ACCESSORS

    /// Return the value of the `id` attribute of this object.
    int id() const
    {
        return d_id;
    }
};

                            // ======================
                            // class ExceptionProctor
                            // ======================

/// This class provides a mechanism to verify the strong exception guarantee
/// in exception-throwing code.  On construction, this class stores a copy
/// of an object of the (template parameter) type `OBJECT` and the address
/// of that object.  On destruction, if `release` was not invoked, it will
/// verify the value of the object is the same as the value of the copy
/// created on construction.  This class requires that the copy constructor
/// and `operator ==` be tested before use.
template <class OBJECT, class ALLOCATOR>
struct ExceptionProctor {

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

    /// Create the exception proctor for the specified `object` at the
    /// specified `line` number that uses the specified `basicAllocator` to
    /// supply memory for the control object.
    ExceptionProctor(const OBJECT     *object,
                     int               line,
                     const ALLOCATOR&  basicAllocator)
    : d_line(line)
    , d_control(*object, basicAllocator)
    , d_object_p(object)
    {
    }

    /// Create an exception proctor for the specified `object` at the
    /// specified `line` number using the specified `control` object.
    ExceptionProctor(const OBJECT              *object,
                     int                        line,
                     bslmf::MovableRef<OBJECT>  control)
    : d_line(line)
    , d_control(bslmf::MovableRefUtil::move(control))
    , d_object_p(object)
    {
    }

    /// Destroy this exception proctor.  If the proctor was not released,
    /// verify that the state of the object supplied at construction has not
    /// changed.
    ~ExceptionProctor()
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_control, *d_object_p, d_control == *d_object_p);
        }
    }

    // MANIPULATORS

    /// Release this proctor from verifying the state of the object
    /// supplied at construction.
    void release()
    {
        d_object_p = 0;
    }
};

bool g_enableLessThanFunctorFlag = true;

                       // ====================
                       // class TestComparator
                       // ====================

/// This test class provides a mechanism that defines a function-call
/// operator that compares two objects of the parameterized `TYPE`.  The
/// function-call operator is implemented with integer comparison using
/// integers converted from objects of `TYPE` by the class method
/// `TemplateTestFacility::getIdentifier`.  The function-call operator also
/// increments a counter used to keep track the method call count.  Object
/// of this class can be identified by an id passed on construction.
template <class TYPE>
class TestComparator {

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
    mutable int d_count;        // number of times `operator()` is called

  public:
    // CLASS METHOD

    /// Disable all objects of `TestComparator` such that an `ASSERT` will
    /// be triggered if `operator()` is invoked
    static void disableFunctor()
    {
        g_enableLessThanFunctorFlag = false;
    }

    /// Enable all objects of `TestComparator` such that `operator()` may
    /// be invoked
    static void enableFunctor()
    {
        g_enableLessThanFunctorFlag = true;
    }

    // CREATORS
    //! TestComparator(const TestComparator& original) = default;
        // Create a copy of the specified `original`.

    /// Create a `TestComparator`.  Optionally, specify `id` that can be
    /// used to identify the object.
    explicit TestComparator(int id = 0, bool compareLess = true)
    : d_id(id)
    , d_compareLess(compareLess)
    , d_count(0)
    {
    }

    // ACCESSORS

    /// Increment a counter that records the number of times this method is
    /// called.   Return `true` if the integer representation of the
    /// specified `lhs` is less than integer representation of the specified
    /// `rhs`.
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
    {
        if (!g_enableLessThanFunctorFlag) {
            ASSERTV("`TestComparator` was invoked when disabled", false);
        }

        ++d_count;

        if (d_compareLess) {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
            < bsltf::TemplateTestFacility::getIdentifier(rhs);        // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
            > bsltf::TemplateTestFacility::getIdentifier(rhs);        // RETURN
        }
    }

    bool operator== (const TestComparator& rhs) const
    {
        return (id() == rhs.id() && d_compareLess == rhs.d_compareLess);
    }

    /// Return the `id` of this object.
    int id() const
    {
        return d_id;
    }

    /// Return the number of times `operator()` is called.
    size_t count() const
    {
        return d_count;
    }
};

                       // ============================
                       // class TestComparatorNonConst
                       // ============================

/// This test class provides a mechanism that defines a non-`const`
/// function-call operator that compares two objects of the parameterized
/// `TYPE`.  The function-call operator is implemented with integer
/// comparison using integers converted from objects of `TYPE` by the class
/// method `TemplateTestFacility::getIdentifier`.  The function-call
/// operator also increments a counter used to keep track the method call
/// count.  Object of this class can be identified by an id passed on
/// construction.
template <class TYPE>
class TestComparatorNonConst {

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
    mutable int d_count;        // number of times `operator()` is called

  public:
    // CREATORS

    explicit TestComparatorNonConst(int id = 0, bool compareLess = true)
    : d_id(id)
    , d_compareLess(compareLess)
    , d_count(0)
    {
    }

    // ACCESSORS

    /// Increment a counter that records the number of times this method is
    /// called.   Return `true` if the integer representation of the
    /// specified `lhs` is less than integer representation of the specified
    /// `rhs`.
    bool operator() (const TYPE& lhs, const TYPE& rhs)
    {
        ++d_count;

        if (d_compareLess) {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
                 < bsltf::TemplateTestFacility::getIdentifier(rhs);   // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
                 > bsltf::TemplateTestFacility::getIdentifier(rhs);   // RETURN
        }
    }

    bool operator== (const TestComparatorNonConst& rhs) const
    {
        return id() == rhs.id();
    }

    /// Return the `id` of this object.
    int id() const
    {
        return d_id;
    }

    /// Return the number of times `operator()` is called.
    size_t count() const
    {
        return d_count;
    }
};

                    // ============================
                    // struct TransparentComparator
                    // ============================

/// This class can be used as a comparator for containers.  It has a nested
/// type `is_transparent`, so it is classified as transparent by the
/// `bslmf::IsTransparentPredicate` metafunction and can be used for
/// heterogeneous comparison.
struct TransparentComparator
 {
    typedef void is_transparent;

    /// Return `true` if the specified `lhs` is less than the specified
    /// `rhs` and `false` otherwise.
    template <class LHS, class RHS>
    bool operator()(const LHS& lhs, const RHS& rhs) const
    {
        return lhs < rhs;
    }
};

                    // =============================
                    // class TransparentlyComparable
                    // =============================

class TransparentlyComparable {
    // DATA
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
    mutable
#endif
    int d_conversionCount;  // number of times `operator int` has been called
    int d_value;            // the value

    // NOT IMPLEMENTED
    TransparentlyComparable(const TransparentlyComparable&);  // = delete

  public:
    // CREATORS

    /// Create an object having the specified `value`.
    explicit TransparentlyComparable(int value)

    : d_conversionCount(0)
    , d_value(value)
    {
    }

    // MANIPULATORS

    /// Return the current value of this object.
    operator int()
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L
    const
#endif
    {
        ++d_conversionCount;
        return d_value;
    }

    /// return a count of the number of times this object has been converted to
    /// an int.
    void resetConversionCount()
    {
        d_conversionCount = 0;
    }

    // ACCESSORS

    /// Return the number of times `operator int` has been called.
    int conversionCount() const
    {
        return d_conversionCount;
    }

    /// Return the current value of this object.
    int value() const
    {
        return d_value;
    }

    /// Return `true` if the value of the specified `lhs` is less than the
    /// specified `rhs`, and `false` otherwise.
    friend bool operator<(const TransparentlyComparable& lhs, int rhs)
    {
        return lhs.d_value < rhs;
    }

    /// Return `true` if the specified `lhs` is less than the value of the
    /// specified `rhs`, and `false` otherwise.
    friend bool operator<(int lhs, const TransparentlyComparable& rhs)
    {
        return lhs < rhs.d_value;
    }
};

/// Search for a value equal to the specified `initKeyValue` in the
/// specified `container`, and count the number of conversions expected
/// based on the specified `isTransparent`.  Note that `Container` may
/// resolve to a `const`-qualified type, we are using the "reference" here
/// as a sort of universal reference.  Conceptually, the object remains
/// constant, but we want to test `const`-qualified and
/// non-`const`-qualified overloads.
template <class Container>
void testTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
{
    typedef typename Container::const_iterator Iterator;
    typedef typename Container::size_type      Count;

    const int expectedConversionCount = isTransparent ? 0 : 1;

    TransparentlyComparable existingKey(initKeyValue);
    TransparentlyComparable nonExistingKey(initKeyValue ? -initKeyValue
                                                        : -100);

    {
        // Testing `find`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Iterator EXISTING_F = container.find(existingKey);

        ASSERT(container.end()               != EXISTING_F);
        ASSERT(existingKey.value()           == *EXISTING_F);
        ASSERTV(isTransparent,
                  expectedConversionCount,   existingKey.conversionCount(),
                  expectedConversionCount == existingKey.conversionCount());

        const Iterator NON_EXISTING_F = container.find(nonExistingKey);
        ASSERT(container.end()                  == NON_EXISTING_F);
        ASSERTV(isTransparent,
                expectedConversionCount,   nonExistingKey.conversionCount(),
                expectedConversionCount == nonExistingKey.conversionCount());
    }

    {
        // Testing `contains`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const bool EXISTING_CONTAINS = container.contains(existingKey);

        ASSERT(true == EXISTING_CONTAINS);
        ASSERTV(isTransparent,
                  expectedConversionCount,   existingKey.conversionCount(),
                  expectedConversionCount == existingKey.conversionCount());

        const bool NON_EXISTING_CONTAINS = container.contains(nonExistingKey);
        ASSERT(false == NON_EXISTING_CONTAINS);
        ASSERTV(isTransparent,
                expectedConversionCount,   nonExistingKey.conversionCount(),
                expectedConversionCount == nonExistingKey.conversionCount());
    }

    {
        // Testing `count`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Count EXPECTED_C = initKeyValue ? initKeyValue : 1;
        const Count EXISTING_C = container.count(existingKey);

        ASSERT(EXPECTED_C              == EXISTING_C);
        ASSERTV(isTransparent,
                  expectedConversionCount,   existingKey.conversionCount(),
                  expectedConversionCount == existingKey.conversionCount());

        const Count NON_EXISTING_C = container.count(nonExistingKey);
        ASSERT(0                       == NON_EXISTING_C);
        ASSERTV(isTransparent,
                expectedConversionCount,   nonExistingKey.conversionCount(),
                expectedConversionCount == nonExistingKey.conversionCount());
    }

    {
        // Testing `lower_bound`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const Iterator EXISTING_LB = container.lower_bound(existingKey);

        ASSERT(existingKey.value() == *EXISTING_LB);
        ASSERTV(isTransparent,
                  expectedConversionCount,   existingKey.conversionCount(),
                  expectedConversionCount == existingKey.conversionCount());

        const Iterator NON_EXISTING_LB = container.lower_bound(nonExistingKey);

        ASSERT(container.begin()       == NON_EXISTING_LB);
        ASSERTV(isTransparent,
                expectedConversionCount,   nonExistingKey.conversionCount(),
                expectedConversionCount == nonExistingKey.conversionCount());
    }

    {
        // Testing `upper_bound`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        TransparentlyComparable upperBoundValue(initKeyValue + 1);
        const Iterator          EXPECTED_UB = container.find(upperBoundValue);
        const Iterator          EXISTING_UB = container.upper_bound(existingKey);

        ASSERT(EXPECTED_UB             == EXISTING_UB);
        ASSERTV(isTransparent,
                  expectedConversionCount,   existingKey.conversionCount(),
                  expectedConversionCount == existingKey.conversionCount());

        const Iterator NON_EXISTING_UB = container.upper_bound(nonExistingKey);

        ASSERT(container.begin()       == NON_EXISTING_UB);
        ASSERTV(isTransparent,
                expectedConversionCount,   nonExistingKey.conversionCount(),
                expectedConversionCount == nonExistingKey.conversionCount());
    }

    {
        // Testing `equal_range`.
        existingKey.resetConversionCount();
        nonExistingKey.resetConversionCount();

        const bsl::pair<Iterator, Iterator> EXISTING_ER =
                                                container.equal_range(existingKey);
        ASSERT(EXISTING_ER.first != EXISTING_ER.second);
        ASSERT(0 < static_cast<Count>(
                        bsl::distance(EXISTING_ER.first, EXISTING_ER.second)));
        for (Iterator it = EXISTING_ER.first; it != EXISTING_ER.second; ++it) {
            ASSERT(existingKey.value() == *it);
        }

        ASSERTV(isTransparent,
                  expectedConversionCount,   existingKey.conversionCount(),
                  expectedConversionCount == existingKey.conversionCount());

        const bsl::pair<Iterator, Iterator> NON_EXISTING_ER =
                                             container.equal_range(nonExistingKey);

        ASSERT(NON_EXISTING_ER.first == NON_EXISTING_ER.second);
        ASSERTV(isTransparent,
                expectedConversionCount,   nonExistingKey.conversionCount(),
                expectedConversionCount == nonExistingKey.conversionCount());
    }
}

                            // ========================
                            // class ThrowingComparator
                            // ========================

/// This dummy class implements the minimal interface that meets the
/// requirements for a comparator that can throw exceptions from the move
/// assignment operator and from the `swap` method/free function.
template <class TYPE>
struct ThrowingComparator : public std::less<TYPE> {

  public:
    // CREATORS

    /// Create a `ThrowingComparator` object.
    ThrowingComparator()
    {
    }

    /// Create a `ThrowingComparator` object.
    ThrowingComparator(const ThrowingComparator&)
    {
    }

    /// Create a `ThrowingComparator` object.
    ThrowingComparator(bslmf::MovableRef<ThrowingComparator>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    {
    }

    // MANIPULATORS

    /// Return a reference, providing modifiable access to this object.
    ThrowingComparator &operator=(const ThrowingComparator&)
    {
        return *this;
    }

    /// Return a reference, providing modifiable access to this object.
    ThrowingComparator&
    operator=(BloombergLP::bslmf::MovableRef<ThrowingComparator>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    {
        return *this;
    }

    /// Do nothing.
    void swap(ThrowingComparator&) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    {
    }
};

// FREE FUNCTIONS

/// Do nothing.
template <class TYPE>
void swap(ThrowingComparator<TYPE>&,
          ThrowingComparator<TYPE>&) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
{
}

                       // =====================
                       // class TemplateWrapper
                       // =====================

/// This class inherits from the container, but do nothing otherwise.  A
/// compiler bug in AIX prevents the compiler from finding the definition of
/// the default arguments for the constructor.  This class is created to
/// test this scenario.
template <class KEY, class COMPARATOR, class ALLOCATOR>
class TemplateWrapper {

    // DATA
    bsl::multiset<KEY, COMPARATOR, ALLOCATOR> d_member;

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

                       // =====================
                       // class TemplateWrapper
                       // =====================

/// A dummy comparator class.  Must be defined after `TemplateWrapper` to
/// reproduce the AIX bug.
class DummyComparator {

  public:
    bool operator() (int, int)
    {
        return true;
    }
};

                       // ====================
                       // class DummyAllocator
                       // ====================

/// A dummy allocator class.  Must be defined after `TemplateWrapper` to
/// reproduce the AIX bug.  Every method is a noop.
template <class TYPE>
class DummyAllocator {

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
    DummyAllocator() {}

    // DummyAllocator(const DummyAllocator& original) = default;

    template <class BDE_OTHER_TYPE>
    DummyAllocator(const DummyAllocator<BDE_OTHER_TYPE>&) {}

    // ~DummyAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    // DummyAllocator& operator=(const DummyAllocator& rhs) = default;

    pointer allocate(size_type    /* numElements */,
                     const void * /* hint */ = 0)
    {
        return 0;
    }

    void deallocate(pointer /* address */, size_type /* numElements */ = 1) {}

    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE * /* address */) {}
    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE * /* address */,
                   const ELEMENT_TYPE& /* value */) { }

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE * /* address */) {}

    // ACCESSORS
    pointer address(reference /* object */) const { return 0; }

    const_pointer address(const_reference /* object */) const { return 0; }

    size_type max_size() const { return 0; }
};

/// This test class provides a mechanism that defines a function-call
/// operator that compares two objects of the parameterized `TYPE`.  The
/// function-call operator is implemented with integer comparison using
/// integers converted from objects of `TYPE` by the class method
/// `TemplateTestFacility::getIdentifier`.
template <class TYPE>
class GreaterThanFunctor {

  public:
    // ACCESSORS

    /// Return `true` if the integer representation of the specified `lhs`
    /// is less than integer representation of the specified `rhs`.
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
    {
        return bsltf::TemplateTestFacility::getIdentifier(lhs)
             > bsltf::TemplateTestFacility::getIdentifier(rhs);
    }
};

// FREE OPERATORS

/// Return `true` if the integer representation of the specified `lhs` is
/// less than integer representation of the specified `rhs`.
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
{
    return bsltf::TemplateTestFacility::getIdentifier(lhs)
         < bsltf::TemplateTestFacility::getIdentifier(rhs);
}

}  // close unnamed namespace

                             // ============
                             // class EqPred
                             // ============

/// A predicate for testing `erase_if`; it takes a value at construction
/// and uses it for comparisons later.
template <class TYPE>
struct EqPred
{
    TYPE d_ch;
    EqPred(TYPE ch) : d_ch(ch) {}

    /// return `true` if the specified `ch` is equal to the stored value,
    /// and `false` otherwise.
    bool operator() (TYPE ch) const
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

/// This templatized struct provide a namespace for testing the `multiset`
/// container.  The parameterized `KEY`, `COMP` and `ALLOC` specifies the
/// value type, comparator type and allocator type respectively.  Each
/// "testCase*" method test a specific aspect of
/// `multiset<KEY, COMP, ALLOC>`.  Every test cases should be invoked with
/// various parameterized type to fully test the container.
template <class KEY,
          class COMP  = TestComparator<KEY>,
          class ALLOC = bsl::allocator<KEY> >
class TestDriver {

  private:
    // TYPES

    // Shorthands
    typedef bsl::multiset<KEY, COMP, ALLOC>       Obj;
    typedef typename Obj::iterator                Iter;
    typedef typename Obj::const_iterator          CIter;
    typedef typename Obj::reverse_iterator        RIter;
    typedef typename Obj::const_reverse_iterator  CRIter;
    typedef typename Obj::size_type               SizeType;
    typedef typename Obj::value_type              ValueType;

    typedef bslma::ConstructionUtil               ConsUtil;
    typedef bsltf::MoveState                      MoveState;
    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::TemplateTestFacility           TstFacility;
    typedef bsltf::TestValuesArray<KEY, ALLOC>    TestValues;
    typedef TestMovableTypeUtil<Iter, KEY>        TstMoveUtil;

    /// Comparator functor with a non-`const` function call operator.
    typedef TestComparatorNonConst<KEY>           NonConstComp;

    typedef bsl::allocator_traits<ALLOC>                AllocatorTraits;
    typedef typename ALLOC::template rebind<KEY>::other KeyAllocator;

    enum AllocCategory { e_BSLMA, e_STDALLOC, e_ADAPTOR, e_STATEFUL };

    enum { k_KEY_IS_WELL_BEHAVED =
                      bsl::is_same<KEY,
                               bsltf::WellBehavedMoveOnlyAllocTestType>::value,
           k_KEY_IS_MOVE_ENABLED =
                      bsl::is_same<KEY, bsltf::MovableTestType>::value ||
                      bsl::is_same<KEY, bsltf::MovableAllocTestType>::value ||
                      bsl::is_same<KEY, bsltf::MoveOnlyAllocTestType>::value ||
                      k_KEY_IS_WELL_BEHAVED };

  public:
    typedef bsltf::StdTestAllocator<KEY>          StlAlloc;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given `spec` in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the `multiset<KEY, COMP, ALLOC>` object.
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
    // <ITEM>       ::= <ELEMENT>
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

    // CLASS DATA
    static
    const AllocCategory s_allocCategory =
                        bsl::is_same<ALLOC, bsl::allocator<KEY> >::value
                        ? e_BSLMA
                        : bsl::is_same<ALLOC,
                                       bsltf::StdAllocTestType<
                                                 bsl::allocator<KEY> > >::value
                        ? e_STDALLOC
                        : bsl::is_same<ALLOC,
                                       bsltf::StdAllocatorAdaptor<
                                           bsl::allocator<ValueType> > >::value
                        ? e_ADAPTOR
                        : e_STATEFUL;

    // CLASS METHODS
    static
    const char *allocCategoryAsStr()
    {
        return e_BSLMA == s_allocCategory ? "bslma"
                                          : e_STDALLOC == s_allocCategory
                                          ? "stdalloc"
                                          : e_ADAPTOR == s_allocCategory
                                          ? "adaptor"
                                          : e_STATEFUL == s_allocCategory
                                          ? "stateful"
                                          : "<INVALID>";
    }

    /// Configure the specified `object` according to the specified `spec`,
    /// using only the primary manipulator function `insert` and white-box
    /// manipulator `clear`.  Optionally specify a zero `verbose` to
    /// suppress `spec` syntax error messages.  Return the index of the
    /// first invalid character, and a negative value otherwise.  Note that
    /// this function is used to implement `gg` as well as allow for
    /// verification of syntax error detection.
    static int ggg(Obj *object, const char *spec, int verbose = 1);

    /// Return, by reference, the specified object with its value adjusted
    /// according to the specified `spec`.
    static Obj& gg(Obj *object, const char *spec);

    static void storeFirstNElemAddr(typename Obj::const_pointer *pointers,
                                    const Obj&          object,
                                    size_t              n)
    {
        size_t i = 0;
        for (CIter b = object.cbegin(); b != object.cend() && i < n; ++b)
        {
            pointers[i++] = bsls::Util::addressOf(*b);
        }
    }

    static
    int checkFirstNElemAddr(typename Obj::const_pointer *pointers,
                            const Obj&           object,
                            size_t               n)
    {
        int count = 0;
        size_t i = 0;
        for (CIter b = object.cbegin(); b != object.end() && i < n; ++b)
        {
            if (pointers[i++] != bsls::Util::addressOf(*b)) {
                ++count;
            }
        }
        return count;
    }

    /// Insert into the specified `container` the value object indicated by
    /// the specified `identifier`, ensuring that the overload of the
    /// primary manipulator taking a modifiable rvalue is invoked (rather
    /// than the one taking an lvalue).  Return the result of invoking the
    /// primary manipulator.
    static Iter primaryManipulator(Obj   *container,
                                   int    identifier,
                                   ALLOC  allocator)
    {
        bsls::ObjectBuffer<ValueType> buffer;
        TstFacility::emplace(buffer.address(),
                             identifier,
                             allocator);
        bslma::DestructorGuard<KEY> guard(buffer.address());

        return container->insert(MoveUtil::move(buffer.object()));
    }

    template <class T>
    static const T& testArg(T& t, bsl::false_type)
    {
        return t;
    }

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
    {
        return MoveUtil::move(t);
    }

    /// Call `emplace` on the specified `target` container and verify that a
    /// value was inserted.  Forward (template parameter) `N_ARGS` arguments
    /// to the `emplace` method and ensure 1) that values are properly
    /// passed to the constructor of `value_type`, 2) that the allocator is
    /// correctly configured for each argument in the newly inserted element
    /// in `target`, and 3) that the arguments are forwarded using copy or
    /// move semantics based on integer template parameters `N01` ... `N10`.
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
    static void testCase30a_RunTest(Obj *target);

    /// Call `emplace_hint` on the specified `target` container and verify
    /// that a value was inserted.  Forward (template parameter) `N_ARGS`
    /// arguments to the `emplace` method and ensure 1) that values are
    /// properly passed to the constructor of `value_type`, 2) that the
    /// allocator is correctly configured for each argument in the newly
    /// inserted element in `target`, and 3) that the arguments are
    /// forwarded using copy or move semantics based on integer template
    /// parameters `N01` ... `N10`.
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
    static Iter testCase31a_RunTest(Obj *target, CIter hint);

  public:
    // TEST CASES

    /// Test whether `multiset` is a C++20 range.
    static void testCase37_isRange();

    /// Test free function `bsl::erase_if`
    static void testCase36();

    /// Test `noexcept` specifications
    static void testCase33();

    /// Test initializer list functions.
    static void testCase32();

    /// Test forwarding of arguments in `emplace_hint` method.
    static void testCase31a();

    /// Test `emplace_hint` method.
    static void testCase31();

    /// Test forwarding of arguments in `emplace` method.
    static void testCase30a();

    /// Test `emplace` method.
    static void testCase30();

    /// Test `insert` on movable value with hint.
    static void testCase29();

    /// Test `insert` on movable value.
    static void testCase28();

    /// Test move assignment.
    static void testCase27_dispatch();

    /// Test move construction.
    static void testCase26();

    /// Test standard interface coverage.
    static void testCase25();

    /// Test constructor of a template wrapper class.
    static void testCase24();

    /// Test type traits.
    static void testCase23();

    /// Test STL allocator.
    static void testCase22();

    /// Test comparators.
    static void testCase21();

    /// Test `max_size` and `empty`.
    static void testCase20();

    /// Test comparison free operators.  `operator <` must be defined for
    /// the parameterized `KEY`.
    static void testCase19();

    /// Test `erase`.
    static void testCase18();

    /// Test range `insert`.
    static void testCase17();

    /// Test `insert` with hint.
    static void testCase16();

    /// Test `insert`.
    static void testCase15();

    /// Test iterators.
    static void testCase14();

    /// Test find, upper_bound, lower_bound.
    static void testCase13();

    /// Test user-supplied constructors.
    static void testCase12();

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();

    /// Test `propagate_on_container_copy_assignment`.
    static void testCase9_propagate_on_container_copy_assignment();

    /// Test assignment operator (`operator=`).
    static void testCase9();
};

template <class KEY>
class StdAllocTestDriver : public TestDriver<KEY,
                                             TestComparator<KEY>,
                                             bsltf::StdTestAllocator<KEY> >
{
};

template <class KEY>
class StdBslmaTestDriver : public TestDriver<KEY,
                                             TestComparator<KEY>,
                             bsltf::StdAllocatorAdaptor<bsl::allocator<KEY> > >
{
};
                               // ----------------
                               // class TestDriver
                               // ----------------

template <class KEY, class COMP, class ALLOC>
int TestDriver<KEY, COMP, ALLOC>::ggg(Obj        *object,
                                      const char *spec,
                                      int         verbose)
{
    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            primaryManipulator(object, spec[i], object->get_allocator());
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

template <class KEY, class COMP, class ALLOC>
bsl::multiset<KEY, COMP, ALLOC>& TestDriver<KEY, COMP, ALLOC>::gg(
                                                            Obj        *object,
                                                            const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase37_isRange()
{
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
    BSLMF_ASSERT((std::ranges::common_range<Obj>));
    BSLMF_ASSERT((std::ranges::bidirectional_range<Obj>));
    BSLMF_ASSERT((std::ranges::sized_range<Obj>));
    BSLMF_ASSERT((std::ranges::viewable_range<Obj>));

    BSLMF_ASSERT((!std::ranges::view<Obj>));
    BSLMF_ASSERT((!std::ranges::borrowed_range<Obj>));
#endif
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase36()
{
    // --------------------------------------------------------------------
    // TESTING FREE FUNCTION `BSL::ERASE_IF`
    //
    // Concerns:
    // 1. The free function exists, and is callable with a multiset.
    //
    // Plan:
    // 1. Fill a set with known values, then attempt to erase some of
    //    the values using `bsl::erase_if`.  Verify that the resultant set
    //    is the right size, contains the correct values, and that the
    //    value returned from the functions is correct.
    //
    // Testing:
    //   size_t erase_if(multiset&, PREDICATE);
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


template <class KEY, class COMP, class ALLOC>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, COMP, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of `bsltf::StdStatefulAllocator` that are not
    // under test in this test case to `false`.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS> StdAlloc;

    typedef bsl::multiset<KEY, COMP, StdAlloc>  Obj;

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

        const Obj W(IVALUES.begin(), IVALUES.end(), COMP(), scratch);
                                                                     // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                IVALUES.resetIterators();

                Obj mY(IVALUES.begin(), IVALUES.end(), COMP(), mas);
                const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                Obj mX(JVALUES.begin(), JVALUES.end(), COMP(), mat);
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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::
                             testCase9_propagate_on_container_copy_assignment()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR: ALLOCATOR PROPAGATION
    //
    // Concerns:
    // 1. If the `propagate_on_container_copy_assignment` trait is `false`, the
    //    allocator used by the target object remains unchanged (i.e., the
    //    source object's allocator is *not* propagated).
    //
    // 2. If the `propagate_on_container_copy_assignment` trait is `true`, the
    //    allocator used by the target object is updated to be a copy of that
    //    used by the source object (i.e., the source object's allocator *is*
    //    propagated).
    //
    // 3. The allocator used by the source object remains unchanged whether or
    //   not it is propagated to the target object.
    //
    // 4. If the allocator is propagated from the source object to the target
    //    object, all memory allocated from the target object's original
    //    allocator is released.
    //
    // 5. The effect of the `propagate_on_container_copy_assignment` trait is
    //    independent of the other three allocator propagation traits.
    //
    // Plan:
    // 1. Specify a set S of object values with varied differences, ordered by
    //    increasing length, to be used in the following tests.
    //
    // 2. Create two `bsltf::StdStatefulAllocator` objects with their
    //    `propagate_on_container_copy_assignment` property configured to
    //    `false`.  In two successive iterations of P-3, first configure the
    //    three properties not under test to be `false`, then configure them
    //    all to be `true`.
    //
    // 3. For each value `(x, y)` in the cross product S x S:  (C-1)
    //
    //   1. Initialize an object `X` from `x` using one of the allocators from
    //      P-2.
    //
    //   2. Initialize two objects from `y`, a control object `W` using a
    //      scratch allocator and an object `Y` using the other allocator from
    //      P-2.
    //
    //   3. Copy-assign `Y` to `X` and use `operator==` to verify that both
    //      `X` and `Y` subsequently have the same value as `W`.
    //
    //   4. Use the `get_allocator` method to verify that the allocator of `Y`
    //      is *not* propagated to `X` and that the allocator used by `Y`
    //      remains unchanged.  (C-1)
    //
    // 4. Repeat P-2..3 except that this time configure the allocator property
    //    under test to `true` and verify that the allocator of `Y` *is*
    //    propagated to `X`.  Also verify that all memory is released to the
    //    allocator that was in use by `X` prior to the assignment.  (C-2..5)
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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    // 1. The assignment operator can change the value of any modifiable target
    //    object to that of any source object.
    //
    // 2. If allocator propagation is not enabled for copy assignment, the
    //    allocator address held by the target object is unchanged.
    //
    // 3. If allocator propagation is not enabled for copy assignment, any
    //    memory allocation is from the target object's allocator.
    //
    // 4. The signature and return type are standard.
    //
    // 5. The reference returned is to the target object (i.e., `*this`).
    //
    // 6. The value of the source object is not modified.
    //
    // 7. The allocator address held by the source object is unchanged.
    //
    // 8. QoI: Assigning a source object having the default-constructed value
    //    allocates no memory.
    //
    // 9. Any memory allocation is exception neutral.
    //
    // 10. Assigning an object to itself behaves as expected (alias-safety).
    //
    // 11. Every object releases any allocated memory at destruction.
    //
    // 12. If allocator propagation is enabled for copy assignment,
    //    any memory allocation is from the source object's
    //    allocator.
    //
    // 13. If allocator propagation is enabled for copy assignment, the
    //    allocator address held by the target object is changed to that of the
    //    source.
    //
    // 14. If allocator propagation is enabled for copy assignment, any memory
    //    allocation is from the original target allocator will be released
    //    after copy assignment.
    //
    // Plan:
    // 1. Use the address of `operator=` to initialize a member-function
    //    pointer having the appropriate signature and return type for the
    //    copy-assignment operator defined in this component.  (C-4)
    //
    // 2. Create a `bslma::TestAllocator` object, and install it as the default
    //    allocator (note that a ubiquitous test allocator is already installed
    //    as the global allocator).
    //
    // 3. Using the table-driven technique:
    //
    //   1. Specify a set of (unique) valid object values.
    //
    // 4. For each row `R1` (representing a distinct object value, `V`) in the
    //    table described in P-3: (C-1..2, 5..8, 11)
    //
    //   1. Use the value constructor and a "scratch" allocator to create two
    //      `const` `Obj`, `Z` and `ZZ`, each having the value `V`.
    //
    //   2. Execute an inner loop that iterates over each row `R2`
    //      (representing a distinct object value, `W`) in the table described
    //      in P-3:
    //
    //   3. For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //
    //     1. Create a `bslma::TestAllocator` object, `oa`.
    //
    //     2. Use the value constructor and `oa` to create a modifiable `Obj`,
    //        `mX`, having the value `W`.
    //
    //     3. Assign `mX` from `Z` in the presence of injected exceptions
    //        (using the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros).
    //
    //     4. Verify that the address of the return value is the same as that
    //        of `mX`.  (C-5)
    //
    //     5. Use the equality-comparison operator to verify that: (C-1, 6)
    //
    //       1. The target object, `mX`, now has the same value as that of `Z`.
    //          (C-1)
    //
    //       2. `Z` still has the same value as that of `ZZ`.  (C-6)
    //
    //     6. Use the `allocator` accessor of both `mX` and `Z` to verify that
    //        the respective allocator addresses held by the target and source
    //        objects are unchanged.  (C-2, 7)
    //
    //     7. Use the appropriate test allocators to verify that: (C-8, 11)
    //
    //       1. For an object that (a) is initialized with a value that did NOT
    //          require memory allocation, and (b) is then assigned a value
    //          that DID require memory allocation, the target object DOES
    //          allocate memory from its object allocator only (irrespective of
    //          the specific number of allocations or the total amount of
    //          memory allocated); also cross check with what is expected for
    //          `mX` and `Z`.
    //
    //       2. An object that is assigned a value that did NOT require memory
    //          allocation, does NOT allocate memory from its object allocator;
    //          also cross check with what is expected for `Z`.
    //
    //       3. No additional memory is allocated by the source object.  (C-8)
    //
    //       4. All object memory is released when the object is destroyed.
    //          (C-11)
    //
    // 5. Repeat steps similar to those described in P-4 except that, this
    //    time, there is no inner loop (as in P-4.2); instead, the source
    //    object, `Z`, is a reference to the target object, `mX`, and both `mX`
    //    and `ZZ` are initialized to have the value `V`.  For each row
    //    (representing a distinct object value, `V`) in the table described in
    //    P-3: (C-9)
    //
    //   1. Create a `bslma::TestAllocator` object, `oa`.
    //
    //   2. Use the value constructor and `oa` to create a modifiable `Obj`
    //      `mX`; also use the value constructor and a distinct "scratch"
    //      allocator to create a `const` `Obj` `ZZ`.
    //
    //   3. Let `Z` be a reference providing only `const` access to `mX`.
    //
    //   4. Assign `mX` from `Z` in the presence of injected exceptions (using
    //      the `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros).  (C-9)
    //
    //   5. Verify that the address of the return value is the same as that of
    //      `mX`.
    //
    //   6. Use the equality-comparison operator to verify that the target
    //      object, `mX`, still has the same value as that of `ZZ`.
    //
    //   7. Use the `allocator` accessor of `mX` to verify that it is still the
    //      object allocator.
    //
    //   8. Use the appropriate test allocators to verify that:
    //
    //     1. Any memory that is allocated is from the object allocator.
    //
    //     2. No additional (e.g., temporary) object memory is allocated when
    //        assigning an object value that did NOT initially require
    //        allocated memory.
    //
    //     3. All object memory is released when the object is destroyed.
    //
    // 6. Use the test allocator from P-2 to verify that no memory is ever
    //    allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   multiset& operator=(const multiset& rhs);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

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
            KeyAllocator         xscratch(&scratch);

            Obj mZ(xscratch);  const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(xscratch); const Obj& ZZ = gg(&mZZ, SPEC1);

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
                KeyAllocator         xoa(&oa);

                {
                    Obj mX(xoa); const Obj& X = gg(&mX,  SPEC2);

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
            KeyAllocator         xoa(&oa);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(xoa);       const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(xscratch); const Obj& ZZ = gg(&mZZ, SPEC1);

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

template <class KEY, class COMP, class ALLOC>
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
TestDriver<KEY, COMP, ALLOC>::testCase30a_RunTest(Obj *target)
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

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
        ASSERTV("Allocator in test case 30 is not a test allocator!", false);

        return;                                                       // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target; const Obj& X = mX;

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
            ASSERTV("Invalid # of args!", false);
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

template <class KEY, class COMP, class ALLOC>
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
typename TestDriver<KEY, COMP, ALLOC>::Iter
TestDriver<KEY, COMP, ALLOC>::testCase31a_RunTest(Obj *target,
                                                  CIter hint)
{
    const bool hintWasEnd = (hint == target->end());
    const int  TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                            bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

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
        ASSERTV("Allocator in test case 31 is not a test allocator!", false);

        return hint;                                                  // RETURN
    }

    bslma::TestAllocator& oa = *testAlloc;
    Obj& mX = *target; const Obj& X = mX;

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
            ASSERTV("Invalid # of args!", false);
          } break;
        }
        proctor.release();

        // If `hint` was the end iterator, it was invalidated by the insertion
        // (and we should not dereference it, even if it wasn't invalidated).
        ASSERT(X.end() != result);
        ASSERTV(hintWasEnd, hintWasEnd || &(*result) != &(*hint));

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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase33()
{
    // ------------------------------------------------------------------------
    // `noexcept` SPECIFICATION
    //
    // Concerns:
    // 1. The `noexcept` specification has been applied to all class interfaces
    //    required by the standard.
    //
    // Plan:
    // 1. Apply the uniary `noexcept` operator to expressions that mimic those
    //    appearing in the standard and confirm that calculated boolean value
    //    matches the expected value.
    //
    // 2. Since the `noexcept` specification does not vary with the `TYPE`
    //    of the container, we need test for just one general type and any
    //    `TYPE` specializations.
    //
    // Testing:
    //   CONCERN: Methods qualifed `noexcept` in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        P(bsls::NameOf<Obj>())
        P(bsls::NameOf<KEY>())
        P(bsls::NameOf<COMP>())
        P(bsls::NameOf<ALLOC>())
    }

    // N4594: page 875: 23.4.7: Class template `multiset` [multiset]

    // page 876
    // ```
    //  // 23.4.7.2, construct/copy/destroy:
    //  multiset& operator=(multiset&& x)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_move_assignable<Compare>::value);
    //  allocator_type get_allocator() const noexcept;
    // ```

    {
        Obj mX, mY;    const Obj& X = mX;    (void) X;    (void) mY;

        bool expected = false;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        expected = bsl::allocator_traits<ALLOC> ::is_always_equal::value
                && std::is_nothrow_move_assignable<COMP>::value;
#endif
        ASSERT(expected
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX =
                                             bslmf::MovableRefUtil::move(mY)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.get_allocator()));
    }

    // page 877
    // ```
    //  // iterators:
    //  iterator begin() noexcept;
    //  const_iterator begin() const noexcept;
    //  iterator end() noexcept;
    //  const_iterator end() const noexcept;
    //  reverse_iterator rbegin() noexcept;
    //  const_reverse_iterator rbegin() const noexcept;
    //  reverse_iterator rend() noexcept;
    //  const_reverse_iterator rend() const noexcept;
    //  const_iterator cbegin() const noexcept;
    //  const_iterator cend() const noexcept;
    //  const_reverse_iterator crbegin() const noexcept;
    //  const_reverse_iterator crend() const noexcept;
    // ```

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
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rbegin()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(mX.rend()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.rend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.cend()));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crbegin()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR( X.crend()));
    }

    // page 877
    // ```
    //  // capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    // ```

    {
        Obj mX; const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_size()));
    }

    // page 877
    // ```
    //  // modifiers:
    //  void swap(multiset&)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_swappable_v<Compare>);
    //  void clear() noexcept;
    // ```

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        bool expected =  false;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        expected = bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                   bsl::is_nothrow_swappable<COMP>::value;
#endif

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 878
    // ```
    //  // 23.4.7.3, specialized algorithms:
    //  template <class Key, class Compare, class Allocator>
    //  void swap(multiset<Key, Compare, Allocator>& x,
    //            multiset<Key, Compare, Allocator>& y)
    //      noexcept(noexcept(x.swap(y)));
    // ```

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        bool expected = BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y));

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(x, y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }
}



template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase32()
{
    // ------------------------------------------------------------------------
    // TESTING FUNCTIONS TAKING INITIALIZER LISTS
    //
    // Concerns:
    //   The three functions that take an initializer lists (a constructor, an
    //   assignment operator, and the `insert` function) simply forward to
    //   another already tested function.  We are interested here only in
    //   ensuring that the forwarding is working -- not retesting already
    //   tested functionality.
    //
    // Plan:
    //   TBD.
    //
    // Testing:
    //   multiset(initializer_list<value_type>, const C& comp, const A& alloc);
    //   multiset(initializer_list<value_type>, const A& alloc);
    //   multiset& operator=(initializer_list<value_type>);
    //   iterator insert(initializer_list<value_type>);
    // -----------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    const TestValues V;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    if (verbose)
        printf("\nTesting constructor with initializer lists\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    {
        const struct {
            int                        d_line;   // source line number
            std::initializer_list<KEY> d_list;   // source list
            const char                *d_result; // expected result
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

        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);
        const COMP MYCOMP(7);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        KeyAllocator         xoa(&oa);

        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mY(xscratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, MYCOMP, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa    == X.get_allocator());
                ASSERT(MYCOMP == X.key_comp());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || 0 == ti) == (1 == numPasses));

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    {
        const struct {
            int                        d_line;   // source line number
            std::initializer_list<KEY> d_list;   // source list
            const char                *d_result; // expected result
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

        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        KeyAllocator         xoa(&oa);

        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mY(xscratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, xoa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa    == X.get_allocator());
                ASSERT(COMP() == X.key_comp());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || 0 == ti) == (1 == numPasses));

            ASSERT(&scratch == Y.get_allocator());
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting `operator=` with initializer lists\n");

    {
        const struct {
            int                        d_line;   // source line number
            const char                *d_spec;   // target string
            std::initializer_list<KEY> d_list;   // source list
            const char                *d_result; // expected result
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

        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        KeyAllocator         xoa(&oa);

        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mY(xscratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

            Obj mX(xoa); const Obj& X = gg(&mX, DATA[ti].d_spec);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj *mR = &(mX = DATA[ti].d_list);
                ASSERTV(mR, &mX, mR == &mX);
                ASSERTV(Y,  X,   Y  == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(dam.isTotalSame());
    }

    if (verbose)
        printf("\nTesting `insert` with initializer lists\n");
    {
        const struct {
            int                        d_line;   // source line number
            const char                *d_spec;   // target string
            std::initializer_list<KEY> d_list;   // source list
            const char                *d_result; // expected result
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

        const int NUM_SPECS = static_cast<int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        KeyAllocator         xoa(&oa);

        bslma::TestAllocatorMonitor dam(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mY(xscratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

            Obj mX(xoa); const Obj& X = gg(&mX, DATA[ti].d_spec);

            mX.insert(DATA[ti].d_list);

            ASSERTV(X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }
#endif  // defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase31a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    // 1. `emplace_hint` correctly forwards arguments to the constructor of the
    //    value type, up to 10 arguments, the max number of arguments provided
    //    for C++03 compatibility.  Note that only the forwarding of arguments
    //    is tested in this function; all other functionality is tested in
    //   `testCase31`.
    //
    // 2. `emplace_hint` is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    // 1. This test makes material use of template method `testCase31a_RunTest`
    //    with first integer template parameter indicating the number of
    //    arguments to use, the next 10 integer template parameters indicating
    //    '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //    the number of arguments), and taking as arguments a pointer to a
    //    modifiable container and a hint.
    //   1. Create 10 argument values with their own argument-specific
    //      allocator.
    //
    //   2. Based on (first) template parameter indicating the number of args
    //      to pass in, call `emplace_hint` with the hint passed in as an
    //      argument and the corresponding constructor argument values,
    //      performing an explicit move of the argument if so indicated by
    //      the template parameter corresponding to the argument, all of this
    //      in the presence of injected exceptions.
    //
    //   3. Verify that the argument values were passed correctly.
    //
    //   4. Verify that the allocator was forwarded correctly.
    //
    //   5. Verify that the move-state for each argument is as expected.
    //
    //   6. If the object did not contain the emplaced value, verify it now
    //      exists.  Otherwise, verify the return value is as expected.
    //
    //   7. Return the iterator returned by the call to `emplace_hint`.
    //
    // 2. Create a container with it's own object-specific allocator.
    //
    // 3. Call `testCase31a_RunTest` in various configurations:
    //   1. For 1..10 arguments, call with the move flag set to `1` and then
    //      with the move flag set to `0`.
    //
    //   2. For 1, 2, 3, and 10 arguments, call with move flags set to `0`,
    //      `1`, and each move flag set independently.
    //
    // Testing:
    // iterator emplace_hint(const_iterator hint, Args&&... args);
    // ------------------------------------------------------------------------

    Iter hint;
#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace_hint 1..10 args, move=1"
                        "\n---------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, hint);
        hint = testCase31a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace_hint 1..10 args, move=0"
                        "\n---------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace_hint with 0 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
    }

    if (verbose) printf("\nTesting emplace_hint with 1 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace_hint with 2 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace_hint with 3 args"
                        "\n--------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, hint);
    }

    if (verbose) printf("\nTesting emplace_hint with 10 args"
                        "\n---------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end());
        hint = testCase31a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, hint);
        hint = testCase31a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint);
    }
#else
    if (verbose) printf("\nTesting emplace_hint 1..10 args, move=0"
                        "\n---------------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa); const Obj& X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end());
        hint = testCase31a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint);
        hint = testCase31a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint);
    }
#endif
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase31()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE WITH HINT
    //
    // Concerns:
    // 1. `emplace_hint` returns an iterator referring to the newly inserted
    //    element.
    //
    // 2. A new element is added to the container at the end of the range
    //    containing equivalent elements.
    //
    // 3. Inserting with the correct hint places the new element right before
    //    the hint.
    //
    // 4. Inserting with the correct hint requires no more than 2 comparisons.
    //
    // 5. Incorrect hint will be ignored and `insert` will proceed as if the
    //    hint is not supplied.
    //
    // 6. Internal memory management system is hooked up properly
    //    so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 7. Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    // 1. We will use `value` as the single argument to the `emplace_hint`
    //    function and will test proper forwarding of constructor arguments in
    //    test `testCase31a`.
    //
    // 2. For insertion we will create objects of varying sizes and capacities
    //    containing default values, and insert a `value`.
    //
    //   1. For each set of values, set hint to be `lower_bound`, `begin`,
    //      `begin + 1`, `end - 1`, `end`
    //
    //     1. For each value in the set, `insert` the value with hint.
    //
    //       1. Compute the number of allocations and verify it is as
    //          expected.
    //
    //       2. Verify the return value and the resulting data in the container
    //          is as expected.
    //
    //       3. Verify the new element is inserted right before the hint if
    //          the hint is valid.  (C-3)
    //
    //       4. Verify the number of comparisons is no more than 2 if the hint
    //          is valid.  (C-4)
    //
    //       5. Verify all allocations are from the object's allocator.  (C-6)
    //
    // 3. Repeat P-2 under the presence of exception  (C-7)
    //
    // Testing:
    //   iterator emplace_hint(const_iterator position, Args&&... args);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
        const char *d_allocs;  // expected pool resizes
    } DATA[] = {
        //line  spec           isUnique       poolAlloc
        //----  -------------- -------------- -------------

        { L_,   "A",           "Y",           "+"           },
        { L_,   "AAA",         "YNN",         "++-"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY",    "++-+---+"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY", "++-+---+---" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN",  "++-+---+--"  }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting `emplace_hint`.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const char *const ALLOCS = DATA[ti].d_allocs;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int)strlen(UNIQUE));
            ASSERTV(LINE, LENGTH == (int)strlen(ALLOCS));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.lower_bound(VALUES[tj]);
                      } break;
                      case 'b': {
                          hint = X.begin();
                      } break;
                      case 'c': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'd': {
                          hint = X.end();
                          if (hint != X.begin()) {
                              --hint;
                          }
                      } break;
                      case 'e': {
                          hint = X.end();
                      } break;
                      default: {
                          ASSERTV("Unexpected configuration", false);
                      }
                    }

                    size_t EXP_COMP = X.key_comp().count();
                    if (CONFIG == 'a') {
                        if (hint != X.begin()) {
                            ++EXP_COMP;
                        }
                        if (hint != X.end()) {
                            ++EXP_COMP;
                        }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT = mX.emplace_hint(hint, VALUES[tj]);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    if (CONFIG == 'a') {
                        ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                EXP_COMP == X.key_comp().count());

                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }

                    if ('+' == ALLOCS[tj]) {
                        ASSERTV(LINE, tj, AA, BB,
                                BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B,
                                B + 1 + TYPE_ALLOC == A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB,
                                BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                    }
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);

                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
            }
        }
    }
    if (verbose) printf("\nTesting `emplace_hint` with exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int)strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.lower_bound(VALUES[tj]);
                      } break;
                      case 'b': {
                          hint = X.begin();
                      } break;
                      case 'c': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'd': {
                          hint = X.end();
                          if (hint != X.begin()) {
                              --hint;
                          }
                      } break;
                      case 'e': {
                          hint = X.end();
                      } break;
                      default: {
                          ASSERTV("Unexpected configuration", false);
                      }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);

                    Iter RESULT;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                        RESULT = mX.emplace_hint(hint, VALUES[tj]);
                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
                }
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase30a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    // 1. `emplace` correctly forwards arguments to the constructor of the
    //    value type, up to 10 arguments, the max number of arguments provided
    //    for C++03 compatibility.  Note that only the forwarding of arguments
    //    is tested in this function; all other functionality is tested in
    //   `testCase30`.
    //
    // 2. `emplace` is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    // 1. This test makes material use of template method `testCase30a_RunTest`
    //    with first integer template parameter indicating the number of
    //    arguments to use, the next 10 integer template parameters indicating
    //    '0' for copy, '1' for move, and '2' for not-applicable (i.e., beyond
    //    the number of arguments), and taking as the only argument a pointer
    //    to a modifiable container.
    //   1. Create 10 argument values with their own argument-specific
    //      allocator.
    //
    //   2. Based on (first) template parameter indicating the number of args
    //      to pass in, call `emplace` with the corresponding argument values,
    //      performing an explicit move of the argument if so indicated by
    //      the template parameter corresponding to the argument, all in the
    //      presence of injected exceptions.
    //
    //   3. Verify that the argument values were passed correctly.
    //
    //   4. Verify that the allocator was forwarded correctly.
    //
    //   5. Verify that the move-state for each argument is as expected.
    //
    //   6. If the object did not contain the emplaced value, verify it now
    //      exists.  Otherwise, verify the return value is as expected.
    //
    // 2. Create a container with it's own object-specific allocator.
    //
    // 3. Call `testCase31a_RunTest` in various configurations:
    //   1. For 1..10 arguments, call with the move flag set to `1` and then
    //      with the move flag set to `0`.
    //
    //   2. For 1, 2, 3, and 10 arguments, call with move flags set to `0`,
    //      `1`, and each move flag set independently.
    //
    //
    // Testing:
    // iterator emplace(Args&&... args);
    // ------------------------------------------------------------------------

#ifndef BSL_DO_NOT_TEST_MOVE_FORWARDING
    if (verbose) printf("\nTesting emplace 1..10 args, move=1"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX);
        testCase30a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX);
        testCase30a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX);
        testCase30a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX);
        testCase30a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }

    if (verbose) printf("\nTesting emplace 1..10 args, move=0"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase30a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase30a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase30a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 0 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 1 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 2 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 3 args"
                        "\n---------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX);
    }

    if (verbose) printf("\nTesting emplace with 10 args"
                        "\n----------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
        testCase30a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX);
        testCase30a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX);
        testCase30a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX);
        testCase30a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX);
        testCase30a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX);
        testCase30a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX);
    }
#else
    if (verbose) printf("\nTesting emplace 1..10 args, move=0"
                        "\n----------------------------------\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj mX(&oa);

        testCase30a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX);
        testCase30a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX);
        testCase30a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX);
        testCase30a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX);
        testCase30a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX);
    }
#endif
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase30()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    // 1. The iterator returned refers to the newly inserted element.
    //
    // 2. A new element is added to the container and the order of the
    //    container remains correct.
    //
    // 3. Internal memory management system is hooked up properly
    //    so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 4. Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    // 1. We will use `value` as the single argument to the `emplace` function
    //    and will test proper forwarding of constructor arguments in test
    //    `testCase30a`.
    //
    // 2. For emplace, we will create objects of varying sizes and capacities
    //    containing default values, and insert a `value`.
    //
    //   1. Compute the number of allocations and verify it is as expected.
    //
    //   2. If the object did not contain `value`, verify it now exist.
    //      Otherwise, verify the return value is as expected.  (C-1..2)
    //
    //   3. Verify all allocations are from the object's allocator.  (C-3)
    //
    // 3. Repeat P-2 under the presence of exception  (C-4)
    //
    // Testing:
    //   iterator emplace(Args&&... arguments);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
        const char *d_allocs;  // expected pool resizes
    } DATA[] = {
        //line  spec           isUnique       poolAlloc
        //----  -------------- -------------- -------------

        { L_,   "A",           "Y",           "+"           },
        { L_,   "AAA",         "YNN",         "++-"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY",    "++-+---+"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY", "++-+---+---" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN",  "++-+---+--"  }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting `emplace` without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const char *const ALLOCS = DATA[ti].d_allocs;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int)strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                Iter RESULT = mX.emplace(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if ('+' == ALLOCS[tj]) {
                    ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                    ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                    ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                }
                ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                TestValues exp(EXPECTED);
                ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }
    }
    if (verbose) printf("\nTesting `emplace` with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int)strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xoa);

                    Iter RESULT = mX.emplace(VALUES[tj]);

                    ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                TestValues exp(EXPECTED);
                ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT ON MOVABLE VALUES
    //
    // Concerns:
    // 1. `insert` returns an iterator referring to the newly inserted element.
    //
    // 2. A new element is added to the container at the end of the range
    //    containing equivalent elements.
    //
    // 3. Inserting with the correct hint places the new element right before
    //    the hint.
    //
    // 4. Inserting with the correct hint requires no more than 2 comparisons.
    //
    // 5. Incorrect hint will be ignored and `insert` will proceed as if the
    //    hint is not supplied.
    //
    // 6. The new element is move-inserted into the container.
    //
    // 7. Internal memory management system is hooked up properly
    //    so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 8. Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    // 1. For insertion we will create objects of varying sizes and capacities
    //    containing default values, and insert a `value`.
    //
    //   1. For each set of values, set hint to be `lower_bound`, `begin`,
    //      `begin + 1`, `end - 1`, `end`
    //
    //     1. For each value in the set, `insert` the value with hint.
    //
    //       1. Compute the number of allocations and verify it is as
    //          expected.
    //
    //       2. Verify the return value and the resulting data in the container
    //          is as expected.
    //
    //       3. Verify the new element is inserted right before the hint if
    //          the hint is valid.  (C-3)
    //
    //       4. Verify the number of comparisons is no more than 2 if the hint
    //          is valid.  (C-4)
    //
    //       5. Ensure that the move constructor was called on the value type
    //          (if that type has a mechanism to detect such) where the value
    //          has the same allocator as that of the container and a different
    //          allocator than that of the container.  (C-5)
    //
    //       5. Verify all allocations are from the object's allocator.  (C-7)
    //
    // 2. Repeat P-1 under the presence of exception  (C-8)
    //
    // Testing:
    //   iterator insert(const_iterator position, value_type&& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
    } DATA[] = {
        //line  spec
        //----  --------------

        { L_,   "A"            },
        { L_,   "AAA"          },
        { L_,   "ABCDEFGH"     },
        { L_,   "ABCDEABCDEF"  },
        { L_,   "EEDDCCBBAA"   },
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting `insert` with hint.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {
                    const char CONFIG2 = cfg2;

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    Obj mX(xoa);  const Obj &X = mX;

                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
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
                        ASSERTV(CONFIG, "Bad allocator config.", false);
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    KeyAllocator          xsa(&sa);

                    for (int tj = 0; tj < LENGTH; ++tj) {
                        const size_t SIZE = X.size();

                        if (veryVeryVerbose) { P(SIZE); }

                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }

                        CIter hint;
                        switch(CONFIG) {
                          case 'a': {
                              hint = X.lower_bound(VALUES[tj]);
                          } break;
                          case 'b': {
                              hint = X.begin();
                          } break;
                          case 'c': {
                              hint = X.begin();
                              if (hint != X.end()) {
                                  ++hint;
                              }
                          } break;
                          case 'd': {
                              hint = X.end();
                              if (hint != X.begin()) {
                                  --hint;
                              }
                          } break;
                          case 'e': {
                              hint = X.end();
                          } break;
                          default: {
                              ASSERTV("Unexpected configuration", false);
                          }
                        }

                        size_t EXP_COMP = X.key_comp().count();
                        if (CONFIG == 'a') {
                            if (hint != X.begin()) {
                                ++EXP_COMP;
                            }
                            if (hint != X.end()) {
                                ++EXP_COMP;
                            }
                        }

                        const bsls::Types::Int64 BB = oa.numBlocksTotal();
                        const bsls::Types::Int64 B  = oa.numBlocksInUse();

                        bsls::ObjectBuffer<ValueType> buffer;
                        ValueType *valptr = buffer.address();
                        TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));

                        Iter RESULT = mX.insert(hint, MoveUtil::move(*valptr));

                        MoveState::Enum mState =
                                       TstFacility::getMovedFromState(*valptr);

                        bslma::DestructionUtil::destroy(valptr);

                        const bsls::Types::Int64 AA = oa.numBlocksTotal();
                        const bsls::Types::Int64 A  = oa.numBlocksInUse();

                        ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                        if (CONFIG == 'a') {
                            ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                    EXP_COMP == X.key_comp().count());

                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }

                        bsls::Types::Int64 expAA = BB + TYPE_ALLOC; // new item
                        bsls::Types::Int64  expA =  B + TYPE_ALLOC; // new item

                        const MoveState::Enum expMove =
                                         k_KEY_IS_MOVE_ENABLED
                                         ? (k_KEY_IS_WELL_BEHAVED && &sa != &oa
                                            ? MoveState::e_NOT_MOVED
                                            : MoveState::e_MOVED)
                                         : MoveState::e_UNKNOWN;
                        ASSERTV(expMove, mState, NameOf<KEY>(),
                                                            expMove == mState);

                        if (expectToAllocate(SIZE + 1)) {
                            expA =  expA + 1;      // resize on pool
                            expAA = expAA + 1;     // resize on pool
                        }
                        if (&sa == &oa && MoveState::e_MOVED != mState) {
                            expAA += TYPE_ALLOC;   // temporary (not moved)
                        }
                        ASSERTV(LINE, CONFIG2, tj, AA, expAA, AA == expAA);
                        ASSERTV(LINE, CONFIG2, tj,  A,  expA,  A ==  expA);
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                        TestValues exp(EXPECTED);

                        ASSERTV(LINE, tj, 0 == verifyContainer(X,
                                                               exp,
                                                               SIZE + 1));
                    }
                }
            }
        }
    }
    if (verbose) printf("\nTesting `insert` with exception.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);
                    Obj mX(xoa);  const Obj &X = mX;

                    bslma::TestAllocator za("different", veryVeryVeryVerbose);
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
                        ASSERTV(CONFIG, "Bad allocator config.", false);
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    KeyAllocator         xsa(&sa);

                    for (int tj = 0; tj < LENGTH; ++tj) {
                        const size_t SIZE    = X.size();

                        if (veryVeryVerbose) { P(SIZE); }

                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }

                        CIter hint;
                        switch(CONFIG) {
                          case 'a': {
                              hint = X.lower_bound(VALUES[tj]);
                          } break;
                          case 'b': {
                              hint = X.begin();
                          } break;
                          case 'c': {
                              hint = X.begin();
                              if (hint != X.end()) {
                                  ++hint;
                              }
                          } break;
                          case 'd': {
                              hint = X.end();
                              if (hint != X.begin()) {
                                  --hint;
                              }
                          } break;
                          case 'e': {
                              hint = X.end();
                          } break;
                          default: {
                              ASSERTV("Unexpected configuration", false);
                          }
                        }

                        bslma::TestAllocator scratch("scratch",
                                                     veryVeryVeryVerbose);
                        KeyAllocator         xscratch(&scratch);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            Obj mZ(xscratch); const Obj& Z = mZ;

                            for (int tk = 0; tk < tj; ++tk) {
                                primaryManipulator(
                                        &mZ,
                                        TstFacility::getIdentifier(VALUES[tk]),
                                        xscratch);
                            }
                            ASSERTV(Z, X, Z == X);

                            ExceptionProctor<Obj, ALLOC> proctor(&X, L_,
                                                          MoveUtil::move(mZ));

                            bsls::ObjectBuffer<ValueType> buffer;
                            ValueType *valptr = buffer.address();
                            TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        xsa);
                            bslma::DestructorGuard<ValueType> guard(valptr);

                            Iter RESULT =
                                      mX.insert(hint, MoveUtil::move(*valptr));

                            proctor.release();

                            ASSERTV(LINE, CONFIG, tj, SIZE,
                                    VALUES[tj] == *RESULT);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj, 0 == verifyContainer(X,
                                                               exp,
                                                               SIZE + 1));
                    }
                }
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase28()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION ON MOVABLE VALUES
    //
    // Concerns:
    // 1. The iterator returned refers to the newly inserted element.
    //
    // 2. The new element is move-inserted into the container.
    //
    // 3. Internal memory management system is hooked up properly
    //    so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 4. Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    // 1. For insertion we will create objects of varying sizes and capacities
    //    containing default values, and insert a `value`.
    //
    //   1. Compute the number of allocations and verify it is as expected.
    //
    //   2. If the object did not contain `value`, verify it now exist.
    //      Otherwise, verify the return value is as expected.  (C-1..2)
    //
    //   3. Ensure that the move constructor was called on the value type (if
    //      that type has a mechanism to detect such) where the value has the
    //      same allocator as that of the container and a different allocator
    //      than that of the container.  (C-2)
    //
    //   4. Verify all allocations are from the object's allocator.  (C-3)
    //
    // 2. Repeat P-1 under the presence of exception  (C-4)
    //
    // Testing:
    //   iterator insert(value_type&& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

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
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting `insert` without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                const char CONFIG = cfg;

                bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj &X = mX;

                bslma::TestAllocator za("different", veryVeryVeryVerbose);
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
                    ASSERTV(CONFIG, "Bad allocator config.", false);
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVeryVerbose) { P_(IS_UNIQ) P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    bsls::ObjectBuffer<ValueType> buffer;
                    ValueType *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));

                    Iter RESULT = mX.insert(MoveUtil::move(*valptr));

                    MoveState::Enum mState =
                                       TstFacility::getMovedFromState(*valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, CONFIG, tj, SIZE, VALUES[tj] == *RESULT);

                    Iter AFTER = RESULT; ++AFTER;
                    ASSERTV(LINE, tj, AFTER  == X.upper_bound(VALUES[tj]));
                    if (IS_UNIQ) {
                        ASSERTV(LINE, tj, RESULT == X.lower_bound(VALUES[tj]));
                        if (RESULT != X.begin()) {
                            Iter BEFORE = RESULT;  --BEFORE;
                            ASSERTV(LINE, tj, VALUES[tj] != *BEFORE);
                        }
                    }
                    else{
                        ASSERTV(LINE, tj, X.begin() != RESULT);

                        Iter BEFORE = RESULT;  --BEFORE;
                        ASSERTV(LINE, tj, VALUES[tj] == *BEFORE);
                    }

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    bsls::Types::Int64 expAA = BB + TYPE_ALLOC; // new item
                    bsls::Types::Int64  expA =  B + TYPE_ALLOC; // new item

                    const MoveState::Enum expMove =
                                         k_KEY_IS_MOVE_ENABLED
                                         ? (k_KEY_IS_WELL_BEHAVED && &sa != &oa
                                            ? MoveState::e_NOT_MOVED
                                            : MoveState::e_MOVED)
                                         : MoveState::e_UNKNOWN;
                    ASSERTV(expMove, mState, NameOf<KEY>(), expMove == mState);

                    if (((SIZE + 1) & SIZE) == 0) {
                        expA =  expA + 1;       // resize on pool
                        expAA = expAA + 1;      // resize on pool
                    }
                    if (&sa == &oa && MoveState::e_MOVED != mState) {
                        expAA += TYPE_ALLOC;    // temporary (not moved)
                    }

                    ASSERTV(LINE, CONFIG, tj, AA, expAA, expAA == AA);
                    ASSERTV(LINE, CONFIG, tj,  A,  expA,  expA ==  A);
                    ASSERTV(LINE, CONFIG, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, CONFIG, tj, 0 == verifyContainer(X,
                                                                   exp,
                                                                   SIZE + 1));
                }
            }
        }
    }

    if (verbose) printf("\nTesting `insert` with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj &X = mX;

                bslma::TestAllocator za("different", veryVeryVeryVerbose);
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
                    ASSERTV(CONFIG, "Bad allocator config.", false);
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const size_t SIZE    = X.size();

                    if (veryVeryVerbose) { P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(xscratch);   const Obj& Z = mZ;
                        for (int tk = 0; tk < tj; ++tk) {
                            primaryManipulator(
                                        &mZ,
                                        TstFacility::getIdentifier(VALUES[tk]),
                                        xscratch);
                        }
                        ASSERTV(Z, X, Z == X);

                        ExceptionProctor<Obj, ALLOC>
                                           proctor(&X, L_, MoveUtil::move(mZ));

                        bsls::ObjectBuffer<ValueType> buffer;
                        ValueType *valptr = buffer.address();
                        TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        xsa);
                        bslma::DestructorGuard<ValueType> guard(valptr);

                        Iter RESULT = mX.insert(MoveUtil::move(*valptr));

                        ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj, 0 == verifyContainer(X,
                                                           exp,
                                                           SIZE + 1));
                }
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase27_dispatch()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //  TBD: the test does not yet cover the case where allocator propagation
    //       is enabled for move assignment (hard-coded to `false`) -- i.e.,
    //       parts of C-5..6 are currently not addressed.
    //
    // 1. The signature and return type are standard.
    //
    // 2. The reference returned is to the target object (i.e., `*this`).
    //
    // 3. The move assignment operator can change the value of a modifiable
    //    target object to that of any source object.
    //
    // 4. The object has its internal memory management system hooked up
    //    properly so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 5. If allocator propagation is not enabled for move-assignment, the
    //    allocator address held by the target object is unchanged; otherwise,
    //    the allocator address held by the target object is changed to that of
    //    the source.
    //
    // 6. If allocator propagation is enabled for move-assignment, any memory
    //    allocation from the original target allocator is released after the
    //    operation has completed.
    //
    // 7. All elements in the target object are either move-assigned to or
    //    destroyed.
    //
    // 8. The source object is left in a valid state but unspecified state; the
    //    allocator address held by the original object is unchanged.
    //
    // 9. Subsequent changes to or destruction of the original object have no
    //    effect on the move-constructed object and vice-versa.
    //
    // 10. Assigning a source object having the default-constructed value
    //    allocates no memory; assigning a value to a target object in the
    //    default state does not allocate or deallocate any memory.
    //
    // 11. Every object releases any allocated memory at destruction.
    //
    // 12. Any memory allocation is exception neutral.
    //
    // 13. Assigning an object to itself behaves as expected (alias-safety).
    //
    // Plan:
    //
    // 1. Use the address of `operator=` to initialize a member-function
    //    pointer having the appropriate signature and return type for the
    //    copy-assignment operator defined in this component.             (C-1)
    //
    // 2. Iterate over a set of object values with substantial and varied
    //    differences, ordered by increasing length, and create for each a
    //    control object representing the source of the assignment, with its
    //    own scratch allocator.
    //
    // 3. Iterate again over the same set of object values and create a
    //    object representing the target of the assignment, with its own unique
    //    object allocator.
    //
    // 4. In a loop consisting of two iterations, create a source object (a
    //    copy of the control object in P-1) with 1) a different allocator than
    //    that of target and 2) the same allocator as that of the target,
    //
    // 5. Call the move-assignment operator in the presence of exceptions
    //    during memory allocations (using a `bslma::Allocator` and varying
    //    its allocation limit) and verify the following:                (C-12)
    //
    //   1. The address of the return value is the same as that of the target
    //      object.                                                       (C-2)
    //
    //   2. The object being assigned to has the same value as that of the
    //      source object before assignment (i.e., the control object).   (C-3)
    //
    //   3. CONTAINER SPECIFIC NOTE: Ensure that the comparator was assigned.
    //
    //   4. If the source and target objects use the same allocator, ensure
    //      that there is no net increase in memory use from the common
    //      allocator.  Also consider the following cases:
    //
    //     1. If the source object is empty, confirm that there are no bytes
    //        currently in use from the common allocator.                (C-10)
    //
    //     2. If the target object is empty, confirm that there was no memory
    //        change in memory usage.                                    (C-10)
    //
    //   5. If the source and target objects use different allocators, ensure
    //      that each element in the source object is move-inserted into the
    //      target object.                                                (C-7)
    //
    //   6. Ensure that the source, target, and control object continue to have
    //      the correct allocator and that all memory allocations come from the
    //      appropriate allocator.                                        (C-4)
    //
    //   7. Manipulate the source object (after assignment) to ensure that it
    //      is in a valid state, destroy it, and then manipulate the target
    //      object to ensure that it is in a valid state.                 (C-8)
    //
    //   8. Verify all memory is released when the source and target objects
    //      are destroyed.                                               (C-11)
    //
    // 6. Use a test allocator installed as the default allocator to verify
    //    that no memory is ever allocated from the default allocator.
    //
    // Testing:
    //   multiset& operator=(bslmf::MovableRef<multiset> rhs);
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                AllocatorTraits::propagate_on_container_move_assignment::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING MOVE ASSIGN `%s` OTHER:%c PROP:%c"
                                                                " ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        NameOf<KEY>().name(),
                        otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    const bool keyUsesDefaultAlloc = (bslma::UsesBslmaAllocator<KEY>::value &&
                                      e_STATEFUL == s_allocCategory);

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator doa("default",   veryVeryVeryVerbose);
    bslma::TestAllocator soa("scratch",   veryVeryVeryVerbose);
    bslma::TestAllocator ooa("object",    veryVeryVeryVerbose);
    bslma::TestAllocator zoa("other",     veryVeryVeryVerbose);
    bslma::TestAllocator foa("footprint", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&doa);

    ALLOC da(&doa);
    ALLOC sa(&soa);
    ALLOC oa(&ooa);
    ALLOC za(&zoa);

    // Check remaining properties of allocator to make sure they all match
    // `otherTraitsSet`.

    BSLMF_ASSERT(otherTraitsSet ==
                          AllocatorTraits::propagate_on_container_swap::value);
    ASSERT((otherTraitsSet ? sa : da) ==
                   AllocatorTraits::select_on_container_copy_construction(sa));

    {
        using namespace bsl;

        Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
        (void) operatorMAg;  // quash potential compiler warning
    }

    // Create first object
    if (veryVerbose) printf("Testing move assignment.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const char *const RESULT1 = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(RESULT1);

            if (ti && INDEX1 == DATA[ti-1].d_index) {
                continue;    // redundant, skip
            }

            const bsls::Types::Int64 BEFORE = soa.numBytesInUse();

            Obj  mZZ(sa); const Obj&  ZZ = gg(&mZZ, SPEC1);

            const bsls::Types::Int64 BIU    = soa.numBytesInUse() - BEFORE;

            if (veryVerbose) { T_ P_(SPEC1) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            if (0 == ti) {
                ASSERTV(SPEC1, Obj(sa), ZZ, Obj(sa) == ZZ);
            }

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const char *const RESULT2 = DATA[tj].d_results;
                const size_t      LENGTH2 = strlen(RESULT2);

                if (tj && INDEX2 == DATA[tj-1].d_index) {
                    continue;    // redundant, skip
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    Obj *objPtr = new (foa) Obj(oa);
                    Obj& mX = *objPtr; const Obj& X = gg(&mX, SPEC2);

                    Obj *srcPtr = 0;
                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (foa) Obj(za); gg(srcPtr, SPEC1);
                      } break;
                      case 'b': {
                        srcPtr = new (foa) Obj(oa); gg(srcPtr, SPEC1);
                      } break;
                      default: {
                        ASSERTV(CONFIG, "Bad allocator config.", false);
                      } return;                                       // RETURN
                    }

                    Obj& mZ = *srcPtr; const Obj& Z = mZ;
                    bslma::TestAllocator& roa   = 'a' == CONFIG ? zoa : ooa;
                    ALLOC& ra                   = 'a' == CONFIG ? za  : oa;
                    bslma::TestAllocator& dstOA = isPropagate ? roa : ooa;

                    if (veryVerbose) { T_ P_(SPEC1) P(Z) }
                    if (veryVerbose) { T_ P_(SPEC2) P(X) }

                    ASSERTV(SPEC1, SPEC2, Z, X, (Z == X) == (ti == tj));
                    const bool empty = 0 == ZZ.size();

                    typename Obj::const_pointer pointers[2] = { 0, 0 };
                    storeFirstNElemAddr(pointers, Z,
                            sizeof pointers / sizeof *pointers);

                    bslma::TestAllocatorMonitor oam(&ooa), zam(&zoa);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                    if (0 == LENGTH1) {
                        // assigned an empty multiset
                        ASSERTV(SPEC1, SPEC2, ooa.numBytesInUse(),
                                0 == ooa.numBytesInUse());
                    }

                    // CONTAINER SPECIFIC NOTE: For `multiset`, the original
                    // object is left in the default state even when the source
                    // and target objects use different allocators because
                    // move-insertion changes the value of the source key and
                    // violates the requirements for keys contained in the
                    // `multiset`.

                    if (isPropagate || &ra == &oa) {
                        ASSERTV(SPEC1, SPEC2, oam.isTotalSame());

                        // same allocator
                        if (0 == LENGTH2) {
                            // assigning to an empty multiset
                            ASSERTV(SPEC1, SPEC2, oam.isInUseSame());
                            ASSERTV(SPEC1, SPEC2, BIU, dstOA.numBytesInUse(),
                                    BIU == dstOA.numBytesInUse());
                        }

                        // 2. unchanged address of contained element(s)
                        ASSERT(0 == checkFirstNElemAddr(pointers,
                                                        X,
                                                        sizeof pointers /
                                                            sizeof *pointers));

                        // 3. original object left empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 4. nothing from the other allocator
                        ASSERTV(SPEC1, SPEC2, zam.isTotalSame());
                    }
                    else {
                        // 1. each element in original move-inserted
                        ASSERTV(SPEC1,
                                SPEC2,
                                X.end() == TstMoveUtil::findFirstNotMovedInto(
                                               X.begin(), X.end()));

                        // 2. CONTAINER SPECIFIC NOTE: original object left
                        // empty
                        ASSERTV(SPEC1, SPEC2, &ra == &oa, Z, 0 == Z.size());

                        // 3. additional memory checks
                        ASSERTV(SPEC1, SPEC2, &ra == &oa,
                                empty || oam.isTotalUp());
                        ASSERTV(SPEC1, SPEC2, 0 == zam.isInUseUp());
                    }

                    // Verify that `X`, `Z`, and `ZZ` have correct allocator.
                    ASSERTV(SPEC1, SPEC2, sa == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2, (isPropagate ? ra : oa) ==
                                                            X.get_allocator());
                    ASSERTV(SPEC1, SPEC2, ra == Z.get_allocator());

                    // Manipulate source object `Z` to ensure it is in a valid
                    // state and is independent of `X`.

                    Iter RESULT = primaryManipulator(&mZ, 'Z', ra);
                    ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    foa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, SPEC2, roa.numBlocksInUse(),
                            empty || ((isPropagate || &ra == &oa) ==
                                                  (0 < roa.numBlocksInUse())));

                    // Verify subsequent manipulation of target object `X`.
                    RESULT = primaryManipulator(&mX, 'Z', oa);
                    ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                            LENGTH1 + 1 == X.size());
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                    foa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, ooa.numBlocksInUse(),
                            0 == ooa.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, roa.numBlocksInUse(),
                            0 == roa.numBlocksInUse());
                }
            }

            // self-assignment
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                Obj mX(oa);       const Obj& X  = gg(&mX,  SPEC1);

                ASSERTV(SPEC1, ZZ, X, ZZ == X);

                bslma::TestAllocatorMonitor oam(&ooa);

                int numPasses = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                    ++numPasses;
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
                    ASSERTV(SPEC1, mR,  &X, mR == &X);
                    ASSERTV(SPEC1, ZZ,   X, ZZ == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERT(1 == numPasses);

                ASSERTV(SPEC1, oa == X.get_allocator());

                ASSERTV(SPEC1, oam.isTotalSame());
            }
            // Verify all object memory is released on destruction.
            ASSERTV(SPEC1, ooa.numBlocksInUse(), 0 == ooa.numBlocksInUse());
        }
    }

    if (veryVerbose) printf(
                        "Testing move assignment with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const char *const RESULT1 = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(RESULT1);

            if (ti && INDEX1 == DATA[ti-1].d_index) {
                continue;    // redundant, skip
            }
            if (5 < LENGTH1) {
                continue;    // too time consuming (O(LENGTH1^2)), skip
            }

            Obj mZZ(sa); const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(SPEC1) P(ZZ) }

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         INDEX2 = DATA[tj].d_index;
                const char *const SPEC2  = DATA[tj].d_spec;

                if (tj && INDEX2 == DATA[tj-1].d_index) {
                    continue;    // redundant, skip
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ooa) {
                        ++numPasses;
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        bsls::Types::Int64 al = ooa.allocationLimit();
                        ooa.setAllocationLimit(-1);
                        Obj mX(oa); const Obj& X = gg(&mX, SPEC2);

                        ALLOC& ra  = 'a' == CONFIG ? za  : oa;

                        Obj mZ(ra); const Obj& Z = gg(&mZ, SPEC1);

                        if (veryVerbose) { T_ P_(X) P(Z) }

                        ASSERTV(SPEC1, SPEC2, Z, X,
                                (Z == X) == (ti == tj));

                        typename Obj::const_pointer pointers[2];
                        storeFirstNElemAddr(pointers, Z,
                                            sizeof pointers /sizeof *pointers);

                        Obj mE(sa);
                        if (&ra != &oa && al >= 0 && al <= TYPE_ALLOC) {
                            // We will throw on the reserveNodes so that source
                            // object will be unchanged on exception.
                            gg(&mE, SPEC1);
                        }
                        // The else here is that the source object will be made
                        // empty on exception.
                        ExceptionProctor<Obj, ALLOC> proctor(&Z, L_,
                                                           MoveUtil::move(mE));

                        ooa.setAllocationLimit(al);

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        proctor.release();
                        ooa.setAllocationLimit(-1);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                        if (veryVerbose) { T_ P_(ZZ) P_(Z) P(X) }

                        // Manipulate source object `Z` to ensure it is in a
                        // valid state and is independent of `X`.
                        (void) primaryManipulator(&mZ, 'Z', ra);
                        ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                        ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                        // Verify subsequent manipulation of target object `X`.
                        (void) primaryManipulator(&mX, 'Z', oa);
                        ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                                LENGTH1 + 1 == X.size());
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV((!PLAT_EXC || 0 == ti || 'b' == CONFIG ||
                                                                   isPropagate)
                                                          == (1 == numPasses));
                }
            }
        }
    }

    ASSERTV(keyUsesDefaultAlloc, doa.numBlocksTotal(),
                                         bslma::UsesBslmaAllocator<KEY>::value,
                          !keyUsesDefaultAlloc == (0 == doa.numBlocksTotal()));
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase26()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR:
    //
    // Concerns:
    // 1. The newly created object has the same value (using the equality
    //    operator) as that of the original object before the call.
    //
    // 2. All internal representations of a given value can be used to create a
    //    new object of equivalent value.
    //
    // 3. The allocator is propagated to the newly created object if (and only
    //    if) no allocator is specified in the call to the move constructor.
    //
    // 4. A constant-time move, with no additional memory allocations or
    //    deallocations, is performed when no allocator or the same allocator
    //    as that of the original object is passed to the move constructor.
    //
    // 5. A linear operation, where each element is move-inserted into the
    //    newly created object, is performed when a `0` or an allocator that is
    //    different than that of the original object is explicitly passed to
    //    the move constructor.
    //
    // 6. The original object is always left in a valid state; the allocator
    //    address held by the original object is unchanged.
    //
    // 7. Subsequent changes to or destruction of the original object have no
    //    effect on the move-constructed object and vice-versa.
    //
    // 8. The object has its internal memory management system hooked up
    //    properly so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // 9. Every object releases any allocated memory at destruction.
    //
    // 10. Any memory allocation is exception neutral.
    //
    // Plan:
    // 1. Specify a set `S` of object values with substantial and varied
    //    differences, ordered by increasing length, to be used sequentially in
    //    the following tests; for each entry, create a control object.   (C-2)
    //
    // 2. Call the move constructor to create the container in all relevant use
    //    cases involving the allocator: 1) no allocator passed in, 2) a `0` is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //    as that of the original object is explicitly passed in, and 4) a
    //    different allocator than that of the original object is passed in.
    //
    // 3. For each of the object values (P-1) and for each configuration (P-2),
    //    verify the following:
    //
    //   1. Verify the newly created object has the same value as that of the
    //      original object before the call to the move constructor (control
    //      value).                                                       (C-1)
    //
    //   2. CONTAINER SPECIFIC NOTE: Ensure that the comparator was copied.
    //
    //   3. Where a constant-time move is expected, ensure that no memory was
    //      allocated, that element addresses did not change, and that the
    //      original object is left in the default state.         (C-3..5, C-7)
    //
    //   4. Where a linear-time move is expected, ensure that the move
    //      constructor was called for each element.                   (C-6..7)
    //
    //   5. CONTAINER SPECIFIC:
    //      Where a linear-time move is expected, the value of the original
    //      object is also left in the default state because move insertion
    //      changes the value of the original key object and violates the class
    //      invariant enforcing uniqueness of contained keys.             (C-7)
    //
    //   6. Ensure that the new original, and control object continue to have
    //      the correct allocator and that all memory allocations come from the
    //      appropriate allocator.                                    (C-3,C-9)
    //
    //   7. Manipulate the original object (after the move construction) to
    //      ensure it is in a valid state, destroy it, and then manipulate the
    //      newly created object to ensure that it is in a valid state.   (C-8)
    //
    //   8. Verify all memory is released when the object is destroyed.  (C-11)
    //
    // 4. Perform tests in the presence of exceptions during memory allocations
    //    using a `bslma::TestAllocator` and varying its *allocation* *limit*.
    //                                                                   (C-10)
    //
    // Testing:
    //   multiset(bslmf::MovableRef<multiset> original);
    //   multiset(bslmf::MovableRef<multiset> original, const A& allocator);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const TestValues VALUES;

    static const char *SPECS[] = {
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

    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (verbose)
        printf("\nTesting both versions of move constructor.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object ZZ with the scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mZZ(xscratch); const Obj& ZZ = gg(&mZZ, SPEC);

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

                // Create source object `Z`.
                Obj *srcPtr = new (fa) Obj(xsa);
                Obj& mZ = *srcPtr; const Obj& Z = gg(&mZ, SPEC);

                typename Obj::const_pointer pointers[2] = { 0, 0 };
                storeFirstNElemAddr(pointers, Z,
                                    sizeof pointers / sizeof *pointers);

                bslma::TestAllocatorMonitor oam(&da), sam(&sa);

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
                    ASSERTV(CONFIG, "Bad allocator config.", false);
                  } return;                                           // RETURN
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;

                KeyAllocator         xoa(&oa);

                Obj& mX = *objPtr; const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // Verify that the comparator was copied.
                ASSERTV(SPEC, CONFIG,
                        X.key_comp().count() == Z.key_comp().count());

                // CONTAINER SPECIFIC NOTE: For `multiset`, the original object
                // is left in the default state even when the source and target
                // objects use different allocators because move-insertion
                // changes the value of the source key and violates the
                // requirements for keys contained in the `multiset`.

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
                    ASSERTV(SPEC, CONFIG, &sa == &oa,
                                          oam.isTotalUp() || empty);
                }

                // Verify that `X`, `Z`, and `ZZ` have the correct allocator.
                ASSERTV(SPEC, CONFIG, &scratch == ZZ.get_allocator());
                ASSERTV(SPEC, CONFIG,      &sa ==  Z.get_allocator());
                ASSERTV(SPEC, CONFIG,      &oa ==  X.get_allocator());

                // Verify no allocation from the non-object allocator and that
                // object allocator is hooked up.
                ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || empty);

                // Manipulate source object `Z` to ensure it is in a valid
                // state and is independent of `X`.
                Iter RESULT = primaryManipulator(&mZ, 'Z', xsa);
                ASSERTV(SPEC, CONFIG, Z, 1 == Z.size());
                ASSERTV(SPEC, CONFIG, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X, ZZ, X == ZZ);

                // Verify subsequent manipulation of new object `X`.
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
        printf("\nTesting move constructor with injected exceptions.\n");
#if defined(BDE_BUILD_TARGET_EXC)
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);

            Obj mZZ(xscratch);    const Obj& ZZ = gg(&mZZ, SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            KeyAllocator         xoa(&oa);
            KeyAllocator         xza(&za);

            const bsls::Types::Int64 BB = oa.numBlocksTotal();
            const bsls::Types::Int64  B = oa.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
            }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj mE(xscratch);
                if (oa.allocationLimit() >= 0
                 && oa.allocationLimit() <= TYPE_ALLOC) {
                    // We will throw on the reserveNodes so that source object
                    // will be unchanged on exception.
                    gg(&mE, SPEC);
                }
                // The else here is that the source object will be made empty
                // on exception.
                Obj mZ(xza);  const Obj& Z = gg(&mZ, SPEC);
                ExceptionProctor<Obj, ALLOC> proctor(&Z, L_,
                                                     MoveUtil::move(mE));

                Obj mX(bslmf::MovableRefUtil::move(mZ), xoa);
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\t\tException Case  :\n");
                    printf("\t\t\t\tObj : "); P(X);
                }
                ASSERTV(SPEC, X, ZZ, ZZ == X);
                ASSERTV(SPEC, 0 == Z.size());
                ASSERTV(SPEC, Z.get_allocator() != X.get_allocator());

                proctor.release();

                // Manipulate source object `Z` to ensure it is in a
                // valid state and is independent of `X`.
                Iter RESULT = primaryManipulator(&mZ, 'Z', xza);
                ASSERTV(RESULT != Z.end());
                ASSERTV(SPEC, Z, 1 == Z.size());
                ASSERTV(SPEC, Z, ZZ, Z != ZZ);
                ASSERTV(SPEC, X, ZZ, X == ZZ);

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
                       static_cast<int>(LENGTH + LENGTH * (1 + LENGTH) / 2);
                ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                ASSERTV(SPEC, B + 0 == A);
            }
        }
    }
#endif
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase25()
{
    // ------------------------------------------------------------------------
    // TESTING STANDARD INTERFACE COVERAGE
    //
    // Concern:
    // 1. The type provides the full interface defined by the section
    //    `[multiset.overview]` in the C++11 standard, with exceptions for
    //    methods that require C+11 compiler support.
    //
    // Plan:
    // 1. Invoke each constructor defined by the standard.  (C-1)
    //
    // 2. For each method and free function defined in the standard, use the
    //    respective address of the method to initialize function pointers
    //    having the appropriate signatures and return type for that method
    //    according to the standard.  (C-1)
    //
    // Testing:
    //   CONCERN: The type provides the full interface defined by the standard.
    // ------------------------------------------------------------------------

    // construct/copy/destroy:

    // explicit multiset(const Compare& comp = Compare(),
    //                   const Allocator& = Allocator());
    bsl::multiset<KEY, COMP, StlAlloc> A((COMP()), (StlAlloc()));


    // template <class InputIterator>
    // multiset(InputIterator first, InputIterator last,
    // const Compare& comp = Compare(), const Allocator& = Allocator());

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    TestValues values("ABC", &scratch);

    bsl::multiset<KEY, COMP, StlAlloc> B(values.begin(),
                                         values.end(),
                                         COMP(),
                                         StlAlloc());

    // multiset(const multiset<Key,Compare,Allocator>& x);
    bsl::multiset<KEY, COMP, StlAlloc> C(B);

    // multiset(multiset<Key,Compare,Allocator>&& x); <- C++11 only

    // explicit multiset(const Allocator&);
    bsl::multiset<KEY, COMP, StlAlloc> D((StlAlloc()));

    // multiset(const multiset&, const Allocator&);
    bsl::multiset<KEY, COMP, StlAlloc> E(B, StlAlloc());

    // multiset(multiset&&, const Allocator&); <- C++11 only

    // multiset(initializer_list<value_type>, const Compare& = Compare(), const
    // Allocator& = Allocator()); <- C++11 only

    // ~multiset();
    // destructor always exist


    {
        using namespace bsl;

        //multiset<Key,Compare,Allocator>& operator=
        // (const multiset<Key,Compare,Allocator>& x);
        Obj& (Obj::*operatorAg) (const Obj&) = &Obj::operator=;
        (void) operatorAg;  // quash potential compiler warning
    }

    // multiset<Key,Compare,Allocator>& operator=(
    // multiset<Key,Compare,Allocator>&&
    // x); <- C++11 only

    // multiset& operator=(initializer_list<value_type>); <- C++11 only

    // allocator_type get_allocator() const noexcept;
    typename Obj::allocator_type (Obj::*methodGetAllocator) () const =
                                                           &Obj::get_allocator;
    (void) methodGetAllocator;

    // iterators

    // iterator begin() noexcept;
    typename Obj::iterator (Obj::*methodBegin) () = &Obj::begin;
    (void) methodBegin;

    // const_iterator begin() const noexcept;
    typename Obj::const_iterator (Obj::*methodBeginConst) () const =
                                                                   &Obj::begin;
    (void) methodBeginConst;

    // iterator end() noexcept;
    typename Obj::iterator (Obj::*methodEnd) () = &Obj::end;
    (void) methodEnd;

    // const_iterator end() const noexcept;
    typename Obj::const_iterator (Obj::*methodEndConst) () const = &Obj::end;
    (void) methodEndConst;

    // reverse_iterator rbegin() noexcept;
    typename Obj::reverse_iterator (Obj::*methodRbegin) () = &Obj::rbegin;
    (void) methodRbegin;

    // const_reverse_iterator rbegin() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodRbeginConst) () const =
                                                                  &Obj::rbegin;
    (void) methodRbeginConst;

    // reverse_iterator rend() noexcept;
    typename Obj::reverse_iterator (Obj::*methodRend) () = &Obj::rend;
    (void) methodRend;

    // const_reverse_iterator rend() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodRendConst) () const =
                                                                    &Obj::rend;
    (void) methodRendConst;

    // const_iterator cbegin() const noexcept;
    typename Obj::const_iterator (Obj::*methodCbegin) () const = &Obj::cbegin;
    (void) methodCbegin;

    // const_iterator cend() const noexcept;
    typename Obj::const_iterator (Obj::*methodCend) () const = &Obj::cend;
    (void) methodCend;

    // const_reverse_iterator crbegin() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodCrbegin) () const =
                                                                 &Obj::crbegin;
    (void) methodCrbegin;

    // const_reverse_iterator crend() const noexcept;
    typename Obj::const_reverse_iterator (Obj::*methodCrend) () const =
                                                                   &Obj::crend;
    (void) methodCrend;

    // capacity:
    // bool empty() const noexcept;
    bool (Obj::*methodEmpty) () const = &Obj::empty;
    (void) methodEmpty;

    // size_type size() const noexcept;
    typename Obj::size_type (Obj::*methodSize) () const = &Obj::size;
    (void) methodSize;

    // size_type max_size() const noexcept;
    typename Obj::size_type (Obj::*methodMaxSize) () const = &Obj::max_size;
    (void) methodMaxSize;

    // modifiers:

    // template <class... Args> iterator emplace(Args&&... args);
    // <- C++11 only

    // template <class... Args> iterator emplace_hint(const_iterator position,
    // Args&&... args); <- C++11 only

    // iterator insert(const value_type& x);

    typename Obj::iterator (Obj::*methodInsert) (
                               const typename Obj::value_type&) = &Obj::insert;
    (void) methodInsert;

    // iterator insert(P&& x); <- C++11 only

    // iterator insert(const_iterator position, const value_type& x);
    typename Obj::iterator (Obj::*methodInsert2) (
        typename Obj::const_iterator, const typename Obj::value_type&) =
                                                                  &Obj::insert;
    (void) methodInsert2;


    // iterator insert(const_iterator position, value_type&& x); <-
    // C++11 only

    // template <class InputIterator>
    // void insert(InputIterator first, InputIterator last);
    void (Obj::*methodInsert3) (
                typename Obj::iterator, typename Obj::iterator) = &Obj::insert;
    (void) methodInsert3;

    // void insert(initializer_list<value_type>); <- C++11 only

    // iterator erase(const_iterator position);
    typename Obj::iterator (Obj::*methodErase) (typename Obj::const_iterator) =
                                                                   &Obj::erase;
    (void) methodErase;

    // size_type erase(const key_type& x);
    typename Obj::size_type (Obj::*methodErase2) (
        const typename Obj::key_type&) = &Obj::erase;
    (void) methodErase2;

    // iterator erase(const_iterator first, const_iterator last);
    typename Obj::iterator (Obj::*methodErase3) (
        typename Obj::const_iterator, typename Obj::const_iterator) =
                                                                   &Obj::erase;
    (void) methodErase3;

    // void swap(multiset& other);
    void (Obj::*methodSwap) (Obj&) = &Obj::swap;
    (void) methodSwap;

    // void clear() noexcept;
    void (Obj::*methodClear) () = &Obj::clear;
    (void) methodClear;

    // observers:
    // key_compare key_comp() const;
    typename Obj::key_compare (Obj::*methodKeyComp) () const = &Obj::key_comp;
    (void) methodKeyComp;

    // value_compare value_comp() const;
    typename Obj::value_compare (Obj::*methodValueComp) () const =
                                                           &Obj::value_comp;
    (void) methodValueComp;

    // multiset operations:
    // iterator find(const key_type& x);
    typename Obj::iterator (Obj::*methodFind) (const typename Obj::key_type&) =
                                                                    &Obj::find;
    (void) methodFind;

    // const_iterator find(const key_type& x) const;
    typename Obj::const_iterator (Obj::*methodFindConst) (
                             const typename Obj::key_type&) const = &Obj::find;
    (void) methodFindConst;

    // size_type count(const key_type& x) const;
    typename Obj::size_type (Obj::*methodCount) (
                            const typename Obj::key_type&) const = &Obj::count;
    (void) methodCount;

    // iterator lower_bound(const key_type& x);
    typename Obj::iterator (Obj::*methodLowerBound) (
                            const typename Obj::key_type&) = &Obj::lower_bound;
    (void) methodLowerBound;

    // const_iterator lower_bound(const key_type& x) const;
    typename Obj::const_iterator (Obj::*methodLowerBoundConst) (
                      const typename Obj::key_type&) const = &Obj::lower_bound;
    (void) methodLowerBoundConst;

    // iterator upper_bound(const key_type& x);
    typename Obj::iterator (Obj::*methodUpperBound) (
                            const typename Obj::key_type&) = &Obj::upper_bound;
    (void) methodUpperBound;

    // const_iterator upper_bound(const key_type& x) const;
    typename Obj::const_iterator (Obj::*methodUpperBoundConst) (
                      const typename Obj::key_type&) const = &Obj::upper_bound;
    (void) methodUpperBoundConst;

    // pair<iterator,iterator> equal_range(const key_type& x);
    pair<typename Obj::iterator, typename Obj::iterator> (
        Obj::*methodEqualRange) (const typename Obj::key_type&) =
                                                             &Obj::equal_range;
    (void) methodEqualRange;

    // pair<const_iterator,const_iterator> equal_range(const key_type& x)
    // const;
    pair<typename Obj::const_iterator, typename Obj::const_iterator> (
        Obj::*methodEqualRangeConst) (const typename Obj::key_type&) const =
                                                             &Obj::equal_range;
    (void) methodEqualRangeConst;

    using namespace bsl;

    // template <class Key, class Compare, class Allocator>
    // bool operator==(const multiset<Key,Compare,Allocator>& x,
    // const multiset<Key,Compare,Allocator>& y);
    bool (*operatorEq)(const Obj&, const Obj&) = operator==;
    (void) operatorEq;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs != rhs;
    };
#else
    // template <class Key, class Compare, class Allocator>
    // bool operator!=(const multiset<Key,Compare,Allocator>& x,
    // const multiset<Key,Compare,Allocator>& y);
    bool (*operatorNe)(const Obj&, const Obj&) = operator!=;
    (void) operatorNe;
#endif

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs < rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs > rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs <= rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs >= rhs;
    };
    (void) [](const Obj& lhs, const Obj& rhs) {
        return lhs <=> rhs;
    };
#else
    // template <class Key, class Compare, class Allocator>
    // bool operator< (const multiset<Key,Compare,Allocator>& x,
    // const multiset<Key,Compare,Allocator>& y);
    bool (*operatorLt)(const Obj&, const Obj&) = operator<;
    (void) operatorLt;

    // template <class Key, class Compare, class Allocator>
    // bool operator> (const multiset<Key,Compare,Allocator>& x,
    // const multiset<Key,Compare,Allocator>& y);
    bool (*operatorGt)(const Obj&, const Obj&) = operator>;
    (void) operatorGt;

    // template <class Key, class Compare, class Allocator>
    // bool operator>=(const multiset<Key,Compare,Allocator>& x,
    // const multiset<Key,Compare,Allocator>& y);
    bool (*operatorGe)(const Obj&, const Obj&) = operator>=;
    (void) operatorGe;

    // template <class Key, class Compare, class Allocator>
    // bool operator<=(const multiset<Key,Compare,Allocator>& x,
    // const multiset<Key,Compare,Allocator>& y);
    bool (*operatorLe)(const Obj&, const Obj&) = operator<=;
    (void) operatorLe;
#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

    // specialized algorithms:
    // template <class Key, class Compare, class Allocator>
    // void swap(multiset<Key,Compare,Allocator>& x,
    // multiset<Key,Compare,Allocator>& y);

    void (*functionSwap) (Obj&, Obj&) = &swap;
    (void) functionSwap;
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase24()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTOR OF A TEMPLATE WRAPPER CLASS
    //
    // Concern:
    // 1. The constructor of a templatized wrapper around the container will
    //    compile.  (C-1)
    //
    // Plan:
    // 1. Invoke each constructor of a class that inherits from the container.
    //
    // Testing:
    //   CONCERN: Constructor of a template wrapper class compiles
    // ------------------------------------------------------------------------

    // The following may fail to compile on AIX

    TemplateWrapper<KEY, DummyComparator, DummyAllocator<KEY> > obj1;
    (void) obj1;

    // This would compile because the copy constructor doesn't use a default
    // argument.

    TemplateWrapper<KEY, DummyComparator, DummyAllocator<KEY> > obj2(obj1);
    (void) obj2;

    // This would also compile, most likely because the constructor is
    // templatized.

    typename Obj::value_type array[1];
    TemplateWrapper<KEY, DummyComparator, DummyAllocator<KEY> > obj3(array,
                                                                     array);
    (void) obj3;
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase23()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concern:
    // 1. The object has the necessary type traits.
    //
    // Plan:
    // 1. Use `BSLMF_ASSERT` to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits
    // ------------------------------------------------------------------------

    // Verify multiset defines the expected traits.
    BSLMF_ASSERT((1 == bslalg::HasStlIterators<bsl::multiset<KEY> >::value));
    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<bsl::multiset<KEY> >::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<bsl::multiset<KEY,
                                          std::less<KEY>, StlAlloc> >::value));

    // Verify multiset does not define other common traits.
    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<
                                                 bsl::multiset<KEY> >::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<
                                                 bsl::multiset<KEY> >::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseMoveable<bsl::multiset<KEY> >::value));

    BSLMF_ASSERT((0 ==bslmf::HasPointerSemantics<bsl::multiset<KEY> >::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<
                                                 bsl::multiset<KEY> >::value));
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase22()
{
    // ------------------------------------------------------------------------
    // TESTING STL ALLOCATOR
    //
    // Concern:
    // 1. A standard compliant allocator can be used instead of
    //    `bsl::allocator`.
    //
    // 2. Methods that uses the allocator (e.g., variations of constructor,
    //    `insert` and `swap`) can successfully populate the object.
    //
    // 3. `KEY` types that allocate memory uses the default allocator instead
    //    of the object allocator.
    //
    // 4. Every object releases any allocated memory at destruction.
    //
    // Plan:
    // 1. Using a loop base approach, create a list of specs and their
    //    expected value.  For each spec:
    //
    //   1. Create an object using a standard allocator through multiple ways,
    //      including: range-based constructor, copy constructor, range-based
    //      insert, multiple inserts, and swap.
    //
    //   2. Verify the value of each objects is as expected.
    //
    //   3. For types that allocate memory, verify memory for the elements
    //      comes from the default allocator.
    //
    // Testing:
    //  CONCERN: `multiset` is compatible with a standard allocator.
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::multiset<KEY, COMP, StlAlloc> Obj;

    StlAlloc scratch;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const ptrdiff_t   LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results, scratch);
        ASSERT(0 <= LENGTH);

        TestValues CONT(SPEC, scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        {
            Obj mX(BEGIN, END);  const Obj& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());

            Obj mY(X);  const Obj& Y = mY;

            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH == da.numBlocksInUse());

            Obj mZ;  const Obj& Z = mZ;

            mZ.swap(mX);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    2 * TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            Obj mX;  const Obj& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        CONT.resetIterators();

        {
            Obj mX;  const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::multiset<int, std::less<int>, StlAlloc> TestObj;
        TestObj mX;
        mX.insert(1);
        TestObj mY;
        mY = mX;
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase21()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARATOR
    //
    // Concern:
    // 1. Both functor and function type can be used.
    //
    // 2. If a comparator is not supplied, it defaults to `std::less`.
    //
    // 3. The comparator is set correctly.
    //
    // 4. `key_comp` and `value_comp` return the comparator that was passed in
    //    on construction.
    //
    // 5. The supplied comparator is used in all operations involving
    //    comparisons instead of `operator <`.
    //
    // 6. Comparator is properly propagated on copy construction, copy
    //    assignment, and swap.
    //
    // 7. Functor with a non-`const` function call operator can be used -- any
    //    non-`const` operation on the multiset that utilizes the comparator
    //    can be invoked from a reference providing non-modifiable access to
    //    the multiset.
    //
    // Plan:
    // 1. Create default object and verify comparator is `std::less`.  (C-2)
    //
    // 2. Create object with a function type comparator.
    //
    // 3. Create object with functor type comparator for all constructor.
    //    (C-1, 3..4)
    //
    // 4. Using the table-driven technique:
    //
    //   1. Specify a set of (unique) valid object values.
    //
    // 5. For each row (representing a distinct object value set, `V`) in the
    //    table described in P-4:
    //
    //   1. Create a comparator object using the greater than operator for
    //      comparisons.
    //
    //   2. Create an object having the value `V` using the range constructor
    //      passing in the comparator created in P-5.1.  Verify the object's
    //      value.  (P-5)
    //
    //   3. Copy construct an object passing in the object created in P-5.2,
    //      verify that the comparator compare equals to the one created in
    //      P-5.1.  (P-6)
    //
    //   4. Default construct an object and assign to the object from the one
    //      created in P-5.2.  Verify that the comparator of the new object
    //      compare equals to the one created in P-5.1.  (P-6)
    //
    //   5. Default construct an object and swap this object with the one
    //      created in P-5.2.  Verify that the comparator of the new object
    //      compare equals to the one created in P-5.1, and the original object
    //      has a default constructed comparator.  (P-6)
    //
    // 6. Repeat P-5 except with a comparator having non-`const` function call
    //    operator.  (P-7)
    //
    // Testing:
    //   key_compare key_comp() const;
    //   value_compare value_comp() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    {
        bsl::multiset<int> X;
        std::less<int> keyComparator   = X.key_comp();
        std::less<int> valueComparator = X.value_comp();

        (void) keyComparator;  // quash potential compiler warning
        (void) valueComparator;  // quash potential compiler warning
    }

    {
        typedef bool (*ComparatorFunction)(const KEY&, const KEY&);
        bsl::multiset<KEY, ComparatorFunction> X(&lessThanFunction<KEY>);
        ASSERTV((ComparatorFunction)&lessThanFunction<KEY> == X.key_comp());
        ASSERTV((ComparatorFunction)&lessThanFunction<KEY> == X.value_comp());
    }

    static const int ID[] = { 0, 1, 2 };
    const int NUM_ID = sizeof ID / sizeof *ID;

    for (int ti = 0; ti < NUM_ID; ++ti) {
        const COMP C(ti);
        {
            const Obj X(C);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti, C.id() == X.value_comp().id());
            ASSERTV(ti, 0 == X.key_comp().count());
        }
        {
            const Obj X(C, &scratch);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti, C.id() == X.value_comp().id());
            ASSERTV(ti, 0 == X.key_comp().count());
        }
        {
            const Obj X((KEY *)0, (KEY *)0, C, &scratch);
            ASSERTV(ti, C.id() == X.key_comp().id());
            ASSERTV(ti, C.id() == X.value_comp().id());
            ASSERTV(ti, 0 == X.key_comp().count());
        }
    }

    static const struct {
        int         d_line;             // source line number
        const char *d_spec;             // spec
        const char *d_results;
    } DATA[] = {
        { L_,  "",           ""          },
        { L_,  "A",          "A"         },
        { L_,  "ABC",        "CBA"       },
        { L_,  "ACBD",       "DCBA"      },
        { L_,  "BCDAE",      "EDCBA"     },
        { L_,  "GFEDCBA",    "GFEDCBA"   },
        { L_,  "ABCDEFGH",   "HGFEDCBA"  },
        { L_,  "BCDEFGHIA",  "IHGFEDCBA" }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        const COMP C(1, false);  // create comparator that uses greater than

        {
            Obj mW(BEGIN, END, C);  const Obj& W = mW;

            ASSERTV(LINE, 0 == verifyContainer(W, EXP, LENGTH));
            ASSERTV(LINE, C == W.key_comp());

            Obj mX(W);  const Obj& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, C == X.key_comp());

            Obj mY;  const Obj& Y = mY;
            mY = mW;
            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, C == Y.key_comp());

            Obj mZ;  const Obj& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, C == Z.key_comp());
            ASSERTV(LINE, COMP() == W.key_comp());
        }

        CONT.resetIterators();

        {
            Obj mX(C);  const Obj& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        CONT.resetIterators();

        {
            Obj mX(C);  const Obj& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results, &scratch);

        TestValues CONT(SPEC, &scratch);

        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        const NonConstComp C(1, false);  // create non const comparator that
                                         // uses the greater than operator
        typedef bsl::multiset<KEY, NonConstComp> ObjNCC;

        {
            ObjNCC mW(BEGIN, END, C);  const ObjNCC& W = mW;

            ASSERTV(LINE, 0 == verifyContainer(W, EXP, LENGTH));
            ASSERTV(LINE, C == W.key_comp());

            ObjNCC mX(W);  const ObjNCC& X = mX;

            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, C == X.key_comp());

            ObjNCC mY;  const ObjNCC& Y = mY;
            mY = mW;
            ASSERTV(LINE, 0 == verifyContainer(Y, EXP, LENGTH));
            ASSERTV(LINE, C == Y.key_comp());

            ObjNCC mZ;  const ObjNCC& Z = mZ;
            mZ.swap(mW);

            ASSERTV(LINE, 0 == verifyContainer(Z, EXP, LENGTH));
            ASSERTV(LINE, C == Z.key_comp());
            ASSERTV(LINE, NonConstComp() == W.key_comp());
        }

        CONT.resetIterators();

        {
            ObjNCC mX(C);  const ObjNCC& X = mX;
            mX.insert(BEGIN, END);
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        CONT.resetIterators();

        {
            ObjNCC mX(C);  const ObjNCC& X = mX;
            for (size_t tj = 0; tj < CONT.size(); ++tj) {
                Iter RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *RESULT);
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase20()
{
    // ------------------------------------------------------------------------
    // TESTING `max_size` and `empty`
    //
    // Concern:
    // 1. `max_size` returns the `max_size` of the supplied allocator.
    //
    // 2. `empty` returns `true` only when the object is empty.
    //
    // Plan:
    // 1. Run each function and verify the result.  (C-1..2)
    //
    // Testing:
    //  bool empty() const;
    //  size_type max_size() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa(veryVeryVerbose);

    if (verbose) printf("\tTesting `max_size`.\n");
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
    const size_t NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("\tTesting `empty`.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON FREE OPERATORS
    //
    // Concerns:
    // 1. `operator<` returns the lexicographic comparison on two containers.
    //
    // 2. Comparison operator uses `operator<` on `key_type` instead of the
    //    supplied comparator.
    //
    // 3. `operator>`, `operator<=`, and `operator>=` are correctly tied to
    //    `operator<`.  i.e., For two multisets, `a` and `b`:
    //
    //   1. `(a > b) == (b < a)`
    //
    //   2. `(a <= b) == !(b < a)`
    //
    //   3. `(a >= b) == !(a < b)`
    //
    // 4. `operator<=>` is consistent with `<`, `>`, `<=`, `>=`.
    //
    // Plan:
    // 1. For a variety of objects of different sizes and different values,
    //    test that the comparison returns as expected.  (C-1..4)
    //
    // Testing:
    //   bool operator< (const multiset& lhs, const multiset& rhs);
    //   bool operator> (const multiset& lhs, const multiset& rhs);
    //   bool operator>=(const multiset& lhs, const multiset& rhs);
    //   bool operator<=(const multiset& lhs, const multiset& rhs);
    //   auto operator<=>(const multiset& lhs, const multiset& rhs);
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
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

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                // Create two distinct test allocators, `oax` and `oay`.

                bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                // Map allocators above to objects `X` and `Y` below.

                Obj mX(&oax); const Obj& X = gg(&mX, SPEC1);
                Obj mY(&oay); const Obj& Y = gg(&mY, SPEC2);

                if (veryVerbose) { T_ T_ P_(X) P(Y); }

                // Verify value and no memory allocation.

                bslma::TestAllocatorMonitor oaxm(&oax);
                bslma::TestAllocatorMonitor oaym(&oay);

                const bool isLess = INDEX1 < INDEX2;
                const bool isLessEq = INDEX1 <= INDEX2;

                TestComparator<KEY>::disableFunctor();

                ASSERTV(LINE1, LINE2,  isLess   == (X < Y));
                ASSERTV(LINE1, LINE2, !isLessEq == (X > Y));
                ASSERTV(LINE1, LINE2,  isLessEq == (X <= Y));
                ASSERTV(LINE1, LINE2, !isLess   == (X >= Y));

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
                ASSERTV(LINE1, LINE2,  isLess   == (X <=> Y < 0));
                ASSERTV(LINE1, LINE2, !isLessEq == (X <=> Y > 0));
                ASSERTV(LINE1, LINE2,  isLessEq == (X <=> Y <= 0));
                ASSERTV(LINE1, LINE2, !isLess   == (X <=> Y >= 0));
#endif

                TestComparator<KEY>::enableFunctor();

                ASSERTV(LINE1, LINE2, oaxm.isTotalSame());
                ASSERTV(LINE1, LINE2, oaym.isTotalSame());
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase18()
{
    // ------------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concern:
    // 1. `erase` with iterators returns the iterator right after the erased
    //    value(s).
    //
    // 2. `erase` with `key` returns the number of elements equals to `key`
    //    prior to erase.
    //
    // 3. Erased values are removed.
    //
    // 4. Erasing do not throw.
    //
    // 5. No memory is allocated.
    //
    // 6. QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    // 1. For each distinct length, `l`:
    //
    //   1. For each value, v, that would be in the object with that length:
    //
    //     1. Create an object with length, `l`.
    //
    //     2. Find `v` to get its iterator
    //
    //     2. Erase `v` with `erase(const_iterator position)`.  (C-4)
    //
    //     2. Verify return value.  (C-1)
    //
    //     3. Verify value is erased with `find`.  (C-3)
    //
    //     4. Verify no memory is allocated.  (C-5)
    //
    // 2. Repeat P-1 with `erase(const key_type& key)` (C-2).
    //
    // 3. For range erase, call erase on all possible range of for each length,
    //    `l` and verify result is as expected.
    //
    // 4. Verify that, in appropriate build modes, defensive checks are
    //    triggered for invalid values, but not triggered for adjacent valid
    //    ones (using the `BSLS_ASSERTTEST_*` macros).  (C-6)
    //
    // Testing:
    //   size_type erase(const key_type& key);
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nTesting `erase(pos)` on non-empty multiset.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj& X = gg(&mX, SPEC);

                CIter POS = X.begin();
                for (size_t i = 0; i < tj; ++i) {
                    ++POS;
                }

                CIter AFTER = POS;
                ++AFTER;
                CIter BEFORE = POS;
                if (BEFORE != X.begin()) {
                    --BEFORE;
                }

                if (veryVerbose) { P(*POS); }

                bslma::TestAllocatorMonitor oam(&oa);

                const Iter R = mX.erase(POS);

                if (veryVeryVerbose) { T_ T_ P(X); }

                ASSERTV(LINE, tj, AFTER == R); // Check return value

                // Check the element does not exist

                if (tj == 0) {
                    ASSERTV(LINE, tj, X.begin() == AFTER);
                }
                else {
                    ++BEFORE;
                    ASSERTV(LINE, tj, BEFORE == AFTER);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }
                else {
                    ASSERTV(LINE, tj, oam.isInUseSame());
                }
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());
            }
        }
    }

    if (verbose) printf("\nTesting `erase(key)` on non-empty multiset.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj& X = gg(&mX, SPEC);
                ASSERTV(LINE, tj, LENGTH == X.size());

                CIter AFTER = X.upper_bound(VALUES[tj]);
                CIter BEFORE = X.lower_bound(VALUES[tj]);
                if (BEFORE != X.begin()) {
                    --BEFORE;
                    ASSERTV(LINE, tj, BEFORE != AFTER);
                }
                else {
                    BEFORE = AFTER;
                }

                if (veryVerbose) { T_ P_(tj) P_(VALUES[tj]) P(X); }

                bslma::TestAllocatorMonitor oam(&oa);

                const size_t COUNT = X.count(VALUES[tj]);
                ASSERTV(LINE, tj, COUNT == mX.erase(VALUES[tj]));

                if (veryVerbose) {
                    T_ P(X);
                }

                // Check the element does not exist

                ASSERTV(LINE, tj, X.end() == X.find(VALUES[tj]));
                if (BEFORE == AFTER) {
                    ASSERTV(LINE, tj, X.begin() == AFTER);
                }
                else {
                    ++BEFORE;
                    ASSERTV(LINE, tj, BEFORE == AFTER);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                if (TYPE_ALLOC) {
                    ASSERTV(LINE, tj, oam.isInUseDown());
                }
                else {
                    ASSERTV(LINE, tj, oam.isInUseSame());
                }
                ASSERTV(LINE, tj, LENGTH, COUNT,  LENGTH - COUNT == X.size());

                // Erase a non-existing element.
                ASSERTV(LINE, tj, 0 == mX.erase(VALUES[tj]));
                ASSERTV(LINE, tj, X.size(), LENGTH - COUNT == X.size());
            }
        }
    }

    if (verbose) printf("\nTesting `erase(first, last)`.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0;  tj <= LENGTH; ++tj) {
            for (size_t tk = tj; tk <= LENGTH; ++tk) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj& X = gg(&mX, SPEC);

                CIter FIRST = X.begin();
                for (size_t i = 0; i < tj; ++i) {
                    ++FIRST;
                }

                CIter LAST  = X.begin();
                for (size_t i = 0; i < tk; ++i) {
                    ++LAST;
                }
                const size_t NUM_ELEMENTS = tk - tj;

                CIter AFTER  = LAST;
                CIter BEFORE = FIRST;
                if (BEFORE != X.begin()) {
                    --BEFORE;
                }

                if (veryVerbose) {
                    if (FIRST != X.end()) {
                        P(*FIRST)
                    }
                    if (LAST != X.end()) {
                        P(*LAST);
                    }
                }


                bslma::TestAllocatorMonitor oam(&oa);

                const Iter R = mX.erase(FIRST, LAST);
                ASSERTV(LINE, tj, AFTER == R); // Check return value

                if (veryVeryVerbose) {
                    T_ T_ P(X);
                }

                // Check the element does not exist

                if (tj == 0) {
                    ASSERTV(LINE, tj, tk, X.begin() == AFTER);
                }
                else {
                    CIter next = BEFORE;
                    ++next;
                    ASSERTV(LINE, tj, tk, AFTER == next);
                }

                ASSERTV(LINE, tj, oam.isTotalSame());
                if (TYPE_ALLOC && 1 <= NUM_ELEMENTS) {
                    ASSERTV(LINE, tj, tk, oam.isInUseDown());
                }
                else {
                    ASSERTV(LINE, tj, tk, oam.isInUseSame());
                }
                ASSERTV(LINE, LENGTH, X.size(), NUM_ELEMENTS,
                        LENGTH == X.size() + NUM_ELEMENTS);
            }
            }
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        if (veryVerbose) printf("'erase\n");
        {
            const TestValues VALUES;

            Obj mX;  const Obj& X = mX;
            (void) X;
            Iter it = mX.insert(mX.end(), VALUES[0]);

            ASSERT_SAFE_FAIL(mX.erase(X.end()));
            ASSERT_SAFE_PASS(mX.erase(it));
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase17()
{
    // ------------------------------------------------------------------------
    // RANGE `insert`
    //
    // Concern:
    // 1. All values within the range [first, last) are inserted.
    //
    // 2. Each iterator is deferenced only once.
    //
    // 3. Repeated values are also inserted.
    //
    // 4. Any memory allocation is from the object allocator.
    //
    // 5. There is no temporary memory allocation from any allocator.
    //
    // 6. Inserting no elements allocates no memory.
    //
    // 7. Any memory allocation is exception neutral.
    //
    // 8. QoI: Range insertion allocates a single block for nodes when the
    //    number of elements can be determined and no free nodes are available.
    //    (The contained elements may require additional allocations.)
    //
    // Plan:
    // 1. Using the table-driven technique:
    //
    //   1. Specify a set of (unique) valid object values.
    //
    // 2. For each row (representing a distinct object value set, `V`) in the
    //    table described in P-1:
    //
    //   1. Use the range constructor to create a object with part of the
    //      elements in `V`.
    //
    //   2. Insert the rest of `V` under the presence of exception.  (C-7)
    //
    //   3. Verify the object's value.  (C-1-3)
    //
    //   4. If the range is empty, verify no memory is allocated  (C-6)
    //
    //   5. Verify no temporary memory is allocated.  (C-5)
    //
    //   6. Verify no memory is allocated from the default allocator (C-4)
    //
    // 5. Invoke the `testRangeInsertOptimization` function that creates a
    //    container for a non-allocating type that uses a test allocator to
    //    supply memory.  `insert` elements using both forward and random
    //    access iterators.  `clear` the elements and re-insert them.  Compare
    //    the state of the allocator to expected memory needs at each step of
    //    the scenario.  (C-8).
    //
    // Testing:
    //   void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
    // ------------------------------------------------------------------------

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_results);
        const TestValues  EXP(DATA[ti].d_results);

        TestValues CONT(SPEC);

        CONT.resetIterators();
        typename TestValues::iterator BEGIN = CONT.begin();
        typename TestValues::iterator END   = CONT.end();

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVerbose);

        KeyAllocator         xoa(&oa);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(xoa);  const Obj& X = mX;

        bslma::TestAllocatorMonitor oam(&oa);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

            mX.clear();
            CONT.resetIterators();

            mX.insert(BEGIN, END);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        if (LENGTH == 0) {
            ASSERTV(LINE, oam.isTotalSame());
            ASSERTV(LINE, oam.isInUseSame());
        }
        else {
            ASSERTV(LINE, oam.isTotalUp());
            ASSERTV(LINE, oam.isInUseUp());
        }
        ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));

        ASSERTV(LINE, da.numBlocksTotal(), 0 == da.numBlocksTotal());
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT
    //
    // Concerns:
    // 1. `insert` returns an iterator referring to the newly inserted element
    //    if it did not already exists, and to the existing element if it did.
    //
    // 2. A new element is added to the container and the order of the
    //    container remains correct.
    //
    // 3. Inserting with the correct hint places the new element right before
    //    the hint.
    //
    // 4. Inserting with correct hint requires no more than 2 comparisons.
    //
    // 5. Incorrect hint will be ignored and `insert` will proceed as if the
    //    hint is not supplied.
    //
    // 6. Insertion is exception neutral w.r.t. memory allocation.
    //
    // 7. Internal memory management system is hooked up properly
    //    so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // Plan:
    // 1. For insertion we will create objects of varying sizes and capacities
    //    containing default values, and insert a `value`.
    //
    //   1. For each set of values, set hint to be `lower_bound`,
    //      `upper_bound`, `begin`, `begin + 1`, `end - 1`, `end`
    //
    //     1. For each value in the set, `insert` the value with hint.
    //
    //       1. Compute the number of allocations and verify it is as
    //          expected.
    //
    //       2. Verify the return value and the resulting data in the container
    //          is as expected.
    //
    //       3. Verify the new element is inserted right before the hint if
    //          the hint is valid.  (C-3)
    //
    //       4. Verify the number of comparisons is no more than 2 if the hint
    //          is valid.  (C-4)
    //
    //       5. Verify all allocations are from the object's allocator.  (C-6)
    //
    // 2. Repeat P-1 under the presence of exception  (C-7)
    //
    // Testing:
    //   iterator insert(const_iterator position, const value_type& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
    } DATA[] = {
        //line  spec
        //----  --------------

        { L_,   "A",           },
        { L_,   "AAA",         },
        { L_,   "ABCDEFGH",    },
        { L_,   "ABCDEABCDEF", },
        { L_,   "EEDDCCBBAA",  }
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting `insert` with hint.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'f'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.lower_bound(VALUES[tj]);
                      } break;
                      case 'b': {
                          hint = X.upper_bound(VALUES[tj]);
                      } break;
                      case 'c': {
                          hint = X.begin();
                      } break;
                      case 'd': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'e': {
                          hint = X.end();
                          if (hint != X.begin()) {
                              --hint;
                          }
                      } break;
                      case 'f': {
                          hint = X.end();
                      } break;
                      default: {
                          ASSERTV("Unexpected configuration", false);
                      }
                    }

                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P_(SIZE); P(EXPECTED); }

                    size_t EXP_COMP = X.key_comp().count();
                    if (CONFIG == 'a') {
                        if (hint != X.begin()) {
                            ++EXP_COMP;
                        }
                        if (hint != X.end()) {
                            ++EXP_COMP;
                        }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT = mX.insert(hint, VALUES[tj]);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *RESULT);

                    if (CONFIG == 'a') {
                        ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                EXP_COMP == X.key_comp().count());
                        ASSERTV(LINE, tj, hint == ++RESULT);
                    }


                    if (expectToAllocate(SIZE + 1)) {
                        ASSERTV(LINE, tj, AA, BB,
                                BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B,
                                B + 1 + TYPE_ALLOC == A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB,
                                BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                    }
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
            }
        }
    }

    if (verbose) printf("\nTesting `insert` with exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj &X = mX;

                for (int tj = 0; tj < LENGTH; ++tj) {
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { P(SIZE); }

                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }

                    CIter hint;
                    switch(CONFIG) {
                      case 'a': {
                          hint = X.lower_bound(VALUES[tj]);
                      } break;
                      case 'b': {
                          hint = X.begin();
                      } break;
                      case 'c': {
                          hint = X.begin();
                          if (hint != X.end()) {
                              ++hint;
                          }
                      } break;
                      case 'd': {
                          hint = X.end();
                          if (hint != X.begin()) {
                              --hint;
                          }
                      } break;
                      case 'e': {
                          hint = X.end();
                      } break;
                      default: {
                          ASSERTV("Unexpected configuration", false);
                      }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                veryVeryVeryVerbose);
                    KeyAllocator         xscratch(&scratch);

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                        RESULT = mX.insert(hint, VALUES[tj]);
                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *RESULT);

                    if (expectToAllocate(SIZE + 1)) {
                        ASSERTV(LINE, tj, AA, BB,
                                BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB,
                                BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                    }
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
            }
        }
    }

}
template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
    //
    // Concerns:
    // 1. `insert` adds an additional element to the object and return the
    //    iterator to the newly added element.
    //
    // 2. Inserting duplicated value do not overwrite previous ones.
    //
    // 3. Duplicated values are inserted at the end of its range.
    //
    // 4. A new element is added to the container if the element did not
    //    already exist, and the order of the container remains correct.
    //
    // 5. Insertion is exception neutral w.r.t. memory allocation.
    //
    // 6. Internal memory management system is hooked up properly
    //    so that *all* internally allocated memory draws from a
    //    user-supplied allocator whenever one is specified.
    //
    // Plan:
    // 1. For insertion we will create objects of varying sizes and capacities
    //    containing default values, and insert a `value`.
    //
    //   1. Compute the number of allocations and verify it is as expected.
    //
    //   2. If the object did not contain `value`, verify it now exist.
    //      Otherwise, verify the return value is as expected.  (C-1..4)
    //
    //   3. Verify all allocations are from the object's allocator.  (C-5)
    //
    // 2. Repeat P-1 under the presence of exception  (C-6)
    //
    // Testing:
    //   iterator insert(const value_type& value);
    // -----------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;    // source line number
        const char *d_spec;    // specification string
        const char *d_unique;  // expected element values
    } DATA[] = {
        //line  spec           isUnique
        //----  --------       --------

        { L_,   "A",           "Y"           },
        { L_,   "AAA",         "YNN"         },
        { L_,   "ABCDEFGH",    "YYYYYYYY"    },
        { L_,   "ABCDEABCDEF", "YYYYYNNNNNY" },
        { L_,   "EEDDCCBBAA",  "YNYNYNYNYN"  }
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    const int MAX_LENGTH = 16;

    if (verbose) printf("\nTesting `insert` without exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == (int)strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P_(IS_UNIQ) P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                Iter RESULT = mX.insert(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                Iter AFTER = RESULT; ++AFTER;
                ASSERTV(LINE, tj, AFTER  == X.upper_bound(VALUES[tj]));
                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, RESULT == X.lower_bound(VALUES[tj]));
                    if (RESULT != X.begin()) {
                        Iter BEFORE = RESULT;  --BEFORE;
                        ASSERTV(LINE, tj, VALUES[tj] != *BEFORE);
                    }
                }
                else{
                    ASSERTV(LINE, tj, X.begin() != RESULT);

                    Iter BEFORE = RESULT;  --BEFORE;
                    ASSERTV(LINE, tj, VALUES[tj] == *BEFORE);
                }

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (expectToAllocate(SIZE + 1)) {
                    ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                    ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                    ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                }
                ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                TestValues exp(EXPECTED);
                ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }
    }

    if (verbose) printf("\nTesting `insert` with injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const int         LENGTH = (int)strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj &X = mX;

            for (int tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { P(SIZE); }

                EXPECTED[SIZE] = SPEC[tj];
                std::sort(EXPECTED, EXPECTED + SIZE + 1);
                EXPECTED[SIZE + 1] = '\0';

                if (veryVeryVerbose) { P(EXPECTED); }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                Iter RESULT;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    RESULT = mX.insert(VALUES[tj]);
                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *RESULT);

                Iter AFTER = RESULT; ++AFTER;
                ASSERTV(LINE, tj, AFTER  == X.upper_bound(VALUES[tj]));

                if (IS_UNIQ) {
                    ASSERTV(LINE, tj, RESULT == X.lower_bound(VALUES[tj]));
                    if (RESULT != X.begin()) {
                        Iter BEFORE = RESULT;  --BEFORE;
                        ASSERTV(LINE, tj, VALUES[tj] != *BEFORE);
                    }
                }
                else{
                    ASSERTV(LINE, tj, X.begin() != RESULT);

                    Iter BEFORE = RESULT;  --BEFORE;
                    ASSERTV(LINE, tj, VALUES[tj] == *BEFORE);
                }

                TestValues exp(EXPECTED);
                ASSERTV(LINE, tj, 0 == verifyContainer(X, exp, SIZE + 1));
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    // Concerns:
    // 1. `begin` and `end` return non-mutable iterators.
    //
    // 2. The range `[begin(), end())` contains all values inserted into the
    //    container in ascending order.
    //
    // 3. The range `[rbegin(), rend())` contains all values inserted into the
    //    container in descending order.
    //
    // 4. `iterator` is a pointer to `const KEY`.
    //
    // 5. `const_iterator` is a pointer to `const KEY`.
    //
    // 6. `reverse_iterator` and `const_reverse_iterator` are implemented by
    //    the (fully-tested) `bslstl_ReverseIterator` over a pointer to
    //    `const TYPE`.
    //
    // Plan:
    // 1. For each value given by variety of specifications of different
    //    lengths:
    //
    //   1. Create an object this value, and access each element in sequence
    //      and in reverse sequence, both as a modifiable reference (setting it
    //      to a default value, then back to its original value, and as a
    //      non-modifiable reference.  (C-1..3)
    //
    // 2. Use `bsl::is_same` to assert the identity of iterator types.
    //    (C-4..6)
    //
    // Testing:
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin();
    //   const_iterator end();
    //   const_reverse_iterator rbegin();
    //   const_reverse_iterator rend();
    // ------------------------------------------------------------------------

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("Testing `iterator`, `begin`, and `end`,"
                        " and `const` variants.\n");
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

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (verbose) { P_(LINE); P(SPEC); }

            size_t i = 0;
            if (ti == 0) {
                ASSERTV(mX.size(), 0 == mX.size());
                ASSERTV(mX.begin() == mX.end());
            }
            for (Iter iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                ASSERTV(LINE, i, VALUES[i] == *iter);
            }
            ASSERTV(LINE, LENGTH == i);

            i = 0;
            for (CIter iter = X.begin(); iter != X.end(); ++iter, ++i) {
                ASSERTV(LINE, i, VALUES[i] == *iter);
            }
            ASSERTV(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("Testing `reverse_iterator`, `rbegin`, and `rend`,"
                        " and `const` variants.\n");
    {
        ASSERTV(1 == (bsl::is_same<RIter,
                                   bsl::reverse_iterator<Iter> >::value));
        ASSERTV(1 == (bsl::is_same<CRIter,
                                   bsl::reverse_iterator<CIter> >::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (verbose) { P_(LINE); P(SPEC); }

            int i = static_cast<int>(LENGTH) - 1;
            for (RIter riter = mX.rbegin(); riter != mX.rend(); ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);

            i = static_cast<int>(LENGTH) - 1;
            for (CRIter riter = X.rbegin(); riter != X.rend(); ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);

            i = static_cast<int>(LENGTH) - 1;
            for (CRIter riter = mX.crbegin(); riter != mX.crend();
                                                                ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase13()
{
    // ------------------------------------------------------------------------
    // SEARCH FUNCTIONS
    //
    // Concern:
    // 1. If the key being searched exists in the container, `find` and
    //    `lower_bound` returns the iterator referring to the existing element,
    //    `contains` returns `true`, and `upper_bound` returns the iterator to
    //    the element after the searched element.
    //
    // 2. If the key being searched does not exists in the container, `find`
    //    returns the `end` iterator, `contains` returns `false`, `lower_bound`
    //    and `upper_bound` return the iterator to the smallest element
    //    greater than searched element.
    //
    // 3. `equal_range(key)` returns
    //    `std::make_pair(lower_bound(key), upper_bound(key))`.
    //
    // 2. `count` returns the number of elements with the same value as defined
    //    by the comparator.
    //
    // 3. Both the `const` and non-`const` versions returns the same value.
    //
    // 4. No memory is allocated.
    //
    // Plan:
    // 1. Use a loop-based approach for different lengths:
    //
    //   1. Create an object for each length using values where every
    //      consecutive values have at least 1 value that is between those two
    //      values.
    //
    //   2. Use run all search functions on all values in the container and
    //      values between each consecutive values in the container.
    //
    //   3. Verify expected result is returned.  (C-1..3)
    //
    //   4. Verify no memory is allocated from any allocators.  (C-4)
    //
    // Testing:
    //   bool contains(const key_type& key);
    //   bool contains(const LOOKUP_KEY& key);
    //   iterator find(const key_type& key);
    //   const_iterator find(const key_type& key) const;
    //   size_type count(const key_type& key) const;
    //   iterator lower_bound(const key_type& key);
    //   const_iterator lower_bound(const key_type& key) const;
    //   iterator upper_bound(const key_type& key);
    //   const_iterator upper_bound(const key_type& key) const;
    //   pair<iterator, iterator> equal_range(const key_type& key);
    //   pair<const_iter, const_iter> equal_range(const key_type&) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;  // contains 52 distinct increasing values

    const int MAX_LENGTH = 17;

    if (verbose) printf("\nTesting various search methods.\n");
    {
        for (int ti = 0; ti < MAX_LENGTH; ++ti) {
            const int LENGTH = ti;

            CIter CITER[MAX_LENGTH];
            Iter  ITER[MAX_LENGTH];

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator oa("object", veryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);  const Obj& X = mX;

            for (int i = 0; i < LENGTH; ++i) {
                int idx = 2 * i + 1;
                Iter RESULT = mX.insert(VALUES[idx]);

                ASSERTV(ti, i, VALUES[idx] == *RESULT);

                CITER[i] = RESULT;
                ITER[i] = RESULT;
            }
            CITER[LENGTH] = X.end();
            ITER[LENGTH] = mX.end();
            ASSERTV(ti, LENGTH == (int)X.size());

            for (size_t NUM_REPEATS = 0; NUM_REPEATS < 3; ++NUM_REPEATS) {
                bslma::TestAllocatorMonitor oam(&oa);

                for (int tj = 0; tj <= 2 * LENGTH; ++tj) {
                    if (1 == tj % 2) {
                        const int idx = tj / 2;
                        ASSERTV(ti, tj, CITER[idx] == X.find(VALUES[tj]));
                        ASSERTV(ti, tj, ITER[idx] == mX.find(VALUES[tj]));

                        bool cShouldBeFound = CITER[idx] != X.end();
                        ASSERTV(ti,
                                tj,
                                cShouldBeFound,
                                cShouldBeFound == X.contains(VALUES[tj]));

                        bool shouldBeFound = ITER[idx] != mX.end();
                        ASSERTV(cShouldBeFound,
                                shouldBeFound,
                                cShouldBeFound == shouldBeFound);

                        ASSERTV(ti,
                                tj,
                                shouldBeFound,
                                shouldBeFound == mX.contains(VALUES[tj]));

                        ASSERTV(ti, tj,
                                CITER[idx] == X.lower_bound(VALUES[tj]));
                        ASSERTV(ti, tj,
                                ITER[idx] == mX.lower_bound(VALUES[tj]));
                        ASSERTV(ti, tj,
                                CITER[idx + 1] == X.upper_bound(VALUES[tj]));
                        ASSERTV(ti, tj,
                                ITER[idx + 1] == mX.upper_bound(VALUES[tj]));

                        pair<CIter, CIter> R1 = X.equal_range(VALUES[tj]);
                        ASSERTV(ti, tj, CITER[idx] == R1.first);
                        ASSERTV(ti, tj, CITER[idx + 1] == R1.second);

                        pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj]);
                        ASSERTV(ti, tj, ITER[idx] == R2.first);
                        ASSERTV(ti, tj, ITER[idx + 1] == R2.second);

                        ASSERTV(ti, tj,
                                NUM_REPEATS + 1 == mX.count(VALUES[tj]));
                    }
                    else {
                        const int idx = tj / 2;
                        ASSERTV(ti, tj, X.end() == X.find(VALUES[tj]));
                        ASSERTV(ti, tj, mX.end() == mX.find(VALUES[tj]));
                        ASSERTV(ti, tj,
                                CITER[idx] == X.lower_bound(VALUES[tj]));
                        ASSERTV(ti, tj,
                                ITER[idx] == mX.lower_bound(VALUES[tj]));
                        ASSERTV(ti, tj,
                                CITER[idx] == X.upper_bound(VALUES[tj]));
                        ASSERTV(ti, tj,
                                ITER[idx] == mX.upper_bound(VALUES[tj]));

                        pair<CIter, CIter> R1 = X.equal_range(VALUES[tj]);
                        ASSERTV(ti, tj, CITER[idx] == R1.first);
                        ASSERTV(ti, tj, CITER[idx] == R1.second);

                        pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj]);
                        ASSERTV(ti, tj, ITER[idx] == R2.first);
                        ASSERTV(ti, tj, ITER[idx] == R2.second);

                        ASSERTV(ti, tj, 0 == mX.count(VALUES[tj]));
                    }
                }

                ASSERTV(ti, oam.isTotalSame());
                ASSERTV(ti, da.numBlocksInUse(), 0 == da.numBlocksInUse());

                for (int i = 0; i < LENGTH; ++i) {
                    int idx = 2 * i + 1;
                    Iter RESULT = mX.insert(VALUES[idx]);

                    ASSERTV(ti, i, VALUES[idx] == *RESULT);
                }
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // RANGE (TEMPLATE) CONSTRUCTORS:
    //
    // Concern:
    // 1. All values within the range [first, last) are inserted.
    //
    // 2. Each iterator is deferenced only once.
    //
    // 3. Repeated values inserted properly.
    //
    // 4. If an allocator is NOT supplied to the value constructor, the
    //    default allocator in effect at the time of construction becomes
    //    the object allocator for the resulting object.
    //
    // 5. If an allocator IS supplied to the value constructor, that
    //    allocator becomes the object allocator for the resulting object.
    //
    // 6. Supplying a null allocator address has the same effect as not
    //    supplying an allocator.
    //
    // 7. Supplying an allocator to the value constructor has no effect
    //    on subsequent object values.
    //
    // 8. Constructing from an ordered list requires linear time.
    //
    // 9. Any memory allocation is from the object allocator.
    //
    // 10. There is no temporary memory allocation from any allocator.
    //
    // 11. Every object releases any allocated memory at destruction.
    //
    // 12. QoI: Creating an object having the default-constructed value
    //    allocates no memory.
    //
    // 13. Any memory allocation is exception neutral.
    //
    // 14. QoI: Range insertion allocates a single block for nodes when the
    //    number of elements can be determined and no free nodes are available.
    //    (The contained elements may require additional allocations.)
    //
    // Plan:
    // 1. Using the table-driven technique:
    //
    //   1. Specify a set of (unique) valid object values.
    //
    // 2. For each row (representing a distinct object value, `V`) in the table
    //    described in P-1:
    //
    //   1. Execute an inner loop creating three distinct objects, in turn,
    //      each object having the same value, `V`, but configured differently:
    //      (a) without passing an allocator, (b) passing a null allocator
    //      address explicitly, and (c) passing the address of a test allocator
    //      distinct from the default allocator.
    //
    //   2. For each of the three iterations in P-2.1:
    //
    //     1. Insert the test data to a specialized container that have returns
    //        standard conforming input iterators.
    //
    //     2. Create three `bslma::TestAllocator` objects, and install one as
    //        the current default allocator (note that a ubiquitous test
    //        allocator is already installed as the global allocator).
    //
    //     3. Use the value constructor to dynamically create an object using
    //        `begin` and `end` from the container in P-2.1, with its object
    //        allocator configured appropriately (see P-2.2), supplying all the
    //        arguments as `const`; use a distinct test allocator for the
    //        object's footprint.
    //
    //     4. Verify that all of the attributes of each object have their
    //        expected values.
    //
    //     5. Use the `allocator` accessor of each underlying attribute capable
    //        of allocating memory to ensure that its object allocator is
    //        properly installed; also invoke the (as yet unproven) `allocator`
    //        accessor of the object under test.
    //
    //     6. Use the appropriate test allocators to verify that:
    //
    //       1. An object that IS expected to allocate memory does so from the
    //          object allocator only (irrespective of the specific number of
    //          allocations or the total amount of memory allocated).
    //
    //       2. An object that is expected NOT to allocate memory does not
    //          allocate memory.
    //
    //       3. If an allocator was supplied at construction (P-2.1c), the
    //          default allocator doesn't allocate any memory.
    //
    //       4. If the input range is ordered, verify the number of comparisons
    //          is equal to `LENGTH - 1`, where `LENGTH` is the number of
    //          elements in the input range.
    //
    //       5. No temporary memory is allocated from the object allocator.
    //
    //       6. All object memory is released when the object is destroyed.
    //
    // 3. Repeat the steps in P-2 for the supplied allocator configuration
    //    (P-2.1c) on the data of P-1, but this time create the object as an
    //    automatic variable in the presence of injected exceptions (using the
    //    `BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*` macros); represent any string
    //    arguments in terms of `string` using a "scratch" allocator.
    //
    // 4. Invoke the `testRangeCtorOptimization` function that creates a
    //    containers for a non-allocating type using both random access and
    //    forward iterators.  Each container is given a test allocator to
    //    supply memory.  The test allocator is state compared to the expected
    //    state in each step of the scenario.  (C-14)
    //
    // Testing:
    //   multiset(ITER first, ITER last, const A& alloc);
    //   multiset(ITER first, ITER last, const C& comp, const A& alloc);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting `%s` (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    static const struct {
        int         d_line;         // source line number
        const char *d_spec;         // specification string
        const char *d_results;      // expected element values
        bool        d_orderedFlag;  // is the spec in ascending order
    } DATA[] = {
        //line  spec          elements      ordered
        //----  -----------   ------------  -------
        { L_,   "",           "",           true   },
        { L_,   "A",          "A",          true   },
        { L_,   "AB",         "AB",         true   },
        { L_,   "ABC",        "ABC",        true   },
        { L_,   "ABCD",       "ABCD",       true   },
        { L_,   "ABCDE",      "ABCDE",      true   },
        { L_,   "DABEC",      "ABCDE",      false  },
        { L_,   "EDCBACBA",   "AABBCCDE",   false  },
        { L_,   "ABCDEABCD",  "AABBCCDDE",  false  }
    };
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("\nTesting without injected exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const size_t      LENGTH  = strlen(DATA[ti].d_results);
            const bool        ORDERED = DATA[ti].d_orderedFlag;
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ T_ P(CONFIG) }

                TestValues CONT(SPEC);
                typename TestValues::iterator BEGIN = CONT.begin();
                typename TestValues::iterator END   = CONT.end();

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                KeyAllocator         xsa(&sa);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(BEGIN, END);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(BEGIN, END, ALLOC(0));
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(BEGIN, END, COMP(), ALLOC(0));
                      objAllocatorPtr = &da;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(BEGIN, END, xsa);
                      objAllocatorPtr = &sa;
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(BEGIN, END, COMP(), xsa);
                      objAllocatorPtr = &sa;
                  } break;
                  default: {
                      ASSERTV(LINE, CONFIG, "Bad allocator config.", false);
                  } return;                                           // RETURN
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' >= CONFIG ? sa : da;

                // Use untested functionality to help ensure the first row
                // of the table contains the default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(LINE, CONFIG, Obj(), *objPtr, Obj() == *objPtr);
                    firstFlag = false;
                }

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                if (ORDERED && LENGTH > 0) {
                    ASSERTV(LINE, CONFIG, LENGTH - 1, X.key_comp().count(),
                            LENGTH - 1 == X.key_comp().count());
                }
                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                             0 == noa.numBlocksTotal());

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(LINE, CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());

            }  // end foreach configuration
        }  // end foreach row
    }
    if (verbose) printf("\nTesting with injected exceptions.\n");
    {
        // Note that any string arguments are now of type `string`, which
        // require their own "scratch" allocator.

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_line;
            const char        *SPEC   = DATA[ti].d_spec;
            const size_t       LENGTH = strlen(DATA[ti].d_results);
            const TestValues   EXP(DATA[ti].d_results);

            TestValues CONT(SPEC);
            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator END   = CONT.end();

            if (verbose) { P_(LINE) P(SPEC) P_(LENGTH); }

            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            KeyAllocator         xsa(&sa);

            bslma::DefaultAllocatorGuard dag(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                CONT.resetIterators();

                Obj mX(BEGIN, END, COMP(), xsa);
                ASSERTV(LINE, mX, 0 == verifyContainer(mX, EXP, LENGTH));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
    }

}

                       // =====================
                       // struct MetaTestDriver
                       // =====================

/// This `struct` is to be call by the `RUN_EACH_TYPE` macro, and the
/// functions within it dispatch to functions in `TestDriver` instantiated
/// with different types of allocator.
template <class KEY,
          class COMP = TestComparator<KEY> >
struct MetaTestDriver {

    typedef bsl::allocator<KEY>             BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    /// Test move-assign.
    static void testCase27();
};

template <class KEY, class COMP>
void MetaTestDriver<KEY, COMP>::testCase27()
{
    // The low-order bit of the identifier specifies whether the fourth boolean
    // argument of the stateful allocator, which indicates propagate on move
    // assign, is set.

    typedef bsltf::StdStatefulAllocator<KEY, false, false, false, false> A00;
    typedef bsltf::StdStatefulAllocator<KEY, false, false, false, true>  A01;
    typedef bsltf::StdStatefulAllocator<KEY, true,  true,  true,  false> A10;
    typedef bsltf::StdStatefulAllocator<KEY, true,  true,  true,  true>  A11;

    if (verbose) printf("\n");

    TestDriver<KEY, COMP, BAP>::testCase27_dispatch();

    TestDriver<KEY, COMP, SAA>::testCase27_dispatch();

    TestDriver<KEY, COMP, A00>::testCase27_dispatch();
    TestDriver<KEY, COMP, A01>::testCase27_dispatch();
    TestDriver<KEY, COMP, A10>::testCase27_dispatch();
    TestDriver<KEY, COMP, A11>::testCase27_dispatch();
}

// ============================================================================
//                      RANGE CTOR/INSERT OPTIMIZATION
// ----------------------------------------------------------------------------

typedef int                DataType;
typedef bsl::multiset<int> ContainerType;

static DataType  ARRAY[] = { DataType( 0)
                           , DataType( 1)
                           , DataType( 2)
                           , DataType( 3)
                           , DataType( 4)
                           , DataType( 5)
                           , DataType( 6)
                           , DataType( 7)
                           , DataType( 8)
                           , DataType( 9)
                           , DataType(10)
                           , DataType(11)
                           , DataType(12)
                           , DataType(13)
                           , DataType(14)
                           , DataType(15)
                           , DataType(16)
                           , DataType(17)
                           , DataType(18)
                           , DataType(19)
                           , DataType(20)
                           , DataType(21)
                           , DataType(22)
                           , DataType(23)
                           , DataType(24)
                           , DataType(25)
                           , DataType(26)
                           , DataType(27)
                           , DataType(28)
                           , DataType(29)
                           , DataType(30)
                           , DataType(31)
                           , DataType(32)
                           , DataType(33)
                           , DataType(34)
                           , DataType(35)
                           , DataType(36)
                           , DataType(37)
                           , DataType(38)
                           , DataType(39)
                           };
const std::size_t NUM_ELEMENTS = sizeof ARRAY / sizeof *ARRAY;

typedef bslstl::ForwardIterator<     DataType, DataType *> FwdItr;
typedef bslstl::RandomAccessIterator<DataType, DataType *> RndItr;

static void testRangeCtorOptimization()
{
   if (verbose) printf("\nTest Range CTOR Optimization\n");

    FwdItr beginFwd(ARRAY), endFwd(ARRAY + NUM_ELEMENTS);
    RndItr beginRnd(ARRAY), endRnd(ARRAY + NUM_ELEMENTS);

    bslma::TestAllocator        sa("scratch", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor sam(&sa);

    if (verbose) {
        printf("\nAfore: Object-Allocator\n");
        sa.print();
    }

    ContainerType mXF(beginFwd, endFwd, &sa); const ContainerType& XF = mXF;
    ASSERT(1            == sam.numBlocksTotalChange());
    ASSERT(NUM_ELEMENTS == XF.size());

    ContainerType mXR(beginRnd, endRnd, &sa); const ContainerType& XR = mXR;
    ASSERT(2            == sam.numBlocksTotalChange());
    ASSERT(NUM_ELEMENTS == XR.size());

    if (verbose) {
         P(XF.size());
        P_(XR.size());
        printf("\nAfter: Object-Allocator\n");
        sa.print();
    }

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) {
        printf("\nAfore: Default-Allocator\n");
        da.print();
    }

    ContainerType mYF(beginFwd, endFwd, &sa); const ContainerType& YF = mYF;
    ASSERT(3            == sam.numBlocksTotalChange());
    ASSERT(NUM_ELEMENTS == YF.size());

    ContainerType mYR(beginRnd, endRnd, &sa); const ContainerType& YR = mYR;
    ASSERT(4            == sam.numBlocksTotalChange());
    ASSERT(NUM_ELEMENTS == YR.size());

    if (verbose) {
         P(YF.size());
        P_(YR.size());
        printf("\nAfter: Default-Allocator\n");
        da.print();
    }
}

static void testRangeInsertOptimization()
{
   if (verbose) printf("\nTest Range Insert Optimization\n");

    FwdItr beginFwd(ARRAY);
    FwdItr   midFwd(ARRAY + NUM_ELEMENTS/2);
    FwdItr   endFwd(ARRAY + NUM_ELEMENTS);

    RndItr beginRnd(ARRAY);
    RndItr   midRnd(ARRAY + NUM_ELEMENTS/2);
    RndItr   endRnd(ARRAY + NUM_ELEMENTS);

    bslma::TestAllocator        sa("scratch", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor sam(&sa);

    if (verbose) {
        printf("\nAfore: Object-Allocator\n");
        sa.print();
    }

    ContainerType mX(&sa); const ContainerType& X = mX;
    ASSERT(0            == X.size());
    ASSERT(0            == sam.numBlocksTotalChange());

    mX.insert(beginFwd, endFwd);            // Insert entire range.
    ASSERT(NUM_ELEMENTS     == X.size());   // Added elements.
    ASSERT(1                == sam.numBlocksTotalChange());
                                            // Had to allocate nodes.
                                            // No free nodes left.

    mX.insert(beginFwd, endFwd);           // Re-insert entire range.
    ASSERT(NUM_ELEMENTS * 2 == X.size());  // Duplicates allowed.
    ASSERT(2                == sam.numBlocksTotalChange());
                                           // No free nodes so allocated more.
                                           // No free nodes left.

    mX.insert(beginFwd, endFwd);           // Re-re-insert entire range.
    ASSERT(NUM_ELEMENTS * 3 == X.size());  // Duplciates allowed.
    ASSERT(3                == sam.numBlocksTotalChange());
                                           // No free nodes so allocated more.
                                           // No free nodes left.

    mX.insert(beginFwd, endFwd);           // Re-re-re-insert entire range.
    ASSERT(NUM_ELEMENTS * 4 == X.size());  // Duplicates allowed.
    ASSERT(4                == sam.numBlocksTotalChange());
                                           // No free nodes so allocated more.
                                           // No free nodes left.
    // ...

    ContainerType mY(&sa); const ContainerType& Y = mY;

    bslma::TestAllocatorMonitor sam2(&sa);

    ASSERT(0              == sam2.numBlocksInUseChange());

    mY.insert(beginFwd, midFwd);        // Insert first half of `DATA`.
    ASSERT(NUM_ELEMENTS/2 == Y.size());
    ASSERT(1              == sam2.numBlocksInUseChange());

    mY.clear();                         // Clear
    ASSERT(0              == Y.size());
    ASSERT(1              == sam2.numBlocksInUseChange());

    mY.insert(beginFwd, midFwd);       // Re-insert previous elements
    ASSERT(NUM_ELEMENTS/2 == Y.size());
    ASSERT(1              == sam2.numBlocksInUseChange());
                                      // Reused nodes.  No new allocation.
                                      // No free nodes left.

    mY.insert(midRnd, endRnd);        // Insert additional elements
    ASSERT(NUM_ELEMENTS   == Y.size());
    ASSERT(2              == sam2.numBlocksInUseChange());
                                      // Allocated more nodes.

    if (verbose) {
        P(X.size());
        printf("\nAfter: Object-Allocator\n");
        sa.print();
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
/// This struct provides a namespace for functions testing deduction guides.
/// The tests are compile-time only; it is not necessary that these routines
/// be called at run-time.  Note that the following constructors do not have
/// associated deduction guides because the template parameters for
/// `bsl::multiset` cannot be deduced from the constructor parameters.
/// ```
/// multiset()
/// explicit multiset(COMPARATOR, ALLOCATOR = ALLOCATOR());
/// multiset(ALLOCATOR)
/// ```
struct TestDeductionGuides {

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    /// Test that constructing a `bsl::priority_queue` from various
    /// combinations of arguments deduces the correct type.
    /// ```
    /// multiset(const multiset&  ms)            -> decltype(ms)
    /// multiset(const multiset&  ms, ALLOCATOR) -> decltype(ms)
    /// multiset(      multiset&& ms)            -> decltype(ms)
    /// multiset(      multiset&& ms, ALLOCATOR) -> decltype(ms)
    ///
    /// multiset(Iter, Iter, COMPARATOR = COMPARATOR(),
    ///                                             ALLOCATOR = ALLOCATOR())
    /// multiset(Iter, Iter, ALLOCATOR)
    ///
    /// multiset(initializer_list, COMPARATOR = COMPARATOR(),
    ///                                             ALLOCATOR = ALLOCATOR())
    /// multiset(initializer_list, ALLOCATOR)
    /// ```
    void SimpleConstructors ()
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::multiset<T1> ms1;
        bsl::multiset     ms1a(ms1);
        ASSERT_SAME_TYPE(decltype(ms1a), bsl::multiset<T1>);

        typedef float T2;
        bsl::multiset<T2> ms2;
        bsl::multiset     ms2a(ms2, bsl::allocator<T2>());
        bsl::multiset     ms2b(ms2, a1);
        bsl::multiset     ms2c(ms2, a2);
        bsl::multiset     ms2d(ms2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(ms2a), bsl::multiset<T2>);
        ASSERT_SAME_TYPE(decltype(ms2b), bsl::multiset<T2>);
        ASSERT_SAME_TYPE(decltype(ms2c), bsl::multiset<T2>);
        ASSERT_SAME_TYPE(decltype(ms2d), bsl::multiset<T2>);

        typedef short T3;
        bsl::multiset<T3> ms3;
        bsl::multiset     ms3a(std::move(ms3));
        ASSERT_SAME_TYPE(decltype(ms3a), bsl::multiset<T3>);

        typedef long double T4;
        bsl::multiset<T4> ms4;
        bsl::multiset     ms4a(std::move(ms4), bsl::allocator<T4>{});
        bsl::multiset     ms4b(std::move(ms4), a1);
        bsl::multiset     ms4c(std::move(ms4), a2);
        bsl::multiset     ms4d(std::move(ms4), bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(ms4a), bsl::multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ms4b), bsl::multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ms4c), bsl::multiset<T4>);
        ASSERT_SAME_TYPE(decltype(ms4d), bsl::multiset<T4>);


        typedef long T5;
        typedef std::greater<T5> CompT5;
        T5                          *p5b = nullptr;
        T5                          *p5e = nullptr;
        bsl::multiset<T5>::iterator  i5b;
        bsl::multiset<T5>::iterator  i5e;

        bsl::multiset ms5a(p5b, p5e);
        bsl::multiset ms5b(i5b, i5e);
        bsl::multiset ms5c(p5b, p5e, CompT5{});
        bsl::multiset ms5d(i5b, i5e, CompT5{});
        bsl::multiset ms5e(p5b, p5e, CompT5{}, bsl::allocator<T5>{});
        bsl::multiset ms5f(p5b, p5e, CompT5{}, a1);
        bsl::multiset ms5g(p5b, p5e, CompT5{}, a2);
        bsl::multiset ms5h(p5b, p5e, CompT5{}, std::allocator<T5>{});
        bsl::multiset ms5i(i5b, i5e, CompT5{}, bsl::allocator<T5>{});
        bsl::multiset ms5j(i5b, i5e, CompT5{}, a1);
        bsl::multiset ms5k(i5b, i5e, CompT5{}, a2);
        bsl::multiset ms5l(i5b, i5e, CompT5{}, std::allocator<T5>{});

        ASSERT_SAME_TYPE(decltype(ms5a), bsl::multiset<T5>);
        ASSERT_SAME_TYPE(decltype(ms5b), bsl::multiset<T5>);
        ASSERT_SAME_TYPE(decltype(ms5c), bsl::multiset<T5, CompT5>);
        ASSERT_SAME_TYPE(decltype(ms5d), bsl::multiset<T5, CompT5>);
        ASSERT_SAME_TYPE(decltype(ms5e),
                                bsl::multiset<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5f),
                                bsl::multiset<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5g),
                                bsl::multiset<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5h),
                                bsl::multiset<T5, CompT5, std::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5i),
                                bsl::multiset<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5j),
                                bsl::multiset<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5k),
                                bsl::multiset<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(ms5l),
                                bsl::multiset<T5, CompT5, std::allocator<T5>>);


        typedef short T6;
        T6                          *p6b = nullptr;
        T6                          *p6e = nullptr;
        bsl::multiset<T6>::iterator  i6b;
        bsl::multiset<T6>::iterator  i6e;

        bsl::multiset ms6a(p6b, p6e, bsl::allocator<T6>{});
        bsl::multiset ms6b(p6b, p6e, a1);
        bsl::multiset ms6c(p6b, p6e, a2);
        bsl::multiset ms6d(p6b, p6e, std::allocator<T6>{});

        ASSERT_SAME_TYPE(decltype(ms6a),
                         bsl::multiset<T6, std::less<T6>, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(ms6b),
                         bsl::multiset<T6, std::less<T6>, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(ms6c),
                         bsl::multiset<T6, std::less<T6>, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(ms6d),
                         bsl::multiset<T6, std::less<T6>, std::allocator<T6>>);


        typedef long T7;
        typedef std::greater<T7> CompT7;
        std::initializer_list<T7> il7{1L, 2L, 3L};

        bsl::multiset ms7a(il7);
        bsl::multiset ms7b(il7, CompT7{});
        bsl::multiset ms7c(il7, CompT7{}, bsl::allocator<T7>{});
        bsl::multiset ms7d(il7, CompT7{}, a1);
        bsl::multiset ms7e(il7, CompT7{}, a2);
        bsl::multiset ms7f(il7, CompT7{}, std::allocator<T7>{});

        ASSERT_SAME_TYPE(decltype(ms7a), bsl::multiset<T7>);
        ASSERT_SAME_TYPE(decltype(ms7b), bsl::multiset<T7, CompT7>);
        ASSERT_SAME_TYPE(decltype(ms7c),
                                bsl::multiset<T7, CompT7, bsl::allocator<T7>>);
        ASSERT_SAME_TYPE(decltype(ms7d),
                                bsl::multiset<T7, CompT7, bsl::allocator<T7>>);
        ASSERT_SAME_TYPE(decltype(ms7e),
                                bsl::multiset<T7, CompT7, bsl::allocator<T7>>);
        ASSERT_SAME_TYPE(decltype(ms7f),
                                bsl::multiset<T7, CompT7, std::allocator<T7>>);

        typedef long long T8;
        std::initializer_list<T8> il8{3LL, 2LL, 1LL};

        bsl::multiset ms8a(il8, bsl::allocator<T8>{});
        bsl::multiset ms8b(il8, a1);
        bsl::multiset ms8c(il8, a2);
        bsl::multiset ms8d(il8, std::allocator<T8>{});

        ASSERT_SAME_TYPE(decltype(ms8a),
                         bsl::multiset<T8, std::less<T8>, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(ms8b),
                         bsl::multiset<T8, std::less<T8>, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(ms8c),
                         bsl::multiset<T8, std::less<T8>, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(ms8d),
                         bsl::multiset<T8, std::less<T8>, std::allocator<T8>>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_MULTISET_COMPILE_FAIL_SWAPPED_COMPARATOR_AND_ALLOCATOR
#ifdef  BSLSTL_MULTISET_COMPILE_FAIL_SWAPPED_COMPARATOR_AND_ALLOCATOR
        typedef long T99;
        T99           *p99 = nullptr;
        bsl::multiset  s99(p99, p99, bsl::allocator<T99>{}, std::less<T99>{});
        // This should fail to compile (allocator and comparator are swapped)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

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

    // Test integrity of DATA

    {
        const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (unsigned ii = 0; ii < NUM_DATA; ++ii) {
            ASSERT(!ii || DATA[ii-1].d_index <= DATA[ii].d_index);

            for (unsigned jj = 0; jj < NUM_DATA; ++jj) {
                ASSERT((DATA[ii].d_index == DATA[jj].d_index) ==
                         !std::strcmp(DATA[ii].d_results, DATA[jj].d_results));
            }
        }
    }

    switch (test) { case 0:
      case 37: {
        // --------------------------------------------------------------------
        // CONCERN: `multiset` IS A C++20 RANGE
        //
        // Concerns:
        // 1. `multiset` models `ranges::common_range` concept.
        //
        // 2. `multiset` models `ranges::bidirectional_range` concept.
        //
        // 3. `multiset` models `ranges::sized_range` concept.
        //
        // 4. `multiset` models `ranges::viewable_range` concept.
        //
        // 5. `multiset` doesn't model `ranges::view` concept.
        //
        // 6. `multiset` doesn't model `ranges::borrowed_range` concept.
        //
        // Plan:
        // 1. `static_assert` every above-mentioned concept for different `T`.
        //
        // Testing:
        //   CONCERN: `multiset` IS A C++20 RANGE
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONCERN: `multiset` IS A C++20 RANGE"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase37_isRange,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);
      } break;
      case 36: {
        // --------------------------------------------------------------------
        // TESTING ERASE_IF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTION `BSL::ERASE_IF`"
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
        // 1. Construction from iterators deduces the value type from the value
        //    type of the iterator.
        //
        // 2. Construction with a `bslma::Allocator *` deduces the correct
        //    specialization of `bsl::allocator` for the type of the allocator.
        //
        // Plan:
        // 1. Create a multiset by invoking the constructor without supplying
        //    the template arguments explicitly.
        //
        // 2. Verify that the deduced type is correct.
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
        (void) test; // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        // 1. `multiset` has does not have a transparent set of lookup
        //    functions if the comparator is not transparent.
        //
        // 2. `multiset` has a transparent set of lookup functions if the
        //    comparator is transparent.
        //
        // Plan:
        // 1. Construct a non-transparent multiset and call the lookup
        //    functions with a type that is convertible to the `value_type`.
        //    There should be exactly one conversion per call to a lookup
        //    function.  (C-1)
        //
        // 2. Construct a transparent multiset and call the lookup functions
        //    with a type that is convertible to the `value_type`.  There
        //    should be no conversions.  (C-2)
        //
        // Testing:
        //   CONCERN: `find`        properly handles transparent comparators.
        //   CONCERN: `count`       properly handles transparent comparators.
        //   CONCERN: `lower_bound` properly handles transparent comparators.
        //   CONCERN: `upper_bound` properly handles transparent comparators.
        //   CONCERN: `equal_range` properly handles transparent comparators.
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::multiset<int>                      NonTransparentMultiset;
        typedef bsl::multiset<int, TransparentComparator>
                                                        TransparentMultiset;

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
            const int VALUE = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a value of %d\n",
                       VALUE);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent multiset.\n");
            }
            testTransparentComparator( XNT, false, VALUE);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent multiset.\n");
            }
            testTransparentComparator(mXNT, false, VALUE);

            if (veryVerbose) {
                printf("\tTesting const transparent multiset.\n");
            }
            testTransparentComparator( XT,  true,  VALUE);

            if (veryVerbose) {
                printf("\tTesting mutable transparent multiset.\n");
            }
            testTransparentComparator(mXT,  true,  VALUE);
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // `noexcept` SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "`noexcept` SPECIFICATION" "\n"
                                 "========================" "\n");

        typedef bsltf::StdStatefulAllocator
                           <int,    // TYPE
                            false,  // PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION
                            false,  // PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT
                            false,  // PROPAGATE_ON_CONTAINER_SWAP
                            false,  // PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT
                            false   // IS_ALWAYS_EQUAL
                           > AllFalseAlloc;

        typedef bsltf::StdStatefulAllocator
                           <int,    // TYPE
                            false,  // PROPAGATE_ON_CONTAINER_COPY_CONSTRUCTION
                            false,  // PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT
                            false,  // PROPAGATE_ON_CONTAINER_SWAP
                            false,  // PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT
                            true    // IS_ALWAYS_EQUAL
                           > AlwaysEqualAlloc;

        typedef TestComparator<int>     NonThrowingComparator;
        typedef ThrowingComparator<int> ThrowingComparator;

        TestDriver<int>::testCase33();

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        ASSERT( std::is_nothrow_move_assignable<NonThrowingComparator>::value);
        ASSERT(!std::is_nothrow_move_assignable<   ThrowingComparator>::value);
#endif
#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
        ASSERT( bsl::is_nothrow_swappable<NonThrowingComparator>::value);
#endif
        ASSERT(!bsl::is_nothrow_swappable<   ThrowingComparator>::value);
#endif

        TestDriver<int, ThrowingComparator,    AllFalseAlloc   >::testCase33();
        TestDriver<int, ThrowingComparator,    AlwaysEqualAlloc>::testCase33();
        TestDriver<int, NonThrowingComparator, AllFalseAlloc   >::testCase33();
        TestDriver<int, NonThrowingComparator, AlwaysEqualAlloc>::testCase33();

      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZER LIST FUNCTIONS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase32,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE WITH HINT
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase31,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType);
        RUN_EACH_TYPE(TestDriver,
                      testCase31a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase31,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING EMPLACE
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase30,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonDefaultConstructibleTestType);
        RUN_EACH_TYPE(TestDriver,
                      testCase30a,
                      bsltf::EmplacableTestType,
                      bsltf::AllocEmplacableTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase30,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING INSERTION WITH HINT MOVABLE VALUES
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase29,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING INSERTION ON MOVABLE VALUES
        // --------------------------------------------------------------------
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
        // TESTING MOVE ASSIGNMENT
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTION
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase26,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase26,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING STANDARD INTERFACE COVERAGE
        // --------------------------------------------------------------------
        // Test only `int` and `char` parameter types, because multiset's
        // `operator<` and related operators only support parameterized types
        // that define `operator<`.
        RUN_EACH_TYPE(TestDriver, testCase25, int, char);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR OF TEMPLATE WRAPPER
        // --------------------------------------------------------------------
        // KEY doesn't affect the test.  So run test only for `int`.
        TestDriver<int>::testCase24();
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
        RUN_EACH_TYPE(StdAllocTestDriver,
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
        // TESTING `max_size` and `empty`
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
        // TESTING `erase`
        // --------------------------------------------------------------------
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
        // TESTING RANGE `insert`
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase17,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        testRangeInsertOptimization();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING `insert` WITH HINT
        // --------------------------------------------------------------------
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
        // TESTING `insert`
        // --------------------------------------------------------------------
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
        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING `find`, `contains`
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RANGE (TEMPLATE) CONSTRUCTORS:
        // --------------------------------------------------------------------

        if (verbose) printf("\nRANGE (TEMPLATE) CONSTRUCTORS"
                            "\n=============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase12,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        testRangeCtorOptimization();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION `g`
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting `g`"
                            "\n===========\n");
        if (verbose) printf("\nThis test has been disabled.\n");
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
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase9,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // `propagate_on_container_copy_assignment` testing

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_copy_assignment,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
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
// Copyright 2020 Bloomberg Finance L.P.
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
