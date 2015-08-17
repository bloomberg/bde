// bslstl_stack.t.cpp                                                 -*-C++-*-
#include <bslstl_stack.h>

#include <bslstl_vector.h>

#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslalg_rangecompare.h>

#include <bslmf_issame.h>
#include <bslmf_haspointersemantics.h>

#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <algorithm>
#include <functional>
#include <typeinfo>
#include <cstdio>

#include <cstdio>
#include <cstdlib>
#include <string.h>

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
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
//: o 'push'
//: o 'pop
//
// Basic Accessors:
//: o 'empty'
//: o 'size'
//: o 'top'
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push' and 'pop' methods to be used by the generator functions 'g' and
// 'gg'.  Note that some manipulators must support aliasing, and those that
// perform memory allocation must be tested for exception neutrality via the
// 'bslma::TestAllocator' component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.
//
// Certain standard value-semantic-type test cases are omitted:
//: o BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// CREATORS
// [ 7] copy c'tor
// [ 2] stack, stack(bslma::Allocator *bA)
//
// MANIPULATORS
// [ 9] 'operator='
// [ 8] member swap
// [ 2] Primary Manipulators -- push and pop
//
// ACCESSORS
// [15] testing empty, size
// [ 4] Primary Accessors
//
// FREE FUNCTIONS
// [12] inequality comparisons: '<', '>', '<=', '>='
// [ 6] equality comparisons: '==', '!='
// [ 5] operator<< (N/A)
//
// OTHER
// [16] Usage Example
// [14] testing simple container that does not support allocators
// [13] testing container override of specified 'VALUE'
// [11] type traits
// [10] allocator
// [ 3] Primary generator functions 'gg' and 'ggg'
// [ 1] Breathing Test
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

// Define DEFAULT DATA used in multiple test cases.

static const size_t DEFAULT_MAX_LENGTH = 32;

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line spec                 results
    //---- --------             -------
    { L_,  "",                  "" },
    { L_,  "A",                 "A" },
    { L_,  "AA",                "A" },
    { L_,  "B",                 "B" },
    { L_,  "AB",                "AB" },
    { L_,  "BA",                "AB" },
    { L_,  "AC",                "AC" },
    { L_,  "CD",                "CD" },
    { L_,  "ABC",               "ABC" },
    { L_,  "ACB",               "ABC" },
    { L_,  "BAC",               "ABC" },
    { L_,  "BCA",               "ABC" },
    { L_,  "CAB",               "ABC" },
    { L_,  "CBA",               "ABC" },
    { L_,  "BAD",               "ABD" },
    { L_,  "ABCA",              "ABC" },
    { L_,  "ABCB",              "ABC" },
    { L_,  "ABCC",              "ABC" },
    { L_,  "ABCABC",            "ABC" },
    { L_,  "AABBCC",            "ABC" },
    { L_,  "ABCD",              "ABCD" },
    { L_,  "ACBD",              "ABCD" },
    { L_,  "BDCA",              "ABCD" },
    { L_,  "DCBA",              "ABCD" },
    { L_,  "BEAD",              "ABDE" },
    { L_,  "BCDE",              "BCDE" },
    { L_,  "ABCDE",             "ABCDE" },
    { L_,  "ACBDE",             "ABCDE" },
    { L_,  "CEBDA",             "ABCDE" },
    { L_,  "EDCBA",             "ABCDE" },
    { L_,  "FEDCB",             "BCDEF" },
    { L_,  "FEDCBA",            "ABCDEF" },
    { L_,  "ABCDEFG",           "ABCDEFG" },
    { L_,  "ABCDEFGH",          "ABCDEFGH" },
    { L_,  "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,  "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP" },
    { L_,  "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP" },
    { L_,  "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,  "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};
static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifndef BSLS_PLATFORM_OS_WINDOWS
# define TEST_TYPES_REGULAR(containerArg)                                     \
        containerArg<signed char>,                                            \
        containerArg<size_t>,                                                 \
        containerArg<bsltf::TemplateTestFacility::ObjectPtr>,                 \
        containerArg<bsltf::TemplateTestFacility::FunctionPtr>,               \
        containerArg<bsltf::TemplateTestFacility::MethodPtr>,                 \
        containerArg<bsltf::EnumeratedTestType::Enum>,                        \
        containerArg<bsltf::UnionTestType>,                                   \
        containerArg<bsltf::SimpleTestType>,                                  \
        containerArg<bsltf::AllocTestType>,                                   \
        containerArg<bsltf::BitwiseMoveableTestType>,                         \
        containerArg<bsltf::AllocBitwiseMoveableTestType>,                    \
        containerArg<bsltf::NonTypicalOverloadsTestType>
#else
# define TEST_TYPES_REGULAR(containerArg)                                     \
        containerArg<signed char>,                                            \
        containerArg<size_t>,                                                 \
        containerArg<bsltf::TemplateTestFacility::ObjectPtr>,                 \
        containerArg<bsltf::TemplateTestFacility::MethodPtr>,                 \
        containerArg<bsltf::EnumeratedTestType::Enum>,                        \
        containerArg<bsltf::UnionTestType>,                                   \
        containerArg<bsltf::SimpleTestType>,                                  \
        containerArg<bsltf::AllocTestType>,                                   \
        containerArg<bsltf::BitwiseMoveableTestType>,                         \
        containerArg<bsltf::AllocBitwiseMoveableTestType>,                    \
        containerArg<bsltf::NonTypicalOverloadsTestType>
#endif

#define TEST_TYPES_INEQUAL_COMPARABLE(containerArg)                           \
        containerArg<signed char>,                                            \
        containerArg<size_t>,                                                 \
        containerArg<bsltf::TemplateTestFacility::ObjectPtr>,                 \
        containerArg<bsltf::EnumeratedTestType::Enum>

template <class VALUE>
struct NonAllocCont {
    // PUBLIC TYPES
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;

  private:
    // DATA
    bsl::vector<VALUE> d_vector;

  public:
    // CREATORS
    NonAllocCont() : d_vector(&bslma::MallocFreeAllocator::singleton()) {}

    ~NonAllocCont() {}

    // MANIPULATORS
    NonAllocCont& operator=(const NonAllocCont& rhs)
    {
        d_vector = rhs.d_vector;
        return *this;
    }

    reference back() { return d_vector.back(); }

    void pop_back() { d_vector.pop_back(); }

    void push_back(const value_type& value) { d_vector.push_back(value); }

    bsl::vector<value_type>& contents() { return d_vector; }

    // ACCESSORS
    bool operator==(const NonAllocCont& rhs) const
    {
        return d_vector == rhs.d_vector;
    }

    bool operator!=(const NonAllocCont& rhs) const
    {
        return !operator==(rhs);
    }

    bool operator<(const NonAllocCont& rhs) const
    {
        return d_vector < rhs.d_vector;
    }

    bool operator>=(const NonAllocCont& rhs) const
    {
        return !operator<(rhs);
    }

    bool operator>(const NonAllocCont& rhs) const
    {
        return d_vector > rhs.d_vector;
    }

    bool operator<=(const NonAllocCont& rhs) const
    {
        return !operator>(rhs);
    }

    const_reference back() const { return d_vector.back(); }

    size_type size() const { return d_vector.size(); }
};

namespace std {
template <class VALUE>
void swap(NonAllocCont<VALUE>& lhs, NonAllocCont<VALUE>& rhs)
{
    lhs.contents().swap(rhs.contents());
}
}  // close namespace std

template <class VALUE>
struct ValueName {
  private:
    // NOT IMPLEMENTED
    static const char *name();
        // Not implemented, so that an attempt to show the name of an
        // unrecognized type will result in failure to link.
};

template <>
struct ValueName<signed char> {
    static const char *name() { return "signed char"; }
};

template <>
struct ValueName<size_t> {
    static const char *name() { return "size_t"; }
};

template <>
struct ValueName<bsltf::TemplateTestFacility::ObjectPtr> {
    static const char *name() { return "TemplateTestFacility::ObjectPtr"; }
};

template <>
struct ValueName<bsltf::TemplateTestFacility::FunctionPtr> {
    static const char *name() { return "TemplateTestFacility::FunctionPtr"; }
};

template <>
struct ValueName<bsltf::TemplateTestFacility::MethodPtr> {
    static const char *name() { return "TemplateTestFacility::MethodPtr"; }
};

template <>
struct ValueName<bsltf::EnumeratedTestType::Enum> {
    static const char *name() { return "EnumeratedTestType::Enum"; }
};

template <>
struct ValueName<bsltf::UnionTestType> {
    static const char *name() { return "UnionTestType"; }
};

template <>
struct ValueName<bsltf::SimpleTestType> {
    static const char *name() { return "SimpleTestType"; }
};

template <>
struct ValueName<bsltf::AllocTestType> {
    static const char *name() { return "AllocTestType"; }
};

template <>
struct ValueName<bsltf::BitwiseMoveableTestType> {
    static const char *name() { return "BitwiseMoveableTestType"; }
};

template <>
struct ValueName<bsltf::AllocBitwiseMoveableTestType> {
    static const char *name() { return "AllocBitwiseMoveableTestType"; }
};

template <>
struct ValueName<bsltf::NonTypicalOverloadsTestType> {
    static const char *name() { return "NonTypicalOverloadsTestType"; }
};

template <class CONTAINER>
struct ContainerName {
    static const char *name();
};

template <class VALUE>
struct ContainerName<deque<VALUE> > {
    static const char *name()
    {
        static char buf[1000];
        strcpy(buf, "deque<");
        strcat(buf, ValueName<VALUE>::name());
        strcat(buf, ">");
        return buf;
    }
};

template <class VALUE>
struct ContainerName<vector<VALUE> > {
    static const char *name()
    {
        static char buf[1000];
        strcpy(buf, "vector<");
        strcat(buf, ValueName<VALUE>::name());
        strcat(buf, ">");
        return buf;
    }
};



bool expectToAllocate(int n)
    // Return 'true' if the container is expected to allocate memory on the
    // specified 'n'th element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

template<class CONTAINER, class VALUES>
void emptyNVerifyStack(stack<typename CONTAINER::value_type,
                             CONTAINER> *pmX,
                       const VALUES&     expectedValues,
                       size_t            expectedSize,
                       const int         LINE)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    const char *cont = ContainerName<CONTAINER>::name();
    const char *val  = ValueName<typename CONTAINER::value_type>::name();

    ASSERTV(cont, val, LINE, expectedSize, pmX->size(),
                                                  expectedSize == pmX->size());

    if (expectedSize != pmX->size()) {
        return;                                                       // RETURN
    }

    for (int i = static_cast<int>(expectedSize) - 1; i >= 0; --i) {
        if (expectedValues[i] != pmX->top()) P_(cont);
        ASSERTV(val, i, LINE, expectedValues[i], pmX->top(),
                                              expectedValues[i] == pmX->top());
        pmX->pop();
    }
}

template<class CONTAINER, class VALUES>
void verifyStack(const stack<typename CONTAINER::value_type,
                             CONTAINER>&  X,
                 const VALUES&            expectedValues,
                 size_t                   expectedSize,
                 const int                LINE,
                 bslma::Allocator         *allocator = 0)
{
    stack<typename CONTAINER::value_type, CONTAINER>
                                copyX(X, bslma::Default::allocator(allocator));
    emptyNVerifyStack(&copyX, expectedValues, expectedSize, LINE);
}

//=============================================================================
//                                   USAGE EXAMPLE
//=============================================================================

// Suppose a husband wants to keep track of chores his wife has asked him to
// do.  Over the years of being married, he has noticed that his wife generally
// wants the most recently requested task done first.  If she has a new task in
// mind that is low-priority, she will avoid asking for it until higher
// priority tasks are finished.  When he has finished all tasks, he is to
// report to his wife that he is ready for more.

// First, we define the class implementing the 'to-do' list.

class ToDoList {
    // DATA
    bsl::stack<const char *> d_stack;

  public:
    // MANIPULATORS
    void enqueueTask(const char *task);
        // Add the specified 'task', a string describing a task, to the
        // list.  Note the lifetime of the string referred to by 'task' must
        // exceed the lifetime of the task in this list.

    bool finishTask();
        // Remove the current task from the list.  Return 'true' if a task was
        // removed and it was the last task on the list, and return 'false'
        // otherwise.

    // ACCESSORS
    const char *currentTask() const;
        // Return the string representing the current task.  If there
        // is no current task, return the string "<EMPTY>", which is
        // not a valid task.
};

// MANIPULATORS
void ToDoList::enqueueTask(const char *task)
{
    d_stack.push(task);
}

bool ToDoList::finishTask()
{
    if (!d_stack.empty()) {
        d_stack.pop();

        return d_stack.empty();                                       // RETURN
    }

    return false;
};

// ACCESSORS
const char *ToDoList::currentTask() const
{
    if (d_stack.empty()) {
        return "<EMPTY>";                                             // RETURN
    }

    return d_stack.top();
}

//=============================================================================

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class OBJECT>
struct ExceptionGuard {
    // This class provide a mechanism to verify the strong exception guarantee
    // in exception-throwing code.  On construction, this class stores the
    // a copy of an object of the parameterized type 'OBJECT' and the address
    // of that object.  On destruction, if 'release' was not invoked, it will
    // verify the value of the object is the same as the value of the copy
    // create on construction.  This class requires the copy constructor and
    // 'operator ==' to be tested before use.

    // DATA
    int           d_line;      // the line number at construction
    OBJECT        d_copy;      // copy of the object being tested
    const OBJECT *d_object_p;  // address of the original object

  public:
    // CREATORS
    ExceptionGuard(const OBJECT    *object,
                   int              line,
                   bslma::Allocator *basicAllocator = 0)
    : d_line(line)
    , d_copy(*object, basicAllocator)
    , d_object_p(object)
        // Create the exception guard for the specified 'object' at the
        // specified 'line' number.  Optionally, specify 'basicAllocator' used
        // to supply memory.
    {}

    ~ExceptionGuard()
        // Destroy the exception guard.  If the guard was not released, verify
        // that the state of the object supplied at construction has not
        // change.
    {
        if (d_object_p) {
            const int LINE = d_line;
            ASSERTV(LINE, d_copy == *d_object_p);
        }
    }

    // MANIPULATORS
    void release()
        // Release the guard from verifying the state of the object.
    {
        d_object_p = 0;
    }
};

namespace {

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
    // increments a global counter used to keep track the method call count.
    // Object of this class can be identified by an id passed on construction.

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
            return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
                 < bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
                                                                      // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
                 > bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
                                                                      // RETURN
        }
    }

    bool operator== (const TestComparator& rhs) const
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
        return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
             > bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
    }
};

// FREE OPERATORS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
         < bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
}

}  // close unnamed namespace

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

template <class CONTAINER>
class TestDriver {
    // This templatized struct provide a namespace for testing the 'set'
    // container.  The parameterized 'KEY', 'COMP' and 'ALLOC' specifies the
    // value type, comparator type and allocator type respectively.  Each
    // "testCase*" method test a specific aspect of 'set<KEY, COMP, ALLOC>'.
    // Every test cases should be invoked with various parameterized type to
    // fully test the container.

  private:
    // TYPES
    typedef bsl::stack<typename CONTAINER::value_type, CONTAINER> Obj;
        // Type under testing.

    typedef typename Obj::value_type      value_type;
    typedef typename Obj::reference       reference;
    typedef typename Obj::const_reference const_reference;
    typedef typename Obj::size_type       size_type;
    typedef CONTAINER                     container_type;
        // Shorthands

    typedef bsltf::TestValuesArray<value_type> TestValues;

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

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static void emptyAndVerify(Obj               *obj,
                               const TestValues&  testValues,
                               size_t             numTestValues,
                               const int          LINE);
        // Pop the elements out of 'obj', verifying that they exactly match
        // the first 'numTestValues' elements in 'testValues'.

    static bool typeAlloc()
    {
        return  bslma::UsesBslmaAllocator<value_type>::value;
    }

    static bool emptyWillAlloc()
    {
        // Creating an empty 'deque' allocates memory, creating an empty
        // 'vector' does not.

        return bsl::is_same<CONTAINER, deque<value_type> >::value;
    }

  public:
                                  // test cases

    static void testCase12();
        // Test inequality operators

    static void testCase11();
        // Test type traits.

    static void testCase10();
        // Test bslma::Allocator.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8();
        // Test 'swap' member.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Reserved for (<<) operator.

    static void testCase4();
        // Test basic accessors ('size' and 'top').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators ('push' and 'pop').

    static void testCase1(int    *testKeys,
                          size_t  numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void testCase1_NoAlloc(int    *testValues,
                                  size_t  numValues);
        // Breathing test, except on a non-allocator container.  This test
        // *exercises* basic functionality but *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class CONTAINER>
int TestDriver<CONTAINER>::ggg(Obj        *object,
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

template <class CONTAINER>
bsl::stack<typename CONTAINER::value_type, CONTAINER>&
TestDriver<CONTAINER>::gg(Obj        *object,
                          const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class CONTAINER>
bsl::stack<typename CONTAINER::value_type, CONTAINER>
TestDriver<CONTAINER>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class CONTAINER>
void TestDriver<CONTAINER>::emptyAndVerify(Obj               *obj,
                                           const TestValues&  testValues,
                                           size_t             numTestValues,
                                           const int          LINE)
{
    ASSERTV(LINE, numTestValues, obj->size(), numTestValues == obj->size());

    for (int ti = static_cast<int>(numTestValues) - 1; ti >= 0; --ti) {
        ASSERTV(LINE, testValues[ti], obj->top(),testValues[ti] == obj->top());
        obj->pop();
    }

    ASSERTV(LINE, obj->size(), obj->empty());
    ASSERTV(LINE, obj->size(), 0 == obj->size());
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING INEQUALITY OPERATORS
    //
    // Concern:
    //   That the inequality operators function correctly.
    //
    // Plan:
    //   Load 2 stack objects according to two SPEC's via the 'ggg' function,
    //   and compare them.  It turns out that 'strcmp' comparing the two
    //   'SPEC's will correspond directly to the result of inequality
    //   operators, which is very convenient.
    //
    //   Repeat the test a second time, with the second stack object created
    //   with a different allocator than the first, to verify that creation
    //   via different allocators has no impact on value.
    // ------------------------------------------------------------------------

    const char *cont = ContainerName<container_type>::name();

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator ta("testA", veryVeryVeryVerbose);
    bslma::TestAllocator tb("testB", veryVeryVeryVerbose);

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) printf("    %s ---------------------------", cont);

    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPECX = DATA[ti].d_spec;

            Obj mX(&ta); const Obj& X = gg(&mX, SPECX);

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const char *const SPECY = DATA[tj].d_spec;

                Obj mY(&ta); const Obj& Y = gg(&mY, SPECY);

                const int CMP = ti == tj
                              ? 0
                              : strcmp(SPECX, SPECY) > 0 ? 1 : -1;

                const bool EQ = X == Y;
                const bool NE = X != Y;
                const bool LT = X <  Y;
                const bool LE = X <= Y;
                const bool GT = X >  Y;
                const bool GE = X >= Y;

                ASSERTV(cont, SPECX, SPECY, EQ == (Y == X));
                ASSERTV(cont, SPECX, SPECY, NE == (Y != X));
                ASSERTV(cont, SPECX, SPECY, LT == (Y >  X));
                ASSERTV(cont, SPECX, SPECY, LE == (Y >= X));
                ASSERTV(cont, SPECX, SPECY, GT == (Y <  X));
                ASSERTV(cont, SPECX, SPECY, GE == (Y <= X));

                ASSERTV(cont, SPECX, SPECY, LT == !GE);
                ASSERTV(cont, SPECX, SPECY, GT == !LE);

                ASSERTV(cont, SPECX, SPECY, !(LT && GT));
                ASSERTV(cont, SPECX, SPECY, LE || GE);

                if (0 == CMP) {
                    ASSERTV(cont, SPECX, SPECY, !LT && !GT);
                    ASSERTV(cont, SPECX, SPECY, LE  &&  GE);
                }
                else {
                    ASSERTV(cont, SPECX, SPECY, LT || GT);
                }

                ASSERTV(cont, SPECX, SPECY, CMP, (CMP < 0) == LT);
                ASSERTV(cont, SPECX, SPECY, CMP, (CMP < 0) == !GE);

                ASSERTV(cont, SPECX, SPECY, CMP, !((CMP == 0) && LT));
                ASSERTV(cont, SPECX, SPECY, CMP, !((CMP == 0) && GT));

                ASSERTV(cont, SPECX, SPECY, CMP, (CMP > 0) == GT);
                ASSERTV(cont, SPECX, SPECY, CMP, (CMP > 0) == !LE);

                ASSERTV(cont, SPECX, SPECY, CMP, (CMP == 0) == EQ);
                ASSERTV(cont, SPECX, SPECY, CMP, (CMP != 0) == NE);
            }

            // Do it all over again, this time using a different allocator
            // for 'mY' to verify changing the allocator has no impact on
            // comparisons.  Note we are re-testing the equality comparators
            // so this memory allocation aspect is tested for them too.

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const char *const SPECY = DATA[tj].d_spec;

                Obj mY(g(SPECY), &tb); const Obj& Y = mY;

                const int CMP = ti == tj
                              ? 0
                              : strcmp(SPECX, SPECY) > 0 ? 1 : -1;

                const bool EQ = X == Y;
                const bool NE = X != Y;
                const bool LT = X <  Y;
                const bool LE = X <= Y;
                const bool GT = X >  Y;
                const bool GE = X >= Y;

                ASSERTV(cont, SPECX, SPECY, EQ == (Y == X));
                ASSERTV(cont, SPECX, SPECY, NE == (Y != X));
                ASSERTV(cont, SPECX, SPECY, LT == (Y >  X));
                ASSERTV(cont, SPECX, SPECY, LE == (Y >= X));
                ASSERTV(cont, SPECX, SPECY, GT == (Y <  X));
                ASSERTV(cont, SPECX, SPECY, GE == (Y <= X));

                ASSERTV(cont, SPECX, SPECY, LT == !GE);
                ASSERTV(cont, SPECX, SPECY, GT == !LE);

                ASSERTV(cont, SPECX, SPECY, !(LT && GT));
                ASSERTV(cont, SPECX, SPECY, LE || GE);

                if (EQ) {
                    ASSERTV(cont, SPECX, SPECY, !LT && !GT);
                    ASSERTV(cont, SPECX, SPECY, LE  &&  GE);
                }
                else {
                    ASSERTV(cont, SPECX, SPECY, LT || GT);
                }

                ASSERTV(cont, SPECX, SPECY, CMP, (CMP < 0) == LT);
                ASSERTV(cont, SPECX, SPECY, CMP, (CMP < 0) == !GE);

                ASSERTV(cont, SPECX, SPECY, CMP, !((CMP == 0) && LT));
                ASSERTV(cont, SPECX, SPECY, CMP, !((CMP == 0) && GT));

                ASSERTV(cont, SPECX, SPECY, CMP, (CMP > 0) == GT);
                ASSERTV(cont, SPECX, SPECY, CMP, (CMP > 0) == !LE);

                ASSERTV(cont, SPECX, SPECY, CMP, (CMP == 0) == EQ);
                ASSERTV(cont, SPECX, SPECY, CMP, (CMP != 0) == NE);
            }
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase11()
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

    // Verify set defines the expected traits.

    enum { CONTAINER_USES_ALLOC =
                                 bslma::UsesBslmaAllocator<CONTAINER>::value };

    BSLMF_ASSERT(
        ((int) CONTAINER_USES_ALLOC == bslma::UsesBslmaAllocator<Obj>::value));

    // Verify stack does not define other common traits.

    BSLMF_ASSERT((0 == bslalg::HasStlIterators<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_copyable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseEqualityComparable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::IsBitwiseMoveable<Obj>::value));

    BSLMF_ASSERT((0 == bslmf::HasPointerSemantics<Obj>::value));

    BSLMF_ASSERT((0 == bsl::is_trivially_default_constructible<Obj>::value));
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase10()
{
    // ------------------------------------------------------------------------
    // TESTING BSLMA ALLOCATOR
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

    const char *cont = ContainerName<container_type>::name();

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    for (size_t ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(DATA[ti].d_spec);
        const TestValues  EXP(DATA[ti].d_spec, &scratch);

        TestValues values(SPEC, &scratch);

        bslma::TestAllocator ta("test",    veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  tam(&ta);

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bslma::TestAllocatorMonitor  dam(&da);

        {
            container_type tmpCont(&ta);
            for (size_t tk = 0; tk < LENGTH; ++tk) {
                tmpCont.push_back(values[tk]);
            }
            Obj mX(tmpCont, &ta);  const Obj& X = mX;

            verifyStack(X, EXP, LENGTH, L_, &ta);

            Obj mY(X, &ta);  const Obj& Y = mY;

            verifyStack(Y, EXP, LENGTH, L_, &ta);

            Obj mZ(&ta);  const Obj& Z = mZ;

            mZ.swap(mX);

            verifyStack(Z, EXP, LENGTH, L_, &ta);

            ASSERTV(LINE, X.empty());
            ASSERTV(LINE, 0 == X.size());
        }

        ASSERT(tam.isTotalUp() || 0 == LENGTH);
        ASSERT(tam.isInUseSame());

        tam.reset();

        {
            Obj mX(&ta);  const Obj& X = mX;
            for (size_t tj = 0; tj < LENGTH; ++tj) {
                mX.push(values[tj]);

                ASSERTV(LINE, tj, LENGTH, values[tj] == X.top());
            }

            verifyStack(X, EXP, LENGTH, L_, &ta);
        }

        ASSERT(tam.isTotalUp() || 0 == LENGTH);
        ASSERT(tam.isInUseSame());
        ASSERT(dam.isTotalSame());

        {
            container_type tmpCont;
            for (size_t tk = 0; tk < LENGTH; ++tk) {
                tmpCont.push_back(values[tk]);
            }
            Obj mX(tmpCont);  const Obj& X = mX;

            verifyStack(X, EXP, LENGTH, L_);

            Obj mY(X);  const Obj& Y = mY;

            verifyStack(Y, EXP, LENGTH, L_);

            Obj mZ;  const Obj& Z = mZ;

            mZ.swap(mX);

            verifyStack(Z, EXP, LENGTH, L_);

            ASSERTV(LINE, X.empty());
            ASSERTV(LINE, 0 == X.size());
        }

        ASSERTV(cont, dam.isTotalUp() == (emptyWillAlloc() || LENGTH > 0));

        dam.reset();

        {
            Obj mX;  const Obj& X = mX;
            for (size_t tj = 0; tj < LENGTH; ++tj) {
                mX.push(values[tj]);

                ASSERTV(LINE, tj, LENGTH, values[tj] == X.top());
            }

            verifyStack(X, EXP, LENGTH, L_);
        }

        ASSERTV(cont, dam.isTotalUp() == (emptyWillAlloc() || LENGTH > 0));

        ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
    }

}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase9()
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
    //: 2 The allocator address held by the target object is unchanged.
    //:
    //: 3 Any memory allocation is from the target object's allocator.
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
    //:       (using the 'bslma::TestAllocator_EXCEPTION_TEST_*' macros).
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
    //:     the 'bslma::TestAllocator_EXCEPTION_TEST_*' macros).  (C-9)
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
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
            Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(&scratch) == Z);
                firstFlag = false;
            }

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                    ASSERTV(LINE1, LINE2, (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z == X);
                        ASSERTV(LINE1, LINE2, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ == Z);

//                  ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
//                  ASSERTV(LINE1, LINE2, &scratch == Z.get_allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                                                     0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);        const Obj&  X  = gg(&mX,  SPEC1);
                Obj mZZ(&scratch);  const Obj& ZZ  = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ == Z);
                    ASSERTV(LINE1, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

//              ASSERTV(LINE1, &oa == Z.get_allocator());

                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase8()
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
    //: 2 The common object allocator address held by both objects is
    //:   unchanged.
    //:
    //: 3 If the two objects being swapped uses the same allocators, neither
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
    //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-6)
    //:
    //:     1 The value is unchanged.  (C-6)
    //:
    //:     2 The allocator address held by the object is unchanged.
    //:
    //:     3 There was no additional object memory allocation.
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
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
    //:         is unchanged in both objects.  (C-2)
    //:
    //:       3 There was no additional object memory allocation.
    //:
    //:     5 Create a new object allocator, 'oaz'
    //:
    //:     6 Repeat P-4.4.2 with 'oaz' under the presence of exception.
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-6)
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
    //   void swap(set& other);
    //   void swap(set<K, C, A>& a, set<K, C, A>& b);
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
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, &scratch);

        // Ensure the first row of the table contains the
        // default-constructed value.

        static bool firstFlag = true;
        if (firstFlag) {
            ASSERTV(LINE1, Obj() == W);
            firstFlag = false;
        }

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX == W);
//          ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX == W);
//          ASSERTV(LINE1, &oa == W.get_allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            Obj mX(XX, &oa);  const Obj& X = mX;

            Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, &scratch);

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, YY == X);
                ASSERTV(LINE1, LINE2, XX == Y);
//              ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
//              ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, XX == X);
                ASSERTV(LINE1, LINE2, YY == Y);
//              ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
//              ASSERTV(LINE1, LINE2, &oa == Y.get_allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

            Obj mZ(&oaz);  const Obj& Z = gg(&mZ, SPEC2);
            const Obj ZZ(Z, &scratch);

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    mX.swap(mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END


                ASSERTV(LINE1, LINE2, ZZ == X);
                ASSERTV(LINE1, LINE2, XX == Z);
//              ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
//              ASSERTV(LINE1, LINE2, &oaz == Z.get_allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, emptyWillAlloc()||oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, emptyWillAlloc() ||
                                                           oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);
                bslma::TestAllocatorMonitor oazm(&oaz);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guardX(&X, L_, &scratch);
                    ExceptionGuard<Obj> guardZ(&Z, L_, &scratch);

                    swap(mX, mZ);

                    guardX.release();
                    guardZ.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, LINE2, XX == X);
                ASSERTV(LINE1, LINE2, ZZ == Z);
//              ASSERTV(LINE1, LINE2, &oa == X.get_allocator());
//              ASSERTV(LINE1, LINE2, &oaz == Z.get_allocator());

                if (0 == X.size()) {
                    ASSERTV(LINE1, LINE2, emptyWillAlloc()||oam.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oam.isTotalUp());
                }

                if (0 == Z.size()) {
                    ASSERTV(LINE1, LINE2, emptyWillAlloc() ||
                                                           oazm.isTotalSame());
                }
                else {
                    ASSERTV(LINE1, LINE2, oazm.isTotalUp());
                }
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
        const Obj XX(X, &scratch);

        Obj mY(&oa);  const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY == X);
        ASSERTV(XX == Y);
        ASSERT(oam.isTotalSame());
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator) and created with the correct
    //:   capacity.
    //:
    //: 2 The value of the original object is left unaffected.
    //:
    //: 3 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 4 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original and change the capacity of the new object
    //:   correctly.
    //:
    //: 5 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 6 The function is exception neutral w.r.t. memory allocation.
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
    //:   'push'.  Using the 'operator!=' verify that y differs from x and
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

    const char *cont = ContainerName<container_type>::name();
    const char *val  = ValueName<value_type>::name();

    if (verbose) { P_(cont);  P_(val);  P(typeAlloc()); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::TestAllocator oa(veryVeryVerbose);

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

            // Create control object w.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);
            ASSERT(W == X);

            {   // Testing concern 1..4.
                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(&oa);
                gg(pX, SPEC);

                const Obj Y0(*pX);

                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);
//              ASSERTV(SPEC, Y0.get_allocator() ==
//                                         bslma::Default::defaultAllocator());

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj mY1(X);    const Obj& Y1 = mY1;

                mY1.push(VALUES['Z' - 'A']);

                ASSERTV(SPEC, Y1.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y1);
                ASSERTV(SPEC, X != Y1);
                ASSERTV(SPEC, W == X);
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                bslma::TestAllocatorMonitor dam(&da);
                bslma::TestAllocatorMonitor oam(&oa);

                Obj mY11(X, &oa);    const Obj& Y11 = mY11;

                ASSERT(dam.isTotalSame());
                ASSERTV(cont, LENGTH, oam.isTotalSame(), emptyWillAlloc(),
                      oam.isTotalSame() == (!emptyWillAlloc() && 0 == LENGTH));

                mY11.push(VALUES['Z' - 'A']);

                ASSERT(dam.isTotalSame());

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
//              ASSERTV(SPEC, Y11.get_allocator() == X.get_allocator());
            }
            {   // Exception checking.

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    bslma::TestAllocatorMonitor dam(&da);
                    bslma::TestAllocatorMonitor oam(&oa);

                    const Obj Y2(X, &oa);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                    }

                    ASSERT(dam.isTotalSame());
                    ASSERT(oam.isTotalUp() || (!emptyWillAlloc() &&
                                                0 == LENGTH));

                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, W == X);
//                  ASSERTV(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase6()
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

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = operator==;
        operatorPtr operatorNe = operator!=;

        (void) operatorEq;  // quash potential compiler warnings
        (void) operatorNe;
    }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);
    bslma::TestAllocatorMonitor  dam(&da);

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_spec);

            if (veryVerbose) { T_ P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(SPEC1,   X == X);
                ASSERTV(SPEC1, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_spec);

                if (veryVerbose) {
                              T_ T_ P(SPEC2) }

                const bool EXP = ti == tj;  // expected equality

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

                    ASSERTV(CONFIG, LENGTH1 == X.size());
                    ASSERTV(CONFIG, LENGTH2 == Y.size());

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(CONFIG,  EXP == (X == Y));
                    ASSERTV(CONFIG,  EXP == (Y == X));

                    ASSERTV(CONFIG, !EXP == (X != Y));
                    ASSERTV(CONFIG, !EXP == (Y != X));

                    ASSERTV(CONFIG, oaxm.isTotalSame());
                    ASSERTV(CONFIG, oaym.isTotalSame());
                }
            }
        }
    }

    ASSERT(dam.isTotalSame());
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase4()
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
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Default construct the object with various configuration:
    //:
    //:   2 Add in a series of objects.
    //:
    //:   3 Verify 'top' yields the expected result.
    //
    // Testing:
    //   const_iterator cbegin();
    //   const_iterator cend();
    //   size_type size() const;
    // ------------------------------------------------------------------------

    const char *cont = ContainerName<container_type>::name();
    const char *val  = ValueName<value_type>::name();

    if (verbose) { P_(cont);  P_(val);  P(typeAlloc()); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    static const struct {
        int         d_line;                     // source line number
        const char *d_spec;                     // specification string
        const char *d_results;                  // expected results
    } DATA[] = {
        //line  spec      result
        //----  --------  ------
        { L_,   "",       ""      },
        { L_,   "A",      "A"     },
        { L_,   "AB",     "AB"    },
        { L_,   "ABC",    "ABC"   },
        { L_,   "ABCD",   "ABCD"  },
        { L_,   "ABCDE",  "ABCDE" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                bslma::TestAllocator& oa = 'a' == CONFIG || 'b' == CONFIG
                                           ? da
                                           : 'c' == CONFIG
                                             ? sa1
                                             : sa2;
                bslma::TestAllocator& noa = &oa != &da ? da : sa1;

                bslma::TestAllocatorMonitor  oam(&oa);
                bslma::TestAllocatorMonitor noam(&noa);

                Obj& mX = 'a' == CONFIG
                          ? * new (fa) Obj()
                          : 'b' == CONFIG
                            ? * new (fa) Obj((bslma::Allocator *) 0)
                            : 'c' == CONFIG
                              ? * new (fa) Obj(&sa1)
                              : * new (fa) Obj(&sa2);

                ASSERT( oam.isTotalUp() == emptyWillAlloc());
                ASSERT(noam.isTotalSame());

                const Obj& X = gg(&mX, SPEC);
                ASSERT(&X == &mX);

                oam.reset();

                // --------------------------------------------------------
                // Verify basic accessors

//              ASSERTV(LINE, SPEC, CONFIG, &oa == X.get_allocator());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH == X.size());
                if (LENGTH > 0) {
                    ASSERTV(LINE, SPEC, CONFIG, EXP[LENGTH - 1] == mX.top());
                    ASSERTV(LINE, SPEC, CONFIG, EXP[LENGTH - 1] ==  X.top());
                }
                else {
                    bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

                    ASSERT_SAFE_FAIL(mX.top());
                }

                ASSERTV(LINE, LENGTH, X.empty(), (0 == LENGTH) == mX.empty());
                ASSERTV(LINE, LENGTH, X.empty(), (0 == LENGTH) ==  X.empty());

                ASSERT( oam.isTotalSame());
                ASSERT(noam.isTotalSame());

                fa.deleteObject(&mX);
            }
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase3()
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

    const char *cont = ContainerName<container_type>::name();
    const char *val  = ValueName<value_type>::name();

    if (verbose) { P_(cont);  P(val); }

    bslma::TestAllocator oa(veryVeryVerbose);
    bslma::TestAllocator da(veryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;                 // source line number
            const char *d_spec;                 // specification string
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const TestValues  EXP(DATA[ti].d_results);
            const int         curLen = (int)strlen(SPEC);

            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);

            LOOP3_ASSERT(oam.isTotalUp(), emptyWillAlloc(), LENGTH,
                         oam.isTotalUp() == (emptyWillAlloc() || LENGTH > 0));
            ASSERT(dam.isTotalSame());

            const Obj& Y =  g(     SPEC);

            ASSERT(&mX == &X);
            ASSERT(Y   == X);

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERTV(LINE, LENGTH == Y.size());
            emptyNVerifyStack(&mX,                   EXP, LENGTH, L_);
            emptyNVerifyStack(const_cast<Obj *>(&Y), EXP, LENGTH, L_);
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // control

            { L_,   "A",      -1,     }, // control
            { L_,   " ",       0,     },
            { L_,   ".",       0,     },
            { L_,   "E",       -1,    }, // control
            { L_,   "a",       0,     },
            { L_,   "z",       0,     },

            { L_,   "AE",     -1,     }, // control
            { L_,   "aE",      0,     },
            { L_,   "Ae",      1,     },
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",      0,     },

            { L_,   "ABC",    -1,     }, // control
            { L_,   " BC",     0,     },
            { L_,   "A C",     1,     },
            { L_,   "AB ",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",     0,     },

            { L_,   "ABCDE",  -1,     }, // control
            { L_,   "aBCDE",   0,     },
            { L_,   "ABcDE",   2,     },
            { L_,   "ABCDe",   4,     },
            { L_,   "AbCdE",   1,     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                 ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING CONSTRUCTORS AND PRIMARY MANIPULATORS (BOOTSTRAP):
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
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory (only true if the
    //:   underlying container is 'vector').
    //:
    //:10 'insert' adds an additional element to the object if the element
    //:   being inserted does not already exist.
    //:
    //:11 'push' pushes a sequence of objects into the stack, and 'pop' will
    //:   recover those same values in reverse order.
    //:
    //:12 Any argument can be 'const'.
    //:
    //:13 Any memory allocation is exception neutral.
    //:
    //:14 All version of the copy c'tor produce an object with the same value
    //:   as the original, and allocate memory from the appropriate allocator.
    //:
    //:15 All versions of the c'tor from container produce an object with the
    //:   appropriate value, and allocate memory from the appropriate
    //:   allocator.
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
    //:     9 Make a copy of the object using the appropriate copy c'tor.
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (Implicit in test allocator).  (C-8)
    //:    11 Verify that calling 'pop' on an empty stack will fail an assert
    //:       in safe mode.
    //
    // Testing:
    //   stack;
    //   stack(bslma_Allocator *);
    //   ~stack();
    //   push(const value_type& value);
    //   stack(const CONTAINER& container, bslma_allocator *);
    //   stack(const stack& stack, bslma_allocator *);
    // ------------------------------------------------------------------------

    const char *cont = ContainerName<container_type>::name();
    const char *val  = ValueName<value_type>::name();

    if (verbose) { P_(cont);  P_(val);  P(typeAlloc()); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

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

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objPtr = new (fa) Obj((bslma::Allocator *) 0);
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
                return;                                               // RETURN
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = 'c' == CONFIG ? sa : da;

            // Verify any attribute allocators are installed properly.

//          ASSERTV(LENGTH, CONFIG, &oa == X.get_allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG, emptyWillAlloc() == !!oa.numBlocksTotal());

            ASSERTV(LENGTH, CONFIG, 0 == X.size());
            ASSERTV(LENGTH, CONFIG, X.empty());

            {
                bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

                ASSERT_SAFE_FAIL(mX.pop());
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'push' (bootstrap).\n"); }

            for (size_t tj = 0; tj + 1 < LENGTH; ++tj) {
                mX.push(VALUES[tj]);
                ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] == mX.top());
                ASSERTV(LENGTH, tj, CONFIG, VALUES[tj] ==  X.top());
            }

            if (LENGTH > 1) {
                ASSERTV(CONFIG, oa.numBlocksTotal() > 0);
            }

            if (0 < LENGTH) {
                ASSERTV(LENGTH, CONFIG, LENGTH - 1 == X.size());

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                // insert the last element with an exception guard

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    mX.push(VALUES[LENGTH - 1]);
                    guard.release();

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    if (1 == LENGTH && !typeAlloc()) {
                        // If the vector grows, the old vector will be
                        // deallocated, so only do this test on '1 == LENGTH'.

                        ASSERTV(LENGTH, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                   oa.numBlocksTotal() == oa.numBlocksInUse());
                    }

                    ASSERTV(LENGTH, CONFIG, VALUES[LENGTH - 1] == mX.top());
                    ASSERTV(LENGTH, CONFIG, VALUES[LENGTH - 1] ==  X.top());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LENGTH, CONFIG, LENGTH == X.size());
            }

            // Test copy c'tors
            {
                bslma::TestAllocatorMonitor oaMonitor(&oa);

                Obj *copyPtr;
                switch (CONFIG) {
                  case 'a': {
                    copyPtr = new (fa) Obj(X);
                  } break;
                  case 'b': {
                    copyPtr = new (fa) Obj(X, (bslma::Allocator *) 0);
                  } break;
                  case 'c': {
                    copyPtr = new (fa) Obj(X, &sa);
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                    return;                                           // RETURN
                  } break;
                }

                ASSERT(X == *copyPtr);

                ASSERT((0 < LENGTH || emptyWillAlloc()) ==
                                                        oaMonitor.isTotalUp());

                emptyAndVerify(copyPtr, VALUES, LENGTH, L_);
                fa.deleteObject(copyPtr);
            }

            // Test container c'tors
            {
                bslma::TestAllocatorMonitor oaMonitor(&oa);
                bslma::TestAllocator ca;


                CONTAINER c(&ca);    const CONTAINER& C = c;

                // We have to insert the values one at a time, 'vector' has a
                // problem with range inserts of function ptrs.

                for (size_t tk = 0; tk < LENGTH; ++tk) {
                    c.push_back(VALUES[tk]);
                }

                Obj *cCopyPtr;
                switch (CONFIG) {
                  case 'a': {
                    cCopyPtr = new (fa) Obj(C);
                  } break;
                  case 'b': {
                    cCopyPtr = new (fa) Obj(C, (bslma::Allocator *) 0);
                  } break;
                  case 'c': {
                    cCopyPtr = new (fa) Obj(C, &sa);
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                    return;                                           // RETURN
                  } break;
                }

                ASSERT(X == *cCopyPtr);

                ASSERT((0 < LENGTH || emptyWillAlloc()) ==
                                                        oaMonitor.isTotalUp());

                if ('a' == CONFIG) {
                    // Sometimes don't do this, just so we test the case where
                    // we destroy a non-empty object.

                    emptyAndVerify(cCopyPtr, VALUES, LENGTH, L_);
                }
                fa.deleteObject(cCopyPtr);
            }

            emptyAndVerify(&mX, VALUES, LENGTH, L_);

            if (&oa != &da) {
                ASSERT(0 == da.numBlocksTotal());
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase1_NoAlloc(int    *testValues,
                                              size_t  numValues)
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
    //: 1 Execute each method to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Default construct an empty set.\n");
    }
    {
        Obj x; const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(true == X.empty());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test use of allocators.\n");
    }
    {
        Obj o1; const Obj& O1 = o1;

        for (size_t i = 0; i < numValues; ++i) {
            o1.push(value_type(testValues[i]));
        }
        ASSERTV(numValues == O1.size());

        Obj o2(O1); const Obj& O2 = o2;

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());

        Obj o3; const Obj& O3 = o3;

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0         == O3.size());

        o1.swap(o3);
        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());

        ASSERTV(0         == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(numValues == O3.size());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    native_std::sort(testValues, testValues + numValues);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x; const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X); const Obj& Y = y;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            // Test 'insert'.
            value_type value(testValues[i]);
            x.push(value);
            ASSERTV(testValues[i] == x.top());

            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(false == X.empty());

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(X.size() == numValues);

        for (int i = static_cast<int>(numValues) - 1; i >= 0; --i) {
            testValues[i] = X.top();
            x.pop();
        }

        ASSERTV(X.size() == 0);
    } while (native_std::next_permutation(testValues,
                                          testValues + numValues));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test class comparison operators.\n");
    }
    {
        // Iterate over possible selections of elements to add to two
        // containers, 'X' and 'Y' then compare the results of the comparison
        // operators to the comparison between two containers equivalent to
        // the underlying containers in the stack objects.

        for (size_t i = 0; i < numValues; ++i) {
            for (size_t j = 0; j < numValues; ++j) {
                for (size_t length = 0; length < numValues; ++length) {
                    for (size_t m = 0; m < j; ++m) {
                        Obj x; const Obj& X = x;
                        Obj y; const Obj& Y = y;
                        CONTAINER xx;
                        const CONTAINER& XX = xx;
                        CONTAINER yy;
                        const CONTAINER& YY = yy;
                        for (size_t k = 0; k < j; ++k) {
                            size_t xIndex = (i + length) % numValues;
                            size_t yIndex = (j + length) % numValues;

                            x.push(      testValues[xIndex]);
                            xx.push_back(testValues[xIndex]);

                            if (k < m) {
                                y.push(      testValues[yIndex]);
                                yy.push_back(testValues[yIndex]);
                            }
                        }

                        ASSERTV((X == Y) == (XX == YY));
                        ASSERTV((X != Y) == (XX != YY));
                        ASSERTV((X <  Y) == (XX <  YY));
                        ASSERTV((X >  Y) == (XX >  YY));
                        ASSERTV((X <= Y) == (XX <= YY));
                        ASSERTV((X >= Y) == (XX >= YY));

                        ASSERTV((X == Y) == !(X != Y));
                        ASSERTV((X != Y) == !(X == Y));
                        ASSERTV((X <  Y) == !(X >= Y));
                        ASSERTV((X >  Y) == !(X <= Y));
                        ASSERTV((X <= Y) == !(X >  Y));
                        ASSERTV((X >= Y) == !(X <  Y));

                        ASSERTV((Y == X) == (YY == XX));
                        ASSERTV((Y != X) == (YY != XX));
                        ASSERTV((Y <  X) == (YY <  XX));
                        ASSERTV((Y >  X) == (YY >  XX));
                        ASSERTV((Y <= X) == (YY <= XX));
                        ASSERTV((Y >= X) == (YY >= XX));

                        ASSERTV((Y == X) == !(Y != X));
                        ASSERTV((Y != X) == !(Y == X));
                        ASSERTV((Y <  X) == !(Y >= X));
                        ASSERTV((Y >  X) == !(Y <= X));
                        ASSERTV((Y <= X) == !(Y >  X));
                        ASSERTV((Y >= X) == !(Y <  X));
                    }
                }
            }
        }
    }
}

template <class CONTAINER>
void TestDriver<CONTAINER>::testCase1(int    *testValues,
                                      size_t  numValues)
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
    //: 1 Execute each method to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    bslma::TestAllocator defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator");

    // Sanity check.

    ASSERTV(0 < numValues);
    ASSERTV(8 > numValues);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Default construct an empty set.\n");
    }
    {
        Obj x(&objectAllocator); const Obj& X = x;
        ASSERTV(0    == X.size());
        ASSERTV(true == X.empty());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(emptyWillAlloc() == (0 != objectAllocator.numBytesInUse()));
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test use of allocators.\n");
    }
    {
        bslma::TestAllocatorMonitor defaultMonitor(&defaultAllocator);

        bslma::TestAllocator objectAllocator1("objectAllocator1");
        bslma::TestAllocator objectAllocator2("objectAllocator2");

        Obj o1(&objectAllocator1); const Obj& O1 = o1;
//      ASSERTV(&objectAllocator1 == O1.get_allocator().mechanism());

        for (size_t i = 0; i < numValues; ++i) {
            o1.push(value_type(testValues[i]));
        }
        ASSERTV(numValues == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());

        Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
//      ASSERTV(&objectAllocator2 == O2.get_allocator().mechanism());

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0 < objectAllocator1.numBytesInUse());
        ASSERTV(0 < objectAllocator2.numBytesInUse());

        Obj o3(&objectAllocator1); const Obj& O3 = o3;
//      ASSERTV(&objectAllocator1 == O3.get_allocator().mechanism());

        bslma::TestAllocatorMonitor monitor1(&objectAllocator1);

        ASSERTV(numValues == O1.size());
        ASSERTV(numValues == O2.size());
        ASSERTV(0         == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

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
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 <  objectAllocator2.numBytesInUse());

//      ASSERTV(&objectAllocator1 == O1.get_allocator().mechanism());
//      ASSERTV(&objectAllocator2 == O2.get_allocator().mechanism());
//      ASSERTV(&objectAllocator1 == O3.get_allocator().mechanism());

        ASSERTV(! defaultMonitor.isTotalUp());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    native_std::sort(testValues, testValues + numValues);
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        bslma::TestAllocatorMonitor defaultMonitor(&defaultAllocator);
        Obj x(&objectAllocator); const Obj& X = x;
        for (size_t i = 0; i < numValues; ++i) {
            Obj y(X, &objectAllocator); const Obj& Y = y;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            // Test 'insert'.
            value_type value(testValues[i]);
            x.push(value);
            ASSERTV(testValues[i] == x.top());

            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(false == X.empty());

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(X.size() == numValues);

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());

        for (int i = static_cast<int>(numValues) - 1; i >= 0; --i) {
            testValues[i] = (int) X.top();
            x.pop();
        }

        ASSERTV(X.size() == 0);
        ASSERTV(! defaultMonitor.isTotalUp());
    } while (native_std::next_permutation(testValues,
                                          testValues + numValues));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test class comparison operators.\n");
    }
    {
        // Iterate over possible selections of elements to add to two
        // containers, 'X' and 'Y' then compare the results of the comparison
        // operators to the comparison between two containers equivalent to
        // the underlying containers in the stack objects.

        for (size_t i = 0; i < numValues; ++i) {
            for (size_t j = 0; j < numValues; ++j) {
                for (size_t length = 0; length < numValues; ++length) {
                    for (size_t m = 0; m < j; ++m) {
                        Obj x(&objectAllocator); const Obj& X = x;
                        Obj y(&objectAllocator); const Obj& Y = y;
                        CONTAINER xx(&objectAllocator);
                        const CONTAINER& XX = xx;
                        CONTAINER yy(&objectAllocator);
                        const CONTAINER& YY = yy;
                        for (size_t k = 0; k < j; ++k) {
                            size_t xIndex = (i + length) % numValues;
                            size_t yIndex = (j + length) % numValues;

                            x.push(      testValues[xIndex]);
                            xx.push_back(testValues[xIndex]);

                            if (k < m) {
                                y.push(      testValues[yIndex]);
                                yy.push_back(testValues[yIndex]);
                            }
                        }

                        ASSERTV((X == Y) == (XX == YY));
                        ASSERTV((X != Y) == (XX != YY));
                        ASSERTV((X <  Y) == (XX <  YY));
                        ASSERTV((X >  Y) == (XX >  YY));
                        ASSERTV((X <= Y) == (XX <= YY));
                        ASSERTV((X >= Y) == (XX >= YY));

                        ASSERTV((X == Y) == !(X != Y));
                        ASSERTV((X != Y) == !(X == Y));
                        ASSERTV((X <  Y) == !(X >= Y));
                        ASSERTV((X >  Y) == !(X <= Y));
                        ASSERTV((X <= Y) == !(X >  Y));
                        ASSERTV((X >= Y) == !(X <  Y));

                        ASSERTV((Y == X) == (YY == XX));
                        ASSERTV((Y != X) == (YY != XX));
                        ASSERTV((Y <  X) == (YY <  XX));
                        ASSERTV((Y >  X) == (YY >  XX));
                        ASSERTV((Y <= X) == (YY <= XX));
                        ASSERTV((Y >= X) == (YY >= XX));

                        ASSERTV((Y == X) == !(Y != X));
                        ASSERTV((Y != X) == !(Y == X));
                        ASSERTV((Y <  X) == !(Y >= X));
                        ASSERTV((Y >  X) == !(Y <= X));
                        ASSERTV((Y <= X) == !(Y >  X));
                        ASSERTV((Y >= X) == !(Y <  X));
                    }
                }
            }
        }
    }
}

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

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 16: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //   Demonstrate the use of the stack.
        //
        // Plan:
        //   Create the class 'ToDoList', which implements a list of chores to
        //   be done, using the 'stack' container adapter, and demonstrate the
        //   use of 'ToDoList'.
        // --------------------------------------------------------------------

        // Then, create an object of type 'ToDoList'.

        ToDoList toDoList;

        // Next, a few tasks are requested:

        toDoList.enqueueTask("Change the car's oil.");
        toDoList.enqueueTask("Pay the bills.");

        // Then, the husband watches the Yankee's game on TV.  Upon returning
        // to the list he consults the list to see what task is up next:

        ASSERT(!strcmp("Pay the bills.", toDoList.currentTask()));

        // Next, he sees that he has to pay the bills.  When the bills are
        // finished, he flushes that task from the list:

        ASSERT(false == toDoList.finishTask());

        // Then, he consults the list for the next task.

        ASSERT(!strcmp("Change the car's oil.", toDoList.currentTask()));

        // Next, he sees he has to change the car's oil.  Before he can get
        // started, another request comes:

        toDoList.enqueueTask("Get some hot dogs.");
        ASSERT(!strcmp("Get some hot dogs.", toDoList.currentTask()));

        // Then, he drives the car to the convenience store and picks up some
        // hot dogs and buns.  Upon returning home, he gives the hot dogs to
        // his wife, updates the list, and consults it for the next task.

        ASSERT(false == toDoList.finishTask());
        ASSERT(!strcmp("Change the car's oil.", toDoList.currentTask()));

        // Next, he finishes the oil change, updates the list, and consults it
        // for the next task.

        ASSERT(true == toDoList.finishTask());
        ASSERT(!strcmp("<EMPTY>", toDoList.currentTask()));

        // Finally, the wife has been informed that everything is done, and she
        // makes another request:

        toDoList.enqueueTask("Clean the rain gutters.");
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING EMPTY, SIZE
        //
        // Concern:
        //   That the 'empty()' and 'size()' accessors work according to their
        //   specifications.
        //
        // Plan:
        //   Manipulate a 'stack' object, and observe that 'empty()' and
        //   'size()' return the expected values.
        // --------------------------------------------------------------------

        stack<int> mX;    const stack<int>& X = mX;

        ASSERT(mX.empty());         ASSERT(X.empty());
        ASSERT(0 == mX.size());     ASSERT(0 == X.size());

        for (int i = 7; i < 22; ++i) {
            mX.push(i);
            ASSERT(! mX.empty());              ASSERT(! X.empty());
            ASSERT(i - 6 == (int) mX.size());  ASSERT(i - 6 == (int) X.size());
            ASSERT(i == X.top());

            mX.top() = static_cast<int>(X.size());  // 'top()' returns a ref to
                                                    // modifiable
            ASSERT((int) X.size() == X.top());
        }

        for (size_t i = X.size(); i > 0; --i, mX.pop()) {
            ASSERT(! mX.empty());             ASSERT(! X.empty());
            ASSERT(i == X.size());
            ASSERT(X.top() == static_cast<int>(i));
        }

        ASSERT(mX.empty());         ASSERT(X.empty());
        ASSERT(0 == mX.size());     ASSERT(0 == X.size());
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING NON ALLOCATOR SUPPORTING TYPE
        // --------------------------------------------------------------------

        typedef stack<int, NonAllocCont<int> > IStack;

        IStack mX;    const IStack& X = mX;

        ASSERT(X.empty());

        mX.push(3);
        mX.push(4);
        mX.push(5);

        ASSERT(! X.empty());
        ASSERT(3 == X.size());
        ASSERT(5 == X.top());

        IStack mY(X);   const IStack& Y = mY;

        ASSERT(X == Y);         ASSERT(!(X != Y));
        ASSERT(X <= Y);         ASSERT(!(X >  Y));
        ASSERT(X >= Y);         ASSERT(!(X <  Y));

        mY.pop();
        mY.push(6);

        ASSERT(X != Y);         ASSERT(!(X == Y));
        ASSERT(X <  Y);         ASSERT(!(X >= Y));
        ASSERT(X <= Y);         ASSERT(!(X >  Y));
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING CONTAINER OVERRIDE
        // --------------------------------------------------------------------

        // Verify that a stack with no container specified is the same as one
        // we 'deque' specified.

        typedef stack<int>                IStack;
        typedef stack<int, deque<int> >   IDStack;

        BSLMF_ASSERT((bsl::is_same<IStack, IDStack>::value));

        // Verify that if a container is specified, the first template
        // argument is ignored.

        typedef stack<void,   vector<int> > VIVStack;
        typedef stack<double, vector<int> > DIVStack;

        BSLMF_ASSERT((bsl::is_same<VIVStack::value_type, int>::value));
        BSLMF_ASSERT((bsl::is_same<DIVStack::value_type, int>::value));

        VIVStack vivs;          const VIVStack& VIVS = vivs;

        vivs.push(4);           ASSERT(4 == VIVS.top());
        vivs.push(7);           ASSERT(7 == VIVS.top());
        ASSERT(2 == VIVS.size());
        ASSERT(!VIVS.empty());
        vivs.pop();             ASSERT(4 == VIVS.top());
      }  break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INEQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Inequality Operators\n"
                            "\n============================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase12,
                                        TEST_TYPES_INEQUAL_COMPARABLE(deque));

        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase12,
                                        TEST_TYPES_INEQUAL_COMPARABLE(vector));
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Type Traits\n"
                            "\n===================\n");

        // Verify the bslma-allocator trait is not defined for non
        // bslma-allocators.

        typedef bsltf::StdTestAllocator<bsltf::AllocTestType> StlAlloc;

        typedef bsltf::AllocTestType ATT;

        typedef deque< ATT, StlAlloc> WeirdAllocDeque;
        typedef vector<ATT, StlAlloc> WeirdAllocVector;

        typedef bsl::stack<ATT, WeirdAllocDeque >   WeirdAllocDequeStack;
        typedef bsl::stack<ATT, WeirdAllocVector>   WeirdAllocVectorStack;
        typedef bsl::stack<int, NonAllocCont<int> > NonAllocStack;

        if (verbose) printf("NonAllocCont --------------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                 NonAllocCont<int> >::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                 NonAllocStack>::value));
        TestDriver<NonAllocCont<int> >::testCase11();

        if (verbose) printf("deque ---------------------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                 WeirdAllocDeque>::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                 WeirdAllocDequeStack>::value));
        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR(deque));

        if (verbose) printf("vector --------------------------------------\n");
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                 WeirdAllocVector>::value));
        BSLMF_ASSERT((0 == bslma::UsesBslmaAllocator<
                                 WeirdAllocVectorStack>::value));
        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR(vector));
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STL ALLOCATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting STL ALLOCTOR\n"
                            "\n====================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase10,TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase10,TEST_TYPES_REGULAR(vector));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES_REGULAR(vector));
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase8, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase8, TEST_TYPES_REGULAR(vector));
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES_REGULAR(vector));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES_REGULAR(vector));
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES_REGULAR(vector));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase3, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase3, TEST_TYPES_REGULAR(vector));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting C'tors and Primary Manipulators\n"
                              "=======================================\n");

        if (verbose) printf("deque ---------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES_REGULAR(deque));
        if (verbose) printf("vector --------------------------------------\n");
        RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES_REGULAR(vector));
        if (verbose) printf("NonAllocCont --------------------------------\n");
//      RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES_REGULAR(NonAllocCont));
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
        {
            int INT_VALUES[]   = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
            int NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

            if (verbose) printf("deque:\n");
            TestDriver<bsl::deque<int>  >::testCase1(INT_VALUES,
                                                     NUM_INT_VALUES);
            if (verbose) printf("vector:\n");
            TestDriver<bsl::vector<int> >::testCase1(INT_VALUES,
                                                     NUM_INT_VALUES);

            if (verbose) printf("deque<double>:\n");
            TestDriver<bsl::deque<double> >::testCase1(INT_VALUES,
                                                       NUM_INT_VALUES);

            if (verbose) printf("NonAllocCont<int>:\n");
            TestDriver<NonAllocCont<int> >::testCase1_NoAlloc(INT_VALUES,
                                                              NUM_INT_VALUES);

#if 0
            // add once 'list' is in bslstl, add it

            if (verbose) printf("list:\n");
            TestDriver<bsl::list<int>   >::testCase1(INT_VALUES,
                                                     NUM_INT_VALUES);
#endif
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
