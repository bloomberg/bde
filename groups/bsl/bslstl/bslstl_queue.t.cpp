// bslstl_queue.t.cpp                                                 -*-C++-*-

#include <bslstl_queue.h>

#include <bslstl_allocator.h>
#include <bslstl_deque.h>
#include <bslstl_iterator.h>
#include <bslstl_forwarditerator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_newdeleteallocator.h>
#include <bslma_mallocfreeallocator.h>
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatormonitor.h>    // for testing only
#include <bslma_testallocatorexception.h>  // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_stopwatch.h>                // for testing only
#include <bsls_util.h>

#include <algorithm>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>
#include <bsltf_stdtestallocator.h>

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
// The component under test has two value-semantic container adaptors whose
// states are represented by their underlying containers.
//
// The primary manipulators for 'bsl::queue' are the default constructor,
// 'push', and 'pop'.  The basic accessors are 'size' , 'front', and 'back'.
//
// The primary manipulators for 'bsl::priority_queue' are the default
// constructor, 'push', and 'pop'.  The basic accessors are 'size' and 'top'.
// ----------------------------------------------------------------------------
// CLASS 'bsl::queue'
//
// CREATORS
// [ 2]explicit queue();
// [12]explicit queue(const CONTAINER& container);
// [ 7]queue(const queue& original);
// [ 2]explicit queue(const ALLOCATOR& allocator);
// [12]queue(const CONTAINER& container, const ALLOCATOR& allocator);
// [ 7]queue(const queue& original, const ALLOCATOR& allocator);
//
// MANIPULATORS
// [ 9]queue& operator=(const queue& rhs);
// [ 2]void push(const value_type& value);
// [ 2]void pop();
// [ 8]void swap(queue& other);
// [13]reference front();
// [13]reference back();
//
// ACCESSORS
// [14]bool empty() const;
// [ 4]size_type size() const;
// [ 4]const_reference front() const;
// [ 4]const_reference back() const;

// FREE FUNCTIONS
// [ 6]bool operator==(const queue& lhs, const queue& rhs);
// [ 6]bool operator!=(const queue& lhs, const queue& rhs);
// [15]bool operator< (const queue& lhs, const queue& rhs);
// [15]bool operator> (const queue& lhs, const queue& rhs);
// [15]bool operator>=(const queue& lhs, const queue& rhs);
// [15]bool operator<=(const queue& lhs, const queue& rhs);
//
// specialized algorithms:
// [ 8]void swap(queue& lhs,queue& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [16] TESTING NON ALLOCATOR SUPPORTING TYPE
// [17] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] ggg(queue<V,C> *object, const char *spec, int verbose = 1);
// [ 3] gg(queue<V,C> *object, const char *spec);
// [ 3] ggg(priority_queue<V,C,P> *object, const char *spec, int verbose = 1);
// [ 3] gg(priority_queue<V,C,P> *object, const char *spec);
// [11] queue<V,C> g(const char *spec);
// [11] priority_queue<V,C,P> g(const char *spec);
//
// [ 5] TESTING OUTPUT: Not Applicable
// [10] STREAMING: Not Applicable
// [**] CONCERN: The object is compatible with STL allocator.

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

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P_(I) P(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P_(I) P_(J) P(K) \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { P_(I) P_(J) P_(K) P(L)\
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

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

const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';

// Define default data for testing 'bsl::queue'.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line index  spec
    //---- -----  ----
    { L_,      0,  "",                 },
    { L_,      1,  "A",                },
    { L_,     21,  "B",                },
    { L_,      2,  "AA",               },
    { L_,      4,  "AB",               },
    { L_,     22,  "BA",               },
    { L_,     17,  "AC",               },
    { L_,     31,  "CD",               },
    { L_,      5,  "ABC",              },
    { L_,     18,  "ACB",              },
    { L_,     23,  "BAC",              },
    { L_,     25,  "BCA",              },
    { L_,     29,  "CAB",              },
    { L_,     30,  "CBA",              },
    { L_,     24,  "BAD",              },
    { L_,      6,  "ABCA",             },
    { L_,      8,  "ABCB",             },
    { L_,      9,  "ABCC",             },
    { L_,     10,  "ABCD",             },
    { L_,     19,  "ACBD",             },
    { L_,     27,  "BDCA",             },
    { L_,     33,  "DCBA",             },
    { L_,     28,  "BEAD",             },
    { L_,     26,  "BCDE",             },
    { L_,     11,  "ABCDE",            },
    { L_,     20,  "ACBDE",            },
    { L_,     32,  "CEBDA",            },
    { L_,     35,  "EDCBA",            },
    { L_,     36,  "FEDCB",            },
    { L_,     37,  "FEDCBA",           },
    { L_,      7,  "ABCABC",           },
    { L_,      3,  "AABBCC",           },
    { L_,     12,  "ABCDEFG",          },
    { L_,     13,  "ABCDEFGH",         },
    { L_,     14,  "ABCDEFGHI",        },
    { L_,     15,  "ABCDEFGHIJKLMNOP", },
    { L_,     38,  "PONMLKJIGHFEDCBA", },
    { L_,     16,  "ABCDEFGHIJKLMNOPQ",},
    { L_,     34,  "DHBIMACOPELGFKNJQ",},
};

static
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

typedef bsltf::AllocTestType TestValueType;

int SPECIAL_INT_VALUES[]       = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
int NUM_SPECIAL_INT_VALUES     =
                      sizeof(SPECIAL_INT_VALUES) / sizeof(*SPECIAL_INT_VALUES);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE
//#define TEST_TYPES_REGULAR BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR

#ifndef BSLS_PLATFORM_OS_WINDOWS
# define TEST_TYPES_REGULAR                                                   \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr,                             \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType,                                  \
        bsltf::NonTypicalOverloadsTestType
#else
# define TEST_TYPES_REGULAR                                                   \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::SimpleTestType,                                                \
        bsltf::AllocTestType,                                                 \
        bsltf::BitwiseMoveableTestType,                                       \
        bsltf::AllocBitwiseMoveableTestType,                                  \
        bsltf::NonTypicalOverloadsTestType
#endif


// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val);
    fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val);
    fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val);
    fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val);
    fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val);
    fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

// 'queue' specific print function

template <class VALUE, class CONTAINER>
void dbg_print(const bsl::queue<VALUE, CONTAINER>& q)
{
    if (q.empty()) {
        printf("<empty>");
    }
    else {
        printf("size: %d, front: ", (int) q.size());
        dbg_print(static_cast<char>(
                       bsltf::TemplateTestFacility::getIdentifier(q.front())));
        printf(", back: ");
        dbg_print(static_cast<char>(
                        bsltf::TemplateTestFacility::getIdentifier(q.back())));
    }
    fflush(stdout);
}

// generic debug print function (3-arguments)
template <class T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

                            // ==================
                            // class NonAllocCont
                            // ==================

template <class VALUE>
class NonAllocCont {
  private:
    // DATA
    bsl::deque<VALUE> d_deque;

  public:
    // PUBLIC TYPES
    typedef VALUE        value_type;
    typedef VALUE&       reference;
    typedef const VALUE& const_reference;
    typedef std::size_t  size_type;

    // CREATORS
    NonAllocCont() : d_deque(&bslma::MallocFreeAllocator::singleton()) {}

    ~NonAllocCont() {}

    // MANIPULATORS
    NonAllocCont& operator=(const NonAllocCont& other)
    {
        d_deque = other.d_deque;
        return *this;
    }

    reference front() { return d_deque.front(); }

    reference back() { return d_deque.back(); }

    void pop_front() { d_deque.pop_front(); }

    void push_back(const value_type& value) { d_deque.push_back(value); }

    bsl::deque<value_type>& contents() { return d_deque; }

    // ACCESSORS
    bool operator==(const NonAllocCont& rhs) const
    {
        return d_deque == rhs.d_deque;
    }

    bool operator!=(const NonAllocCont& rhs) const
    {
        return !operator==(rhs);
    }

    bool operator<(const NonAllocCont& rhs) const
    {
        return d_deque < rhs.d_deque;
    }

    bool operator>=(const NonAllocCont& rhs) const
    {
        return !operator<(rhs);
    }

    bool operator>(const NonAllocCont& rhs) const
    {
        return d_deque > rhs.d_deque;
    }

    bool operator<=(const NonAllocCont& rhs) const
    {
        return !operator>(rhs);
    }

    const_reference front() const { return d_deque.front(); }

    const_reference back() const { return d_deque.back(); }

    size_type size() const { return d_deque.size(); }

    bool empty() const { return d_deque.empty(); }
};

namespace std {
    template <class VALUE>
    void swap(NonAllocCont<VALUE>& lhs, NonAllocCont<VALUE>& rhs)
    {
        lhs.contents().swap(rhs.contents());
    }
}  // close namespace std

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


//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                            // ----------------
                            // class TestDriver
                            // ----------------

template <class VALUE, class CONTAINER = deque<VALUE> >
class TestDriver {
    // Test driver class for 'queue'

  private:

    // TYPES
    typedef bsl::queue<VALUE, CONTAINER> Obj;
        // type under testing

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
    // to right to configure the 'queue<VALUE, CONTAINER>' object according to
    // a custom language.  Uppercase letters [A..Z] correspond to arbitrary
    // (but unique) 'VALUE' object.
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

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    template <class VALUES>
    static size_t verify_object(Obj&          object,
                                const VALUES& expectedValues,
                                size_t        expectedSize);

    static bool use_same_allocator(Obj&                 object,
                                   int                  TYPE_ALLOC,
                                   bslma::TestAllocator *ta);

    static void populate_container(CONTAINER&        container,
                                   const char*       SPEC,
                                   size_t            length);

  public:

    // TEST CASES

    static void testCase15();
        // Test comparison free operators.

    static void testCase14();
        // Test accessor 'empty'.

    static void testCase13();
        // Test manipulators 'front' and 'back'.

    static void testCase12();
        // Test user-supplied constructors.

    static void testCase11();
        // Test generator functions 'g'.

    // static void testCase10();
        // Reserved for BSLX.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8();
        // Test 'swap' member.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    // static void testCase5();
        // Reserved for (<<) operator.

    static void testCase4();
        // Test basic accessors ('size', 'front', and 'back').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators (default ctor, 'push', and 'pop')

    static void testCase1(const VALUE  *testValues, size_t  numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.

    static void testCase1_NoAlloc(const VALUE  *testValues, size_t  numValues);
        // Breathing test, except on a non-allocator container.  This test
        // *exercises* basic functionality but *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class VALUE, class CONTAINER>
template <class VALUES>
size_t TestDriver<VALUE, CONTAINER>::verify_object(Obj&          object,
                                                   const VALUES& expectedValues,
                                                   size_t        expectedSize)
    // Verify the specified 'object' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'object' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, object.size(), expectedSize == object.size());

    if(expectedSize != object.size()) {
        return static_cast<size_t>(-1);                               // RETURN
    }

    for (size_t i = 0; i < expectedSize; ++i) {
        if (!(object.front() == expectedValues[i])) {
            return i + 1;                                             // RETURN
        }
        object.pop();
    }
    return 0;
}

template <class VALUE, class CONTAINER>
bool TestDriver<VALUE, CONTAINER>::use_same_allocator(
                                               Obj&                 object,
                                               int                  TYPE_ALLOC,
                                               bslma::TestAllocator *ta)
{
    bslma::DefaultAllocatorGuard guard(&bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    if (0 == TYPE_ALLOC)  // If 'VALUE' does not use allocator, return true.
        return true;                                                  // RETURN
    const bsls::Types::Int64 BB = ta->numBlocksTotal();
    const bsls::Types::Int64  B = ta->numBlocksInUse();

    object.push(VALUES[0]);

    const bsls::Types::Int64 AA = ta->numBlocksTotal();
    const bsls::Types::Int64  A = ta->numBlocksInUse();

    if (BB + TYPE_ALLOC <= AA && B + TYPE_ALLOC <= A)
        return true;                                                  // RETURN
    return false;
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::populate_container(
                                                   CONTAINER&        container,
                                                   const char*       SPEC,
                                                   size_t            length)
{
    bslma::DefaultAllocatorGuard guard(&bslma::NewDeleteAllocator::singleton());
    const TestValues VALUES;

    for (size_t i = 0;i < length; ++i) {
        container.push_back(VALUES[SPEC[i] - 'A']);
    }
}


template <class VALUE, class CONTAINER>
int TestDriver<VALUE, CONTAINER>::ggg(Obj        *object,
                                      const char *spec,
                                      int         verbose)
{
    bslma::DefaultAllocatorGuard guard(&bslma::NewDeleteAllocator::singleton());
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

template <class VALUE, class CONTAINER>
queue<VALUE, CONTAINER>&
TestDriver<VALUE, CONTAINER>::gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class VALUE, class CONTAINER>
queue<VALUE, CONTAINER>
TestDriver<VALUE, CONTAINER>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

                                // ----------
                                // TEST CASES
                                // ----------

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING COMPARISON FREE OPERATORS
    //
    // Concerns:
    //: 1 'operator<' returns the lexicographic comparison on two objects.
    //:
    //: 2 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //:   'operator<'.  i.e., For two objects, 'a' and 'b':
    //:
    //:   1 '(a > b) == (b < a)'
    //:
    //:   2 '(a <= b) == !(b < a)'
    //:
    //:   3 '(a >= b) == !(a < b)'
    //
    // Plan:
    //: 1 For a variety of objects of different sizes and different values,
    //:   test that the comparison returns as expected.  (C-1..3)
    //
    // Testing:
    //   bool operator< (const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator> (const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator>=(const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator<=(const queue<V, C>& lhs, const queue<V, C>& rhs);
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_spec);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            // Create second object.

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_spec);

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

                ASSERTV(LINE1, LINE2,  isLess   == (X < Y));
                ASSERTV(LINE1, LINE2, !isLessEq == (X > Y));
                ASSERTV(LINE1, LINE2,  isLessEq == (X <= Y));
                ASSERTV(LINE1, LINE2, !isLess   == (X >= Y));

                ASSERTV(LINE1, LINE2, oaxm.isTotalSame());
                ASSERTV(LINE1, LINE2, oaym.isTotalSame());
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING 'empty'
    //
    // Concern:
    //: 1 'empty' returns 'true' only when the object is empty.
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
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty'.\n");
    {
        const TestValues VALUES;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            // Verify 'empty' returns correct values.

            ASSERTV(LINE, SPEC, (0 == ti) == X.empty());

            for (size_t tj = 0; tj < strlen(SPEC); ++tj) {
                mX.pop();
            }

            ASSERTV(LINE, SPEC, true == X.empty());
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING 'front' and 'back'
    //
    // Concern:
    //: 1 'front' and 'back' returns a modifiable reference to the front and
    //:   back element of the queue respectively.
    //
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:  (C-1)
    //:
    //:   1 Invoke 'front' and 'back', use the return object as left hand
    //:     operator and assign a distinct value.  Invoke the constant version
    //:     of 'front' and 'back' to verify the changes.  (C-1)
    //
    // Testing:
    //  reference front();
    //  reference back();
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
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'empty', 'front' and 'back'.\n");
    {
        const TestValues VALUES;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(DATA[ti].d_spec);

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (0 != ti) {

                ASSERTV(LINE, SPEC, X.front() == VALUES[SPEC[0] - 'A']);
                ASSERTV(LINE, SPEC,
                        X.back() == VALUES[SPEC[LENGTH - 1] - 'A']);

                // Test 'front'.

                mX.front() = VALUES['Y' - 'A'];
                ASSERTV(LINE, SPEC, X.front() == VALUES['Y' - 'A']);

                // Test 'back'.

                mX.back() = VALUES['Z' - 'A'];
                ASSERTV(LINE, SPEC, X.back() == VALUES['Z' - 'A']);
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase12()
{
    // ------------------------------------------------------------------------
    // VALUE (TEMPLATE) CONSTRUCTORS:
    //
    // Concern:
    //: 1 The object is constructed with all values in the user-supplied
    //:   container in the same order.
    //:
    //: 2 If an allocator is NOT supplied to the value constructor, the
    //:   default allocator in effect at the time of construction becomes
    //:   the object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the value constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the value constructor has no effect
    //:   on subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary memory allocation from any allocator.
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
    //:   described in P-1:  (C-1..8)
    //:
    //:   1 Execute an inner loop creating three distinct objects, in turn,
    //:     each object having the same value, 'V', but configured differently:
    //:     (a) without passing an allocator, (b) passing a null allocator
    //:     address explicitly, and (c) passing the address of a test allocator
    //:     distinct from the default allocator.
    //:
    //:   2 For each of the three iterations in P-2.1:  (C-1..8)
    //:
    //:     1 Insert the test data to a specialized container
    //:
    //:     2 Create three 'bslma_TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     3 Use the value constructor to dynamically create an object using
    //:       the container in P-2.2.1, with its object allocator configured
    //:       appropriately (see P-2.2.2), supplying all the arguments as
    //:       'const'; use a distinct test allocator for the object's
    //:       footprint.
    //:
    //:     4 Verify that all of the attributes of each object have their
    //:       expected values.  (C-1, 5)
    //:
    //:     5 Use the appropriate test allocators to verify that:
    //:       (C-2..4, 6..8)
    //:
    //:       1 An object that IS expected to allocate memory does so from the
    //:         object allocator only (irrespective of the specific number of
    //:         allocations or the total amount of memory allocated).  (C-6)
    //:
    //:       2 An object that is expected NOT to allocate memory does not
    //:         allocate memory.
    //:
    //:       3 If an allocator was supplied at construction (P-2.1c), the
    //:         default allocator doesn't allocate any memory.  (C-6)
    //:
    //:       4 No temporary memory is allocated from the object allocator.
    //:         (C-7)
    //:
    //:       5 All object memory is released when the object is destroyed.
    //:         (C-8)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure
    //:       that its object allocator is properly installed.  (C-2..4)
    //:
    // Testing:
    //   queue(const CONTAINER& container);
    //   queue(const CONTAINER& container, const ALLOCATOR& allocator);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) printf("\nTesting value constructors.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_spec);
            const size_t      LENGTH  = strlen(DATA[ti].d_spec);

            if (verbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ T_ P(CONFIG) }

                // Insert test data to a container.

                CONTAINER container;
                populate_container(container, SPEC, LENGTH);

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                // Install default allocator.

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(container);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(container, (bslma::Allocator*)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(container, &sa);
                      objAllocatorPtr = &sa;
                  } break;
                  default: {
                      ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      return;                                         // RETURN
                  } break;
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(LINE, CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    ASSERTV(LINE, CONFIG, Obj(), *objPtr, Obj() == *objPtr);
                    firstFlag = false;
                }

                // Verify the expected attributes values.

                ASSERTV(LINE, SPEC, LENGTH,
                        0 == verify_object(mX, EXP, LENGTH));

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

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g:
    //
    // Concern:
    //: 1 Since 'g' is implemented almost entirely using 'gg', we need to
    //:   verify only that the arguments are properly forwarded.
    //:
    //: 2 'g' does not affect the test allocator, and that 'g' returns an
    //:   object by value.
    //
    // Plan:
    //: 1 For each SPEC in a short list of specifications:  (C-1)
    //:
    //:   1 Compare the object returned (by value) from the generator function,
    //:     'g(SPEC)' with the value of a newly constructed OBJECT configured
    //:     using 'gg(&OBJECT,  SPEC)'.
    //:
    //:   2 Compare the results of calling the allocator's 'numBlocksTotal' and
    //:     'numBytesInUse' methods before and after calling 'g' in order to
    //:     demonstrate that 'g' has no effect on the test allocator.  (C-1)
    //:
    //: 2 Use a specific SPEC, verify that 'g' returns by value by checking the
    //:   two objects returned by calling 'g' twice are different objects.
    //:   (C-2)
    //
    // Testing:
    //   queue g(const char *spec);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose)
        printf("\nCompare values produced by 'g' and 'gg' "
               "for various inputs.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const char *SPEC = DATA[ti].d_spec;
        if (veryVerbose) { P_(ti);  P(SPEC); }

        Obj mX(&oa);
        gg(&mX, SPEC);  const Obj& X = mX;

        if (veryVerbose) {
            printf("\t g = "); dbg_print(g(SPEC)); printf("\n");
            printf("\tgg = "); dbg_print(X); printf("\n");
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

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase9()
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
    //: 9 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:10 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma_TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 10)
    //:
    //:   1 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3. For each of the iterations (P-4.2): (C-1..2, 5..8, 10):
    //:
    //:     1 Use the value constructor and a "scratch" allocator to create two
    //:       'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:     2 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:     3 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     4 Assign 'mX' from 'Z'.
    //:
    //:     5 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     6 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     7 Verify that the respective allocator addresses held by 'mX' and
    //:       'Z' are unchanged.  (C-2, 7)
    //:
    //:     8 Use the appropriate test allocators to verify that: (C-8, 10)
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
    //:         (C-10)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z'.
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Verify 'mX' is still using the object allocator.
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
    //   queue& operator=(const queue& rhs);
    // ------------------------------------------------------------------------


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

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

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
                Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

                if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mX, TYPE_ALLOC, &oa));
                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mZ, TYPE_ALLOC, &scratch));

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

                Obj  mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                Obj  mZZ(&scratch);  const Obj& ZZ = gg(&mZZ,  SPEC1);
                Obj& mZ = mX;        const Obj& Z  = mZ;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR,  &X, mR == &X);

                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, use_same_allocator(mZ, TYPE_ALLOC, &oa));

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase8()
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
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.  Use the member and free 'swap' functions
    //:     to swap the value of 'mW' with itself; verify, after each swap,
    //:     that:  (C-6)
    //:
    //:     1 The value is unchanged.  (C-6)
    //:
    //:     2 There was no additional object memory allocation.
    //:
    //:   3 For each row 'R2' in the table of P-3:  (C-1..2)
    //:
    //:     1 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mW', having the value described by 'R1'; also use the
    //:       copy constructor and a "scratch" allocator to create a 'const'
    //:       'Obj' 'XX' from 'mW'.
    //:
    //:     2 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 There was no additional object memory allocation.
    //:
    //:       3 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  Note that a 'queue' object
    //:         does not provide public access to the allocator held.  We test
    //:         this indirectly by calling 'push' to add a value to the object
    //:         and then check the allocator statistics variation.  (C-2)
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

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

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

            if (veryVerbose) { T_ P_(LINE1) P_(W) }

            ASSERTV(LINE1, Obj(), W, Obj() == W);
            firstFlag = false;
        }

        // member 'swap'
        {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, oam.isTotalSame());
        }

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            // member 'swap'
            {
                Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
                const Obj XX(W, &scratch);

                if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

                Obj mX(XX, &oa);  const Obj& X = mX;

                Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
                ASSERT(use_same_allocator(mX, TYPE_ALLOC, &oa));
                ASSERT(use_same_allocator(mY, TYPE_ALLOC, &oa));
            }

            // free function 'swap'
            {
                Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
                const Obj XX(W, &scratch);

                if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

                Obj mX(XX, &oa);  const Obj& X = mX;

                Obj mY(&oa);  const Obj& Y = gg(&mY, SPEC2);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
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
        const Obj XX(X, &scratch);

        Obj mY(&oa);  const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator).
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('push's) on the created object have no
    //:   effect on the original.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
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
    //:   'push'.  Using the 'operator!=' verify that y differs from x and w.
    //:   (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    // Testing:
    //   queue(const queue& original);
    //   queue(const queue& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa(veryVeryVerbose);

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            // Create control object w.
            Obj mW; const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX = new Obj(&oa);
                gg(pX, SPEC);

                const Obj Y0(*pX);

                ASSERTV(SPEC, W == Y0);
                ASSERTV(SPEC, W == X);

                delete pX;
                ASSERTV(SPEC, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tPush into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Y1.push(VALUES[0]);

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
                    printf("\t\t\tPush into created obj, "
                           "with test allocator:\n");

                const bsls::Types::Int64 BB = oa.numBlocksTotal();
                const bsls::Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, &oa);

                const bsls::Types::Int64 AA = oa.numBlocksTotal();
                const bsls::Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                if (0 == LENGTH) {
                    ASSERTV(SPEC, BB <= AA);
                    ASSERTV(SPEC,  B <=  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC
                                            * static_cast<int>(X.size());
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS <= AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS <=  A);
                }

                const bsls::Types::Int64 CC = oa.numBlocksTotal();
                const bsls::Types::Int64  C = oa.numBlocksInUse();

                Y11.push(VALUES[0]);

                const bsls::Types::Int64 DD = oa.numBlocksTotal();
                const bsls::Types::Int64  D = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, CC + TYPE_ALLOC <= DD);
                ASSERTV(SPEC,  C + TYPE_ALLOC <=  D);

                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERTV(SPEC, W != Y11);
                ASSERTV(SPEC, X != Y11);
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase6()
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
    //:10 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:11 The equality operator's signature and return type are standard.
    //:
    //:12 The inequality operator's signature and return type are standard.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 11..12)
    //:
    //: 2 Create a 'bslma_TestAllocator' object, and install it as the default
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
    //:       4 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring both 'oax' and 'oay' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7, 10)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //   bool operator==(const queue<V, C>& lhs, const queue<V, C>& rhs);
    //   bool operator!=(const queue<V, C>& lhs, const queue<V, C>& rhs);
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

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;
            const size_t      LENGTH1 = strlen(DATA[ti].d_spec);

           if (veryVerbose) { T_ P_(LINE1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;
                const size_t      LENGTH2 = strlen(DATA[tj].d_spec);

                if (veryVerbose) { T_ T_ P_(LINE2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = !strcmp(SPEC1, SPEC2);  // expected result

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

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                }
            }
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - front
    //     - back
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
    //:     3 Use 'front' and 'back' to verify the values are as expected.
    //:       (C-1..2)
    //:
    //:     4 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //
    // Testing:
    //   const_reference front() const;
    //   const_reference back()  const;
    //   size_type       size()  const;
    // ------------------------------------------------------------------------

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_spec);
            const size_t      LENGTH = strlen(DATA[ti].d_spec);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

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
                    ASSERTV(LINE, SPEC, CONFIG,          EXP[0] == X.front());
                    ASSERTV(LINE, SPEC, CONFIG, EXP[LENGTH - 1] == X.back ());
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

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase3()
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
    //   queue<V,C>& gg(queue<V,C> *object, const char* spec);
    //   int ggg(queue<V,C> *object, const char *spec, int verbose = 1);
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
            const size_t      LENGTH = strlen(DATA[ti].d_spec);
            const TestValues  EXP(DATA[ti].d_spec);
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&oa);
            const Obj& X = gg(&mX, SPEC);   // original spec

            Obj mY(&oa);
            const Obj& Y = gg(&mY, SPEC);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         EXPR   = DATA[ti].d_return;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&oa);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                ASSERTV(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, EXPR == RESULT);
        }
    }
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase2()
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
    //: 9 'push' adds an additional element to the end of the queue.
    //:
    //:10 'pop' removes the element at front of the queue if there is any.
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
    //:       'X', with its object allocator configured appropriately; use a
    //:       distinct test allocator for the object's footprint.
    //:
    //:     3 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-8)
    //:
    //:     4 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     5 Push 'L' elements in order of increasing value into the container
    //:       and use the (as yet unproven) basic accessors to verify the
    //:       container has the expected values.  Verify the number of
    //:       allocation is as expected. (C-5..6)
    //:
    //:     6 Invoke 'pop' and verify that the container has one fewer element
    //:       (C-10).
    //:
    //:     7 Verify that all object memory is released when the object is
    //:       destroyed.  (C-7)
    //
    // Testing:
    //   queue()
    //   queue(const A& allocator);
    //   ~queue();
    //   void push(const value_type& value);
    //   void pop();
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    if (verbose) { P(TYPE_ALLOC); }

    const TestValues VALUES;  // contains 52 distinct increasing values

    const size_t MAX_LENGTH = 9;

    for (size_t ti = 1; ti < MAX_LENGTH; ++ti) {
        const size_t LENGTH = ti;

        if (verbose) {
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
                  objPtr = new (fa) Obj(static_cast<bslma::Allocator*>(0));
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

                ASSERTV(X.front() == VALUES[0]);
                ASSERTV(X.back () == VALUES[tj]);

                if (0 < TYPE_ALLOC) {
                    //ASSERTV(CONFIG, tam.isTotalUp());
                    ASSERTV(CONFIG, tam.isInUseUp());
                }
                else {
                    ASSERTV(CONFIG, tam.isTotalSame());
                    ASSERTV(CONFIG, tam.isInUseSame());
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) printf("\n\tTesting 'pop'.\n");

            if (verbose) {
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
                    ASSERTV(X.front() == VALUES[tj + 1]);
                    ASSERTV(X.back () == VALUES[LENGTH - 1]);
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

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase1_NoAlloc(const VALUE  *testValues,
                                                     size_t        numValues)
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
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators [1, 5], basic
    //   accessors, equality operators, copy constructors [2, 8], and the
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
    //: 5  Append another element of value B to x2). { x1:A x2:AB }
    //:
    //: 6  Remove element of value A  from x1.       { x1: x2:AB }
    //:
    //: 7  Create a third object x3 (default ctor).  { x1: x2:AB x3: }
    //:
    //: 8  Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
    //:
    //: 9  Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
    //:
    //: 10 Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
    //:
    //: 11 Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    ASSERT(testValues);
    ASSERT(1 < numValues);  // Need at least two test elements

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 1) Create an object x1 (default ctor)."
                            "\t\t\t{ x1: }\n");
    Obj mX1;  const Obj& X1 = mX1;

    ASSERT(   0 == X1.size());
    ASSERT(true == X1.empty());

    ASSERT(  X1  == X1 );        ASSERT(!(X1  != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 2) Create a second object x2 (copy from x1)."
                            "\t\t{ x1: x2: }\n");
    Obj mX2(X1);  const Obj& X2 = mX2;
    ASSERT(   0 == X2.size ());
    ASSERT(true == X1.empty());

    ASSERT(X2 == X1 );           ASSERT(!(X2 != X1));
    ASSERT(X2 == X2 );           ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 3) Push an element of value A to x1)."
                                "\t\t\t{ x1:A x2: }\n");

    mX1.push(testValues[0]);

    ASSERT(            1 == X1.size ());
    ASSERT(        false == X1.empty());
    ASSERT(testValues[0] == X1.front());
    ASSERT(testValues[0] == X1.back ());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 4) Append the same value A to x2)."
                            "\t\t\t{ x1:A x2:A }\n");

    mX2.push(testValues[0]);

    ASSERT(            1 == X2.size ());
    ASSERT(        false == X2.empty());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[0] == X2.back ());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 5) Append another element of value B to x2)."
                            "\t\t{ x1:A x2:AB }\n");

    mX2.push(testValues[1]);

    ASSERT(            2 == X2.size ());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[1] == X2.back ());

    ASSERT(!(X2 == X1));         ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 6) Remove element of value A from x1."
                            "\t\t\t{ x1: x2:AB }\n");
    mX1.pop();

    ASSERT(0 == X1.size());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 7) Create a third object x3 (default ctor)."
                            "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3;  const Obj& X3 = mX3;

    ASSERT(   0 == X3.size ());
    ASSERT(true == X3.empty());

    ASSERT(  X3 == X1 );         ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                            "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2);  const Obj& X4 = mX4;


    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));         ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );         ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));         ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );         ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                            "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;

    ASSERT(   0 == X2.size ());
    ASSERT(true == X2.empty());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );         ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));         ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;

    ASSERT(            2 == X3.size ());
    ASSERT(        false == X3.empty());
    ASSERT(testValues[0] == X3.front());
    ASSERT(testValues[1] == X3.back ());

    ASSERT(!(X3 == X1));         ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );         ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n11) Assign x4 = x4 (aliasing)."
                            "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;

    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}

template <class VALUE, class CONTAINER>
void TestDriver<VALUE, CONTAINER>::testCase1(const VALUE  *testValues,
                                             size_t        numValues)
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
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators [1, 5], basic
    //   accessors, equality operators, copy constructors [2, 8], and the
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
    //: 5  Append another element of value B to x2). { x1:A x2:AB }
    //:
    //: 6  Remove element of value A  from x1.       { x1: x2:AB }
    //:
    //: 7  Create a third object x3 (default ctor).  { x1: x2:AB x3: }
    //:
    //: 8  Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
    //:
    //: 9  Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
    //:
    //: 10 Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
    //:
    //: 11 Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    ASSERT(testValues);
    ASSERT(1 < numValues);  // Need at least two test elements

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 1) Create an object x1 (default ctor)."
                            "\t\t\t{ x1: }\n");
    Obj mX1(&testAllocator);  const Obj& X1 = mX1;

    ASSERT(   0 == X1.size());
    ASSERT(true == X1.empty());

    ASSERT(  X1  == X1 );        ASSERT(!(X1  != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 2) Create a second object x2 (copy from x1)."
                            "\t\t{ x1: x2: }\n");
    Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
    ASSERT(   0 == X2.size ());
    ASSERT(true == X1.empty());

    ASSERT(X2 == X1 );           ASSERT(!(X2 != X1));
    ASSERT(X2 == X2 );           ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 3) Push an element of value A to x1)."
                                "\t\t\t{ x1:A x2: }\n");

    mX1.push(testValues[0]);

    ASSERT(            1 == X1.size ());
    ASSERT(        false == X1.empty());
    ASSERT(testValues[0] == X1.front());
    ASSERT(testValues[0] == X1.back ());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 4) Append the same value A to x2)."
                            "\t\t\t{ x1:A x2:A }\n");

    mX2.push(testValues[0]);

    ASSERT(            1 == X2.size ());
    ASSERT(        false == X2.empty());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[0] == X2.back ());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 5) Append another element of value B to x2)."
                            "\t\t{ x1:A x2:AB }\n");

    mX2.push(testValues[1]);

    ASSERT(            2 == X2.size ());
    ASSERT(testValues[0] == X2.front());
    ASSERT(testValues[1] == X2.back ());

    ASSERT(!(X2 == X1));         ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 6) Remove element of value A from x1."
                            "\t\t\t{ x1: x2:AB }\n");
    mX1.pop();

    ASSERT(0 == X1.size());

    ASSERT(  X1 == X1 );         ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));         ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 7) Create a third object x3 (default ctor)."
                            "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3(&testAllocator);  const Obj& X3 = mX3;

    ASSERT(   0 == X3.size ());
    ASSERT(true == X3.empty());

    ASSERT(  X3 == X1 );         ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                            "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;


    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));         ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );         ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));         ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );         ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                            "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;

    ASSERT(   0 == X2.size ());
    ASSERT(true == X2.empty());

    ASSERT(  X2 == X1 );         ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );         ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );         ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));         ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;

    ASSERT(            2 == X3.size ());
    ASSERT(        false == X3.empty());
    ASSERT(testValues[0] == X3.front());
    ASSERT(testValues[1] == X3.back ());

    ASSERT(!(X3 == X1));         ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));         ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );         ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );         ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\n11) Assign x4 = x4 (aliasing)."
                            "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;

    ASSERT(            2 == X4.size ());
    ASSERT(        false == X4.empty());
    ASSERT(testValues[0] == X4.front());
    ASSERT(testValues[1] == X4.back ());

    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}


// ============================================================================
//                                  USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace UsageExample {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Messages Queue
///- - - - - - - - - - - - -
// In this example, we will use the 'bsl::queue' container adapter to implement
// a message processor in a server program that receives and displays messages
// from clients.
//
// Suppose we want to write a server program that has two threads: one thread
// (receiving thread) receives messages from clients, passing them to a message
// processor; the other thread (processing thread) runs the message processor,
// printing the messages to console in the same order as they were received.
// To accomplish this task, we can use 'bsl::queue' in the message processor to
// buffer received, but as yet unprinted messages.  The message processor
// pushes newly received messages onto the queue in the receiving thread, and
// pops them off the queue in the processing thread.
//
// First, we define a 'Message' type:
//..
struct Message {
    int         d_msgId;  // message identifier given by client
    const char *d_msg_p;  // message content (null terminated byte string,
                          // not owned)
};
//..
// Then, we define the class 'MessageProcessor', which provides methods to
// receive and process messages:
//..
class MessageProcessor {
    // This class receives and processes messages from clients.
//..
// Here, we define a private data member of 'bsl::queue<Message>' type, which
// is an instantiation of 'bsl::queue' that uses 'Message' for its 'VALUE'
// (template parameter) type and (by default) 'bsl::deque<Message>' for its
// 'CONTAINER' (template parameter) type:
//..
    // DATA
    bsl::queue<Message> d_msgQueue;  // queue holding received but
                                     // unprocessed messages
    // ...

  public:
    // CREATORS
    explicit MessageProcessor(bslma::Allocator *basicAllocator = 0);
        // Create a message processor object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.

    // MANIPULATORS
    void receiveMessage(const Message &message);
        // Enqueue the specified 'message' for processing.

    void processMessages(int verbose);
        // Dequeue all messages currently contained by this processor,
        // and print them to the console if the specified 'verbose' flag
        // is not 0.
};
//..
// Next, we implement the 'MessageProcessor' constructor:
//..
MessageProcessor::MessageProcessor(bslma::Allocator *basicAllocator)
: d_msgQueue(basicAllocator)
{
}
//..
// Notice that we pass to the contained 'd_msgQueue' object the
// 'bslma::Allocator' supplied to the 'MessageProcessor' at construction.
//
// Now, we implement the 'receiveMessage' method, which pushes the given
// message onto the queue object:
//..
void MessageProcessor::receiveMessage(const Message &message)
{
    // ... (some synchronization)

    d_msgQueue.push(message);

    // ...
}
//..
// Finally, we implement the 'processMessages' method, which pops all messages
// off the queue object:
//..
void MessageProcessor::processMessages(int verbose)
{
    // ... (some synchronization)

    while (!d_msgQueue.empty()) {
        const Message& message = d_msgQueue.front();
        if (verbose) {
            printf("Msg %d: %s\n", message.d_msgId, message.d_msg_p);
        }
        d_msgQueue.pop();
    }

    // ...
}
//..
// Notice that the sequence of messages popped out will be in exactly the same
// order in which they are pushed, due to the first-in-first-out property of
// the queue.

}  // close namespace UsageExample

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
      case 17: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example"
                            "\n=====================\n");

        using namespace UsageExample;

        const int TOTAL_MSGS = 20;
        const int PROCESS_TIME[] = {1, 4, 9, 13, 19,};
        const int numProcess = sizeof(PROCESS_TIME) / sizeof(PROCESS_TIME[0]);

        int              k = 0;
        char             buffer[256];
        Message          msg;
        MessageProcessor msgProcessor(&ta);

        for (int i = 0;i < TOTAL_MSGS; ++i) {
            sprintf(buffer, "This is message %d", i);
            msg.d_msgId = i;
            msg.d_msg_p = buffer;
            msgProcessor.receiveMessage(msg);

            if (k < numProcess && i == PROCESS_TIME[k]) {
                msgProcessor.processMessages(veryVerbose);
                ++k;
            }
            msgProcessor.processMessages(veryVerbose);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING NON ALLOCATOR SUPPORTING TYPE
        // --------------------------------------------------------------------

        typedef queue<int, NonAllocCont<int> > NonAllocQueue;

        NonAllocQueue mX;    const NonAllocQueue& X = mX;

        ASSERT(X.empty());

        mX.push(3);
        mX.push(4);
        mX.push(5);

        ASSERT(! X.empty());
        ASSERT(3 == X.size());
        ASSERT(3 == X.front());
        ASSERT(5 == X.back());

        NonAllocQueue mY(X);   const NonAllocQueue& Y = mY;

        ASSERT(X == Y);         ASSERT(!(X != Y));
        ASSERT(X <= Y);         ASSERT(!(X >  Y));
        ASSERT(X >= Y);         ASSERT(!(X <  Y));

        mY.pop();
        mY.push(6);

        ASSERT(X != Y);         ASSERT(!(X == Y));
        ASSERT(X <  Y);         ASSERT(!(X >= Y));
        ASSERT(X <= Y);         ASSERT(!(X >  Y));
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING FREE COMPARISON OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Free Comparison Operators"
                            "\n=================================\n");

        RUN_EACH_TYPE(TestDriver, testCase15, char, int);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // OTHER ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Other Accessors"
                            "\n=======================\n");

        RUN_EACH_TYPE(TestDriver, testCase14, TEST_TYPES_REGULAR);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // OTHER MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Other Manipulators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver, testCase13, TEST_TYPES_REGULAR);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Constructor"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver, testCase12, TEST_TYPES_REGULAR);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'g'"
                            "\n===========\n");

        RUN_EACH_TYPE(TestDriver, testCase11, TEST_TYPES_REGULAR);
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

        RUN_EACH_TYPE(TestDriver,  testCase9, TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        RUN_EACH_TYPE(TestDriver,  testCase8, TEST_TYPES_REGULAR);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        RUN_EACH_TYPE(TestDriver,  testCase7, TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
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

        RUN_EACH_TYPE(TestDriver,  testCase4, TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        RUN_EACH_TYPE(TestDriver,  testCase3, TEST_TYPES_REGULAR);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        RUN_EACH_TYPE(TestDriver,  testCase2, TEST_TYPES_REGULAR);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        if (verbose) printf("deque<int>:\n");
        TestDriver<int, deque<int> >::testCase1(SPECIAL_INT_VALUES,
                                                NUM_SPECIAL_INT_VALUES);

        //TBD: uncomment when 'bsl::list' is available in bslstl
        //if (verbose) printf("list:\n");
        //TestDriver<int,  list<int> >::testCase1(INT_VALUES, NUM_INT_VALUES);

        if (verbose) printf("NonAllocCont<int>:\n");
        TestDriver<int, NonAllocCont<int> >::testCase1_NoAlloc(
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
