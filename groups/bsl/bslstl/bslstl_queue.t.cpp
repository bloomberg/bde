//bslstl_queue.t.cpp                                                  -*-C++-*-

#include <bslstl_queue.h>

#include <bslstl_allocator.h>
#include <bslstl_iterator.h>
#include <bslstl_forwarditerator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatormonitor.h>    // for testing only
#include <bslma_testallocatorexception.h>  // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>
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
//
// ACCESSORS
// [13]bool empty() const;
// [ 4]size_type size() const;
// [13]reference front();
// [ 4]const_reference front() const;
// [13]reference back();
// [ 4]const_reference back() const;

// FREE FUNCTIONS
// [ 6]bool operator==(const queue& lhs, const queue& rhs);
// [ 6]bool operator!=(const queue& lhs, const queue& rhs);
// [14]bool operator< (const queue& lhs, const queue& rhs);
// [14]bool operator> (const queue& lhs, const queue& rhs);
// [14]bool operator>=(const queue& lhs, const queue& rhs);
// [14]bool operator<=(const queue& lhs, const queue& rhs);
//
// speicalized algorithms:
// [ 8]void swap(queue& lhs,queue& rhs);
//
// ----------------------------------------------------------------------------
// CLASS 'bsl::priority_queue'
//
// CREATORS
// [ 2] explicit priority_queue();
// [12] priority_queue(const COMPARATOR& comp, const CONTAINER& cont);
// [12] priority_queue(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [12] priority_queue(first, last, comparator, container);
// [ 7] priority_queue(const priority_queue&);
// [ 2] explicit priority_queue(const ALLOCATOR& allocator);
// [12] priority_queue(const COMPARATOR& comp, const ALLOCATOR& allocator);
// [12] priority_queue(const COMPARATOR&, const CONTAINER&, const ALLOCATOR&);
// [ 7] priority_queue(const priority_queue&, const ALLOCATOR&);
//
// MANIPULATORS
// [ 9] priority_queue& operator=(const priority_queue& rhs);
// [ 2] void push(const value_type& value);
// [ 2] void pop();
// [ 8] void swap(priority_queue& other);
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
// [15] USAGE EXAMPLE
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
// [  ] TODO: CONCERN: The object is comppatible with STL allocator.

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

struct DefaultQDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
};

static
const DefaultQDataRow DEFAULT_QDATA[] = {
    //line spec
    //---- ----
    { L_,  "",                 },
    { L_,  "A",                },
    { L_,  "B",                },
    { L_,  "AA",               },
    { L_,  "AB",               },
    { L_,  "BA",               },
    { L_,  "AC",               },
    { L_,  "CD",               },
    { L_,  "ABC",              },
    { L_,  "ACB",              },
    { L_,  "BAC",              },
    { L_,  "BCA",              },
    { L_,  "CAB",              },
    { L_,  "CBA",              },
    { L_,  "BAD",              },
    { L_,  "ABCA",             },
    { L_,  "ABCB",             },
    { L_,  "ABCC",             },
    { L_,  "ABCD",             },
    { L_,  "ACBD",             },
    { L_,  "BDCA",             },
    { L_,  "DCBA",             },
    { L_,  "BEAD",             },
    { L_,  "BCDE",             },
    { L_,  "ABCDE",            },
    { L_,  "ACBDE",            },
    { L_,  "CEBDA",            },
    { L_,  "EDCBA",            },
    { L_,  "FEDCB",            },
    { L_,  "FEDCBA",           },
    { L_,  "ABCABC",           },
    { L_,  "AABBCC",           },
    { L_,  "ABCDEFG",          },
    { L_,  "ABCDEFGH",         },
    { L_,  "ABCDEFGHI",        },
    { L_,  "ABCDEFGHIJKLMNOP", },
    { L_,  "PONMLKJIGHFEDCBA", },
    { L_,  "ABCDEFGHIJKLMNOPQ",},
    { L_,  "DHBIMACOPELGFKNJQ",},
};

static
const int DEFAULT_NUM_QDATA = sizeof DEFAULT_QDATA / sizeof *DEFAULT_QDATA;

// Define default data for testing 'bsl::priority_queue'.

struct DefaultPQDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultPQDataRow DEFAULT_PQDATA[] = {
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

typedef bsltf::AllocTestType TestValueType;

static
const int DEFAULT_NUM_PQDATA = sizeof DEFAULT_PQDATA / sizeof *DEFAULT_PQDATA;

int SPECIAL_INT_VALUES[]       = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
int NUM_SPECIAL_INT_VALUES     =
                      sizeof(SPECIAL_INT_VALUES) / sizeof(*SPECIAL_INT_VALUES);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }


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
                   bslma_Allocator *basicAllocator = 0)
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
            //ASSERTV(LINE, d_copy == *d_object_p);
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
    // 'TemplateTestFacility::getValue'.  The function-call operator also
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
            return bsltf::TemplateTestFacility::getValue<TYPE>(lhs)
                 < bsltf::TemplateTestFacility::getValue<TYPE>(rhs);  // RETURN
        }
        else {
            return bsltf::TemplateTestFacility::getValue<TYPE>(lhs)
                 > bsltf::TemplateTestFacility::getValue<TYPE>(rhs);  // RETURN
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
    // 'TemplateTestFacility::getValue'.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const
        // Return 'true' if the integer representation of the specified 'lhs'
        // is less than integer representation of the specified 'rhs'.
    {
        return bsltf::TemplateTestFacility::getValue<TYPE>(lhs)
             > bsltf::TemplateTestFacility::getValue<TYPE>(rhs);
    }
};

// FREE OPERATORS
template <class TYPE>
bool lessThanFunction(const TYPE& lhs, const TYPE& rhs)
    // Return 'true' if the integer representation of the specified 'lhs' is
    // less than integer representation of the specified 'rhs'.
{
    return bsltf::TemplateTestFacility::getValue<TYPE>(lhs)
         < bsltf::TemplateTestFacility::getValue<TYPE>(rhs);
}

}  // close unnamed namespace


//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// 'queue' specific print function

template <class VALUE, class CONTAINER>
void dbg_print(const bsl::queue<VALUE, CONTAINER>& q)
{
    if (q.empty()) {
        printf("<empty>");
    }
    else {
        printf("size: %d, front: ", q.size());
        dbg_print(static_cast<char>(
                    bsltf::TemplateTestFacility::getValue(q.front())));
        printf(", back: ");
        dbg_print(static_cast<char>(
                    bsltf::TemplateTestFacility::getValue(q.back())));
    }
    fflush(stdout);
}

// 'priority_queue' specific print function

template <class VALUE, class CONTAINER, class COMPARATOR>
void dbg_print(const bsl::priority_queue<VALUE, CONTAINER, COMPARATOR>& pq)
{
    if (pq.empty()) {
        printf("<empty>");
    }
    else {
        printf("size: %d, top: ", pq.size());
        dbg_print(static_cast<char>(
                    bsltf::TemplateTestFacility::getValue(pq.top())));
    }
    fflush(stdout);
}

// generic debug print function (3-arguments)
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

                            // -----------------
                            // class QTestDriver
                            // -----------------

template <class VALUE, class CONTAINER = deque<VALUE> >
class QTestDriver {
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
    static int verify_object(Obj&          object,
                             const VALUES& expectedValues,
                             size_t        expectedSize);

    static bool use_same_allocator(Obj&                 object,
                                   int                  TYPE_ALLOC,
                                   bslma_TestAllocator *ta);

    static void populate_container(CONTAINER&        container,
                                   const char*       SPEC,
                                   size_t            length);

  public:

    // TEST CASES

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
};

                            // ------------------
                            // class PQTestDriver
                            // ------------------

template <class VALUE,
          class CONTAINER  = deque<VALUE>,
          class COMPARATOR = TestComparator<VALUE> >
class PQTestDriver {
    // Test driver class for 'priority_queue'

  private:

    // TYPES
    typedef bsl::priority_queue<VALUE, CONTAINER, COMPARATOR>  Obj;
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

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    template <class VALUES>
    static int verify_object(Obj&          object,
                             const VALUES& expectedValues,
                             size_t        expectedSize);

    static bool use_same_allocator(Obj&                 object,
                                   int                  TYPE_ALLOC,
                                   bslma_TestAllocator *ta);

    static void populate_container(CONTAINER&        container,
                                   const char*       SPEC,
                                   size_t            length);

    static bool is_equal(Obj& lhs, Obj& rhs);

  public:

    // TEST CASES

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

    static void testCase1(const COMPARATOR& comparator,
                          const VALUE       *testValues,
                          size_t            numValues);
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};


                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class VALUE, class CONTAINER>
template <class VALUES>
int QTestDriver<VALUE, CONTAINER>::verify_object(Obj&          object,
                                                const VALUES& expectedValues,
                                                size_t        expectedSize)
    // Verify the specified 'object' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'object' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, object.size(), expectedSize == object.size());

    if(expectedSize != object.size()) {
        return -1;                                                    // RETURN
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
bool QTestDriver<VALUE, CONTAINER>::use_same_allocator(
                                               Obj&                 object,
                                               int                  TYPE_ALLOC,
                                               bslma_TestAllocator *ta)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());
    const TestValues VALUES;

    if (0 == TYPE_ALLOC)  // If 'VALUE' does not use allocator, return true.
        return true;                                                  // RETURN
    const bsls_Types::Int64 BB = ta->numBlocksTotal();
    const bsls_Types::Int64  B = ta->numBlocksInUse();

    object.push(VALUES[0]);

    const bsls_Types::Int64 AA = ta->numBlocksTotal();
    const bsls_Types::Int64  A = ta->numBlocksInUse();

    if (BB + TYPE_ALLOC <= AA && B + TYPE_ALLOC <= A)
        return true;                                                  // RETURN
    return false;
}

template <class VALUE, class CONTAINER>
void QTestDriver<VALUE, CONTAINER>::populate_container(
                                                   CONTAINER&        container,
                                                   const char*       SPEC,
                                                   size_t            length)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());
    const TestValues VALUES;

    for (size_t i = 0;i < length; ++i) {
        container.push_back(VALUES[SPEC[i] - 'A']);
    }
}


template <class VALUE, class CONTAINER>
int QTestDriver<VALUE, CONTAINER>::ggg(Obj        *object,
                                       const char *spec,
                                       int         verbose)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());
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
QTestDriver<VALUE, CONTAINER>::gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class VALUE, class CONTAINER>
queue<VALUE, CONTAINER>
QTestDriver<VALUE, CONTAINER>::g(const char *spec)
{
    Obj object((bslma_Allocator *)0);
    return gg(&object, spec);
}

// ----------------------------------------------------------------------------

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class VALUES>
int PQTestDriver<VALUE, CONTAINER, COMPARATOR>::verify_object(
                                                  Obj&          object,
                                                  const VALUES& expectedValues,
                                                  size_t        expectedSize)
    // Verify the specified 'object' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'object' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, object.size(), expectedSize == object.size());

    if(expectedSize != object.size()) {
        return -1;                                                    // RETURN
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
bool PQTestDriver<VALUE, CONTAINER, COMPARATOR>::use_same_allocator(
                                               Obj&                 object,
                                               int                  TYPE_ALLOC,
                                               bslma_TestAllocator *ta)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());
    const TestValues VALUES;

    if (0 == TYPE_ALLOC)  // If 'VALUE' does not use allocator, return true.
        return true;                                                  // RETURN
    const bsls_Types::Int64 BB = ta->numBlocksTotal();
    const bsls_Types::Int64  B = ta->numBlocksInUse();

    object.push(VALUES[0]);

    const bsls_Types::Int64 AA = ta->numBlocksTotal();
    const bsls_Types::Int64  A = ta->numBlocksInUse();

    if (BB + TYPE_ALLOC <= AA && B + TYPE_ALLOC <= A)
        return true;                                                  // RETURN
    return false;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::populate_container(
                                                   CONTAINER&        container,
                                                   const char*       SPEC,
                                                   size_t            length)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());
    const TestValues VALUES;

    for (size_t i = 0;i < length; ++i) {
        container.push_back(VALUES[SPEC[i] - 'A']);
    }
}

// 'priority_queue' specific compare equal function

template <class VALUE, class CONTAINER, class COMPARATOR>
bool PQTestDriver<VALUE, CONTAINER, COMPARATOR>::is_equal(Obj& lhs, Obj& rhs)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());

    if (lhs.size() != rhs.size()) {
        return false;                                                 // RETURN
    }
    while (!lhs.empty() && !rhs.empty()) {
        if (lhs.top() != rhs.top()) {
            return false;                                             // RETURN
        }
        lhs.pop();
        rhs.pop();
    }
    if (!lhs.empty() || !rhs.empty()) {
        return false;                                                 // RETURN
    }
    return true;
}


template <class VALUE, class CONTAINER, class COMPARATOR>
int PQTestDriver<VALUE, CONTAINER, COMPARATOR>::ggg(Obj        *object,
                                                    const char *spec,
                                                    int         verbose)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());
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
PQTestDriver<VALUE, CONTAINER, COMPARATOR>::gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
priority_queue<VALUE, CONTAINER, COMPARATOR>
PQTestDriver<VALUE, CONTAINER, COMPARATOR>::g(const char *spec)
{
    Obj object((bslma_Allocator *)0);
    return gg(&object, spec);
}

                                // ----------
                                // TEST CASES
                                // ----------

template <class VALUE, class CONTAINER>
void QTestDriver<VALUE, CONTAINER>::testCase12()
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

    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

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

                bslma_TestAllocator da("default",   veryVeryVeryVerbose);
                bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

                // Install default allocator.

                bslma_DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma_TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(container);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(container, (bslma_Allocator*)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(container, &sa);
                      objAllocatorPtr = &sa;
                  } break;
                  default: {
                      ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                  } break;
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma_TestAllocator&  oa = *objAllocatorPtr;
                bslma_TestAllocator& noa = 'c' != CONFIG ? sa : da;

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
void QTestDriver<VALUE, CONTAINER>::testCase11()
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

    bslma_TestAllocator oa(veryVeryVerbose);

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

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
        const bsls_Types::Int64 TOTAL_BLOCKS_BEFORE = oa.numBlocksTotal();
        const bsls_Types::Int64 IN_USE_BYTES_BEFORE = oa.numBytesInUse();
        ASSERTV(ti, X == g(SPEC));
        const bsls_Types::Int64 TOTAL_BLOCKS_AFTER = oa.numBlocksTotal();
        const bsls_Types::Int64 IN_USE_BYTES_AFTER = oa.numBytesInUse();
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
void QTestDriver<VALUE, CONTAINER>::testCase9()
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


    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

    bslma_TestAllocator         da("default", veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
                Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

                if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

                bslma_TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    bslma_TestAllocatorMonitor oam(&oa), sam(&scratch);

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

            bslma_TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj  mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                Obj  mZZ(&scratch);  const Obj& ZZ = gg(&mZZ,  SPEC1);
                Obj& mZ = mX;        const Obj& Z  = mZ;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma_TestAllocatorMonitor oam(&oa), sam(&scratch);

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
void QTestDriver<VALUE, CONTAINER>::testCase8()
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

    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma_TestAllocator         da("default", veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nUse a table of distinct object values.\n");

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma_TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);


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

            bslma_TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            Obj mW(&oa);  const Obj& W = gg(&mW,  SPEC1);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            bslma_TestAllocatorMonitor oam(&oa);

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

                bslma_TestAllocatorMonitor oam(&oa);

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

                bslma_TestAllocatorMonitor oam(&oa);

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

        bslma_TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mX(&oa);  const Obj& X = mX;
        const Obj XX(X, &scratch);

        Obj mY(&oa);  const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, &scratch);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma_TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }
}

template <class VALUE, class CONTAINER>
void QTestDriver<VALUE, CONTAINER>::testCase7()
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

    bslma_TestAllocator oa(veryVeryVerbose);

    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        const int NUM_DATA                      = DEFAULT_NUM_QDATA;
        const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = (int) strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length %d:\n", LENGTH);
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

                const bsls_Types::Int64 BB = oa.numBlocksTotal();
                const bsls_Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, &oa);

                const bsls_Types::Int64 AA = oa.numBlocksTotal();
                const bsls_Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                if (0 == LENGTH) {
                    ASSERTV(SPEC, BB <= AA);
                    ASSERTV(SPEC,  B <=  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS <= AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS <=  A);
                }

                const bsls_Types::Int64 CC = oa.numBlocksTotal();
                const bsls_Types::Int64  C = oa.numBlocksInUse();

                Y11.push(VALUES[0]);

                const bsls_Types::Int64 DD = oa.numBlocksTotal();
                const bsls_Types::Int64  D = oa.numBlocksInUse();

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
void QTestDriver<VALUE, CONTAINER>::testCase6()
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

        (void)operatorEq;  // quash potential compiler warnings
        (void)operatorNe;
    }

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

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
                bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

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

                    bslma_TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma_TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma_TestAllocator& xa = oax;
                    bslma_TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(&xa); const Obj& X = gg(&mX, SPEC1);
                    Obj mY(&ya); const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2 == Y.size());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma_TestAllocatorMonitor oaxm(&xa);
                    bslma_TestAllocatorMonitor oaym(&ya);

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
void QTestDriver<VALUE, CONTAINER>::testCase4()
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
    //   const_reference  front() const;
    //   const_reference  back()  const;
    //   size_type        size()  const;
    // ------------------------------------------------------------------------

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_spec);
            const int         LENGTH = strlen(DATA[ti].d_spec);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma_TestAllocator da("default",   veryVeryVeryVerbose);
                bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma_TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma_TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma_DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma_TestAllocator *objAllocatorPtr;

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
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma_TestAllocator&  oa = *objAllocatorPtr;
                bslma_TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                         ? da
                                         : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma_TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == (int)X.size());
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
void QTestDriver<VALUE, CONTAINER>::testCase3()
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

    bslma_TestAllocator oa(veryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int NUM_DATA                      = DEFAULT_NUM_QDATA;
        const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

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
            const size_t      LENGTH = (int)strlen(SPEC);

            Obj mX(&oa);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                 ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, EXPR == RESULT);
        }
    }
}

template <class VALUE, class CONTAINER>
void QTestDriver<VALUE, CONTAINER>::testCase2()
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

    const int TYPE_ALLOC =
             bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

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

            bslma_TestAllocator da("default",   veryVeryVeryVerbose);
            bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma_DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                 *objPtr;
            bslma_TestAllocator *objAllocatorPtr;

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
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma_TestAllocator&  oa = *objAllocatorPtr;
            bslma_TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the non-object allocator.

            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 ==  noa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, 0 == X.size());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'push' (bootstrap).\n"); }

            if (veryVeryVerbose) {
                printf("\t\tOn an object of initial length %d.\n", LENGTH);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma_TestAllocatorMonitor tam(&oa);

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
                printf("\t\tOn an object of initial length %d.\n", LENGTH + 1);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bsls_Types::Int64 B  = oa.numBlocksInUse();

                mX.pop();

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 - tj == X.size());
                if (LENGTH - 1 == tj) {
                    ASSERTV(true == X.empty());
                }
                else {
                    ASSERTV(X.front() == VALUES[tj + 1]);
                    ASSERTV(X.back () == VALUES[LENGTH - 1]);
                }

                const bsls_Types::Int64 A  = oa.numBlocksInUse();
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
void QTestDriver<VALUE, CONTAINER>::testCase1(const VALUE  *testValues,
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
    //   non-empty instance [11].
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

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

// ----------------------------------------------------------------------------

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase12()
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
    //: 6 Any memory allocation is from the user-supplied allocator if
    //:   supplied, and otherwise the default allocator.
    //:
    //: 7 Every object releases any allocated memory at destruction.
    //:
    //: 8 QoI: Creating an object having the default-constructed value
    //:   allocates no memory.
    //:
    // Plan:
    //: 1 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 2 For each row (representing a distinct object value, 'V') in the table
    //:   described in P-1:  (C-1..7)
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
    //:       expected values.  (C-1, 5)
    //:
    //:     5 Use the appropriate test allocators to verify that:
    //:       (C-2..4, 6..7)
    //:
    //:       1 An object that IS expected to allocate memory does so from the
    //:         object allocator (the number of allocations in use).  (C-6)
    //:
    //:       2 An object that is expected NOT to allocate memory does not
    //:         allocate memory.
    //:
    //:       3 If an allocator was supplied at construction (P-2.1c) and
    //:         'CONFIG' is not 'g', the non-object allocator doesn't allocate
    //:         any memory.  (C-6)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-7)
    //:
    //:     6 Use the helper function 'use_same_allocator' to verify each
    //:       underlying attribute capable of allocating memory to ensure
    //:       that its object allocator is properly installed.  (C-2..4)
    //:
    // Testing:
    //   queue(const CONTAINER& container);
    //   queue(const CONTAINER& container, const ALLOCATOR& allocator);
    // ------------------------------------------------------------------------

    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    const int NUM_DATA                       = DEFAULT_NUM_PQDATA;
    const DefaultPQDataRow (&DATA)[NUM_DATA] = DEFAULT_PQDATA;

    if (verbose) printf("\nTesting value constructors.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char       *SPEC    = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_results);
            const size_t      LENGTH  = strlen(DATA[ti].d_spec);
            const TestValues  EXP0("");
            const size_t      LENGTH0 = 0;

            if (verbose) { P_(LINE) P_(SPEC) P(LENGTH); }

            for (char cfg = 'a'; cfg <= 'g'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { T_ T_ P(CONFIG) }

                // Initialize user-supplied data.  Some value constructors
                // take user-supplied container, some use user-supplied
                // iterators.

                CONTAINER container, CONT0;
                populate_container(container, SPEC, LENGTH);

                typename CONTAINER::iterator BEGIN = container.begin();
                typename CONTAINER::iterator END   = container.end();

                COMPARATOR comparator;

                bslma_TestAllocator da("default",   veryVeryVeryVerbose);
                bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

                // Install default allocator.

                bslma_DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma_TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                      objPtr = new (fa) Obj(comparator, container);
                      objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                      objPtr = new (fa) Obj(BEGIN, END);
                      objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                      objPtr = new (fa) Obj(BEGIN, END, comparator, CONT0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'd': {
                      objPtr = new (fa) Obj(comparator, (bslma_Allocator*)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'e': {
                      objPtr = new (fa) Obj(comparator,
                                            container,
                                            (bslma_Allocator*)0);
                      objAllocatorPtr = &da;
                  } break;
                  case 'f': {
                      objPtr = new (fa) Obj(comparator, &sa);
                      objAllocatorPtr = &sa;
                  } break;
                  case 'g': {
                      objPtr = new (fa) Obj(comparator, container, &sa);
                      objAllocatorPtr = &sa;
                  } break;
                  default: {
                      ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                  } break;
                }
                ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                bslma_TestAllocator&  oa = *objAllocatorPtr;
                bslma_TestAllocator& noa = 'f' > CONFIG ? sa : da;

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    bslma_DefaultAllocatorGuard guard(
                                       &bslma_NewDeleteAllocator::singleton());
                    Obj objTemp;
                    ASSERTV(LINE, CONFIG, objTemp, *objPtr,
                            is_equal(objTemp, *objPtr));
                    firstFlag = false;
                }

                // Verify the expected attributes values.

                if ('d' == CONFIG || 'f' == CONFIG) {
                    ASSERTV(LINE, SPEC, LENGTH, CONFIG,
                            0 == verify_object(mX, EXP0, LENGTH0));
                }
                else {
                    ASSERTV(LINE, SPEC, LENGTH, CONFIG,
                            0 == verify_object(mX, EXP, LENGTH));
                }

                // Verify any attribute allocators are installed properly.

                ASSERTV(LINE, CONFIG, use_same_allocator(mX, TYPE_ALLOC, &oa));

                // Verify no allocation from the non-object allocator.

                if ('g' != CONFIG) {
                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());
                }

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
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase11()
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
    //   priority_queue g(const char *spec);
    // ------------------------------------------------------------------------

    bslma_TestAllocator oa(veryVeryVerbose);

    const int NUM_DATA                       = DEFAULT_NUM_PQDATA;
    const DefaultPQDataRow (&DATA)[NUM_DATA] = DEFAULT_PQDATA;

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
        const bsls_Types::Int64 TOTAL_BLOCKS_BEFORE = oa.numBlocksTotal();
        const bsls_Types::Int64 IN_USE_BYTES_BEFORE = oa.numBytesInUse();
        Obj temp = g(SPEC);
        const bsls_Types::Int64 TOTAL_BLOCKS_AFTER = oa.numBlocksTotal();
        const bsls_Types::Int64 IN_USE_BYTES_AFTER = oa.numBytesInUse();
        ASSERTV(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        ASSERTV(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        ASSERTV(ti, is_equal(mX, temp));
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

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase9()
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
    //:     1 Use the value constructor and a "scratch" allocator to create
    //:       three 'Obj', 'mZ1', 'mZ2',  and 'mZ3', each having the value 'V'.
    //:
    //:     2 Create a 'bslma_TestAllocator' object, 'oa'.
    //:
    //:     3 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     4 Assign 'mX' from constant reference of 'mZ1': 'Z1'.
    //:
    //:     5 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     6 Use the helper function 'is_equal' to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', has the same value as that of 'mZ2'.
    //:         (C-1)
    //:
    //:       2 'mZ1' still has the same value as that of 'mZ3'.  (C-6)
    //:
    //:     7 Use the helper function 'use_same_allocator' to verify that the
    //:       respective allocator addresses held by 'mX' and 'mZ1' are
    //:       unchanged.  (C-2, 7)
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
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions (using
    //:     the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the helper function 'is_equal' to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the helper function 'use_same_allocator' to verify that 'mX'
    //:     still has the object allocator.
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
    //   priority_queue& operator=(const priority_queue& rhs);
    // ------------------------------------------------------------------------


    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    const int NUM_DATA                       = DEFAULT_NUM_PQDATA;
    const DefaultPQDataRow (&DATA)[NUM_DATA] = DEFAULT_PQDATA;

    bslma_TestAllocator         da("default", veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

            // Create second object
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                Obj mZ1(&scratch);  const Obj& Z1 = gg(&mZ1, SPEC1);
                Obj mZ2(&scratch);  const Obj& Z2 = gg(&mZ2, SPEC1);
                Obj mZ3(&scratch);  const Obj& Z3 = gg(&mZ3, SPEC1);

                if (veryVerbose) { T_ P_(LINE1) P_(Z1) P_(Z2) P(Z3) }

                bslma_TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    bslma_TestAllocatorMonitor sam(&scratch);
                    Obj *mR = &(mX = Z1);
                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, Z2,   X, is_equal(mZ2, mX));
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                    ASSERTV(LINE1, LINE2, Z3,  Z1, is_equal(mZ3, mZ1));
                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mX, TYPE_ALLOC, &oa));
                    ASSERTV(LINE1, LINE2,
                            use_same_allocator(mZ1, TYPE_ALLOC, &scratch));
                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma_TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj  mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                Obj  mZZ(&scratch);  const Obj& ZZ = gg(&mZZ,  SPEC1);
                Obj& mZ = mX;        const Obj& Z  = mZ;

                bslma_TestAllocatorMonitor oam(&oa), sam(&scratch);
                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, sam.isTotalSame());
                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, ZZ,   Z, is_equal(mZZ, mZ));
                ASSERTV(LINE1, mR,  &X, mR == &X);
                ASSERTV(LINE1, use_same_allocator(mZ, TYPE_ALLOC, &oa));

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase8()
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
    //:       'mW1', haviing the value described by 'R1'.  Use the copy
    //:       constructor and 'oa' to create a modifiable 'Obj', 'mX', from
    //:       'mW1'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mW2', haviing the value described by 'R2'.  Use the copy
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

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bsl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma_TestAllocator         da("default", veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard dag(&da);

    if (verbose) printf("\nUse a table of distinct object values.\n");

    const int NUM_DATA                      = DEFAULT_NUM_QDATA;
    const DefaultQDataRow (&DATA)[NUM_DATA] = DEFAULT_QDATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        bslma_TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

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

            bslma_TestAllocatorMonitor oam(&oa);

            mW2.swap(mW2);

            ASSERTV(LINE1, oam.isTotalSame());
            ASSERTV(LINE1, W2, W1, is_equal(mW2, mW1));
        }

        // free function 'swap'
        {
            Obj mW1(&oa);           const Obj& W1 = gg(&mW1,  SPEC1);
            Obj mW2(mW1, &scratch); const Obj& W2 = mW2;

            bslma_TestAllocatorMonitor oam(&oa);

            swap(mW2, mW2);

            ASSERTV(LINE1, oam.isTotalSame());
            ASSERTV(LINE1, W2, W1, is_equal(mW2, mW1));
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

                bslma_TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, oam.isTotalSame());

                ASSERTV(LINE1, LINE2, X, W2, is_equal(mX, mW2));
                ASSERTV(LINE1, LINE2, Y, W1, is_equal(mY, mW1));
                ASSERT(use_same_allocator(mX, TYPE_ALLOC, &oa));
                ASSERT(use_same_allocator(mY, TYPE_ALLOC, &oa));
            }

            // free function 'swap'
            {
                Obj mW1(&oa);           const Obj& W1 = gg(&mW1,  SPEC1);
                Obj mX(W1, &oa);        const Obj& X = mX;

                Obj mW2(&oa);           const Obj& W2 = gg(&mW2,  SPEC2);
                Obj mY(W2, &oa);        const Obj& Y = mY;

                if (veryVerbose) { T_ P_(LINE2) P_(W1) P_(W2) P(X) P(Y) }

                bslma_TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, oam.isTotalSame());

                ASSERTV(LINE1, LINE2, X, W2, is_equal(mX, mW2));
                ASSERTV(LINE1, LINE2, Y, W1, is_equal(mY, mW1));
                ASSERT(use_same_allocator(mX, TYPE_ALLOC, &oa));
                ASSERT(use_same_allocator(mY, TYPE_ALLOC, &oa));
            }
        }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma_TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma_TestAllocator scratch("scratch", veryVeryVeryVerbose);

        Obj mW1(&oa);         const Obj& W1 = mW1;
        Obj mX(W1, &scratch); const Obj& X  = mX;

        Obj mW2(&oa);         const Obj& W2 = gg(&mW2, "ABC");
        Obj mY(W2, &scratch); const Obj& Y  = mY;

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma_TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }

        ASSERTV(W2, X, is_equal(mW2, mX));
        ASSERTV(W1, Y, is_equal(mW1, mY));
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //: 1 The new object's value is the same as that of the original object.
    //:   Note that given 'bsl::priority_queue' class has no equlity operators,
    //:   two 'bsl::priority_queue' objects are tested equal only if they
    //:   have same size, and have same return values from 'top' operations
    //:   until they are both empty.
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
    //: 2 For each value in S, initialize objects w1, w2 and x, copy construct
    //:   y from x and use the test equal function 'is_equal' to verify that y
    //:   has the same value as w1 and x has the same value as w2. (C-1..3)
    //:
    //: 3 For each value in S, initialize objects w and x, copy construct y
    //:   from x.  Let x go out of scope and use the test equal function
    //:   'is_equal' to verify that y has the same value as w. (C-4)
    //:
    //: 4 For each value in S, initialize objects x, and copy construct y from
    //:   x.  Change the state of y, by using the *primary* *manipulator*
    //:   'push'.  Using the test equal function 'is_equal' to verify that y
    //:   differs from x.
    //:   (C-5)
    //:
    //: 5 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    // Testing:
    //   priority_queue(const priority_queue& original);
    //   priority_queue(const priority_queue& original, const A& allocator);
    // ------------------------------------------------------------------------

    bslma_TestAllocator oa(veryVeryVerbose);

    const int TYPE_ALLOC =
           bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d.\n", TYPE_ALLOC);
    {
        const int NUM_DATA                       = DEFAULT_NUM_PQDATA;
        const DefaultPQDataRow (&DATA)[NUM_DATA] = DEFAULT_PQDATA;
        const TestValues VALUES;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = (int) strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length %d:\n", LENGTH);
                P(SPEC);
            }
            {   // Testing concern 1..3.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                // Create control object w1 and w2.
                Obj mW1; const Obj& W1 = gg(&mW1, SPEC);
                ASSERTV(ti, LENGTH == W1.size()); // same lengths
                if (veryVerbose) { printf("\tControl Obj W1: "); P(W1); }

                Obj mW2; const Obj& W2 = gg(&mW2, SPEC);
                ASSERTV(ti, LENGTH == W2.size()); // same lengths
                if (veryVerbose) { printf("\tControl Obj W2: "); P(W2); }

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);
                ASSERTV(ti, LENGTH == X.size());  // same lengths
                if (veryVerbose) { printf("\t\tTest Equal Obj: "); P(X); }

                Obj mY(X);
                ASSERTV(is_equal(mW1, mY));
                ASSERTV(is_equal(mW2, mX));
            }
            {   // Testing concern 4.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                // Create control object w.
                Obj mW; const Obj& W = gg(&mW, SPEC);
                ASSERTV(ti, LENGTH == W.size()); // same lengths
                if (veryVerbose) { printf("\tControl Obj W: "); P(W); }

                Obj *pX = new Obj(&oa); gg(pX, SPEC);
                ASSERTV(ti, LENGTH == W.size()); // same lengths
                if (veryVerbose) { printf("\t\tDynamic Obj: "); P(*pX); }

                Obj mY(*pX);
                delete pX;
                ASSERTV(is_equal(mW, mY));
            }
            {   // Testing concern 5.

                if (veryVerbose) printf("\t\t\tPush into created obj, "
                                        "without test allocator:\n");

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);
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
                ASSERT(!is_equal(mX, Y1));
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tPush into created obj, "
                           "with test allocator:\n");

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                const bsls_Types::Int64 BB = oa.numBlocksTotal();
                const bsls_Types::Int64  B = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, &oa);

                const bsls_Types::Int64 AA = oa.numBlocksTotal();
                const bsls_Types::Int64  A = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                if (0 == LENGTH) {
                    ASSERTV(SPEC, BB <= AA);
                    ASSERTV(SPEC,  B <=  A);
                }
                else {
                    const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    ASSERTV(SPEC, BB + 1 + TYPE_ALLOCS <= AA);
                    ASSERTV(SPEC,  B + 1 + TYPE_ALLOCS <=  A);
                }

                const bsls_Types::Int64 CC = oa.numBlocksTotal();
                const bsls_Types::Int64  C = oa.numBlocksInUse();

                Y11.push(VALUES[0]);

                const bsls_Types::Int64 DD = oa.numBlocksTotal();
                const bsls_Types::Int64  D = oa.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                ASSERTV(SPEC, CC + TYPE_ALLOC <= DD);
                ASSERTV(SPEC,  C + TYPE_ALLOC <=  D);
                ASSERTV(SPEC, Y11.size() == LENGTH + 1);
                ASSERT(!is_equal(mX, Y11));
            }
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase4()
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

    const int NUM_DATA                       = DEFAULT_NUM_PQDATA;
    const DefaultPQDataRow (&DATA)[NUM_DATA] = DEFAULT_PQDATA;

    if (verbose) { printf(
                "\nCreate objects with various allocator configurations.\n"); }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const TestValues  EXP(DATA[ti].d_results);
            const int         LENGTH = strlen(DATA[ti].d_results);

            if (verbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                const char CONFIG = cfg;

                bslma_TestAllocator da("default",   veryVeryVeryVerbose);
                bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma_TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
                bslma_TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

                bslma_DefaultAllocatorGuard dag(&da);

                Obj                 *objPtr;
                bslma_TestAllocator *objAllocatorPtr;

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
                  } break;
                }

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);
                bslma_TestAllocator&  oa = *objAllocatorPtr;
                bslma_TestAllocator& noa = ('c' == CONFIG || 'd' == CONFIG)
                                         ? da
                                         : sa1;

                // --------------------------------------------------------

                // Verify basic accessor

                bslma_TestAllocatorMonitor oam(&oa);

                ASSERTV(LINE, SPEC, CONFIG, LENGTH == (int)X.size());
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
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase3()
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
    //   priority_queue& gg(priority_queue *object, const char* spec);
    //   int ggg(priority_queue *object, const char *spec, int verbose = 1);
    // ------------------------------------------------------------------------

    bslma_TestAllocator oa(veryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int NUM_DATA                       = DEFAULT_NUM_PQDATA;
        const DefaultPQDataRow (&DATA)[NUM_DATA] = DEFAULT_PQDATA;

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
            const size_t      LENGTH = (int)strlen(SPEC);

            Obj mX(&oa);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                 ASSERTV(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, EXPR == RESULT);
        }
    }
}

template <class VALUE, class CONTAINER, class COMPARATOR>
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase2()
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

    const int TYPE_ALLOC =
             bslalg_HasTrait<VALUE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

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

            bslma_TestAllocator da("default",   veryVeryVeryVerbose);
            bslma_TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma_TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma_DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting default constructor.\n");
            }

            Obj                 *objPtr;
            bslma_TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  objPtr = new (fa) Obj((bslma_Allocator*)0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                  mX = *objPtr;  const Obj& X = mX;
            bslma_TestAllocator&  oa = *objAllocatorPtr;
            bslma_TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify no allocation from the non-object allocator.

            ASSERTV(LENGTH, CONFIG, noa.numBlocksTotal(),
                    0 ==  noa.numBlocksTotal());
            ASSERTV(LENGTH, CONFIG, 0 == X.size());

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'push' (bootstrap).\n"); }

            if (veryVeryVerbose) {
                printf("\t\tOn an object of initial length %d.\n", LENGTH);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                bslma_TestAllocatorMonitor tam(&oa);

                mX.push(VALUES[tj]);
                ASSERTV(LENGTH, CONFIG, tj + 1 == X.size());

                ASSERTV(X.top() == VALUES[tj]);

                if (0 < TYPE_ALLOC) {
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
                printf("\t\tOn an object of initial length %d.\n", LENGTH + 1);
            }

            for (size_t tj = 0; tj < LENGTH; ++tj) {
                const bsls_Types::Int64 B  = oa.numBlocksInUse();

                mX.pop();

                ASSERTV(LENGTH, CONFIG, LENGTH - 1 - tj == X.size());
                if (LENGTH - 1 == tj) {
                    ASSERTV(true == X.empty());
                }
                else {
                    ASSERTV(X.top() == VALUES[LENGTH - 2 - tj]);
                }

                const bsls_Types::Int64 A  = oa.numBlocksInUse();
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
void PQTestDriver<VALUE, CONTAINER, COMPARATOR>::testCase1(
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
    //   non-empty instance [11].
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

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

    bslma_TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);
    bslma_TestAllocator ta(veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 12: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Constructor"
                            "\n=========================\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver,
                            testCase12,
                            BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver,
                            testCase12,
                            BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase12();
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'g'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'g'"
                            "\n===========\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver,
                            testCase11,
                            BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver,
                            testCase11,
                            BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase11();
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

        BSLTF_RUN_EACH_TYPE(QTestDriver,  testCase9, BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver, testCase9, BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase9();
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver,  testCase8, BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver, testCase8, BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase8();
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver,  testCase7, BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver, testCase7, BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase7();
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver, testCase6, BSLTF_TEST_TYPES_REGULAR);
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

        BSLTF_RUN_EACH_TYPE(QTestDriver,  testCase4, BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver, testCase4, BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver,  testCase3, BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver, testCase3, BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        BSLTF_RUN_EACH_TYPE(QTestDriver,  testCase2, BSLTF_TEST_TYPES_REGULAR);
        BSLTF_RUN_EACH_TYPE(PQTestDriver, testCase2, BSLTF_TEST_TYPES_REGULAR);
        PQTestDriver<TestValueType, vector<TestValueType> >::testCase2();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        // queue
        QTestDriver<int, deque<int> >::testCase1(SPECIAL_INT_VALUES,
                                                 NUM_SPECIAL_INT_VALUES);
        //TODO: uncomment when 'bsl::list' is available
        //QTestDriver<int,  list<int> >::testCase1(INT_VALUES, NUM_INT_VALUES);

        // priority_queue
        typedef bool (*Comparator)(int, int);
        PQTestDriver<int, deque<int>, Comparator>::testCase1(
                                                       &intLessThan,
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
        PQTestDriver<int, deque<int>, std::less<int> >::testCase1(
                                                       std::less<int>(),
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
        PQTestDriver<int, vector<int>, std::less<int> >::testCase1(
                                                       std::less<int>(),
                                                       SPECIAL_INT_VALUES,
                                                       NUM_SPECIAL_INT_VALUES);
        //TODO: uncomment when 'bsl::list' is available
        //QTestDriver<int,  list<int> >::testCase1(INT_VALUES, NUM_INT_VALUES);
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
