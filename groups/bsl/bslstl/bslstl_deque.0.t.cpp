// bslstl_deque.0.t.cpp                                               -*-C++-*-
#include <bslstl_deque.h>

// This test driver contains common test machinery for all 'bslstl_deque' test
// drivers.  If '#include'd after defining the 'BSLSTL_DEQUE_0T_AS_INCLUDE'
// macro it will just provide those common pieces (and no 'main' function).

#include <bsla_fallthrough.h>
#include <bsla_unused.h>

#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>
#include <bslstl_randomaccessiterator.h>
#include <bslstl_string.h>
#include <bslstl_vector.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_rawdeleterproctor.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_ispointer.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentutil.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_allocargumenttype.h>
#include <bsltf_allocemplacabletesttype.h>
#include <bsltf_argumenttype.h>
#include <bsltf_emplacabletesttype.h>
#include <bsltf_movablealloctesttype.h>
#include <bsltf_movabletesttype.h>
#include <bsltf_moveonlyalloctesttype.h>
#include <bsltf_movestate.h>
#include <bsltf_nondefaultconstructibletesttype.h>
#include <bsltf_nontypicaloverloadstesttype.h>
#include <bsltf_stdallocatoradaptor.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_wellbehavedmoveonlyalloctesttype.h>

#include <iterator>
#include <new>         // placement 'new'
#include <stdexcept>
#include <utility>     // move

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Don't put any 'using' declaration or directive here, see swap helpers below.

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
// NOTICE: To reduce the compilation time, this test driver has been broken
// into 4 parts, 'bslstl_deque.0.t.cpp' (common code), 'bslstl_deque.1.t.cpp'
// (cases 1-10, and usage examples), 'bslstl_deque.2.t.cpp' (cases 11-22), and
// 'bslstl_deque.3.t.cpp' (cases 23 and higher).
//
//                                 Overview
//                                 --------
// The class under test is a container whose interface and contract is dictated
// by the C++ standard.  In particular, the standard mandates strong exception
// safety (with full guarantee of rollback) in many cases, along with throwing
// 'std::length_error' on any request to increase the size of a deque to have
// more than 'max_size()' elements.  (Note: the value returned by 'max_size'
// depends on the (template parameter) 'VALUE_TYPE'.)  The general concerns are
// compliance, exception safety, and proper dispatching (for member function
// templates such as 'assign' and 'insert').  In addition, deque is a
// value-semantic type whose salient attributes are its size and the value of
// each element in its (ordered) sequence.  The deque container is implemented
// in the form of a class template, so its proper instantiation for several
// types is a concern.  Regarding the allocator template argument, we use
// mostly a 'bsl::allocator' together with a 'bslma::TestAllocator' mechanism,
// but we also verify the C++ standard allocator.
//
// Important states that must be covered by testing include filling "pages" (or
// "blocks") of the deque up to their page boundaries, both at the 'front' and
// the 'back', and behavior crossing those boundaries.  Therefore, the
// generator language provides a way to insert, independently, at the front and
// at the back, to fill up pages at either the front or the back.
//
// This test driver follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' and 'push_front' methods taking rvalue references as well as the
// 'clear' method, with all three methods to be used by the generator function
// 'gg'.  Note that this choice is conditional on the implementation of
// 'clear', which clears all blocks and then sets 'd_start' to 'd_finish'.
// Additional helper functions are provided to facilitate perturbation of
// internal state (e.g., capacity and the start iterator within the start
// block).  Note that some manipulators must support aliasing, and those that
// perform memory allocation must be tested for exception neutrality via
// 'bslma::TestAllocator'.  After the mandatory sequence of cases (1-10) for
// value-semantic types (cases 5 and 9 are not implemented, as there is no
// output or BDEX streaming below 'bslstl'), we test each individual
// constructor, manipulator, and accessor in subsequent cases.  Move semantics
// (C++11) are tested within relevant test cases.  Note that due to the test
// driver split up test cases 11 and 12 are also the case numbers for the usage
// example tests in test driver part 1.
//
// Abbreviations
// -------------
// Throughout this test driver, we use:
//  T        VALUE_TYPE (template argument, no default)
//  A        ALLOCATOR (template argument, default is 'bsl::allocator<T>')
//  deque    bsl::deque<VALUE_TYPE, ALLOCATOR>
//  Args...  shorthand for a family of templates <A1>, <A1, A2>, etc.
// ----------------------------------------------------------------------------
// class bsl::deque<T, A>
// ============================================================================
// ~~ ALL (except 0)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// [ *] CONCERN: In no case does memory come from the global allocator.
//
// ~~ bslstl_deque.1.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CREATORS
// [ 2] deque(const A& a = A());
// [ 2] ~deque();
// [ 7] deque(const deque& original, const A& = A());
//
/// MANIPULATORS
// [ 2] void clear();
// [ 8] deque& operator=(const deque& rhs);
//
// ACCESSORS
// [ 4] allocator_type get_allocator() const;
// [ 4] size_type size() const;
// [ 4] const_reference operator[](size_type position) const;
// [ 4] const_reference at(size_type position) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const deque& lhs, const deque& rhs);
// [ 6] bool operator!=(const deque& lhs, const deque& rhs);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING TEST MACHINERY
// [11] USAGE EXAMPLE 1
// [12] USAGE EXAMPLE 2
// [10] ALLOCATOR-RELATED CONCERNS
//
// ----------------------------------------------------------------------------
// [-1] PERFORMANCE TEST
//
// ~~ bslstl_deque.2.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CREATORS
// [11] deque(size_type n, const A& a = A());
// [11] deque(size_type n, const T& value, const A& a = A());
// [11] deque(ITER first, ITER last, const A& a = A());
//
/// MANIPULATORS
// [12] void assign(size_type n, const T& value);
// [12] void assign(ITER first, ITER last);
// [13] void reserve(size_type n);
// [13] void resize(size_type n);
// [13] void resize(size_type n, const T& value);
// [16] void push_front(const T& value);
// [16] void push_back(const T& value);
// [19] void pop_front();
// [19] void pop_back();
// [17] iterator insert(const_iterator pos, const T& value);
// [17] iterator insert(const_iterator pos, size_type n, const T& value);
// [18] iterator insert(const_iterator pos, ITER first, ITER last);
// [19] iterator erase(const_iterator pos);
// [19] iterator erase(const_iterator first, const_iterator last);
// [20] void swap(deque& other);
// [15] iterator begin();
// [15] iterator end();
// [15] reverse_iterator rbegin();
// [15] reverse_iterator rend();
// [14] reference operator[](size_type position);
// [14] reference at(size_type position);
// [14] reference front();
// [14] reference back();
//
// ACCESSORS
// [13] size_type max_size() const;
// [13] size_type capacity() const;
// [13] bool empty() const;
// [15] const_iterator begin() const;
// [15] const_iterator cbegin() const;
// [15] const_iterator end() const;
// [15] const_iterator cend() const;
// [15] const_reverse_iterator rbegin() const;
// [15] const_reverse_iterator crbegin() const;
// [15] const_reverse_iterator rend() const;
// [15] const_reverse_iterator crend() const;
// [14] const_reference front() const;
// [14] const_reference back() const;
//
// FREE OPERATORS
// [21] bool operator< (const deque& lhs, const deque& rhs);
// [21] bool operator> (const deque& lhs, const deque& rhs);
// [21] bool operator<=(const deque& lhs, const deque& rhs);
// [21] bool operator>=(const deque& lhs, const deque& rhs);
// [21] auto operator<=>(const deque& lhs, const deque& rhs);
//
// FREE FUNCTIONS
// [20] void swap(deque& a, deque& b);
// [33] size_t erase(deque<T,A>&, const U&);
// [33] size_t erase_if(deque<T,A>&, PREDICATE);
// ----------------------------------------------------------------------------
// [11] CONCERN: non-copyable/non-movable types are supported.
// [13] CONCERN: non-copyable/non-movable types are supported.
// [22] CONCERN: 'std::length_error' is used properly.
//
// ~~ bslstl_deque.3.t.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// CREATORS
// [23] deque(deque&& original);
// [23] deque(deque&& original, const A& basicAllocator);
// [29] deque(initializer_list<T>, const A& = A());
//
/// MANIPULATORS
// [24] deque& operator=(deque&& rhs);
// [29] deque& operator=(initializer_list<T>);
// [29] void assign(initializer_list<T>);
// [30] void shrink_to_fit();
// [25] void push_front(T&& rvalue);
// [25] void push_back(T&& rvalue);
// [27] reference emplace_front(Args&&... args);
// [27] reference emplace_back(Args&&... args);
// [28] iterator emplace(const_iterator pos, Args&&... args);
// [26] iterator insert(const_iterator pos, T&& rvalue);
// [29] iterator insert(const_iterator pos, initializer_list<T>);
//
// ----------------------------------------------------------------------------
// [31] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [32] CLASS TEMPLATE DEDUCTION GUIDES

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

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                  TARGET SPECIFIC CONFIGURATION
// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SIMULATE_FORWARD_WORKAROUND)
# define BSL_DO_NOT_TEST_MOVE_FORWARDING 1
// Some compilers produce ambiguities when trying to construct our test types
// for 'emplace'-type functionality with the C++03 move-emulation.  This is a
// compiler bug triggering in lower level components, so we simply disable
// those aspects of testing, and rely on the extensive test coverage on other
// platforms.
#endif

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
using bsl::deque;

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// uses 'bslma' allocators
class TestTypeAlloc;
class BitwiseMoveableTestTypeAlloc;

typedef TestTypeAlloc                   TTA;
typedef BitwiseMoveableTestTypeAlloc    BMTTA;

// does not use 'bslma' allocators
class SmallTestTypeNoAlloc;
class MediumTestTypeNoAlloc;
class LargeTestTypeNoAlloc;
class BitwiseCopyableTestTypeNoAlloc;

typedef SmallTestTypeNoAlloc            SmlTT;
typedef MediumTestTypeNoAlloc           MedTT;
typedef LargeTestTypeNoAlloc            LrgTT;
typedef BitwiseCopyableTestTypeNoAlloc  BCTT;

typedef bsls::Types::IntPtr             IntPtr;
typedef bsls::Types::Int64              Int64;
typedef bsls::Types::Uint64             Uint64;

// CONSTANTS
const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';
const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
    // All test types have character value type.

const int NOMINAL_BLOCK_BYTES = bsl::Deque_BlockLengthCalcUtil<int>::
                                                            DEFAULT_BLOCK_SIZE;
    // Used 'int', but any type will do here, since DEFAULT_BLOCK_SIZE is
    // independent of the type.

const int MEDIUM_TEST_TYPE_SIZE = NOMINAL_BLOCK_BYTES / 2;
    // Size such that there should be only two objects in a deque block.

const int LARGE_TEST_TYPE_SIZE = NOMINAL_BLOCK_BYTES * 2;
    // Size such that there should be only a single object in a deque block.

const int  LARGE_SIZE_VALUE = 10;
    // Declare a large value for insertions into the deque.  Note this value
    // will cause multiple resizes during insertion into the deque.

const int NUM_PADDING = 2;
    // BLOCK_ARRAY_PADDING as defined in 'bslalg_dequeimputil'.

const int NUM_INTERNAL_STATE_TEST = 10;
    // Number of different internal states to check.

const int k_MINIMUM_PAGE_LENGTH = NOMINAL_BLOCK_BYTES;

// ============================================================================
//                             DEFAULT TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA used in multiple test cases.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // canonical equivalent spec
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "a",                 "A" },
    { L_,    2, "aa",                "AA" },
    { L_,    3, "aBCaBC",            "AABCBC" },
    { L_,    4, "aBCa",              "AABC" },
    { L_,    5, "aB",                "AB" },
    { L_,    6, "BCaB",              "ABCB" },
    { L_,    7, "BaC",               "ABC" },
    { L_,    9, "BCDa",              "ABCD" },
    { L_,   10, "BCaDE",             "ABCDE" },
    { L_,   11, "FEDCBa",            "AFEDCB" },
    { L_,   13, "BCDEFaGH",          "ABCDEFGH" },
    { L_,   14, "GCDFEabHI",         "BAGCDFEHI" },
    { L_,   15, "BFEDFGaMNOPHbIJKL", "BABFEDFGMNOPHIJKL" },
    { L_,   17, "BaD",               "ABD" },
    { L_,   19, "aC",                "AC" },
    { L_,   20, "B",                 "B" },
    { L_,   21, "BCDE",              "BCDE" },
    { L_,   22, "FEDCBc",            "CFEDCB" },
    { L_,   23, "cde",               "EDC" },
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

// ============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

                               // debugPrint

namespace bsl {

template <class TYPE, class ALLOC>
void debugprint(const bsl::deque<TYPE,ALLOC>& v)
{
    using bsls::debugprint;

    if (v.empty()) {
        printf("<empty>");
    }
    else {
        putchar('"');
        for (size_t ii = 0; ii < v.size(); ++ii) {
            putchar(static_cast<char>(
                           bsltf::TemplateTestFacility::getIdentifier(v[ii])));
        }
        putchar('"');
    }
}

}  // close namespace bsl

                             // verifySpec

template <class CONTAINER>
bool verifySpec(const CONTAINER& c, const char *spec)
{
    const size_t sz = c.size();
    if (strlen(spec) != sz) {
        return false;                                                 // RETURN
    }

    for (unsigned ii = 0; ii < sz; ++ii) {
        if (c[ii] != spec[ii]) {
            return false;                                             // RETURN
        }
    }

    return true;
}

// ============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

// STATIC DATA
static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

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
class ExceptionProctor {
    // This class provides a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores a copy
    // of an object of the (template parameter) type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // created on construction.  This class requires that the copy constructor
    // and 'operator ==' to be tested before use.

    // DATA
    int                         d_line;      // line number at construction
    bsls::ObjectBuffer<OBJECT>  d_control;   // copy of the object being tested
    const OBJECT               *d_object_p;  // address of the original object

  private:
    // NOT IMPLEMENTED
    ExceptionProctor(const ExceptionProctor&);
    ExceptionProctor& operator=(const ExceptionProctor&);

  public:
    // CREATORS
    ExceptionProctor(const OBJECT    *object,
                     int              line,
                     const ALLOCATOR& basicAllocator = ALLOCATOR())
    : d_line(line)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number that uses the specified 'basicAllocator' to
        // supply memory for the control object.
    {
        if (d_object_p) {
            new (d_control.buffer()) OBJECT(*object, basicAllocator);
        }
    }

    ExceptionProctor(const OBJECT     *object,
                     const OBJECT&     control,
                     int               line,
                     const ALLOCATOR&  basicAllocator = ALLOCATOR())
    : d_line(line)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.  Use
        // the specified 'basicAllocator' to supply memory for the control
        // object.
    {
        if (d_object_p) {
            new (d_control.buffer()) OBJECT(control, basicAllocator);
        }
    }

    ExceptionProctor(const OBJECT              *object,
                     bslmf::MovableRef<OBJECT>  control,
                     int                        line)
    : d_line(line)
    , d_object_p(object)
        // Create an exception proctor for the specified 'object' at the
        // specified 'line' number using the specified 'control' object.
    {
        if (d_object_p) {
            new (d_control.buffer()) OBJECT(
                                         bslmf::MovableRefUtil::move(control));
        }
    }

    ~ExceptionProctor()
        // Destroy the exception proctor.  If the proctor was not released,
        // verify that the state of the object supplied at construction has not
        // changed.
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
        // Release the guard from verifying the state of the object.
    {
        if (d_object_p) {
            d_control.object().~OBJECT();
        }
        d_object_p = 0;
    }
};

                               // ===================
                               // class TestTypeAlloc
                               // ===================

class TestTypeAlloc {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default constructions, copy constructions, assignments,
    // and destructions.  It declares the 'bslma::UsesBslmaAllocator' trait.
    // It could also have the bitwise moveable trait, but we defer that to the
    // 'BitwiseMoveableTestTypeAlloc' type.

    // DATA
    char             *d_data_p;
    bslma::Allocator *d_allocator_p;

#if defined(BDE_USE_ADDRESSOF)
    // PRIVATE ACCESSORS
    void operator&() const;     // = delete;
        // Suppress the use of address-of operator on this type.
#endif

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestTypeAlloc, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    TestTypeAlloc(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestTypeAlloc(int c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = static_cast<char>(c);
    }

    TestTypeAlloc(const TestTypeAlloc& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCopyCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }

    ~TestTypeAlloc()
    {
        ++numDestructorCalls;
        *d_data_p = UNINITIALIZED_VALUE;
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
        d_allocator_p = 0;
    }

    // MANIPULATORS
    TestTypeAlloc& operator=(const TestTypeAlloc& rhs)
    {
        ++numAssignmentCalls;
        if (BSLS_UTIL_ADDRESSOF(rhs) != this) {
            char *newData = (char *)d_allocator_p->allocate(sizeof(char));
            *d_data_p = UNINITIALIZED_VALUE;
            d_allocator_p->deallocate(d_data_p);
            d_data_p  = newData;
            *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(char c)
    {
        *d_data_p = c;
    }

    // ACCESSORS
    operator int() const
    {
        return static_cast<int>(*d_data_p);
    }

    char value() const
    {
        return *d_data_p;
    }

    bslma::Allocator *allocator() const
    {
        return d_allocator_p;
    }

    void print() const
    {
        if (d_data_p) {
            ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

// FREE OPERATORS
bool operator==(const TestTypeAlloc& lhs, const TestTypeAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

#ifdef BSLALG_SYNTHTHREEWAYUTIL_AVAILABLE
auto operator<=>(const TestTypeAlloc& lhs, const TestTypeAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() <=> rhs.value();
}
#else
bool operator<(const TestTypeAlloc& lhs, const TestTypeAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() < rhs.value();
}
#endif

// TestTypeAlloc-specific print function.
void debugprint(const TestTypeAlloc& rhs)
{
    printf("%c", rhs.value());
//    fflush(stdout);
}

                       // ==========================
                       // class SmallTestTypeNoAlloc
                       // ==========================

class SmallTestTypeNoAlloc {
    // This test type has a footprint and interface identical to
    // 'TestTypeAlloc', except that it does not use allocators.  It also counts
    // the number of default constructions, copy constructions, assignments,
    // and destructions.  It does not allocate, and thus could have the bitwise
    // copyable trait, but we defer that to the
    // 'BitwiseCopyableTestTypeNoAlloc' type.

    // DATA
    union {
        char d_char;
        char d_fill[sizeof(TestTypeAlloc)];
    } d_u;

  public:
    // CREATORS
    SmallTestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    SmallTestTypeNoAlloc(int c)
    {
        d_u.d_char = static_cast<char>(c);
        ++numCharCtorCalls;
    }

    SmallTestTypeNoAlloc(const SmallTestTypeNoAlloc& original)
    {
        d_u.d_char = original.d_u.d_char;
        ++numCopyCtorCalls;
    }

    ~SmallTestTypeNoAlloc()
    {
        ++numDestructorCalls;
        d_u.d_char = '_';
    }

    // MANIPULATORS
    SmallTestTypeNoAlloc& operator=(const SmallTestTypeNoAlloc& rhs)
    {
        ++numAssignmentCalls;
        d_u.d_char = rhs.d_u.d_char;
        return *this;
    }

    // ACCESSORS
    operator int() const
    {
        return static_cast<int>(d_u.d_char);
    }

    char value() const
    {
        return d_u.d_char;
    }

    void print() const
    {
        ASSERT(isalpha(d_u.d_char));
        printf("%c (int: %d)\n", d_u.d_char, (int)d_u.d_char);
    }
};

// FREE OPERATORS
bool operator==(const SmallTestTypeNoAlloc& lhs,
                const SmallTestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

// SmallTestTypeNoAlloc-specific print function.
void debugprint(const SmallTestTypeNoAlloc& rhs)
{
    printf("%c", rhs.value());
    fflush(stdout);
}

                       // ===========================
                       // class MediumTestTypeNoAlloc
                       // ===========================

class MediumTestTypeNoAlloc {
    // This test type has a larger footprint than 'TestTypeAlloc', so that
    // there are fewer objects in a deque's block.  It also counts the number
    // of default constructions, copy constructions, assignments, and
    // destructions.  It does not allocate, and thus could have the bitwise
    // copyable trait, but we defer that to the
    // 'BitwiseCopyableTestTypeNoAlloc' type.

    // DATA
    union {
        char d_char;
        char d_fill[MEDIUM_TEST_TYPE_SIZE];
    } d_u;

  public:
    // CREATORS
    MediumTestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    MediumTestTypeNoAlloc(int c)
    {
        d_u.d_char = static_cast<char>(c);
        ++numCharCtorCalls;
    }

    MediumTestTypeNoAlloc(const MediumTestTypeNoAlloc&  original)
    {
        d_u.d_char = original.d_u.d_char;
        ++numCopyCtorCalls;
    }

    ~MediumTestTypeNoAlloc()
    {
        ++numDestructorCalls;
        d_u.d_char = '_';
    }

    // MANIPULATORS
    MediumTestTypeNoAlloc& operator=(const MediumTestTypeNoAlloc& rhs)
    {
        ++numAssignmentCalls;
        d_u.d_char = rhs.d_u.d_char;
        return *this;
    }

    // ACCESSORS
    operator int() const
    {
        return static_cast<int>(d_u.d_char);
    }

    char value() const
    {
        return d_u.d_char;
    }

    void print() const
    {
        ASSERT(isalpha(d_u.d_char));
        printf("%c (int: %d)\n", d_u.d_char, (int)d_u.d_char);
    }
};

// FREE OPERATORS
bool operator==(const MediumTestTypeNoAlloc& lhs,
                const MediumTestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

// MediumTestTypeNoAlloc-specific print function.
void debugprint(const MediumTestTypeNoAlloc& rhs)
{
    printf("%c", rhs.value());
//    fflush(stdout);
}

                       // ==========================
                       // class LargeTestTypeNoAlloc
                       // ==========================

class LargeTestTypeNoAlloc {
    // This test type has a much larger footprint than 'TestTypeAlloc', so that
    // there are just 16 objects in a deque's block.  It also counts the number
    // of default constructions, copy constructions, assignments, and
    // destructions.  It does not allocate, and thus could have the bitwise
    // copyable trait, but we defer that to the
    // 'BitwiseCopyableTestTypeNoAlloc' type.

    // DATA
    union {
        char d_char;
        char d_fill[LARGE_TEST_TYPE_SIZE];
    } d_u;

  public:
    // CREATORS
    LargeTestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    LargeTestTypeNoAlloc(int c)
    {
        d_u.d_char = static_cast<char>(c);
        ++numCharCtorCalls;
    }

    LargeTestTypeNoAlloc(const LargeTestTypeNoAlloc&  original)
    {
        d_u.d_char = original.d_u.d_char;
        ++numCopyCtorCalls;
    }

    ~LargeTestTypeNoAlloc()
    {
        ++numDestructorCalls;
        d_u.d_char = '_';
    }

    // MANIPULATORS
    LargeTestTypeNoAlloc& operator=(const LargeTestTypeNoAlloc& rhs)
    {
        ++numAssignmentCalls;
        d_u.d_char = rhs.d_u.d_char;
        return *this;
    }

    // ACCESSORS
    operator int() const
    {
        return static_cast<int>(d_u.d_char);
    }

    char value() const
    {
        return d_u.d_char;
    }

    void print() const
    {
        ASSERT(isalpha(d_u.d_char));
        printf("%c (int: %d)\n", d_u.d_char, (int)d_u.d_char);
    }
};

// FREE OPERATORS
bool operator==(const LargeTestTypeNoAlloc& lhs,
                const LargeTestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

// LargeTestTypeNoAlloc-specific print function.
void debugprint(const LargeTestTypeNoAlloc& rhs)
{
    printf("%c", rhs.value());
//    fflush(stdout);
}

                       // ==================================
                       // class BitwiseMoveableTestTypeAlloc
                       // ==================================

class BitwiseMoveableTestTypeAlloc : public TestTypeAlloc {
    // This test type is identical to 'TestTypeAlloc' except that it has the
    // bitwise moveable trait.  All members are inherited.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMoveableTestTypeAlloc,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMoveableTestTypeAlloc,
                                   bslmf::IsBitwiseMoveable);

    // CREATORS
    explicit
    BitwiseMoveableTestTypeAlloc(bslma::Allocator *ba = 0)
    : TestTypeAlloc(ba)
    {
    }

    explicit
    BitwiseMoveableTestTypeAlloc(int c, bslma::Allocator *ba = 0)
    : TestTypeAlloc(c, ba)
    {
    }

    BitwiseMoveableTestTypeAlloc(const BitwiseMoveableTestTypeAlloc&  original,
                                 bslma::Allocator                    *ba = 0)
    : TestTypeAlloc(original, ba)
    {
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // MANIPULATORS
    BitwiseMoveableTestTypeAlloc& operator=(
                            const BitwiseMoveableTestTypeAlloc& rhs) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif
};

                       // ====================================
                       // class BitwiseCopyableTestTypeNoAlloc
                       // ====================================

class BitwiseCopyableTestTypeNoAlloc : public SmallTestTypeNoAlloc {
    // This test type is identical to 'SmallTestTypeNoAlloc' except that it has
    // the bitwise copyable and bitwise equality-comparable traits.  All
    // members are inherited.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyableTestTypeNoAlloc,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyableTestTypeNoAlloc,
                                   bslmf::IsBitwiseEqualityComparable);

    // CREATORS
    BitwiseCopyableTestTypeNoAlloc()
    : SmallTestTypeNoAlloc()
    {
    }

    explicit
    BitwiseCopyableTestTypeNoAlloc(int c)
    : SmallTestTypeNoAlloc(c)
    {
        ++numCharCtorCalls;
    }

    BitwiseCopyableTestTypeNoAlloc(
                                const BitwiseCopyableTestTypeNoAlloc& original)
    : SmallTestTypeNoAlloc(original.value())
    {
    }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // MANIPULATORS
    BitwiseCopyableTestTypeNoAlloc& operator=(
                          const BitwiseCopyableTestTypeNoAlloc& rhs) = default;
        // Assign to this object the value of the specified 'rhs', and return
        // a reference providing modifiable access to this object.
#endif
};

                               // ==============
                               // class CharList
                               // ==============

template <class TYPE>
class CharList {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the (template
    // parameter) 'TYPE'.

    // DATA
    bsl::vector<TYPE> d_value;

  public:
    // TYPES
    typedef bslstl::ForwardIterator<const TYPE, const TYPE *> const_iterator;
        // Input iterator.

    // CREATORS
    CharList() {}
    explicit CharList(const bsl::vector<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE>
CharList<TYPE>::CharList(const bsl::vector<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE&
CharList<TYPE>::operator[](size_t index) const
{
    return d_value[index];
}

template <class TYPE>
typename CharList<TYPE>::const_iterator
CharList<TYPE>::begin() const
{
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename CharList<TYPE>::const_iterator
CharList<TYPE>::end() const
{
    return const_iterator(d_value.end());
}

                              // ===============
                              // class CharArray
                              // ===============

template <class TYPE>
class CharArray {
    // This array class is a simple wrapper on a deque offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically a *random-access* iterator and its value type is the
    // (template parameter) 'TYPE'.

    // DATA
    bsl::vector<TYPE>  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    CharArray() {}
    explicit CharArray(const bsl::vector<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE>
CharArray<TYPE>::CharArray(const bsl::vector<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE& CharArray<TYPE>::operator[](size_t index) const
{
    return d_value[index];
}

template <class TYPE>
typename CharArray<TYPE>::const_iterator CharArray<TYPE>::begin() const
{
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename CharArray<TYPE>::const_iterator CharArray<TYPE>::end() const
{
    return const_iterator(d_value.end());
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

    template <class BDE_OTHER_TYPE>
    struct rebind {
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

  private:
    // NOT IMPLEMENTED
    LimitAllocator& operator=(const LimitAllocator&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    LimitAllocator()
    : d_limit(-1) {}

    explicit
    LimitAllocator(bslma::Allocator *mechanism)
    : AllocBase(mechanism)
    , d_limit(-1)
    {
    }

    explicit
    LimitAllocator(const ALLOC& alloc)
    : AllocBase((const AllocBase&)alloc)
    , d_limit(-1)
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
{
};

}  // close namespace bslmf
}  // close enterprise namespace

                             // =====================
                             // class NonCopyableType
                             // =====================

class NonCopyableType {
    // Non-copyable and non-movable type.

    // DATA
    int value;

    // NOT IMPLEMENTED
    NonCopyableType(const NonCopyableType &) BSLS_KEYWORD_DELETED;
    NonCopyableType &operator=(const NonCopyableType &) BSLS_KEYWORD_DELETED;
  public:
    // CREATORS
    NonCopyableType() : value(0)
        // Create a 'NonCopyableType' object.
     {
     }

    // FREE OPERATORS
    friend bool operator==(const NonCopyableType &v1,
                           const NonCopyableType &v2)
        // Equality comparison.
    {
        return v1.value == v2.value;
    }

#ifndef BSLS_COMPILERFEATURES_SUPPORT_THREE_WAY_COMPARISON
    friend bool operator!=(const NonCopyableType &v1,
                           const NonCopyableType &v2)
        // Inequality comparison.
    {
        return !(v1 == v2);
    }
#endif
};

                        // ==========================
                        // template struct PageLength
                        // ==========================

template <class T>
struct PageLength {
    // This class indicates, through an 'enum' 'k_VALUE', the number of objects
    // that can be inserted onto a page of a 'deque<T>'.  Note that the
    // effective capacity when inserting at the back is one fewer, as the last
    // slot must be occupied by a non-created object, which is the target of
    // the 'end' iterator.  This slot *is* occupied when inserting the last
    // element, but it requires allocating a fresh page with a new address for
    // the dummy one-past-the-end object.  This template must be specialized
    // with a hand-computed result for each type under test, in order to verify
    // that the internally computed page-size corresponds to the value that we
    // wish to test.
    //
    // Note that the explicit specializations of this class that defined its
    // value members must follow all the types that we wish to support in this
    // file, so that we can evaluate static assertions on the types' sizes.

    enum { k_VALUE = 16 };  // default value; see specializations below
};

                        // =========================
                        // class template PageLength
                        // =========================

template <>
struct PageLength<char> {
    typedef char Type;

    BSLMF_ASSERT(sizeof(Type) == 1);  // by definition, but preserves pattern

    enum { k_VALUE = 200 };
};

template <>
struct PageLength<TTA> {
    typedef TTA Type;

    BSLMF_ASSERT(sizeof(Type) == 2 * sizeof(void *));

    enum { k_VALUE = sizeof(char *) == 4 ? 25
                   : sizeof(char *) == 8 ? 16
                   : 0 };
};

template <>
struct PageLength<SmlTT> {
    typedef SmlTT Type;

    BSLMF_ASSERT(sizeof(Type) == 2 * sizeof(void *));

    enum { k_VALUE = sizeof(char *) == 4 ? 25
                   : sizeof(char *) == 8 ? 16
                   : 0 };
};

template <>
struct PageLength<MedTT> {
    typedef MedTT Type;

    BSLMF_ASSERT(sizeof(Type) < k_MINIMUM_PAGE_LENGTH &&
                 sizeof(Type) > k_MINIMUM_PAGE_LENGTH / 16);

    enum { k_VALUE = 16 };
};

template <>
struct PageLength<LrgTT> {
    typedef LrgTT Type;

    BSLMF_ASSERT(sizeof(Type) > k_MINIMUM_PAGE_LENGTH);

    enum { k_VALUE = 16 };
};

template <>
struct PageLength<BMTTA> {
    typedef BMTTA Type;

    BSLMF_ASSERT(sizeof(Type) == 2 * sizeof(void *));

    enum { k_VALUE = sizeof(char *) == 4 ? 25
                   : sizeof(char *) == 8 ? 16
                   : 0 };
};

template <>
struct PageLength<BCTT> {
    typedef BCTT Type;

    BSLMF_ASSERT(sizeof(Type) == 2 * sizeof(void *));

    enum { k_VALUE = sizeof(char *) == 4 ? 25
                   : sizeof(char *) == 8 ? 16
                   : 0 };
};

// ============================================================================
//                            TEST APPARATUS
// ----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestSupport {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters '[A .. E]' correspond to arbitrary (but unique) 'char'
    // values to be appended to the 'bsl::deque<T>' object.  A tilde ('~')
    // indicates that the logical (but not necessarily physical) state of the
    // object is to be set to its initial, empty state (via the 'clear'
    // method).
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
    // <ITEM>       ::= <ELEMENT> | <POP_BACK> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'H'
    //                                      // unique but otherwise arbitrary
    // <POP_BACK>   ::= '<'
    //
    // <CLEAR>      ::= '~'
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Append the value corresponding to A.
    // "AA"         Append two values both corresponding to A.
    // "ABC"        Append three values corresponding to A, B and C.
    // "ABC~"       Append three values corresponding to A, B and C and then
    //              remove all the elements (set array length to 0).  Note that
    //              this spec yields an object that is logically equivalent
    //              (but not necessarily identical internally) to one
    //              yielded by ("").
    // "ABC~DE"     Append three values corresponding to A, B, and C; empty
    //              the object; and append values corresponding to D and E.
    //              (Note that the resulting object has the capacity for at
    //              least three elements at the *front* and none, or the
    //              remainder of the block, at the back.)
    // "ABC<<<~DE"  Append three values corresponding to A, B, and C, then pop
    //              them back; empty the object; and append values
    //              corresponding to D and E.  (Note that the result object has
    //              a capacity for at least one more object at the back, and
    //              none at the front.)
    // ------------------------------------------------------------------------

    // CONSTANTS
    static const int START_POS  =
        -1 * NUM_PADDING * NOMINAL_BLOCK_BYTES / (int) sizeof (TYPE);
        // The integer offset to the first element pointed by the first block
        // pointer of a newly created empty 'bsl::deque'.

    static const int FINISH_POS =
        ((NUM_PADDING + 1) * NOMINAL_BLOCK_BYTES / (int) sizeof (TYPE)) - 1;
        // The integer offset to the last element pointed by the last block
        // pointer of a newly created empty 'bsl::deque'.

    static const int INCREMENT =
        FINISH_POS - START_POS >= NUM_INTERNAL_STATE_TEST
            ? (FINISH_POS - START_POS) / NUM_INTERNAL_STATE_TEST
            : 1;
        // The integer increment that will be used when permutating through the
        // various internal states.

    static const bool k_IS_WELL_BEHAVED =
        bsl::is_same<TYPE, bsltf::WellBehavedMoveOnlyAllocTestType>::value;

    // TYPES
    typedef bsl::deque<TYPE, ALLOC>               Obj;

    typedef typename Obj::iterator                iterator;
    typedef typename Obj::const_iterator          const_iterator;
    typedef typename Obj::reverse_iterator        reverse_iterator;
    typedef typename Obj::const_reverse_iterator  const_reverse_iterator;
    typedef typename Obj::value_type              ValueType;

    typedef typename Obj::const_iterator          CIter;

    typedef bslma::ConstructionUtil               ConstrUtil;
    typedef bslmf::MovableRefUtil                 MoveUtil;

    typedef bsltf::MoveState                      MoveState;
    typedef bsltf::TemplateTestFacility           TstFacility;
    typedef bsltf::TestValuesArray<TYPE>          TestValues;

    typedef bsl::allocator_traits<ALLOC>          AllocatorTraits;

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

    static void primaryManipulatorBack(Obj   *container,
                                       int    identifier)
        // Insert at the back of specified 'container' the value object
        // indicated by the specified 'identifier', ensuring that the overload
        // of the primary manipulator taking a modifiable rvalue is invoked
        // (rather than the one taking an lvalue).

    {
        bsls::ObjectBuffer<ValueType> buffer;
        TstFacility::emplace(buffer.address(),
                             identifier,
                             container->get_allocator());
        bslma::DestructorGuard<ValueType> guard(buffer.address());

        container->push_back(MoveUtil::move(buffer.object()));
    }

    static void primaryManipulatorFront(Obj   *container,
                                        int    identifier)
        // Insert at the front of specified 'container' the value object
        // indicated by the specified 'identifier', ensuring that the overload
        // of the primary manipulator taking a modifiable rvalue is invoked
        // (rather than the one taking an lvalue).
    {
        bsls::ObjectBuffer<ValueType> buffer;
        TstFacility::emplace(buffer.address(),
                             identifier,
                             container->get_allocator());
        bslma::DestructorGuard<ValueType> guard(buffer.address());

        container->push_front(MoveUtil::move(buffer.object()));
    }

    static int getValues(const TYPE **valuesPtr);
        // Load the specified 'valuesPtr' with the address of an array
        // containing initialized values of the (template parameter) 'TYPE',
        // and return the length of that array.

    static int ggg(Obj *object, const char *spec, bool verboseFlag = true);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push_back' and
        // white-box manipulator 'clear'.  Optionally specify a 'false'
        // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
        // index of the first invalid character, and a negative value
        // otherwise.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.
        //
        // The language of the grammar consists of characters in the ranges
        // 'a..z' and 'A..Z', to insert a new object to either the front or the
        // back of the deque, '*object', where lower-case indicates the front
        // and upper-case indicates the back.  A sequence of identical objects
        // can be inserted by preceding the letter with a digit, [2..0], where
        // 0 represents 10 objects.  For tests that require filling to a page
        // boundary of the deque, a sequence contained between '<' and '|' will
        // cause a sequence of insertions to fill the deque all the way to the
        // next page boundary with a an object corresponding to a supplied
        // character, where a lower-case character will fill to the front page
        // of the deque, and an upper-case character will fill the back page.
        // Padding to allow a few additional insertions before reaching the
        // page boundary is requested by a sequence of dots, where '...' would
        // mean to leave room for three more elements before reading the page
        // boundary.  Larger sequences of free slots can be requested by a
        // leading digit before a '.', where 0 indicates 10.  No more than 10
        // free slots can be requested this way.  To aid clarity, non-salient
        // whitespace is permitted to easily depict groups of operations.  If
        // the current page is already full, in the desired direction, or does
        // not have sufficient capacity to honor the requested number of free
        // slots, then an error result will be returned.
        //
        // Arbitrary sequences of elements can be pushed to the front and back
        // of a deque using 'ggg' at any point, but the fill-to-boundary
        // functionality is supported only for deques in their initial (default
        // constructed) state, which is empty and a capacity indicating only
        // one page has been allocated.  Any attempt to interpret a 'spec'
        // string containing a fill-to-end range will return an error unless
        // '*object' is in the default-constructed state.  Note that the
        // insert-point for a push in this state must also correspond to the
        // middle of the page, per a default-constructed deque.
        //
        // Examples:
        // ---------
        // "A"      // push_back a single element of configuration 'A'.
        // "a"      // push_front a single element of configuration 'A'.
        // "7A 3b"  // push_back 7 'A' objects, then push_front 3 'B' objects.
        // "A<B|"   // push_back an 'A', then fill-to-back with 'B'.
        // "A<b.|"  // push_back an 'A', then fill-to-front with 'B', leaving
        //          // space at the front for exactly one more object.
        //
        // "<A...|BCC <a...|bcc CCC7D ccc7d"
        //   Fill to the back with 'A' objects, leaving space for 3 more before
        //   an allocation would be required, then push_back a 'B' object
        //   followed by two 'C' objects.  Then, fill to the front with 'A'
        //   objects, leaving space for 3 more before an allocation would be
        //   required, then push_front a 'B' object followed by two 'C'
        //   objects, exactly filling the first page.  Then push_back a 'C'
        //   object (causing an allocation) followed by another 2 C objects,
        //   and then seven 'D' objects.  Then push_front another 'C' object,
        //   followed by two more 'C' objects and then another seven 'D'
        //   objects.
        //
        // "<A| A<A| A<A| 3A"
        //   Fill the current empty deque towards the back with 'A' objects,
        //   filling from the middle of the initial page, creating and filling
        //   two more complete pages, and finally pushed 3 'A' objects to the
        //   start of the fourth page.
        //
        // "<A| A<A| 3A ~ <A| A"
        //   Populate a default-constructed deque to the back of the first page
        //   then fill a newly allocated second page with 'A' objects, and then
        //   insert three more 'A' objects at the start of a third page.  Then
        //   clear the deque, and fill to the back of the current page.
        //   Finally push_back one more 'A' object, which should re-use one of
        //   the existing pages rather than allocate a new page.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static bsl::vector<TYPE> gV(const char *spec);
        // Return, by value, a new vector corresponding to the specified
        // 'spec'.

    static void resetMovedInto(Obj *object)
    {
        for (size_t i = 0; i < object->size(); ++i) {
            TstFacility::setMovedIntoState(BSLS_UTIL_ADDRESSOF((*object)[i]),
                                           bsltf::MoveState::e_NOT_MOVED);
        }
    }

    static void stretch(Obj         *object,
                        std::size_t  size,
                        int          identifier = int('Z'));
        // Using only primary manipulators, extend the length of the specified
        // 'object' by the specified 'size' by adding 'size' copies of the
        // value indicated by the optionally specified 'identifier'.  If
        // 'identifier' is not specified, 'Z' is used.  The resulting value is
        // not specified.

    static void stretchRemoveAll(Obj         *object,
                                 std::size_t  size,
                                 int          identifier = int('Z'));
        // Using only primary manipulators, extend the capacity of the
        // specified 'object' by the specified 'size' by adding 'size' copies
        // of the value indicated by the optionally specified 'identifier'.  If
        // 'identifier' is not specified, 'Z' is used.  The resulting value is
        // not specified.

    static int findRoomierEnd(Obj *container);
        // Determine which end of the specified '*container' has more room.
        // Return a value > 0 if it's the front, < 0 if it's the back, and
        // 0 if the two ends have the same amount of room.  Note that this
        // function modifies 'container' but restores it to its original state.

    static void setInternalState(Obj *object, int n);
        // Using 'push_back', 'push_front', 'pop_back', and 'pop_front', set
        // the internal state of the specified 'object' by moving the start and
        // finish iterators by the specified 'n' positions.
};

// Unfortunately these names have to be made available "by hand" due to two
// phase name lookup not reaching into dependent bases.
#define BSLSTL_DEQUE_0T_PULL_TESTSUPPORT_NAMES                                \
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
    typedef typename Base::ConstrUtil             ConstrUtil;                 \
    typedef typename Base::MoveUtil               MoveUtil;                   \
                                                                              \
    typedef typename Base::MoveState              MoveState;                  \
    typedef typename Base::TstFacility            TstFacility;                \
    typedef typename Base::TestValues             TestValues;                 \
                                                                              \
    typedef typename Base::AllocatorTraits        AllocatorTraits;            \
                                                                              \
    typedef typename Base::AllocCategory          AllocCategory;              \
                                                                              \
    using Base::START_POS;                                                    \
    using Base::FINISH_POS;                                                   \
    using Base::INCREMENT;                                                    \
    using Base::k_IS_WELL_BEHAVED;                                            \
    using Base::s_allocCategory;                                              \
                                                                              \
    static const AllocCategory e_BSLMA         = Base::e_BSLMA;               \
    static const AllocCategory e_STDALLOC      = Base::e_STDALLOC;            \
    static const AllocCategory e_ADAPTOR       = Base::e_ADAPTOR;             \
    static const AllocCategory e_STATEFUL      = Base::e_STATEFUL;            \
                                                                              \
    using Base::allocCategoryAsStr;                                           \
    using Base::findRoomierEnd;                                               \
    using Base::getValues;                                                    \
    using Base::gg;                                                           \
    using Base::ggg;                                                          \
    using Base::gV;                                                           \
    using Base::primaryManipulatorBack;                                       \
    using Base::primaryManipulatorFront;                                      \
    using Base::setInternalState;                                             \
    using Base::stretch;                                                      \
    using Base::stretchRemoveAll

// ----------------------------------------------------------------------------
//                    TEST APPARATUS - IMPLEMENTATIONS
// ----------------------------------------------------------------------------

template <class TYPE, class ALLOC>
int TestSupport<TYPE,ALLOC>::getValues(const TYPE **valuesPtr)
{
    bslma::DefaultAllocatorGuard
                                guard(&bslma::NewDeleteAllocator::singleton());

    static TYPE values[5]; // avoid DEFAULT_VALUE and UNINITIALIZED_VALUE
    values[0] = TYPE(VA);
    values[1] = TYPE(VB);
    values[2] = TYPE(VC);
    values[3] = TYPE(VD);
    values[4] = TYPE(VE);

    const int NUM_VALUES = 5;

    *valuesPtr = values;
    return NUM_VALUES;
}

template <class TYPE, class ALLOC>
int TestSupport<TYPE,ALLOC>::ggg(Obj        *object,
                                 const char *spec,
                                 bool        verboseFlag)
{
    BSLS_ASSERT_OPT(object);
    BSLS_ASSERT_OPT(spec);

    // In order to execute the 'spec' program, we are going to need to track a
    // little state.  Based on the known size of a page (counted in objects) we
    // can separately track the available capacity to the front and back of the
    // currently inserting blocks.  The numbers must be tracked through every
    // insertion operation.  Several characters when parsed indicate the start
    // of a command sequence, which should be parsed in full before evaluating
    // as a single command:
    //..
    //  '<'   : Record a command sequence until a '|' character
    //  digit : Repeat the next (simple) command digit times.
    //..
    // Note that a 'digit' can occur in a insert-to-boundary sequence.

    enum { SUCCESS = -1 };

    int  frontCapacity = (PageLength<TYPE>::k_VALUE - 1) / 2;
    int  backCapacity  = PageLength<TYPE>::k_VALUE - 1 - frontCapacity;
    int  repeatCount   = 1;
    int  tailPadding   = 0;
    bool applyToBack   = true;  // 'true' > 'push back', 'false' > 'push_front'
    int  fillPage      = -1;    // index of 'start range' command

    int VALUE = 0;  // 0 when in "immediate mode"; otherwise, the identifier of
                    // the object to fill-to-page-boundary with.

    // The parser below would be much simpler with C++11 lambda expressions.

    for (int i = 0; spec[i]; ++i) {
        if (' ' == spec[i]) {
            continue;   // whitespace is not salient in any parse mode
        }

        if (-1 == fillPage) {  // immediate mode, not forming a fill-page range
            if ('A' <= spec[i] && spec[i] <= 'Y') {
                for (int j = 0; j != repeatCount; ++j) {
                    primaryManipulatorBack(object, spec[i]);
                    if (backCapacity) {
                        --backCapacity;
                    }
                    else {
                        backCapacity = PageLength<TTA>::k_VALUE - 1;
                    }
                }
            }
            else if ('a' <= spec[i] && spec[i] <= 'y') {
                for (int j = 0; j != repeatCount; ++j) {
                    primaryManipulatorFront(object, toupper(spec[i]));
                    if (frontCapacity) {
                        --frontCapacity;
                    }
                    else {
                        frontCapacity = PageLength<TTA>::k_VALUE - 1;
                    }
                }
            }
            else if (1 != repeatCount) {
                if (verboseFlag) {
                    printf("Error, bad character ('%c') in spec \"%s\" at"
                           " position %d: cannot follow a repeat-count.\n",
                           spec[i], spec, i);
                }
                return i;  // Discontinue processing this spec.       // RETURN
            }
            else if ('~' == spec[i]) {
                object->clear();
                backCapacity  = PageLength<TTA>::k_VALUE / 2;
                frontCapacity = (PageLength<TYPE>::k_VALUE + 1) / 2 -1;;
            }
            else if ('<' == spec[i]) {
                fillPage = i;    // Enter fill-page mode
                // Note we cannot diagnose an at-boundary error until we know
                // whether we push at the front or the back, so wait until the
                // range direction is specified before (potentially) reporting
                // an error.
            }
            else if ('2' <= spec[i] && spec[i] <= '9') {
                repeatCount = spec[i] - '0';
            }
            else if ('0' == spec[i]) {
                repeatCount = 10;
            }
            // Any other character is invalid, although we can give stronger
            // diagnostics in some cases.
            else if ('|' == spec[i]) {
                if (verboseFlag) {
                    printf("Error, bad character ('%c') in spec \"%s\" at"
                           " position %d: terminator for an unopened range.\n",
                           spec[i], spec, i);
                }
                return i;  // Discontinue processing this spec.       // RETURN
            }
            else if ('.' == spec[i]) {
                if (verboseFlag) {
                    printf("Error, bad character ('%c') in spec \"%s\" at"
                           " position %d: padding allowed only in a range.\n",
                           spec[i], spec, i);
                }
                return i;  // Discontinue processing this spec.       // RETURN
            }
            else {
                if (verboseFlag) {
                    printf("Error, bad character ('%c') in spec \"%s\" at"
                           " position %d.\n", spec[i], spec, i);
                }
                return i;  // Discontinue processing this spec.       // RETURN
            }
        }
        else {  // forming a fill-page range
            if ('|' == spec[i]) { // execute range-fill
                if (0 == VALUE) {
                    if (verboseFlag) {
                        printf("Error, invalid fill-range in spec \"%s\""
                               " starting at position %d: no fill char.\n",
                               spec, fillPage);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }

                int *capacity = applyToBack ? &backCapacity : &frontCapacity;
                if (*capacity <= tailPadding) {
                    if (verboseFlag) {
                        printf("Error, invalid fill-range in spec \"%s\""
                               " starting at position %d: no room for padding."
                               "\n", spec, fillPage);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }

//T_; P(*capacity);
                while (*capacity != tailPadding) {
                    if (applyToBack) {
                        primaryManipulatorBack(object, VALUE);
                    }
                    else {
                        primaryManipulatorFront(object, VALUE);
                    }
                    --*capacity;
                }

                // Reset state after fill
                fillPage    = -1;
                repeatCount = 1;
                tailPadding = 0;
                VALUE       = 0;
            }
            else if ('A' <= spec[i] && spec[i] <= 'E') {
                if (0 != VALUE) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: only one fill char allowed.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }

                applyToBack = true;
                VALUE = spec[i];
            }
            else if ('a' <= spec[i] && spec[i] <= 'e') {
                if (0 != VALUE) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: only one fill char allowed.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }

                applyToBack = false;
                VALUE = toupper(spec[i]);
            }
            else if ('.' == spec[i]) { // tail padding
                if (0 == VALUE) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: padding preceding fill char.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }
                tailPadding += repeatCount;
            }
// TBD
#if 0  // Future extension      - allow a repeater for tail-padding.
       // Implementation detail - introduces another bool state to track.
       //     If we do not track the state-flag, then a simple trailing digit
       //     would indicate padding, without an optional '.' suffix.  Not sure
       //     if we should allow this, or pick it up as an error, but currently
       //     no expected use cases - will review again after updating the
       //     'insert' test case.
            else if ('2' <= spec[i] && spec[i] <= '9') {
                if (0 != tailPadding) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: digit cannot follow padding.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }
                if (0 == VALUE) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: no repeated objects in range.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }
                repeatCount = spec[i] - '0';
            }
            else if ('0' == spec[i]) {
                if (0 != tailPadding) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: digit cannot follow padding.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }
                if (0 == VALUE) {
                    if (verboseFlag) {
                        printf("Error, bad character ('%c') in spec \"%s\" at"
                               " position %d: no repeated objects in range.\n",
                               spec[i], spec, i);
                    }
                    return i;  // Discontinue processing this spec.   // RETURN
                }
                repeatCount = 10;
            }
#endif
            // We could provide nicer errors for '1', '<', and '~'
            else {
                if (verboseFlag) {
                    printf("Error, bad character ('%c') in spec \"%s\" at"
                           " position %d.\n", spec[i], spec, i);
                }
                return i;  // Discontinue processing this spec.       // RETURN
            }
        }
    }

    if (-1 != fillPage) {  // We have a range command that is not terminated.
        if (verboseFlag) {
            printf("Error, bad character ('<') in spec \"%s\" at"
                   " position %d: nonterminated fill-range.\n",
                   spec, fillPage);
        }
        return fillPage;  // Discontinue processing this spec.        // RETURN
    }

    resetMovedInto(object);

    return SUCCESS;
}

template <class TYPE, class ALLOC>
bsl::deque<TYPE,ALLOC>& TestSupport<TYPE,ALLOC>::gg(Obj        *object,
                                                    const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class ALLOC>
bsl::vector<TYPE> TestSupport<TYPE,ALLOC>::gV(const char *spec)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    bsl::vector<TYPE> result;
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            result.push_back(VALUES[spec[i] - 'A']);
        }
        else if ('<' == spec[i]) {
            result.pop_back();
        }
        else if ('~' == spec[i]) {
            result.clear();
        }
        else {
            ASSERT(0);
        }
   }
   return result;
}

template <class TYPE, class ALLOC>
void TestSupport<TYPE,ALLOC>::setInternalState(Obj *object, int n)
{
    ASSERT(object);
    ASSERT(0 == object->size());
    const TYPE value = TYPE();
    if (n > 0) {
        for (; n > 0; --n) {
            object->push_back(value);
            object->pop_front();
        }
    }
    else {
        for (; n < 0; ++n) {
            object->push_front(value);
            object->pop_back();
        }
    }
    ASSERT(0 == object->size());
}

template <class TYPE, class ALLOC>
void TestSupport<TYPE,ALLOC>::stretch(Obj         *object,
                                      std::size_t  size,
                                      int          identifier)
{
    ASSERT(object);

    const std::size_t initialSize = object->size();

    bslma::TestAllocator scratch;

    for (std::size_t i = 0; i < size; ++i) {
// TBD perhaps add an argument to stretch: front only, back only, both ends
//        if (0 == i % 2) {
            primaryManipulatorBack(object, identifier);
//        }
//        else {
//            primaryManipulatorFront(object, identifier, &scratch);
//        }
    }
    ASSERT(object->size() == initialSize + size);
}

template <class TYPE, class ALLOC>
void TestSupport<TYPE,ALLOC>::stretchRemoveAll(Obj         *object,
                                               std::size_t  size,
                                               int          identifier)
{
    ASSERT(object);
    stretch(object, size, identifier);
    object->clear();
    ASSERT(0 == object->size());
}

template <class TYPE, class ALLOC>
int TestSupport<TYPE, ALLOC>::findRoomierEnd(Obj *container)
{
    typedef typename Obj::size_type size_type;

    const size_type cap = container->capacity();

    if (container->empty()) {
        // The container will always have room for at least one element without
        // doing any block allocations.

        container->push_back(TYPE(VA));
        const bool backGrew = container->capacity() > cap;
        container->pop_back();

        container->push_front(TYPE(VA));
        const bool frontGrew = container->capacity() > cap;
        container->pop_front();

        ASSERT(backGrew || frontGrew);

        if (backGrew) {
            if (frontGrew) {
                return 0;                                             // RETURN
            }
            return -1;                                                // RETURN
        }
        return 1;                                                     // RETURN
    }

    TYPE val = container->back();
    container->pop_back();
    const bool backShrunk = container->capacity() < cap;
    container->push_back(val);

    val = container->front();
    container->pop_front();
    const bool frontShrunk = container->capacity() < cap;
    container->push_front(val);

    ASSERT(backShrunk || frontShrunk);

    if (backShrunk) {
        if (frontShrunk) {
            return 0;                                                 // RETURN
        }
        return -1;                                                    // RETURN
    }
    return 1;
}

                     // ===============================
                     // template struct AssertAllocator
                     // ===============================

template <class TYPE,
           bool TYPE_USES_ALLOCATOR = bslma::UsesBslmaAllocator<TYPE>::value>
struct AssertAllocator {
    void operator() (const TYPE& element, bslma::Allocator *allocator) const
    {
        // Assert that the specified 'element' has the specified 'allocator'.
        // This function template exists to allow conditional testing of
        // allocator properties in template code where not all types use
        // allocators, but all branches within the function must parse
        // correctly for any type.

        ASSERT(element.allocator() == allocator);
    }
};

template <class TYPE>
struct AssertAllocator<TYPE, false> {
    void operator() (const TYPE&, bslma::Allocator *) const
    {}
};

                    // =================================
                    // template class StatelessAllocator
                    // =================================

template <class TYPE>
struct StatelessAllocator {
    // Stateless std allocator with 'is_always_equal == true_type'

    // TYPES
    typedef TYPE      value_type;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;

    typedef value_type       *pointer;
    typedef const value_type *const_pointer;

    template <class OTHER_TYPE> struct rebind {
        typedef StatelessAllocator<OTHER_TYPE> other;
    };

    typedef bsl::true_type is_always_equal;

    // CREATORS
    StatelessAllocator()
        // Create a 'StatelessAllocator' object.
    {
    }
    template <class OTHER_TYPE>
    StatelessAllocator(const StatelessAllocator<OTHER_TYPE>&)
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

                 // =======================================
                 // template class StdBslmaTestDriverHelper
                 // =======================================

template <template <class, class> class DRIVER, class TYPE>
class StdBslmaTestDriverHelper : public DRIVER<
                            TYPE,
                            bsltf::StdAllocatorAdaptor<bsl::allocator<TYPE> > >
{
};

#ifndef BSLSTL_DEQUE_0T_AS_INCLUDE

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main()
{
    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    (void)LARGE_SIZE_VALUE;
    (void)DEFAULT_DATA;

    return -1;
}

#endif  // not defined 'BSLSTL_DEQUE_0T_AS_INCLUDE'

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
