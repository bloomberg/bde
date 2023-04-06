// bslstl_vector.0.t.cpp                                              -*-C++-*-
#include <bslstl_vector.h>

// This test driver contains common test machinery for all 'bslstl_vector' test
// drivers.  If '#include'd after defining the 'BSLSTL_VECTOR_0T_AS_INCLUDE'
// macro it will just provide those common pieces (and no 'main' function).

#include <bsla_fallthrough.h>
#include <bsla_unused.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_destructorproctor.h>
#include <bslma_rawdeleterproctor.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_exceptionutil.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_allocemplacabletesttype.h>
#include <bsltf_emplacabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_nontypicaloverloadstesttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>

#include <iterator>   // 'iterator_traits'
#include <new>        // ::operator new
#include <stdexcept>  // 'length_error', 'out_of_range'
#include <utility>    // 'move'

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Don't put any 'using' declaration or directive here, see swap helpers below.

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 4 parts, 'bslstl_vector.0.t.cpp' (common code), 'bslstl_vector.1.t.cpp'
// (cases 1-11, and the usage example), 'bslstl_vector.2.t.cpp' (cases 12-23),
// and 'bslstl_vector.3.t.cpp' (cases 24 and higher).
//
//                              Overview
//                              --------
// The component under test defines a container whose interface and contract is
// dictated by the C++ standard.  In particular, the standard mandates "strong"
// exception safety (with full guarantee of rollback) along with throwing
// 'std::length_error' if about to request memory for more than 'max_size()'
// elements.  (Note: 'max_size' depends on the parameterized 'VALUE_TYPE'.) The
// general concerns are compliance, exception safety, and proper dispatching
// (for member function templates such as assign and insert).  In addition, it
// is a value-semantic type whose salient attributes are size and value of each
// element in sequence.  This container is implemented in the form of a class
// template, and thus its proper instantiation for several types is a concern.
// Regarding the allocator template argument, we use mostly a 'bsl::allocator'
// together with a 'bslma::TestAllocator' mechanism, but we also verify the C++
// standard.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' method (taking an rvalue reference to template parameter type
// 'VALUE_TYPE') and the 'clear' method to be used by the generator function
// 'gg'.  Additional helper functions are provided to facilitate perturbation
// of internal state (e.g., capacity).  Note that some manipulators must
// support aliasing, and those that perform memory allocation must be tested
// for exception neutrality via the 'bslma_testallocator' component.  After the
// mandatory sequence of cases (1--10) for value-semantic types (cases 5 and 10
// are not implemented, as there is not output or streaming below bslstl), we
// test each individual constructor, manipulator, and accessor in subsequent
// cases.  Note that due to the test driver split up test case 12 is also the
// case number for the usage example test in test driver part 1.
//
// TBD Test coverage of support for move-only types is incomplete, as the
// 'bsltf' test infrastructure is largely built around making copies, and
// providing immutable access to reference objects.  Replacing copy operations
// with potentially-modifying move operations requires support in several
// lower level components, as well as a careful redesign of test code to
// validate moves *as* *well* *as* copies.  This will also require use of
// 'std::move_itearator' to support range-based operations in c++99 and later.
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     T            VALUE_TYPE (template argument, no default)
//     A            ALLOCATOR (template argument, default: bsl::allocator<T>)
//     vector<T,A>  bsl::vector<VALUE_TYPE, ALLOCATOR>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//     ImpUtil      bsl::Vector_Util
//-----------------------------------------------------------------------------
// class vector<T,A> (vector)
// ============================================================================
// ~~ bslstl_vector.1.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CREATORS:
// [ 2] vector<T,A>();
// [ 2] vector<T,A>(const A& a);
// [ 7] vector<T,A>(const vector<T,A>& original);
// [ 7] vector<T,A>(const vector<T,A>& original, const A& alloc);
// [ 2] ~vector<T,A>();
//
/// MANIPULATORS:
// [ 9] operator=(vector<T,A>&);
// [ 2] void clear();
// [ 8] void swap(vector<T,A>&);
//
// ACCESSORS:
// [ 4] allocator_type get_allocator() const;
// [ 4] const_reference operator[](size_type pos) const;
// [ 4] const_reference at(size_type pos) const;
// [ 4] size_type size() const;
//
// FREE OPERATORS:
// [ 6] bool operator==(const vector<T,A>&, const vector<T,A>&);
// [ 6] bool operator!=(const vector<T,A>&, const vector<T,A>&);
// [ 8] void swap(vector<T,A>& lhs, vector<T,A>& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] ALLOCATOR-RELATED CONCERNS
// [12] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(vector<T,A> *object, const char *spec, int vF = 1);
// [ 3] vector<T,A>& gg(vector<T,A> *object, const char *spec);
//
// INTERACTIVE AND SPECIAL TESTS
// [-1] PERFORMANCE TEST
//
// ~~ bslstl_vector.2.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CREATORS:
// [12] vector<T,A>(size_type n, const A& alloc = A());
// [12] vector<T,A>(size_type n, const T& value, const A& alloc = A());
// [12] vector<T,A>(InputIter first, InputIter last, const A& alloc = A());
// [23] vector<T,A>(vector&& original);
// [23] vector<T,A>(vector&& original, const A& alloc);
//
/// MANIPULATORS:
// [13] template <class InputIter> void assign(InputIter f, InputIter l);
// [13] void assign(size_type numElements, const T& val);
// [15] reference operator[](size_type pos);
// [15] reference at(size_type pos);
// [16] iterator begin();
// [16] iterator end();
// [16] reverse_iterator rbegin();
// [16] reverse_iterator rend();
// [14] void resize(size_type n);
// [14] void resize(size_type n, const T& val);
// [14] void reserve(size_type n);
// [14] void shrink_to_fit();
// [15] reference front();
// [15] reference back();
// [15] VALUE_TYPE *data();
// [17] void push_back(const T&);
// [19] void pop_back();
// [17] iterator insert(const_iterator pos, const T& val);
// [17] iterator insert(const_iterator pos, size_type n, const T& val);
// [17] iterator insert(const_iterator pos, InputIter first, InputIter last);
// [19] iterator erase(const_iterator pos);
// [19] iterator erase(const_iterator first, const_iterator last);
//
// ACCESSORS:
// [15] const_reference front() const;
// [15] const_reference back() const;
// [15] const VALUE_TYPE *data() const;
// [14] size_type max_size() const;
// [14] size_type capacity() const;
// [14] bool empty() const;
// [16] const_iterator begin() const;
// [16] const_iterator end() const;
// [16] const_reverse_iterator rbegin() const;
// [16] const_reverse_iterator rend() const;
//
// FREE OPERATORS:
// [20] bool operator<(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator>(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator<=(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator>=(const vector<T,A>&, const vector<T,A>&);
// [20] auto operator<=>(const vector<T,A>&, const vector<T,A>&);
//-----------------------------------------------------------------------------
// [21] CONCERN: 'std::length_error' is used properly
// [22] CONCERN: Vector support types with overloaded new/delete
//
// ~~ bslstl_vector.3.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CREATORS:
// [29] vector<T,A>(initializer_list<T>, const A& alloc = A());
//
/// MANIPULATORS:
// [29] void assign(initializer_list<T>);
// [24] operator=(vector<T,A>&&);
// [29] operator=(initializer_list<T>);
// [27] reference emplace_back(Args...);
// [25] void push_back(T&&);
// [28] iterator emplace(const_iterator pos, Args...);
// [26] iterator insert(const_iterator pos, T&& val);
// [29] iterator insert(const_iterator pos, initializer_list<T>);
//
// FREE OPERATORS:
// [34] void hashAppend(HASHALG& hashAlg, const vector<T,A>&);
// [40] size_t erase(vector<T,A>&, const U&);
// [40] size_t erase_if(vector<T,A>&, PREDICATE);
//-----------------------------------------------------------------------------
// [30] DRQS 31711031
// [31] DRQS 34693876
// [32] CONCERN: Range operations slice from ranges of derived types
// [33] CONCERN: Range ops work correctly for types convertible to 'iterator'
// [35] CONCERN: Methods qualified 'noexcept' in standard are so implemented
// [36] CONCERN: 'vector<bool>' is also verified
// [37] CONCERN: Access through member pointers compiles
// [38] CONCERN: Movable types are moved when growing a vector
// [39] CLASS TEMPLATE DEDUCTION GUIDES
// [41] INCOMPLETE TYPE SUPPORT
// [42] CONCERN: Default constructor is called for default-inserted elems

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

#ifndef BSLSTL_LIST_0T_AS_INCLUDE
void aSsErT(bool condition, const char *message, int line) BSLA_UNUSED;
#endif

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                  TEST DRIVER SPECIFIC MACROS
// ----------------------------------------------------------------------------

#define ITER_CONTAINER_RUN_EACH_TYPE(C, M, T1, T2, T3, T4)                    \
        C<T1>::M(ListLike<T1>()); C<T1>::M(ArrayLike<T1>());                  \
        C<T2>::M(ListLike<T2>()); C<T2>::M(ArrayLike<T2>());                  \
        C<T3>::M(ListLike<T3>()); C<T3>::M(ArrayLike<T3>());                  \
        C<T4>::M(ListLike<T4>()); C<T4>::M(ArrayLike<T4>());                  \
        C<T1>::M(InputSequence<T1>());                                        \
        C<T2>::M(InputSequence<T2>());                                        \
        C<T3>::M(InputSequence<T3>());                                        \
        C<T4>::M(InputSequence<T4>());

// ============================================================================
//                  TARGET SPECIFIC CONFIGURATION
// ----------------------------------------------------------------------------

enum {
// The following enum is set to 1 when exceptions are enabled and to 0
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

// These swap test helpers are not shared between ttest drivers, but because
// they require "moving" the using directive and declarations after their
// definition, and they are unchanged between test driver, we best define them
// in the common part (so nobody will put the 'using' to the top of this common
// file).

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
using bsl::vector;
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
using bsl::Vector_IsRandomAccessIterator;
#endif
using bsl::Vector_Util;
using bsls::NameOf;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

typedef Vector_Util         ImpUtil;

typedef bsltf::TemplateTestFacility TstFacility;
typedef bsltf::MoveState            MoveState;

typedef bslma::ConstructionUtil ConstrUtil;
typedef bslmf::MovableRefUtil   MoveUtil;

// ============================================================================
//                             DEFAULT TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used in multiple test cases.

static const size_t DEFAULT_MAX_LENGTH = 17;

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line spec                 results
    //---- --------             -------
    { L_,  "" },
    { L_,  "A" },
    { L_,  "AA" },
    { L_,  "ABCABC" },
    { L_,  "AABBCC" },
    { L_,  "ABCA" },
    { L_,  "AB" },
    { L_,  "BA" },
    { L_,  "ABCB" },
    { L_,  "ABC" },
    { L_,  "ACB" },
    { L_,  "BAC" },
    { L_,  "BCA" },
    { L_,  "CAB" },
    { L_,  "CBA" },
    { L_,  "ABCC" },
    { L_,  "ABCD" },
    { L_,  "ACBD" },
    { L_,  "BDCA" },
    { L_,  "DCBA" },
    { L_,  "ABCDE" },
    { L_,  "ACBDE" },
    { L_,  "CEBDA" },
    { L_,  "EDCBA" },
    { L_,  "FEDCBA" },
    { L_,  "ABCDEFG" },
    { L_,  "ABCDEFGH" },
    { L_,  "ABCDEFGHI" },
    { L_,  "ABCDEFGHIJKLMNOP" },
    { L_,  "PONMLKJIGHFEDCBA" },
    { L_,  "ABCDEFGHIJKLMNOPQ" },
    { L_,  "DHBIMACOPELGFKNJQ" },
    { L_,  "BAD" },
    { L_,  "BEAD" },
    { L_,  "AC" },
    { L_,  "B" },
    { L_,  "BCDE" },
    { L_,  "FEDCB" },
    { L_,  "CD" }
};
static const size_t DEFAULT_NUM_DATA =
                                    sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

const int NUM_ALLOCS[] = {
    // Number of allocations (blocks) to create a vector of the following size
    // by using 'push_back' repeatedly (without initial reserve):
    //
    // 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,  1,  2,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6,

    // 18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,

    // 36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,

    // 54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71
    // --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8
};
static const size_t NUM_NUM_ALLOCS = sizeof NUM_ALLOCS / sizeof *NUM_ALLOCS;

static const size_t ZERO = 0;   // Add this to a signed expression to easily
                                // make it unsigned.

const int LARGE_SIZE_VALUE = 10;
    // Declare a large value for insertions into the vector.  Note this value
    // will cause multiple resizes during insertion into the vector.

//=============================================================================
//                       GLOBAL OBJECTS SHARED BY TEST CASES
//-----------------------------------------------------------------------------

// STATIC DATA
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

                  // 'AllocTestType' less-than operator

namespace BloombergLP {
namespace bsltf {

bool operator<(const AllocTestType& lhs, const AllocTestType& rhs)
{
    return lhs.data() < rhs.data();
}

}  // close namespace bsltf
}  // close enterprise namespace

namespace bsl {

                   // vector-specific print function

template <class TYPE, class ALLOC>
inline
void debugprint(const bsl::vector<TYPE, ALLOC>& v)
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        putchar('"');
        const size_t sz = v.size();
        for (size_t ii = 0; ii < sz; ++ii) {
            const char c = static_cast<char>(
                            bsltf::TemplateTestFacility::getIdentifier(v[ii]));

            putchar(c ? c : '@');
        }
        putchar('"');
    }
    fflush(stdout);
}

}  // close namespace bsl

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

template <class CONTAINER>
size_t numMovedInto(const CONTAINER& X,
                    size_t           startIndex = 0,
                    size_t           endIndex = 0)
{
    int    numMoved = 0;
    size_t upTo     = endIndex > startIndex ? endIndex : X.size();
    for (size_t i = startIndex; i < upTo; ++i) {
        MoveState::Enum mState = TstFacility::getMovedIntoState(X[i]);
        if (MoveState::e_MOVED == mState || MoveState::e_UNKNOWN == mState) {
            ++numMoved;
        }
    }
    return numMoved;
}

template <class CONTAINER>
size_t numNotMovedInto(const CONTAINER& X,
                       size_t           startIndex = 0,
                       size_t           endIndex = 0)
{
    int    numNotMoved = 0;
    size_t upTo        = endIndex > startIndex ? endIndex : X.size();
    for (size_t i = startIndex; i < upTo; ++i) {
        MoveState::Enum mState = TstFacility::getMovedIntoState(X[i]);
        if (MoveState::e_NOT_MOVED == mState ||
            MoveState::e_UNKNOWN == mState) {
            ++numNotMoved;
        }
    }
    return numNotMoved;
}

bool expectToAllocate(size_t n)
    // Return 'true' if the container is expected to allocate memory on the
    // specified 'n'th element, and 'false' otherwise.
{
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

template <class TYPE, class ALLOC>
void primaryManipulator(bsl::vector<TYPE, ALLOC> *container,
                        int                       identifier)
    // Insert into the specified 'container' the value object indicated by the
    // specified 'identifier', ensuring that the overload of the primary
    // manipulator taking a modifiable rvalue is invoked (rather than the one
    // taking an lvalue).  Return the result of invoking the primary
    // manipulator.  Note that this function acts as the primary manipulator
    // for testing 'vector'.
{
    typedef bslmf::MovableRefUtil       MoveUtil;
    typedef bsltf::TemplateTestFacility TstFacility;

    bsls::ObjectBuffer<TYPE> buffer;
    TstFacility::emplace(buffer.address(),
                         identifier,
                         container->get_allocator());
    bslma::DestructorGuard<TYPE> guard(buffer.address());

    container->emplace_back(MoveUtil::move(buffer.object()));
}

template <class TYPE, class ALLOC>
void resetMovedInto(bsl::vector<TYPE, ALLOC> *object)
{
    typedef bsltf::TemplateTestFacility TstFacility;

    for (size_t i = 0; i < object->size(); ++i) {
        TstFacility::setMovedIntoState(object->data() + i,
                                       bsltf::MoveState::e_NOT_MOVED);
    }
}

//=============================================================================
//                      TEST APPARATUS: GENERATOR FUNCTIONS
//-----------------------------------------------------------------------------

// The generating functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. E] correspond to arbitrary (but unique) char values to be
// appended to the 'bslstl::Vector<T>' object.  A tilde ('~') indicates that
// the logical (but not necessarily physical) state of the object is to be set
// to its initial, empty state (via the 'clear' method).
//..
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
// <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Y'
//                                      // unique but otherwise arbitrary
// <CLEAR>      ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unchanged.
// "A"          Append the value corresponding to A.
// "AA"         Append two values both corresponding to A.
// "ABC"        Append three values corresponding to A, B, and C.
// "ABC~"       Append three values corresponding to A, B, and C and then
//              remove all the elements (set array length to 0).  Note that
//              this spec yields an object that is logically equivalent (but
//              not necessarily identical internally) to one yielded by ("").
// "ABC~DE"     Append three values corresponding to A, B, and C; empty the
//              object; and append values corresponding to D and E.
//-----------------------------------------------------------------------------
//..

template <class TYPE, class ALLOC>
int ggg(bsl::vector<TYPE, ALLOC> *object,
        const char               *spec,
        bool                      verboseFlag = true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'push_back' and white-box
    // manipulator 'clear'.  Optionally specify 'false' for 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Y') {
            primaryManipulator(object, spec[i]);
        }
        else if ('~' == spec[i]) {
            object->clear();
        }
        else {
            if (verboseFlag) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;  // Discontinue processing this spec.           // RETURN
        }
   }
   resetMovedInto(object);
   return SUCCESS;
}

template <class TYPE, class ALLOC>
bsl::vector<TYPE, ALLOC>& gg(bsl::vector<TYPE, ALLOC> *object,
                             const char               *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

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
    // in exception-throwing code.  On construction, this class stores the a
    // copy of an object of the parameterized type 'OBJECT' and the address of
    // that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int                         d_line;      // the line number to report
    bsls::ObjectBuffer<OBJECT>  d_control;   // copy of object being tested
    const OBJECT               *d_object_p;  // address of original object

  public:
    // CREATORS
    ExceptionProctor(const OBJECT     *object,
                     int               line,
                     const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number.  Optionally specify 'basicAllocator' to
        // supply memory.
    : d_line(line)
    , d_object_p(object)
    {
        if (d_object_p) {
            new (d_control.buffer()) OBJECT(*object, basicAllocator);
        }
    }

    ExceptionProctor(const OBJECT     *object,
                     const OBJECT&     control,
                     int               line,
                     const ALLOCATOR&  basicAllocator = ALLOCATOR())
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
        // Optionally specify 'basicAllocator' to supply memory.
    : d_line(line)
    , d_object_p(object)
    {
        if (d_object_p) {
            new (d_control.buffer()) OBJECT(control, basicAllocator);
        }
    }

    ExceptionProctor(const OBJECT              *object,
                     bslmf::MovableRef<OBJECT>  control,
                     int                        line)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
    : d_line(line)
    , d_object_p(object)
    {
        if (d_object_p) {
            new (d_control.buffer()) OBJECT(
                                         bslmf::MovableRefUtil::move(control));
        }
    }

    ~ExceptionProctor()
        // Destroy the exception proctor.  If the proctor was not released,
        // verify that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_control.object(), *d_object_p,
                    d_control.object() == *d_object_p);
            d_control.object().~OBJECT();
        }
    }

    // MANIPULATORS
    void release()
        // Release the proctor from verifying the state of the object.
    {
        if (d_object_p) {
            d_control.object().~OBJECT();
        }
        d_object_p = 0;
    }
};

namespace BloombergLP {
namespace bslma {

template <class OBJECT, class ALLOCATOR>
struct UsesBslmaAllocator<ExceptionProctor<OBJECT, ALLOCATOR> >
    : bsl::false_type {
};

}  // close namespace bslma
}  // close enterprise namespace

                               // ==============
                               // class ListLike
                               // ==============

template <class TYPE>
class ListLike {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the parameterized
    // 'TYPE'.

    // DATA
    bsl::vector<TYPE> d_value;

  public:
    // TYPES
    typedef bslstl::ForwardIterator<const TYPE, const TYPE*>  const_iterator;
        // Input iterator.

    // CREATORS
    ListLike();

    explicit
    ListLike(const bsl::vector<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;

    const_iterator begin() const;

    const_iterator end() const;
};

// CREATORS
template <class TYPE>
ListLike<TYPE>::ListLike()
{
}

template <class TYPE>
ListLike<TYPE>::ListLike(const bsl::vector<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE&
ListLike<TYPE>::operator[](size_t index) const
{
    return d_value[index];
}

template <class TYPE>
typename ListLike<TYPE>::const_iterator
ListLike<TYPE>::begin() const
{
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename ListLike<TYPE>::const_iterator
ListLike<TYPE>::end() const
{
    return const_iterator(d_value.end());
}

                              // ===============
                              // class ArrayLike
                              // ===============

template <class TYPE>
class ArrayLike {
    // This array class is a simple wrapper on a vector offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically a *random-access* iterator and its value type is the
    // parameterized 'TYPE'.

    // DATA
    bsl::vector<TYPE>  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    ArrayLike();

    explicit
    ArrayLike(const bsl::vector<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;

    const_iterator begin() const;

    const_iterator end() const;
};

// CREATORS
template <class TYPE>
ArrayLike<TYPE>::ArrayLike()
{
}

template <class TYPE>
ArrayLike<TYPE>::ArrayLike(const bsl::vector<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE& ArrayLike<TYPE>::operator[](size_t index) const
{
    return d_value[index];
}

template <class TYPE>
typename ArrayLike<TYPE>::const_iterator ArrayLike<TYPE>::begin() const
{
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename ArrayLike<TYPE>::const_iterator ArrayLike<TYPE>::end() const
{
    return const_iterator(d_value.end());
}

                               // ================
                               // class ArrowProxy
                               // ================

template <class TARGET>
class ArrowProxy {
    const TARGET *d_ptr;

  public:
    explicit ArrowProxy(const TARGET *ptr) : d_ptr(ptr) {}

    const TARGET *operator->() const { return d_ptr; }
};

                               // ================
                               // class InputProxy
                               // ================

template <class TARGET>
class InputProxy {
    const TARGET *d_ptr;

  public:
    explicit InputProxy(const TARGET *ptr) : d_ptr(ptr) {}

    operator const TARGET &() const
    {
        BSLS_ASSERT_OPT(d_ptr);

        return *d_ptr;
    }
};

                               // ===================
                               // class InputIterator
                               // ===================

template <class TARGET>
class InputIterator {
    const TARGET *d_ptr;

  public:
    typedef int                     difference_type;
    typedef TARGET                  value_type;
    typedef ArrowProxy<TARGET>      pointer;
    typedef InputProxy<TARGET>      reference;
    typedef std::input_iterator_tag iterator_category;

    explicit InputIterator(const TARGET *ptr)
    : d_ptr(ptr)
    {
    }

    reference operator*() const
    {
        BSLS_ASSERT_OPT(d_ptr);

        return reference(d_ptr);
    }

    pointer operator->() const
    {
        BSLS_ASSERT_OPT(d_ptr);

        return pointer(d_ptr);
    }

    InputIterator& operator++()
    {
        BSLS_ASSERT_OPT(d_ptr);

        ++d_ptr; return *this;
    }

    InputIterator  operator++(int)
    {
        BSLS_ASSERT_OPT(d_ptr);

        InputIterator result(*this);
        ++d_ptr;
        return result;
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON

    bool operator==(const InputIterator&) const = default;

#else
    friend bool operator==(const InputIterator& lhs, const InputIterator& rhs)
    {
        return lhs.d_ptr == rhs.d_ptr;
    }

    friend bool operator!=(const InputIterator& lhs, const InputIterator& rhs)
    {
        return lhs.d_ptr != rhs.d_ptr;
    }
#endif
};

                               // ===================
                               // class InputSequence
                               // ===================

template <class TYPE>
class InputSequence {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the parameterized
    // 'TYPE'.

    // DATA
    bsl::vector<TYPE> d_value;

  public:
    // TYPES
    typedef InputIterator<TYPE>  const_iterator;
        // Input iterator.

    // CREATORS
    InputSequence();

    explicit
    InputSequence(const bsl::vector<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;

    const_iterator begin() const;

    const_iterator end() const;
};

// CREATORS
template <class TYPE>
InputSequence<TYPE>::InputSequence()
{
}

template <class TYPE>
InputSequence<TYPE>::InputSequence(const bsl::vector<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE&
InputSequence<TYPE>::operator[](size_t index) const
{
    return d_value[index];
}

template <class TYPE>
typename InputSequence<TYPE>::const_iterator
InputSequence<TYPE>::begin() const
{
    return const_iterator(d_value.data());
}

template <class TYPE>
typename InputSequence<TYPE>::const_iterator
InputSequence<TYPE>::end() const
{
    return const_iterator(d_value.data() + d_value.size());
}

                              // ====================
                              // class LimitAllocator
                              // ====================

template <class ALLOC>
class LimitAllocator : public ALLOC {
  private:
    // PRIVATE TYPES
    typedef ALLOC                        AllocBase;
    typedef bsl::allocator_traits<ALLOC> TraitsBase;

  public:
    // TYPES
    typedef typename TraitsBase::size_type         size_type;

    template <class BDE_OTHER_TYPE> struct rebind {
        // It is better not to inherit the 'rebind' template, or else
        // 'rebind<X>::other' would be 'ALLOC::rebind<BDE_OTHER_TYPE>::other'
        // instead of 'LimitAlloc<ALLOC::rebind<BDE_OTHER_TYPE>::otherX>'.

        typedef typename TraitsBase::template rebind_traits<BDE_OTHER_TYPE>
                                                              RebindTraitsBase;

        typedef LimitAllocator<typename RebindTraitsBase::allocator_type>
                                                              other;
    };

  private:
    // DATA
    size_type d_limit;

  public:
    // CREATORS
    LimitAllocator()
    : d_limit(-1)
    {
    }

    explicit
    LimitAllocator(bslma::Allocator *mechanism)
    : AllocBase(mechanism)
    , d_limit(-1)
    {
    }

    explicit
    LimitAllocator(const ALLOC& original)
    : AllocBase((const AllocBase&) original)
    , d_limit(-1)
    {
    }

    template <class REBOUND_ALLOC>
    LimitAllocator(const LimitAllocator<REBOUND_ALLOC>& original)
    : AllocBase((const AllocBase&) original)
    , d_limit(original.max_size())
    {
    }

    ~LimitAllocator()
    {
    }

    // MANIPULATORS
    void setMaxSize(size_type maxSize)
    {
        d_limit = maxSize;
    }

    // ACCESSORS
    size_type max_size() const
    {
        return d_limit;
    }
};

namespace BloombergLP {
namespace bslmf {

template <class ALLOCATOR>
struct IsBitwiseMoveable<LimitAllocator<ALLOCATOR> >
    : IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf
}  // close enterprise namespace

template <class ITER, class VALUE_TYPE>
class TestMovableTypeUtil
{
  public:
    static ITER findFirstNotMovedInto(ITER begin, ITER end)
    {
        for (; begin != end; ++begin) {
            MoveState::Enum mState = TstFacility::getMovedIntoState(*begin);
            if (MoveState::e_NOT_MOVED == mState) {
                break;
            }
        }
        return begin;
    }
};

//=============================================================================
//                              AWKWARD TEST TYPES
//=============================================================================

                            // ===================
                            // class NotAssignable
                            // ===================

class NotAssignable {
    // Several 'vector' methods have "instertable" requirements without also
    // demanding "assignable" requirements.  This type is designed to support
    // testing to ensure that no accidental dependencies on the "assignable"
    // requirements leak into those methods.  Note that, by providing a value
    // constructor, this class is not default constructible either.

    int d_data;

  private:
    // NOT IMPLEMENTED
    NotAssignable& operator=(const NotAssignable&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    NotAssignable(int value)                                       // IMPLICIT
        : d_data(value) {}

    // NotAssignable(const NotAssignable& original); // = default;
        // Create a copy of the specified 'original'.

    // ACCESSORS
    operator int() const { return d_data; }
    int value() const { return d_data; }
};

// ASPECTS

void debugprint(const NotAssignable& v)
{
    printf("%d", v.value());
}

                       // ==========================
                       // class BitwiseNotAssignable
                       // ==========================

class BitwiseNotAssignable {
    // Several 'vector' methods have "instertable" requirements without also
    // demanding "assignable" requirements.  This type is designed to support
    // testing to ensure that no accidental dependencies on the "assignable"
    // requirements leak into those methods, specifically handling the code
    // paths that detect the bitwise-movable trait.  Note that, by providing a
    // value constructor, this class is not default constructible either.

    int d_data;

  private:
    // NOT IMPLEMENTED
    BitwiseNotAssignable& operator=(const BitwiseNotAssignable&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    BitwiseNotAssignable(int value)                                // IMPLICIT
        : d_data(value) {}

    // BitwiseNotAssignable(const BitwiseNotAssignable& original) = default;
        // Create a copy of the specified 'original'.

    // ~BitwiseNotAssignable() = default;
        // Destroy this object.

    // ACCESSORS
    operator int() const { return d_data; }
    int value() const { return d_data; }
};

// ASPECTS

void debugprint(const BitwiseNotAssignable& v)
{
    printf("%d", v.value());
}

                             // ==================
                             // class MoveOnlyType
                             // ==================

#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 201103L
// This class doesn't make sence for C++03 tests.  Even though we can emulate
// move-operations, we cannot make them noexcept as 'vector' requires.
class MoveOnlyType {
    // Non-copyable but movable type.

    // DATA
    void *ptr;
  public:
    // CREATORS
    MoveOnlyType() : ptr(0)
        // Create a 'MoveOnlyType' object.
    {
    }
    MoveOnlyType(MoveOnlyType &&) = default;
    MoveOnlyType(const MoveOnlyType &) = delete;

    // MANIPULATORS
    MoveOnlyType &operator=(MoveOnlyType &&) = default;
    MoveOnlyType &operator=(const MoveOnlyType &) = delete;
};
#endif

                    // =================================
                    // template class StatelessAllocator
                    // =================================

template <class TYPE,
          bool  PROPAGATE_ON_CONTAINER_SWAP            = false,
          bool  PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT = false>
struct StatelessAllocator {
    // Stateless std allocator with 'is_always_equal == true_type'

    // TYPES
    typedef TYPE      value_type;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;

    typedef value_type       *pointer;
    typedef const value_type *const_pointer;

    template <class OTHER_TYPE>
    struct rebind {
        typedef StatelessAllocator<OTHER_TYPE,
                                   PROPAGATE_ON_CONTAINER_SWAP,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
            other;
    };

    typedef bsl::true_type is_always_equal;
    typedef bsl::integral_constant<bool, PROPAGATE_ON_CONTAINER_SWAP>
        propagate_on_container_swap;
    typedef bsl::integral_constant<bool,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>
        propagate_on_container_move_assignment;

    // CREATORS
    StatelessAllocator()
        // Create a 'StatelessAllocator' object.
    {
    }
    template <class OTHER_TYPE>
    StatelessAllocator(
             const StatelessAllocator<OTHER_TYPE,
                                      PROPAGATE_ON_CONTAINER_SWAP,
                                      PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT>&)
        // Create a 'StatelessAllocator' object.
    {
    }

    // MANIPULATORS
    pointer allocate(size_type count)
        // Return a pointer to an uninitialized memory that is enough to store
        // an array of the specified 'count' objects.
    {
        return static_cast<pointer>(::operator new(count *
                                                   sizeof(value_type)));
    }
    void deallocate(pointer address, size_type)
        // Return the memory at the specified 'address' to this allocator.
    {
        ::operator delete(static_cast<void *>(address));
    }

    // FREE OPERATORS
    friend bool operator==(StatelessAllocator, StatelessAllocator)
    {
        return true;
    }
#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    friend bool operator!=(StatelessAllocator, StatelessAllocator)
    {
        return false;
    }
#endif
};

// TBD: duplicate these types as allocator-aware
// TBD: duplicate the allocator-aware types for std allocators

namespace BloombergLP {
namespace bslmf {

template <>
struct IsBitwiseMoveable<BitwiseNotAssignable> : bsl::true_type {};

}  // close namespace bslmf
}  // close enterprise namespace

//=============================================================================
//                       TEST DRIVER TEMPLATE
//=============================================================================

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestSupport {
    // TYPES
    typedef bsl::vector<TYPE, ALLOC>             Obj;

    typedef typename Obj::iterator               iterator;
    typedef typename Obj::const_iterator         const_iterator;
    typedef typename Obj::reverse_iterator       reverse_iterator;
    typedef typename Obj::const_reverse_iterator const_reverse_iterator;
    typedef typename Obj::value_type             ValueType;

    typedef typename Obj::const_iterator         CIter;

    typedef TestMovableTypeUtil<CIter, TYPE>     TstMoveUtil;
    typedef bsltf::TestValuesArray<TYPE>         TestValues;

    typedef bsl::allocator_traits<ALLOC>         AllocatorTraits;

    enum AllocCategory { e_BSLMA, e_STDALLOC, e_ADAPTOR, e_STATEFUL };

    // TEST APPARATUS

    // CLASS DATA
    static
    const AllocCategory s_allocCategory =
                        bsl::is_same<ALLOC, bsl::allocator<TYPE> >::value
                        ? e_BSLMA
                        : bsl::is_same<ALLOC,
                                       bsltf::StdAllocTestType<
                                                bsl::allocator<TYPE> > >::value
                        ? e_STDALLOC
                        : bsl::is_same<ALLOC,
                                       bsltf::StdAllocatorAdaptor<
                                                bsl::allocator<TYPE> > >::value
                        ? e_ADAPTOR
                        : e_STATEFUL;
    static
    const bool k_IS_MOVABLE = bslmf::IsBitwiseMoveable<TYPE>::value    ||
                              !bsl::is_copy_constructible<TYPE>::value ||
                              bsl::is_nothrow_move_constructible<TYPE>::value;
    static
    const bool k_IS_WELL_BEHAVED =
            bsl::is_same<TYPE, bsltf::WellBehavedMoveOnlyAllocTestType>::value;

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

    static int checkFirstNElemAddr(typename Obj::const_pointer *pointers,
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

    static void
    stretch(Obj *object, std::size_t size, int identifier = int('Z'));
        // Using only primary manipulators, extend the length of the specified
        // 'object' by the specified 'size' by adding copies of the specified
        // 'value'.  The resulting value is not specified.

    static void
    stretchRemoveAll(Obj *object, std::size_t size, int identifier = int('Z'));
        // Using only primary manipulators, extend the capacity of the
        // specified 'object' to (at least) the specified 'size' by adding
        // copies of the optionally specified 'value'; then remove all elements
        // leaving 'object' empty.

    template <class ARG_TYPE>
    static bslmf::MovableRef<ARG_TYPE> testArg(ARG_TYPE& t, bsl::true_type )
    {
        return MoveUtil::move(t);
    }

    template <class ARG_TYPE>
    static const ARG_TYPE&             testArg(ARG_TYPE& t, bsl::false_type)
    {
        return t;
    }
    static std::size_t allocationsByPrimaryManipulator()
        // Return the number of allocations made by the primary manipulator
        // inserting a single element into a 'vector' that has sufficient
        // capacity.
    {
        bslma::TestAllocator ta("Count allocs by primary-manipulator");

        typename Obj::allocator_type a(&ta);
        Obj mX(a);
        primaryManipulator(&mX, 'a');
        mX.clear();

        const Int64 ALLOCS_TO_PREPARE = ta.numAllocations();
        primaryManipulator(&mX, 'a');

        return static_cast<std::size_t>(
                                      ta.numAllocations() - ALLOCS_TO_PREPARE);
    }
};

// Unfortunately these names have to be made available "by hand" due to two
// phase name lookup not reaching into dependent bases.
#define BSLSTL_VECTOR_0T_PULL_TESTSUPPORT_NAMES                               \
    typedef TestSupport<TYPE, ALLOC> Base;                                    \
                                                                              \
    typedef typename Base::Obj                    Obj;                        \
                                                                              \
    typedef typename Base::iterator               iterator;                   \
    typedef typename Base::const_iterator         const_iterator;             \
    typedef typename Base::reverse_iterator       reverse_iterator;           \
    typedef typename Base::const_reverse_iterator const_reverse_iterator;     \
    typedef typename Base::ValueType              ValueType;                  \
                                                                              \
    typedef typename Base::CIter                  CIter;                      \
                                                                              \
    typedef typename Base::TstMoveUtil            TstMoveUtil;                \
    typedef typename Base::TestValues             TestValues;                 \
                                                                              \
    typedef typename Base::AllocatorTraits        AllocatorTraits;            \
                                                                              \
    typedef typename Base::AllocCategory          AllocCategory;              \
                                                                              \
    using Base::s_allocCategory;                                              \
    using Base::k_IS_MOVABLE;                                                 \
    using Base::k_IS_WELL_BEHAVED;                                            \
                                                                              \
    static const AllocCategory e_BSLMA         = Base::e_BSLMA;               \
    static const AllocCategory e_STDALLOC      = Base::e_STDALLOC;            \
    static const AllocCategory e_ADAPTOR       = Base::e_ADAPTOR;             \
    static const AllocCategory e_STATEFUL      = Base::e_STATEFUL;            \
                                                                              \
    using Base::allocationsByPrimaryManipulator;                              \
    using Base::allocCategoryAsStr;                                           \
    using Base::checkFirstNElemAddr;                                          \
    using Base::storeFirstNElemAddr;                                          \
    using Base::stretch;                                                      \
    using Base::stretchRemoveAll;                                             \
    using Base::testArg

                 // =======================================
                 // template class StdBslmaTestDriverHelper
                 // =======================================

template <template <class, class> class DRIVER, class TYPE>
class StdBslmaTestDriverHelper : public DRIVER<
                            TYPE,
                            bsltf::StdAllocatorAdaptor<bsl::allocator<TYPE> > >
{
};

                                // --------------
                                // TEST APPARATUS
                                // --------------

template <class TYPE, class ALLOC>
void TestSupport<TYPE, ALLOC>::stretch(Obj         *object,
                                       std::size_t  size,
                                       int          identifier)
{
    ASSERT(object);
    for (std::size_t i = 0; i < size; ++i) {
        primaryManipulator(object, identifier);
    }
    ASSERT(object->size() >= size);
}

template <class TYPE, class ALLOC>
void TestSupport<TYPE, ALLOC>::stretchRemoveAll(Obj         *object,
                                                std::size_t  size,
                                                int          identifier)
{
    ASSERT(object);
    stretch(object, size, identifier);
    object->clear();
    ASSERT(0 == object->size());
}

#ifndef BSLSTL_VECTOR_0T_AS_INCLUDE

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main()
{
    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    (void)DEFAULT_MAX_LENGTH;
    (void)DEFAULT_DATA;
    (void)DEFAULT_NUM_DATA;
    (void)NUM_ALLOCS;
    (void)NUM_NUM_ALLOCS;
    (void)ZERO;
    (void)LARGE_SIZE_VALUE;

    return -1;
}

#endif  // not defined 'BSLSTL_VECTOR_0T_AS_INCLUDE'

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
