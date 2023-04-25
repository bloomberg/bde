// bslstl_set.t.cpp                                                   -*-C++-*-
#include <bslstl_set.h>

#include <bslstl_iterator.h>
#include <bslstl_pair.h>

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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <initializer_list>
#endif

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#include <random>
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
// [13] bool contains(const key_type& key);
// [13] bool contains(const LOOKUP_KEY& key);
// [13] iterator find(const key_type& key);
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
// [19] bool operator< (const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [ 6] bool operator!=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [19] bool operator> (const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [19] bool operator>=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [19] bool operator<=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
// [19] auto operator<=>(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
//
//// specialized algorithms:
// [ 8] void swap(set<K, C, A>& a, set<K, C, A>& b);
// [37] erase_if(set<K, C, A>&s, PREDICATE)
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [38] USAGE EXAMPLE
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
//                      TEST CONFIGURATION MACROS
// ----------------------------------------------------------------------------

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

  public:
    // TEST CASES
    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_copy_assignment_dispatch();
    static void testCase9_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8_dispatch();
        // Test member and free 'swap'.

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase7_select_on_container_copy_construction_dispatch();
    static void testCase7_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

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

    static void testCase1(const COMP&  comparator,
                          KEY         *testKeys,
                          size_t       numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
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
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, COMP, ALLOC>::
                    testCase9_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   KEY,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>  StdAlloc;

    typedef bsl::set<KEY, COMP, StdAlloc>        Obj;

    const bool PROPAGATE = PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG;

    static const char *SPECS[] = {
        "",
        "A",
        "BC",
        "CDE",
    };
    const int NUM_SPECS = static_cast<int>(sizeof SPECS / sizeof *SPECS);

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

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == false'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\t'propagate_on_container_copy_assignment::value == true'\n");

    testCase9_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase9_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase9()
{
    // ------------------------------------------------------------------------
    // TESTING COPY-ASSIGNMENT OPERATOR
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

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    {
        // Create first object
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
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
            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                KeyAllocator         xoa(&oa);

                {
                    Obj mX(xoa);  const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa);
                    bslma::TestAllocatorMonitor sam(&scratch);

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

                if (0 != oa.numBlocksInUse()) {
                    ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                            0 == oa.numBlocksInUse());
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
                KeyAllocator         xscratch(&scratch);

                Obj mX(xoa);       const Obj& X  = gg(&mX,  SPEC1);
                Obj mZZ(xscratch); const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor sam(&scratch);

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
void TestDriver<KEY, COMP, ALLOC>::testCase8_dispatch()
{
    // ------------------------------------------------------------------------
    // MANIPULATOR AND FREE FUNCTION 'swap'
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects, whether they use the same
    //   allocator or not.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not affect
    //:   the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped uses the same allocator, neither
    //:   function allocates memory from any allocator.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both functions may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined in
    //:   this component to initialize, respectively, member-function and
    //:   free-function pointers having the appropriate signatures and return
    //:   types.  (C-2)
    //:
    //: 2 Create 4 'bslma::TestAllocator' objects, and create a corresponding
    //:   'ALLOC' object based on each one:
    //:
    //:   o 'doa', installed as the 'bslma' default allocator, and
    //:     'ALLOC da(&doa)'.
    //:
    //:   o 'soa', the scratch allocator, and 'ALLOC sa(&soa)'.
    //:
    //:   o 'xoa', the allocator for 'mX' and 'mY' objects, and
    //:     'ALLOC xa(&xoa)'.
    //:
    //:   o 'zoa', the allocator for 'mZ', and 'ALLOC za(&zoa)'.
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values (one per row) in
    //:     terms of their individual attributes, including (a) first, the
    //:     default value, (b) boundary values corresponding to every range of
    //:     values that each individual attribute can independently attain, and
    //:     (c) values that should require allocation from each individual
    //:     attribute that can independently allocate memory.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1, 3..6)
    //:
    //:   1 Use the default constructor to create two modifiable 'Obj's, 'mXX'
    //:     and 'mX', and use the 'gg' function to give them the value
    //:     described by 'R1', and create 'const' references 'XX' and 'X' to
    //:     each of them, respectively.  Use the 'sa' allocator when creating
    //:     'mXX', and the 'xa' allocator when creating 'mX'.
    //:
    //:   2 Use the member and free 'swap' functions to swap the value of 'mX'
    //:     with itself; verify, after each swap, that: (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   3 For each row 'R2' in the table of P-3: (C-1, 4)
    //:
    //:     1 Use the default constructor to create two modifiable 'Obj's,
    //:       'mYY' and 'mY', and use the 'gg' function to give them the value
    //:       described by 'R2', and create 'const' references 'YY' and 'Y' to
    //:       each of them, respectively.  Use the 'sa' allocator when creating
    //:       'mYY', and the 'xa' allocator when creating 'mY'.
    //:
    //:     2 Use, in turn, the member and free 'swap' functions to swap the
    //:       values of 'mX' and 'mY'; verify, after each swap, that: (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY' is
    //:         unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     3 Use the default constructor and the 'za' allocator to create a
    //:       modifiable 'Obj' 'mZ' and use the 'gg' function to give it the
    //:       value described by 'R2', and create 'const' references 'Z' to
    //:       'mZ', and 'ZZ' to 'YY' respectively.
    //:
    //:     4 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ'; verify, after each swap, that: (C-1, 5)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 If 'propagate_on_container_swap' was 'false', the allocators
    //:         held by 'mX' and 'mZ' is unchanged in both objects.  (C-5)
    //:
    //:       3 If 'propagate_on_container_swap' was 'true', the allocators
    //:         held by 'mX' and 'mZ' is exchanged between the objects.  (C-6)
    //:
    //:       4 Temporary memory were allocated from 'xa' if 'ZZ' is not empty,
    //:         and temporary memory were allocated from 'oaz' if 'ZX' is not
    //:         empty.  (C-5)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-7)
    //:
    //:   1 Create two values distinct from the values corresponding to the
    //:     default-constructed object, choosing values that allocate memory if
    //:     possible.
    //:
    //:   2 Use the default constructor and 'xa' to create a modifiable 'Obj'
    //:     'mX' and use 'gg' to initialize it to a value distinct from the
    //:     default value; similarly create with the 'sa' allocator a separate
    //:     'const' 'Obj' 'XX' distinct from, but equal to, 'mX'.
    //:
    //:   3 Use the default constructor and 'xa' to create a modifiable 'Obj'
    //:     'mY' and use 'gg' to initialize it to a value distinct from the
    //:     default value and unequal to that of 'mX'; similarly create with
    //:     the 'sa' allocator a separate 'const' 'Obj' 'YY' distinct from, but
    //:     equal to, 'mY'.
    //:
    //:   4 Use the 'invokeAdlSwap' helper function template to swap the values
    //:     of 'mX' and 'mY', using the free 'swap' function defined in this
    //:     component, then verify that: (C-7)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  From either
    //:       the default allocator or the allocator used by the containers.
    //:       (C-4)
    //
    // Testing:
    //   void swap(set& other);
    //   void swap(set<K, C, A>& a, set<K, C, A>& b);
    //   propagate_on_container_swap
    // ------------------------------------------------------------------------

    // Since this function is called with a variety of template arguments, it
    // is necessary to infer some things about our template arguments in order
    // to print a meaningful banner.

    const bool isPropagate =
                           AllocatorTraits::propagate_on_container_swap::value;
    const bool otherTraitsSet =
                AllocatorTraits::propagate_on_container_copy_assignment::value;

    // We can print the banner now:

    if (verbose) printf("%sTESTING SWAP '%s' OTHER:%c PROP:%c ALLOC: %s\n",
                        veryVerbose ? "\n" : "",
                        NameOf<KEY>().name(), otherTraitsSet ? 'T' : 'F',
                        isPropagate ? 'T' : 'F',
                        allocCategoryAsStr());

    BSLMF_ASSERT(otherTraitsSet ==
               AllocatorTraits::propagate_on_container_move_assignment::value);

    // If we are using the stateful allocator, no allocator will be passed to
    // 'KEY' objects in the container at construction, so if they allocate
    // memory, it will be from the default allocator.

    const bool keyUsesDefaultAlloc = bslma::UsesBslmaAllocator<KEY>::value &&
                                                 e_STATEFUL == s_allocCategory;

    // Types that have move c'tors.

    enum { NUM_DATA                        = DEFAULT_NUM_DATA };
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator doa("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&doa);
    ALLOC da(&doa);

    bslma::TestAllocator soa("scratch", veryVeryVeryVerbose);
    ALLOC sa(&soa);

    ASSERT((otherTraitsSet ? sa : da) ==
                   AllocatorTraits::select_on_container_copy_construction(sa));

    bslma::TestAllocator xoa("x-original", veryVeryVeryVerbose);
    bslma::TestAllocator zoa("z-original", veryVeryVeryVerbose);

    ALLOC xa(&xoa);
    ALLOC za(&zoa);

    if (veryVerbose) printf(
                       "Assign the address of each function to a variable.\n");
    {
        typedef void (Obj::*MemberFuncPtr)(Obj&);
        typedef void (*FreeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        MemberFuncPtr memberSwap = &Obj::swap;
        FreeFuncPtr   freeSwap   = &bsl::swap;

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (veryVerbose) printf("Aliasing, swapping with matching"
                            " allocators, and\n        swapping with"
                            " non-matching allocators.\n");

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const char *const ISPEC = DATA[ti].d_results;

        if (ti && DATA[ti].d_index == DATA[ti-1].d_index) {
            // redundant, skip

            continue;
        }

        Obj mXX(sa);    const Obj& XX = gg(&mXX, ISPEC);        // control
        Obj mX(xa);     const Obj& X  = gg(&mX,  ISPEC);

        // Verify that the first row of the table contains the
        // default-constructed value.

        static bool firstFlag = true;
        if (firstFlag) {
            firstFlag = false;
            const Obj& D = Obj(sa);
            ASSERTV(ISPEC, D, X, !*ISPEC && D == X);
        }

        // swap with itself (aliasing)

        for (int member = 0; member < 2; ++member) {
            bslma::TestAllocatorMonitor xoam(&xoa);

            ASSERTV(ISPEC, XX, X, XX == X);

            if (member) {
                mX.swap(mX);
            }
            else {
                swap(mX, mX);
            }

            ASSERTV(ISPEC, XX, X, XX == X);
            ASSERTV(ISPEC, xa == X.get_allocator());
            ASSERTV(ISPEC, xoam.isTotalSame());
        }

        for (size_t tj = 0; tj < NUM_DATA; ++tj) {
            const char *const JSPEC = DATA[tj].d_results;

            if (tj && DATA[tj].d_index == DATA[tj-1].d_index) {
                // redundant, skip

                continue;
            }

            if (veryVerbose) { T_ P_(ISPEC) P(JSPEC); }

            Obj mYY(sa);    const Obj& YY = gg(&mYY, JSPEC);    // control
            Obj mY(xa);     const Obj& Y  = gg(&mY,  JSPEC);

            for (int member = 0; member < 2; ++member) {
                ASSERTV(ISPEC, JSPEC, XX, X, XX == X);
                ASSERTV(ISPEC, JSPEC, YY, Y, YY == Y);

                bslma::TestAllocatorMonitor doam(&doa);
                bslma::TestAllocatorMonitor xoam(&xoa);

                if (member) {
                    mX.swap(mY);
                }
                else {
                    swap(mX, mY);
                }

                ASSERTV(ISPEC, JSPEC, doam.isTotalSame());
                ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());

                ASSERTV(ISPEC, JSPEC, YY, X, YY == X);
                ASSERTV(ISPEC, JSPEC, XX, Y, XX == Y);

                ASSERTV(ISPEC, JSPEC, xa == X.get_allocator());
                ASSERTV(ISPEC, JSPEC, xa == Y.get_allocator());

                // swap back

                mX.swap(mY);
            }

            const Obj& ZZ = YY;
            Obj        mZ(za);        const Obj& Z = gg(&mZ, JSPEC);

            ASSERT(Z == Y);

            const bool bothEmpty = XX.empty() && ZZ.empty();

            for (int member = 0; member < 2; ++member) {
                ASSERTV(ISPEC, JSPEC, XX, X, XX == X);
                ASSERTV(ISPEC, JSPEC, ZZ, Z, ZZ == Z);

                ASSERTV(ISPEC, JSPEC, xa == X.get_allocator());
                ASSERTV(ISPEC, JSPEC, za == Z.get_allocator());

                bslma::TestAllocatorMonitor doam(&doa);
                bslma::TestAllocatorMonitor xoam(&xoa);
                bslma::TestAllocatorMonitor zoam(&zoa);

                if (member) {
                    swap(mX, mZ);
                }
                else {
                    mX.swap(mZ);
                }

                if (isPropagate) {
                    ASSERTV(NameOf<KEY>(), doam.isTotalSame(),
                                                           doam.isTotalSame());

                    ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                    ASSERTV(ISPEC, JSPEC, zoam.isTotalSame());
                }
                else {
                    ASSERTV(NameOf<KEY>(), bothEmpty, doam.isTotalSame(),
                            (!keyUsesDefaultAlloc || k_KEY_IS_MOVE_ENABLED ||
                                                                     bothEmpty)
                                                        == doam.isTotalSame());

                    ASSERTV(ISPEC, XX.empty() == zoam.isTotalSame());
                    ASSERTV(JSPEC, ZZ.empty() == xoam.isTotalSame());
                }

                ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
                ASSERTV(ISPEC, JSPEC, XX, Z, XX == Z);

                ASSERTV(ISPEC, JSPEC, isPropagate,
                                 (isPropagate ? za : xa) == X.get_allocator());
                ASSERTV(ISPEC, JSPEC, isPropagate,
                                 (isPropagate ? xa : za) == Z.get_allocator());

                // swap back

                mX.swap(mZ);
            }
        }

        ASSERTV(0 == zoa.numBlocksInUse());
    }

    ASSERTV(0 == doa.numBlocksInUse());
    ASSERTV(0 == xoa.numBlocksInUse());
    ASSERTV(0 == zoa.numBlocksInUse());

    if (veryVerbose) printf(
              "Invoke free 'swap' function in a context where ADL is used.\n");
    {
        // Two different values: Both should cause memory allocation if
        // possible.

        Obj mX(xa) ;    const Obj& X  = gg(&mX,  "DE");
        Obj mXX(sa);    const Obj& XX = gg(&mXX, "DE");

        Obj mY(xa);     const Obj& Y  = gg(&mY,  "ABC");
        Obj mYY(sa);    const Obj& YY = gg(&mYY, "ABC");

        if (veryVerbose) { T_ T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor doam(&doa);
        bslma::TestAllocatorMonitor xoam(&xoa);

        invokeAdlSwap(&mX, &mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(doam.isTotalSame());
        ASSERT(xoam.isTotalSame());

        if (veryVerbose) { T_ T_ P_(X) P(Y) }

        invokePatternSwap(&mX, &mY);

        ASSERTV(YY, X, XX == X);
        ASSERTV(XX, Y, YY == Y);
        ASSERT(doam.isTotalSame());
        ASSERT(xoam.isTotalSame());

        if (veryVerbose) { T_ T_ P_(X) P(Y) }
    }

    ASSERTV(!keyUsesDefaultAlloc == (0 == doa.numBlocksTotal()));
}

template <class KEY, class COMP, class ALLOC>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<KEY, COMP, ALLOC>::
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
                                    OTHER_FLAGS> StdAlloc;

    typedef bsl::set<KEY, COMP, StdAlloc>        Obj;

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
            const Obj W(VALUES.begin(), VALUES.end(), COMP(), ma);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), COMP(), ma);
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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::
                              testCase7_select_on_container_copy_construction()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR ALLOCATOR PROPAGATION
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

    typedef StatefulStlAllocator<KEY>      Allocator;
    typedef bsl::set<KEY, COMP, Allocator> Obj;

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

            Allocator a;
            a.setId(ALLOC_ID);

            const Obj W(VALUES.begin(), VALUES.end(), COMP(), a);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\t\tControl Obj: "); P(W); }

            VALUES.resetIterators();

            Obj mX(VALUES.begin(), VALUES.end(), COMP(), a); const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

            ASSERTV(SPEC,        W == Y);
            ASSERTV(SPEC,        W == X);
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase7()
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
    //   set(const set& original);
    //   set(const set& original, const A& allocator);
    // ------------------------------------------------------------------------

    const TestValues VALUES;

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);
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

        const size_t NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        bslma::TestAllocator oa(veryVeryVerbose);
        KeyAllocator         xoa(&oa);

        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\tFor an object of length " ZU ":\n", LENGTH);
                T_ P(SPEC);
            }

            // Create control object w.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\t\tControl Obj: "); P(W); }

            Obj mX(xoa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(xoa);
                gg(pX, SPEC);

                const Obj Y0(*pX);

                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);
                ASSERTV(SPEC, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                bsl::pair<Iter, bool> RESULT = Y1.insert(VALUES['Z' - 'A']);

                ASSERTV(true == RESULT.second);

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

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, xoa);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                if (LENGTH == 0) {
                    ASSERTV(SPEC, BB + 0 == AA);
                    ASSERTV(SPEC,  B + 0 ==  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC *
                                                    static_cast<int>(X.size());
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS == AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS ==  A);
                }

                const bsls::Types::Int64 CC = oa.numBlocksTotal();
                const bsls::Types::Int64 C  = oa.numBlocksInUse();

                bsl::pair<Iter, bool> RESULT = Y11.insert(VALUES['Z' - 'A']);
                ASSERTV(true == RESULT.second);

                const bsls::Types::Int64 DD = oa.numBlocksTotal();
                const bsls::Types::Int64 D  = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, CC + 1 + TYPE_ALLOC == DD);
                ASSERTV(SPEC, C  + 1 + TYPE_ALLOC ==  D);

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
                ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
#if defined(BDE_BUILD_TARGET_EXC)
            {   // Exception checking.

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    const Obj Y2(X, xoa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
                    ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
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
#endif  // BDE_BUILD_TARGET_EXC
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
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
    //   bool operator==(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    //   bool operator!=(const set<K, C, A>& lhs, const set<K, C, A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting '%s'.\n", NameOf<KEY>().name());

    if (verbose)
              printf("\tAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        using namespace bsl;

        operatorPtr operatorEq = operator==;
        (void) operatorEq;  // quash potential compiler warnings

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
        (void) [](const Obj& lhs, const Obj& rhs) -> bool {
            return lhs != rhs;
        };
#else
        operatorPtr operatorNe = operator!=;
        (void) operatorNe;
#endif
    }

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\tCompare every value with every value.\n");
    {
        // Create first object
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_results);

           if (veryVerbose) { T_ T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                KeyAllocator         xscratch(&scratch);

                Obj mX(xscratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_results);

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

                    KeyAllocator        xxa(&xa);
                    KeyAllocator        xya(&ya);

                    Obj mX(xxa); const Obj& X = gg(&mX, SPEC1);
                    Obj mY(xya); const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    TestComparator<KEY>::disableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    TestComparator<KEY>::enableFunctor();

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase4()
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
    //: 4 The range '[cbegin(), cend())' contains inserted elements the sorted
    //:   order.
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

    static const struct {
        int         d_line;     // source line number
        const char *d_spec;     // specification string
        const char *d_results;  // expected results
    } DATA[] = {
        //line  spec      result
        //----  --------  -------
        { L_,   "",       ""      },
        { L_,   "A",      "A"     },
        { L_,   "AB",     "AB"    },
        { L_,   "ABC",    "ABC"   },
        { L_,   "ABCD",   "ABCD"  },
        { L_,   "ABCDE",  "ABCDE" }
    };
    const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) { printf(
                "\tCreate objects with various allocator configurations.\n"); }
    {
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { T_ T_ P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(0);
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
                  } return;                                           // RETURN
                }

                Obj&                  mX = *objPtr;
                const Obj&            X = gg(&mX, SPEC);
                bslma::TestAllocator& oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                            ? da
                                            : sa1;

                // ------------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());

                size_t i = 0;
                for (CIter iter = X.cbegin(); iter != X.cend(); ++iter, ++i) {
                    ASSERTV(LINE, SPEC, CONFIG, EXP[i] == *iter);
                }

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == i);

                ASSERT(oam.isTotalSame());

                // ------------------------------------------------------------

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

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase3()
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
    //   set<K,A>& gg(set<K,A> *object, const char *spec);
    //   int ggg(set<K,A> *object, const char *spec, int verbose = 1);
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
            { L_,   "AB",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "ABCDE",  "ABCDE" },

        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        size_t oldLen = 0;
        for (size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const size_t      curLen = strlen(SPEC);

            Obj        mX(xoa);
            const Obj& X = gg(&mX, SPEC);  // original spec

            Obj        mY(xoa);
            const Obj& Y = gg(&mY, SPEC);  // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\t\tof length " ZU ":\n", curLen);
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
            ASSERTV(0 == verifyContainer(X, EXP, LENGTH));
            ASSERTV(0 == verifyContainer(Y, EXP, LENGTH));
        }
    }

    if (verbose) printf("\tTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;   // source line number
            const char *d_spec;   // specification string
            int         d_index;  // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,   },  // control

            { L_,   "A",      -1,   },  // control
            { L_,   " ",       0,   },
            { L_,   ".",       0,   },
            { L_,   "E",      -1,   },  // control
            { L_,   "a",       0,   },
            { L_,   "z",       0,   },

            { L_,   "AE",     -1,   },  // control
            { L_,   "aE",      0,   },
            { L_,   "Ae",      1,   },
            { L_,   ".~",      0,   },
            { L_,   "~!",      0,   },
            { L_,   "  ",      0,   },

            { L_,   "ABC",    -1,   },  // control
            { L_,   " BC",     0,   },
            { L_,   "A C",     1,   },
            { L_,   "AB ",     2,   },
            { L_,   "?#:",     0,   },
            { L_,   "   ",     0,   },

            { L_,   "ABCDE",  -1,   },  // control
            { L_,   "aBCDE",   0,   },
            { L_,   "ABcDE",   2,   },
            { L_,   "ABCDe",   4,   },
            { L_,   "AbCdE",   1,   }
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        size_t oldLen = 0;
        for (size_t ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(xoa);

            if (LENGTH != oldLen) {
                if (verbose) printf("\t\tof length " ZU ":\n", LENGTH);
                ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase2()
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
    //   set(const C& comparator, const A& allocator);
    //   ~set();
    //   bsl::pair<iterator, bool> insert(value_type&& value);
    //   void clear();
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value ||
                           bsl::uses_allocator<KEY, ALLOC>::value;

    if (verbose) printf("\nTesting '%s' (TYPE_ALLOC = %d).\n",
                        NameOf<KEY>().name(),
                        TYPE_ALLOC);

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
            printf("\tTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
            KeyAllocator         xsa(&sa);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  if (verbose) {
                      printf("\t\tTesting default constructor.\n");
                  }
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  if (verbose) {
                      printf("\t\tTesting constructor with null allocator.\n");
                  }
                  objPtr = new (fa) Obj(ALLOC(0));
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  if (verbose) {
                      printf("\t\tTesting constructor with an allocator.\n");
                  }
                  objPtr = new (fa) Obj(xsa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              } return;                                               // RETURN
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

            ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\t\tTesting 'insert' (bootstrap).\n"); }

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            KeyAllocator         xscratch(&scratch);
            if (0 < LENGTH) {
                if (veryVerbose) {
                    printf("\t\tOn an object of initial length " ZU ".\n",
                           LENGTH);
                }

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj]);

                    bsl::pair<Iter, bool> RESULT =
                                         primaryManipulator(&mX, id, xscratch);
                    ASSERTV(LENGTH, tj, CONFIG, true       == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                }

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                // Verify behavior when element already exist in the object

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj]);

                    bsl::pair<Iter, bool> RESULT =
                                         primaryManipulator(&mX, id, xscratch);
                    ASSERTV(LENGTH, tj, CONFIG, false      == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\t\tTesting 'clear'.\n");
            {
                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.clear();

                ASSERTV(LENGTH, CONFIG, 0 == X.size());
                ASSERTV(LENGTH, CONFIG, X.cbegin() == X.cend());

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64 A  = oa.numBlocksInUse();

                ASSERTV(LENGTH, CONFIG, BB == AA);
                ASSERTV(LENGTH, CONFIG, B, A,
                        B - (int)LENGTH * TYPE_ALLOC == A);

                for (size_t tj = 0; tj < LENGTH; ++tj) {
                    int id = TstFacility::getIdentifier(VALUES[tj]);

                    bsl::pair<Iter, bool> RESULT =
                                         primaryManipulator(&mX, id, xscratch);
                    ASSERTV(LENGTH, tj, CONFIG, true       == RESULT.second);
                    ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == *(RESULT.first));
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
    }
}

template <class KEY, class COMP, class ALLOC>
void TestDriver<KEY, COMP, ALLOC>::testCase1(const COMP&  comparator,
                                             KEY         *testKeys,
                                             size_t       numValues)
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //: 1 Execute each methods to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    typedef bsl::set<KEY, COMP>                  Obj;
    typedef typename Obj::iterator               iterator;
    typedef typename Obj::const_iterator         const_iterator;
    typedef typename Obj::reverse_iterator       reverse_iterator;
    typedef typename Obj::const_reverse_iterator const_reverse_iterator;

    typedef typename Obj::value_type             Value;
    typedef bsl::pair<iterator, bool>            InsertResult;

    bslma::TestAllocator         defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator");

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("\tDefault construct an empty set.\n");
    }
    {
        Obj x(&objectAllocator); const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(true == X.empty());
        ASSERTV(0    <  X.max_size());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(0    == objectAllocator.numBytesInUse());
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("\tTest use of allocators.\n");
    }
    {
        bslma::TestAllocator objectAllocator1("objectAllocator1");
        bslma::TestAllocator objectAllocator2("objectAllocator2");

        Obj o1(comparator, &objectAllocator1); const Obj& O1 = o1;
        ASSERTV(&objectAllocator1 == O1.get_allocator().mechanism());

        for (size_t i = 0; i < numValues; ++i) {
            o1.insert(Value(testKeys[i]));
        }
        ASSERTV(numValues == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());

        bslma::TestAllocatorMonitor monitor1(&objectAllocator1);

        Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
        ASSERTV(&objectAllocator2 == O2.get_allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 < objectAllocator1.numBytesInUse());
        ASSERTV(0 < objectAllocator2.numBytesInUse());

        Obj o3(comparator, &objectAllocator1); const Obj& O3 = o3;
        ASSERTV(&objectAllocator1 == O3.get_allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0         == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 < objectAllocator1.numBytesInUse());
        ASSERTV(0 < objectAllocator2.numBytesInUse());

        o1.swap(o3);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

        o3.swap(o2);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
        ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down depending
                                         // on implementation
        ASSERTV(monitor1.isTotalUp());
        ASSERTV(0 < objectAllocator1.numBytesInUse());
        ASSERTV(0 < objectAllocator2.numBytesInUse());

        ASSERTV(&objectAllocator1 == O1.get_allocator().mechanism());
        ASSERTV(&objectAllocator2 == O2.get_allocator().mechanism());
        ASSERTV(&objectAllocator1 == O3.get_allocator().mechanism());
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf(
              "\tTest primary manipulators/accessors on every permutation.\n");
    }

    std::sort(testKeys, testKeys + numValues, comparator);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x(comparator, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X, &objectAllocator); const Obj& Y = y;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            ASSERTV(i, X.end() == X.find(testKeys[i]));

            // Test 'insert'.
            Value value(testKeys[i]);
            InsertResult RESULT = x.insert(value);
            ASSERTV(X.end()       != RESULT.first);
            ASSERTV(true          == RESULT.second);
            ASSERTV(testKeys[i]   == *(RESULT.first));


            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(false == X.empty());

            // Test insert duplicate key.
            ASSERTV(RESULT.first == x.insert(value).first);
            ASSERTV(false        == x.insert(value).second);
            ASSERTV(i + 1        == X.size());

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());
        // Verify sorted order of elements.

        {
            const_iterator last = X.begin();
            const_iterator it   = ++(X.begin());
            while (it != X.end()) {
                ASSERTV(comparator(*last, *it));
                ASSERTV(comparator(*last, *it));

                last = it;
                ++it;
            }
        }

        // Test iterators.
        {
            const_iterator cbi  = X.begin();
            const_iterator ccbi = X.cbegin();
            iterator       bi   = x.begin();

            const_iterator last = X.begin();
            while (cbi != X.end()) {
                ASSERTV(cbi == ccbi);
                ASSERTV(cbi == bi);

                if (cbi != X.begin()) {
                    ASSERTV(comparator(*last, *cbi));
                }
                last = cbi;
                ++bi; ++ccbi; ++cbi;
            }

            ASSERTV(cbi  == X.end());
            ASSERTV(ccbi == X.end());
            ASSERTV(bi   == X.end());
            --bi; --ccbi; --cbi;

            reverse_iterator       ri   = x.rbegin();
            const_reverse_iterator rci  = X.rbegin();
            const_reverse_iterator rcci = X.crbegin();

            while  (rci != X.rend()) {
                ASSERTV(cbi == ccbi);
                ASSERTV(cbi == bi);
                ASSERTV(rci == rcci);
                ASSERTV(*ri == *rcci);

                if (rci !=  X.rbegin()) {
                    ASSERTV(comparator(*cbi, *last));
                    ASSERTV(comparator(*rci, *last));
                }

                last = cbi;
                if (cbi != X.begin()) {
                    --bi; --ccbi; --cbi;
                }
                ++ri; ++rcci; ++rci;
            }
            ASSERTV(cbi  == X.begin());
            ASSERTV(ccbi == X.begin());
            ASSERTV(bi   == X.begin());

            ASSERTV(rci  == X.rend());
            ASSERTV(rcci == X.rend());
            ASSERTV(ri   == x.rend());
        }

        // Use erase(iterator) on all the elements.
        for (size_t i = 0; i < numValues; ++i) {
            const_iterator it     = x.find(testKeys[i]);
            const_iterator nextIt = it;
            ++nextIt;

            ASSERTV(X.end()     != it);
            ASSERTV(testKeys[i] == *it);

            const_iterator resIt = x.erase(it);

            ASSERTV(resIt             == nextIt);
            ASSERTV(numValues - i - 1 == X.size());

            if (resIt != X.end()) {
                ASSERTV(comparator(testKeys[i], *resIt));
            }
        }
    } while (std::next_permutation(testKeys,
                                   testKeys + numValues,
                                   comparator));

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    std::sort(testKeys, testKeys + numValues, comparator);
    if (veryVerbose) {
        printf("\tTest 'lower_bound' and 'upper_bound'.\n");
    }
    {
        Obj x(comparator, &objectAllocator); const Obj& X = x;

        // Insert every other value into the set.
        for (size_t i = 0; i < numValues; ++i) {
            if (i % 2) {
                Value value(testKeys[i]);
                x.insert(value);
            }
        }

        for (size_t i = 0; i < numValues; ++i) {
            iterator       li = x.lower_bound(testKeys[i]);
            const_iterator LI = X.lower_bound(testKeys[i]);
            iterator       ui = x.upper_bound(testKeys[i]);
            const_iterator UI = X.upper_bound(testKeys[i]);

            ASSERTV(li == LI);
            ASSERTV(ui == UI);

            // If test value 'i' was inserted in the map then 'lower_bound'
            // will return an iterator to that value; otherwise, 'lower_bound'
            // will return an iterator to the subsequent inserted value if one
            // exists, and the end iterator otherwise.
            const_iterator EXP_LOWER = i % 2
                                       ? X.find(testKeys[i])
                                       : i + 1 < numValues
                                             ? X.find(testKeys[i+1])
                                             : X.end();

            // If test value 'i' was inserted in the map, then 'upper_bound'
            // should return an iterator to the subsequent value as
            // 'lower_bound', and the same iterator otherwise.
            const_iterator EXP_UPPER = EXP_LOWER;
            if (i % 2) {
                ++EXP_UPPER;
            }

            ASSERTV(EXP_LOWER == li);
            ASSERTV(EXP_LOWER == LI);
            ASSERTV(EXP_UPPER == ui);
            ASSERTV(EXP_UPPER == UI);
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
    std::shuffle(testKeys,
                 testKeys + numValues,
                 std::default_random_engine());
#else  // fall-back for C++03, potentially unsupported in C++17
    std::random_shuffle(testKeys,  testKeys + numValues);
#endif
    if (veryVerbose) {
        printf("\tTest 'erase(const key_type&)'.\n");
    }
    {
        Obj x(comparator, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            const Value  VALUE(testKeys[i]);
            InsertResult RESULT = x.insert(VALUE);
            ASSERTV(VALUE == *RESULT.first);
        }

        for (size_t i = 0; i < numValues; ++i) {
            ASSERTV(1 == x.erase(testKeys[i]));
            ASSERTV(0 == x.erase(testKeys[i]));
            ASSERTV(numValues - i - 1 == X.size());
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("\tTest 'erase(const_iterator, const_iterator )'.\n");
    }
    {
        for (size_t i = 0; i < numValues; ++i) {
            for (size_t j = 0; j < numValues; ++j) {
                Obj x(comparator, &objectAllocator); const Obj& X = x;
                for (size_t k = 0; k < numValues; ++k) {
                    const Value  VALUE(testKeys[k]);
                    InsertResult RESULT = x.insert(VALUE);
                    ASSERTV(VALUE == *RESULT.first);
                }

                const_iterator a = X.find(testKeys[i]);
                const_iterator b = X.find(testKeys[j]);

                if (!comparator(testKeys[i], testKeys[j])) {
                    std::swap(a, b);
                }
                KEY min = *a;
                KEY max = *b;
                ASSERTV(!comparator(max, min)); // min <= max

                typename Obj::difference_type numElements
                                                         = bsl::distance(a, b);

                iterator endPoint = x.erase(a, b);

                ASSERTV(numValues - numElements == X.size());
                ASSERTV(endPoint                == b);

                for (size_t k = 0; k < numValues; ++k) {
                    if (comparator(testKeys[k], min) ||
                        !comparator(testKeys[k], max)) {
                        ASSERTV(testKeys[k] == *X.find(testKeys[k]));
                    }
                    else {
                        ASSERTV(X.end() == X.find(testKeys[k]));
                    }
                }
            }
        }

        // Test 'erase(const_iterator, const_iterator )' for end of range.
        for (size_t i = 0; i < numValues; ++i) {
            Obj x(comparator, &objectAllocator); const Obj& X = x;
            for (size_t k = 0; k < numValues - 1; ++k) {
                // Insert 1 fewer than the total number of keys.

                const Value  VALUE(testKeys[k]);
                InsertResult RESULT = x.insert(VALUE);
                ASSERTV(VALUE == *RESULT.first);
            }

            const_iterator a = X.find(testKeys[i]);
            const_iterator b = X.end();
            typename Obj::difference_type numElements = bsl::distance(a, b);
            iterator endPoint = x.erase(a, b);

            ASSERTV(numValues - numElements - 1 == X.size());
            ASSERTV(endPoint                    == b);
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("\tTest insert & set for iterator ranges.\n");
    }
    {

        typedef KEY NonConstValue;
        NonConstValue *myValues = new NonConstValue[numValues];
        for (size_t i = 0; i < numValues; ++i) {
            myValues[i]  = testKeys[i];
        }

        for (size_t i = 0; i < numValues; ++i) {
            for (size_t length = 0; length <= numValues - i; ++length) {
                Obj x(comparator, &objectAllocator); const Obj& X = x;
                for (size_t k = 0; k < length; ++k) {
                    size_t       index = i + k;
                    InsertResult RESULT = x.insert(myValues[index]);
                    ASSERTV(myValues[index] == *RESULT.first);
                }
                Obj y(comparator, &objectAllocator); const Obj& Y = y;
                y.insert(myValues + i, myValues + (i + length));

                Obj z(myValues + i,
                      myValues + (i + length),
                      comparator,
                      &objectAllocator);
                const Obj& Z = z;
                ASSERTV(X == Y);
                ASSERTV(X == Z);
            }
        }
        delete [] myValues;
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("\tTest 'equal_range'\n");
    }
    {
        Obj x(comparator, &objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            const Value  VALUE(testKeys[i]);
            InsertResult RESULT = x.insert(VALUE);
            ASSERTV(VALUE == *RESULT.first);
        }

        for (size_t i = 0; i < numValues; ++i) {
            pair<iterator, iterator>             RESULT =
                                                    x.equal_range(testKeys[i]);
            pair<const_iterator, const_iterator> cresult =
                                                    X.equal_range(testKeys[i]);

            ASSERTV(cresult.first  == RESULT.first);
            ASSERTV(cresult.second == RESULT.second);

            ASSERTV(*RESULT.first == testKeys[i]);
            ASSERTV(X.end() == RESULT.second ||
                   *RESULT.second != testKeys[i]);
        }
        for (size_t i = 0; i < numValues; ++i) {
            x.erase(testKeys[i]);
            pair<iterator, iterator>             RESULT =
                                                    x.equal_range(testKeys[i]);
            pair<const_iterator, const_iterator> cresult =
                                                    x.equal_range(testKeys[i]);

            iterator       li = x.lower_bound(testKeys[i]);
            const_iterator LI = X.lower_bound(testKeys[i]);
            iterator       ui = x.upper_bound(testKeys[i]);
            const_iterator UI = X.upper_bound(testKeys[i]);

            ASSERTV(RESULT.first   == li);
            ASSERTV(RESULT.second  == ui);
            ASSERTV(cresult.first  == LI);
            ASSERTV(cresult.second == UI);
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf(
             "\tTest 'operator<', 'operator>', 'operator<=', 'operator>='.\n");
    }
    {
        // Iterate over possible selections of elements to add to two
        // containers, 'X' and 'Y' then compare the results of the comparison
        // operators to an "oracle" result from
        // 'bslalg::RangeCompare::lexicographical' over the same range.

        for (size_t i = 0; i < numValues; ++i) {
            for (size_t j = 0; j < numValues; ++j) {
                for (size_t length = 0; length < numValues; ++length) {
                    Obj x(comparator, &objectAllocator); const Obj& X = x;
                    Obj y(comparator, &objectAllocator); const Obj& Y = y;
                    for (size_t k = 0; k < j; ++k) {
                        size_t xIndex = (i + length) % numValues;
                        size_t yIndex = (j + length) % numValues;

                        Value xValue(testKeys[xIndex]);
                        x.insert(xValue);
                        Value yValue(testKeys[yIndex]);
                        y.insert(yValue);
                    }

                    int comp = bslalg::RangeCompare::lexicographical(X.begin(),
                                                                     X.end(),
                                                                     Y.begin(),
                                                                     Y.end());
                    ASSERTV((comp < 0)  == (X < Y));
                    ASSERTV((comp > 0)  == (X > Y));
                    ASSERTV((comp <= 0) == (X <= Y));
                    ASSERTV((comp >= 0) == (X >= Y));
                }
            }
        }
    }

    //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    std::sort(testKeys, testKeys + numValues, comparator);
    if (veryVerbose) {
        printf("\tTest 'key_comp' and 'value_comp'.\n");
    }
    {
        Obj x(comparator, &objectAllocator); const Obj& X = x;
        typename Obj::key_compare   keyComp   = X.key_comp();
        typename Obj::value_compare valueComp = X.value_comp();
        for (size_t i = 0; i < numValues - 1; ++i) {
            ASSERTV(keyComp(testKeys[i], testKeys[i+1]));
            ASSERTV(valueComp(Value(testKeys[i]),
                             Value(testKeys[i+1])));
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

    static void testCase8();
        // Test member and free 'swap'.
};

template <class KEY, class COMP>
void MetaTestDriver<KEY, COMP>::testCase8()
{
    // The low-order bit of the identifier specifies whether the third boolean
    // argument of the stateful allocator, which indicates propagate on
    // container swap, is set.

    typedef bsltf::StdStatefulAllocator<KEY, false, false, false, false> A00;
    typedef bsltf::StdStatefulAllocator<KEY, false, false, true,  false> A01;
    typedef bsltf::StdStatefulAllocator<KEY, true,  true,  false, true>  A10;
    typedef bsltf::StdStatefulAllocator<KEY, true,  true,  true,  true>  A11;

    if (verbose) printf("\n");

    TestDriver<KEY, COMP, BAP>::testCase8_dispatch();

    TestDriver<KEY, COMP, SAA>::testCase8_dispatch();

    TestDriver<KEY, COMP, A00>::testCase8_dispatch();
    TestDriver<KEY, COMP, A01>::testCase8_dispatch();
    TestDriver<KEY, COMP, A10>::testCase8_dispatch();
    TestDriver<KEY, COMP, A11>::testCase8_dispatch();
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Holiday Calendar
/// - - - - - - - - - - - - - - - - - - -
// In this example, we will utilize 'bsl::set' to define and implement a class,
// 'HolidayCalendar', that provides a calendar that allows client to add and
// remove holiday dates and determine whether a particular date is a holiday.
//
// First, we define and implement the methods of a value-semantic type,
// 'MyDate', that represents a date: (Note that for brevity, we do not
// explicitly document the invariants of a valid date.)
//..
class MyDate {
    // This class implements a value-semantic attribute class characterizing a
    // date according to the (Gregorian) Unix date convention.

    // DATA
    int d_year;
    int d_month;
    int d_day;

  public:
    // CREATORS
    MyDate(int year, int month, int day)
        // Create a 'MyDate' object having the value represented by the
        // specified 'year', 'month', and 'day.  The behavior is undefined
        // unless the value represented by 'year', 'month', and 'day is valid.
    : d_year(year), d_month(month), d_day(day)
    {
    }

    MyDate(const MyDate& original)
        // Create a 'MyDate' object having the same value as the specified
        // 'original' object.
    : d_year(original.d_year), d_month(original.d_month), d_day(original.d_day)
    {
    }

    //! ~MyDate() = default;
        // Destroy this object

    // MANIPULATORS
    MyDate& operator=(const MyDate& rhs)
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.
    {
        d_year = rhs.d_year;
        d_month = rhs.d_month;
        d_day = rhs.d_day;
        return *this;
    }

    // ACCESSORS
    int year() const
        // Return the year of this date.
    {
        return d_year;
    }

    int month() const
        // Return the month of this date.
    {
        return d_month;
    }

    int day() const
        // Return the day of this date.
    {
        return d_day;
    }

};

// FREE FUNCTIONS
inline
bool operator==(const MyDate& lhs, const MyDate& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyDate' objects have the same value
    // if each of their corresponding 'year', 'month', and 'day' attributes
    // respective have the same value.
{
    return lhs.year()  == rhs.year() &&
           lhs.month() == rhs.month() &&
           lhs.day()   == rhs.day();
}

inline
bool operator!=(const MyDate& lhs, const MyDate& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MyDate' objects do not have the
    // same value if each of their corresponding 'year', 'month', and 'day'
    // attributes respective do not have the same value.
{
    return !(lhs == rhs);
}
//..
// Then, we define a comparison functor for 'MyDate' objects in order for them
// to be stored in a 'set' object:
//..
struct MyDateLess {
    // This 'struct' defines an ordering on 'MyDate' objects, allowing them to
    // be included in associative containers such as 'bsl::set'.

    bool operator() (const MyDate& lhs, const MyDate& rhs) const
        // Return 'true' if the value of the specified 'lhs' is less than
        // (ordered before) the value of the specified 'rhs', and 'false'
        // otherwise.  The 'lhs' value is considered less than the 'rhs' value
        // if the date represented by 'lhs' is earlier than the date
        // represented by 'rhs' in time.
    {
        if (lhs.year() < rhs.year()) return true;                     // RETURN
        if (lhs.year() == rhs.year()) {
            if (lhs.month() < rhs.month()) return true;               // RETURN
            if (lhs.month() == rhs.month()) {
                if (lhs.day() < rhs.day()) return true;               // RETURN
            }
        }
        return false;
    }
};
//..
// Next, we define 'HolidayCalendar':
//..
class HolidayCalendar {
    // This class provides a value-semantic type that allows clients to modify
    // and query a set of dates considered to be holidays.
//..
// Here, we create a type alias, 'DateSet', for a 'bsl::set' that will serve as
// the data member for a 'HolidayCalendar'.  A 'DateSet' has keys of type
// 'MyDate', and a comparator of type 'MyDateLess'.  We use the default
// 'ALLOCATOR' template parameter as we intend to use 'HolidayCalendar' with
// 'bslma' style allocators:
//..
    // PRIVATE TYPES
    typedef bsl::set<MyDate, MyDateLess> DateSet;
        // This 'typedef' is an alias for a set of 'MyDate' objects.

    // DATA
    DateSet d_holidayDates;  // set of dates considered to be holidays

    // FRIENDS
    friend bool operator==(const HolidayCalendar&, const HolidayCalendar&);

  public:
    // PUBLIC TYPES
    typedef DateSet::const_iterator ConstIterator;
        // This 'typedef' provides an alias for the type of an iterator
        // providing non-modifiable access to holiday dates in a
        // 'HolidayCalendar'.

    // CREATORS
    explicit
    HolidayCalendar(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'HolidayCalendar' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    HolidayCalendar(const HolidayCalendar&  original,
                    bslma::Allocator       *basicAllocator = 0);
        // Create a 'HolidayCalendar' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    //! ~HolidayCalendar() = default;
        // Destroy this object.

    // MANIPULATORS
    void addHolidayDate(const MyDate& date);
        // Add the specified 'date' as a holiday date maintained by this
        // calendar.  If 'date' is already a holiday date, this method has no
        // effect.

    void removeHolidayDate(const MyDate& date);
        // Remove the specify 'date' from the set of holiday dates maintained
        // by this calendar.  If 'date' is not a holiday date, this method has
        // no effect.

    // ACCESSORS
    bool isHolidayDate(const MyDate& date) const;
        // Return 'true' if the specified 'date' is in the set of holiday dates
        // maintained by this calendar, and return 'false' otherwise.

    ConstIterator beginHolidayDates() const;
        // Return an iterator providing non-modifiable access to the first date
        // in the ordered sequence of holiday dates maintained by this
        // calendar.

    ConstIterator endHolidayDates() const;
        // Return an iterator providing non-modifiable access to past-the-end
        // date in the ordered sequence of holiday dates maintained by this
        // calendar.
};
//..
// Then, we declare the free operators for 'HolidayCalendar':
//..
inline
bool operator==(const HolidayCalendar& lhs, const HolidayCalendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'HolidayCalendar' objects have the
    // same value if they have the same number of holiday dates, and each
    // corresponding holiday date, in their respective ordered sequence of
    // dates, is the same.

inline
bool operator!=(const HolidayCalendar& lhs, const HolidayCalendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'HolidayCalendar' objects do not
    // have the same value if they either differ in their number of holiday
    // dates, or if any of the corresponding holiday dates, in their respective
    // ordered sequences of dates, is not the same.
//..
// Now, we define the implementations methods of the 'HolidayCalendar' class:
//..
// CREATORS
HolidayCalendar::HolidayCalendar(bslma::Allocator *basicAllocator)
: d_holidayDates(basicAllocator)
{
}
//..
// Notice that, on construction, we pass the 'bsl::set' object the specified
// 'bsl::Allocator' object.
//..
// MANIPULATORS
void HolidayCalendar::addHolidayDate(const MyDate& date)
{
    d_holidayDates.insert(date);
}

void HolidayCalendar::removeHolidayDate(const MyDate& date)
{
    d_holidayDates.erase(date);
}

// ACCESSORS
bool HolidayCalendar::isHolidayDate(const MyDate& date) const
{
    return d_holidayDates.find(date) != d_holidayDates.end();
}

HolidayCalendar::ConstIterator HolidayCalendar::beginHolidayDates() const
{
    return d_holidayDates.begin();
}

HolidayCalendar::ConstIterator HolidayCalendar::endHolidayDates() const
{
    return d_holidayDates.end();
}
//..
// Finally, we implement the free operators for 'HolidayCalendar':
//..
inline
bool operator==(const HolidayCalendar& lhs, const HolidayCalendar& rhs)
{
    return lhs.d_holidayDates == rhs.d_holidayDates;
}

inline
bool operator!=(const HolidayCalendar& lhs, const HolidayCalendar& rhs)
{
    return !(lhs == rhs);
}
//..

}  // close namespace UsageExample

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

bool intLessThan(int a, int b)
{
    return a < b;
}

class MyUserType0 {
    int *d_value_p;

  public:
    explicit
    MyUserType0(int value)
    : d_value_p(new int(value))
    {
    }

    MyUserType0(int value1, int value2)
    : d_value_p(new int(value1 * value2))
    {
    }

    MyUserType0(const MyUserType0& original)
    : d_value_p(0)
    {
        if (original.d_value_p) {
            d_value_p = new int(*original.d_value_p);
        }
    }

    MyUserType0(bslmf::MovableRef<MyUserType0> value)               // IMPLICIT
    : d_value_p(0)
    {
        printf("my user type move ctor called\n");
        MyUserType0& lvalue = value;
        d_value_p = lvalue.d_value_p;
        lvalue.d_value_p = 0;
    }

    ~MyUserType0()
    {
        if (d_value_p) {
            delete d_value_p;
        }
    }

    // MANIPULATORS
    MyUserType0& operator=(const MyUserType0& rhs)
    {
        if (this != &rhs) {
            if (rhs.d_value_p) {
                d_value_p = new int(*rhs.d_value_p);
            }
            else {
                delete d_value_p;
                d_value_p = 0;
            }
        }
        return *this;
    }

    MyUserType0& operator=(bslmf::MovableRef<MyUserType0> rhs)
    {
        MyUserType0& lvalue = rhs;
        if (this != &lvalue) {
            d_value_p = lvalue.d_value_p;
            lvalue.d_value_p = 0;
        }
        return *this;
    }

    int& value()
    {
        return *d_value_p;
    }

    operator int() const
    {
        return d_value_p ? *d_value_p : -999999999;
    }

    // ACCESSORS
    const int& value() const
    {
        return *d_value_p;
    }

    const int *pointer() const
    {
        return d_value_p;
    }
};

bool operator==(const MyUserType0& lhs, const MyUserType0& rhs)
{
    return lhs.value() == rhs.value();
}

bool operator!=(const MyUserType0& lhs, const MyUserType0& rhs)
{
    return lhs.value() != rhs.value();
}

bool operator<(const MyUserType0& lhs, const MyUserType0& rhs)
{
    return lhs.value() < rhs.value();
}

void pintset(const set<int>& s)
{
    if (0 == s.size())
    {
        printf("(* Empty *)");
    }
    else {
        printf("[");
        for (set<int>::const_iterator it = s.begin(); it != s.end(); ++it)
        {
            printf(" %d", *it);
        }
        printf("]");
    }
}

class MyStatefulComparator
{
    int  d_count;
    bool d_less;

    MyStatefulComparator& operator=(const MyStatefulComparator&);

    MyStatefulComparator(bslmf::MovableRef<MyStatefulComparator> original);
                                                                    // IMPLICIT

    MyStatefulComparator&
                        operator=(bslmf::MovableRef<MyStatefulComparator> rhs);
  public:
    explicit
    MyStatefulComparator(bool lessthan) : d_count(0), d_less(lessthan) { }

    MyStatefulComparator(const MyStatefulComparator& original)
    : d_count(original.d_count)
    , d_less(original.d_less)
    {
    }

    void swap(MyStatefulComparator& other)
    {
        std::swap(d_count, other.d_count);
        std::swap(d_less, other.d_less);
    }

    bool operator()(int lhs, int rhs)
    {
        ++d_count;
        return d_less ? lhs < rhs : rhs < lhs;
    }

    int count() const
    {
        return d_count;
    }
};

void swap(MyStatefulComparator& a, MyStatefulComparator& b)
{
    a.swap(b);
}

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

        {
            using namespace UsageExample;

            bslma::TestAllocator         defaultAllocator("defaultAllocator");
            bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

            bslma::TestAllocator objectAllocator("objectAllocator");

            HolidayCalendar cal(&objectAllocator);

            ASSERT(false == cal.isHolidayDate(MyDate(2012, 7, 4)));

            cal.addHolidayDate(MyDate(2012, 7, 4));

            ASSERT(true == cal.isHolidayDate(MyDate(2012, 7, 4)));

            cal.addHolidayDate(MyDate(2012, 1, 1));

            ASSERT(true == cal.isHolidayDate(MyDate(2012, 1, 1)));

            cal.removeHolidayDate(MyDate(2012, 1, 1));

            ASSERT(false == cal.isHolidayDate(MyDate(2012, 1, 1)));

            cal.addHolidayDate(MyDate(2012, 12, 25));

            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 < objectAllocator.numBytesInUse());
        }

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
                    "\nTEST CASE %d IS DELEGATED TO 'bslstl_set_test.t.cpp'"
                    "\n====================================================\n",
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
        // TESTING COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY-ASSIGNMENT OPERATOR"
                            "\n================================\n");

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
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(MetaTestDriver,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType);
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
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase7,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);

        // 'select_on_container_copy_construction' testing

        RUN_EACH_TYPE(TestDriver,
                      testCase7_select_on_container_copy_construction,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
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
                      bsltf::MoveOnlyAllocTestType);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTIONS 'gg' and 'ggg'"
                            "\n==========================================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
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

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::MoveOnlyAllocTestType);
        RUN_EACH_TYPE(StdBslmaTestDriver,
                      testCase2,
                      bsltf::StdAllocTestType<bsl::allocator<int> >,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // check integrity of 'DEFAULT_DATA'.

        {
            const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                    ASSERT((DATA[ti].d_index == DATA[tj].d_index) ==
                              !strcmp(DATA[ti].d_results, DATA[tj].d_results));
                }
            }
        }

        {
            int    INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
            size_t NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

            typedef bool (*ComparatorFunction)(int, int);
            TestDriver<int, ComparatorFunction>::testCase1(&intLessThan,
                                                           INT_VALUES,
                                                           NUM_INT_VALUES);
            TestDriver<int, std::less<int> >::testCase1(std::less<int>(),
                                                        INT_VALUES,
                                                        NUM_INT_VALUES);
        }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
        {
            ASSERT((0 == []() -> bsl::set<int> { return {}; }().size()));
            ASSERT((1 == []() -> bsl::set<int> { return {1}; }().size()));
            ASSERT((2 == []() -> bsl::set<int> { return {3,1,3}; }().size()));
        }
#endif
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
