// bslstl_set_test.t.cpp                                              -*-C++-*-
#include <bslstl_set_test.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>
#include <bslstl_map.h>
#include <bslstl_pair.h>
#include <bslstl_randomaccessiterator.h>
#include <bslstl_set.h>
#include <bslstl_string.h>

#include <bslalg_rangecompare.h>

#include <bslma_default.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>
#include <bslmf_haspointersemantics.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsltf_allocargumenttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <algorithm>
#include <functional>
#include <utility>      // move

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>      // atoi
#include <string.h>      // strlen

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 2 parts, 'bslstl_set.t.cpp' (cases 1-11, plus the usage example), and
// 'bslstl_set_test.cpp' (cases 12 and higher).
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
// 23.4.6.2, construct/copy/destroy:
// [19] set(const C& comparator, const A& allocator);
// [12] set(ITER first, ITER last, const C& comparator, const A& allocator);
// [12] set(ITER first, ITER last, const A& allocator);
// [32] set(initializer_list<value_type>, const C& comp, const A& allocator);
// [32] set(initializer_list<value_type>, const A& allocator);
// [ 7] set(const set& original);
// [26] set(set&& original);
// [ 2] explicit set(const A& allocator);
// [ 7] set(const set& original, const A& allocator);
// [26] set(set&&, const ALLOCATOR&);
// [ 2] ~set();
// [ 9] set& operator=(const set& rhs);
// [27] set& operator=(set&& x);
// [32] set& operator=(initializer_list<value_type>);
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
// [18] bool empty() const;
// [ 4] size_type size() const;
// [18] size_type max_size() const;
//
// modifiers:
// [15] bsl::pair<iterator, bool> insert(const value_type& value);
// [28] bsl::pair<iterator, bool> insert(value_type&& value);
// [15] iterator insert(const_iterator position, const value_type& value);
// [29] iterator insert(const_iterator position, value_type&& value);
// [15] void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [32] void insert(initializer_list<value_type>);
//
// [30] pair<iterator, bool> emplace(Args&&... args);
// [31] iterator emplace_hint(const_iterator position, Args&&... args);
//
// [16] iterator erase(const_iterator position);
// [16] size_type erase(const key_type& key);
// [16] iterator erase(const_iterator first, const_iterator last);
// [ 8] void swap(set& other);
// [ 2] void clear();
//
// observers:
// [19] key_compare key_comp() const;
// [19] value_compare value_comp() const;
//
// set operations:
// [13] iterator find(const key_type& key);
// [13] const_iterator find(const key_type& key) const;
// [13] size_type count(const key_type& key) const;
// [13] iterator lower_bound(const key_type& key);
// [13] const_iterator lower_bound(const key_type& key) const;
// [13] iterator upper_bound(const key_type& key);
// [13] const_iterator upper_bound(const key_type& key) const;
// [13] bsl::pair<iterator, iterator> equal_range(const key_type& key);
// [13] bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
//
// [ 6] bool operator==(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [17] bool operator< (const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [ 6] bool operator!=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [17] bool operator> (const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [17] bool operator>=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [17] bool operator<=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
//
//// specialized algorithms:
// [ 8] void swap(set<K, C, A>& a, set<K, C, A>& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [35] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(set<T,A> *object, const char *spec, int verbose = 1);
// [ 3] set<T,A>& gg(set<T,A> *object, const char *spec);
//
// [22] CONCERN: The object is compatible with STL allocators.
// [23] CONCERN: The object has the necessary type traits
// [24] CONCERN: The type provides the full interface defined by the standard.
// [33] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [34] CONCERN: 'find'        properly handles transparent comparators.
// [34] CONCERN: 'count'       properly handles transparent comparators.
// [34] CONCERN: 'lower_bound' properly handles transparent comparators.
// [34] CONCERN: 'upper_bound' properly handles transparent comparators.
// [34] CONCERN: 'equal_range' properly handles transparent comparators.
// [35] CONCERN: 'count'       properly handles multi-value comparators.
// [35] CONCERN: 'find'        properly handles multi-value comparators.
// [35] CONCERN: 'lower_bound' properly handles multi-value comparators.
// [35] CONCERN: 'upper_bound' properly handles multi-value comparators.
// [35] CONCERN: 'equal_range' properly handles multi-value comparators.
// [36] CLASS TEMPLATE DEDUCTION GUIDES

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
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//              ADDITIONAL TEST MACROS FOR THIS TEST DRIVER
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
# define BSLSTL_SET_TEST_LOW_MEMORY     1
#endif

#if defined(BSLSTL_SET_TEST_LOW_MEMORY)
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
// The following enum is set to '1' when exceptions are enabled and to '0'
// otherwise.  It's here to avoid having preprocessor macros throughout.
enum { PLAT_EXC = 1 };
#else
enum { PLAT_EXC = 0 };
#endif

// ============================================================================
//                             SWAP TEST HELPERS
// ----------------------------------------------------------------------------

namespace incorrect {

template <class TYPE>
void swap(TYPE&, TYPE&)
    // Fail.  In a successful test, this 'swap' should never be called.  It is
    // set up to be called (and fail) in the case where ADL fails to choose the
    // right 'swap' in 'invokeAdlSwap' below.
{
    ASSERT(0 && "incorrect swap called");
}

}  // close namespace incorrect

template <class TYPE>
void invokeAdlSwap(TYPE *a, TYPE *b)
    // Exchange the values of the specified '*a' and '*b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).
{
    using incorrect::swap;

    // A correct ADL will key off the types of '*a' and '*b', which will be of
    // our 'bsl' container type, to find the right 'bsl::swap' and not
    // 'incorrect::swap'.

    swap(*a, *b);
}

template <class TYPE>
void invokePatternSwap(TYPE *a, TYPE *b)
    // Exchange the values of the specified '*a' and '*b' objects using the
    // 'swap' method found by the recommended pattern for calling 'swap'.
{
    // Invoke 'swap' using the recommended pattern for 'bsl' clients.

    using bsl::swap;

    swap(*a, *b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' and 'invokePatternSwap' (above).

using namespace BloombergLP;
using bsl::pair;
using bsl::set;
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
    { L_,    3, "ABCA",              "ABC" },
    { L_,    3, "ABCB",              "ABC" },
    { L_,    3, "ABCC",              "ABC" },
    { L_,    3, "ABCABC",            "ABC" },
    { L_,    3, "AABBCC",            "ABC" },
    { L_,   12, "BAD",               "ABD" },
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
static const size_t DEFAULT_NUM_DATA =
                                    sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl {

template <class KEY, class COMP, class ALLOC>
void debugprint(const bsl::set<KEY, COMP, ALLOC>& s)
    // set-specific print function.
{
    if (s.empty()) {
        printf("<empty>");
    }
    else {
        typedef typename bsl::set<KEY, COMP, ALLOC>::const_iterator CIter;
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

bool expectToAllocate(size_t n)
    // Return 'true' if the container is expected to allocate memory on the
    // specified 'n'th element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

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
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores a copy
    // of an object of the (template parameter) type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // created on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_control;   // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionProctor(const OBJECT    *object,
                     int              line,
                     const ALLOCATOR& basicAllocator)
    : d_line(line)
    , d_control(*object, basicAllocator)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number that uses the specified 'basicAllocator' to
        // supply memory.  Note that a 'bslma::Allocator *' can be supplied for
        // 'basicAllocator' if the type 'ALLOCATOR' is 'bsl::allocator'.
    {
    }

    ExceptionProctor(const OBJECT             *object,
                     int                       line,
                     bslmf::MovableRef<OBJECT> control)
    : d_line(line)
    , d_control(bslmf::MovableRefUtil::move(control))
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
    {
    }

    ~ExceptionProctor()
        // Destroy this object.  If the proctor was not released, verify that
        // the state of the object supplied at construction has not change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_control, *d_object_p, d_control == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release this proctor from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

bool g_enableLessThanFunctorFlag = true;

                       // ====================
                       // class TestComparator
                       // ====================

template <class TYPE>
class TestComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
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
    //! TestComparator(const TestComparator& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestComparator(int id = 0, bool compareLess = true)
        // Create a 'TestComparator'.  Optionally, specify 'id' that can be
        // used to identify the object.
    : d_id(id)
    , d_compareLess(compareLess)
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

        if (d_compareLess) {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
            < bsltf::TemplateTestFacility::getIdentifier(rhs);        // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier(lhs)
            > bsltf::TemplateTestFacility::getIdentifier(rhs);        // RETURN
        }
    }

    bool operator==(const TestComparator& rhs) const
    {
        return (id() == rhs.id() && d_compareLess == rhs.d_compareLess);
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


                       // ============================
                       // class TestComparatorNonConst
                       // ============================

template <class TYPE>
class TestComparatorNonConst {
    // This test class provides a mechanism that defines a non-'const'
    // function-call operator that compares two objects of the parameterized
    // 'TYPE'.  The function-call operator is implemented with integer
    // comparison using integers converted from objects of 'TYPE' by the class
    // method 'TemplateTestFacility::getIdentifier'.  The function-call
    // operator also increments a counter used to keep track the method call
    // count.  Object of this class can be identified by an id passed on
    // construction.

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CREATORS
    explicit TestComparatorNonConst(int id = 0, bool compareLess = true)
    : d_id(id)
    , d_compareLess(compareLess)
    , d_count(0)
    {
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs)
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.
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

    bool operator==(const TestComparatorNonConst& rhs) const
    {
        return id() == rhs.id();
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
        // Return 'true' if the specified 'lhs' is less than the specified
        // 'rhs' and 'false' otherwise.
    {
        return lhs < rhs;
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

    friend bool operator<(const TransparentlyComparable& lhs, int rhs)
        // Return 'true' if the value of the specified 'lhs' is less than the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs.d_value < rhs;
    }

    friend bool operator<(int lhs, const TransparentlyComparable& rhs)
        // Return 'true' if the specified 'lhs' is less than the value of the
        // specified 'rhs', and 'false' otherwise.
    {
        return lhs < rhs.d_value;
    }
};

template <class Container>
void testTransparentComparator(Container& container,
                               bool       isTransparent,
                               int        initKeyValue)
    // Search for a value equal to the specified 'initKeyValue' in the
    // specified 'container', and count the number of conversions expected
    // based on the specified 'isTransparent'.  Note that 'Container' may
    // resolve to a 'const'-qualified type, we are using the "reference" here
    // as a sort of universal reference.  Conceptually, the object remains
    // constant, but we want to test 'const'-qualified and
    // non-'const'-qualified overloads.
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

    const Count EXISTING_C = container.count(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(1                       == EXISTING_C);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Count NON_EXISTING_C = container.count(nonExistingKey);
    ASSERT(0                       == NON_EXISTING_C);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'lower_bound'.

    const Iterator EXISTING_LB = container.lower_bound(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXISTING_F              == EXISTING_LB);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Iterator NON_EXISTING_LB = container.lower_bound(nonExistingKey);

    ASSERT(container.begin()       == NON_EXISTING_LB);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'upper_bound'.

    TransparentlyComparable upperBoundValue(initKeyValue + 1);
    const Iterator          EXPECTED_UB = container.find(upperBoundValue);
    const Iterator          EXISTING_UB = container.upper_bound(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXPECTED_UB             == EXISTING_UB);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const Iterator NON_EXISTING_UB = container.upper_bound(nonExistingKey);

    ASSERT(container.begin()       == NON_EXISTING_UB);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());

    // Testing 'equal_range'.

    const bsl::pair<Iterator, Iterator> EXISTING_ER =
                                            container.equal_range(existingKey);
    if (!isTransparent) {
        ++expectedConversionCount;
    }

    ASSERT(EXISTING_LB             == EXISTING_ER.first);
    ASSERT(EXPECTED_UB             == EXISTING_ER.second);
    ASSERT(expectedConversionCount == existingKey.conversionCount());

    const bsl::pair<Iterator, Iterator> NON_EXISTING_ER =
                                         container.equal_range(nonExistingKey);

    ASSERT(NON_EXISTING_LB         == NON_EXISTING_ER.first);
    ASSERT(NON_EXISTING_UB         == NON_EXISTING_ER.second);
    ASSERT(expectedConversionCount == nonExistingKey.conversionCount());
}

            // ====================================================
            // struct TransparentComparatorWithMultiValueEqualRange
            // ====================================================

struct TransparentComparatorWithMultiValueEqualRange {
    // This class can be used as a comparator for containers, having
    // 'bsl::string' type as a 'KEY'.  It is classified as transparent by the
    // 'bslmf::IsTransparentPredicate' metafunction and can be used for
    // heterogeneous comparison.  But note that additional operators accepting
    // 'char' objects behave differently than the main one accepting
    // 'bsl::string' objects only.  That leads to puzzling return values of
    // such container methods as 'count' and 'equal_range' accepting 'char'
    // objects.  'count' can return a value greater than one and 'equal_range'
    // can return a range containing more than one element, while container is
    // still has only unique values.

    typedef void is_transparent;

    bool operator()(const bsl::string& lhs, const bsl::string& rhs) const
        // Return 'true' if the specified 'lhs' is less than the specified
        // 'rhs', and 'false' otherwise.
    {
        return lhs < rhs;
    }

    bool operator()(const bsl::string& s, char c) const
        // Return 'true' if the first symbol of the specified 's' is less than
        // the specified 'c', and 'false' otherwise.
    {
        return s.length() == 0 || s[0] < c;
    }

    bool operator()(char c, const bsl::string& s) const
        // Return 'true' if the specified 'c' is less than the first symbol of
        // the specified 's', and 'false' otherwise.
    {
        return s.length() != 0 && c < s[0];
    }
};

                            // ========================
                            // class ThrowingComparator
                            // ========================

template <class TYPE>
struct ThrowingComparator : public std::less<TYPE> {
    // This dummy class implements the minimal interface that meets the
    // requirements for a comparator that can throw exceptions from the move
    // assignment operator and from the 'swap' method/free function.

  public:
    // CREATORS
    ThrowingComparator()
        // Create a 'ThrowingComparator' object.
    {
    }

    ThrowingComparator(const ThrowingComparator&)
        // Create a 'ThrowingComparator' object.
    {
    }

    ThrowingComparator(bslmf::MovableRef<ThrowingComparator>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Create a 'ThrowingComparator' object.
    {
    }

    // MANIPULATORS
    ThrowingComparator &operator=(const ThrowingComparator&)
        // Return a reference, providing modifiable access to this object.
    {
        return *this;
    }

    ThrowingComparator&
    operator=(BloombergLP::bslmf::MovableRef<ThrowingComparator>)
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Return a reference, providing modifiable access to this object.
    {
        return *this;
    }

    void swap(ThrowingComparator&) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Do nothing.
    {
    }
};

// FREE FUNCTIONS
template <class TYPE>
void swap(ThrowingComparator<TYPE>&,
          ThrowingComparator<TYPE>&) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
    // Do nothing.
{
}

                       // =====================
                       // class TemplateWrapper
                       // =====================

template <class KEY, class COMPARATOR, class ALLOCATOR>
class TemplateWrapper {
    // This class inherits from the container, but do nothing otherwise.  A
    // compiler bug in AIX prevents the compiler from finding the definition of
    // the default arguments for the constructor.  This class is created to
    // test this scenario.

    // DATA
    bsl::set<KEY, COMPARATOR, ALLOCATOR> d_member;

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

class DummyComparator {
    // A dummy comparator class.  Must be defined after 'TemplateWrapper' to
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
    // reproduce the AIX bug.  Every method is a noop.

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
    {
    }

    //! DummyAllocator(const DummyAllocator& original) = default;

    template <class BDE_OTHER_TYPE>
    DummyAllocator(const DummyAllocator<BDE_OTHER_TYPE>& original)
    {
        (void) original;
    }

    //! ~DummyAllocator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! DummyAllocator& operator=(const DummyAllocator& rhs) = default;

    pointer allocate(size_type    /* numElements */,
                     const void * /* hint */ = 0)
    {
        return 0;
    }

    void deallocate(pointer /* address */, size_type /* numElements */ = 1)
    {
    }

    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE * /* address */)
    {
    }

    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE * /* address */,
                   const ELEMENT_TYPE& /* value */)
    {
    }

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE * /* address */)
    {
    }

    // ACCESSORS
    pointer address(reference /* object */) const
    {
        return 0;
    }

    const_pointer address(const_reference /* object */) const
    {
        return 0;
    }

    size_type max_size() const
    {
        return 0;
    }
};

template <class TYPE>
class GreaterThanFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Return 'true' if the integer representation of the specified 'lhs'
        // is less than integer representation of the specified 'rhs'.
    {
        return bsltf::TemplateTestFacility::getIdentifier(lhs)
             > bsltf::TemplateTestFacility::getIdentifier(rhs);
    }
};

// FREE OPERATORS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getIdentifier(lhs)
         < bsltf::TemplateTestFacility::getIdentifier(rhs);
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
          class COMP = TestComparator<KEY>,
          class ALLOC = bsl::allocator<KEY> >
class TestDriver {
    // This templatized struct provide a namespace for testing the 'set'
    // container.  The parameterized 'KEY', 'COMP' and 'ALLOC' specifies the
    // value type, comparator type and allocator type respectively.  Each
    // "testCase*" method test a specific aspect of 'set<KEY, COMP, ALLOC>'.
    // Every test cases should be invoked with various parameterized type to
    // fully test the container.

  private:
    // TYPES

    // Shorthands

    typedef bsl::set<KEY, COMP, ALLOC>            Obj;
    typedef typename Obj::iterator                Iter;
    typedef typename Obj::const_iterator          CIter;
    typedef typename Obj::reverse_iterator        RIter;
    typedef typename Obj::const_reverse_iterator  CRIter;
    typedef typename Obj::size_type               SizeType;
    typedef typename Obj::value_type              ValueType;

    typedef bslma::ConstructionUtil               ConsUtil;
    typedef bslmf::MovableRefUtil                 MoveUtil;
    typedef bsltf::TestValuesArray<KEY, ALLOC>    TestValues;
    typedef bsltf::MoveState                      MoveState;
    typedef bsltf::TemplateTestFacility           TstFacility;
    typedef TestMovableTypeUtil<Iter, KEY>        TstMoveUtil;

    typedef TestComparatorNonConst<KEY>           NonConstComp;
        // Comparator functor with a non-'const' function call operator.

    typedef bsl::allocator_traits<ALLOC>                AllocatorTraits;
    typedef typename ALLOC::template rebind<KEY>::other KeyAllocator;

    enum AllocCategory { e_BSLMA, e_ADAPTOR, e_STATEFUL };

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

    // CLASS DATA
    static
    const AllocCategory s_allocCategory =
                        bsl::is_same<ALLOC, bsl::allocator<KEY> >::value
                        ? e_BSLMA
                        : bsl::is_same<ALLOC,
                                       bsltf::StdAllocatorAdaptor<
                                                 bsl::allocator<KEY> > >::value
                        ? e_ADAPTOR
                        : e_STATEFUL;

    static
    const char *allocCategoryAsStr()
    {
        return e_BSLMA == s_allocCategory ? "bslma"
                                          : e_ADAPTOR == s_allocCategory
                                          ? "adaptor"
                                          : e_STATEFUL == s_allocCategory
                                          ? "stateful"
                                          : "<INVALID>";
    }

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
        size_t i     = 0;

        for (CIter b = object.cbegin(); b != object.end() && i < n; ++b)
        {
            if (pointers[i++] != bsls::Util::addressOf(*b)) {
                ++count;
            }
        }
        return count;
    }

    static pair<Iter, bool> primaryManipulator(Obj   *container,
                                               int    identifier,
                                               ALLOC  allocator)
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

    template <class T>
    static bslmf::MovableRef<T> testArg(T& t, bsl::true_type )
    {
        return MoveUtil::move(t);
    }

    template <class T>
    static const T& testArg(T& t, bsl::false_type)
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
    static void testCase30a_RunTest(Obj *target, bool inserted);
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
    static Iter testCase31a_RunTest(Obj *target, CIter hint, bool inserted);
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

    static void testCase37();
        // Test free function 'bsl::erase_if'

    static void testCase33();
        // Test 'noexcept' specifications

    static void testCase32();
        // Test initializer list functions.

    static void testCase31a();
        // Test forwarding of arguments in 'emplace_hint' method.

    static void testCase31();
        // Test 'emplace_hint' method.

    static void testCase30a();
        // Test forwarding of arguments in 'emplace' method.

    static void testCase30();
        // Test 'emplace' method.

    static void testCase29();
        // Test insert on movable value with hint.

    static void testCase28();
        // Test insert on movable value.

    static void testCase27_dispatch();
        // Test move assignment.

    static void testCase26();
        // Test move construction.

    static void testCase25();
        // Test standard interface coverage.

    static void testCase24();
        // Test constructor of a template wrapper class.

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
};

template <class KEY>
class StdAllocTestDriver : public TestDriver<KEY,
                                             TestComparator<KEY>,
                                             bsltf::StdTestAllocator<KEY> > {
};

template <class KEY>
class StdBslmaTestDriver : public TestDriver<KEY,
                                             TestComparator<KEY>,
                             bsltf::StdAllocatorAdaptor<bsl::allocator<KEY> > >
{
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY, class COMP, class ALLOC>
int TestDriver<KEY, COMP, ALLOC>::ggg(Obj        *object,
                                      const char *spec,
                                      int         verbose)
{
    enum { SUCCESS = -1 };

    bslma::TestAllocator scratch;
    ALLOC sscratch(&scratch);
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            primaryManipulator(object, spec[i], sscratch);
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
bsl::set<KEY, COMP, ALLOC>& TestDriver<KEY, COMP, ALLOC>::gg(
                                                            Obj        *object,
                                                            const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase37()
{
    // --------------------------------------------------------------------
    // TESTING FREE FUNCTION 'BSL::ERASE_IF'
    //
    // Concerns:
    //: 1 The free function exists, and is callable with an set.
    //
    // Plan:
    //: 1 Fill a set with known values, then attempt to erase some of
    //:   the values using 'bsl::erase_if'.  Verify that the resultant set
    //:   is the right size, contains the correct values, and that the
    //:   value returned from the functions is correct.
    //
    // Testing:
    //   size_t erase_if(set&, PREDICATE);
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
TestDriver<KEY, COMP, ALLOC>::testCase30a_RunTest(Obj *target, bool inserted)
{
    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<KEY>::value;
    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

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
TestDriver<KEY, COMP, ALLOC>::testCase31a_RunTest(Obj   *target,
                                                  CIter  hint,
                                                  bool   inserted)
{
    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<KEY>::value;
    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

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
        return hint;                                                  // RETURN
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

        ASSERTV(inserted, inserted == (&(*result) != &(*hint)));

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
    //   CONCERN: Methods qualified 'noexcept' in standard are so implemented.
    // ------------------------------------------------------------------------

    if (verbose) {
        T_ P(bsls::NameOf<Obj>())
        T_ P(bsls::NameOf<KEY>())
        T_ P(bsls::NameOf<COMP>())
        T_ P(bsls::NameOf<ALLOC>())
    }

    // N4594: page 872:  23.4.6: Class template 'set' [set]

    // page 873
    //..
    //  // 23.4.6.2, construct/copy/destroy:
    //  set& operator=(set&& x)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_move_assignable<Compare>::value);
    //  allocator_type get_allocator() const noexcept;
    //..

    {
        Obj mX;  const Obj& X = mX;
        Obj mY;

        (void) X;    (void) mY;

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

    // page 873
    //..
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
    //..

    {
        Obj mX;    const Obj& X = mX;    (void) X;

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

    // page 873-874
    //..
    //  // capacity:
    //  bool empty() const noexcept;
    //  size_type size() const noexcept;
    //  size_type max_size() const noexcept;
    //..

    {
        Obj mX;    const Obj& X = mX;    (void) X;

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.empty()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.size()));
        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(X.max_size()));
    }

    // page 874
    //..
    //  // modifiers:
    //  void swap(set&)
    //      noexcept(allocator_traits<Allocator>::is_always_equal::value &&
    //               is_nothrow_swappable_v<Compare>);
    //  void clear() noexcept;
    //..

    {
        Obj x;    (void) x;
        Obj y;    (void) y;

        bool expected = false;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        expected = bsl::allocator_traits<ALLOC>::is_always_equal::value &&
                   bsl::is_nothrow_swappable<COMP>::value;
#endif

        ASSERT(expected == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(y)));

        ASSERT(BSLS_KEYWORD_NOEXCEPT_AVAILABLE
            == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.clear()));
    }

    // page 875
    //..
    //  // 23.4.6.3, specialized algorithms:
    //  template <class Key, class Compare, class Allocator>
    //  void swap(set<Key, Compare, Allocator>& x,
    //            set<Key, Compare, Allocator>& y)
    //      noexcept(noexcept(x.swap(y)));
    //..

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
    //   set(initializer_list<value_type>, const C& comp, const A& allocator);
    //   set(initializer_list<value_type>, const A& allocator);
    //   set& operator=(initializer_list<value_type>);
    //   bsl::pair<iterator, bool> insert(initializer_list<value_type>);
    // -----------------------------------------------------------------------
#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    const TestValues V;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    if (verbose)
        printf("\tTesting constructor with initializer lists\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    {
        const struct {
            int                         d_line;    // source line number
            std::initializer_list<KEY>  d_list;    // source list
            const char                 *d_result;  // expected result
        } DATA[] = {
            //line  list                   result
            //----  --------------------   ------
            { L_,   {                  },  ""     },
            { L_,   { V[0]             },  "A"    },
            { L_,   { V[0], V[0]       },  "A"    },
            { L_,   { V[1], V[0]       },  "AB"   },
            { L_,   { V[0], V[1], V[2] },  "ABC"  },
            { L_,   { V[0], V[1], V[0] },  "AB"   },
        };

        const size_t NUM_SPECS = sizeof DATA / sizeof *DATA;
        const COMP MYCOMP(7);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, MYCOMP, &oa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());
                ASSERT(MYCOMP == X.key_comp());

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
            //line  list                      result
            //----  --------------------      ------
            { L_,   {                  },     ""    },
            { L_,   { V[0]             },     "A"   },
            { L_,   { V[0], V[0]       },     "A"   },
            { L_,   { V[1], V[0]       },     "AB"  },
            { L_,   { V[0], V[1], V[2] },     "ABC" },
            { L_,   { V[0], V[1], V[0] },     "AB"  },
        };

        const size_t NUM_SPECS = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            Obj mY(&scratch); const Obj& Y = gg(&mY, DATA[ti].d_result);

            int numPasses = 0;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ++numPasses;
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                Obj mX(DATA[ti].d_list, &oa); const Obj& X = mX;

                ASSERTV(Y, X, Y == X);

                ASSERT(&oa == X.get_allocator());
                ASSERT(COMP() == X.key_comp());

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            ASSERTV((!PLAT_EXC || 0 == ti) == (1 == numPasses));

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
            //line  lhs       list                     result
            //----  ------    --------------------     ------
            { L_,   "",       {                  },    ""    },
            { L_,   "",       { V[0]             },    "A"   },
            { L_,   "A",      {                  },    ""    },
            { L_,   "A",      { V[1]             },    "B"   },
            { L_,   "A",      { V[0], V[1]       },    "AB"  },
            { L_,   "A",      { V[1], V[2]       },    "BC"  },
            { L_,   "AB",     {                  },    ""    },
            { L_,   "AB",     { V[0], V[1], V[2] },    "ABC" },
            { L_,   "AB",     { V[2], V[3], V[4] },    "CDE" },
        };

        const size_t NUM_SPECS = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mY(&scratch); const Obj& Y = gg(&mY, DATA[ti].d_result);
            Obj mX(&oa);      const Obj& X = gg(&mX, DATA[ti].d_spec);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

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
            //line  source    list                    result
            //----  ------    --------------------    --------
            { L_,   "",       {                  },    ""     },
            { L_,   "",       { V[0]             },   "A"     },
            { L_,   "A",      {                  },   "A"     },
            { L_,   "A",      { V[0]             },   "A"     },
            { L_,   "A",      { V[1]             },   "AB"    },
            { L_,   "AB",     { V[0], V[1]       },   "AB"    },
            { L_,   "AB",     { V[1], V[2]       },   "ABC"   },
            { L_,   "AB",     { V[0], V[1], V[2] },   "ABC"   },
            { L_,   "AB",     { V[2], V[3], V[4] },   "ABCDE" },
        };

        const size_t NUM_SPECS = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator        oa("object", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor dam(&da);

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {

            ASSERT(0 == oa.numBytesInUse());

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mY(&scratch); const Obj& Y = gg(&mY, DATA[ti].d_result);
            Obj mX(&oa);      const Obj& X = gg(&mX, DATA[ti].d_spec);

            mX.insert(DATA[ti].d_list);

            ASSERTV(X, Y, X == Y);
        }
        ASSERT(dam.isTotalSame());
    }
#endif
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase31a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE WITH HINT
    //
    // Concerns:
    //: 1 'emplace_hint' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase31'.
    //:
    //: 2 'emplace_hint' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase31a_RunTest'
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
    //: 3 Call 'testCase31a_RunTest' in various configurations:
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

        hint = testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, hint, true);
        hint = testCase31a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint, true);
    }
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint, true);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint, true);
    }
    if (verbose) printf("\tTesting emplace_hint with 0 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
    }
    if (verbose) printf("\tTesting emplace_hint with 1 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, hint, false);
    }
    if (verbose) printf("\tTesting emplace_hint with 2 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, hint, false);
    }
    if (verbose) printf("\tTesting emplace_hint with 3 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, hint, false);
        hint = testCase31a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, hint, false);
    }

    if (verbose) printf("\tTesting emplace_hint with 10 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, X.end(), true);
        hint = testCase31a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, hint, false);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, hint, false);
        hint = testCase31a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, hint, false);
    }
#else
    if (verbose) printf("\tTesting emplace_hint 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);
        const Obj&           X = mX;

        hint = testCase31a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, X.end(), true);
        hint = testCase31a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, hint, true);
        hint = testCase31a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, hint, true);
        hint = testCase31a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, hint, true);
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
    //: 1 'emplace_hint' returns an iterator referring to the newly inserted
    //:   element if it did not already exists, and to the existing element if
    //:   it did.
    //:
    //: 2 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Inserting with the correct hint requires no more than 2 comparisons.
    //:
    //: 5 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace_hint'
    //:   function and will test proper forwarding of constructor arguments in
    //:   test 'testCase31a'.
    //:
    //: 2 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'lower_bound', 'begin',
    //:     'begin' + 1, 'end' - 1, 'end'
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Compute the number of allocations and verify it is as
    //:         expected.
    //:
    //:       2 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       3 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify the number of comparisons is no more than 2 if the hint
    //:         is valid.  (C-4)
    //:
    //:       5 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 3 Repeat P-2 under the presence of exception  (C-7)
    //
    // Testing:
    //   iterator emplace_hint(const_iterator position, Args&&... args);
    // ------------------------------------------------------------------------

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
    const size_t NUM_DATA   = sizeof DATA / sizeof *DATA;
    const size_t MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'emplace' with hint.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const char *const ALLOCS = DATA[ti].d_allocs;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj& X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

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
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    size_t EXP_COMP = X.key_comp().count();
                    if (CONFIG == 'a') {
                        if (!IS_UNIQ) {
                            EXP_COMP += 2;
                        }
                        else {
                            if (hint != X.begin()) {
                                ++EXP_COMP;
                            }
                            if (hint != X.end()) {
                                ++EXP_COMP;
                            }
                        }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    Iter RESULT = mX.emplace_hint(hint, VALUES[tj]);

                    const bsls::Types::Int64 AA = oa.numBlocksTotal();
                    const bsls::Types::Int64 A  = oa.numBlocksInUse();

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *RESULT);

                    if (CONFIG == 'a') {
                        ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                EXP_COMP == X.key_comp().count());

                        if (IS_UNIQ) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }
                        else {
                            ASSERTV(LINE, tj, hint == RESULT);
                        }
                    }

                    if (IS_UNIQ) {
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

                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X,
                                                     exp,
                                                     SIZE + 1));
                    }
                    else {
                        if ('+' == ALLOCS[tj]) {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 1 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj,  A,  B,
                                    B + 1               ==  A);
                        }
                        else {
                            ASSERTV(LINE, tj, AA, BB,
                                    BB + 0 + TYPE_ALLOC == AA);
                            ASSERTV(LINE, tj,  A,  B,
                                    B + 0               ==  A);
                        }
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
    }

    if (verbose) printf("\tTesting 'emplace_hint' with exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj& X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

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
                          ASSERTV(!"Unexpected configuration");
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

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *RESULT);

                    if (IS_UNIQ) {
                        ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, tj,
                                0 == verifyContainer(X,
                                                     exp,
                                                     SIZE + 1));
                    }
                    else {
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
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase30a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:  'testCase30'.
    //:
    //: 2 'emplace' is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 This test makes material use of template method 'testCase30a_RunTest'
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

        testCase30a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 1,1,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 2,1,1,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 3,1,1,1,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 4,1,1,1,1,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 5,1,1,1,1,1,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 6,1,1,1,1,1,1,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 7,1,1,1,1,1,1,1,2,2,2>(&mX, true);
        testCase30a_RunTest< 8,1,1,1,1,1,1,1,1,2,2>(&mX, true);
        testCase30a_RunTest< 9,1,1,1,1,1,1,1,1,1,2>(&mX, true);
        testCase30a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, true);
    }
    if (verbose) printf("\tTesting emplace 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, true);
        testCase30a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, true);
        testCase30a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, true);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, true);
    }
    if (verbose) printf("\tTesting emplace with 0 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest<0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
    }
    if (verbose) printf("\tTesting emplace with 1 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest<1,0,2,2,2,2,2,2,2,2,2>(&mX,  true);
        testCase30a_RunTest<1,1,2,2,2,2,2,2,2,2,2>(&mX, false);
    }
    if (verbose) printf("\tTesting emplace with 2 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest<2,0,0,2,2,2,2,2,2,2,2>(&mX,  true);
        testCase30a_RunTest<2,1,0,2,2,2,2,2,2,2,2>(&mX, false);
        testCase30a_RunTest<2,0,1,2,2,2,2,2,2,2,2>(&mX, false);
        testCase30a_RunTest<2,1,1,2,2,2,2,2,2,2,2>(&mX, false);
    }
    if (verbose) printf("\tTesting emplace with 3 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest<3,0,0,0,2,2,2,2,2,2,2>(&mX,  true);
        testCase30a_RunTest<3,1,0,0,2,2,2,2,2,2,2>(&mX, false);
        testCase30a_RunTest<3,0,1,0,2,2,2,2,2,2,2>(&mX, false);
        testCase30a_RunTest<3,0,0,1,2,2,2,2,2,2,2>(&mX, false);
        testCase30a_RunTest<3,1,1,1,2,2,2,2,2,2,2>(&mX, false);
    }

    if (verbose) printf("\tTesting emplace with 10 args\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX,  true);
        testCase30a_RunTest<10,1,0,0,0,0,0,0,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,1,0,0,0,0,0,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,1,0,0,0,0,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,1,0,0,0,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,0,1,0,0,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,0,0,1,0,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,0,0,0,1,0,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,1,0,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,1,0>(&mX, false);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,1>(&mX, false);
        testCase30a_RunTest<10,1,1,1,1,1,1,1,1,1,1>(&mX, false);
    }
#else
    if (verbose) printf("\tTesting emplace 1..10 args, move=0\n");
    {
        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        Obj                  mX(&oa);

        testCase30a_RunTest< 0,2,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 1,0,2,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 2,0,0,2,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 3,0,0,0,2,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 4,0,0,0,0,2,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 5,0,0,0,0,0,2,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 6,0,0,0,0,0,0,2,2,2,2>(&mX, true);
        testCase30a_RunTest< 7,0,0,0,0,0,0,0,2,2,2>(&mX, true);
        testCase30a_RunTest< 8,0,0,0,0,0,0,0,0,2,2>(&mX, true);
        testCase30a_RunTest< 9,0,0,0,0,0,0,0,0,0,2>(&mX, true);
        testCase30a_RunTest<10,0,0,0,0,0,0,0,0,0,0>(&mX, true);
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
    //: 1 'emplace' returns a pair containing an iterator and a 'bool'
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
    //:
    //
    // Plan:
    //: 1 We will use 'value' as the single argument to the 'emplace' function
    //:   and will test proper forwarding of constructor arguments in test
    //:   'testCase30a'.
    //:
    //: 2 For emplace, we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 Compute the number of allocations and verify it is as expected.
    //:
    //:   2 If the object did not contain 'value', verify it now exist.
    //:     Otherwise, verify the return value is as expected.  (C-1..4)
    //:
    //:   3 Verify all allocations are from the object's allocator.  (C-5)
    //:
    //: 3 Repeat P-2 under the presence of exception  (C-6)
    //
    // Testing:
    //   bsl::pair<iterator, bool> emplace(Args&&... arguments);
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
    const size_t NUM_DATA   = sizeof DATA / sizeof *DATA;
    const size_t MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'emplace' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const char *const ALLOCS = DATA[ti].d_allocs;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj& X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                bsl::pair<Iter, bool> RESULT = mX.emplace(VALUES[tj]);

                ASSERTV(LINE, tj, SIZE, IS_UNIQ    == RESULT.second);
                ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (IS_UNIQ) {
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
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
                else {
                    if ('+' == ALLOCS[tj]) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 1              ==  A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj,  A,  B,  B + 0              ==  A);
                    }
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

    if (verbose) printf("\tTesting 'emplace' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            KeyAllocator         xoa(&oa);

            Obj mX(xoa);  const Obj& X = mX;

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                const size_t SIZE    = X.size();

                if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xoa);

                    bsl::pair<Iter, bool> RESULT = mX.emplace(VALUES[tj]);

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                    ASSERTV(LINE, tj, SIZE,
                            VALUES[tj] == *(RESULT.first));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase29()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT ON MOVABLE VALUES
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element
    //:   if it did not already exists, and to the existing element if it did.
    //:
    //: 2 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Inserting with the correct hint requires no more than 2 comparisons.
    //:
    //: 5 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 6 The new element is move-inserted into the container.
    //:
    //: 7 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 8 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'lower_bound', 'begin',
    //:     'begin' + 1, 'end' - 1, 'end'
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Compute the number of allocations and verify it is as
    //:         expected.
    //:
    //:       2 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       3 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify the number of comparisons is no more than 2 if the hint
    //:         is valid.  (C-4)
    //:
    //:       5 Ensure that the move constructor was called on the value type
    //:         (if that type has a mechanism to detect such) where the value
    //:         has the same allocator as that of the container and a different
    //:         allocator than that of the container.  (C-5)
    //:
    //:       5 Verify all allocations are from the object's allocator.  (C-7)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-8)
    //
    // Testing:
    //   iterator insert(const_iterator position, value_type&& value);
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
    const size_t NUM_DATA   = sizeof DATA / sizeof *DATA;
    const size_t MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'insert' with hint.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {
                    const char CONFIG2 = cfg2;

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    Obj mX(xoa); const Obj& X = mX;

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
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    KeyAllocator          xsa(&sa);

                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                        const size_t SIZE    = X.size();

                        if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                        if (IS_UNIQ) {
                            EXPECTED[SIZE] = SPEC[tj];
                            std::sort(EXPECTED, EXPECTED + SIZE + 1);
                            EXPECTED[SIZE + 1] = '\0';

                            if (veryVeryVerbose) { P(EXPECTED); }
                        }

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
                              ASSERTV(!"Unexpected configuration");
                          }
                        }

                        size_t EXP_COMP = X.key_comp().count();
                        if (CONFIG == 'a') {
                            if (!IS_UNIQ) {
                                EXP_COMP += 2;
                            }
                            else {
                                if (hint != X.begin()) {
                                    ++EXP_COMP;
                                }
                                if (hint != X.end()) {
                                    ++EXP_COMP;
                                }
                            }
                        }

                        const bsls::Types::Int64 BB = oa.numBlocksTotal();
                        const bsls::Types::Int64 B  = oa.numBlocksInUse();

                        bsls::ObjectBuffer<ValueType>  buffer;
                        ValueType                     *valptr =
                                                              buffer.address();
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

                        ASSERTV(LINE, CONFIG, tj, SIZE,
                                VALUES[tj] == *RESULT);

                        if (CONFIG == 'a') {
                            ASSERTV(LINE, tj, EXP_COMP, X.key_comp().count(),
                                    EXP_COMP == X.key_comp().count());

                            if (IS_UNIQ) {
                                ASSERTV(LINE, tj, hint == ++RESULT);
                            }
                            else {
                                ASSERTV(LINE, tj, hint == RESULT);
                            }
                        }

                        const MoveState::Enum expMove =
                           k_KEY_IS_MOVE_ENABLED
                           ? ((k_KEY_IS_WELL_BEHAVED && &sa != &oa) || !IS_UNIQ
                              ? MoveState::e_NOT_MOVED
                              : MoveState::e_MOVED)
                           : MoveState::e_UNKNOWN;
                        ASSERTV(expMove, mState, NameOf<KEY>(),
                                                            expMove == mState);

                        if (IS_UNIQ) {
                            bsls::Types::Int64 expAA = BB + TYPE_ALLOC;
                                                                    // new item
                            bsls::Types::Int64 expA  = B  + TYPE_ALLOC;
                                                                    // new item

                            if (expectToAllocate(SIZE + 1)) {
                                expA  = expA  + 1;            // resize on pool
                                expAA = expAA + 1;            // resize on pool
                            }
                            if (&sa == &oa && MoveState::e_MOVED != mState) {
                                expAA += TYPE_ALLOC;   // temporary (not moved)
                            }
                            ASSERTV(LINE, CONFIG2, tj, AA, expAA, AA == expAA);
                            ASSERTV(LINE, CONFIG2, tj,  A,  expA,  A ==  expA);
                            ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());

                            TestValues exp(EXPECTED);

                            ASSERTV(LINE, tj,
                                    0 == verifyContainer(X,
                                                         exp,
                                                         SIZE + 1));
                        }
                        else {
                            if (&sa == &oa) {
                                ASSERTV(LINE, CONFIG2, tj, AA, BB,
                                        BB + TYPE_ALLOC == AA);
                            }
                            else {
                                ASSERTV(LINE, CONFIG2, tj, AA, BB, BB == AA);
                            }
                            ASSERTV(LINE, tj, A,  B,  B == A);
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
        }
    }
    if (verbose) printf("\tTesting 'insert' with exception.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                for (char cfg2 = 'a'; cfg2 <= 'b'; ++cfg2) {

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    Obj mX(xoa);  const Obj& X = mX;

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
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *valAllocator;
                    KeyAllocator          xsa(&sa);

                    for (size_t tj = 0; tj < LENGTH; ++tj) {
                        const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                        const size_t SIZE    = X.size();

                        if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                        if (IS_UNIQ) {
                            EXPECTED[SIZE] = SPEC[tj];
                            std::sort(EXPECTED, EXPECTED + SIZE + 1);
                            EXPECTED[SIZE + 1] = '\0';

                            if (veryVeryVerbose) { P(EXPECTED); }
                        }

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
                              ASSERTV(!"Unexpected configuration");
                          }
                        }

                        bslma::TestAllocator scratch("scratch",
                                                     veryVeryVeryVerbose);
                        KeyAllocator         xscratch(&scratch);
                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            Obj mZ(xscratch);   const Obj& Z = mZ;
                            for (size_t tk = 0; tk < tj; ++tk) {
                                primaryManipulator(
                                        &mZ,
                                        TstFacility::getIdentifier(VALUES[tk]),
                                        xscratch);
                            }
                            ASSERTV(Z, X, Z == X);

                            ExceptionProctor<Obj,ALLOC>
                                           proctor(&X, L_, MoveUtil::move(mZ));

                            bsls::ObjectBuffer<ValueType>  buffer;
                            ValueType                     *valptr =
                                                              buffer.address();
                            TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));

                            bslma::DestructorGuard<ValueType> guard(valptr);
                            Iter                              RESULT =
                                      mX.insert(hint, MoveUtil::move(*valptr));

                            proctor.release();

                            ASSERTV(LINE, CONFIG, tj, SIZE,
                                    VALUES[tj] == *RESULT);

                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        if (IS_UNIQ) {
                            ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                            TestValues exp(EXPECTED);
                            ASSERTV(LINE, tj,
                                    0 == verifyContainer(X,
                                                         exp,
                                                         SIZE + 1));
                        }
                        else {
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
    //: 1 'insert' returns a pair containing an iterator and a 'bool'
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
    //: 5 The new element is move-inserted into the container.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
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
    //:   3 Ensure that the move constructor was called on the value type (if
    //:     that type has a mechanism to detect such) where the value has the
    //:     same allocator as that of the container and a different allocator
    //:     than that of the  container.  (C-5)
    //:
    //:   4 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-7)
    //
    // Testing:
    //   bsl::pair<iterator, bool> insert(value_type&& value);
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
    const size_t NUM_DATA   = sizeof DATA / sizeof *DATA;
    const size_t MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa);  const Obj& X = mX;

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
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                bslma::TestAllocator& sa = *valAllocator;
                KeyAllocator          xsa(&sa);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    const bsls::Types::Int64 BB = oa.numBlocksTotal();
                    const bsls::Types::Int64 B  = oa.numBlocksInUse();

                    bsls::ObjectBuffer<ValueType>  buffer;
                    ValueType                     *valptr = buffer.address();
                    TstFacility::emplace(
                                        valptr,
                                        TstFacility::getIdentifier(VALUES[tj]),
                                        ALLOC(xsa));
                    bsl::pair<Iter, bool> RESULT =
                                            mX.insert(MoveUtil::move(*valptr));
                    MoveState::Enum       mState =
                                       TstFacility::getMovedFromState(*valptr);
                    bslma::DestructionUtil::destroy(valptr);

                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            IS_UNIQ    == RESULT.second);
                    ASSERTV(LINE, CONFIG, tj, SIZE,
                            VALUES[tj] == *(RESULT.first));

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
                        bsls::Types::Int64 expAA = BB + TYPE_ALLOC; // new item
                        bsls::Types::Int64 expA  = B  + TYPE_ALLOC; // new item

                        if (((SIZE + 1) & SIZE) == 0) {
                            expA  = expA  + 1;  // resize on pool
                            expAA = expAA + 1;  // resize on pool
                        }
                        if (&sa == &oa && MoveState::e_MOVED != mState) {
                            expAA += TYPE_ALLOC;  // temporary (not moved)
                        }

                        ASSERTV(LINE, CONFIG, tj, AA, expAA, expAA == AA);
                        ASSERTV(LINE, CONFIG, tj,  A,  expA,  expA ==  A);
                        ASSERTV(LINE, CONFIG, tj, SIZE, SIZE + 1 == X.size());
                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, CONFIG, tj, 0 == verifyContainer(
                                                                    X,
                                                                    exp,
                                                                    SIZE + 1));
                    }
                    else {
                        if (&sa == &oa) {
                            ASSERTV(LINE, CONFIG, tj, AA, BB,
                                    BB + TYPE_ALLOC == AA);
                        }
                        else {
                            ASSERTV(LINE, CONFIG, tj, AA, BB, BB == AA);
                        }
                        ASSERTV(LINE, CONFIG, tj,  A,  B,  B ==  A);
                        ASSERTV(LINE, CONFIG, tj, SIZE == X.size());

                        TestValues exp(EXPECTED);
                        ASSERTV(LINE, CONFIG, tj, 0 == verifyContainer(X,
                                                                       exp,
                                                                       SIZE));
                    }
                }
            }
        }
    }

    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj& X = mX;

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

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    KeyAllocator        xscratch(&scratch);
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        Obj mZ(xscratch);   const Obj& Z = mZ;
                        for (size_t tk = 0; tk < tj; ++tk) {
                            primaryManipulator(
                                        &mZ,
                                        TstFacility::getIdentifier(VALUES[tk]),
                                        xscratch);
                        }
                        ASSERTV(Z, X, Z == X);

                        ExceptionProctor<Obj, ALLOC>
                                           proctor(&X, L_, MoveUtil::move(mZ));

                        bsls::ObjectBuffer<ValueType>  buffer;
                        ValueType                     *valptr =
                                                              buffer.address();
                        TstFacility::emplace(
                            valptr,
                            TstFacility::getIdentifier(VALUES[tj]),
                            ALLOC(xsa));
                        bslma::DestructorGuard<ValueType> guard(valptr);
                        bsl::pair<Iter, bool>             RESULT =
                                            mX.insert(MoveUtil::move(*valptr));

                        ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                        ASSERTV(LINE, tj, SIZE, VALUES[tj] == *(RESULT.first));

                        proctor.release();
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase27_dispatch()
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
    //   set& operator=(bslmf::MovableRef<set> rhs);
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                AllocatorTraits::propagate_on_container_move_assignment::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING MOVE ASSIGN '%s' OTHER:%c PROP:%c"
                                                                " ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        NameOf<KEY>().name(),
                        otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());


    const int isTypeOtherAlloc = bsl::uses_allocator<KEY, ALLOC>::value;
    const int isTypeAlloc = bslma::UsesBslmaAllocator<KEY>::value ||
                                                              isTypeOtherAlloc;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    ALLOC                        sda(&da);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    ALLOC                sscratch(&scratch);

    // Check remaining properties of allocator to make sure they all match
    // 'otherTraitsSet'.

    BSLMF_ASSERT(otherTraitsSet ==
                          AllocatorTraits::propagate_on_container_swap::value);
    ASSERT((otherTraitsSet ? sscratch : sda) ==
             AllocatorTraits::select_on_container_copy_construction(sscratch));

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    // Create first object
    if (veryVerbose)
        printf("\nTesting move assignment.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(SPEC1);

            if (ti && INDEX1 == DATA[ti-1].d_index) {
                // redundant, skip

                continue;
            }

            const bsls::Types::Int64 BEFORE = scratch.numBytesInUse();

            Obj  mZZ(sscratch); const Obj&  ZZ = gg(&mZZ, SPEC1);

            const bsls::Types::Int64 BIU = scratch.numBytesInUse() - BEFORE;

            if (veryVeryVerbose) { T_ P_(LINE1) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.
            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(sscratch), ZZ, Obj(sscratch) == ZZ);
                firstFlag = false;
            }

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_results;
                const size_t      LENGTH2 = strlen(SPEC2);

                if (tj && INDEX2 == DATA[tj-1].d_index) {
                    // redundant, skip

                    continue;
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator ra(   "source", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);

                    ALLOC soa(&oa);
                    ALLOC sra(&ra);

                    Obj        *objPtr = new (fa) Obj(soa);
                    Obj&        mX = *objPtr;
                    const Obj&  X = gg(&mX, SPEC2);

                    Obj                  *srcPtr = 0;
                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (fa) Obj(sra); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &ra;
                      } break;
                      case 'b': {
                        srcPtr = new (fa) Obj(soa); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }

                    Obj& mZ = *srcPtr;      const Obj& Z = mZ;

                    bslma::TestAllocator& sa = *srcAllocatorPtr;
                    ALLOC ssa(&sa);

                    if (veryVeryVerbose) { T_ P_(LINE2) P(Z) }
                    if (veryVeryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(SPEC1, SPEC2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bool empty = 0 == ZZ.size();

                    typename Obj::const_pointer pointers[2];
                    storeFirstNElemAddr(pointers,
                                        Z,
                                        sizeof pointers / sizeof *pointers);

                    bslma::TestAllocatorMonitor oam(&oa), sam(&sa), dam(&da);

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
                        ASSERT(0 == checkFirstNElemAddr(pointers,
                                                        X,
                                                        sizeof pointers /
                                                            sizeof *pointers));

                        // 3. original object left empty
                        ASSERTV(SPEC1, SPEC2, &sa == &oa, Z, 0 == Z.size());

                        // 4. nothing from the other allocator
                        ASSERTV(SPEC1, SPEC2, 0 == ra.numBlocksTotal());
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
                        ASSERTV(SPEC1, SPEC2, sam.isTotalSame());
                        if (isPropagate) {
                            ASSERTV(SPEC1, SPEC2, oam.isTotalSame());
                            ASSERTV(SPEC1, SPEC2, dam.isTotalSame());
                            ASSERTV(SPEC1, SPEC2, ssa ==  X.get_allocator());
                        }
                        else {
                            ASSERTV(SPEC1, SPEC2, &sa == &oa,
                                    empty || oam.isTotalUp());
                            ASSERTV(NameOf<KEY>(), NameOf<ALLOC>(),
                                               e_STATEFUL == s_allocCategory ||
                                                            dam.isTotalSame());
                            ASSERTV(SPEC1, SPEC2, soa ==  X.get_allocator());
                        }
                    }
                    // Verify that 'X', 'Z', and 'ZZ' have correct allocator.
                    ASSERTV(SPEC1, SPEC2, sscratch == ZZ.get_allocator());
                    ASSERTV(SPEC1, SPEC2,      ssa ==  Z.get_allocator());

                    // Manipulate source object 'Z' to ensure it is in a valid
                    // state and is independent of 'X'.
                    pair<Iter, bool> RESULT =
                                             primaryManipulator(&mZ, 'Z', ssa);
                    ASSERTV(true == RESULT.second);
                    ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                    ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                    ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                    fa.deleteObject(srcPtr);

                    ASSERTV(SPEC1, SPEC2, oa.numBlocksInUse(),
                            empty || ((isPropagate || &sa == &oa) ==
                                                   (0 < sa.numBlocksInUse())));

                    // Verify subsequent manipulation of target object 'X'.
                    RESULT = primaryManipulator(&mX, 'Z', soa);
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
            ALLOC soa(&oa);

            {
                Obj mX(soa);  const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(sscratch);  const Obj& ZZ  = gg(&mZZ,  SPEC1);

                const Obj& Z = mX;

                ASSERTV(SPEC1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch), dam(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = MoveUtil::move(mX));
                    ASSERTV(SPEC1, ZZ,   Z, ZZ == Z);
                    ASSERTV(SPEC1, mR,  &X, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(SPEC1, soa == Z.get_allocator());

                ASSERTV(SPEC1, sam.isTotalSame());
                ASSERTV(SPEC1, oam.isTotalSame());
                ASSERTV(SPEC1, dam.isTotalSame());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(SPEC1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
    if (veryVerbose)
        printf("\nTesting move assignment with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_results;
            const size_t      LENGTH1 = strlen(SPEC1);

            if (ti && INDEX1 == DATA[ti-1].d_index) {
                // redundant, skip

                continue;
            }

            if (LENGTH1 > 4) {
                // this will just take too long to test with exceptions, skip

                continue;
            }

            Obj  mZZ(sscratch); const Obj&  ZZ = gg(&mZZ, SPEC1);

            if (veryVeryVerbose) { T_ P_(LINE1) P(ZZ) }

            // Create second object
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_results;

                if (tj && INDEX2 == DATA[tj-1].d_index) {
                    // redundant, skip

                    continue;
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator ra("different", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);

                    ALLOC sra(&ra);
                    ALLOC soa(&oa);

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        bsls::Types::Int64 al = oa.allocationLimit();
                        oa.setAllocationLimit(-1);
                        Obj mX(soa); const Obj& X = gg(&mX, SPEC2);

                        bslma::TestAllocator *srcAllocatorPtr;
                        switch (CONFIG) {
                          case 'a': {
                            srcAllocatorPtr = &ra;
                          } break;
                          case 'b': {
                            srcAllocatorPtr = &oa;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"Bad allocator config.");
                          } return;                                   // RETURN
                        }
                        bslma::TestAllocator& sa = *srcAllocatorPtr;
                        ALLOC ssa(&sa);

                        Obj mZ(ssa); const Obj& Z = gg(&mZ, SPEC1);

                        if (veryVeryVerbose) { T_ P_(LINE2) P(Z) }
                        if (veryVeryVerbose) { T_ P_(LINE2) P(X) }

                        ASSERTV(SPEC1, SPEC2, Z, X,
                                (Z == X) == (INDEX1 == INDEX2));

                        oa.setAllocationLimit(al);

                        Obj mE(sscratch);
                        if (&sa != &oa
                         && oa.allocationLimit() >= 0
                         && oa.allocationLimit() <= isTypeAlloc) {
                            // We will throw on the reserveNodes so that source
                            // object will be unchanged on exception.
                            gg(&mE, SPEC1);
                        }
                        // The else here is that the source object will be made
                        // empty on exception.
                        ExceptionProctor<Obj,ALLOC>
                                           proctor(&Z, L_, MoveUtil::move(mE));

                        Obj *mR = &(mX = MoveUtil::move(mZ));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2,  X,  ZZ,  X ==  ZZ);

                        proctor.release();

                        al = oa.allocationLimit();
                        oa.setAllocationLimit(-1);

                        // Manipulate source object 'Z' to ensure it is in a
                        // valid state and is independent of 'X'.
                        pair<Iter, bool> RESULT =
                                       primaryManipulator(&mZ, 'Z', ssa);
                        ASSERTV(true == RESULT.second);
                        ASSERTV(SPEC1, SPEC2, Z, 1 == Z.size());
                        ASSERTV(SPEC1, SPEC2, Z, ZZ, Z != ZZ);
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X == ZZ);

                        // Verify subsequent manipulation of target object 'X'.
                        RESULT = primaryManipulator(&mX, 'Z', soa);
                        ASSERTV(true == RESULT.second);
                        ASSERTV(SPEC1, SPEC2, LENGTH1 + 1, X.size(),
                                LENGTH1 + 1 == X.size());
                        ASSERTV(SPEC1, SPEC2, X, ZZ, X != ZZ);

                        oa.setAllocationLimit(al);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV((!PLAT_EXC || 0 == ti || 'b' == CONFIG ||
                                                                   isPropagate)
                                                          == (1 == numPasses));
                }
            }
        }
    }

    ASSERTV((isTypeAlloc && e_STATEFUL == s_allocCategory) ||
                                                     0 == da.numAllocations());
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase26()
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
    //   set(bslmf::MovableRef<set> original);
    //   set(bslmf::MovableRef<set> original, const A& allocator);
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

    const size_t NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (verbose)
        printf("\tTesting both versions of move constructor.\n");
    {
        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
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
                storeFirstNElemAddr(pointers, Z,
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

                KeyAllocator        xoa(&oa);

                Obj& mX = *objPtr; const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == ZZ);

                // Verify that the comparator was copied.
                ASSERTV(SPEC, CONFIG,
                        X.key_comp().count() == Z.key_comp().count());

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
                pair<Iter, bool> RESULT = primaryManipulator(&mZ, 'Z', xsa);
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
        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
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
                if (oa.allocationLimit() >= 0
                 && oa.allocationLimit() <= TYPE_ALLOC) {
                    // We will throw on the reserveNodes so that source object
                    // will be unchanged on exception.
                    gg(&mE, SPEC);
                }
                // The else here is that the source object will be made empty
                // on exception.
                Obj mZ(xda);  const Obj& Z = gg(&mZ, SPEC);

                ExceptionProctor<Obj, ALLOC>
                                           proctor(&Z, L_, MoveUtil::move(mE));

                Obj mX(MoveUtil::move(mZ), xoa);
                const Obj& X = mX;

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
    //: 1 The type provides the full interface defined by the section
    //:   '[set.overview]' in the C++11 standard, with exceptions for methods
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    // construct/copy/destroy:

    // explicit set(const Compare& comp = Compare(),
    //              const Allocator& = Allocator());
    bsl::set<KEY, COMP, StlAlloc> A((COMP()), (StlAlloc()));


    // template <class InputIterator>
    // set(InputIterator first, InputIterator last,
    // const Compare& comp = Compare(), const Allocator& = Allocator());

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    TestValues           values("ABC", &scratch);

    bsl::set<KEY, COMP, StlAlloc> B(values.begin(),
                                    values.end(),
                                    COMP(),
                                    StlAlloc());

    // set(const set<Key,Compare,Allocator>& x);
    bsl::set<KEY, COMP, StlAlloc> C(B);

    // set(set<Key,Compare,Allocator>&& x); <- C++11 only

    // explicit set(const Allocator&);
    bsl::set<KEY, COMP, StlAlloc> D((StlAlloc()));

    // set(const set&, const Allocator&);
    bsl::set<KEY, COMP, StlAlloc> E(B, StlAlloc());

    // set(set&&, const Allocator&); <- C++11 only

    // set(initializer_list<value_type>, const Compare& = Compare(), const
    // Allocator& = Allocator()); <- C++11 only

    // ~set();
    // destructor always exist


    //set<Key,Compare,Allocator>& operator=
    // (const set<Key,Compare,Allocator>& x);
    Obj& (Obj::*operatorAg) (const Obj&) = &Obj::operator=;
    (void) operatorAg;  // quash potential compiler warning

    // set<Key,Compare,Allocator>& operator=(set<Key,Compare,Allocator>&&
    // x); <- C++11 only

    // set& operator=(initializer_list<value_type>); <- C++11 only

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

    // template <class... Args> pair<iterator, bool> emplace(Args&&... args);
    // <- C++11 only

    // template <class... Args> iterator emplace_hint(const_iterator position,
    // Args&&... args); <- C++11 only

    // pair<iterator, bool> insert(const value_type& x);

    bsl::pair<typename Obj::iterator, bool> (Obj::*methodInsert) (
                               const typename Obj::value_type&) = &Obj::insert;
    (void) methodInsert;

    // template <class P> pair<iterator, bool> insert(P&& x); <- C++11 only

    // iterator insert(const_iterator position, const value_type& x);
    typename Obj::iterator (Obj::*methodInsert2) (
        typename Obj::const_iterator, const typename Obj::value_type&) =
                                                                  &Obj::insert;
    (void) methodInsert2;

    // template <class P> iterator insert(const_iterator position, P&&); <-
    // C++11 only

    // template <class InputIterator>
    //    void insert(InputIterator first, InputIterator last);
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

    // void swap(map<Key,T,Compare,Allocator>&);
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

    // set operations:
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
    bsl::pair<typename Obj::iterator, typename Obj::iterator> (
        Obj::*methodEqualRange) (const typename Obj::key_type&) =
                                                             &Obj::equal_range;
    (void) methodEqualRange;

    // pair<const_iterator,const_iterator> equal_range(const key_type& x)
    // const;
    bsl::pair<typename Obj::const_iterator, typename Obj::const_iterator> (
        Obj::*methodEqualRangeConst) (const typename Obj::key_type&) const =
                                                             &Obj::equal_range;
    (void) methodEqualRangeConst;

    using namespace bsl;

    // template <class Key, class Compare, class Allocator>
    // bool operator==(const set<Key,Compare,Allocator>& x,
    // const set<Key,Compare,Allocator>& y);
    bool (*operatorEq)(const Obj&, const Obj&) = operator==;
    (void) operatorEq;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    (void) [](const Obj& lhs, const Obj& rhs) -> bool {
        return lhs != rhs;
    };
#else
    // template <class Key, class Compare, class Allocator>
    // bool operator!=(const set<Key,Compare,Allocator>& x,
    // const set<Key,Compare,Allocator>& y);
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
    // bool operator< (const set<Key,Compare,Allocator>& x,
    // const set<Key,Compare,Allocator>& y);
    bool (*operatorLt)(const Obj&, const Obj&) = operator<;
    (void) operatorLt;

    // template <class Key, class Compare, class Allocator>
    // bool operator> (const set<Key,Compare,Allocator>& x,
    // const set<Key,Compare,Allocator>& y);
    bool (*operatorGt)(const Obj&, const Obj&) = operator>;
    (void) operatorGt;

    // template <class Key, class Compare, class Allocator>
    // bool operator>=(const set<Key,Compare,Allocator>& x,
    // const set<Key,Compare,Allocator>& y);
    bool (*operatorGe)(const Obj&, const Obj&) = operator>=;
    (void) operatorGe;

    // template <class Key, class Compare, class Allocator>
    // bool operator<=(const set<Key,Compare,Allocator>& x,
    // const set<Key,Compare,Allocator>& y);
    bool (*operatorLe)(const Obj&, const Obj&) = operator<=;
    (void) operatorLe;
#endif  // BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE

    // specialized algorithms:
    // template <class Key, class Compare, class Allocator>
    // void swap(set<Key,Compare,Allocator>& x,
    // set<Key,Compare,Allocator>& y);

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
    //: 1 The constructor of a templatized wrapper around the container will
    //:   compile.  (C-1)
    //
    // Plan:
    //: 1 Invoke each constructor of a class that inherits from the container.
    //
    // Testing:
    //   CONCERN: Constructor of a template wrapper class compiles
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

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
    BSLMF_ASSERT((1 == bslalg::HasStlIterators<bsl::set<KEY> >::value));
    BSLMF_ASSERT((1 == bslma::UsesBslmaAllocator<bsl::set<KEY> >::value));

    // Verify the bslma-allocator trait is not defined for non
    // bslma-allocators.
    BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<bsl::set<KEY, std::less<KEY>,
                                                          StlAlloc> >::value));

    // Verify set does not define other common traits.
    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<bsl::set<KEY> >::value));

    BSLMF_ASSERT((0 ==
                  bslmf::IsBitwiseEqualityComparable<bsl::set<KEY> >::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseMoveable<bsl::set<KEY> >::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<bsl::set<KEY> >::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<
                                                     bsl::set<KEY> >::value));
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase22()
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

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef bsl::set<KEY, COMP, StlAlloc> Obj;

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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
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
                bsl::pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
            }
            ASSERTV(LINE, 0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(LINE, da.numBlocksInUse(),
                    TYPE_ALLOC * LENGTH == da.numBlocksInUse());
        }

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

    // IBM empty class swap bug test

    {
        typedef bsl::set<int, std::less<int>, StlAlloc> TestObj;
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
    //:     passing in the comparator created in P-5.1.  Verify the object's
    //:     value.  (P-5)
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
    //:   operator.  (P-7)
    //
    // Testing:
    //   key_compare key_comp() const;
    //   value_compare value_comp() const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    {
        bsl::set<int>  X;
        std::less<int> keyComparator   = X.key_comp();
        std::less<int> valueComparator = X.value_comp();

        (void) keyComparator;  // quash potential compiler warning
        (void) valueComparator;  // quash potential compiler warning
    }

    {
        typedef bool (*ComparatorFunction)(const KEY&, const KEY& rhs);
        bsl::set<KEY, ComparatorFunction> X(&lessThanFunction<KEY>);
        ASSERTV((ComparatorFunction)&lessThanFunction<KEY> == X.key_comp());
        ASSERTV((ComparatorFunction)&lessThanFunction<KEY> == X.value_comp());
    }

    static const int ID[] = { 0, 1, 2 };
    const int        NUM_ID = sizeof ID / sizeof *ID;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

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
        int         d_line;     // source line number
        const char *d_spec;     // spec
        const char *d_results;  // expected result
    } DATA[] = {
        //line spec          result
        //---- -----------   -----------
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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
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
                bsl::pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const NonConstComp C(1, false);  // create non const comparator that
                                         // uses the greater than operator
        typedef bsl::set<KEY, NonConstComp> ObjNCC;

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
                bsl::pair<Iter, bool> RESULT = mX.insert(CONT[tj]);

                ASSERTV(LINE, tj, LENGTH, CONT[tj] == *(RESULT.first));
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
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_lineNum;
            const char *SPEC = DATA[ti].d_spec;

            bslma::TestAllocator  oa(veryVeryVerbose);

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
    //: 1 'operator<' returns the lexicographic comparison on two containers.
    //:
    //: 2 Comparison operator uses 'operator<' on 'key_type' instead of the
    //:   supplied comparator.
    //:
    //: 3 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //:   'operator<'.  i.e., For two sets, 'a' and 'b':
    //:
    //:   1 '(a > b) == (b < a)'
    //:
    //:   2 '(a <= b) == !(b < a)'
    //:
    //:   3 '(a >= b) == !(a < b)'
    //:
    //: 4 'operator<=>' is consistent with '<', '>', '<=', '>='.
    //
    // Plan:
    //: 1 For a variety of objects of different sizes and different values,
    //:   test that the comparison returns as expected.  (C-1..4)
    //
    // Testing:
    //   bool operator< (const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    //   bool operator> (const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    //   bool operator>=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    //   bool operator<=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    //   auto operator<=>(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

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

                // Create two distinct test allocators, 'oax' and 'oay'.

                bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                // Map allocators above to objects 'X' and 'Y' below.

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

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\tTesting 'erase(pos)' on non-empty set.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj& X = gg(&mX, SPEC);

                CIter POS = X.find(VALUES[tj]);
                CIter AFTER = (tj == LENGTH - 1)
                            ? X.end()
                            : X.find(VALUES[tj + 1]);
                CIter BEFORE = (tj == 0) ? X.end() : X.find(VALUES[tj - 1]);

                ASSERTV(LINE, tj, POS != X.end());
                ASSERTV(LINE, tj, AFTER  != POS);
                ASSERTV(LINE, tj, BEFORE != POS);

                if (veryVerbose) { T_ T_ T_ P(*POS); }

                bslma::TestAllocatorMonitor oam(&oa);

                const Iter R = mX.erase(POS);

                if (veryVeryVerbose) { T_ T_ P(X); }

                ASSERTV(LINE, tj, AFTER == R); // Check return value

                // Check the element does not exist

                ASSERTV(LINE, tj, X.end() == X.find(VALUES[tj]));
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

    if (verbose) printf("\tTesting 'erase(key)' on non-empty set.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocator oa("object", veryVeryVerbose);
                KeyAllocator         xoa(&oa);

                Obj mX(xoa); const Obj& X = gg(&mX, SPEC);

                CIter AFTER = (tj == LENGTH - 1)
                            ? X.end()
                            : X.find(VALUES[tj + 1]);
                CIter BEFORE = (tj == 0) ? X.end() : X.find(VALUES[tj - 1]);

                if (veryVerbose) { T_ T_ T_ P(tj); }

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, tj, 1 == mX.erase(VALUES[tj]));

                if (veryVeryVerbose) {
                    T_ T_ P(X);
                }

                // Check the element does not exist

                ASSERTV(LINE, tj, X.end() == X.find(VALUES[tj]));
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

                // Erase a non-existing element.
                ASSERTV(LINE, tj, 0 == mX.erase(VALUES[tj]));
                ASSERTV(LINE, tj, X.size(), LENGTH - 1 == X.size());
            }
        }
    }

    if (verbose) printf("\tTesting 'erase(first, last)'.\n");
    {
        for (size_t ti = 1; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  VALUES(DATA[ti].d_results);

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (size_t tj = 0;  tj <= LENGTH; ++tj) {
                for (size_t tk = tj; tk <= LENGTH; ++tk) {
                    bslma::TestAllocator oa("object", veryVeryVerbose);
                    KeyAllocator         xoa(&oa);

                    Obj mX(xoa); const Obj& X = gg(&mX, SPEC);

                    CIter FIRST = (tj == LENGTH) ? X.end()
                                                 : X.find(VALUES[tj]);
                    CIter LAST  = (tk == LENGTH) ? X.end()
                                                 : X.find(VALUES[tk]);
                    const size_t NUM_ELEMENTS = tk - tj;

                    const CIter AFTER  = LAST;
                    const CIter BEFORE = (tj == 0)
                                       ? X.end()
                                       : X.find(VALUES[tj - 1]);

                    if (veryVerbose) {
                        if (FIRST != X.end()) {
                            T_ T_ T_ P(*FIRST)
                        }
                        if (LAST != X.end()) {
                            T_ T_ T_ P(*LAST);
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
                    ASSERTV(LINE, tj, tk, LENGTH,
                            LENGTH == X.size() + NUM_ELEMENTS);
                }
            }
        }
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

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
    // RANGE 'insert'
    //
    // Concern:
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

            bslma::TestAllocator         oa("object", veryVeryVerbose);
            KeyAllocator                 xoa(&oa);

            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(BEGIN, MID, COMP(), xoa);  const Obj& X = mX;

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

            ASSERTV(LINE, tj, oa.numBlocksTotal(), oa.numBlocksInUse(),
                    oa.numBlocksTotal() == oa.numBlocksInUse());

            ASSERTV(LINE, tj, da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION WITH HINT
    //
    // Concerns:
    //: 1 'insert' returns an iterator referring to the newly inserted element
    //:   if it did not already exists, and to the existing element if it did.
    //:
    //: 2 A new element is added to the container if the element did not
    //:   already exist, and the order of the container remains correct.
    //:
    //: 3 Inserting with the correct hint places the new element right before
    //:   the hint.
    //:
    //: 4 Inserting with the correct hint requires no more than 2 comparisons.
    //:
    //: 5 Incorrect hint will be ignored and 'insert' will proceed as if the
    //:   hint is not supplied.
    //:
    //: 6 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 Insertion is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //: 1 For insertion we will create objects of varying sizes and capacities
    //:   containing default values, and insert a 'value'.
    //:
    //:   1 For each set of values, set hint to be 'lower_bound', 'begin',
    //:     'begin' + 1, 'end' - 1, 'end'
    //:
    //:     1 For each value in the set, 'insert' the value with hint.
    //:
    //:       1 Compute the number of allocations and verify it is as
    //:         expected.
    //:
    //:       2 Verify the return value and the resulting data in the container
    //:         is as expected.
    //:
    //:       3 Verify the new element is inserted right before the hint if
    //:         the hint is valid.  (C-3)
    //:
    //:       4 Verify the number of comparisons is no more than 2 if the hint
    //:         is valid.  (C-4)
    //:
    //:       5 Verify all allocations are from the object's allocator.  (C-6)
    //:
    //: 2 Repeat P-1 under the presence of exception  (C-7)
    //
    // Testing:
    //   iterator insert(const_iterator position, const value_type& value);
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
    const size_t NUM_DATA   = sizeof DATA / sizeof *DATA;
    const size_t MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'insert' with hint.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

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
                          ASSERTV(!"Unexpected configuration");
                      }
                    }

                    size_t EXP_COMP = X.key_comp().count();
                    if (CONFIG == 'a') {
                        if (!IS_UNIQ) {
                            EXP_COMP += 2;
                        }
                        else {
                            if (hint != X.begin()) {
                                ++EXP_COMP;
                            }
                            if (hint != X.end()) {
                                ++EXP_COMP;
                            }
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

                        if (IS_UNIQ) {
                            ASSERTV(LINE, tj, hint == ++RESULT);
                        }
                        else {
                            ASSERTV(LINE, tj, hint == RESULT);
                        }
                    }

                    if (IS_UNIQ) {
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
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB == AA);
                        ASSERTV(LINE, tj, A,  B,  B == A);
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
    }

    if (verbose) printf("\tTesting 'insert' with exception.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const char *const UNIQUE = DATA[ti].d_unique;
            const size_t      LENGTH = strlen(SPEC);

            const TestValues VALUES(SPEC);

            char EXPECTED[MAX_LENGTH];

            if (veryVerbose) { T_ T_ P_(LINE) P_(SPEC) P_(UNIQUE) P(LENGTH); }

            ASSERTV(LINE, LENGTH == strlen(UNIQUE));

            for (char cfg = 'a'; cfg <= 'e'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);
                Obj                  mX(xoa);
                const Obj&           X = mX;

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    const bool   IS_UNIQ = UNIQUE[tj] == 'Y';
                    const size_t SIZE    = X.size();

                    if (veryVerbose) { T_ T_ P_(IS_UNIQ) P(SIZE); }

                    if (IS_UNIQ) {
                        EXPECTED[SIZE] = SPEC[tj];
                        std::sort(EXPECTED, EXPECTED + SIZE + 1);
                        EXPECTED[SIZE + 1] = '\0';

                        if (veryVeryVerbose) { P(EXPECTED); }
                    }

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
                          ASSERTV(!"Unexpected configuration");
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

                    if (IS_UNIQ) {
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
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB == AA);
                        ASSERTV(LINE, tj, A,  B,  B == A);
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
    }

}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING INSERTION
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
    //:   already exist, and the order of the container remains correct.
    //:
    //: 5 Internal memory management system is hooked up properly
    //:   so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 Insertion is exception neutral w.r.t. memory allocation.
    //:
    //: 7 QoI: Range insertion allocates a single block for nodes when the
    //:   number of elements can be determined and no free nodes are available.
    //:   (The contained elements may require additional allocations.)
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
    //:
    //: 3 Invoke the 'testRangeInsertOptimization' function that creates a
    //:   container for a non-allocating type that uses a test allocator to
    //:   supply memory.  'insert' elements using both forward and random
    //:   access iterators.  'clear' the elements and re-insert them.  Compare
    //:   the state of the allocator to expected memory needs at each step of
    //:   the scenario.  (C-7).
    //
    // Testing:
    //   bsl::pair<iterator, bool> insert(const value_type& value);
    //   void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
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
    const size_t NUM_DATA   = sizeof DATA / sizeof *DATA;
    const size_t MAX_LENGTH = 10;

    if (verbose) printf("\tTesting 'insert' without exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
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

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
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
                    if (((SIZE + 1) & SIZE) == 0) {
                        ASSERTV(LINE, tj, AA, BB, BB + 1 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 1 + TYPE_ALLOC == A);
                    }
                    else {
                        ASSERTV(LINE, tj, AA, BB, BB + 0 + TYPE_ALLOC == AA);
                        ASSERTV(LINE, tj, A, B, B + 0 + TYPE_ALLOC == A);
                    }
                    ASSERTV(LINE, tj, SIZE, SIZE + 1 == X.size());
                    TestValues exp(EXPECTED);
                    ASSERTV(LINE, tj,
                            0 == verifyContainer(X,
                                                 exp,
                                                 SIZE + 1));
                }
                else {
                    ASSERTV(LINE, tj, AA, BB, BB == AA);
                    ASSERTV(LINE, tj, A,  B,  B == A);
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

    if (verbose) printf("\tTesting 'insert' with injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
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

                if (IS_UNIQ) {
                    EXPECTED[SIZE] = SPEC[tj];
                    std::sort(EXPECTED, EXPECTED + SIZE + 1);
                    EXPECTED[SIZE + 1] = '\0';

                    if (veryVeryVerbose) { P(EXPECTED); }
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionProctor<Obj, ALLOC> proctor(&X, L_, xscratch);

                    bsl::pair<Iter, bool> RESULT = mX.insert(VALUES[tj]);

                    ASSERTV(LINE, tj, SIZE, IS_UNIQ == RESULT.second);
                    ASSERTV(LINE, tj, SIZE,
                            VALUES[tj] == *(RESULT.first));

                    proctor.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //: 1 'begin' and 'end' return non-mutable iterators.
    //:
    //: 2 The range '[begin(), end())' contains all values inserted into the
    //:   container in ascending order.
    //:
    //: 3 The range '[rbegin(), rend())' contains all values inserted into the
    //:   container in descending order.
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
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin();
    //   const_iterator end();
    //   const_reverse_iterator rbegin();
    //   const_reverse_iterator rend();
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const TestValues VALUES;

    bslma::TestAllocator oa(veryVeryVerbose);

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // initial
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
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'iterator', 'begin', and 'end',"
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

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (verbose) { T_ P_(LINE); P(SPEC); }

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

    if (verbose) printf("\tTesting 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
        ASSERTV(1 == (bsl::is_same<RIter,
                                   bsl::reverse_iterator<Iter> >::value));
        ASSERTV(1 == (bsl::is_same<CRIter,
                                   bsl::reverse_iterator<CIter> >::value));

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *SPEC   = DATA[ti].d_spec;
            const int   LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (verbose) { T_ P_(LINE); P(SPEC); }

            int i = LENGTH - 1;
            for (RIter riter = mX.rbegin(); riter != mX.rend(); ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);

            i = LENGTH - 1;
            for (CRIter riter = X.rbegin(); riter != X.rend(); ++riter, --i) {
                ASSERTV(LINE, VALUES[i] == *riter);
            }
            ASSERTV(LINE, -1 == i);

            i = LENGTH - 1;
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
    // TESTING SEARCH FUNCTIONS
    //
    // Concern:
    //: 1 If the key being searched exists in the container, 'find' and
    //:   'lower_bound' returns the iterator referring to the existing element,
    //:   'contains' returns 'true', and 'upper_bound' returns the iterator to
    //:   the element after the searched element.
    //:
    //: 2 If the key being searched does not exists in the container, 'find'
    //:   returns the 'end' iterator, 'contains' returns 'false', 'lower_bound'
    //:   and 'upper_bound' return the iterator to the smallest element
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
    //   iterator lower_bound(const key_type& key);
    //   const_iterator lower_bound(const key_type& key) const;
    //   iterator upper_bound(const key_type& key);
    //   const_iterator upper_bound(const key_type& key) const;
    //   bsl::pair<iterator, iterator> equal_range(const key_type& key);
    //   bsl::pair<const_iter, const_iter> equal_range(const key_type&) const;
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const TestValues VALUES;  // contains 52 distinct increasing values

    const int MAX_LENGTH = 17;

    {
        for (int ti = 0; ti < MAX_LENGTH; ++ti) {
            const int LENGTH = ti;

            CIter CITER[MAX_LENGTH];
            Iter  ITER[MAX_LENGTH];

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object", veryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);  const Obj& X = mX;

            for (int i = 0; i < LENGTH; ++i) {
                int idx = 2 * i + 1;
                bsl::pair<Iter, bool> RESULT = mX.insert(VALUES[idx]);

                ASSERTV(ti, i, true == RESULT.second);
                ASSERTV(ti, i, VALUES[idx] == *RESULT.first);

                CITER[i] = RESULT.first;
                ITER[i] = RESULT.first;
            }
            CITER[LENGTH] = X.end();
            ITER[LENGTH] = mX.end();
            ASSERTV(ti, LENGTH == (int) X.size());

            bslma::TestAllocatorMonitor oam(&oa);

            for (int tj = 0; tj <= 2 * LENGTH; ++tj) {
                if (1 == tj % 2) {
                    const int idx = tj / 2;
                    ASSERTV(ti, tj, CITER[idx] == X.find(VALUES[tj]));
                    ASSERTV(ti, tj, ITER[idx] == mX.find(VALUES[tj]));

                    bool cShouldBeFound = CITER[idx] != X.end();
                    ASSERTV(
                           ti,
                           tj,
                           cShouldBeFound,
                           cShouldBeFound == X.contains(VALUES[tj]));

                    bool shouldBeFound  = ITER[idx] != mX.end();
                    ASSERTV(cShouldBeFound,
                            shouldBeFound,
                            cShouldBeFound == shouldBeFound);

                    ASSERTV(
                           ti,
                           tj,
                           shouldBeFound,
                           shouldBeFound == mX.contains(VALUES[tj]));

                    ASSERTV(ti, tj, CITER[idx] == X.lower_bound(VALUES[tj]));
                    ASSERTV(ti, tj, ITER[idx] == mX.lower_bound(VALUES[tj]));
                    ASSERTV(ti, tj,
                            CITER[idx + 1] == X.upper_bound(VALUES[tj]));
                    ASSERTV(ti, tj,
                            ITER[idx + 1] == mX.upper_bound(VALUES[tj]));

                    bsl::pair<CIter, CIter> R1 = X.equal_range(VALUES[tj]);
                    ASSERTV(ti, tj, CITER[idx] == R1.first);
                    ASSERTV(ti, tj, CITER[idx + 1] == R1.second);

                    bsl::pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj]);
                    ASSERTV(ti, tj, ITER[idx] == R2.first);
                    ASSERTV(ti, tj, ITER[idx + 1] == R2.second);

                    ASSERTV(ti, tj, 1 == mX.count(VALUES[tj]));
                }
                else {
                    const int idx = tj / 2;
                    ASSERTV(ti, tj, X.end() == X.find(VALUES[tj]));
                    ASSERTV(ti, tj, mX.end() == mX.find(VALUES[tj]));
                    ASSERTV(ti, tj, CITER[idx] == X.lower_bound(VALUES[tj]));
                    ASSERTV(ti, tj, ITER[idx] == mX.lower_bound(VALUES[tj]));
                    ASSERTV(ti, tj, CITER[idx] == X.upper_bound(VALUES[tj]));
                    ASSERTV(ti, tj, ITER[idx] == mX.upper_bound(VALUES[tj]));

                    bsl::pair<CIter, CIter> R1 = X.equal_range(VALUES[tj]);
                    ASSERTV(ti, tj, CITER[idx] == R1.first);
                    ASSERTV(ti, tj, CITER[idx] == R1.second);

                    bsl::pair<Iter, Iter> R2 = mX.equal_range(VALUES[tj]);
                    ASSERTV(ti, tj, ITER[idx] == R2.first);
                    ASSERTV(ti, tj, ITER[idx] == R2.second);

                    ASSERTV(ti, tj, 0 == mX.count(VALUES[tj]));
                }
            }
            ASSERTV(ti, oam.isTotalSame());
            ASSERTV(ti, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING RANGE (TEMPLATE) CONSTRUCTORS
    //
    // Concern:
    //: 1 All values within the range [first, last) are inserted.
    //:
    //: 2 Each iterator is dereferenced only once.
    //:
    //: 3 Repeated values are ignored.
    //:
    //: 4 If an allocator is NOT supplied to the value constructor, the
    //:   default allocator in effect at the time of construction becomes
    //:   the object allocator for the resulting object.
    //:
    //: 5 If an allocator IS supplied to the value constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 6 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 7 Supplying an allocator to the value constructor has no effect
    //:   on subsequent object values.
    //:
    //: 8 Constructing from an ordered list requires linear time.
    //:
    //: 9 Any memory allocation is from the object allocator.
    //:
    //:10 There is no temporary memory allocation from any allocator.
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 QoI: Creating an object having the default-constructed value
    //:   allocates no memory.
    //:
    //:13 Any memory allocation is exception neutral.
    //:
    //:14 QoI: Range insertion allocates a single block for nodes when the
    //:   number of elements can be determined and no free nodes are available.
    //:   (The contained elements may require additional allocations.)
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating three distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently:
    //:     (a) without passing an allocator, (b) passing a null allocator
    //:     address explicitly, and (c) passing the address of a test allocator
    //:     distinct from the default allocator.
    //:
    //:   2 For each of the three iterations in P-2.1:
    //:
    //:     1 Insert the test data to a specialized container that have returns
    //:       standard conforming input iterators.
    //:
    //:     2 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     3 Use the value constructor to dynamically create an object using
    //:       'begin' and 'end' from the container in P-2.1, with its object
    //:       allocator configured appropriately (see P-2.2), supplying all the
    //:       arguments as 'const'; use a distinct test allocator for the
    //:       object's footprint.
    //:
    //:     4 Verify that all of the attributes of each object have their
    //:       expected values.
    //:
    //:     5 Use the 'allocator' accessor of each underlying attribute capable
    //:       of allocating memory to ensure that its object allocator is
    //:       properly installed; also invoke the (as yet unproven) 'allocator'
    //:       accessor of the object under test.
    //:
    //:     6 Use the appropriate test allocators to verify that:
    //:
    //:       1 An object that IS expected to allocate memory does so from the
    //:         object allocator only (irrespective of the specific number of
    //:         allocations or the total amount of memory allocated).
    //:
    //:       2 An object that is expected NOT to allocate memory does not
    //:         allocate memory.
    //:
    //:       3 If an allocator was supplied at construction (P-2.1c), the
    //:         default allocator doesn't allocate any memory.
    //:
    //:       4 If the input range is ordered, verify the number of comparisons
    //:         is equal to 'LENGTH - 1', where 'LENGTH' is the number of
    //:         elements in the input range.
    //:
    //:       5 No temporary memory is allocated from the object allocator.
    //:
    //:       6 All object memory is released when the object is destroyed.
    //:
    //: 3 Repeat the steps in P-2 for the supplied allocator configuration
    //:   (P-2.1c) on the data of P-1, but this time create the object as an
    //:   automatic variable in the presence of injected exceptions (using the
    //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros); represent any string
    //:   arguments in terms of 'string' using a "scratch" allocator.
    //:
    //: 4 Invoke the 'testRangeCtorOptimization' function that creates a
    //:   containers for a non-allocating type using both random access and
    //:   forward iterators.  Each container is given a test allocator to
    //:   supply memory.  The test allocator is state compared to the expected
    //:   state in each step of the scenario.  (C-14)
    //
    // Testing:
    //   set(ITER first, ITER last);
    //   set(ITER first, ITER last, const A& allocator);
    //   set(ITER first, ITER last, const C& comparator, const A& allocator);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    static const struct {
        int         d_line;         // source line number
        const char *d_spec;         // specification string
        const char *d_results;      // expected element values
        bool        d_orderedFlag;  // is the spec in ascending order
    } DATA[] = {
        //line  spec          elements  ordered
        //----  -----------   --------  -------
        { L_,   "",           "",       true   },
        { L_,   "A",          "A",      true   },
        { L_,   "AB",         "AB",     true   },
        { L_,   "ABC",        "ABC",    true   },
        { L_,   "ABCD",       "ABCD",   true   },
        { L_,   "AABCCDDD",   "ABCD",   true   },
        { L_,   "ABCDE",      "ABCDE",  true   },
        { L_,   "DABEC",      "ABCDE",  false  },
        { L_,   "EDCBACBA",   "ABCDE",  false  },
        { L_,   "ABCDEABCD",  "ABCDE",  false  }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting without injected exceptions.\n");
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const size_t      LENGTH  = strlen(DATA[ti].d_results);
            const bool        ORDERED = DATA[ti].d_orderedFlag;
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

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

                Obj                  *objPtr;
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
                      ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma::TestAllocator& oa  = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' >= CONFIG ? sa : da;

                // Use untested functionality to help ensure the first row of
                // the table contains the default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(LINE, CONFIG, Obj(), *objPtr, Obj() == *objPtr);
                    firstFlag = false;
                }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, &oa == X.get_allocator());

                if (ORDERED && LENGTH > 0) {
                    const size_t SPECLEN = strlen(DATA[ti].d_spec);
                    ASSERTV(LINE, CONFIG, (SPECLEN - 1) * 2,
                            X.key_comp().count(),
                            (SPECLEN - 1) * 2 == X.key_comp().count());

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

    if (verbose) printf("\tTesting with injected exceptions.\n");
    {
        // Note that any string arguments are now of type 'string', which
        // require their own "scratch" allocator.

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_line;
            const char        *SPEC   = DATA[ti].d_spec;
            const size_t       LENGTH = strlen(DATA[ti].d_results);
            const TestValues   EXP(DATA[ti].d_results);

            TestValues CONT(SPEC);
            typename TestValues::iterator BEGIN = CONT.begin();
            typename TestValues::iterator END   = CONT.end();

            if (verbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

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

template <class KEY,
          class COMP = TestComparator<KEY> >
struct MetaTestDriver {
    // This 'struct' is to be call by the 'RUN_EACH_TYPE' macro, and the
    // functions within it dispatch to functions in 'TestDriver' instantiated
    // with different types of allocator.

    typedef bsl::allocator<KEY>             BAP;
    typedef bsltf::StdAllocatorAdaptor<BAP> SAA;

    static void testCase27();
        // Test move-assign.

    static void testCase8();
        // Test member and free 'swap'.
};

template <class KEY, class COMP>
void MetaTestDriver<KEY, COMP>::testCase27()
{
    // The low-order bit of the identifier specifies whether the fourth boolean
    // argument of the stateful allocator, which indicates propagate on
    // move-assign, is set.

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

typedef int           DataType;
typedef bsl::set<int> ContainerType;

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

    mX.insert(beginFwd, endFwd);        // Insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());   // Added elements.
    ASSERT(1            == sam.numBlocksTotalChange());
                                        // Had to allocate nodes.
                                        // No free nodes left.

    mX.insert(beginFwd, endFwd);       // Re-insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());  // No-change since already in map.
    ASSERT(2            == sam.numBlocksTotalChange());
                                       // No free nodes so allocated more;
                                       // however, did not use them.

    mX.insert(beginFwd, endFwd);       // Re-re-insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());  // No-change since already in map.
    ASSERT(2            == sam.numBlocksTotalChange());
                                       // Have free nodes so no new allocation.
                                       // The free nodes remain unused.

    mX.insert(beginFwd, endFwd);       // Re-re-re-insert entire range.
    ASSERT(NUM_ELEMENTS == X.size());  // No-change since already in map.
    ASSERT(2            == sam.numBlocksTotalChange());
                                       // Have free nodes so no new allocation.
                                       // The free nodes remain unused.
    // ...

    ContainerType mY(&sa); const ContainerType& Y = mY;

    bslma::TestAllocatorMonitor sam2(&sa);

    ASSERT(0              == sam2.numBlocksInUseChange());

    mY.insert(beginFwd, midFwd);        // Insert first half of 'DATA'.
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
struct TestDeductionGuides {
    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::set' cannot be deduced from the constructor parameters.
    //..
    // set()
    // explicit set(COMPARATOR, ALLOCATOR = ALLOCATOR());
    // set(ALLOCATOR)
    //..

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    void SimpleConstructors ()
        // Test that constructing a 'bsl::set' from various combinations of
        // arguments deduces the correct type.
        //..
        // set(const set&  s)            -> decltype(s)
        // set(const set&  s, ALLOCATOR) -> decltype(s)
        // set(      set&& s)            -> decltype(s)
        // set(      set&& s, ALLOCATOR) -> decltype(s)
        //
        // set(Iter, Iter, COMPARATOR = COMPARATOR(), ALLOCATOR = ALLOCATOR())
        // set(Iter, Iter, ALLOCATOR)
        //
        // set(initializer_list, COMPARATOR = COMPARATOR(),
        //                                             ALLOCATOR = ALLOCATOR())
        // set(initializer_list, ALLOCATOR)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::set<T1> s1;
        bsl::set     s1a(s1);
        ASSERT_SAME_TYPE(decltype(s1a), bsl::set<T1>);

        typedef float T2;
        bsl::set<T2> s2;
        bsl::set     s2a(s2, bsl::allocator<T2>());
        bsl::set     s2b(s2, a1);
        bsl::set     s2c(s2, a2);
        bsl::set     s2d(s2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(s2a), bsl::set<T2>);
        ASSERT_SAME_TYPE(decltype(s2b), bsl::set<T2>);
        ASSERT_SAME_TYPE(decltype(s2c), bsl::set<T2>);
        ASSERT_SAME_TYPE(decltype(s2d), bsl::set<T2>);

        typedef short T3;
        bsl::set<T3> s3;
        bsl::set     s3a(std::move(s3));
        ASSERT_SAME_TYPE(decltype(s3a), bsl::set<T3>);

        typedef long double T4;
        bsl::set<T4> s4;
        bsl::set     s4a(std::move(s4), bsl::allocator<T4>{});
        bsl::set     s4b(std::move(s4), a1);
        bsl::set     s4c(std::move(s4), a2);
        bsl::set     s4d(std::move(s4), bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(s4a), bsl::set<T4>);
        ASSERT_SAME_TYPE(decltype(s4b), bsl::set<T4>);
        ASSERT_SAME_TYPE(decltype(s4c), bsl::set<T4>);
        ASSERT_SAME_TYPE(decltype(s4d), bsl::set<T4>);


        typedef long T5;
        typedef std::greater<T5> CompT5;
        T5                       *p5b = nullptr;
        T5                       *p5e = nullptr;
        bsl::set<T5>::iterator    i5b;
        bsl::set<T5>::iterator    i5e;

        bsl::set s5a(p5b, p5e);
        bsl::set s5b(i5b, i5e);
        bsl::set s5c(p5b, p5e, CompT5{});
        bsl::set s5d(i5b, i5e, CompT5{});
        bsl::set s5e(p5b, p5e, CompT5{}, bsl::allocator<T5>{});
        bsl::set s5f(p5b, p5e, CompT5{}, a1);
        bsl::set s5g(p5b, p5e, CompT5{}, a2);
        bsl::set s5h(p5b, p5e, CompT5{}, std::allocator<T5>{});
        bsl::set s5i(i5b, i5e, CompT5{}, bsl::allocator<T5>{});
        bsl::set s5j(i5b, i5e, CompT5{}, a1);
        bsl::set s5k(i5b, i5e, CompT5{}, a2);
        bsl::set s5l(i5b, i5e, CompT5{}, std::allocator<T5>{});

        ASSERT_SAME_TYPE(decltype(s5a), bsl::set<T5>);
        ASSERT_SAME_TYPE(decltype(s5b), bsl::set<T5>);
        ASSERT_SAME_TYPE(decltype(s5c), bsl::set<T5, CompT5>);
        ASSERT_SAME_TYPE(decltype(s5d), bsl::set<T5, CompT5>);
        ASSERT_SAME_TYPE(decltype(s5e),
                                     bsl::set<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5f),
                                     bsl::set<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5g),
                                     bsl::set<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5h),
                                     bsl::set<T5, CompT5, std::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5i),
                                     bsl::set<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5j),
                                     bsl::set<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5k),
                                     bsl::set<T5, CompT5, bsl::allocator<T5>>);
        ASSERT_SAME_TYPE(decltype(s5l),
                                     bsl::set<T5, CompT5, std::allocator<T5>>);


        typedef short T6;
        T6                       *p6b = nullptr;
        T6                       *p6e = nullptr;
        bsl::set<T6>::iterator    i6b;
        bsl::set<T6>::iterator    i6e;

        bsl::set s6a(p6b, p6e, bsl::allocator<T6>{});
        bsl::set s6b(p6b, p6e, a1);
        bsl::set s6c(p6b, p6e, a2);
        bsl::set s6d(p6b, p6e, std::allocator<T6>{});

        ASSERT_SAME_TYPE(decltype(s6a),
                              bsl::set<T6, std::less<T6>, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(s6b),
                              bsl::set<T6, std::less<T6>, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(s6c),
                              bsl::set<T6, std::less<T6>, bsl::allocator<T6>>);
        ASSERT_SAME_TYPE(decltype(s6d),
                              bsl::set<T6, std::less<T6>, std::allocator<T6>>);


        typedef long T7;
        typedef std::greater<T7> CompT7;
        std::initializer_list<T7> il7({1L, 2L, 3L});

        bsl::set s7a(il7);
        bsl::set s7b(il7, CompT7{});
        bsl::set s7c(il7, CompT7{}, bsl::allocator<T7>{});
        bsl::set s7d(il7, CompT7{}, a1);
        bsl::set s7e(il7, CompT7{}, a2);
        bsl::set s7f(il7, CompT7{}, std::allocator<T7>{});

        ASSERT_SAME_TYPE(decltype(s7a), bsl::set<T7>);
        ASSERT_SAME_TYPE(decltype(s7b), bsl::set<T7, CompT7>);
        ASSERT_SAME_TYPE(decltype(s7c),
                                     bsl::set<T7, CompT7, bsl::allocator<T7>>);
        ASSERT_SAME_TYPE(decltype(s7d),
                                     bsl::set<T7, CompT7, bsl::allocator<T7>>);
        ASSERT_SAME_TYPE(decltype(s7e),
                                     bsl::set<T7, CompT7, bsl::allocator<T7>>);
        ASSERT_SAME_TYPE(decltype(s7f),
                                     bsl::set<T7, CompT7, std::allocator<T7>>);

        typedef long long T8;
        std::initializer_list<T8> il8({3LL, 2LL, 1LL});

        bsl::set s8a(il8, bsl::allocator<T8>{});
        bsl::set s8b(il8, a1);
        bsl::set s8c(il8, a2);
        bsl::set s8d(il8, std::allocator<T8>{});

        ASSERT_SAME_TYPE(decltype(s8a),
                              bsl::set<T8, std::less<T8>, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(s8b),
                              bsl::set<T8, std::less<T8>, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(s8c),
                              bsl::set<T8, std::less<T8>, bsl::allocator<T8>>);
        ASSERT_SAME_TYPE(decltype(s8d),
                              bsl::set<T8, std::less<T8>, std::allocator<T8>>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_SET_COMPILE_FAIL_SWAPPED_COMPARATOR_AND_ALLOCATOR
#ifdef BSLSTL_SET_COMPILE_FAIL_SWAPPED_COMPARATOR_AND_ALLOCATOR
        typedef long T99;
        T99      *p99 = nullptr;
        bsl::set  s99(p99, p99, bsl::allocator<T99>{}, std::less<T99>{});
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

    switch (test) { case 0:
      case 37: {
        // --------------------------------------------------------------------
        // TESTING ERASE_IF
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE FUNCTION 'BSL::ERASE_IF'"
                            "\n=====================================\n");

        TestDriver<char>::testCase37();
        TestDriver<int>::testCase37();
        TestDriver<long>::testCase37();
      } break;
      case 36: {
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
        //: 1 Create a set by invoking the constructor without supplying the
        //:   template arguments explicitly.
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
        (void) test; // This variable only exists for ease of IDE navigation.
#endif
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATORS WITH MULTI-VALUE EQUAL RANGES
        //   Although in most cases additional operators accepting reference
        //   to the 'LOOKUP_KEY' value are in compliance with the main
        //   comparator's operator, accepting two references to the 'KEY'
        //   value, user can provide a transparent comparator, whose operators
        //   change the equality ranges.  We need to check that our container
        //   handles such comparators correctly.
        //
        // Concerns:
        //: 1 The following 'set' methods: 'find, 'count', 'lower_bound',
        //:   'upper_bound' and 'equal_range' correctly handle additional
        //:   operators (those that accept parameters other than set 'KEY') of
        //:   supplied comparator, even if their behavior differs from the
        //:   behavior of the main one.
        //:
        //: 2 'count' can return a value greater than one.
        //:
        //: 3 'equal_range' can return range, containing more than one element
        //
        // Plan:
        //: 1 Construct a set with a transparent comparator, that has
        //:   additional operators, allowing multiple matches for the same
        //:   parameter value.
        //:
        //: 2 Fill the set with unique values and call the lookup functions
        //:   with a parameters that can be both unambiguously and ambiguously
        //:   matched with the set elements.  Verify the results.  (C-1..3)
        //
        // Testing:
        //   CONCERN: 'count'       properly handles multi-value comparators.
        //   CONCERN: 'find'        properly handles multi-value comparators.
        //   CONCERN: 'lower_bound' properly handles multi-value comparators.
        //   CONCERN: 'upper_bound' properly handles multi-value comparators.
        //   CONCERN: 'equal_range' properly handles multi-value comparators.
        // --------------------------------------------------------------------

        if (verbose) printf(
                "\n" "TESTING COMPARATORS WITH MULTI-VALUE EQUAL RANGES" "\n"
                     "=================================================" "\n");

        typedef bsl::set<bsl::string,
                         TransparentComparatorWithMultiValueEqualRange>
                                                             TransparentSet;
        typedef TransparentSet::size_type                    size_type;
        typedef TransparentSet::iterator                     Iterator;
        typedef TransparentSet::const_iterator               ConstIterator;
        typedef bsl::pair<Iterator, Iterator>                ERResultType;
        typedef bsl::pair<ConstIterator, ConstIterator>      ConstERResultType;

        static const struct {
            int         d_lineNum;
            const char *d_keyValue_p;

        } DATA[] = {
        //    LINE  VALUE
        //    ----  ------
            { L_,   ""    },
            { L_,   "b"   },
            { L_,   "d"   },
            { L_,   "dd"  },
            { L_,   "f"   },
            { L_,   "ff"  },
            { L_,   "fff" },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bsl::map<char, int>   symbolEntryNumMap;

        TransparentSet        mX;
        const TransparentSet& X = mX;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int                 LINE  = DATA[i].d_lineNum;
            const bsl::string         VALUE = DATA[i].d_keyValue_p;

            mX.insert(VALUE);
            symbolEntryNumMap[VALUE[0]] += 1;

            for (char c = 'a'; c < 'h'; ++c) {
                // Testing 'find'.
                {
                    ConstIterator constResult = X.find(c);
                    Iterator      result      = mX.find(c);
                    if (0 == symbolEntryNumMap[c]) {
                        ASSERTV(LINE, c, X.end()  == constResult);
                        ASSERTV(LINE, c, mX.end() == result     );
                    }
                    else {
                        ASSERTV(LINE, c, X.end()      != constResult);
                        ASSERTV(LINE, c, mX.end()     != result     );
                        ASSERTV(LINE, c, *constResult == *result    );

                        ASSERTV(LINE, c, (*constResult)[0],
                                c == (*constResult)[0]);
                    }
                }

                // Testing 'lower_bound'.
                {
                    ConstIterator EXPECTED_CONST_LB =  X.begin();
                    Iterator      EXPECTED_LB       = mX.begin();

                    while (EXPECTED_CONST_LB != X.end()) {
                        if (c <= (*EXPECTED_CONST_LB)[0]) {
                            break;
                        }
                        ++EXPECTED_CONST_LB;
                        ++EXPECTED_LB;
                    }

                    ConstIterator constResult = X.lower_bound(c);
                    Iterator      result      = mX.lower_bound(c);

                    ASSERTV(LINE, c, EXPECTED_CONST_LB == constResult);
                    ASSERTV(LINE, c, EXPECTED_LB       == result     );
                }

                // Testing 'upper_bound'.
                {
                    ConstIterator EXPECTED_CONST_UB =  X.begin();
                    Iterator      EXPECTED_UB       = mX.begin();

                    while (EXPECTED_CONST_UB != X.end()) {
                        if (c < (*EXPECTED_CONST_UB)[0]) {
                            break;
                        }
                        ++EXPECTED_CONST_UB;
                        ++EXPECTED_UB;
                    }

                    ConstIterator constResult = X.upper_bound(c);
                    Iterator      result      = mX.upper_bound(c);

                    ASSERTV(LINE, c, EXPECTED_CONST_UB == constResult);
                    ASSERTV(LINE, c, EXPECTED_UB       == result     );
                }

                // Testing 'count'.
                {
                    ConstIterator   EXPECTED_CONST_LB = X.lower_bound(c);
                    const size_type EXPECTED_COUNT =
                     (X.end() == EXPECTED_CONST_LB) ? 0 : symbolEntryNumMap[c];

                    size_type       count       = X.count(c);

                    ASSERTV(c, EXPECTED_COUNT, count ,EXPECTED_COUNT == count);
                }

                // Testing 'equal_range'.
                {
                    ConstIterator   EXPECTED_CONST_LB = X.lower_bound(c);
                    ConstIterator   EXPECTED_CONST_UB = X.upper_bound(c);
                    Iterator        EXPECTED_LB = mX.lower_bound(c);
                    Iterator        EXPECTED_UB = mX.upper_bound(c);
                    const size_type EXPECTED_COUNT = X.count(c);

                    ERResultType      result      = mX.equal_range(c);
                    ConstERResultType constResult = X.equal_range(c);

                    ConstIterator  resultConstLB = constResult.first;
                    ConstIterator  resultConstUB = constResult.second;
                    Iterator       resultLB      = result.first;
                    Iterator       resultUB      = result.second;

                    ASSERTV(LINE, c, EXPECTED_CONST_LB == resultConstLB);
                    ASSERTV(LINE, c, EXPECTED_CONST_UB == resultConstUB);
                    ASSERTV(LINE, c, EXPECTED_LB       == resultLB     );
                    ASSERTV(LINE, c, EXPECTED_UB       == resultUB     );

                    for (size_type counter = 0;
                         counter < EXPECTED_COUNT;
                         ++counter) {
                         ++resultConstLB;
                         ++resultLB;
                    }

                    ASSERTV(LINE, c, EXPECTED_COUNT,
                            resultConstUB == resultConstLB);
                    ASSERTV(LINE, c, EXPECTED_COUNT,
                            resultUB      == resultLB     );
                }
            }
        }
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING TRANSPARENT COMPARATOR
        //
        // Concerns:
        //: 1 'set' has does not have a transparent set of lookup functions if
        //:   the comparator is not transparent.
        //: 2 'set' has a transparent set of lookup functions if the comparator
        //:   is transparent.
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
        //   CONCERN: 'lower_bound' properly handles transparent comparators
        //   CONCERN: 'upper_bound' properly handles transparent comparators
        //   CONCERN: 'equal_range' properly handles transparent comparators
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "TESTING TRANSPARENT COMPARATOR" "\n"
                                 "==============================" "\n");

        typedef bsl::set<int>                        NonTransparentSet;
        typedef bsl::set<int, TransparentComparator> TransparentSet;

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
            const int VALUE = DATA[i];
            if (veryVerbose) {
                printf("Testing transparent comparators with a value of %d\n",
                       VALUE);
            }

            if (veryVerbose) {
                printf("\tTesting const non-transparent set.\n");
            }
            testTransparentComparator( XNT, false, VALUE);

            if (veryVerbose) {
                printf("\tTesting mutable non-transparent set.\n");
            }
            testTransparentComparator(mXNT, false, VALUE);

            if (veryVerbose) {
                printf("\tTesting const transparent set.\n");
            }
            testTransparentComparator( XT,  true,  VALUE);

            if (veryVerbose) {
                printf("\tTesting mutable transparent set.\n");
            }
            testTransparentComparator(mXT,  true,  VALUE);
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
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
        if (verbose) printf("\nTESTING INITIALIZER LIST FUNCTIONS"
                            "\n==================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase32,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING 'emplace' WITH HINT
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'emplace' WITH HINT"
                            "\n===========================\n");

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
        // TESTING 'emplace'
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'emplace'"
                            "\n=================\n");

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
        // TESTING INSERTION OF MOVABLE VALUES WITH HINT
        // --------------------------------------------------------------------
        if (verbose) printf(
                          "\nTESTING INSERTION OF MOVABLE VALUES WITH HINT"
                          "\n=============================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase29,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase29,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING INSERTION OF MOVABLE VALUES
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING INSERTION OF MOVABLE VALUES"
                            "\n===================================\n");

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
        // TESTING MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase27,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase27,
                      bsltf::MoveOnlyAllocTestType,
                      bsltf::WellBehavedMoveOnlyAllocTestType);

        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase27_dispatch,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

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
        if (verbose) printf("\nTESTING STANDARD INTERFACE COVERAGE"
                            "\n===================================\n");

        // Test only 'int' and 'char' parameter types, because set's
        // 'operator<' and related operators only support parameterized types
        // that defines 'operator<'.
        RUN_EACH_TYPE(TestDriver, testCase25, int, char);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTOR OF TEMPLATE WRAPPER
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING CONSTRUCTOR OF TEMPLATE WRAPPER"
                            "\n=======================================\n");

        // KEY doesn't affect the test.  So run test only for 'int'.
        TestDriver<int>::testCase24();
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING TYPE TRAITS"
                            "\n===================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING STL ALLOCATOR"
                            "\n=====================\n");

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
        // TESTING 'max_size' and 'empty'
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'max_size' and 'empty'"
                            "\n==============================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING FREE COMPARISON OPERATORS"
                            "\n=================================\n");

        RUN_EACH_TYPE(TestDriver, testCase19, int, char);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase'
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING 'erase'"
                            "\n===============\n");

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
        if (verbose) printf("\nTESTING 'insert' WITH HINT"
                            "\n==========================\n");

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
        if (verbose) printf("\nTESTING 'insert'"
                            "\n================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase15,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        testRangeInsertOptimization();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING ITERATORS"
                            "\n=================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING SEARCH FUNCTIONS
        // --------------------------------------------------------------------
        if (verbose) printf("\nTESTING SEARCH FUNCTIONS"
                            "\n========================\n");

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

        testRangeCtorOptimization();
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
      case -1: {
        // --------------------------------------------------------------------
        // MEMORY USAGE TEST
        //
        // Concerns:
        //: 1 Determine the memory usage of the set.
        //
        // Plan:
        //: 2 Print the memory after insert.
        // --------------------------------------------------------------------

        bslma::TestAllocator ta;

        bsl::set<int> x(&ta);
        P(sizeof(bsl::set<int>));

        for (int i = 1; i <= 100000; ++i) {
            x.insert(i);
            if (i < 64 || i == 100 || i == 1000 || i == 10000 || i == 100000) {
                P_(i) P(ta.numBytesInUse());
            }
        }
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
// Copyright 2019 Bloomberg Finance L.P.
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
