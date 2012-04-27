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

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test has two value-semantic container adaptors whose states are
// represented by their underlying containers.
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
// [11]explicit queue(const CONTAINER& container);
// [ 7]queue(const queue& original);
// [ 2]explicit queue(const ALLOCATOR& allocator);
// [11]queue(const CONTAINER& container, const ALLOCATOR& allocator);
// [ 7]queue(const queue& original, const ALLOCATOR& allocator);
//
// MANIPULATORS
// [ 9]queue& operator=(const queue& rhs);
// [ 2]void push(const value_type& value);
// [ 2]void pop();
// [12]void swap(queue& other);
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
// [11]void swap(queue& lhs,queue& rhs);
//
// ----------------------------------------------------------------------------
// CLASS 'bsl::priority_queue'
//
// CREATORS
// [ 2] explicit priority_queue();
// [11] priority_queue(const COMPARATOR& comp, const CONTAINER& cont);
// [11] priority_queue(INPUT_ITERATOR first, INPUT_ITERATOR last);
// [11] priority_queue(first, last, comparator, container);
// [ 7] priority_queue(const priority_queue&);
// [ 2] explicit priority_queue(const ALLOCATOR& allocator);
// [11] priority_queue(const COMPARATOR& comp, const ALLOCATOR& allocator);
// [11] priority_queue(const COMPARATOR&, const CONTAINER&, const ALLOCATOR&);
// [ 7] priority_queue(const priority_queue&, const ALLOCATOR&);
//
// MANIPULATORS
// [ 9] priority_queue& operator=(const priority_queue& rhs);
// [ 2] void push(const value_type& value);
// [ 2] void pop();
// [12] void swap(priority_queue& other);
//
// ACCESSORS
// [13] bool empty() const;
// [ 4] size_type size() const;
// [ 4] const_reference top() const;
//
// specialized algorithms:
// [12] void swap(priority_queue& lhs, priority_queue& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 3] TODO: TEST APPARATUS: Temporarily Not Applicable
// [ 5] TESTING OUTPUT: Not Applicable
// [ 8] TODO: TEST APPARATUS (using copty ctor): Temporarily Not Applicable
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

const char DEFAULT_VALUES[]    =
                        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
size_t     NUM_DEFAULT_VALUES  =
                              sizeof(DEFAULT_VALUES) / sizeof(*DEFAULT_VALUES);

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_, "",                  "" },
    { L_, "A",                 "A" },
    { L_, "B",                 "B" },
    { L_, "AA",                "AA"},
    { L_, "AB",                "BA" },
#if 0
    { L_, "BA",                "AB" },
    { L_, "AC",                "AC" },
    { L_, "CD",                "CD" },
    { L_, "ABC",               "ABC" },
    { L_, "ACB",               "ABC" },
    { L_, "BAC",               "ABC" },
    { L_, "BCA",               "ABC" },
    { L_, "CAB",               "ABC" },
    { L_, "CBA",               "ABC" },
    { L_, "BAD",               "ABD" },
    { L_, "ABCA",              "ABC" },
    { L_, "ABCB",              "ABC" },
    { L_, "ABCC",              "ABC" },
    { L_, "ABCABC",            "ABC" },
    { L_, "AABBCC",            "ABC" },
    { L_, "ABCD",              "ABCD" },
    { L_, "ACBD",              "ABCD" },
    { L_, "BDCA",              "ABCD" },
    { L_, "DCBA",              "ABCD" },
    { L_, "BEAD",              "ABDE" },
    { L_, "BCDE",              "BCDE" },
    { L_, "ABCDE",             "ABCDE" },
    { L_, "ACBDE",             "ABCDE" },
    { L_, "CEBDA",             "ABCDE" },
    { L_, "EDCBA",             "ABCDE" },
    { L_, "FEDCB",             "BCDEF" },
    { L_, "FEDCBA",            "ABCDEF" },
    { L_, "ABCDEFG",           "ABCDEFG" },
    { L_, "ABCDEFGH",          "ABCDEFGH" },
    { L_, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_, "ABCDEFGHIJKLMNOP",  "ABCDEFGHIJKLMNOP" },
    { L_, "PONMLKJIGHFEDCBA",  "ABCDEFGHIJKLMNOP" },
    { L_, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
#endif
};
static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

int SPECIAL_INT_VALUES[]       = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };
int NUM_SPECIAL_INT_VALUES     =
                      sizeof(SPECIAL_INT_VALUES) / sizeof(*SPECIAL_INT_VALUES);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

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

// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

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


//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

  private:
    char            *d_data_p;
    bslma_Allocator *d_allocator_p;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestType,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    TestType(bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestType(char c, bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numCopyCtorCalls;

        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }

    ~TestType() {
        ++numDestructorCalls;
        *d_data_p = UNINITIALIZED_VALUE;
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
        d_allocator_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
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

    void setDatum(char c) {
        *d_data_p = c;
    }

    // ACCESSORS
    char value() const {
        return *d_data_p;
    }

    bslma_Allocator *allocator() const {
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
bool operator==(const TestType& lhs, const TestType& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

bool operator<(const TestType& lhs, const TestType& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() < rhs.value();
}

// TestType-specific print function.
void dbg_print(const TestType& rhs) {
    printf("%c", rhs.value());
    fflush(stdout);
}

                       // =====================
                       // class TestTypeNoAlloc
                       // =====================

class TestTypeNoAlloc {
    // This test type has footprint and interface identical to 'TestType'.  It
    // also counts the number of default and copy constructions, assignments,
    // and destructions.  It does not allocate, and thus could have the
    // bit-wise copyable trait, but we defer this to the
    // 'BitwiseCopyableTestType'.

    // DATA
    union {
        char  d_char;
        char  d_fill[sizeof(TestType)];
    } d_u;

  public:
    // CREATORS
    TestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    TestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
        ++numCharCtorCalls;
    }

    TestTypeNoAlloc(const TestTypeNoAlloc&  original)
    {
        d_u.d_char = original.d_u.d_char;
        ++numCopyCtorCalls;
    }

    ~TestTypeNoAlloc()
    {
        ++numDestructorCalls;
        d_u.d_char = '_';
    }

    // MANIPULATORS
    TestTypeNoAlloc& operator=(const TestTypeNoAlloc& rhs)
    {
        ++numAssignmentCalls;
        d_u.d_char = rhs.d_u.d_char;
        return *this;
    }

    // ACCESSORS
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
bool operator==(const TestTypeNoAlloc& lhs,
                const TestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

bool operator<(const TestTypeNoAlloc& lhs, const TestTypeNoAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() < rhs.value();
}

// TestType-specific print function.
void dbg_print(const TestTypeNoAlloc& rhs) {
    printf("%c", rhs.value());
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

    static Obj& g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static int verifyContainer(Obj&        container,
                               const char *expectedValues,
                               size_t      expectedSize);
  public:

    // TEST CASES

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

template <class VALUE, class CONTAINER, class COMPARATOR>
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

    static Obj& g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static int verifyContainer(Obj&        container,
                               const char *expectedValues,
                               size_t      expectedSize);

  public:

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
int QTestDriver<VALUE, CONTAINER>::verifyContainer(
                                              Obj&              container,
                                              const char*       expectedValues,
                                              size_t            expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if(expectedSize != container.size()) {
        return -1;                                                    // RETURN
    }

    for (size_t i = 0; i < expectedSize; ++i) {
        if (!(container.front() == VALUE(expectedValues[i]))) {
            return i + 1;                                             // RETURN
        }
        container.pop();
    }
    return 0;
}

template <class VALUE, class CONTAINER>
int QTestDriver<VALUE, CONTAINER>::ggg(Obj        *object,
                                       const char *spec,
                                       int         verbose)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->push(VALUE(spec[i]));
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
queue<VALUE, CONTAINER>&
QTestDriver<VALUE, CONTAINER>::g(const char *spec)
{
    Obj object((bslma_Allocator *)0);
    return gg(&object, spec);
}

// ----------------------------------------------------------------------------

template <class VALUE, class CONTAINER, class COMPARATOR>
int PQTestDriver<VALUE, CONTAINER, COMPARATOR>::verifyContainer(
                                              Obj&              container,
                                              const char*       expectedValues,
                                              size_t            expectedSize)
    // Verify the specified 'container' has the specified 'expectedSize' and
    // contains the same values as the array in the specified 'expectedValues'.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    ASSERTV(expectedSize, container.size(), expectedSize == container.size());

    if(expectedSize != container.size()) {
        return -1;                                                    // RETURN
    }

    for (size_t i = 0; i < expectedSize; ++i) {
        if (!(container.top() == VALUE(expectedValues[i]))) {
            return i + 1;                                             // RETURN
        }
        container.pop();
    }
    return 0;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
int PQTestDriver<VALUE, CONTAINER, COMPARATOR>::ggg(Obj        *object,
                                                    const char *spec,
                                                    int         verbose)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            object->push(VALUE(spec[i]));
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
priority_queue<VALUE, CONTAINER, COMPARATOR>&
PQTestDriver<VALUE, CONTAINER, COMPARATOR>::g(const char *spec)
{
    Obj object((bslma_Allocator *)0);
    return gg(&object, spec);
}

                                // ----------
                                // TEST CASES
                                // ----------

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
    //   const_reference front();
    //   const_reference back();
    //   size_type size() const;
    // ------------------------------------------------------------------------

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
            const char *const RESULT = DATA[ti].d_results;
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
            ASSERTV(0 == verifyContainer(mX, RESULT, LENGTH));
            ASSERTV(0 == verifyContainer(mY, RESULT, LENGTH));
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

                mX.push(VALUE(DEFAULT_VALUES[tj]));
                ASSERTV(LENGTH, CONFIG, tj + 1 == X.size());

                ASSERTV(X.front().value() == DEFAULT_VALUES[0]);
                ASSERTV(X.back ().value() == DEFAULT_VALUES[tj]);

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
                    ASSERTV(X.front().value() == DEFAULT_VALUES[tj + 1]);
                    ASSERTV(X.back ().value() == DEFAULT_VALUES[LENGTH - 1]);
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
    //   We want to exercise basic value-semantic functionality.  In
    //   particular we want to demonstrate a base-line level of correct
    //   operation of the following methods and operators:
    //      - default and copy constructors (and also the destructor)
    //      - the assignment operator (including aliasing)
    //      - equality operators: 'operator==' and 'operator!='
    //      - primary manipulators: 'push' and 'pop' methods
    //   In addition we would like to exercise objects with potentially
    //   different internal organizations representing the same value.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators, basic accessors,
    //   equality operators, and the assignment operator.  Invoke the primary
    //   manipulator [1&5], copy constructor [2&8], and assignment operator
    //   [9&10].  Try aliasing with assignment for a non-empty instance [11].
    //
    // 1) Create an object x1 (default ctor).       { x1: }
    // 2) Create a second object x2 (copy from x1). { x1: x2: }
    // 3) Append an element of value A to x1).      { x1:A x2: }
    // 4) Append the same value A to x2).           { x1:A x2:A }
    // 5) Append another element of value B to x2). { x1:A x2:AB }
    // 6) Remove element of value A  from x1.       { x1: x2:AB }
    // 7) Create a third object x3 (default ctor).  { x1: x2:AB x3: }
    // 8) Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
    // 9) Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
    // 10) Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
    // 11) Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
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
            { L_,   "AB",     "BA"    },
            { L_,   "CD",     "DC"    },
            { L_,   "ABC",    "CBA"   },
            { L_,   "ADBC",   "DCBA"  },
            { L_,   "CEBDA",  "EDCBA" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);
            const char *const RESULT = DATA[ti].d_results;
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
            ASSERTV(0 == verifyContainer(mX, RESULT, LENGTH));
            ASSERTV(0 == verifyContainer(mY, RESULT, LENGTH));
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

                mX.push(VALUE(DEFAULT_VALUES[tj]));
                ASSERTV(LENGTH, CONFIG, tj + 1 == X.size());

                ASSERTV(X.top().value() == DEFAULT_VALUES[tj]);

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
                    ASSERTV(X.top().value() ==
                                              DEFAULT_VALUES[LENGTH - 2 - tj]);
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
    //   We want to exercise basic value-semantic functionality.  In
    //   particular we want to demonstrate a base-line level of correct
    //   operation of the following methods and operators:
    //      - default and copy constructors (and also the destructor)
    //      - the assignment operator (including aliasing)
    //      - equality operators: 'operator==' and 'operator!='
    //      - primary manipulators: 'push' and 'pop' methods
    //   In addition we would like to exercise objects with potentially
    //   different internal organizations representing the same value.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators, basic accessors,
    //   equality operators, and the assignment operator.  Invoke the primary
    //   manipulator [1&5], copy constructor [2&8], and assignment operator
    //   [9&10].  Try aliasing with assignment for a non-empty instance [11].
    //
    // 1) Create an object x1 (default ctor).       { x1: }
    // 2) Create a second object x2 (copy from x1). { x1: x2: }
    // 3) Append an element of value A to x1).      { x1:A x2: }
    // 4) Append the same value A to x2).           { x1:A x2:A }
    // 5) Append two more elements B,C to x2).      { x1:A x2:ABC }
    // 6) Remove top element of value A from x1.    { x1: x2:ABC }
    // 7) Create a third object x3 (default ctor).  { x1: x2:ABC x3: }
    // 8) Create a forth object x4 (copy of x2).    { x1: x2:ABC x3: x4:ABC }
    // 9) Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:ABC }
    // 10) Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:ABC x4:ABC }
    // 11) Assign x4 = x4 (aliasing).               { x1: x2: x3:ABC x4:ABC }
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
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        QTestDriver<char>::testCase4();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'gg'"
                            "\n============\n");

        if (verbose) printf("\n... on 'queue' with 'char'.\n");
        QTestDriver<char>::testCase3();

        if (verbose) printf("\n... on 'queue' with 'TestType'.\n");
        QTestDriver<TestType>::testCase3();

        if (verbose) printf("\n... on 'queue' with 'TestTypeNoAlloc'.\n");
        QTestDriver<TestTypeNoAlloc>::testCase3();

        if (verbose) printf("\n... on 'priority_queue' with 'char'.\n");
        PQTestDriver<char, deque<char>, std::less<char> >::testCase3();

        if (verbose) printf("\n... on 'priority_queue' with 'TestType'.\n");
        PQTestDriver<TestType,
                     deque<TestType>,
                     std::less<TestType> >::testCase3();

        if (verbose)
            printf("\n... on 'priority_queue' with 'TestTypeNoAlloc'.\n");
        PQTestDriver<TestTypeNoAlloc,
                     deque<TestTypeNoAlloc>,
                     std::less<TestTypeNoAlloc> >::testCase3();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

            QTestDriver<TestType>::testCase2();
            QTestDriver<TestTypeNoAlloc>::testCase2();

            PQTestDriver<TestType,
                         deque<TestType>,
                         std::less<TestType> >::testCase2();
            PQTestDriver<TestTypeNoAlloc,
                         deque<TestTypeNoAlloc>,
                         std::less<TestTypeNoAlloc> >::testCase2();
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
