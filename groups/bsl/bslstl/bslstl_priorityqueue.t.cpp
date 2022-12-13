// bslstl_priorityqueue.t.cpp                                         -*-C++-*-
#include <bslstl_priorityqueue.h>

#include <bslstl_vector.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_assert.h>
#include <bslmf_haspointersemantics.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_nameof.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <algorithm>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility> // move

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    // BSLS_ASSERT_OPT(a.get_allocator() == b.get_allocator());

    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a value-semantic container adapter whose state
// is represented by the underlying container it holds.  The component can be
// constructed with or without a memory allocator depending on whether the held
// container needs it or not.  The component does not have public method to
// access configured allocator, so we use a helper method 'use_same_allocator',
// which allocates memory, to verify the configured allocator.  The component
// does not have equality operator, so we use another help method 'is_equal',
// which change the component's state, to verify the component was in an
// expected state.
//
// Primary Manipulators:
//: o priority_queue();
//: o void push(const value_type& value);
//: o void pop();
//
// Basic Accessors:
//: o size_type size() const;
//: o const_reference top() const;
//
// Global Assumptions:
//: o ACCESSOR methods are 'const'.
// ----------------------------------------------------------------------------
// CLASS 'bsl::priority_queue'
//
// CREATORS
// [ 2] priority_queue();
// [12] priority_queue(const COMPARATOR& comp, const CONTAINER& cont);
// [16] explicit priority_queue(const COMPARATOR&, MovableRef<CONTAINER>);
// [12] explicit priority_queue(const COMPARATOR& comparator);
// [12] priority_queue(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [12] priority_queue(ITER, ITER, const COMPARATOR&, const CONTAINER&);
// [16] priority_queue(ITER, ITER, const COMPARATOR&, MovableRef<CONTAINER>);
// [ 7] priority_queue(const priority_queue&);
// [15] priority_queue(MovableRef<priority_queue> original);
// [ 2] explicit priority_queue(const ALLOCATOR& allocator);
// [12] priority_queue(const COMPARATOR& comp, const ALLOCATOR& allocator);
// [12] priority_queue(const COMPARATOR&, const CONTAINER&, const ALLOCATOR&);
// [16] priority_queue(const COMPARATOR&, MovRef<CONTAINER>, const ALLOCATOR&);
// [ 7] priority_queue(const priority_queue&, const ALLOCATOR&);
//
// MANIPULATORS
// [ 9] priority_queue& operator=(const priority_queue& rhs);
// [18] priority_queue& operator=(MovableRef<priority_queue> rhs);
// [ 2] void push(const value_type& value);
// [17] void push(MovableRef<value_type> value);
// [ 2] void pop();
// [ 8] void swap(priority_queue& other);
// [19] void emplace(Args&&... args);
//
// ACCESSORS
// [13] bool empty() const;
// [ 4] size_type size() const;
// [ 4] const_reference top() const;
//
// specialized algorithms:
// [ 8] void swap(priority_queue& lhs, priority_queue& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] TRAITS
// [20] TESTING NON ALLOCATOR SUPPORTING TYPE
// [21] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(priority_queue *object, const char *spec, int verbose = 1);
// [ 3] priority_queue& gg(priority_queue *object, const char *spec);
//
// [ 5] TESTING OUTPUT: Not Applicable
// [10] STREAMING: Not Applicable
// [**] CONCERN: The object is compatible with STL allocator.
// [22] CONCERN: Methods qualified 'noexcept' in standard are so implemented.
// [23] CLASS TEMPLATE DEDUCTION GUIDES

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// Define default data for testing 'bsl::priority_queue'.

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line spec                 results
    //---- ----                 -------
    { L_,  "",                  "" },
    { L_,  "A",                 "A" },
    { L_,  "B",                 "B" },
    { L_,  "AA",                "AA"},
    { L_,  "AB",                "BA" },
    { L_,  "BA",                "BA" },
    { L_,  "AC",                "CA" },
    { L_,  "CD",                "DC" },
    { L_,  "ABC",               "CBA" },
    { L_,  "ACB",               "CBA" },
    { L_,  "BAC",               "CBA" },
    { L_,  "BCA",               "CBA" },
    { L_,  "CAB",               "CBA" },
    { L_,  "CBA",               "CBA" },
    { L_,  "BAD",               "DBA" },
    { L_,  "ABCA",              "CBAA" },
    { L_,  "ABCB",              "CBBA" },
    { L_,  "ABCC",              "CCBA" },
    { L_,  "ABCD",              "DCBA" },
    { L_,  "ACBD",              "DCBA" },
    { L_,  "BDCA",              "DCBA" },
    { L_,  "DCBA",              "DCBA" },
    { L_,  "BEAD",              "EDBA" },
    { L_,  "BCDE",              "EDCB" },
    { L_,  "ABCDE",             "EDCBA" },
    { L_,  "ACBDE",             "EDCBA" },
    { L_,  "CEBDA",             "EDCBA" },
    { L_,  "EDCBA",             "EDCBA" },
    { L_,  "FEDCB",             "FEDCB" },
    { L_,  "FEDCBA",            "FEDCBA" },
    { L_,  "ABCABC",            "CCBBAA" },
    { L_,  "AABBCC",            "CCBBAA" },
    { L_,  "ABCDEFG",           "GFEDCBA" },
    { L_,  "ABCDEFGH",          "HGFEDCBA" },
    { L_,  "ABCDEFGHI",         "IHGFEDCBA" },
    { L_,  "ABCDEFGHIJKLMNOP",  "PONMLKJIHGFEDCBA" },
    { L_,  "PONMLKJIGHFEDCBA",  "PONMLKJIHGFEDCBA" },
    { L_,  "ABCDEFGHIJKLMNOPQ", "QPONMLKJIHGFEDCBA" },
    { L_,  "DHBIMACOPELGFKNJQ", "QPONMLKJIHGFEDCBA" },
};

static
const int DEFAULT_NUM_DATA =
                  static_cast<int>(sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA);

typedef bsltf::AllocTestType TestValueType;
typedef bslmf::MovableRefUtil MoveUtil;

int SPECIAL_INT_VALUES[]       = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
int NUM_SPECIAL_INT_VALUES     =
    static_cast<int>(sizeof(SPECIAL_INT_VALUES) / sizeof(*SPECIAL_INT_VALUES));

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS
# define TEST_TYPES_EMPLACE                                                   \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseCopyableTestType,                                       \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType,                                  \
        bsltf::NonTypicalOverloadsTestType

# define TEST_TYPES_REGULAR                                                   \
        TEST_TYPES_EMPLACE,                                                   \
        bsltf::TemplateTestFacility::FunctionPtr

#else
# define TEST_TYPES_EMPLACE                                                   \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseCopyableTestType,                                       \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType,                                  \
        bsltf::NonTypicalOverloadsTestType

# define TEST_TYPES_REGULAR                                                   \
        TEST_TYPES_EMPLACE
#endif

// 'priority_queue'-specific print function.

template <class VALUE, class CONTAINER, class COMPARATOR>
void debugprint(const bsl::priority_queue<VALUE, CONTAINER, COMPARATOR>& pq)
{
    if (pq.empty()) {
        printf("<empty>");
    }
    else {
        printf("size: %d, top: ", (int) pq.size());
        bsls::BslTestUtil::callDebugprint(static_cast<char>(
                        bsltf::TemplateTestFacility::getIdentifier(pq.top())));
    }
    fflush(stdout);
}

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                            // =======================
                            // class NonAllocContainer
                            // =======================

template <class VALUE>
class NonAllocContainer
    // This class is a value-semantic class template, acting as a transparent
    // proxy for the underlying 'bsl::vector' container, that holds elements of
    // the (template parameter) 'VALUE', using 'bslma::MallocFreeAllocator' to
    // supply memory (an allocator that implements the 'bslma::Allocator'
    // protocol and supplies memory using the system-supplied (native)
    // 'std::malloc' and 'std::free' operators).
{
  private:
    // DATA
    bsl::vector<VALUE> d_vector;  // container for it's behavior simulation

  public:
    // PUBLIC TYPES
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;
    typedef VALUE*       iterator;
    typedef const VALUE* const_iterator;

    // CREATORS
    NonAllocContainer()
        // Create an empty object.
    : d_vector(&bslma::MallocFreeAllocator::singleton())
    {}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS)
    NonAllocContainer(const NonAllocContainer& other) = default;
#endif

    ~NonAllocContainer()
        // Destroy this object.
    {}

    // MANIPULATORS
    NonAllocContainer& operator=(const NonAllocContainer& rhs)
        // Assign to this object the value of the specified 'rhs' object
        // and return a reference providing modifiable access to this object.
    {
        d_vector = rhs.d_vector;
        return *this;
    }

    void pop_back()
        // Erase the last element from the underlying container.
    {
        d_vector.pop_back();
    }

    void push_back(const value_type& value)
        // Append a copy of the specified 'value' at the end of the underlying
        // container.
    {
        d_vector.push_back(value);
    }

    bsl::vector<value_type>& contents()
        // Return a reference providing modifiable access to the underlying
        // container.
    {
        return d_vector;
    }

    // ACCESSORS
    bool operator==(const NonAllocContainer& rhs) const
        // Return 'true' if the specified 'rhs' object has the same value as
        // current object, and 'false' otherwise.
    {
        return d_vector == rhs.d_vector;
    }

    bool operator!=(const NonAllocContainer& rhs) const
        // Return 'true' if the specified 'rhs' object does not have the same
        // value as current object, and 'false' otherwise.
    {
        return !operator==(rhs);
    }

    bool operator<(const NonAllocContainer& rhs) const
        // Return 'true' if the specified 'rhs' object is lexicographically
        // larger than current object, and 'false' otherwise.
    {
        return d_vector < rhs.d_vector;
    }

    bool operator>=(const NonAllocContainer& rhs) const
        // Return 'true' if the specified 'rhs' object is lexicographically
        // smaller than or equal to the current object, and 'false'
        // otherwise.
    {
        return !operator<(rhs);
    }

    bool operator>(const NonAllocContainer& rhs) const
        // Return 'true' if the specified 'rhs' object is lexicographically
        // smaller than the current object, and 'false' otherwise.
    {
        return d_vector > rhs.d_vector;
    }

    bool operator<=(const NonAllocContainer& rhs) const
        // Return 'true' if the specified 'rhs' object is lexicographically
        // larger than or equal to the current object, and 'false'
        // otherwise.
    {
        return !operator>(rhs);
    }

    const_iterator begin() const
        // Return an iterator providing non-modifiable access to the first
        // element of the underlying container, or the 'end' iterator if the
        // underlying container is empty.
    {
        return d_vector.begin();
    }

    const_iterator end() const
        // Return an iterator providing non-modifiable access to the
        // past-the-end element in the underlying container.
    {
        return d_vector.end();
    }

    iterator begin()
        // Return an iterator providing modifiable access to the first element
        // of the underlying container (or the past-the-end iterator if the
        // underlying container is empty).
    {
        return d_vector.begin();
    }

    iterator end()
        // Return the past-the-end iterator for the underlying container.
    {
        return d_vector.end();
    }

    const_reference front() const
        // Return a reference providing non-modifiable access to the first
        // element of the underlying container.  The behavior is undefined if
        // the underlying container is empty.
    {
        return d_vector.front();
    }

    size_type size() const
        // Return the number of elements in the underlying container.
    {
        return d_vector.size();
    }

    bool empty() const
        // Return 'true' if the underlying container has size 0, and 'false'
        // otherwise.
    {
        return d_vector.empty();
    }
};

namespace std {
    template <class VALUE>
    void swap(NonAllocContainer<VALUE>& a, NonAllocContainer<VALUE>& b)
        // Exchange the container of the specified 'a' object with the
        // container of the specified 'b' object.
    {
        a.contents().swap(b.contents());
    }
}  // close namespace std

enum CalledMethod
    // Enumerations used to indicate if appropriate special container's method
    // has been invoked.

{
    e_NONE                              = 0,        // None method
    e_APPROPRIATE_CONSTRUCTOR           = 1 << 0,   // Appropriate constructor
    e_INAPPROPRIATE_CONSTRUCTOR         = 1 << 1,   // Inappropriate ctor
    e_APPROPRIATE_CTOR_WITH_ALLOCATOR   = 1 << 2,   // Appropriate ctor with
                                                    // allocator as a parameter

    e_INAPPROPRIATE_CTOR_WITH_ALLOCATOR = 1 << 3,   // Inappropriate ctor with
                                                    // allocator as a parameter

    e_APPROPRIATE_ASSIGNMENT_OPERATOR   = 1 << 4,   // Appropriate assignment
                                                    // operator

    e_INAPPROPRIATE_ASSIGNMENT_OPERATOR = 1 << 5,   // Inappropriate assignment
                                                    // operator

    e_APPROPRIATE_PUSH_BACK             = 1 << 6,   // 'push_back' method with
                                                    // appropriate parameter

    e_INAPPROPRIATE_PUSH_BACK           = 1 << 7,   // 'push_back' method with
                                                    // inappropriate parameter

    e_EMPLACE                           = 1 << 8,   // 'emplace_back' method
                                                    // without parameters

    e_EMPLACE_1                         = 1 << 9,   // 'emplace_back' method
                                                    // with one parameter

    e_EMPLACE_2                         = 1 << 10,  // 'emplace_back' method
                                                    // with two parameters

    e_EMPLACE_3                         = 1 << 11,  // 'emplace_back' method
                                                    // with three parameters

    e_EMPLACE_4                         = 1 << 12,  // 'emplace_back' method
                                                    // with four parameters

    e_EMPLACE_5                         = 1 << 13,  // 'emplace_back' method
                                                    // with five parameters

    e_EMPLACE_6                         = 1 << 14,  // 'emplace_back' method
                                                    // with six parameters

    e_EMPLACE_7                         = 1 << 15,  // 'emplace_back' method
                                                    // with seven parameters

    e_EMPLACE_8                         = 1 << 16,  // 'emplace_back' method
                                                    // with eight parameters

    e_EMPLACE_9                         = 1 << 17,  // 'emplace_back' method
                                                    // with nine parameters

    e_EMPLACE_10                        = 1 << 18   // 'emplace_back' method
                                                    // with ten parameters
};

inline CalledMethod operator|=(CalledMethod& lhs, CalledMethod rhs)
    // Bitwise OR the values of the specified 'lhs' and 'rhs' flags, and return
    // the resulting value.
{
    lhs = static_cast<CalledMethod>(
                                static_cast<int>(lhs) | static_cast<int>(rhs));
    return lhs;
}

CalledMethod g_calledMethodFlag;  // global variable that stores information
                                  // about called methods for special
                                  // containers 'NonMovableContainer' and
                                  // 'MovableContainer'

                        // =========================
                        // class NonMovableContainer
                        // =========================

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class NonMovableContainer
    // This class is a value-semantic class template, acting as a transparent
    // proxy for the underlying 'bsl::vector' container, that holds elements of
    // the (template parameter) 'VALUE', and recording in the global variable
    // 'g_calledMethodFlag' methods being invoked.  The information recorded
    // is used to verify that 'bsl::priority_queue' invokes expected container
    // methods.
{
  private:
    // DATA
    bsl::vector<VALUE> d_vector;  // container for it's behavior simulation

  public:
    // PUBLIC TYPES
    typedef ALLOCATOR    allocator_type;
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;
    typedef VALUE*       iterator;
    typedef const VALUE* const_iterator;

    // CREATORS
    NonMovableContainer()
    : d_vector()
        // Create an empty object.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_CONSTRUCTOR;
    }

    NonMovableContainer(const ALLOCATOR& basicAllocator)
    : d_vector( basicAllocator)
        // Create an empty object that uses the specified 'basicAllocator' to
        // supply memory.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_CONSTRUCTOR;
    }

    NonMovableContainer(const NonMovableContainer& original)
        // Create an object that has the same value as the specified 'original'
        // object.  Method invocation is recorded.
    : d_vector(original.d_vector)
    {
        g_calledMethodFlag |= e_APPROPRIATE_CONSTRUCTOR;
    }

    NonMovableContainer(const NonMovableContainer& original,
                     const ALLOCATOR& basicAllocator)
        // Create an object that has the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Method invocation is recorded.
    : d_vector(original.d_vector, basicAllocator)
    {
        g_calledMethodFlag |= e_APPROPRIATE_CTOR_WITH_ALLOCATOR;
    }

    // MANIPULATORS
    NonMovableContainer& operator=(const NonMovableContainer& rhs)
        // Assign to this object the value of the specified 'other' object and
        // return a reference providing modifiable access to this object.
        // Method invocation is recorded.
    {
        d_vector = rhs.d_vector;
        g_calledMethodFlag |= e_APPROPRIATE_ASSIGNMENT_OPERATOR;
        return *this;
    }

    void pop_back()
        // Erase the last element from the underlying container.
    {
        d_vector.pop_back();
    }

    void push_back(const value_type& value)
        // Append a copy of the specified 'value' at the end of the underlying
        // container.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_APPROPRIATE_PUSH_BACK;
        d_vector.push_back(value);
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last)
            // Insert at the specified 'position' in the underlying container
            // the values in the range starting at the specified 'first' and
            // ending immediately before the specified 'last' iterators of the
            // (template parameter) type 'INPUT_ITER', and return an iterator
            // to the first newly inserted element.
    {
        return d_vector.insert(position, first, last);
    }

    // ACCESSORS
    iterator begin()
        // Return an iterator pointing the first element in the underlying
        // container (or the past-the-end iterator if the underlying container
        // is empty).
    {
        return d_vector.begin();
    }

    iterator end()
        // Return the past-the-end iterator for the underlying container.
    {
        return d_vector.end();
    }

    const_reference front() const
        // Return a reference providing modifiable access to the first element
        // of the underlying container.  The behavior is undefined if
        // the underlying container is empty.
    {
        return d_vector.front();
    }

    size_type size() const
        // Return the number of elements in the underlying container.
    {
        return d_vector.size();
    }

    bool empty() const
        // Return 'true' if the underlying container has size 0, and 'false'
        // otherwise.
    {
        return d_vector.empty();
    }
};

                       // ======================
                       // class MovableContainer
                       // ======================

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class MovableContainer {
    // This class is a value-semantic class template, acting as a transparent
    // proxy for the underlying 'bsl::vector' container, that holds elements of
    // the (template parameter) 'VALUE', and recording in the global variable
    // 'g_calledMethodFlag' methods being invoked.  The information recorded
    // is used to verify that 'bsl::priority_queue' invokes expected container
    // methods.

  private:
    // DATA
    bsl::vector<VALUE> d_vector;  // container for it's behavior simulation

  public:
    // PUBLIC TYPES
    typedef ALLOCATOR    allocator_type;
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;
    typedef VALUE*       iterator;
    typedef const VALUE* const_iterator;

    // CREATORS
    MovableContainer()
    : d_vector()
        // Create an empty object.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_CONSTRUCTOR;
    }

    MovableContainer(const ALLOCATOR& basicAllocator)
    : d_vector( basicAllocator)
        // Create an empty object that uses the specified 'basicAllocator' to
        // supply memory.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_CONSTRUCTOR;
    }

    MovableContainer(const MovableContainer& original)
        // Create a vector that has the same value as the specified 'original'
        // object.  Method invocation is recorded.
    : d_vector(original.d_vector)
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_CONSTRUCTOR;
    }

    MovableContainer(bslmf::MovableRef<MovableContainer> original)
        // Create an object that has the same value as the specified 'original'
        // object.  Method invocation is recorded.
    : d_vector(MoveUtil::move(MoveUtil::access(original).d_vector))
    {
        g_calledMethodFlag |= e_APPROPRIATE_CONSTRUCTOR;
    }

    MovableContainer(const MovableContainer& original,
                  const ALLOCATOR&     basicAllocator)
        // Create an object that has the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Method invocation is recorded.
    : d_vector(original.d_vector, basicAllocator)
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_CTOR_WITH_ALLOCATOR;
    }

    MovableContainer(bslmf::MovableRef<MovableContainer> original,
                  const ALLOCATOR&                 basicAllocator)
        // Create an object that has the same value as the specified 'original'
        // object that uses the specified 'basicAllocator' to supply memory.
        // Method invocation is recorded.
    : d_vector(MoveUtil::move(MoveUtil::access(original).d_vector),
               basicAllocator)
    {
        g_calledMethodFlag |= e_APPROPRIATE_CTOR_WITH_ALLOCATOR;
    }

    // MANIPULATORS
    MovableContainer& operator=(const MovableContainer& rhs)
        // Assign to this object the value of the specified 'other' object and
        // return a reference providing modifiable access to this object.
        // Method invocation is recorded.
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_ASSIGNMENT_OPERATOR;
        d_vector = rhs.d_vector;
        return *this;
    }

    MovableContainer& operator=(bslmf::MovableRef<MovableContainer> rhs)
        // Assign to this object the value of the specified 'other' object and
        // return a reference providing modifiable access to this object.
        // Method invocation is recorded.
    {
        g_calledMethodFlag |= e_APPROPRIATE_ASSIGNMENT_OPERATOR;
        d_vector = MoveUtil::move(MoveUtil::access(rhs).d_vector);
        return *this;
    }

    void pop_back()
        // Erase the last element from the underlying container.
    {
        d_vector.pop_back();
    }

    void push_back(const value_type& value)
        // Append a copy of the specified 'value' to the end of the underlying
        // container.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_INAPPROPRIATE_PUSH_BACK;
        d_vector.push_back(value);
    }

    void push_back(bslmf::MovableRef<value_type> value)
        // Append a copy of the specified 'value' to the end of the underlying
        // container.  Method invocation is recorded.
    {
        g_calledMethodFlag |= e_APPROPRIATE_PUSH_BACK;
        d_vector.push_back(MoveUtil::move(value));
    }

    template <class INPUT_ITER>
    iterator insert(const_iterator position,
                    INPUT_ITER     first,
                    INPUT_ITER     last)
            // Insert at the specified 'position' in the underlying container
            // the values in the range starting at the specified 'first' and
            // ending immediately before the specified 'last' iterators of the
            // (template parameter) type 'INPUT_ITER', and return an iterator
            // to the first newly inserted element.
    {
        return d_vector.insert(position, first, last);
    }

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    template <class... Args> void emplace_back(Args&&... arguments)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the specified 'arguments'.  Note that this
        // method is written only for testing purposes, it DOESN'T simulate
        // standard vector behavior and requires that the (template parameter)
        // type 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         int argumentsNumber = sizeof...(arguments);
         g_calledMethodFlag |= static_cast<CalledMethod>(
                     static_cast<int>(e_EMPLACE) << argumentsNumber);
         d_vector.push_back(value_type(1));
    }
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
    inline
    void emplace_back()
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter.  Note that this method is written only for testing
        // purposes, it DOESN'T simulate standard vector behavior and requires
        // that the (template parameter) type 'VALUE_TYPE' has constructor,
        // accepting integer value as a parameter.  Method invocation is
        // recorded.
    {
        g_calledMethodFlag |= e_EMPLACE;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed argument.  Note that this method is
        // written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;

        g_calledMethodFlag |= e_EMPLACE_1;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;

        g_calledMethodFlag |= e_EMPLACE_2;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;

        g_calledMethodFlag |= e_EMPLACE_3;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;

        g_calledMethodFlag |= e_EMPLACE_4;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;
        (void)args_05;

        g_calledMethodFlag |= e_EMPLACE_5;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;
        (void)args_05;
        (void)args_06;

        g_calledMethodFlag |= e_EMPLACE_6;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;
        (void)args_05;
        (void)args_06;
        (void)args_07;

        g_calledMethodFlag |= e_EMPLACE_7;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;
        (void)args_05;
        (void)args_06;
        (void)args_07;
        (void)args_08;

        g_calledMethodFlag |= e_EMPLACE_8;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;
        (void)args_05;
        (void)args_06;
        (void)args_07;
        (void)args_08;
        (void)args_09;

        g_calledMethodFlag |= e_EMPLACE_9;
        d_vector.push_back(value_type(1));
    }

    template <class Args_01,
              class Args_02,
              class Args_03,
              class Args_04,
              class Args_05,
              class Args_06,
              class Args_07,
              class Args_08,
              class Args_09,
              class Args_10>
    inline
    void emplace_back(BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) args_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) args_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) args_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) args_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) args_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) args_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) args_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) args_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) args_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) args_10)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the passed arguments.  Note that this method
        // is written only for testing purposes, it DOESN'T simulate standard
        // vector behavior and requires that the (template parameter) type
        // 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
         // Compiler warnings suppression.

        (void)args_01;
        (void)args_02;
        (void)args_03;
        (void)args_04;
        (void)args_05;
        (void)args_06;
        (void)args_07;
        (void)args_08;
        (void)args_09;
        (void)args_10;

        g_calledMethodFlag |= e_EMPLACE_10;
        d_vector.push_back(value_type(1));
    }
#else
    template <class... Args> void emplace_back(
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
        // Append to the end of the underlying container newly created
        // 'value_type' object, constructed with integer literal as a
        // parameter, despite of the specified 'arguments'.  Note that this
        // method is written only for testing purposes, it DOESN'T simulate
        // standard vector behavior and requires that the (template parameter)
        // type 'VALUE_TYPE' has constructor, accepting integer value as a
        // parameter.  Method invocation is recorded.
    {
        int argumentsNumber = sizeof...(arguments);
        g_calledMethodFlag |= static_cast<CalledMethod>(
                     static_cast<int>(e_EMPLACE) << argumentsNumber);
        d_vector.push_back(value_type(1));
    }
#endif

    // ACCESSORS

    iterator begin()
        // Return an iterator providing modifiable access to the first element
        // of the underlying container (or the past-the-end iterator if the
        // underlying container is empty).
    {
        return d_vector.begin(); }

    iterator end()
        // Return the past-the-end iterator for this modifiable vector.
    {
        return d_vector.end();
    }

    const_reference front() const
        // Return a reference providing non-modifiable access to the first
        // element of the underlying container.  The behavior is undefined if
        // the underlying container is empty.
    {
        return d_vector.front();
    }

    size_type size() const
        // Return the number of elements in the underlying container.
    {
        return d_vector.size();
    }

    bool empty() const
        // Return 'true' if the underlying container has size 0, and 'false'
        // otherwise.
    {
        return d_vector.empty();
    }
};

template <class T>
struct SpecialContainerTrait
    // A class should declare this trait if it registers it's methods
    // invocation in 'g_calledMethodFlag' global variable.
{
    static const bool is_special_container = false;
};

template <class T>
struct SpecialContainerTrait<NonMovableContainer<T> >
{
    static const bool is_special_container = true;
};

template <class T>
struct SpecialContainerTrait<MovableContainer<T> >
{
    static const bool is_special_container = true;
};

void setupCalledMethodCheck()
    // Reset 'g_calledMethodFlag' global variable's value.
{
    g_calledMethodFlag = e_NONE;
}

template <class CONTAINER>
bool isCalledMethodCheckPassed(CalledMethod flag)
    // Return 'true' if global variable 'g_calledMethodFlag' has the same value
    // as the specified 'flag', and 'false' otherwise.  Note that this check is
    // performed only for special containers, defined above.  Function always
    // returns 'true' for all other classes.
{
    if (SpecialContainerTrait<CONTAINER>::is_special_container) {
        return (flag == g_calledMethodFlag);
    }
    return true;
}

                       // ====================
                       // class TestComparator
                       // ====================

template <class TYPE>
class TestComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the (template parameter) type
    // 'TYPE'.  The function-call operator is implemented with integer
    // comparison using integers converted from objects of 'TYPE' by the class
    // method 'TemplateTestFacility::getIdentifier'.  The function-call
    // operator also increments a global counter used to keep track the method
    // call count.  Object of this class can be identified by an id passed on
    // construction.

    // DATA
    int         d_id;           // identifier for the functor
    bool        d_compareLess;  // indicate whether this object use '<' or '>'
    mutable int d_count;        // number of times 'operator()' is called

  public:
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
        // called.  Return 'true' if the integer representation of the
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

    bool operator==(const TestComparator& rhs) const
    {
        return (id() == rhs.id() && d_compareLess == rhs.d_compareLess);
    }

    int id() const
        // Return the 'id' of this object.
    {
        return d_id;
    }

    bool compareLess() const
        // Return the flag, indicating whether this object use '<' or '>'.
    {
        return d_compareLess;
    }

    size_t count() const
        // Return the number of times 'operator()' is called.
    {
        return d_count;
    }
};

template <class TYPE>
class GreaterThanFunctor {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the (template parameter) type
    // 'TYPE'.  The function-call operator is implemented with integer
    // comparison using integers converted from objects of 'TYPE' by the class
    // method 'TemplateTestFacility::getIdentifier'.

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

// FREE FUNCTIONS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getIdentifier(lhs)
         < bsltf::TemplateTestFacility::getIdentifier(rhs);
}

                            // =======================
                            // class NothrowSwapVector
                            // =======================

template <class VALUE>
class NothrowSwapVector : public bsl::vector<VALUE, bsl::allocator<VALUE> > {
    // 'vector' with non-throwing 'swap'

    // TYPES
    typedef bsl::vector<VALUE, bsl::allocator<VALUE> > base;
        // Base class alias.
  public:
    // MANIPULATORS
    void swap(NothrowSwapVector& other) BSLS_KEYWORD_NOEXCEPT
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        base::swap(other);
    }

    // FREE FUNCTIONS
    friend void swap(NothrowSwapVector& a,
                     NothrowSwapVector& b) BSLS_KEYWORD_NOEXCEPT
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        a.swap(b);
    }
};
                            // =============================
                            // struct ThrowingSwapComparator
                            // =============================

template <class TYPE>
struct ThrowingSwapComparator {
    // Comparator with throwing 'swap'

    // MANIPULATORS
    void swap(
      ThrowingSwapComparator& other) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the value of this object with that of the specified 'other'
        // object.
    {
        (void) other;
    }

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Return 'true' if the integer representation of the specified 'lhs'
        // is less than integer representation of the specified 'rhs'.
    {
        return lhs < rhs;
    }

    // FREE FUNCTIONS
    friend void swap(
          ThrowingSwapComparator& a,
          ThrowingSwapComparator& b) BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
        // Exchange the values of the specified 'a' and 'b' objects.
    {
        (void) a;
        (void) b;
    }
};

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

//=============================================================================
//                            TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                            // ================
                            // class TestDriver
                            // ================

template <class VALUE,
          class CONTAINER  = vector<VALUE>,
          class COMPARATOR = TestComparator<VALUE> >
class TestDriver {
    // Test driver class for 'priority_queue'.

  private:
    // TYPES
    typedef bsl::priority_queue<VALUE, CONTAINER, COMPARATOR>  Obj;
        // type under testing

    typedef typename Obj::value_compare   value_compare;
    typedef typename Obj::value_type      value_type;
    typedef typename Obj::reference       reference;
    typedef typename Obj::const_reference const_reference;
    typedef typename Obj::size_type       size_type;
    typedef typename Obj::container_type  container_type;
        // shorthands

    typedef bsltf::TestValuesArray<typename Obj::value_type> TestValues;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the 'priority_queue<VALUE, CONTAINER, COMPARATOR>'
    // object according to a custom language.  Uppercase letters [A..Z]
    // correspond to arbitrary (but unique) 'VALUE' object.
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
    // "A"          Push the 'VALUE' object corresponding to A.
    // "AA"         Push two 'VALUE' objects both corresponding to A.
    // "ABC"        Push three 'VALUE' objects corresponding to A, B and 'C'.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push' and 'pop' .
        // Optionally specify a zero 'verbose' to suppress 'spec' syntax error
        // messages.  Return the index of the first invalid character, and a
        // negative value otherwise.  Note that this function is used to
        // implement 'gg' as well as allow for verification of syntax error
        // detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    template <class VALUES>
    static size_t verify_object(Obj&          object,
                                const VALUES& expectedValues,
                                size_t        expectedSize);
        // Verify that the specified 'object' has the specified 'expectedSize'
        // and contains the same values as the array in the specified
        // 'expectedValues'.  Return 0 if 'object' has the expected values, and
        // a non-zero value otherwise.

    static bool use_same_allocator(Obj&                 object,
                                   int                  TYPE_ALLOC,
                                   bslma::TestAllocator *ta);
        // Return 'true' if the specified 'object' uses the specified 'ta'
        // allocator for supplying memory.  The specified 'TYPE_ALLOC'
        // identifies, if 'object' uses allocator at all.  Return 'false' if
        // object doesn't use 'ta'.

    static void populate_container(CONTAINER&        container,
                                   const char*       SPEC,
                                   size_t            length);
        // Add to the specified 'container' values, in accordance with the
        // specified 'SPEC' of the specified 'length'.

    static bool is_equal(Obj& a, Obj& b);
        // Return 'true' if the specified 'a' and 'b' have the same values in
        // the underlying container, placed in the same order.  Return 'false'
        // otherwise.

    static bool use_same_comparator(Obj& a, Obj& b);
        // Return 'true' if the specified 'a' and 'b' use the same comparator
        // for the ordering of values in the underlying container, and return
        // 'false' otherwise.

  public:
    // TEST CASES
    static void testCase22();
        // Test 'noexcept' specifications

    static void testCase19a();
        // Test 'emplace' forwarding parameters.

    static void testCase19();
        // Test 'emplace' member.

    template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase18_propagate_on_container_move_assignment_dispatch();
    static void testCase18_propagate_on_container_move_assignment();
        // Test 'propagate_on_container_move_assignment'.

    static void testCase18();
        // Test move-assignment operator.

    static void testCase17();
        // Test 'push' method that takes a movable ref.

    static void testCase16();
        // Test user-supplied constructors that take a movable ref.

    static void testCase15();
        // Test move constructors.

    //static void testCase14();
        // Reserved for free operators.

    static void testCase13();
        // Test 'empty' accessor.

    static void testCase12();
        // Test type traits.

    template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
              bool OTHER_FLAGS>
    static void testCase11_propagate_on_container_copy_assignment_dispatch();
    static void testCase11_propagate_on_container_copy_assignment();
        // Test 'propagate_on_container_copy_assignment'.

    static void testCase11();
        // Test copy-assignment operator ('operator=').

    // static void testCase10();
        // Reserved for BDEX.

    template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
              bool OTHER_FLAGS>
    static void testCase9_propagate_on_container_swap_dispatch();
    static void testCase9_propagate_on_container_swap();
        // Test 'propagate_on_container_swap'.

    static void testCase9();
        // Test 'swap' member.

    template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
              bool OTHER_FLAGS>
    static void testCase8_select_on_container_copy_construction_dispatch();
    static void testCase8_select_on_container_copy_construction();
        // Test 'select_on_container_copy_construction'.

    static void testCase8();
        // Test copy constructors.

    static void testCase7();
        // Test user-supplied constructors.

    // static void testCase6();
        // Reserved for equality operator ('operator==').

    // static void testCase5();
        // Reserved for (<<) operator.

    static void testCase4();
        // Test basic accessors ('size', 'front', and 'back').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators (default ctor, 'push', and 'pop')

    static void testCase1(const COMPARATOR&  comparator,
                          const VALUE       *testValues,
                          size_t             numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void testCase1_NoAlloc(const COMPARATOR&  comparator,
                                  const VALUE       *testValues,
                                  size_t             numValues);
        // Breathing test, except on a non-allocator container.  This test
        // *exercises* basic functionality but *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class VALUE, class CONTAINER, class COMPARATOR>
int TestDriver<VALUE, CONTAINER, COMPARATOR>::ggg(Obj        *object,
                                                  const char *spec,
                                                  int         verbose)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->push(VALUES[spec[i] - 'A']);
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

template <class VALUE, class CONTAINER, class COMPARATOR>
priority_queue<VALUE, CONTAINER, COMPARATOR>&
TestDriver<VALUE, CONTAINER, COMPARATOR>::gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class VALUES>
size_t TestDriver<VALUE, CONTAINER, COMPARATOR>::verify_object(
                                                  Obj&          object,
                                                  const VALUES& expectedValues,
                                                  size_t        expectedSize)
{
    ASSERTV(expectedSize, object.size(), expectedSize == object.size());

    if(expectedSize != object.size()) {
        return static_cast<size_t>(-1);                               // RETURN
    }

    for (size_t i = 0; i < expectedSize; ++i) {
        if (!(object.top() == expectedValues[i])) {
            return i + 1;                                             // RETURN
        }
        object.pop();
    }
    return 0;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
bool TestDriver<VALUE, CONTAINER, COMPARATOR>::use_same_allocator(
                                              Obj&                  object,
                                              int                   TYPE_ALLOC,
                                              bslma::TestAllocator *ta)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    if (0 == TYPE_ALLOC) {
        // If 'VALUE' does not use allocator, return true.

        return true;                                                  // RETURN
    }

    const bsls::Types::Int64 BB = ta->numBlocksTotal();
    const bsls::Types::Int64  B = ta->numBlocksInUse();

    object.push(VALUES[0]);

    const bsls::Types::Int64 AA = ta->numBlocksTotal();
    const bsls::Types::Int64  A = ta->numBlocksInUse();

    if (BB + TYPE_ALLOC <= AA && B + TYPE_ALLOC <= A) {
        return true;                                                  // RETURN
    }

    return false;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::populate_container(
                                                         CONTAINER&  container,
                                                         const char *SPEC,
                                                         size_t      length)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    for (size_t i = 0;i < length; ++i) {
        container.push_back(VALUES[SPEC[i] - 'A']);
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
bool TestDriver<VALUE, CONTAINER, COMPARATOR>::is_equal(Obj& a, Obj& b)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    if (a.size() != b.size()) {
        return false;                                                 // RETURN
    }
    while (!a.empty() && !b.empty()) {
        if (a.top() != b.top()) {
            return false;                                             // RETURN
        }
        a.pop();
        b.pop();
    }
    if (!a.empty() || !b.empty()) {
        return false;                                                 // RETURN
    }
    return true;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
bool TestDriver<VALUE, CONTAINER, COMPARATOR>::use_same_comparator(Obj& a,
                                                                   Obj& b)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
    const TestValues             VALUES;

    // Prepare objects for testing.

    while (!a.empty()) {
        a.pop();
    }

    while (!b.empty()) {
        b.pop();
    }

    // Add values to containers.

    a.push(VALUES[0]);
    a.push(VALUES[1]);
    a.push(VALUES[2]);
    b.push(VALUES[0]);
    b.push(VALUES[1]);
    b.push(VALUES[2]);

    // Compare order of values in containers.

    return is_equal(a, b);
}

                                // ----------
                                // TEST CASES
                                // ----------

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase22()
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
        P(bsls::NameOf<Obj>())
        P(bsls::NameOf<VALUE>())
        P(bsls::NameOf<CONTAINER>())
        P(bsls::NameOf<COMPARATOR>())
    }

    // N4594: page 902:  23.6.5: Class template 'priority_queue'
    //                                                         [priority.queue]
    //

    // page 903
    //..
    //  void swap(priority_queue& q)
    //      noexcept(is_nothrow_swappable_v<Container> &&
    //               is_nothrow_swappable_v<Compare>)
    //  { using std::swap; swap(c, q.c); swap(comp, q.comp); }
    //..

    {
        Obj x;
        Obj q;

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
        const bool isNoexcept = bsl::is_nothrow_swappable<CONTAINER>::value &&
                                bsl::is_nothrow_swappable<COMPARATOR>::value;
        ASSERT(isNoexcept == BSLS_KEYWORD_NOEXCEPT_OPERATOR(x.swap(q)));
#endif
    }

    // page 903
    //..
    //  // no equality is provided
    //  template <class T, class Container, class Compare>
    //  void swap(priority_queue<T, Container, Compare>& x,
    //            priority_queue<T, Container, Compare>& y)
    //  noexcept(noexcept(x.swap(y)));
    //..

    {
        Obj x;
        Obj y;

        ASSERT(false == BSLS_KEYWORD_NOEXCEPT_OPERATOR(swap(x, y)));
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase19a()
{
    // ------------------------------------------------------------------------
    // TESTING FORWARDING OF ARGUMENTS WITH EMPLACE
    //
    // Concerns:
    //: 1 'emplace' correctly forwards arguments to the constructor of the
    //:   value type, up to 10 arguments, the max number of arguments provided
    //:   for C++03 compatibility.  Note that only the forwarding of arguments
    //:   is tested in this function; all other functionality is tested in
    //:   'testCase19'.  Also note that this test case is run only for special
    //:   container class 'MovableContainer' having 'emplace' method(s) that
    //:   register the number of passed parameters in a global variable.
    //
    // Plan:
    //: 1 Execute an inner loop. On each iteration:
    //:
    //:   1 Create test object, using special container 'MovableContainer<int>'
    //:     as template parameter 'CONTAINER'.
    //:
    //:   2 Call 'emplace' method with parameters number, identified by
    //:     'CONFIG'.
    //:
    //:   3 Verify that container's method 'emplace_back' received as many
    //:     parameters, as were passed to the object's 'emplace' method.
    //
    // Testing:
    //   void emplace(Args&&... args);
    // ------------------------------------------------------------------------

    {
        for (char cfg = 'a'; cfg <= 'k'; ++cfg) {
            const char CONFIG = cfg;  // how many params will be passed

            if (veryVerbose) { P(CONFIG); }

            Obj mX;

            CalledMethod calledMethod = e_NONE;

            setupCalledMethodCheck();

            switch (CONFIG) {
              case 'a': {
                  mX.emplace();
                  calledMethod = e_EMPLACE;
              } break;
              case 'b': {
                  mX.emplace(1);
                  calledMethod = e_EMPLACE_1;
              } break;
              case 'c': {
                  mX.emplace(1, 2);
                  calledMethod = e_EMPLACE_2;
              } break;
              case 'd': {
                  mX.emplace(1, 2, 3);
                  calledMethod = e_EMPLACE_3;
              } break;
              case 'e': {
                  mX.emplace(1, 2, 3, 4);
                  calledMethod = e_EMPLACE_4;
              } break;
              case 'f': {
                  mX.emplace(1, 2, 3, 4, 5);
                  calledMethod = e_EMPLACE_5;
              } break;
              case 'g': {
                  mX.emplace(1, 2, 3, 4, 5, 6);
                  calledMethod = e_EMPLACE_6;
              } break;
              case 'h': {
                  mX.emplace(1, 2, 3, 4, 5, 6, 7);
                  calledMethod = e_EMPLACE_7;
              } break;
              case 'i': {
                  mX.emplace(1, 2, 3, 4, 5, 6, 7, 8);
                  calledMethod = e_EMPLACE_8;
              } break;
              case 'j': {
                  mX.emplace(1, 2, 3, 4, 5, 6, 7, 8, 9);
                  calledMethod = e_EMPLACE_9;
              } break;
              case 'k': {
                  mX.emplace(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
                  calledMethod = e_EMPLACE_10;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad constructor config.");
                  return;                                         // RETURN
              } break;
            }
            ASSERTV(
               CONFIG,
               true == isCalledMethodCheckPassed<CONTAINER>(calledMethod));
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING EMPLACE
    //
    // Concerns:
    //: 1 A new element is added to the container and the order of the
    //:   elements in the container remains correct.
    //
    // Plan:
    //:  1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create test and control objects.
    //:
    //:   2 Call add single element to the container via 'emplace' method for
    //:     the  test object and via 'push_back' method for the control
    //:     object.
    //:
    //:   3 Use the helper function 'is_equal' to verify that:
    //:
    //:     1 The test object, 'mX', has the same elements in container as
    //:       the control one, 'mZ'.  (C-1)
    //:
    //:     2 The test object, 'mX', has the order of the elements in
    //:       container as the control one, 'mZ'.  (C-1)
    //
    // Testing:
    //  void emplace(Args&&... args);
    // ------------------------------------------------------------------------
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) { P(TYPE_ALLOC); }
    {
        TestValues VALUES;
        ASSERTV(NUM_DATA, VALUES.size(),
                NUM_DATA <= static_cast<int>(VALUES.size()));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC = DATA[ti].d_spec;

            if (veryVerbose) {
                T_ P(SPEC);
            }

            // Create control object Z.

            Obj        mZ;
            const Obj& Z = gg(&mZ, SPEC);

            if (veryVeryVerbose) {
                printf("\t\tControl Obj: "); P(Z);
            }

            // Create test object Y.

            Obj mY;  const Obj& Y = gg(&mY, SPEC);

            if (veryVeryVerbose) {
                printf("\t\tTest Obj: "); P(Y);
            }

            mZ.push(VALUES[ti]);

            mY.emplace(VALUES[ti]);

            // Verify the expected container state.

            ASSERTV(SPEC, is_equal(mY, mZ));
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <bool PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                   testCase18_propagate_on_container_move_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   VALUE,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_MOVE_ASSIGNMENT_FLAG>
                                                         StdAlloc;

    typedef bsl::vector<VALUE, StdAlloc>                 CObj;
    typedef bsl::priority_queue<VALUE, CObj, COMPARATOR> Obj;

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

        const CObj CI(IVALUES.begin(), IVALUES.end(), scratch);

        const Obj W(COMPARATOR(), CI, scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                Obj mY(COMPARATOR(), CI, mas);  const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                const CObj CJ(JVALUES.begin(), JVALUES.end(), scratch);

                Obj mX(COMPARATOR(), CJ, mat);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = MoveUtil::move(mY));

// TBD          ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

// TBD no 'get_allocator' in 'priority_queue'
#if 0
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());
#endif

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

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                            testCase18_propagate_on_container_move_assignment()
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

    testCase18_propagate_on_container_move_assignment_dispatch<false, false>();
    testCase18_propagate_on_container_move_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_move_assignment::value == true'\n");

    testCase18_propagate_on_container_move_assignment_dispatch<true, false>();
    testCase18_propagate_on_container_move_assignment_dispatch<true, true>();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase18()
{
    // ------------------------------------------------------------------------
    // MOVE-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.  To provide backward compatibility, copy-assignment should
    //   be performed in the absence of move-assignment operator.  We are going
    //   to use two special containers 'NonMovableContainer' and
    //   'MovableContainer', which register called methods, to verify it.
    //
    // Concerns:
    //: 1 Appropriate method of the underlying container (move assignment or
    //:   copy-assignment operator) is called.
    //:
    //: 2 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 3 The target object gets the same value as the source object has.
    //:
    //: 4 Any memory allocation is from the target object's allocator.
    //:
    //: 5 The signature and return type are standard.
    //:
    //: 6 The reference returned is to the target object (i.e., '*this').
    //:
    //: 7 The source object is left in a valid but unspecified state.
    //:
    //: 8 The allocator address held by the target and source objects are
    //:   unchanged.
    //:
    //: 9 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 3 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-2:
    //:
    //:   1 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-2. For each of the iterations:
    //:
    //:     1 Use the value constructor with 'sa' allocator and 'sc' comparator
    //:       to create dynamic source object 'mX' and control object 'mZ',
    //:       each having the value 'V'.
    //:
    //:     2 Use the value constructor with 'ta' allocator and 'tc' comparator
    //:       to create target object 'mY', having the value 'W'.
    //:
    //:     3 Create a 'bslma_TestAllocator' object, and install it as the
    //:       default allocator (note that a ubiquitous test allocator is
    //:       already installed as the global allocator).
    //:
    //:     4 Assign 'mY' from movable reference of 'mX'.
    //:
    //:     5 Verify that the appropriate method has been called.  Note that
    //:       this check is skipped for all classes except special containers
    //:       'NonMovableContainer' and 'MovableContainer'. (C-1)
    //:
    //:     6 Verify that no memory has been obtained from source or default
    //:       allocators.  (C-4)
    //:
    //:     7 Verify that the address of the return value is the same as that
    //:       of 'mY'.  (C-6)
    //:
    //:     8 Use the helper function 'is_equal' to verify that the target
    //:       object, 'mY', has the same value as that of 'mZ'.  (C-2..3)
    //:
    //:     9 Use the helper function 'use_same_comparator' to verify that the
    //:       target object, 'mY', has the same comparator as that of 'mZ', to
    //:       ensure that the new object's comparator is properly installed.
    //:       (C-3)
    //:
    //:    10 Use the helper function 'use_same_allocator' to verify that the
    //:       respective allocator addresses held by 'mX' and 'mY' are
    //:       unchanged.  (C-7..8)
    //:
    //:    11 Add some values to the source and target object separately.
    //:       Verify that they change independently. Destroy source object.
    //:       Verify that target object is unaffected.  (C-7, 9)
    //:
    //:    12 Let the target and control objects go out of scope and verify,
    //:       that all memory has been released.  (C-10)
    //
    // Testing:
    //   priority_queue& operator=(MovableRef<priority_queue> rhs);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int              NUM_DATA        = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
    TestValues             VALUES;

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    if (verbose) { P(TYPE_ALLOC); }
    {
        // Source object configuration.

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int            LINE1   = DATA[ti].d_line;
            const char *const    SPEC1   = DATA[ti].d_spec;
            bslma::TestAllocator sa("source", veryVeryVeryVerbose);
            COMPARATOR           sc;

            // Target object configuration.

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int            LINE2   = DATA[tj].d_line;
                const char *const    SPEC2   = DATA[tj].d_spec;
                bslma::TestAllocator ta("target", veryVeryVeryVerbose);
                COMPARATOR           tc( 1, !sc.compareLess());
                {
                    Obj        *pX = new Obj(sc, &sa); // source
                    Obj&        mX = gg(pX, SPEC1);
                    const Obj&  X = mX;
                    Obj         mZ(sc);                // control
                    const Obj&  Z = gg(&mZ, SPEC1);

                    if (veryVerbose) {
                        T_ P_(LINE1) P_(X) P(Z)
                    }

                    Obj        mY(tc, &ta);            // target
                    const Obj& Y  = gg(&mY,  SPEC2);

                    if (veryVerbose) {
                        T_ P_(LINE2) P(Y)
                    }

                    bslma::TestAllocatorMonitor  sam(&sa);
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    setupCalledMethodCheck();

                    Obj *mR = &(mY = MoveUtil::move(mX));

                    const CalledMethod flag =
                                             e_APPROPRIATE_ASSIGNMENT_OPERATOR;
                    ASSERTV(
                           LINE1,
                           LINE2,
                           true == isCalledMethodCheckPassed<CONTAINER>(flag));

                    // Verify that no new memory has been obtained from source
                    // allocator.

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    // Verify that no memory has been obtained from default
                    // allocator.

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());

                    // Verify that returned reference points to the target
                    // object.

                    ASSERTV(LINE1, LINE2, mR, &mY, mR == &mY);

                    // Verify correctness of the contents moving.

                    ASSERTV(LINE1, LINE2, Z, Y, is_equal(mZ, mY));

                    // Verify correctness of the comparator moving.

                    ASSERTV(LINE1, LINE2, use_same_comparator(mZ, mY));

                    // Verify that allocators of the underlying containers
                    // haven't been affected.

                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mY, TYPE_ALLOC, &ta));
                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mX, TYPE_ALLOC, &sa));

                    // Verify independence of the target object from the source
                    // one.

                    size_t sourceSize = X.size();
                    size_t targetSize = Y.size();

                    mX.push(VALUES[0]);

                    ASSERTV(LINE1, LINE2, sourceSize != X.size());
                    ASSERTV(LINE1, LINE2, targetSize == Y.size());

                    sourceSize = X.size();

                    mY.push(VALUES[0]);

                    ASSERTV(LINE1, LINE2, sourceSize == X.size());
                    ASSERTV(LINE1, LINE2, targetSize != Y.size());

                    targetSize = Y.size();

                    const VALUE top  = Y.top();

                    // Reclaim dynamically allocated source object.

                    delete pX;

                    ASSERTV(LINE1, LINE2, top        == Y.top());
                    ASSERTV(LINE1, LINE2, targetSize == Y.size());

                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, ta.numBlocksInUse(),
                        0 == ta.numBlocksInUse());
                ASSERTV(LINE1, LINE2, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase17()
{
    // ------------------------------------------------------------------------
    // TESTING 'PUSH' ON MOVABLE VALUES:
    //   To provide backward compatibility, 'push_back' with const reference as
    //   a parameter should be invoked in the absence of method with movable
    //   reference.  We are going to use two special containers
    //   'NonMovableContainer' and 'MovableContainer', which register called
    //   methods, to verify it.  As 'push' with movable parameter has the same
    //   effect for test object as 'push' with const reference parameter (which
    //   has been tested already), we will use the last one to verify behavior
    //   of the first one.
    //
    // Concern:
    //: 1 The 'push_back' method of the underlying container with appropriate
    //:   parameter (move or const reference) is called.
    //:
    //: 2 The 'push' method adds an additional element to the object.
    //:
    //: 3 The 'push' method places all values in the user-supplied container
    //:   in correct order.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Create test and control objects and populate them with with 'V'.
    //:
    //:   2 Push new element to control object by const reference and to tested
    //:     object by movable reference.
    //:
    //:   3 Verify that appropriate container method has been called.  Note
    //:     that this check is skipped for all classes except
    //:     'NonMovableContainer' and 'MovableContainer'.  (C-1)
    //:
    //:   4 Use the helper function 'is_equal' to verify that the test object
    //:     has the same value as the control one.  (C-2..3)
    //
    //
    // Testing:
    //  void push(MovableRef<value_type> value);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) { P(TYPE_ALLOC); }
    {
        TestValues VALUES;  // contains 52 distinct increasing values
        ASSERTV(NUM_DATA, VALUES.size(),
                NUM_DATA <= static_cast<int>(VALUES.size()));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;

            if (veryVerbose) {
                T_ P(SPEC);
            }

            // Create control object Z.

            Obj        mZ;
            const Obj& Z = gg(&mZ, SPEC);

            if (veryVeryVerbose) {
                printf("\t\tControl Obj: "); P(Z);
            }

            // Create test object Y.

            Obj mY;  const Obj& Y = gg(&mY, SPEC);
            if (veryVeryVerbose) {
                printf("\t\tTest Obj: "); P(Y);
            }
            value_type temp = VALUES[ti];

            mZ.push(temp);

            setupCalledMethodCheck();

            mY.push(MoveUtil::move(temp));

            ASSERTV(SPEC,
                    true == isCalledMethodCheckPassed<CONTAINER>(
                                                     e_APPROPRIATE_PUSH_BACK));

            // Verify the expected container state.

            ASSERTV(SPEC, is_equal(mY, mZ));
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase16()
{
    // ------------------------------------------------------------------------
    // VALUE CONSTRUCTORS WITH MOVABLE PARAMETERS:
    //   To provide backward compatibility, constructors with const reference
    //   as a parameter should be invoked in the absence of constructor with
    //   movable reference.  We are going to use two special containers
    //   'NonMovableContainer' and 'MovableContainer', which register called
    //   methods, to verify it.
    //
    // Concern:
    //: 1 The constructor of the underlying container with appropriate
    //:   parameter (move or const reference) is called.
    //:
    //: 2 The object is constructed with all values in the user-supplied
    //:   container in the correct order.
    //:
    //: 3 No additional memory is allocated by the target object.
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
    //: 8 Any memory allocation is from the user-supplied allocator if
    //:   supplied, and otherwise the default allocator.
    //:
    //: 9 Subsequent changes to or destruction of the source container have no
    //:   effect on the move-constructed underlying container and vice-versa.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //:
    //:11 QoI: Creating an object having the default-constructed value
    //:   allocates no memory.
    //:
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating four distinct objects, in turn, each
    //:     object having the same value, 'V', but configured differently
    //:     identified by 'CONFIG':
    //:     'a': passing a comparator, a container populated with 'V', but
    //:          without passing an allocator;
    //:
    //:     'b': passing the begin and end iterators of a container populated
    //:          with 'V', and an empty container, but without passing an
    //:          allocator;
    //:
    //:     'c': passing a comparator, a container populated with 'V', and an
    //:          explicit null allocator;
    //:
    //:     'd': passing a comparator, a container populated with 'V', and the
    //:          address of a test allocator distinct from the default
    //:          allocator.
    //:     Note that containers in all cases are passed as movable references.
    //:
    //:   2 For each of the four iterations in P-2.1:
    //:
    //:     1 Insert the test data to a specialized container, create a default
    //:       comparator, and obtain begin and end iterators of the populated
    //:       container.
    //:
    //:     2 Create three 'bslma_TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     3 Choose the value constructor depending on 'CONFIG' to dynamically
    //:       create an object using the container in P-2.2.1, with its object
    //:       allocator configured appropriately (see P-2.2.2); use a distinct
    //:       test allocator for the object's footprint.
    //:
    //:     4 Verify that correct constructor of the underlying container has
    //:       been called.  Note that this check is skipped for all classes
    //:       except 'NonMovableContainer' and 'MovableContainer'.  (C-1)
    //:
    //:     5 Use the appropriate test allocator to verify that no additional
    //:       memory is allocated by the target object.  (C-3)
    //:
    //:     5 Verify that all of the attributes of each object have their
    //:       expected values.  (C-2, 11)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure
    //:       that its object allocator is properly installed.  (C-4..8)
    //:
    //:     7 Add some values to the source container and target object
    //:       separately.  Verify that they change independently. Destroy
    //:       source container.  Verify that target object is unaffected.
    //:       (C-9)
    //:
    //:     8 Destroy target object and verify that all memory has been
    //:       released.  (C-10)
    //
    // Testing:
    //  explicit priority_queue(const COMPARATOR&, MovableRef<CONTAINER>);
    //  priority_queue(ITER, ITER, const COMPARATOR&, MovableRef<CONTAINER>);
    //  priority_queue(const COMPARATOR&, MovRef<CONTAINER>, const ALLOCATOR&);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int              NUM_DATA        = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
    TestValues             VALUES;

    if (verbose) { P(TYPE_ALLOC); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_results);
            const size_t      LENGTH  = strlen(DATA[ti].d_spec);

            if (veryVerbose) { T_ P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the constructor

                if (veryVerbose) { T_ T_ P(CONFIG) }

                // Initialize user-supplied data.  Some value constructors
                // take user-supplied container, some use user-supplied
                // iterators.
                bslma::TestAllocator ca("container", veryVeryVeryVerbose);

                CONTAINER *moveContainer = new CONTAINER(&ca);
                CONTAINER *insertContainer = new CONTAINER(&ca);
                populate_container(*moveContainer, SPEC, LENGTH);
                populate_container(*insertContainer, SPEC, LENGTH);

                typename CONTAINER::iterator BEGIN = insertContainer->begin();
                typename CONTAINER::iterator END   = insertContainer->end();

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                COMPARATOR comparator;

                // Create control objects.

                Obj mZ1(comparator);      // standard object
                gg(&mZ1, SPEC);

                Obj mZ2(comparator);      // double size object
                gg(&mZ2, SPEC);
                gg(&mZ2, SPEC);

                Obj                  *objPtr;
                Obj                  *controlObjPtr;
                bslma::TestAllocator *objAllocatorPtr;
                CalledMethod          flag = e_NONE;

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&da);

                setupCalledMethodCheck();

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(comparator,
                                            MoveUtil::move(*moveContainer));
                      objAllocatorPtr = &ca;
                      controlObjPtr = &mZ1;
                      flag = e_APPROPRIATE_CONSTRUCTOR;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(BEGIN,
                                            END,
                                            comparator,
                                            MoveUtil::move(*moveContainer));
                      objAllocatorPtr = &ca;
                      controlObjPtr = &mZ2;
                      flag = e_APPROPRIATE_CONSTRUCTOR;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(comparator,
                                            MoveUtil::move(*moveContainer),
                                            (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                      controlObjPtr = &mZ1;
                      flag = e_APPROPRIATE_CTOR_WITH_ALLOCATOR;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(comparator,
                                            MoveUtil::move(*moveContainer),
                                            &sa);
                      objAllocatorPtr = &sa;
                      controlObjPtr = &mZ1;
                      flag = e_APPROPRIATE_CTOR_WITH_ALLOCATOR;
                  } break;
                  default: {
                      ASSERTV(SPEC, CONFIG, !"Bad constructor config.");
                      return;                                         // RETURN
                  } break;
                }

                ASSERTV(SPEC,
                        CONFIG,
                        true == isCalledMethodCheckPassed<CONTAINER>(flag));

                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj&       mY = *objPtr;
                const Obj& Y = mY;
                Obj&       mZ  = *controlObjPtr;
                const Obj& Z = mZ;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(Y) }

                // Ensure the first row of the table contains the
                // default-constructed value.

                if (ti == 0) {
                    bslma::DefaultAllocatorGuard guard(
                                    &bslma::NewDeleteAllocator::singleton());
                    Obj objTemp;
                    ASSERTV(LINE, CONFIG, objTemp, Y,
                            is_equal(objTemp, mY));
                }

                // Verify the expected attributes values.

                ASSERTV(SPEC,
                        CONFIG,
                        Y,
                        Z,
                        is_equal(mY, mZ));

                // Verify any attribute allocators are installed properly.

                bslma::TestAllocator&  ta = *objAllocatorPtr;
                ASSERTV(LINE, CONFIG, use_same_allocator(mY, TYPE_ALLOC, &ta));

                // Verify independence of the target object from the source
                // container.

                size_t moveSourceSize = moveContainer->size();
                size_t insertSourceSize = insertContainer->size();
                size_t targetSize = Y.size();

                moveContainer->push_back(VALUES[0]);
                insertContainer->push_back(VALUES[0]);

                ASSERTV(SPEC, CONFIG,
                        moveSourceSize !=  moveContainer->size());
                ASSERTV(SPEC, CONFIG,
                        insertSourceSize !=  insertContainer->size());
                ASSERTV(SPEC, CONFIG, targetSize == Y.size());

                moveSourceSize =  moveContainer->size();
                insertSourceSize =  insertContainer->size();

                mY.push(VALUES[0]);

                ASSERTV(SPEC, CONFIG,
                        moveSourceSize == moveContainer->size());
                ASSERTV(SPEC, CONFIG,
                        insertSourceSize == insertContainer->size());
                ASSERTV(SPEC, CONFIG, targetSize != Y.size());

                {
                    // For some value types 'top' variable creation leads to
                    // memory allocation from default allocator.  To pass check
                    // at the end of outer block, we need to destroy 'top' and
                    // release memory. So it is created inside another block.

                    targetSize = Y.size();
                    const VALUE top  = Y.top();

                    // Reclaim dynamically allocated source objects.

                    delete moveContainer;
                    delete insertContainer;

                    ASSERTV(SPEC, CONFIG, top        == Y.top());
                    ASSERTV(SPEC, CONFIG, targetSize == Y.size());
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());

            }  // end foreach configuration
        }  // end foreach row
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase15()
{
    // ------------------------------------------------------------------------
    // MOVE CONSTRUCTOR:
    //   Ensure that we can construct any object of the class, having other
    //   object of the class as the source.  To provide backward compatibility,
    //   copy constructor should be used in the absence of move constructor.
    //   We are going to use two special containers 'NonMovableContainer' and
    //   'MovableContainer', which register called methods, to verify it.
    //
    // Concerns:
    //: 1 Appropriate constructor of the underlying container (move or copy) is
    //:   called.
    //:
    //: 2 The new object has the same value as the source object.
    //:
    //: 3 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 4 The source object is left in a valid but unspecified state.
    //:
    //: 5 No additional memory is allocated by the target object.
    //:
    //: 5 If an allocator is NOT supplied to the constructor, the
    //:   allocator of the source object in effect at the time of construction
    //:   becomes the object allocator for the resulting object.
    //:
    //: 6 If an allocator IS supplied to the constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 7 If a null allocator address IS supplied to the constructor, the
    //:   default allocator in effect at the time of construction becomes
    //:   the object allocator for the resulting object.
    //:
    //: 8 Supplying an allocator to the constructor has no effect on subsequent
    //:   object values.
    //:
    //: 9 Subsequent changes to or destruction of the source object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid source object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating three distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently
    //:     identified by 'CONFIG':
    //:     'a': passing a source object without passing an allocator;
    //:
    //:     'b': passing a source object and an explicit null allocator;
    //:
    //:     'c': passing a source object and the address of a test allocator
    //:          distinct from the default and source object's allocators.
    //:
    //:   2 For each of the four iterations in P-2.1:
    //:
    //:     1 Use the value constructor with 'sa' allocator to create dynamic
    //:       source object 'mX' and control object 'mZ', each having the value
    //:       'V'.
    //:
    //:     2 Create a 'bslma_TestAllocator' object, and install it as the
    //:       default allocator (note that a ubiquitous test allocator is
    //:       already installed as the global allocator).
    //:
    //:     3 Choose the move constructor depending on 'CONFIG' to dynamically
    //:       create an object, 'mY', using movable reference of 'mX'.
    //:
    //:     4 Verify that the appropriate constructor of the underlying
    //:       container has been called.  Note that this check is skipped for
    //:       all classes except special containers 'NonMovableContainer' and
    //:       'MovableContainer'.  (C-1)
    //:
    //:     5 Use the appropriate test allocator to verify that no additional
    //:       memory is allocated by the target object.  (C-5)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure
    //:       that its object allocator is properly installed.  (C-6..9)
    //:
    //:     7 Use the helper function 'use_same_comparator' to verify that the
    //:       target object, 'mY', has the same comparator as that of 'mZ', to
    //:       ensure that the new object's comparator is properly installed.
    //:       (C-2..3)
    //:
    //:     8 Add some values to the source and target object separately.
    //:       Verify that they change independently. Destroy source object.
    //:       Verify that target object is unaffected.  (C-4, 10)
    //:
    //:     9 Delete the target object and let the control object go out of
    //:       scope to verify that all memory has been released.  (C-11)
    //
    // Testing:
    //  priority_queue(MovableRef<priority_queue> original);
    //  priority_queue(MovableRef<priority_queue> original, const ALLOCATOR&);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose) { P(TYPE_ALLOC); }
    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;

            if (veryVerbose) {
                T_ P(SPEC);
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator sa("source", veryVeryVeryVerbose);

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we configure constructor

                if (veryVerbose) {
                    T_ T_ P(CONFIG);
                }

                // Create source and control objects.

                Obj        *pX = new Obj(&sa);  // source
                Obj&        mX = gg(pX, SPEC);
                const Obj&  X = mX;
                Obj         mZ;                 // control
                const Obj&  Z = gg(&mZ, SPEC);

                if (veryVeryVerbose) {
                    printf("\t\tSource Obj: "); P(X);
                    printf("\t\tControl Obj: "); P(Z);
                }

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                CalledMethod         calledMethod = e_NONE;

                bslma::TestAllocator  fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator  ta("target",    veryVeryVeryVerbose);

                setupCalledMethodCheck();

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&da);

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(MoveUtil::move(mX));
                      objAllocatorPtr = &sa;
                      calledMethod = e_APPROPRIATE_CONSTRUCTOR;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(MoveUtil::move(mX),
                                            (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                      calledMethod = e_APPROPRIATE_CTOR_WITH_ALLOCATOR;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(MoveUtil::move(mX), &ta);
                      objAllocatorPtr = &ta;
                      calledMethod = e_APPROPRIATE_CTOR_WITH_ALLOCATOR;
                  } break;
                  default: {
                      ASSERTV(SPEC, CONFIG, !"Bad constructor config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj& mY = *objPtr;  const Obj& Y = mY;

                ASSERTV(
                  SPEC,
                  true == isCalledMethodCheckPassed<CONTAINER>(calledMethod));

                ASSERTV(SPEC, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                // Verify correctness of the contents moving.

                ASSERTV(SPEC, CONFIG, Z, Y, is_equal(mZ, mY));

                // Verify any attribute allocators are installed properly.

                ASSERTV(SPEC, CONFIG, use_same_allocator(mY,
                                                         TYPE_ALLOC,
                                                         objAllocatorPtr));

                // Verify correctness of the comparator moving.

                ASSERTV(SPEC, CONFIG, use_same_comparator(mZ, mY));

                // Verify independence of the target object from the source
                // one.

                size_t sourceSize = X.size();
                size_t targetSize = Y.size();

                mX.push(VALUES[0]);

                ASSERTV(SPEC, CONFIG, sourceSize != X.size());
                ASSERTV(SPEC, CONFIG, targetSize == Y.size());

                sourceSize = X.size();

                mY.push(VALUES[0]);

                ASSERTV(SPEC, CONFIG, sourceSize == X.size());
                ASSERTV(SPEC, CONFIG, targetSize != Y.size());

                targetSize = Y.size();
                const VALUE top  = Y.top();

                // Reclaim dynamically allocated source object.

                delete pX;

                ASSERTV(SPEC, CONFIG, top       == Y.top());
                ASSERTV(SPEC, CONFIG, targetSize == Y.size());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(SPEC, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(SPEC, CONFIG, ta.numBlocksInUse(),
                        0 == ta.numBlocksInUse());
            }

            ASSERTV(SPEC, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(SPEC, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'empty'
    //
    // Concern:
    //: 1 'empty' returns 'true' only when the object is empty.
    //:
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:  (C-1)
    //:
    //:   1 Create an object and populate it with 'V'.  Invoke 'empty' to
    //:     verify it returns 'true' if 'V' is not empty.  (C-1)
    //:
    //:   2 Clear the object by popping out 'LENGTH' of elements, where
    //:     'LENGTH' is the length of 'V'.  Invoke 'empty' to verify it returns
    //:     'false'.  (C-1)
    //
    // Testing:
    //  bool empty() const;
    // ------------------------------------------------------------------------

    bslma::TestAllocator  oa(veryVeryVerbose);

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
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (verbose) printf("\tTesting 'empty', 'front' and 'back'.\n");
    {
        const TestValues VALUES;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            // Verify 'empty' returns correct values.

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());
            ASSERTV(LINE, SPEC, (0 == ti) == bsl::empty(X));

            for (size_t tj = 0; tj < strlen(SPEC); ++tj) {
                ASSERTV(LINE, SPEC, tj, false == X.empty());
                mX.pop();
            }

            ASSERTV(LINE, SPEC, true == X.empty());
            ASSERTV(LINE, SPEC, true == bsl::empty(X));
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exist.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits.
    // ------------------------------------------------------------------------

    // Verify 'priority_queue' defines the expected traits.

    enum { CONTAINER_USES_ALLOC =
                                 bslma::UsesBslmaAllocator<CONTAINER>::value };

    BSLMF_ASSERT(
         ((int)CONTAINER_USES_ALLOC == bslma::UsesBslmaAllocator<Obj>::value));

    // Verify 'priority_queue' does not define other common traits.

    BSLMF_ASSERT((0 == bslalg::HasStlIterators<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseMoveable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<Obj>::value));
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <bool PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                   testCase11_propagate_on_container_copy_assignment_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                   VALUE,
                                   OTHER_FLAGS,
                                   PROPAGATE_ON_CONTAINER_COPY_ASSIGNMENT_FLAG,
                                   OTHER_FLAGS,
                                   OTHER_FLAGS>          StdAlloc;

    typedef bsl::vector<VALUE, StdAlloc>                 CObj;
    typedef bsl::priority_queue<VALUE, CObj, COMPARATOR> Obj;

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

        const CObj CI(IVALUES.begin(), IVALUES.end(), scratch);

        const Obj W(COMPARATOR(), CI, scratch);  // control

        // Create target object.
        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            {
                Obj mY(COMPARATOR(), CI, mas);  const Obj& Y = mY;

                if (veryVerbose) { T_ P_(ISPEC) P_(Y) P(W) }

                const CObj CJ(JVALUES.begin(), JVALUES.end(), scratch);

                Obj mX(COMPARATOR(), CJ, mat);  const Obj& X = mX;

                bslma::TestAllocatorMonitor oasm(&oas);
                bslma::TestAllocatorMonitor oatm(&oat);

                Obj *mR = &(mX = Y);

// TBD          ASSERTV(ISPEC, JSPEC,  W,   X,  W == X);
// TBD          ASSERTV(ISPEC, JSPEC,  W,   Y,  W == Y);
                ASSERTV(ISPEC, JSPEC, mR, &mX, mR == &mX);

// TBD no 'get_allocator' in 'priority_queue'
#if 0
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                       !PROPAGATE == (mat == X.get_allocator()));
                ASSERTV(ISPEC, JSPEC, PROPAGATE,
                        PROPAGATE == (mas == X.get_allocator()));

                ASSERTV(ISPEC, JSPEC, mas == Y.get_allocator());
#endif

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

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                            testCase11_propagate_on_container_copy_assignment()
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

    testCase11_propagate_on_container_copy_assignment_dispatch<false, false>();
    testCase11_propagate_on_container_copy_assignment_dispatch<false, true>();

    if (verbose)
        printf("\n'propagate_on_container_copy_assignment::value == true'\n");

    testCase11_propagate_on_container_copy_assignment_dispatch<true, false>();
    testCase11_propagate_on_container_copy_assignment_dispatch<true, true>();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase11()
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
    //: 2 The target object gets the same value as the source object.
    //:
    //: 3 The allocator address held by the target object is unchanged.
    //:
    //: 4 Any memory allocation is from the target object's allocator.
    //:
    //: 5 The signature and return type are standard.
    //:
    //: 6 The reference returned is to the target object (i.e., '*this').
    //:
    //: 7 The value of the source object is not modified.
    //:
    //: 8 The allocator address held by the source object is unchanged.
    //:
    //: 9 Subsequent changes to or destruction of the source object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-5)
    //:
    //: 2 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 3 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-2:
    //:
    //:   1 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-2. For each of the iterations:
    //:
    //:     1 Use the value constructor with a "sa" allocator and 'sc'
    //:       comparator to create dynamic source object, 'mX', and control
    //:       objects 'mZ1' and 'mZ2', each having the value 'V'.
    //:
    //:     2 Create a 'bslma_TestAllocator' object, 'ta' and comparator 'tc',
    //:       having another compare function, than source comparator.
    //:
    //:     3 Use the value constructor with a "ta" allocator and 'tc'
    //:       comparator to create target object, 'mY', having the value 'W'.
    //:
    //:     4 Create a 'bslma_TestAllocator' object, and install it as the
    //:       default allocator (note that a ubiquitous test allocator is
    //:       already installed as the global allocator).
    //:
    //:     5 Assign 'mY' from constant reference of 'mX': 'X'.
    //:
    //:     6 Verify that no memory has been obtained from source or default
    //:       allocators. (C-4)
    //:
    //:     7 Verify that the address of the return value is the same as that
    //:       of 'mY'.  (C-5..6)
    //:
    //:     8 Use the helper function 'is_equal' to verify that:
    //:
    //:       1 The target object, 'mY', has the same value as that of control
    //:         object 'mZ1'.  (C-1..2)
    //:
    //:       2 'mX' still has the same value as that of 'mZ2'.  (C-7)
    //:
    //:     9 Use the helper function 'use_same_comparator' to verify that the
    //:       target object, 'mY', has the same comparator as that of control
    //:       object 'mZ1'.  (C-2)
    //:
    //:    10 Use the helper function 'use_same_allocator' to verify that the
    //:       respective allocator addresses held by 'mY' and 'mX' are
    //:       unchanged.  (C-3, 8)
    //:
    //:    12 Add some values to the source and target object separately.
    //:       Verify that they change independently. Destroy source object.
    //:       Verify that target object is unaffected.  (C-9)
    //:
    //:    13 Use the appropriate test allocator to verify that all object
    //        memory is released when the object is destroyed.  (C-10)
    //
    // Testing:
    //   priority_queue& operator=(const priority_queue& rhs);
    // ------------------------------------------------------------------------


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int              NUM_DATA        = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
    TestValues             VALUES;

    Obj& (Obj::*operatorMAg) (const Obj&) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    if (verbose) { P(TYPE_ALLOC); }
    {

        for (int ti = 0; ti < NUM_DATA; ++ti) {
        // Source and control objects configuration.

            const int            LINE1   = DATA[ti].d_line;
            const char *const    SPEC1   = DATA[ti].d_spec;
            bslma::TestAllocator sa("source", veryVeryVeryVerbose);
            COMPARATOR           sc;
            if (veryVerbose) {
                T_ P(SPEC1)
            }
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                // Target object configuration.

                const int            LINE2   = DATA[tj].d_line;
                const char *const    SPEC2   = DATA[tj].d_spec;
                bslma::TestAllocator ta("target", veryVeryVeryVerbose);
                COMPARATOR           tc(1, !sc.compareLess());

                if (veryVerbose) {
                    T_ T_ P(SPEC2)
                }

                {
                    Obj        *pX = new Obj(sc, &sa);
                    Obj&        mX = gg(pX, SPEC1);
                    const Obj&  X = mX;
                    Obj         mZ1(sc, &sa);
                    const Obj&  Z1 = gg(&mZ1, SPEC1);
                    Obj         mZ2(sc, &sa);
                    const Obj&  Z2 = gg(&mZ2, SPEC1);

                    if (veryVeryVerbose) { T_ T_ P_(LINE1) P_(X) P_(Z1) P(Z2) }

                    Obj        mY(tc, &ta);
                    const Obj& Y  = gg(&mY,  SPEC2);

                    if (veryVeryVerbose) { T_ T_ P_(LINE2) P(Y) }

                    bslma::TestAllocatorMonitor  sam(&sa);
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *mR = &(mY = X);

                    // Verify that no new memory has been obtained from source
                    // allocator.

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    // Verify that no memory has been obtained from default
                    // allocator.

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());

                    // Verify that returned reference points to the target
                    // object.

                    ASSERTV(LINE1, LINE2, mR, &mY, mR == &mY);

                    // Verify correctness of the contents coping.

                    ASSERTV(LINE1, LINE2, Z1, Y, is_equal(mZ1, mY));

                    // Verify that source object is unaffected.
                    ASSERTV(LINE1, LINE2, Z2,  X, is_equal(mZ2, mX));

                    // Verify correctness of the comparator coping.

                    ASSERTV(LINE1, LINE2, use_same_comparator(mZ1, mY));

                    // Verify that allocators of the underlying containers
                    // haven't been affected.

                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mY, TYPE_ALLOC, &ta));
                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mX, TYPE_ALLOC, &sa));

                    // Verify independence of the target object from the source
                    // one.

                    size_t sourceSize = X.size();
                    size_t targetSize = Y.size();

                    mX.push(VALUES[0]);

                    ASSERTV(LINE1, LINE2, sourceSize != X.size());
                    ASSERTV(LINE1, LINE2, targetSize == Y.size());

                    sourceSize = X.size();

                    mY.push(VALUES[0]);

                    ASSERTV(LINE1, LINE2, sourceSize == X.size());
                    ASSERTV(LINE1, LINE2, targetSize != Y.size());

                    targetSize       = Y.size();
                    const VALUE top  = Y.top();

                    // Reclaim dynamically allocated source object.

                    delete pX;

                    ASSERTV(LINE1, LINE2, top        == Y.top());
                    ASSERTV(LINE1, LINE2, targetSize == Y.size());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, ta.numBlocksInUse(),
                             0 == ta.numBlocksInUse());
            }
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <bool PROPAGATE_ON_CONTAINER_SWAP_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                               testCase9_propagate_on_container_swap_dispatch()
{
    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<VALUE,
                                        OTHER_FLAGS,
                                        OTHER_FLAGS,
                                        PROPAGATE_ON_CONTAINER_SWAP_FLAG,
                                        OTHER_FLAGS>     StdAlloc;

    typedef bsl::vector<VALUE, StdAlloc>                 CObj;
    typedef bsl::priority_queue<VALUE, CObj, COMPARATOR> Obj;

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

        StdAlloc xma(&xoa);
        StdAlloc yma(&yoa);

        StdAlloc scratch(&da);

        const CObj CI(IVALUES.begin(), IVALUES.end(), scratch);

        const Obj ZZ(COMPARATOR(), CI, scratch);  // control

        for (int tj = 0; tj < NUM_SPECS; ++tj) {
            const char *const JSPEC   = SPECS[tj];

            TestValues JVALUES(JSPEC);

            const CObj CJ(JVALUES.begin(), JVALUES.end(), scratch);

            const Obj WW(COMPARATOR(), CJ, scratch);  // control

            {
                Obj mX(COMPARATOR(), CI, xma);  const Obj& X = mX;

                if (veryVerbose) { T_ P_(ISPEC) P_(X) P(ZZ) }

                Obj mY(COMPARATOR(), CJ, yma);  const Obj& Y = mY;

// TBD          ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
// TBD          ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    mX.swap(mY);

// TBD              ASSERTV(ISPEC, JSPEC, WW, X, WW == X);
// TBD              ASSERTV(ISPEC, JSPEC, ZZ, Y, ZZ == Y);

                    if (PROPAGATE) {
// TBD no 'get_allocator' in 'priority_queue'
#if 0
                        ASSERTV(ISPEC, JSPEC, yma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, xma == Y.get_allocator());
#endif

                        ASSERTV(ISPEC, JSPEC, dam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, xoam.isTotalSame());
                        ASSERTV(ISPEC, JSPEC, yoam.isTotalSame());
                    }
// TBD no 'get_allocator' in 'priority_queue'
#if 0
                    else {
                        ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                        ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
                    }
#endif
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor xoam(&xoa);
                    bslma::TestAllocatorMonitor yoam(&yoa);

                    swap(mX, mY);

// TBD              ASSERTV(ISPEC, JSPEC, ZZ, X, ZZ == X);
// TBD              ASSERTV(ISPEC, JSPEC, WW, Y, WW == Y);

// TBD no 'get_allocator' in 'priority_queue'
#if 0
                    ASSERTV(ISPEC, JSPEC, xma == X.get_allocator());
                    ASSERTV(ISPEC, JSPEC, yma == Y.get_allocator());
#endif

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

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                                        testCase9_propagate_on_container_swap()
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

    testCase9_propagate_on_container_swap_dispatch<false, false>();
    testCase9_propagate_on_container_swap_dispatch<false, true>();

    if (verbose) printf("\n'propagate_on_container_swap::value == true'\n");

    testCase9_propagate_on_container_swap_dispatch<true, false>();
    testCase9_propagate_on_container_swap_dispatch<true, true>();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase9()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the values of the (two) supplied objects.
    //:
    //: 2 The common object allocator address held by both objects is
    //:   unchanged.
    //:
    //: 3 If the two objects being swapped use the same allocators, neither
    //:   function allocates memory from any allocator.
    //:
    //: 4 Both functions have standard signatures and return types.
    //:
    //: 5 Two objects with different allocators may be swapped.  In which case,
    //:   memory may be allocated.
    //:
    //: 6 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 7 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //:
    //: 8 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-4)
    //:
    //: 2 Create a 'bslma_TestAllocator' object, and install it as the
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
    //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
    //:
    //:   1 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW1', having the value described by 'R1'. also use the
    //:     copy constructor and a "scratch" allocator to create a 'Obj' 'mW2'
    //:     from 'mW1'.  Use the member and free 'swap' functions to swap
    //:     the value of 'mW2' with itself; verify, after each swap:  (C-6)
    //:
    //:     1 The value is unchanged.  (C-6)
    //:
    //:     2 There was no additional object memory allocation.
    //:
    //:   3 For each row 'R2' in the table of P-3:  (C-1..2)
    //:
    //:     1 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mW1', having the value described by 'R1'.  Use the copy
    //:       constructor and 'oa' to create a modifiable 'Obj', 'mX', from
    //:       'mW1'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mW2', having the value described by 'R2'.  Use the copy
    //:       constructor and 'oa' to create a modifiable 'Obj', 'mY', from
    //:       'mW2'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 There was no additional object memory allocation.
    //:
    //:       3 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-2)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-7)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma_TestAllocator' object, 'oa'.
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
    //:     in this component, then verify that:  (C-7)
    //:
    //:     1 The values have been exchanged.
    //:
    //:     2 There was no additional object memory allocation.  (C-7)
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //:
    //: 7 Verify that, in appropriate build modes, defensive checks are
    //:   triggered when an attempt is made to swap objects that do not
    //:   refer to the same allocator, but not when the allocators are the
    //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
    //
    // Testing:
    //   void swap(queue& other);
    //   void swap(queue<V, C>& a, queue<V, C>& b);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

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

    if (verbose) printf("\nUse a table of distinct object values.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        // Ensure the first row of the table contains the
        // default-constructed value.

        static bool firstFlag = true;
        if (firstFlag) {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
            Obj mX;       const Obj& X = mX;
            ASSERTV(LINE1, X, W, is_equal(mX, mW));
            firstFlag = false;
        }

        // member 'swap'
        {
            Obj mW1(&oa);           const Obj& W1 = gg(&mW1,  SPEC1);
            Obj mW2(mW1, &scratch); const Obj& W2 = mW2;

            bslma::TestAllocatorMonitor oam(&oa);

            mW1.swap(mW1);

            ASSERTV(LINE1, oam.isTotalSame());
            ASSERTV(LINE1, W2, W1, is_equal(mW2, mW1));
            ASSERTV(LINE1, use_same_allocator(mW1, TYPE_ALLOC, &oa));
        }

        // free function 'swap'
        {
            Obj mW1(&oa);           const Obj& W1 = gg(&mW1,  SPEC1);
            Obj mW2(mW1, &scratch); const Obj& W2 = mW2;

            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW1, mW1);

            ASSERTV(LINE1, oam.isTotalSame());
            ASSERTV(LINE1, W2, W1, is_equal(mW2, mW1));
            ASSERTV(LINE1, use_same_allocator(mW1, TYPE_ALLOC, &oa));
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            // member 'swap'
            {
                Obj mW1(&oa);           const Obj& W1 = gg(&mW1,  SPEC1);
                Obj mX(W1, &oa);        const Obj& X = mX;

                Obj mW2(&oa);           const Obj& W2 = gg(&mW2,  SPEC2);
                Obj mY(W2, &oa);        const Obj& Y = mY;

                if (veryVerbose) { T_ P_(LINE2) P_(W1) P_(W2) P(X) P(Y) }

                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, oam.isTotalSame());

                ASSERTV(LINE1, LINE2, X, W2, is_equal(mX, mW2));
                ASSERTV(LINE1, LINE2, Y, W1, is_equal(mY, mW1));
                ASSERTV(LINE1, use_same_allocator(mX, TYPE_ALLOC, &oa));
                ASSERTV(LINE1, use_same_allocator(mY, TYPE_ALLOC, &oa));
            }

            // free function 'swap'
            {
                Obj mW1(&oa);           const Obj& W1 = gg(&mW1,  SPEC1);
                Obj mX(W1, &oa);        const Obj& X = mX;

                Obj mW2(&oa);           const Obj& W2 = gg(&mW2,  SPEC2);
                Obj mY(W2, &oa);        const Obj& Y = mY;

                if (veryVerbose) { T_ P_(LINE2) P_(W1) P_(W2) P(X) P(Y) }

                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, oam.isTotalSame());

                ASSERTV(LINE1, LINE2, X, W2, is_equal(mX, mW2));
                ASSERTV(LINE1, LINE2, Y, W1, is_equal(mY, mW1));
                ASSERTV(LINE1, use_same_allocator(mX, TYPE_ALLOC, &oa));
                ASSERTV(LINE1, use_same_allocator(mY, TYPE_ALLOC, &oa));
            }
        }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW1(&oa);         const Obj& W1 = mW1;
        Obj mX(W1, &scratch); const Obj& X  = mX;

        Obj mW2(&oa);         const Obj& W2 = gg(&mW2, "ABC");
        Obj mY(W2, &scratch); const Obj& Y  = mY;

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }

        ASSERTV(W2, X, is_equal(mW2, mX));
        ASSERTV(W1, Y, is_equal(mW1, mY));
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <bool SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
          bool OTHER_FLAGS>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                     testCase8_select_on_container_copy_construction_dispatch()
{
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value +
                           bslma::UsesBslmaAllocator<VALUE>::value;

    // Set the three properties of 'bsltf::StdStatefulAllocator' that are not
    // under test in this test case to 'false'.

    typedef bsltf::StdStatefulAllocator<
                                    VALUE,
                                    SELECT_ON_CONTAINER_COPY_CONSTRUCTION_FLAG,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS,
                                    OTHER_FLAGS>         StdAlloc;

    typedef bsl::vector<VALUE, StdAlloc>                 CObj;
    typedef bsl::priority_queue<VALUE, CObj, COMPARATOR> Obj;

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
        StdAlloc scratch(&da);

        {
            const CObj C(VALUES.begin(), VALUES.end(), scratch);

            const Obj W(COMPARATOR(), C, ma);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(COMPARATOR(), C, ma);  const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor oam(&oa);

            const Obj Y(X);

// TBD      ASSERTV(SPEC, W == Y);
// TBD      ASSERTV(SPEC, W == X);
// TBD no 'get_allocator' in 'priority_queue'
#if 0
            ASSERTV(SPEC, PROPAGATE, PROPAGATE == (ma == Y.get_allocator()));
            ASSERTV(SPEC, PROPAGATE,               ma == X.get_allocator());
#endif

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

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::
                              testCase8_select_on_container_copy_construction()
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

    testCase8_select_on_container_copy_construction_dispatch<false, false>();
    testCase8_select_on_container_copy_construction_dispatch<false, true>();

    if (verbose) printf("\n'select_on_container_copy_construction' "
                        "propagates allocator of source object.\n");

    testCase8_select_on_container_copy_construction_dispatch<true, false>();
    testCase8_select_on_container_copy_construction_dispatch<true, true>();

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (verbose) printf("\nVerify C++03 semantics (allocator has no "
                        "'select_on_container_copy_construction' method).\n");

    typedef StatefulStlAllocator<VALUE>                  Allocator;

    typedef bsl::vector<VALUE, Allocator>                CObj;
    typedef bsl::priority_queue<VALUE, CObj, COMPARATOR> Obj;

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

            const CObj C(VALUES.begin(), VALUES.end(), a);

            const Obj W(COMPARATOR(), C, a);  // control

            ASSERTV(ti, LENGTH == W.size());  // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(COMPARATOR(), C, a);  const Obj& X = mX;

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            const Obj Y(X);

// TBD      ASSERTV(SPEC,        W == Y);
// TBD      ASSERTV(SPEC,        W == X);
// TBD no 'get_allocator' in 'priority_queue'
#if 0
            ASSERTV(SPEC, ALLOC_ID == Y.get_allocator().id());
#endif
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase8()
{
    // ------------------------------------------------------------------------
    // COPY CONSTRUCTOR:
    //
    // Concerns:
    //
    //: 1 The new object's value is the same as that of the original object.
    //:   Note that given 'bsl::priority_queue' class has no equality operator,
    //:   two 'bsl::priority_queue' objects are tested equal only if they
    //:   have same size, and have same return values from 'top' operations
    //:   until they are both empty.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 No additional memory is allocated by the target object.
    //:
    //: 4 The value of the original object is left unaffected.
    //:
    //: 5 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 6 Subsequent changes ('push's) on the created object have no
    //:   effect on the original.
    //:
    //: 7 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
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
    //:     each object having the same value, 'V', but configured differently
    //:     identified by 'CONFIG':
    //:     'a': passing a const reference to source object without passing an
    //:          allocator;
    //:
    //:     'b': passing a const reference to source object and an explicit
    //:          null allocator;
    //:
    //:     'c': passing a const reference to source object and the address of
    //:          a test allocator;
    //:
    //:   2 For each of the three iterations in P-2.1:
    //:     1 Create control objects 'Z1' and 'Z2', dynamically create source
    //:       object 'X', using value constructor and a distinct test
    //:       allocator, 'sa'.
    //:
    //:     2 Install distinct allocator as the current default allocator (note
    //:       that a ubiquitous test allocator is already installed as the
    //:       global allocator).
    //:
    //:     3 Choose the copy constructor depending on 'CONFIG' to dynamically
    //:       create an object; use a distinct test allocator for the object's
    //:       footprint.
    //:
    //:     4 Use the appropriate test allocator to verify that no additional
    //:       memory is allocated by the target object.  (C-3)
    //:
    //:     5 Verify that contents of the source object have been copied
    //:       correctly.  (C-1..2)
    //:
    //:     6 Verify that contents of the source object hasn't been affected.
    //:       (C-4)
    //:
    //:     7 Modify source and target objects separately to establish their
    //:       independence by comparing attributes after each modification.
    //:       (C-5..6)
    //:
    //:     8 Destroy source object and verify that target object hasn't been
    //:       affected. (C-5)
    //:
    //:     9 Destroy target object and verify that all obtained memory has
    //:       been released. (C-7)
    //
    // Testing:
    //   priority_queue(const priority_queue& original);
    //   priority_queue(const priority_queue& original, const ALLOCATOR&);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("Testing parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);

    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC = DATA[ti].d_spec;

            if (veryVerbose) {
                T_ P(SPEC);
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator sa("source", veryVeryVeryVerbose);

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we configure constructor

                if (veryVerbose) {
                    T_ T_ P(CONFIG);
                }

                // Create control objects Z1 and Z2.

                Obj mZ1; const Obj& Z1 = gg(&mZ1, SPEC);
                Obj mZ2; const Obj& Z2 = gg(&mZ2, SPEC);

                if (veryVeryVerbose) {
                    printf("\t\tControl Obj Z1: "); P(Z1);
                    printf("\t\tControl Obj Z2: "); P(Z2);
                }

                // Create source object
                Obj        *pX = new Obj(&sa);
                Obj&        mX = gg(pX, SPEC);
                const Obj&  X = mX;
                if (veryVeryVerbose) { printf("\t\tSource Obj: "); P(X); }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator ta("target",    veryVeryVeryVerbose);

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(X);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(X, (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(X, &ta);
                      objAllocatorPtr = &ta;
                  } break;
                  default: {
                      ASSERTV(SPEC, CONFIG, !"Bad constructor config.");
                      return;                                         // RETURN
                  } break;
                }

                Obj&       mY = *objPtr;
                const Obj& Y  = mY;

                ASSERTV(SPEC, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                // Verify correctness of the contents copying.

                ASSERTV(SPEC, CONFIG, is_equal(mZ1, mY));

                // Verify that source object is unaffected.

                ASSERTV(SPEC, CONFIG, is_equal(mZ2, mX));
                ASSERTV(SPEC, CONFIG,
                        use_same_allocator(mX, TYPE_ALLOC, &sa));

                // Verify any attribute allocators are installed properly.
                ASSERTV(SPEC, CONFIG,
                        use_same_allocator(mY, TYPE_ALLOC, objAllocatorPtr));

                // Verify correctness of the comparator moving.

                ASSERTV(SPEC, CONFIG, use_same_comparator(mX, mY));

                // Verify independence of the target object from the source
                // one.

                size_t sourceSize = X.size();
                size_t targetSize = Y.size();

                ASSERTV(SPEC, CONFIG, sourceSize == targetSize);

                mX.push(VALUES[0]);

                ASSERTV(SPEC, CONFIG, sourceSize != X.size());
                ASSERTV(SPEC, CONFIG, targetSize == Y.size());

                sourceSize = X.size();

                mY.push(VALUES[0]);

                ASSERTV(SPEC, CONFIG, sourceSize == X.size());
                ASSERTV(SPEC, CONFIG, targetSize != Y.size());

                targetSize      = Y.size();
                const VALUE top = Y.top();

                // Reclaim dynamically allocated source object.

                delete pX;

                ASSERTV(SPEC, CONFIG, targetSize == Y.size());
                ASSERTV(SPEC, CONFIG, top        == Y.top());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(SPEC, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(SPEC, CONFIG, ta.numBlocksInUse(),
                        0 == ta.numBlocksInUse());
            }  // end foreach configuration

            ASSERTV(SPEC, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(SPEC, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }  // end foreach row
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase7()
{
    // ------------------------------------------------------------------------
    // VALUE (TEMPLATE) CONSTRUCTORS:
    //
    // Concern:
    //: 1 The object is constructed with all values in the user-supplied
    //:   container in the correct order.
    //:
    //: 2 All values within the range, specified by begin and end iterators,
    //:   are inserted.
    //:
    //: 3 If an allocator is NOT supplied to the value constructor, the
    //:   default allocator in effect at the time of construction becomes
    //:   the object allocator for the resulting object.
    //:
    //: 4 If an allocator IS supplied to the value constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 5 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 6 Supplying an allocator to the value constructor has no effect
    //:   on subsequent object values.
    //:
    //: 7 Any memory allocation is from the user-supplied allocator if
    //:   supplied, and otherwise the default allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: Creating an object having the default-constructed value
    //:   allocates no memory.
    //:
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:
    //:
    //:   1 Execute an inner loop creating seven distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently
    //:     identified by 'CONFIG':
    //:     'a': passing a comparator, a container populated with 'V', but
    //:          without passing an allocator;
    //:
    //:     'b': passing the begin and end iterators of a container populated
    //:          with 'V', but without passing an allocator;
    //:
    //:     'c': passing the begin and end iterators of a container populated
    //:          with 'V', and an empty container, but without passing an
    //:          allocator;
    //:
    //:     'd': passing a comparator, and an explicit null allocator;
    //:
    //:     'e': passing a comparator, a container populated with 'V', and an
    //:          explicit null allocator;
    //:
    //:     'f': passing a comparator, and the address of a test allocator
    //:          distinct from the default allocator;
    //:
    //:     'g': passing a comparator, a container populated with 'V', and the
    //:          address of a test allocator distinct from the default
    //:          allocator.
    //:
    //:   2 For each of the seven iterations in P-2.1:  (C-1..7)
    //:
    //:     1 Insert the test data to a specialized container, create a default
    //:       comparator, and obtain begin and end iterators of the populated
    //:       container.
    //:
    //:     2 Create three 'bslma_TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     3 Choose the value constructor depending on 'CONFIG' to dynamically
    //:       create an object using the container in P-2.2.1, with its object
    //:       allocator configured appropriately (see P-2.2.2), supplying all
    //:       the arguments as 'const'; use a distinct test allocator for the
    //:       object's footprint.
    //:
    //:     4 Verify that all of the attributes of each object have their
    //:       expected values.  (C-1..2, 6)
    //:
    //:     5 Use the appropriate test allocators to verify that:
    //:       (C-3..5, 7..8)
    //:
    //:       1 An object that IS expected to allocate memory does so from the
    //:         object allocator (the number of allocations in use).  (C-7)
    //:
    //:       2 An object that is expected NOT to allocate memory does not
    //:         allocate memory.
    //:
    //:       3 If an allocator was supplied at construction (P-2.1c) and
    //:         'CONFIG' is not 'g', the non-object allocator doesn't allocate
    //:         any memory.  (C-7)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-8)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure
    //:       that its object allocator is properly installed.  (C-3..5)
    //
    // Testing:
    //  priority_queue(const COMPARATOR& comp, const CONTAINER& cont);
    //  explicit priority_queue(const COMPARATOR& comparator);
    //  priority_queue(INPUT_ITERATOR first, INPUT_ITERATOR last);
    //  priority_queue(ITER, ITER, const COMPARATOR&, const CONTAINER&);
    //  priority_queue(const COMPARATOR& comp, const ALLOCATOR& allocator);
    //  priority_queue(const COMPARATOR&, const CONTAINER&, const ALLOCATOR&);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    struct DoubleResultDataRow {
        int         d_line;           // source line number
        const char *d_spec;           // specification string, for input to
                                      // 'gg' function

        const char *d_results;        // expected element values
        const char *d_doubleResults;  // expected element values, doubled in
                                      // the container
    };

    const DoubleResultDataRow DATA[] = {
//line spec               results            double container results
//---- ----               ------------------ -------------------------------
{ L_,  "",                "",                ""                              },
{ L_,  "A",               "A",               "AA"                            },
{ L_,  "B",               "B",               "BB"                            },
{ L_,  "AA",              "AA",              "AAAA"                          },
{ L_,  "AB",              "BA",              "BBAA"                          },
{ L_,  "BA",              "BA",              "BBAA"                          },
{ L_,  "AC",              "CA",              "CCAA"                          },
{ L_,  "CD",              "DC",              "DDCC"                          },
{ L_,  "ABC",             "CBA",             "CCBBAA"                        },
{ L_,  "ACB",             "CBA",             "CCBBAA"                        },
{ L_,  "BAC",             "CBA",             "CCBBAA"                        },
{ L_,  "BCA",             "CBA",             "CCBBAA"                        },
{ L_,  "CAB",             "CBA",             "CCBBAA"                        },
{ L_,  "CBA",             "CBA",             "CCBBAA"                        },
{ L_,  "BAD",             "DBA",             "DDBBAA"                        },
{ L_,  "ABCA",            "CBAA",            "CCBBAAAA"                      },
{ L_,  "ABCB",            "CBBA",            "CCBBBBAA"                      },
{ L_,  "ABCC",            "CCBA",            "CCCCBBAA"                      },
{ L_,  "ABCD",            "DCBA",            "DDCCBBAA"                      },
{ L_,  "ACBD",            "DCBA",            "DDCCBBAA"                      },
{ L_,  "BDCA",            "DCBA",            "DDCCBBAA"                      },
{ L_,  "DCBA",            "DCBA",            "DDCCBBAA"                      },
{ L_,  "BEAD",            "EDBA",            "EEDDBBAA"                      },
{ L_,  "BCDE",            "EDCB",            "EEDDCCBB"                      },
{ L_,  "ABCDE",           "EDCBA",           "EEDDCCBBAA"                    },
{ L_,  "ACBDE",           "EDCBA",           "EEDDCCBBAA"                    },
{ L_,  "CEBDA",           "EDCBA",           "EEDDCCBBAA"                    },
{ L_,  "EDCBA",           "EDCBA",           "EEDDCCBBAA"                    },
{ L_,  "FEDCB",           "FEDCB",           "FFEEDDCCBB"                    },
{ L_,  "FEDCBA",          "FEDCBA",          "FFEEDDCCBBAA"                  },
{ L_,  "ABCABC",          "CCBBAA",          "CCCCBBBBAAAA"                  },
{ L_,  "AABBCC",          "CCBBAA",          "CCCCBBBBAAAA"                  },
{ L_,  "ABCDEFG",         "GFEDCBA",         "GGFFEEDDCCBBAA"                },
{ L_,  "ABCDEFGH",        "HGFEDCBA",        "HHGGFFEEDDCCBBAA"              },
{ L_,  "ABCDEFGHI",       "IHGFEDCBA",       "IIHHGGFFEEDDCCBBAA"            },
{ L_,  "ABCDEFGHIJKLMNO", "ONMLKJIHGFEDCBA", "OONNMMLLKKJJIIHHGGFFEEDDCCBBAA"},
{ L_,  "DHBIMACOELGFKNJ", "ONMLKJIHGFEDCBA", "OONNMMLLKKJJIIHHGGFFEEDDCCBBAA"},
};
    const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

    if (verbose) { P(TYPE_ALLOC); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_results);
            const TestValues  DOUBLE_EXP(DATA[ti].d_doubleResults);
            const size_t      LENGTH  = strlen(DATA[ti].d_spec);
            const TestValues  EMPTY_EXP("");

            if (veryVerbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'g'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ T_ P(CONFIG) }

                // Initialize user-supplied data.  Some value constructors
                // take user-supplied container, some use user-supplied
                // iterators.

                CONTAINER copyContainer;
                CONTAINER insertContainer;
                populate_container(copyContainer, SPEC, LENGTH);
                populate_container(insertContainer, SPEC, LENGTH);

                typename CONTAINER::iterator BEGIN = insertContainer.begin();
                typename CONTAINER::iterator END   = insertContainer.end();

                COMPARATOR comparator;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                const TestValues     *expectedValues;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(comparator, copyContainer);
                      objAllocatorPtr = &da;
                      expectedValues = &EXP;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(BEGIN, END);
                      objAllocatorPtr = &da;
                      expectedValues = &EXP;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(BEGIN,
                                            END,
                                            comparator,
                                            copyContainer);
                      objAllocatorPtr = &da;
                      expectedValues = &DOUBLE_EXP;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(comparator, (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                      expectedValues = &EMPTY_EXP;
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(comparator,
                                            copyContainer,
                                            (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                      expectedValues = &EXP;
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(comparator);
                      objAllocatorPtr = &da;
                      expectedValues = &EMPTY_EXP;
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(comparator, &sa);
                      objAllocatorPtr = &sa;
                      expectedValues = &EMPTY_EXP;
                  } break;
                  case 'h': {
                      objPtr = new (fa) Obj(comparator, copyContainer, &sa);
                      objAllocatorPtr = &sa;
                      expectedValues = &EXP;
                  } break;
                  default: {
                      ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj&       mX = *objPtr;
                const Obj& X = mX;

                if (veryVeryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'g' > CONFIG ? sa : da;

                // Ensure the first row of the table contains the
                // default-constructed value.

                if (ti == 0) {
                    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
                    Obj objTemp;
                    ASSERTV(LINE, CONFIG, objTemp, *objPtr,
                            is_equal(objTemp, *objPtr));
                }

                // Verify the expected attributes values.

                const TestValues& EV = *expectedValues;
                ASSERTV(LINE, SPEC, LENGTH, CONFIG,
                        0 == verify_object(mX, EV, EV.size()));

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, use_same_allocator(mX, TYPE_ALLOC, &oa));

                // Verify no allocation from the non-object allocator.

                ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

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
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - top
    //     - size
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
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the object contains the expected number of elements.
    //:       (C-1)
    //:
    //:     3 Use 'top' to verify the values are as expected. (C-1..2)
    //:
    //:     4 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   const_reference top();
    //   size_type       size() const;
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_results);
            const size_t      LENGTH = strlen(DATA[ti].d_results);

            if (veryVerbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj();
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(static_cast<bslma::Allocator*>(0));
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
                      return;                                         // RETURN
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                         ? da
                                         : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma::TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());
                if (LENGTH) {
                    ASSERTV(LINE, SPEC, CONFIG, EXP[0] == X.top());
                }
                ASSERT(oam.isTotalSame());

                // --------------------------------------------------------

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

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase3()
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
    //:   to set the state of newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //   int ggg(priority_queue *object, const char *spec, int verbose = 1);
    //   priority_queue& gg(priority_queue *object, const char* spec);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            Obj mY(&oa);
            const Obj& Y = gg(&mY, SPEC);    // extended spec

            if (curLen != oldLen) {
                if (veryVerbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            ASSERTV(0 == verify_object(mX, EXP, LENGTH));
            ASSERTV(0 == verify_object(mY, EXP, LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;    // source line number
            const char *d_spec;    // specification string
            int         d_return;  // expected return value from 'gg'
        } DATA[] = {
            //line  spec      expected return
            //----  --------  ---------------
            { L_,   "",       -1,             },

            { L_,   "A",      -1,             },
            { L_,   " ",       0,             },
            { L_,   ".",       0,             },
            { L_,   "E",       -1,            },
            { L_,   "a",       0,             },
            { L_,   "z",       0,             },

            { L_,   "AE",     -1,             },
            { L_,   "aE",      0,             },
            { L_,   "Ae",      1,             },
            { L_,   ".~",      0,             },
            { L_,   "~!",      0,             },
            { L_,   "  ",      0,             },

            { L_,   "ABC",    -1,             },
            { L_,   " BC",     0,             },
            { L_,   "A C",     1,             },
            { L_,   "AB ",     2,             },
            { L_,   "?#:",     0,             },
            { L_,   "   ",     0,             },

            { L_,   "ABCDE",  -1,             },
            { L_,   "aBCDE",   0,             },
            { L_,   "ABcDE",   2,             },
            { L_,   "ABCDe",   4,             },
            { L_,   "AbCdE",   1,             }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         EXPR   = DATA[ti].d_return;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);

            if (LENGTH != oldLen) {
                if (veryVerbose) printf("\tof length %d:\n", LENGTH);
                ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, EXPR == RESULT);
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - push
    //      - pop
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
    //: 7 Every object releases any allocated memory at destruction.
    //:
    //: 8 QoI: The default constructor allocates no memory.
    //:
    //: 9 'push' adds an additional element to the object.
    //:
    //:10 'pop' removes the largest element (in value) from the object.
    //
    // Plan:
    //:   1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..10)
    //:
    //:     1 Create three 'bslma_TestAllocator' objects, and install one as as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-8)
    //:
    //:     4 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     5 Push the 'L + 1' elements in the order of increasing order into
    //:       the container and use the (as yet unproven) basic accessors to
    //:       verify the container has the expected values.  Verify the number
    //:       of allocation is as expected. (C-5..6)
    //:
    //:     6 Invoke 'pop' and verify that the container has one fewer element.
    //:       Verify the removed element had the largest value in the
    //:       container.  Verify that no memory is allocated.  (C-10)
    //:
    //:     7 Verify that all object memory is released when the object is
    //:       destroyed.  (C-7)
    //
    // Testing:
    //   priority_queue()
    //   priority_queue(const A& allocator);
    //   ~priority_queue();
    //   void push(const value_type& value);
    //   void pop();
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose) { P(TYPE_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (veryVerbose) {
            printf("\nTesting with various allocator configurations.\n");
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                 *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj((bslma::Allocator*)0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
                  return;                                             // RETURN
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the non-object allocator.

            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 ==  noa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, 0 == X.size());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'push' (bootstrap).\n"); }

            if (veryVeryVerbose) {
                printf("\t\tOn an object of initial length " ZU ".\n", LENGTH);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma::TestAllocatorMonitor tam(&oa);

                mX.push(VALUES[tj]);
                ASSERTV(LENGTH, CONFIG, tj + 1 == X.size());

                ASSERTV(X.top() == VALUES[tj]);

                if (0 < TYPE_ALLOC) {
                    ASSERTV(CONFIG, tam.isInUseUp());
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'pop'.\n");

            if (veryVerbose) {
                printf("\t\tOn an object of initial length " ZU ".\n",
                       LENGTH + 1);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bsls::Types::Int64 B  = oa.numBlocksInUse();

                mX.pop();

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 - tj == X.size());
                if (LENGTH - 1 == tj) {
                    ASSERTV(true == X.empty());
                }
                else {
                    ASSERTV(X.top() == VALUES[LENGTH - 2 - tj]);
                }

                const bsls::Types::Int64 A  = oa.numBlocksInUse();
                ASSERTV(LENGTH, CONFIG, B, A, B - TYPE_ALLOC == A);
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

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase1_NoAlloc(
                                                  const COMPARATOR& comparator,
                                                  const VALUE      *testValues,
                                                  size_t            numValues)
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //   Create four objects using both the default [1] and copy constructors
    //   [2].  Exercise these objects using primary manipulators [1, 5], basic
    //   accessors, equality operators, copy constructors [2, 8] and the
    //   assignment operator [9, 10].  Try aliasing with assignment for a
    //   non-empty object [11].
    //
    //: 1  Create an object x1 (default ctor).       { x1: }
    //:
    //: 2  Create a second object x2 (copy from x1). { x1: x2: }
    //:
    //: 3  Append an element of value A to x1).      { x1:A x2: }
    //:
    //: 4  Append the same value A to x2).           { x1:A x2:A }
    //:
    //: 5  Append two more elements B,C to x2).      { x1:A x2:ABC }
    //:
    //: 6  Remove top element of value A from x1.    { x1: x2:ABC }
    //:
    //: 7  Create a third object x3 (default ctor).  { x1: x2:ABC x3: }
    //:
    //: 8  Create a forth object x4 (copy of x2).    { x1: x2:ABC x3: x4:ABC }
    //:
    //: 9  Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:ABC }
    //:
    //: 10 Assign x3 = x4 (empty becomes non-empty). { x1: x2: x3:ABC x4:ABC }
    //:
    //: 11 Assign x4 = x4 (aliasing).                { x1: x2: x3:ABC x4:ABC }
    //
    // Testing:
    //   This test *exercises* basic functionality.
    // --------------------------------------------------------------------

    ASSERT(testValues);
    ASSERT(1 < numValues);  // Need at least two test elements

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 1) Create an object x1 (default ctor)."
                            "\t\t\t{ x1: }\n");
    Obj mX1;  const Obj& X1 = mX1;

    ASSERT(   0 == X1.size());
    ASSERT(true == X1.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 2) Create a second object x2 (copy from x1)."
                            "\t\t{ x1: x2: }\n");
    Obj mX2(X1);  const Obj& X2 = mX2;
    ASSERT(   0 == X2.size ());
    ASSERT(true == X1.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 3) Push an element of value A to x1)."
                                "\t\t\t{ x1:A x2: }\n");

    mX1.push(testValues[0]);

    ASSERT(            1 == X1.size ());
    ASSERT(        false == X1.empty());
    ASSERT(testValues[0] == X1.top());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 4) Append the same value A to x2)."
                            "\t\t\t{ x1:A x2:A }\n");

    mX2.push(testValues[0]);

    ASSERT(            1 == X2.size ());
    ASSERT(        false == X2.empty());
    ASSERT(testValues[0] == X2.top());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 5) Append two more elements B,C to x2)."
                            "\t\t{ x1:A x2:ABC }\n");

    mX2.push(testValues[1]);
    mX2.push(testValues[2]);

    int maxIndex;
    if (comparator(testValues[0], testValues[1]))
        maxIndex = 1;
    else
        maxIndex = 0;

    if (comparator(testValues[maxIndex], testValues[2]))
        maxIndex = 2;

    ASSERT(                   3 == X2.size ());
    ASSERT(               false == X2.empty());
    ASSERT(testValues[maxIndex] == X2.top  ());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 6) Remove top element of value A from x1."
                            "\t\t{ x1: x2:ABC }\n");
    mX1.pop();

    ASSERT(0 == X1.size());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 7) Create a third object x3 (default ctor)."
                            "\t\t{ x1: x2:ABC x3: }\n");

    Obj mX3;  const Obj& X3 = mX3;

    ASSERT(   0 == X3.size ());
    ASSERT(true == X3.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                            "\t\t{ x1: x2:ABC x3: x4:ABC }\n");

    Obj mX4(X2);  const Obj& X4 = mX4;

    ASSERT(                   3 == X4.size ());
    ASSERT(               false == X4.empty());
    ASSERT(testValues[maxIndex] == X4.top  ());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                            "\t\t{ x1: x2: x3: x4:ABC }\n");

    mX2 = X1;

    ASSERT(   0 == X2.size ());
    ASSERT(true == X2.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:ABC x4:ABC }\n");

    mX3 = X4;

    ASSERT(                   3 == X3.size ());
    ASSERT(               false == X3.empty());
    ASSERT(testValues[maxIndex] == X3.top  ());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n11) Assign x4 = x4 (aliasing)."
                            "\t\t\t\t{ x1: x2: x3:ABC x4:ABC}\n");

    mX4 = X4;

    ASSERT(                   3 == X4.size ());
    ASSERT(               false == X4.empty());
    ASSERT(testValues[maxIndex] == X4.top  ());
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void TestDriver<VALUE, CONTAINER, COMPARATOR>::testCase1(
                                                  const COMPARATOR& comparator,
                                                  const VALUE      *testValues,
                                                  size_t            numValues)
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive
    //:   testing in subsequent test cases.
    //
    // Plan:
    //   Create four objects using both the default [1] and copy constructors
    //   [2].  Exercise these objects using primary manipulators [1, 5], basic
    //   accessors, equality operators, copy constructors [2, 8] and the
    //   assignment operator [9, 10].  Try aliasing with assignment for a
    //   non-empty object [11].
    //
    //: 1  Create an object x1 (default ctor).       { x1: }
    //:
    //: 2  Create a second object x2 (copy from x1). { x1: x2: }
    //:
    //: 3  Append an element of value A to x1).      { x1:A x2: }
    //:
    //: 4  Append the same value A to x2).           { x1:A x2:A }
    //:
    //: 5  Append two more elements B,C to x2).      { x1:A x2:ABC }
    //:
    //: 6  Remove top element of value A from x1.    { x1: x2:ABC }
    //:
    //: 7  Create a third object x3 (default ctor).  { x1: x2:ABC x3: }
    //:
    //: 8  Create a forth object x4 (copy of x2).    { x1: x2:ABC x3: x4:ABC }
    //:
    //: 9  Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:ABC }
    //:
    //: 10 Assign x3 = x4 (empty becomes non-empty). { x1: x2: x3:ABC x4:ABC }
    //:
    //: 11 Assign x4 = x4 (aliasing).                { x1: x2: x3:ABC x4:ABC }
    //
    // Testing:
    //   This test *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    ASSERT(testValues);
    ASSERT(1 < numValues);  // Need at least two test elements

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 1) Create an object x1 (default ctor)."
                            "\t\t\t{ x1: }\n");
    Obj mX1(comparator, &testAllocator);  const Obj& X1 = mX1;

    ASSERT(   0 == X1.size());
    ASSERT(true == X1.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 2) Create a second object x2 (copy from x1)."
                            "\t\t{ x1: x2: }\n");
    Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
    ASSERT(   0 == X2.size ());
    ASSERT(true == X1.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 3) Push an element of value A to x1)."
                                "\t\t\t{ x1:A x2: }\n");

    mX1.push(testValues[0]);

    ASSERT(            1 == X1.size ());
    ASSERT(        false == X1.empty());
    ASSERT(testValues[0] == X1.top());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 4) Append the same value A to x2)."
                            "\t\t\t{ x1:A x2:A }\n");

    mX2.push(testValues[0]);

    ASSERT(            1 == X2.size ());
    ASSERT(        false == X2.empty());
    ASSERT(testValues[0] == X2.top());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 5) Append two more elements B,C to x2)."
                            "\t\t{ x1:A x2:ABC }\n");

    mX2.push(testValues[1]);
    mX2.push(testValues[2]);

    int maxIndex;
    if (comparator(testValues[0], testValues[1]))
        maxIndex = 1;
    else
        maxIndex = 0;

    if (comparator(testValues[maxIndex], testValues[2]))
        maxIndex = 2;

    ASSERT(                   3 == X2.size ());
    ASSERT(               false == X2.empty());
    ASSERT(testValues[maxIndex] == X2.top  ());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 6) Remove top element of value A from x1."
                            "\t\t{ x1: x2:ABC }\n");
    mX1.pop();

    ASSERT(0 == X1.size());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 7) Create a third object x3 (default ctor)."
                            "\t\t{ x1: x2:ABC x3: }\n");

    Obj mX3(comparator, &testAllocator);  const Obj& X3 = mX3;

    ASSERT(   0 == X3.size ());
    ASSERT(true == X3.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                            "\t\t{ x1: x2:ABC x3: x4:ABC }\n");

    Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;

    ASSERT(                   3 == X4.size ());
    ASSERT(               false == X4.empty());
    ASSERT(testValues[maxIndex] == X4.top  ());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                            "\t\t{ x1: x2: x3: x4:ABC }\n");

    mX2 = X1;

    ASSERT(   0 == X2.size ());
    ASSERT(true == X2.empty());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:ABC x4:ABC }\n");

    mX3 = X4;

    ASSERT(                   3 == X3.size ());
    ASSERT(               false == X3.empty());
    ASSERT(testValues[maxIndex] == X3.top  ());

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n11) Assign x4 = x4 (aliasing)."
                            "\t\t\t\t{ x1: x2: x3:ABC x4:ABC}\n");

    mX4 = X4;

    ASSERT(                   3 == X4.size ());
    ASSERT(               false == X4.empty());
    ASSERT(testValues[maxIndex] == X4.top  ());
}

// ============================================================================
//                                  USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Task Scheduler
///- - - - - - - - - - - - -
// In this example, we will use the 'bsl::priority_queue' class to implement a
// task scheduler that schedules a group of tasks based on their designated
// priorities.
//
// Suppose we want to write a background process that runs tasks needed by
// foreground applications.  Each task has a task id, a priority, and a
// function pointer that can be invoked by the background process.  This
// background process has two threads: one thread (receiving thread) receives
// requests from other applications, passing required tasks to a task
// scheduler; the other thread (processing thread) runs the task scheduler,
// executing the tasks one-by-one from higher to lower priorities.  To
// implement this functionality, we can use 'bsl::priority_queue' in the task
// scheduler to buffer received, but as yet unprocessed, tasks.  The task
// scheduler adds newly received tasks into the priority queue in the receiving
// thread, and extracts tasks from the priority queue for execution according
// to their priorities in the processing thread.
//
// First, we define a 'TaskFunction' type:
//..
typedef void (*TaskFunction)(int, int, int);
//..
// Then, we define a 'Task' class, which contains a task id, a 'TaskFunction'
// object and an associated task priority:
//..
class Task
    // This class represents a task that has an integer task id, a task
    // function, and an integer priority.  The smaller the numerical value
    // of a priority, the higher the priority.
{
  private:
    // DATA
    int          d_taskId;          // task id

    TaskFunction d_taskFunction_p;  // task function

    int          d_priority;        // priority of the task

  public:
    // CREATORS
    explicit Task(int taskId, TaskFunction taskFunction, int priority);
        // Construct a 'Task' object having the specified 'taskId', the
        // specified 'd_taskFunction_p', and the specified 'priority'.

    // ACCESSORS
    int getId() const;
        // Return the contained task id.

    int getPriority() const;
        // Return the priority of the task.

    TaskFunction getFunction() const;
        // Return the contained task function object.
};

// CREATORS
Task::Task(int taskId, TaskFunction taskFunction, int priority)
: d_taskId(taskId)
, d_taskFunction_p(taskFunction)
, d_priority(priority)
{
}

// ACCESSORS
inline
int Task::getId() const
{
    return d_taskId;
}

inline
int Task::getPriority() const
{
    return d_priority;
}

inline
TaskFunction Task::getFunction() const
{
    return d_taskFunction_p;
}
//..
// Next, we define a functor to compare the priorities of two 'Task' objects:
//..
struct TaskComparator {
    // This 'struct' defines an ordering on 'Task' objects, allowing them
    // to be included in sorted data structures such as
    // 'bsl::priority_queue'.

    bool operator()(const Task& lhs, const Task& rhs) const
        // Return 'true' if the priority of the specified 'lhs' is
        // numerically less than that of the specified 'rhs', and 'false'
        // otherwise.  Note that the smaller the value returned by the
        // 'Task::getPriority' method, the higher the priority.
    {
        return lhs.getPriority() > rhs.getPriority();
    }
};
//..
// Then, we define a 'TaskScheduler' class that provides methods to hold and
// schedule unprocessed tasks:
//..
class TaskScheduler {
    // This class holds and schedules tasks to execute.
//..
// Here, we define a private data member that is an instantiation of
// 'bsl::priority_queue', which uses 'Task' for its 'VALUE' (template
// parameter) type, 'bsl::vector<Task>' for its 'CONTAINER' (template
// parameter) type, and 'TaskComparator' for its 'COMPARATOR' (template
// parameter) type:
//..
    // DATA
    bsl::priority_queue<Task,
                        bsl::vector<Task>,
                        TaskComparator>
          d_taskPriorityQueue;  // priority queue holding unprocessed tasks

    // ...

  public:
    // CREATORS
    explicit TaskScheduler(bslma::Allocator *basicAllocator = 0);
        // Create a 'TaskScheduler' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    // MANIPULATORS
    void addTask(int taskId, TaskFunction taskFunction, int priority);
        // Enqueue the specified 'task' having the specified 'priority'
        // onto this scheduler.

    void processTasks(int verbose);
        // Dequeue the task having the highest priority in this scheduler,
        // and call its task function by passing in the specified 'verbose'
        // flag.
};
//..
// Next, we implement the 'TaskScheduler' constructor:
//..
TaskScheduler::TaskScheduler(bslma::Allocator *basicAllocator)
: d_taskPriorityQueue(basicAllocator)
{
}
//..
// Notice that we pass to the contained 'd_taskPriorityQueue' object the
// 'bslma::Allocator' supplied to the 'TaskScheduler' at construction.
//
// Then, we implement the 'addTask' method, which constructs a 'Task' object
// and adds it into the priority queue:
//..
void TaskScheduler::addTask(int taskId,
                            TaskFunction taskFunction,
                            int priority)
{
    // ... (some synchronization)

    d_taskPriorityQueue.push(Task(taskId, taskFunction, priority));

    // ...
}
//..
// Next, we implement the 'processTasks' method, which extracts tasks from the
// priority queue in order of descending priorities, and executes them:
//..
void TaskScheduler::processTasks(int verbose)
{
    // ... (some synchronization)

    while (!d_taskPriorityQueue.empty()) {
        const Task& task = d_taskPriorityQueue.top();
        TaskFunction taskFunction = task.getFunction();
        if (taskFunction) {
            taskFunction(task.getId(), task.getPriority(), verbose);
        }
        d_taskPriorityQueue.pop();
    }

    // ...
}
//..
// Note that the 'top' method always returns the 'Task' object having the
// highest priority in the priority queue.
//
// Then, we define two task functions:
//..
void taskFunction1(int taskId, int priority, int verbose)
{
    if (verbose) {
        printf("Executing task %d (priority = %d) in 'taskFunction1'.\n",
               taskId,
               priority);
    }
}

void taskFunction2(int taskId, int priority, int verbose)
{
    if (verbose) {
        printf("Executing task %d (priority = %d) in 'taskFunction2'.\n",
               taskId,
               priority);
    }
}
//..
// Next, we create a global 'TaskScheduler' object:
//..
//  TaskScheduler taskScheduler;
//..
// Now, we call the 'addTask' method of 'taskScheduler' in the receiving
// thread:
//..
//  // (in receiving thread)
//  // ...
//
//  taskScheduler.addTask(1, taskFunction1, 50);
//
//  // ...
//
//  taskScheduler.addTask(2, taskFunction1, 99);
//
//  // ...
//
//  taskScheduler.addTask(3, taskFunction2, 4);
//
//  // ...
//..
// Finally, we call the 'processTasks' method of 'taskScheduler' in the
// processing thread:
//..
//  // (in processing thread)
//  // ...
//
//  taskScheduler.processTasks(veryVerbose);
//
//  // ...
//..

}  // close namespace UsageExample

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CTAD
struct TestDeductionGuides {

#define ASSERT_SAME_TYPE(...) \
 static_assert((bsl::is_same<__VA_ARGS__>::value), "Types differ unexpectedly")

    template <class KEY_TYPE>
    struct StupidLess {
        bool operator()(const KEY_TYPE&, const KEY_TYPE&) const
            // Always return true
        {
            return true;
        }
    };

    template <class KEY_TYPE>
    static size_t StupidLessFn(const KEY_TYPE&, const KEY_TYPE&)
        // Always return true
    {
        return true;
    }

    // This struct provides a namespace for functions testing deduction guides.
    // The tests are compile-time only; it is not necessary that these routines
    // be called at run-time.  Note that the following constructors do not have
    // associated deduction guides because the template parameters for
    // 'bsl::stack' cannot be deduced from the constructor parameters.
    //..
    // priority_queue()
    // priority_queue(COMPARATOR)
    // priority_queue(ALLOCATOR)
    // priority_queue(COMPARATOR, ALLOCATOR)
    //..

    void SimpleConstructors ()
        // Test that constructing a 'bsl::priority_queue' from various
        // combinations of arguments deduces the correct type.
        //..
        // priority_queue(const priority_queue&  pq)            -> decltype(pq)
        // priority_queue(const priority_queue&  pq, ALLOCATOR) -> decltype(pq)
        // priority_queue(      priority_queue&& pq)            -> decltype(pq)
        // priority_queue(      priority_queue&& pq, ALLOCATOR) -> decltype(pq)
        // priority_queue(COMPARATOR, const CONTAINER &)
        // priority_queue(COMPARATOR,       CONTAINER &&)
        // priority_queue(COMPARATOR, const CONTAINER & , ALLOCATOR)
        // priority_queue(COMPARATOR,       CONTAINER &&, ALLOCATOR)
        // priority_queue(Iter, Iter)
        // priority_queue(Iter, Iter, COMPARATOR, const CONTAINER &)
        // priority_queue(Iter, Iter, COMPARATOR,       CONTAINER &&)
        //..
    {
        bslma::Allocator     *a1 = nullptr;
        bslma::TestAllocator *a2 = nullptr;

        typedef int T1;
        bsl::priority_queue<T1> pq1;
        bsl::priority_queue     pq1a(pq1);
        ASSERT_SAME_TYPE(decltype(pq1a), bsl::priority_queue<T1>);

        typedef float T2;
        bsl::priority_queue<T2> pq2;
        bsl::priority_queue     pq2a(pq2, bsl::allocator<T2>());
        bsl::priority_queue     pq2b(pq2, a1);
        bsl::priority_queue     pq2c(pq2, a2);
        bsl::priority_queue     pq2d(pq2, bsl::allocator<int>());
        ASSERT_SAME_TYPE(decltype(pq2a), bsl::priority_queue<T2>);
        ASSERT_SAME_TYPE(decltype(pq2b), bsl::priority_queue<T2>);
        ASSERT_SAME_TYPE(decltype(pq2c), bsl::priority_queue<T2>);
        ASSERT_SAME_TYPE(decltype(pq2d), bsl::priority_queue<T2>);

        typedef short T3;
        bsl::priority_queue<T3> pq3;
        bsl::priority_queue     pq3a(std::move(pq3));
        ASSERT_SAME_TYPE(decltype(pq3a), bsl::priority_queue<T3>);

        typedef long double T4;
        bsl::priority_queue<T4> pq4;
        bsl::priority_queue     pq4a(std::move(pq4), bsl::allocator<T4>{});
        bsl::priority_queue     pq4b(std::move(pq4), a1);
        bsl::priority_queue     pq4c(std::move(pq4), a2);
        bsl::priority_queue     pq4d(std::move(pq4), bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(pq4a), bsl::priority_queue<T4>);
        ASSERT_SAME_TYPE(decltype(pq4b), bsl::priority_queue<T4>);
        ASSERT_SAME_TYPE(decltype(pq4c), bsl::priority_queue<T4>);
        ASSERT_SAME_TYPE(decltype(pq4d), bsl::priority_queue<T4>);


        typedef long                       T5;
        typedef StupidLess<T5>             CompT5;
        typedef decltype(StupidLessFn<T5>) CompFnT5;

        bsl::vector<T5>       v5;
        NonAllocContainer<T5> nc5;
        bsl::priority_queue   pq5a(CompT5{}, v5);
        bsl::priority_queue   pq5b(CompT5{}, nc5);
        bsl::priority_queue   pq5c(StupidLessFn<T5>, v5);
        bsl::priority_queue   pq5d(StupidLessFn<T5>, nc5);
        ASSERT_SAME_TYPE(decltype(pq5a),
                         bsl::priority_queue<T5, bsl::vector<T5>, CompT5>);
        ASSERT_SAME_TYPE(
                       decltype(pq5b),
                       bsl::priority_queue<T5, NonAllocContainer<T5>, CompT5>);
        ASSERT_SAME_TYPE(decltype(pq5c),
                         bsl::priority_queue<T5, bsl::vector<T5>, CompFnT5 *>);
        ASSERT_SAME_TYPE(
                   decltype(pq5d),
                   bsl::priority_queue<T5, NonAllocContainer<T5>, CompFnT5 *>);


        typedef short                      T6;
        typedef StupidLess<T6>             CompT6;
        typedef decltype(StupidLessFn<T6>) CompFnT6;

        bsl::vector<T6>     v6;
        bsl::priority_queue pq6a(CompT6{}, v6, bsl::allocator<T6>{});
        bsl::priority_queue pq6b(CompT6{}, v6, a1);
        bsl::priority_queue pq6c(CompT6{}, v6, a2);
        bsl::priority_queue pq6d(CompT6{}, v6, bsl::allocator<int>{});
        bsl::priority_queue pq6e(StupidLessFn<T6>, v6, bsl::allocator<T6>{});
        bsl::priority_queue pq6f(StupidLessFn<T6>, v6, a1);
        bsl::priority_queue pq6g(StupidLessFn<T6>, v6, a2);
        bsl::priority_queue pq6h(StupidLessFn<T6>, v6, bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(pq6a),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompT6>);
        ASSERT_SAME_TYPE(decltype(pq6b),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompT6>);
        ASSERT_SAME_TYPE(decltype(pq6c),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompT6>);
        ASSERT_SAME_TYPE(decltype(pq6d),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompT6>);
        ASSERT_SAME_TYPE(decltype(pq6e),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompFnT6 *>);
        ASSERT_SAME_TYPE(decltype(pq6f),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompFnT6 *>);
        ASSERT_SAME_TYPE(decltype(pq6g),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompFnT6 *>);
        ASSERT_SAME_TYPE(decltype(pq6h),
                         bsl::priority_queue<T6, bsl::vector<T6>, CompFnT6 *>);


        typedef double                     T7;
        typedef StupidLess<T7>             CompT7;
        typedef decltype(StupidLessFn<T7>) CompFnT7;

        bsl::vector<T7>       v7;
        NonAllocContainer<T7> nc7;
        bsl::priority_queue   pq7a(CompT7{}, std::move(v7));
        bsl::priority_queue   pq7b(CompT7{}, std::move(nc7));
        bsl::priority_queue   pq7c(StupidLessFn<T7>, std::move(v7));
        bsl::priority_queue   pq7d(StupidLessFn<T7>, std::move(nc7));
        ASSERT_SAME_TYPE(decltype(pq7a),
                         bsl::priority_queue<T7, bsl::vector<T7>, CompT7>);
        ASSERT_SAME_TYPE(
                       decltype(pq7b),
                       bsl::priority_queue<T7, NonAllocContainer<T7>, CompT7>);
        ASSERT_SAME_TYPE(decltype(pq7c),
                         bsl::priority_queue<T7, bsl::vector<T7>, CompFnT7 *>);
        ASSERT_SAME_TYPE(
                   decltype(pq7d),
                   bsl::priority_queue<T7, NonAllocContainer<T7>, CompFnT7 *>);


        typedef short                      T8;
        typedef StupidLess<T8>             CompT8;
        typedef decltype(StupidLessFn<T8>) CompFnT8;

        bsl::vector<T8>     v8;
        bsl::priority_queue pq8a(CompT8{}, v8, bsl::allocator<T8>{});
        bsl::priority_queue pq8b(CompT8{}, v8, a1);
        bsl::priority_queue pq8c(CompT8{}, v8, a2);
        bsl::priority_queue pq8d(CompT8{}, v8, bsl::allocator<int>{});
        bsl::priority_queue pq8e(StupidLessFn<T8>, v8, bsl::allocator<T8>{});
        bsl::priority_queue pq8f(StupidLessFn<T8>, v8, a1);
        bsl::priority_queue pq8g(StupidLessFn<T8>, v8, a2);
        bsl::priority_queue pq8h(StupidLessFn<T8>, v8, bsl::allocator<int>{});
        ASSERT_SAME_TYPE(decltype(pq8a),
                             bsl::priority_queue<T8, bsl::vector<T8>, CompT8>);
        ASSERT_SAME_TYPE(decltype(pq8b),
                             bsl::priority_queue<T8, bsl::vector<T8>, CompT8>);
        ASSERT_SAME_TYPE(decltype(pq8c),
                             bsl::priority_queue<T8, bsl::vector<T8>, CompT8>);
        ASSERT_SAME_TYPE(decltype(pq8d),
                             bsl::priority_queue<T8, bsl::vector<T8>, CompT8>);
        ASSERT_SAME_TYPE(decltype(pq8e),
                         bsl::priority_queue<T8, bsl::vector<T8>, CompFnT8 *>);
        ASSERT_SAME_TYPE(decltype(pq8f),
                         bsl::priority_queue<T8, bsl::vector<T8>, CompFnT8 *>);
        ASSERT_SAME_TYPE(decltype(pq8g),
                         bsl::priority_queue<T8, bsl::vector<T8>, CompFnT8 *>);
        ASSERT_SAME_TYPE(decltype(pq8h),
                         bsl::priority_queue<T8, bsl::vector<T8>, CompFnT8 *>);


        typedef char T9;
        T9                        *p9b = nullptr;
        T9                        *p9e = nullptr;
        bsl::vector<T9>::iterator  i9b = nullptr;
        bsl::vector<T9>::iterator  i9e = nullptr;
        bsl::priority_queue        pq9a(p9b, p9e);
        bsl::priority_queue        pq9b(i9b, i9e);
        ASSERT_SAME_TYPE(decltype(pq9a), bsl::priority_queue<T9>);
        ASSERT_SAME_TYPE(decltype(pq9a), bsl::priority_queue<T9>);


        typedef unsigned char               T10;
        typedef StupidLess<T10>             CompT10;
        typedef decltype(StupidLessFn<T10>) CompFnT10;

        T10                        *p10b = nullptr;
        T10                        *p10e = nullptr;
        bsl::vector<T10>::iterator  i10b = nullptr;
        bsl::vector<T10>::iterator  i10e = nullptr;
        bsl::vector<T10>            v10;

        bsl::priority_queue pq10a(p10b, p10e, CompT10{}, v10);
        bsl::priority_queue pq10b(i10b, i10e, CompT10{}, v10);
        bsl::priority_queue pq10c(p10b, p10e, StupidLessFn<T10>, v10);
        bsl::priority_queue pq10d(i10b, i10e, StupidLessFn<T10>, v10);
        ASSERT_SAME_TYPE(decltype(pq10a),
                         bsl::priority_queue<T10, bsl::vector<T10>, CompT10>);
        ASSERT_SAME_TYPE(decltype(pq10a),
                         bsl::priority_queue<T10, bsl::vector<T10>, CompT10>);
        ASSERT_SAME_TYPE(
                      decltype(pq10c),
                      bsl::priority_queue<T10, bsl::vector<T10>, CompFnT10 *>);
        ASSERT_SAME_TYPE(
                      decltype(pq10d),
                      bsl::priority_queue<T10, bsl::vector<T10>, CompFnT10 *>);


        typedef signed char                 T11;
        typedef StupidLess<T11>             CompT11;
        typedef decltype(StupidLessFn<T11>) CompFnT11;
        T11                        *p11b = nullptr;
        T11                        *p11e = nullptr;
        bsl::vector<T11>::iterator  i11b = nullptr;
        bsl::vector<T11>::iterator  i11e = nullptr;
        bsl::vector<T11>            v11;

        bsl::priority_queue pq11a(p11b, p11e, CompT11{}, std::move(v11));
        bsl::priority_queue pq11b(i11b, i11e, CompT11{}, std::move(v11));
        bsl::priority_queue pq11c(p11b,
                                  p11e,
                                  StupidLessFn<T11>,
                                  std::move(v11));
        bsl::priority_queue pq11d(i11b,
                                  i11e,
                                  StupidLessFn<T11>,
                                  std::move(v11));
        ASSERT_SAME_TYPE(decltype(pq11a),
                         bsl::priority_queue<T11, bsl::vector<T11>, CompT11>);
        ASSERT_SAME_TYPE(decltype(pq11a),
                         bsl::priority_queue<T11, bsl::vector<T11>, CompT11>);
        ASSERT_SAME_TYPE(
                      decltype(pq11c),
                      bsl::priority_queue<T11, bsl::vector<T11>, CompFnT11 *>);
        ASSERT_SAME_TYPE(
                      decltype(pq11d),
                      bsl::priority_queue<T11, bsl::vector<T11>, CompFnT11 *>);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Compile-fail tests
// #define BSLSTL_PRIORITY_COMPILE_FAIL_ALLOCATOR_IS_NOT_A_CONTAINER
#ifdef  BSLSTL_PRIORITY_COMPILE_FAIL_ALLOCATOR_IS_NOT_A_CONTAINER
        typedef unsigned char  T98;
        typedef std::less<T98> Comp98;
        bsl::priority_queue    pq98a(Comp98{}, bsl::allocator<T98>{});
        // This should fail to compile (Allocator is not a container)
#endif

// #define BSLSTL_PRIORITY_QUEUE_COMPILE_FAIL_NON_ALLOCATOR_AWARE_CONTAINER
#ifdef  BSLSTL_PRIORITY_QUEUE_COMPILE_FAIL_NON_ALLOCATOR_AWARE_CONTAINER
        typedef unsigned short T99;
        typedef std::less<T99> Comp99;
        NonAllocContainer<T99> nc99;
        bsl::priority_queue    pq99a(Comp99{}, nc99, bsl::allocator<T99>{});
        bsl::priority_queue    pq99b(Comp99{}, std::move(nc99),
                                                       bsl::allocator<T99>{});
        // These should fail to compile (can't supply an allocator to a
        // non-allocator-aware container.)
#endif
    }

#undef ASSERT_SAME_TYPE
};
#endif  // BSLS_COMPILERFEATURES_SUPPORT_CTAD

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

bool intLessThan(int a, int b)
{
    return a < b;
}

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
    bslma::TestAllocator ta(veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 23: {
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
        //: 1 Create a list by invoking the constructor without supplying the
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
      case 22: {
        // --------------------------------------------------------------------
        // 'noexcept' SPECIFICATION
        // --------------------------------------------------------------------

        if (verbose) printf("\n" "'noexcept' SPECIFICATION" "\n"
                                 "========================" "\n");

#if BSLS_KEYWORD_NOEXCEPT_AVAILABLE
#ifndef BSLMF_ISNOTHROWSWAPPABLE_ALWAYS_FALSE
        ASSERT(!bsl::is_nothrow_swappable<vector<int> >::value);
        ASSERT( bsl::is_nothrow_swappable<TestComparator<int> >::value);
        TestDriver<int, vector<int>, TestComparator<int> >::testCase22();

        ASSERT( bsl::is_nothrow_swappable<NothrowSwapVector<int> >::value);
        ASSERT( bsl::is_nothrow_swappable<TestComparator<int> >::value);
        TestDriver<int, NothrowSwapVector<int>, TestComparator<int> >
                                                                ::testCase22();

        ASSERT( bsl::is_nothrow_swappable<NothrowSwapVector<int> >::value);
        ASSERT(!bsl::is_nothrow_swappable<ThrowingSwapComparator<int> >
                                                                      ::value);
        TestDriver<int, NothrowSwapVector<int>, ThrowingSwapComparator<int> >
                                                                ::testCase22();
#endif

        ASSERT(!bsl::is_nothrow_swappable<vector<int> >::value);
        ASSERT(!bsl::is_nothrow_swappable<ThrowingSwapComparator<int> >
                                                                     ::value);
        TestDriver<int, vector<int>, ThrowingSwapComparator<int> >
                                                                ::testCase22();
#endif
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        using namespace UsageExample;

        TaskScheduler taskScheduler(&ta);

        int priorities[] = {1,
                            65535,
                            530,
                            -200,
                            0,
                            INT_MAX,
                            10005,
                            1366,
                            999999999,
                            INT_MIN,
                            -123456789};
        const int numTasks =
                  static_cast<int>(sizeof(priorities) / sizeof(priorities[0]));

        for (int i = 0;i < 5; ++i) {
            taskScheduler.addTask(i + 1,
                                  (i % 2) ? taskFunction1 : taskFunction2,
                                  priorities[i]);
        }
        taskScheduler.processTasks(veryVerbose);
        for (int i = 5;i < numTasks; ++i) {
            taskScheduler.addTask(i + 1,
                                  (i % 2) ? taskFunction1 : taskFunction2,
                                  priorities[i]);
        }
        taskScheduler.processTasks(veryVerbose);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // NON ALLOCATOR SUPPORTING TYPE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NON ALLOCATOR SUPPORTING TYPE"
                            "\n=====================================\n");

        typedef priority_queue<int, NonAllocContainer<int> >
                                                         NonAllocPriorityQueue;

        NonAllocPriorityQueue        mX;
        const NonAllocPriorityQueue& X = mX;

        ASSERT(X.empty());

        mX.push(3);
        mX.push(4);
        mX.push(5);

        ASSERT(! X.empty());
        ASSERT(3 == X.size());
        ASSERT(5 == X.top());
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // EMPLACE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EMPLACE"
                            "\n===============\n");
        RUN_EACH_TYPE(TestDriver, testCase19, TEST_TYPES_EMPLACE);
        TestDriver<TestValueType, vector<TestValueType> >::testCase19();

        if (verbose) printf("\nTESTING EMPLACE FORWARDING PARAMETERS"
                            "\n=====================================\n");
        TestDriver<int, MovableContainer<int> >::testCase19a();
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE-ASSIGNMENT OPERATOR"
                            "\n================================\n");

        RUN_EACH_TYPE(TestDriver, testCase18, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase18();
        TestDriver<int, NonMovableContainer<int> >::testCase18();
        TestDriver<int, MovableContainer<int> >::testCase18();

        // 'propagate_on_container_move_assignment' testing

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase18_propagate_on_container_move_assignment,
                      TEST_TYPES_EMPLACE);

        RUN_EACH_TYPE(TestDriver,
                      testCase18_propagate_on_container_move_assignment,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PUSH WITH MOVABLE PARAMETERS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PUSH WITH MOVABLE PARAMETERS"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver, testCase17, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase17();
        TestDriver<int, NonMovableContainer<int> >::testCase17();
        TestDriver<int, MovableContainer<int> >::testCase17();
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS WITH MOVABLE PARAMETERS
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING VALUE CONSTRUCTORS WITH MOVABLE PARAMETERS"
                   "\n==================================================\n");

        RUN_EACH_TYPE(TestDriver, testCase16, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase16();
        TestDriver<int, NonMovableContainer<int> >::testCase16();
        TestDriver<int, MovableContainer<int> >::testCase16();
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING MOVE CONSTRUCTOR"
                            "\n========================\n");

        RUN_EACH_TYPE(TestDriver, testCase15, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase15();
        TestDriver<int, NonMovableContainer<int> >::testCase15();
        TestDriver<int, MovableContainer<int> >::testCase15();
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // FREE OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FREE OPERATORS"
                            "\n======================\n");

        if (verbose)
            printf("There is no free operator for this component.\n");

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ACCESSORS"
                            "\n=================\n");

        RUN_EACH_TYPE(TestDriver, testCase13, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase13();
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Type Traits\n"
                            "\n===================\n");

        // Verify the 'UsesBslmaAllocator' trait is not defined for non-'bslma'
        // allocators.

        typedef bsltf::StdTestAllocator<bsltf::AllocTestType> StlAlloc;

        typedef bsltf::AllocTestType ATT;

        typedef vector<ATT, StlAlloc> WeirdAllocVector;

        typedef bsl::priority_queue<ATT, WeirdAllocVector>
                                                 WeirdAllocVectorPriorityQueue;
        typedef bsl::priority_queue<int, NonAllocContainer<int> >
                                                 NonAllocPriorityQueue;

        if (verbose) printf("NonAllocContainer ---------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                             NonAllocContainer<int> >::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                             NonAllocPriorityQueue>::value));
        TestDriver<NonAllocContainer<int> >::testCase12();

        if (verbose) printf("vector --------------------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                       WeirdAllocVector>::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                       WeirdAllocVectorPriorityQueue>::value));
        RUN_EACH_TYPE(TestDriver, testCase12, TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY-ASSIGNMENT OPERATOR"
                            "\n================================\n");

        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase11();

        // 'propagate_on_container_copy_assignment' testing

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase11_propagate_on_container_copy_assignment,
                      TEST_TYPES_EMPLACE);

        RUN_EACH_TYPE(TestDriver,
                      testCase11_propagate_on_container_copy_assignment,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STREAMING FUNCTIONALITY"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'SWAP'
        // --------------------------------------------------------------------

        if (verbose) printf(
                           "\nTESTING MANIPULATOR AND FREE FUNCTION 'SWAP'"
                           "\n============================================\n");

        RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase9();

        // 'propagate_on_container_swap' testing

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_swap,
                      TEST_TYPES_EMPLACE);

        RUN_EACH_TYPE(TestDriver,
                      testCase9_propagate_on_container_swap,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTORS"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver, testCase8, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase8();

        // 'select_on_container_copy_construction' testing

        if (verbose) printf("\nCOPY CONSTRUCTOR: ALLOCATOR PROPAGATION"
                            "\n=======================================\n");

// TBD enable this
#if 0
        RUN_EACH_TYPE(TestDriver,
                      testCase8_select_on_container_copy_construction,
                      TEST_TYPES_EMPLACE);

        RUN_EACH_TYPE(TestDriver,
                      testCase8_select_on_container_copy_construction,
                      bsltf::MovableTestType,
                      bsltf::MovableAllocTestType);
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VALUE CONSTRUCTORS"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase7();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY OPERATORS"
                            "\n==========================\n");

        if (verbose)
            printf("There is no equality operator for this component.\n");

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // OUTPUT (<<) OPERATOR
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

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'GG' AND 'GGG'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTIONS 'GG' AND 'GGG'"
                            "\n==========================================\n");

        RUN_EACH_TYPE(TestDriver, testCase3, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES_REGULAR);
        TestDriver<TestValueType, vector<TestValueType> >::testCase2();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bool (*Comparator)(int, int);
        TestDriver<int, vector<int>, Comparator>::testCase1(
                                                       &intLessThan,
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
        TestDriver<int, vector<int>, std::less<int> >::testCase1(
                                                       std::less<int>(),
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
        TestDriver<int, vector<int>, std::less<int> >::testCase1(
                                                       std::less<int>(),
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);

        TestDriver<int,
                   NonAllocContainer<int>,
                   std::less<int> >::testCase1_NoAlloc(std::less<int>(),
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
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
// Copyright 2016 Bloomberg Finance L.P.
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
