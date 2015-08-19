// bslstl_vector.t.cpp                                                -*-C++-*-

#include <bslstl_vector.h>

#include <bslstl_allocator.h>
#include <bslstl_forwarditerator.h>
#include <bslstl_iterator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_issame.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_stopwatch.h>
#include <bsls_util.h>

#include <bsltf_nontypicaloverloadstesttype.h>

#include <iterator>   // 'iterator_traits'
#include <stdexcept>  // 'length_error', 'out_of_range'

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The object under testing is a container whose interface and contract is
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
// 'push_back' and 'clear' methods to be used by the generator functions 'g'
// and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that some
// manipulators must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bslma_testallocator'
// component.  After the mandatory sequence of cases (1--10) for value-semantic
// types (cases 5 and 10 are not implemented, as there is not output or
// streaming below bslstl), we test each individual constructor, manipulator,
// and accessor in subsequent cases.
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     T            VALUE_TYPE (template argument, no default)
//     A            ALLOCATOR (template argument, default: bsl::allocator<T>)
//     vector<T,A>  Vector_Imp<VALUE_TYPE,ALLOCATOR>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//     ImpUtil      bslstl::VectorImpUtil
//-----------------------------------------------------------------------------
// class Vector_Imp<T,A> (vector)
// ============================================================================
// [11] TRAITS
//
// CREATORS:
// [ 2] vector<T,A>(const A& a = A());
// [12] vector<T,A>(size_type n, const A& a = A());
// [12] vector<T,A>(size_type n, const T& val, const A& a = A());
// [12] template<class InputIter>
//        vector<T,A>(InputIter first, InputIter last, const A& a = A());
// [ 7] vector<T,A>(const vector<T,A>& orig, const A& = A());
// [12] vector(vector<T,A>&& original);
// [ 2] ~vector<T,A>();
//
/// MANIPULATORS:
// [13] template <class InputIter>
//        void assign(InputIter first, InputIter last);
// [13] void assign(size_type numElements, const T& val);
// [ 9] operator=(vector<T,A>&);
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
// [ 2] void clear();
// [15] reference front();
// [15] reference back();
// [  ] VALUE_TYPE *data();
// [20] template <class Args...>
// [17] void emplace_back(Args...);
// [ 2] void push_back(const T&);
// [17] void push_back(T&&);
// [18] void pop_back();
// [17] iterator emplace(const_iterator pos, Args...);
// [17] iterator insert(const_iterator pos, const T& val);
// [17] iterator insert(const_iterator pos, size_type n, const T& val);
// [17] template <class InputIter>
//        void insert(const_iterator pos, InputIter first, InputIter last);
// [18] iterator erase(const_iterator pos);
// [18] iterator erase(const_iterator first, const_iterator last);
// [19] void swap(vector<T,A>&);
//
// ACCESSORS:
// [ 4] const_reference operator[](size_type pos) const;
// [ 4] const_reference at(size_type pos) const;
// [15] const_reference front() const;
// [15] const_reference back() const;
// [  ] const VALUE_TYPE *data() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [14] size_type capacity() const;
// [14] bool empty() const;
// [16] const_iterator begin() const;
// [16] const_iterator end() const;
// [16] const_reverse_iterator rbegin() const;
// [16] const_reverse_iterator rend() const;
//
// FREE OPERATORS:
// [ 6] bool operator==(const vector<T,A>&, const vector<T,A>&);
// [ 6] bool operator!=(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator<(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator>(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator<=(const vector<T,A>&, const vector<T,A>&);
// [20] bool operator>=(const vector<T,A>&, const vector<T,A>&);
// [19] void swap(vector<T,A>& lhs, vector<T,A>& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] ALLOCATOR-RELATED CONCERNS
// [25] USAGE EXAMPLE
// [21] CONCERN: 'std::length_error' is used properly
// [23] DRQS 31711031
// [24] DRQS 34693876
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(vector<T,A> *object, const char *spec, int vF = 1);
// [ 3] vector<T,A>& gg(vector<T,A> *object, const char *spec);
// [ 8] vector<T,A> g(const char *spec);

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
//                      STANDARD BDE TEST DRIVER MACROS
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
class TestType;
class TestTypeNoAlloc;
class BitwiseMoveableTestType;
class BitwiseCopyableTestType;
class BitwiseEqComparableTestType;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators
typedef BitwiseMoveableTestType       BMT;  // uses 'bslma' allocators
typedef BitwiseCopyableTestType       BCT;  // uses 'bslma' allocators
typedef BitwiseEqComparableTestType   BET;  // uses 'bslma' allocators

typedef bsls::Types::Int64            Int64;
typedef bsls::Types::Uint64           Uint64;

// TEST OBJECT (unless o/w specified)
typedef char                          Element;  // every TestType's value type
typedef Vector_Imp<T>                 Obj;
typedef Vector_Util                   ImpUtil;

// CONSTANTS
const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';
const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
    // All test types have character value type.

const int  LARGE_SIZE_VALUE = 10;
    // Declare a large value for insertions into the vector.  Note this value
    // will cause multiple resizes during insertion into the vector.

const int NUM_ALLOCS[] = {
    // Number of allocations (blocks) to create a vector of the following size
    // by using 'push_back' repeatedly (without initial reserve):
    //
    // 0    1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17
    // --   --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,   1,  2,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6
};

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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


// Vector-specific print function.
template <class TYPE, class ALLOC>
void dbg_print(const Vector_Imp<TYPE,ALLOC>& v)
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        for (size_t i = 0; i < v.size(); ++i) {
            dbg_print(v[i]);
        }
    }
    fflush(stdout);
}

// Generic debug print function (3-arguments).
template <class T>
void dbg_print(const char* s, const T& val, const char* nl)
{
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

BSLMF_ASSERT(!Vector_IsRandomAccessIterator<int>::VALUE);
BSLMF_ASSERT(Vector_IsRandomAccessIterator<bsl::vector<int>::iterator>::VALUE);

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;
static bslma::TestAllocator *globalAllocator_p,
                            *defaultAllocator_p,
                            *objectAllocator_p;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

                            // ====================
                            // class ExceptionGuard
                            // ====================

template <class VALUE_TYPE>
struct ExceptionGuard {
    // This scoped guard helps to verify the full guarantee of rollback in
    // exception-throwing code.

    // DATA
    int         d_lineNum;
    VALUE_TYPE  d_value;
    VALUE_TYPE *d_object_p;

  public:
    // CREATORS
    ExceptionGuard(VALUE_TYPE *object, const VALUE_TYPE& value, int line)
    : d_lineNum(line)
    , d_value(value)
    , d_object_p(object)
    {}

    ~ExceptionGuard() {
        if (d_object_p) {
            const int LINE = d_lineNum;
            LOOP_ASSERT(LINE, d_value == *d_object_p);
        }
    }

    // MANIPULATORS
    void release() {
        d_object_p = 0;
    }

    void resetValue(const VALUE_TYPE& value, int line) {
        d_lineNum = line;
        d_value = value;
    }
};

                               // =============
                               // class TestArg
                               // =============

template <int ID>
struct TestArg {
    // This very simple 'struct' is used purely to disambiguate types in
    // passing parameters to 'emplace' and 'emplace_back' due to the fact that
    // 'TestArg<ID1>' is a different type than 'TestArg<ID2>' if ID1 != ID2.
    // This class does not take an optional allocator.

    // DATA
    int d_value;

  public:

    // CREATORS
    TestArg(int value = -1);
        // Create an object having the specified 'value'.
};

// CREATORS
template <int ID>
TestArg<ID>::TestArg(int value)
{
    d_value = value;
}

typedef TestArg<2>  Arg2;
typedef TestArg<3>  Arg3;
typedef TestArg<4>  Arg4;
typedef TestArg<5>  Arg5;

const Arg2  VA2(2);
const Arg3  VA3(3);
const Arg4  VA4(4);
const Arg5  VA5(5);

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

    char             *d_data_p;
    Arg2              d_a2;
    Arg3              d_a3;
    Arg4              d_a4;
    Arg5              d_a5;
    int               d_numOfCtrArgs;
    bslma::Allocator *d_allocator_p;

#if defined(BDE_USE_ADDRESSOF)
    // PRIVATE ACCESSORS
    void operator&() const;     // = delete;
        // Suppress the use of address-of operator on this type.
#endif

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestType, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    TestType(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_numOfCtrArgs(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestType(char c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_numOfCtrArgs(1)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_numOfCtrArgs(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCopyCtorCalls;

        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
        d_numOfCtrArgs = original.d_numOfCtrArgs;
    }

    explicit
    TestType(char c, const Arg2& arg2, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_a2(arg2)
    , d_numOfCtrArgs(2)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    explicit
    TestType(char c,
             const Arg2& arg2,
             const Arg3& arg3,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_a2(arg2)
    , d_a3(arg3)
    , d_numOfCtrArgs(3)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    explicit
    TestType(char c,
             const Arg2& arg2,
             const Arg3& arg3,
             const Arg4& arg4,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_a2(arg2)
    , d_a3(arg3)
    , d_a4(arg4)
    , d_numOfCtrArgs(4)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    explicit
    TestType(char c,
             const Arg2& arg2,
             const Arg3& arg3,
             const Arg4& arg4,
             const Arg5& arg5,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_a2(arg2)
    , d_a3(arg3)
    , d_a4(arg4)
    , d_a5(arg5)
    , d_numOfCtrArgs(5)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
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
            d_data_p       = newData;
            *d_data_p      = *rhs.d_data_p;
            d_a2           = rhs.d_a2;
            d_a3           = rhs.d_a3;
            d_a4           = rhs.d_a4;
            d_a5           = rhs.d_a5;
            d_numOfCtrArgs = rhs.d_numOfCtrArgs;
        }
        return *this;
    }

    void setDatum(char c) {
        *d_data_p = c;
    }

    // ACCESSORS
    bslma::Allocator *allocator() const {
        return d_allocator_p;
    }

    int numOfArgs() const {
        return d_numOfCtrArgs;
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

    char value() const {
        return *d_data_p;
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
    struct DU {
        char  d_char;
        Arg2  d_a2;
        Arg3  d_a3;
        Arg4  d_a4;
        Arg5  d_a5;
        int   d_numOfCtrArgs;
    } d_u;
    char  d_fill[sizeof(TestType) - sizeof(struct DU)];

  public:
    // CREATORS
    TestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        d_u.d_numOfCtrArgs = 0;
        ++numDefaultCtorCalls;
    }

    explicit
    TestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
        d_u.d_numOfCtrArgs = 1;
        ++numCharCtorCalls;
    }

    TestTypeNoAlloc(const TestTypeNoAlloc&  original)
    {
        d_u.d_char = original.d_u.d_char;
        d_u.d_numOfCtrArgs = original.d_u.d_numOfCtrArgs;
        ++numCopyCtorCalls;
    }

    explicit
    TestTypeNoAlloc(char c, const Arg2& arg2)
    {
        d_u.d_char = c;
        d_u.d_a2   = arg2;
        d_u.d_numOfCtrArgs = 2;
        ++numCharCtorCalls;
    }

    explicit
    TestTypeNoAlloc(char c, const Arg2& arg2, const Arg3& arg3)
    {
        d_u.d_char = c;
        d_u.d_a2   = arg2;
        d_u.d_a3   = arg3;
        d_u.d_numOfCtrArgs = 3;
        ++numCharCtorCalls;
    }

    explicit
    TestTypeNoAlloc(char c,
                    const Arg2& arg2,
                    const Arg3& arg3,
                    const Arg4& arg4)
    {
        d_u.d_char = c;
        d_u.d_a2   = arg2;
        d_u.d_a3   = arg3;
        d_u.d_a4   = arg4;
        d_u.d_numOfCtrArgs = 4;
        ++numCharCtorCalls;
    }

    explicit
    TestTypeNoAlloc(char c,
                    const Arg2& arg2,
                    const Arg3& arg3,
                    const Arg4& arg4,
                    const Arg5& arg5)
    {
        d_u.d_char = c;
        d_u.d_a2   = arg2;
        d_u.d_a3   = arg3;
        d_u.d_a4   = arg4;
        d_u.d_a5   = arg5;
        d_u.d_numOfCtrArgs = 5;
        ++numCharCtorCalls;
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
        d_u.d_a2   = rhs.d_u.d_a2;
        d_u.d_a3   = rhs.d_u.d_a3;
        d_u.d_a4   = rhs.d_u.d_a4;
        d_u.d_a5   = rhs.d_u.d_a5;
        d_u.d_numOfCtrArgs = rhs.d_u.d_numOfCtrArgs;
        return *this;
    }

    // ACCESSORS
    int numOfArgs() const {
        return d_u.d_numOfCtrArgs;
    }

    void print() const
    {
        ASSERT(isalpha(d_u.d_char));
        printf("%c (int: %d)\n", d_u.d_char, (int)d_u.d_char);
    }

    char value() const
    {
        return d_u.d_char;
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

// TestType-specific print function.
void dbg_print(const TestTypeNoAlloc& rhs) {
    printf("%c", rhs.value());
    fflush(stdout);
}

                       // =============================
                       // class BitwiseMoveableTestType
                       // =============================

class BitwiseMoveableTestType : public TestType {
    // This test type is identical to 'TestType' except that it has the
    // bit-wise moveable trait.  All members are inherited.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMoveableTestType,
                                   bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseMoveableTestType,
                                   bslmf::IsBitwiseMoveable);

    // CREATORS
    explicit
    BitwiseMoveableTestType(bslma::Allocator *ba = 0)
    : TestType(ba)
    {
    }

    explicit
    BitwiseMoveableTestType(char c, bslma::Allocator *ba = 0)
    : TestType(c, ba)
    {
    }

    BitwiseMoveableTestType(const BitwiseMoveableTestType&  original,
                            bslma::Allocator               *ba = 0)
    : TestType(original, ba)
    {
    }

    explicit
    BitwiseMoveableTestType(char c, const Arg2& arg2, bslma::Allocator *ba = 0)
    : TestType(c, arg2, ba)
    {
    }

    explicit
    BitwiseMoveableTestType(char c,
                            const Arg2& arg2,
                            const Arg3& arg3,
                            bslma::Allocator *ba = 0)
    : TestType(c, arg2, arg3, ba)
    {
    }

    explicit
    BitwiseMoveableTestType(char c,
                            const Arg2& arg2,
                            const Arg3& arg3,
                            const Arg4& arg4,
                            bslma::Allocator *ba = 0)
    : TestType(c, arg2, arg3, arg4, ba)
    {
    }

    explicit
    BitwiseMoveableTestType(char c,
                            const Arg2& arg2,
                            const Arg3& arg3,
                            const Arg4& arg4,
                            const Arg5& arg5,
                            bslma::Allocator *ba = 0)
    : TestType(c, arg2, arg3, arg4, arg5, ba)
    {
    }
};

                       // =============================
                       // class BitwiseCopyableTestType
                       // =============================

class BitwiseCopyableTestType : public TestTypeNoAlloc {
    // This test type is identical to 'TestTypeNoAlloc' except that it has the
    // bit-wise copyable and bit-wise equality comparable traits.  All members
    // are inherited.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyableTestType,
                                   bsl::is_trivially_copyable);

    // CREATORS
    BitwiseCopyableTestType()
    : TestTypeNoAlloc()
    {
    }

    explicit
    BitwiseCopyableTestType(char c)
    : TestTypeNoAlloc(c)
    {
    }

    BitwiseCopyableTestType(const BitwiseCopyableTestType&  original)
    : TestTypeNoAlloc(original.value())
    {
    }

    explicit
    BitwiseCopyableTestType(char c, const Arg2& arg2)
    : TestTypeNoAlloc(c, arg2)
    {
    }

    explicit
    BitwiseCopyableTestType(char c, const Arg2& arg2, const Arg3& arg3)
    : TestTypeNoAlloc(c, arg2, arg3)
    {
    }

    explicit
    BitwiseCopyableTestType(char c,
                            const Arg2& arg2,
                            const Arg3& arg3,
                            const Arg4& arg4)
    : TestTypeNoAlloc(c, arg2, arg3, arg4)
    {
    }

    explicit
    BitwiseCopyableTestType(char c,
                            const Arg2& arg2,
                            const Arg3& arg3,
                            const Arg4& arg4,
                            const Arg5& arg5)
    : TestTypeNoAlloc(c, arg2, arg3, arg4, arg5)
    {
    }
};

                     // =================================
                     // class BitwiseEqComparableTestType
                     // =================================

class BitwiseEqComparableTestType {
    // This test type is identical to 'TestTypeNoAlloc' except that it has the
    // bit-wise equality comparable traits, and has no 'operator=='.

    // DATA
    char d_char;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseEqComparableTestType,
                                   bslmf::IsBitwiseEqualityComparable);

    // CREATORS
    BitwiseEqComparableTestType()
    {
        ++numDefaultCtorCalls;
    }

    explicit
    BitwiseEqComparableTestType(char c)
    {
        ++numCharCtorCalls;
        d_char = c;
    }

    BitwiseEqComparableTestType(const BitwiseEqComparableTestType&  original)
    {
        ++numCopyCtorCalls;
        d_char = original.value();
    }

    // MANIPULATORS
    BitwiseEqComparableTestType&
    operator=(const BitwiseEqComparableTestType& rhs)
    {
        ++numAssignmentCalls;
        d_char = rhs.d_char;
        return *this;
    }

    char value() const
    {
        return d_char;
    }

    void print() const
    {
        ASSERT(isalpha(d_char));
        printf("%c (int: %d)\n", d_char, (int)d_char);
    }
};

// TestType-specific print function.
void dbg_print(const BitwiseEqComparableTestType& rhs) {
    printf("%c", rhs.value());
    fflush(stdout);
}

                               // ==============
                               // class CharList
                               // ==============

template <class TYPE>
class CharList {
    // This array class is a simple wrapper on a 'char' array offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically an *input* iterator and its value type is the parameterized
    // 'TYPE'.

    // DATA
    Vector_Imp<TYPE> d_value;

  public:
    // TYPES
    typedef bslstl::ForwardIterator<const TYPE, const TYPE*>  const_iterator;
        // Input iterator.

    // CREATORS
    CharList() {}
    CharList(const Vector_Imp<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE>
CharList<TYPE>::CharList(const Vector_Imp<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE&
CharList<TYPE>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE>
typename CharList<TYPE>::const_iterator
CharList<TYPE>::begin() const {
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename CharList<TYPE>::const_iterator
CharList<TYPE>::end() const {
    return const_iterator(d_value.end());
}

                              // ===============
                              // class CharArray
                              // ===============

template <class TYPE>
class CharArray {
    // This array class is a simple wrapper on a vector offering an input
    // iterator access via the 'begin' and 'end' accessors.  The iterator is
    // specifically a *random-access* iterator and its value type is the
    // parameterized 'TYPE'.

    // DATA
    Vector_Imp<TYPE>  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    CharArray() {}
    CharArray(const Vector_Imp<TYPE>& value);

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE>
CharArray<TYPE>::CharArray(const Vector_Imp<TYPE>& value)
: d_value(value)
{
}

// ACCESSORS
template <class TYPE>
const TYPE& CharArray<TYPE>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE>
typename CharArray<TYPE>::const_iterator CharArray<TYPE>::begin() const {
    return const_iterator(d_value.begin());
}

template <class TYPE>
typename CharArray<TYPE>::const_iterator CharArray<TYPE>::end() const {
    return const_iterator(d_value.end());
}

                              // ========================
                              // class FixedArrayIterator
                              // ========================

// FORWARD DECLARATIONS
template <class TYPE>
class FixedArray;

template <class TYPE>
class FixedArrayIterator {
    // This testing class provides simple, fixed size array with a non-pointer
    // iterator for use in test case 24.

    // DATA
    FixedArray<TYPE> *d_array_p;
    int               d_index;

  public:
    typedef std::forward_iterator_tag  iterator_category;
    typedef int                        difference_type;
    typedef int                        size_type;
    typedef TYPE                       value_type;
    typedef TYPE                      *pointer;
    typedef TYPE&                      reference;

    // CREATORS
    FixedArrayIterator(const FixedArray<TYPE> *array, int index);

    FixedArrayIterator(const FixedArrayIterator<TYPE>& obj);

    // MANIPULATORS
    FixedArrayIterator<TYPE>& operator++();

    // ACCESSORS
    TYPE& operator*() const;

    bool isEqual(const FixedArrayIterator<TYPE>& obj) const;
};

// FREE OPERATORS
template <class TYPE>
bool operator==(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs);

template <class TYPE>
bool operator!=(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs);

// CREATORS
template <class TYPE>
FixedArrayIterator<TYPE>::FixedArrayIterator(const FixedArray<TYPE> *array,
                                             int                     index)
: d_array_p(const_cast<FixedArray<TYPE> *>(array))
, d_index(index)
{
}

template <class TYPE>
FixedArrayIterator<TYPE>::FixedArrayIterator(
                                           const FixedArrayIterator<TYPE>& obj)
: d_array_p(obj.d_array_p)
, d_index(obj.d_index)
{
}

// MANIPULATORS
template <class TYPE>
FixedArrayIterator<TYPE>& FixedArrayIterator<TYPE>::operator++()
{
    ++d_index;
    return *this;
}

// ACCESSORS
template <class TYPE>
TYPE& FixedArrayIterator<TYPE>::operator*() const
{
    return d_array_p->operator[](d_index);
}

template <class TYPE>
bool FixedArrayIterator<TYPE>::
                             isEqual(const FixedArrayIterator<TYPE>& obj) const
{
    return (d_array_p == obj.d_array_p && d_index == obj.d_index);
}

// FREE OPERATORS
template <class TYPE>
bool operator==(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class TYPE>
bool operator!=(const FixedArrayIterator<TYPE>& lhs,
                const FixedArrayIterator<TYPE>& rhs)
{
    return !lhs.isEqual(rhs);
}

                              // ================
                              // class FixedArray
                              // ================

#define k_FIXED_ARRAY_SIZE 100

template <class TYPE>
class FixedArray {
    // This testing class provides simple, fixed size array with a non-pointer
    // iterator for use in test case 24.

  public:
    // PUBLIC TYPES
    typedef FixedArrayIterator<TYPE> iterator;

  private:
    // DATA
    TYPE d_data[k_FIXED_ARRAY_SIZE];
    int  d_length;

  public:
    // CREATORS
    FixedArray();

    // MANIPULATORS
    TYPE& operator[](int index);

    void push_back(const TYPE& value);

    // ACCESSORS
    iterator begin() const;

    iterator end() const;
};

// CREATORS
template <class TYPE>
FixedArray<TYPE>::FixedArray()
: d_length(0)
{
}

// MANIPULATORS
template <class TYPE>
TYPE& FixedArray<TYPE>::operator[](int index)
{
    return d_data[index];
}

template <class TYPE>
void FixedArray<TYPE>::push_back(const TYPE& value)
{
    d_data[d_length++] = value;
}

// ACCESSORS
template <class TYPE>
typename FixedArray<TYPE>::iterator FixedArray<TYPE>::begin() const {
    return typename FixedArray<TYPE>::iterator(this, 0);
}

template <class TYPE>
typename FixedArray<TYPE>::iterator FixedArray<TYPE>::end() const {
    return typename FixedArray<TYPE>::iterator(this, d_length);
}

                              // ====================
                              // class LimitAllocator
                              // ====================

template <class ALLOC>
class LimitAllocator : public ALLOC {

  public:
    // TYPES
    typedef typename ALLOC::value_type        value_type;
    typedef typename ALLOC::pointer           pointer;
    typedef typename ALLOC::const_pointer     const_pointer;
    typedef typename ALLOC::reference         reference;
    typedef typename ALLOC::const_reference   const_reference;
    typedef typename ALLOC::size_type         size_type;
    typedef typename ALLOC::difference_type   difference_type;

    template <class OTHER_TYPE> struct rebind {
        // It is better not to inherit the 'rebind' template, or else
        // 'rebind<X>::other' would be 'ALLOC::rebind<OTHER_TYPE>::other'
        // instead of 'LimitAlloc<X>'.

        typedef LimitAllocator<typename ALLOC::template
                                             rebind<OTHER_TYPE>::other > other;
    };

  private:
    // PRIVATE TYPES
    typedef ALLOC AllocBase;

    // DATA
    size_type d_limit;

  public:
    // CREATORS
    LimitAllocator()
    : d_limit(-1) {}

    LimitAllocator(BloombergLP::bslma::Allocator *mechanism)
    : AllocBase(mechanism), d_limit(-1) { }

    LimitAllocator(const ALLOC& rhs)
    : AllocBase((const AllocBase&) rhs), d_limit(-1) { }

    ~LimitAllocator() { }

    // MANIPULATORS
    void setMaxSize(size_type maxSize) { d_limit = maxSize; }

    // ACCESSORS
    size_type max_size() const { return d_limit; }
};

namespace BloombergLP {
namespace bslmf {

template <class ALLOCATOR>
struct IsBitwiseMoveable<LimitAllocator<ALLOCATOR> >
    : IsBitwiseMoveable<ALLOCATOR>
{};

}  // close namespace bslmf

}  // close enterprise namespace

//=============================================================================
//                            Test Case 22
//=============================================================================

template <int N>
int myFunc()
{
    return N;
}

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A .. E] correspond to arbitrary (but unique) char
    // values to be appended to the 'bslstl::Vector<T>' object.  A tilde ('~')
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
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'H'
    //                                      // unique but otherwise arbitrary
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
    //-------------------------------------------------------------------------

    // TYPES
    typedef bsl::vector<TYPE,ALLOC>  Obj;
        // Type under testing.

    typedef typename Obj::iterator                iterator;
    typedef typename Obj::const_iterator          const_iterator;
    typedef typename Obj::reverse_iterator        reverse_iterator;
    typedef typename Obj::const_reverse_iterator  const_reverse_iterator;
        // Shorthand.

    // TEST APPARATUS
    static int getValues(const TYPE **values);
        // Load the specified 'values' with the address of an array containing
        // initialized values of the parameterized 'TYPE' and return the length
        // of that array.

    static int ggg(Obj *object, const char *spec, int verboseFlag = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'push_back' and
        // white-box manipulator 'clear'.  Optionally specify a zero
        // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
        // index of the first invalid character, and a negative value
        // otherwise.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static Vector_Imp<TYPE> gV(const char *spec);
        // Return, by value, a new vector corresponding to the specified
        // 'spec'.

    static void stretch(Obj         *object,
                        std::size_t  size,
                        const TYPE&  value = TYPE());
        // Using only primary manipulators, extend the length of the specified
        // 'object' by the specified 'size' by adding copies of the specified
        // 'value'.  The resulting value is not specified.  The behavior is
        // undefined unless 0 <= size.

    static void stretchRemoveAll(Obj         *object,
                                 std::size_t  size,
                                 const TYPE&  value = TYPE());
        // Using only primary manipulators, extend the capacity of the
        // specified 'object' to (at least) the specified 'size' by adding
        // copies of the optionally specified 'value'; then remove all elements
        // leaving 'object' empty.  The behavior is undefined unless
        // '0 <= size'.

    static iterator testEmplace(Obj&         object,
                                int&         numOfArgs,
                                const size_t position,
                                const TYPE&  value);
        // Invoke indirectly the 'emplace' method of the specified vector
        // 'object', in-place constructing the specified 'value' at the
        // specified 'position' by calling the constructor of the parameterized
        // 'TYPE' that has the specified 'numOfArgs' arguments (excluding
        // allocator).

    static void testEmplaceBack(Obj&         object,
                                int&         numOfArgs,
                                const TYPE&  value);
        // Invoke indirectly the 'emplace_back' method of the specified vector
        // 'object', in-place constructing the specified 'value' to the back of
        // the vector by calling the constructor of the parameterized 'TYPE'
        // that has the specified 'numOfArgs' arguments (excluding allocator).


    // TEST CASES
    template <class CONTAINER>
    static void testCaseM1Range(const CONTAINER&);
        // Performance test for operators that take a range of inputs.

    static void testCaseM1();
        // Performance test.

    static void testCase22();
        // Test overloaded new/delete.

    static void testCase21();
        // Test proper use of 'std::length_error'.

    static void testCase20();
        // Test comparison free operators.

    static void testCase19();
        // Test 'swap' member.

    static void testCase18();
        // Test 'erase' and 'pop_back'.

    static void testCase18Negative();
        // Negative testing for 'erase' and 'pop_back'.

    static void testCase17();
        // Test 'insert' members, and move 'push_back' and 'insert' members.

    static void testCase17Variadic(int numOfArgs);
        // Test 'emplace' and 'emplace_back' templates and their move members.

    template <class CONTAINER>
    static void testCase17Range(const CONTAINER&);
        // Test 'insert' member template.

    static void testCase17Negative();
        // Negative testing for 'insert'.

    static void testCase16();
        // Test iterators.

    static void testCase15();
        // Test element access.

    static void testCase15Negative();
        // Negative test for element access.

    static void testCase14();
        // Test reserve and capacity-related methods.

    static void testCase13();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testCase13Range(const CONTAINER&);
        // Test 'assign' member template.

    template <class CONTAINER>
    static void testCase13Negative(const CONTAINER&);
        // Negative-test 'assign' members.

    static void testCase12();
        // Test user-supplied constructors.

    template <class CONTAINER>
    static void testCase12Range(const CONTAINER&);
        // Test user-supplied constructor templates.

    static void testCase11();
        // Test allocator-related concerns.

    static void testCase10();
        // Test streaming functionality.  This test case tests nothing.

    static void testCase9();
        // Test assignment operator ('operator=').

    static void testCase8();
        // Test generator function 'g'.

    static void testCase7();
        // Test copy constructor.

    static void testCase6();
        // Test equality operator ('operator==').

    static void testCase5();
        // Test output (<<) operator.  This test case tests nothing.

    static void testCase4();
        // Test basic accessors ('size' and 'operator[]').

    static void testCase3();
        // Test generator functions 'ggg' and 'gg'.

    static void testCase2();
        // Test primary manipulators ('push_back' and 'clear').

    static void testCase1();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <char N>
char TestFunc()
{
    return N;
}

typedef char (*charFnPtr) ();

template <class TYPE>
void makeElement(TYPE *result, char code)
{
    *result = TYPE(code);
}


template <class TYPE>
void makeElement(TYPE **result, char code)
{
    *result = reinterpret_cast<TYPE *>(code);
}

void makeElement(charFnPtr *result, char code)
{
    switch (code) {
      case VA: {
        *result = TestFunc<'A'>;
      }  break;
      case VB: {
        *result = TestFunc<'B'>;
      } break;
      case VC: {
        *result = TestFunc<'C'>;
      } break;
      case VD: {
        *result = TestFunc<'D'>;
      } break;
      default: {
        *result = TestFunc<'E'>;
      }
    }
}

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::getValues(const TYPE **valuesPtr)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    static TYPE values[5]; // avoid DEFAULT_VALUE and UNINITIALIZED_VALUE
    makeElement(values,     VA);
    makeElement(values + 1, VB);
    makeElement(values + 2, VC);
    makeElement(values + 3, VD);
    makeElement(values + 4, VE);

    const int NUM_VALUES = 5;

    *valuesPtr = values;
    return NUM_VALUES;
}

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::ggg(Obj *object,
                                const char    *spec,
                                int            verboseFlag)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'E') {
            object->push_back(VALUES[spec[i] - 'A']);
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
   return SUCCESS;
}

template <class TYPE, class ALLOC>
bsl::vector<TYPE,ALLOC>& TestDriver<TYPE,ALLOC>::gg(Obj        *object,
                                                   const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class ALLOC>
bsl::vector<TYPE,ALLOC> TestDriver<TYPE,ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class TYPE, class ALLOC>
Vector_Imp<TYPE>  TestDriver<TYPE,ALLOC>::gV(const char *spec)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    Vector_Imp<TYPE> result;
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
void TestDriver<TYPE,ALLOC>::stretch(Obj         *object,
                                     std::size_t  size,
                                     const TYPE&  value)
{
    ASSERT(object);
    ASSERT(0 <= static_cast<int>(size));
    for (std::size_t i = 0; i < size; ++i) {
        object->push_back(value);
    }
    ASSERT(object->size() >= size);
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::stretchRemoveAll(Obj         *object,
                                              std::size_t  size,
                                              const TYPE&  value)
{
    ASSERT(object);
    ASSERT(0 <= static_cast<int>(size));
    stretch(object, size, value);
    object->clear();
    ASSERT(0 == object->size());
}

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testCaseM1Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // PERFORMANCE TEST (RANGE)
    // We have the following concerns:
    //   1) That performance does not regress between versions.
    //   2) That no surprising performance (both extremely fast or slow) is
    //      detected, which might be indicating missed optimizations or
    //      inadvertent loss of performance (e.g., by wrongly setting the
    //      capacity and triggering too frequent reallocations).
    //   3) That small "improvements" can be tested w.r.t. to performance,
    //      in a uniform benchmark (e.g., measuring the overhead of allocating
    //      for empty strings).
    //
    // Plan:  We follow a simple benchmark which performs the operation under
    //   timing test in a loop.  Specifically, we wish to measure the time
    //   taken by:
    //     C1) The constructors.
    //     A1) The 'assign' operation.
    //     I1) The 'insert' operation at the end.
    //     I2) The 'insert' operation at the front.
    //     I3) The 'insert' operation everywhere.
    //     E1) The 'erase' operation.
    // --------------------------------------------------------------------

    bsls::Stopwatch t;

    // DATA INITIALIZATION (NOT TIMED)
    const TYPE         *values      = 0;
    const TYPE *const&  VALUES      = values;
    const int           NUM_VALUES  = getValues(&values);
    const int           LENGTH      = 1000;
    const int           NUM_VECTOR  = 300;

    const char         *SPECREF     = "ABCDE";
    const size_t        SPECREF_LEN = strlen(SPECREF);
    char                SPEC[LENGTH + 1];

    for (int i = 0; i < LENGTH; ++i) {
        SPEC[i] = SPECREF[i % SPECREF_LEN];
    }
    SPEC[LENGTH] = '\0';
    CONTAINER mU(gV(SPEC)); const CONTAINER& U = mU;

    void * addr = bslma::Default::defaultAllocator()->allocate(
                                       NUM_VECTOR * LENGTH * 2 * sizeof(TYPE));
    bslma::Default::defaultAllocator()->deallocate(addr);

    printf("\tC1) Constructor:\n");
    {
        // Vector_Imp(f, l)
        double time = 0.;
        bsls::ObjectBuffer<Obj>  vectorBuffers[NUM_VECTOR];

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            new(&vectorBuffers[i]) Obj(U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tVector_Imp(f,l):             %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectorBuffers[i].object().~Obj();
        }
    }

    printf("\tA1) Assign:\n");
    {
        // assign(f, l)
        double time = 0.;
        Obj *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_VECTOR) / 2, j = 0;
                 i < (LENGTH + NUM_VECTOR) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->assign(U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tassign(f,l):                    %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }

    printf("\tI1) Insert (at front):\n");
    {
        // insert(p, f, l)
        double time = 0.;
        Obj *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_VECTOR) / 2, j = 0;
                 i < (LENGTH + NUM_VECTOR) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->insert(vectors[i]->begin(), U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,f,l):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }

    printf("\tI2) Insert (at back):\n");
    {
        // insert(p, f, l)
        double time = 0.;
        Obj *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_VECTOR) / 2, j = 0;
                 i < (LENGTH + NUM_VECTOR) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->insert(vectors[i]->end(), U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,f,l):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }

    printf("\tI3) Insert (everywhere):\n");
    {
        // insert(p, f, l)
        double time = 0.;
        Obj    *vectors[NUM_VECTOR];

        ASSERT(LENGTH >= NUM_VECTOR);
        // Spread out the initial lengths.
        for (int i = 0; i < NUM_VECTOR; ++i) {
            // NOTE: Uses NUM_VECTOR for length.
            vectors[i] = new Obj(NUM_VECTOR, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR; ++i) {
            vectors[i]->insert(vectors[i]->begin() + i, U.begin(), U.end());
        }
        time += t.elapsedTime();

        printf("\t\tinsert(p,f,l):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR; ++i) {
            delete vectors[i];
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCaseM1()
{
    // --------------------------------------------------------------------
    // PERFORMANCE TEST
    // We have the following concerns:
    //   1) That performance does not regress between versions.
    //   2) That no surprising performance (both extremely fast or slow) is
    //      detected, which might be indicating missed optimizations or
    //      inadvertent loss of performance (e.g., by wrongly setting the
    //      capacity and triggering too frequent reallocations).
    //   3) That small "improvements" can be tested w.r.t. to performance,
    //      in a uniform benchmark (e.g., measuring the overhead of allocating
    //      for empty strings).
    //
    // Plan:  We follow a simple benchmark which performs the operation under
    //   timing test in a loop.  Specifically, we wish to measure the time
    //   taken by:
    //     C1) The various constructors.
    //     C2) The copy constructor.
    //     A1) The copy assignment.
    //     A2) The 'assign' operations.
    //     P1) The 'push_back' operation.
    //     P2) The 'push_front' operation.
    //     P3) The 'pop_back' operation.
    //     P4) The 'pop_front' operation.
    //     I1) The 'insert' operation in its various forms, at the front
    //     I2) The 'insert' operation in its various forms, at the back
    //     I3) The 'insert' operation in its various forms.
    //     E1) The 'erase' operation in its various forms.
    // --------------------------------------------------------------------

    bsls::Stopwatch t;

    // DATA INITIALIZATION (NOT TIMED)
    const TYPE         *values      = 0;
    const TYPE *const&  VALUES      = values;
    const int           NUM_VALUES  = getValues(&values);
    const int           LENGTH_S    = bsl::is_same<TYPE,char>::value
                                    ? 5000 : 1000;
    const int           LENGTH_L    = bsl::is_same<TYPE,char>::value
                                    ? 20000 : 5000;
    const int           NUM_VECTOR_S = bsl::is_same<TYPE,char>::value
                                     ? 500 : 100;
    const int           NUM_VECTOR_L = bsl::is_same<TYPE,char>::value
                                     ? 5000 : 1000;

    // INITIAL ALLOCATION (NOT TIMED)
    void * addr = bslma::Default::defaultAllocator()->allocate(
                                   NUM_VECTOR_L * LENGTH_L * 2 * sizeof(TYPE));
    bslma::Default::defaultAllocator()->deallocate(addr);

    // C1) CONSTRUCTORS
    printf("\tC1) Constructors:\n");
    {
        // Vector_Imp()
        double time = 0.;

        t.reset(); t.start();
        Obj *vectors = new Obj[NUM_VECTOR_L];
        time = t.elapsedTime();

        printf("\t\tVector_Imp():                %1.6fs\n", time);
        delete[] vectors;
    }
    {
        // Vector_Imp(n)
        double time = 0.;
        bsls::ObjectBuffer<Obj> vectorBuffers[NUM_VECTOR_L];

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            new (&vectorBuffers[i]) Obj(LENGTH_S);
        }
        time = t.elapsedTime();

        printf("\t\tVector_Imp(n):               %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            vectorBuffers[i].object().~Obj();
        }
    }
    {
        // Vector_Imp(n,v)
        double time = 0.;
        bsls::ObjectBuffer<Obj> vectorBuffers[NUM_VECTOR_L];

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            new (&vectorBuffers[i]) Obj(LENGTH_S, VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tVector_Imp(n,v):             %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            vectorBuffers[i].object().~Obj();
        }
    }
    // C2) COPY CONSTRUCTOR
    printf("\tC2) Copy Constructors:\n");
    {
        // Vector_Imp(Vector_Imp)
        double time = 0.;
        Obj  original(LENGTH_L);
        bsls::ObjectBuffer<Obj> vectorBuffers[NUM_VECTOR_L];

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            new (&vectorBuffers[i]) Obj(original);
        }
        time = t.elapsedTime();

        printf("\t\tVector_Imp(d):               %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectorBuffers[i].object().~Obj();
        }
    }

    // A1) COPY ASSIGNMENT
    printf("\tA1) Copy Assginment:\n");
    {
        // operator=(Vector_Imp)
        double time = 0;
        Obj deq(LENGTH_L / 2);
        Obj *vectors[NUM_VECTOR_L];

        ASSERT(LENGTH_L >= NUM_VECTOR_L);
        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_L) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_L) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            *vectors[i] = deq;
        }
        time = t.elapsedTime();

        printf("\t\toperator=(d):                   %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            ASSERT(*vectors[i] == deq);
            delete vectors[i];
        }
    }

    // A2) ASSIGN
    printf("\tA2) Assign:\n");
    {
        // assign(n)
        double time = 0;
        Obj *vectors[NUM_VECTOR_L];

        ASSERT(LENGTH_L >= NUM_VECTOR_L);
        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_L) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_L) / 2;
                 ++i, ++j) {
            vectors[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }
        const TYPE val    = TYPE();
        const int  length = LENGTH_L / 2;

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            vectors[i]->assign(length, val);
        }
        time = t.elapsedTime();

        printf("\t\tassign(n):                      %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_L; ++i) {
            delete vectors[i];
        }
    }

    // P1) PUSH_BACK
    printf("\tP1) Push_back:\n");
    {
        // push_back(v)
        double time = 0;
        Obj *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            // Push in 1000 objects.
            for (int j = 0; j < LENGTH_L; ++j) {
                vectors[i]->push_back(VALUES[j % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tpush_back(v):                   %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // P1) POP_BACK
    printf("\tP3) Pop_back:\n");
    {
        // pop_back()
        double time = 0;
        Obj    *vectors[NUM_VECTOR_S];
        size_t  sizes[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            // Pop out all objects.
            for (size_t j = 0; j < sizes[i]; ++j) {
                vectors[i]->pop_back();
            }
        }
        time = t.elapsedTime();

        printf("\t\tpop_back():                     %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // I1) INSERT (FRONT)
    printf("\tI1) Insert (at front):\n");
//  {
//      // Takes too long...
//      // insert(p,v)
//      double  time = 0;
//      Obj    *vectors[NUM_VECTOR_S];
//
//      // Spread out the initial lengths.
//      for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
//               i < (LENGTH_S + NUM_VECTOR_S) / 2;
//               ++i, ++j) {
//          vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
//      }
//
//      t.reset(); t.start();
//      for (int i = 0; i < NUM_VECTOR_S; ++i) {
//          vectors[i]->insert(vectors[i]->begin(), VALUES[i % NUM_VALUES]);
//      }
//      time = t.elapsedTime();
//
//      printf("\t\tinsert(p,v):                    %1.6fs\n", time);
//      for (int i = 0; i < NUM_VECTOR_S; ++i) {
//          delete vectors[i];
//      }
//  }
    {
        // insert(n,p,v)
        double time = 0;
        Obj  *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectors[i]->insert(vectors[i]->begin(), LENGTH_L,
                               VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // I2) INSERT (BACK)
    printf("\tI2) Insert (at back):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            for (int j = 0; j < LENGTH_S; ++j) {
                vectors[i]->insert(vectors[i]->end(), VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):                    %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }
    {
        // insert(n,p,v)
        double time = 0;
        Obj  *vectors[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectors[i]->insert(vectors[i]->end(), LENGTH_L,
                               VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // I3) INSERT
    printf("\tI3) Insert (everywhere):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];
        size_t  sizes[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j]  = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            for (size_t j = 0; j < sizes[i]; ++j) {
                vectors[i]->insert(vectors[i]->begin() + j,
                                   VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):                    %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }
    {
        // insert(n,p,v)
        double time = 0;
        Obj    *vectors[NUM_VECTOR_S];
        size_t  sizes[NUM_VECTOR_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_VECTOR_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_VECTOR_S) / 2;
                 ++i, ++j) {
            vectors[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            // Insert in middle.
            vectors[i]->insert(vectors[i]->begin() + sizes[i] / 2, LENGTH_S,
                               VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):                  %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }

    // E1) ERASE
    printf("\tE1) Erase:\n");
    {
        // erase(p)
        double  time = 0;
        Obj    *vectors[NUM_VECTOR_S];

        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            vectors[i] = new Obj(LENGTH_S * 2, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            for (int j = 0; j < LENGTH_S; ++j) {
                vectors[i]->erase(vectors[i]->begin() + j);
            }
        }
        time = t.elapsedTime();

        printf("\t\terase(p):                       %1.6fs\n", time);
        for (int i = 0; i < NUM_VECTOR_S; ++i) {
            delete vectors[i];
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase22()
{
    // --------------------------------------------------------------------
    // TESTING OVERLOADED NEW/DELETE
    //
    // Concern:
    //: 1 Overloaded new is not used on construction.
    //:
    //: 2 Overloaded new is not used on insert.
    //:
    //: 3 Overloaded delete is not used on destruction.
    //
    // Plan:
    //: 1 Use a type with overloaded new and delete that will assert when
    //:   new/delete is called.
    //:
    //: 2 Construct vectors of that type with multiple elements in the
    //:   vector using different variations of the constructor.  Notice that
    //:   there is no need for other verification as the type should assert if
    //:   the overload new is used.
    //:
    //: 3 Insert an element into the vector.
    //:
    //: 4 Destroy any vecto5rs that was created.
    //
    // Testing:
    //  CONCERN: Vector support types with overloaded new/delete
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    if (verbose) printf("\nTESTING OVERLOADED NEW/DELETE"
                        "\n=============================\n");

    if (veryVerbose) printf(
     "\nTesting with vector(size_type, const VALUE_TYPE&, const ALLOCATOR&\n");
    {
        Obj mX(1, TYPE(), &testAllocator);  const Obj& X = mX;
        ASSERT(TYPE() == X[0]);

        Obj mY(2, TYPE(), &testAllocator);  const Obj& Y = mY;
        ASSERT(TYPE() == Y[0]);
        ASSERT(TYPE() == Y[1]);
    }

    if (veryVerbose) printf(
                       "\nTesting with other constructors and manipulators\n");
    {
        TYPE element;
        Obj mX(BSLS_UTIL_ADDRESSOF(element),
               BSLS_UTIL_ADDRESSOF(element) + 1,
               &testAllocator);
        const Obj& X = mX;
        (void) X;

        Obj mY(mX, &testAllocator);  const Obj& Y = mY;
        ASSERT(TYPE() == Y[0]);

        mX.push_back(TYPE());
        mX.resize(3);
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase21()
{
    // --------------------------------------------------------------------
    // TESTING 'std::length_error'
    //
    // Concerns:
    //   1) That any call to a constructor, 'assign', 'push_back' or 'insert'
    //      which would result in a value exceeding 'max_size()' throws
    //      'std::length_error'.
    //   2) That the 'max_size()' taken into consideration is that of the
    //      allocator, and not an absolute constant.
    //   3) That the value of the vector is unchanged if an exception is
    //      thrown.
    //   4) That integer overflows are correctly handled when 'length_error'
    //      exceeds 'Obj::max_size()' (which is the largest representable
    //      size_type).
    //
    // Plan:
    //   For concern 2, we use an allocator wrapper that provides the same
    //   functionality as 'ALLOC' but changes the return value of 'max_size()'
    //   to a 'limit' value settable at runtime.  Note that the operations
    //   throw unless 'length <= limit'.
    //
    //   Construct objects with value large enough that the constructor throws.
    //   For 'assign', 'insert', 'push_back', we construct a small (non-empty)
    //   object, and use the corresponding function to request an increase in
    //   size that is guaranteed to result in a value exceeding 'max_size()'.
    //
    // Testing:
    //   Proper use of 'std::length_error'
    // ------------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    LimitAllocator<ALLOC> a(&testAllocator);
    a.setMaxSize((size_t)-1);

    const int LENGTH = 32;
    typedef Vector_Imp<TYPE,LimitAllocator<ALLOC> > LimitObj;

    LimitObj mY(LENGTH, DEFAULT_VALUE);  // does not throw
    const LimitObj& Y = mY;

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nConstructor 'vector(n, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        LOOP2_ASSERT(limit, exceptionCaught,
                     (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'vector(n, T x, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        LOOP2_ASSERT(limit, exceptionCaught,
                     (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nConstructor 'vector<Iter>(f, l, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(Y.begin(), Y.end(), a);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        LOOP2_ASSERT(limit, exceptionCaught,
                     (limit < LENGTH) == exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'resize'.\n");
    {
        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a);

                mX.resize(LENGTH, DEFAULT_VALUE);
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'assign'.\n");

    for (int assignMethod = 0; assignMethod <= 1; ++assignMethod) {

        if (veryVerbose) {
            switch (assignMethod) {
                case 0: printf("\tWith assign(n, T x).\n"); break;
                case 1: printf("\tWith assign<Iter>(f, l).\n"); break;
                default: ASSERT(0);
            };
        }

        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a);

                switch (assignMethod) {
                    case 0: {
                                mX.assign(LENGTH, DEFAULT_VALUE);
                    } break;
                    case 1: {
                                mX.assign(Y.begin(), Y.end());
                    } break;
                    default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'insert'.\n");

    for (int insertMethod = 0; insertMethod <= 3; ++insertMethod) {

        if (verbose) {
            switch (insertMethod) {
                case 0: printf("\tWith push_back(c).\n");             break;
                case 1: printf("\tWith insert(p, T x).\n");           break;
                case 2: printf("\tWith insert(p, n, T x).\n");        break;
                case 3: printf("\tWith insert<Iter>(p, f, l).\n");    break;
                default: ASSERT(0);
            };
        }

        for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
            bool exceptionCaught = false;
            a.setMaxSize(limit);

            if (veryVerbose)
                printf("\t\tWith max_size() equal to limit = %d\n", limit);

            try {
                LimitObj mX(a); const LimitObj& X = mX;

                switch (insertMethod) {
                    case 0: {
                                for (int i = 0; i < LENGTH; ++i) {
                                    mX.push_back(Y[i]);
                                }
                    } break;
                    case 1: {
                                for (int i = 0; i < LENGTH; ++i) {
                                    mX.insert(X.begin(), DEFAULT_VALUE);
                                }
                    } break;
                    case 2: {
                                mX.insert(X.begin(), LENGTH, DEFAULT_VALUE);
                    } break;
                    case 3: {
                                mX.insert(X.begin(), Y.begin(), Y.end());
                    } break;
                    default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
            }
            LOOP2_ASSERT(limit, exceptionCaught,
                         (limit < LENGTH) == exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    const int PADDING = 16;

    std::size_t expMaxSize = -1;
    const std::size_t& EXP_MAX_SIZE = expMaxSize;
    {
        const Obj X;
        expMaxSize = X.max_size();
    }
    LOOP_ASSERT(EXP_MAX_SIZE, (size_t)-1 > EXP_MAX_SIZE);

    if (EXP_MAX_SIZE >= (size_t)-1) {
        printf("\n\nERROR: Cannot continue this test case without attempting\n"
               "to allocate huge amounts of memory.  *** Aborting. ***\n\n");
        return;                                                       // RETURN
    }

    const std::size_t DATA[] = {
        EXP_MAX_SIZE + 1,
        EXP_MAX_SIZE + 2,
        EXP_MAX_SIZE / 2 + (size_t)-1 / 2,
        (size_t)-2,
        (size_t)-1,
        0  // must be the last value
    };

    if (verbose) printf("\nConstructor 'vector(n, T x, a = A())'"
                        " and 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int i = 0; DATA[i]; ++i)
    {
        bool exceptionCaught = false;

        if (veryVerbose) printf("\tWith 'n' = " ZU "\n", DATA[i]);

        try {
            Obj mX(DATA[i], DEFAULT_VALUE);  // test here
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'reserve/resize' and"
                        " 'max_size()' equal to " ZU ".\n", EXP_MAX_SIZE);

    for (int capacityMethod = 0; capacityMethod <= 2; ++capacityMethod)
    {
        if (verbose) {
            switch (capacityMethod) {
                case 0: printf("\tWith reserve(n).\n");      break;
                case 1: printf("\tWith resize(n).\n");       break;
                case 2: printf("\tWith resize(n, T x).\n");  break;
                default: ASSERT(0);
            };
        }

        for (int i = 0; DATA[i]; ++i)
        {
            bool exceptionCaught = false;

            if (veryVerbose) printf("\t\tWith 'n' = " ZU "\n", DATA[i]);

            try {
                Obj mX;

                switch (capacityMethod) {
                    case 0:  mX.reserve(DATA[i]);                break;
                    case 1:  mX.resize(DATA[i]);                 break;
                    case 2:  mX.resize(DATA[i], DEFAULT_VALUE);  break;
                    default: ASSERT(0);
                };
            }
            catch (std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (...) {
                if (veryVerbose) {
                    printf("\t\t\tCaught unknown exception.\n");
                }
                ASSERT(0);
            }
            ASSERT(exceptionCaught);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nWith 'insert' and 'max_size()' equal to " ZU ".\n",
                        EXP_MAX_SIZE);

    for (int i = 0; DATA[i]; ++i)  {
        bool exceptionCaught = false;

        if (veryVerbose) printf("\tCreating vector of length " ZU ".\n",
                                DATA[i]);

        try {
            Obj mX(PADDING, DEFAULT_VALUE, a);  const Obj& X = mX;

            mX.insert(X.begin(), DATA[i] - PADDING, DEFAULT_VALUE);
        }
        catch (std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n",
                       e.what());
            }
            exceptionCaught = true;
        }
        catch (...) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());
#endif
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase20()
{
    // --------------------------------------------------------------------
    // TESTING COMPARISON FREE OPERATORS
    //
    // Concerns:
    //   1) 'operator<' returns the lexicographic comparison on two arrays.
    //   2) 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //      'operator<'.
    //   3) That traits get selected properly.
    //
    // Plan:
    //   For a variety of vectors of different sizes and different values, test
    //   that the comparison returns as expected.  Note that capacity is not of
    //   concern here, the implementation specifically uses only 'begin()',
    //   'end()', and 'size()'.
    //
    // Testing:
    //   bool operator<(const vector<T,A>&, const vector<T,A>&);
    //   bool operator>(const vector<T,A>&, const vector<T,A>&);
    //   bool operator<=(const vector<T,A>&, const vector<T,A>&);
    //   bool operator>=(const vector<T,A>&, const vector<T,A>&);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "",
        "A",
        "AA",
        "AAA",
        "AAAA",
        "AAAAA",
        "AAAAAA",
        "AAAAAAA",
        "AAAAAAAA",
        "AAAAAAAAA",
        "AAAAAAAAAA",
        "AAAAAAAAAAA",
        "AAAAAAAAAAAA",
        "AAAAAAAAAAAAA",
        "AAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAA",
        "AAAAAB",
        "AAAAABA",
        "AAAAABAA",
        "AAAAABAAA",
        "AAAAABAAAA",
        "AAAAABAAAAA",
        "AAAAABAAAAAA",
        "AAAAABAAAAAAA",
        "AAAAABAAAAAAAA",
        "AAAAABAAAAAAAAA",
        "AAAAB",
        "AAAABAAAAAA",
        "AAAABAAAAAAA",
        "AAAABAAAAAAAA",
        "AAAABAAAAAAAAA",
        "AAAABAAAAAAAAAA",
        "AAAB",
        "AAABA",
        "AAABAA",
        "AAABAAAAAA",
        "AAB",
        "AABA",
        "AABAA",
        "AABAAA",
        "AABAAAAAA",
        "AB",
        "ABA",
        "ABAA",
        "ABAAA",
        "ABAAAAAA",
        "B",
        "BA",
        "BAA",
        "BAAA",
        "BAAAA",
        "BAAAAA",
        "BAAAAAA",
        "BB",
        0  // null string required as last element
    };

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, v) in S X S \n.");
    {
        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            const char *const U_SPEC = SPECS[si];

            Obj mU(g(U_SPEC));  const Obj& U = mU;

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; SPECS[sj]; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                Obj mV(g(V_SPEC));  const Obj& V = mV;

                if (veryVerbose) {
                    T_; T_; P_(V_SPEC); P(V);
                }

                const bool isLess = si < sj;
                const bool isLessEq = si <= sj;
                LOOP2_ASSERT(si, sj,  isLess   == (U < V));
                LOOP2_ASSERT(si, sj, !isLessEq == (U > V));
                LOOP2_ASSERT(si, sj,  isLessEq == (U <= V));
                LOOP2_ASSERT(si, sj, !isLess   == (U >= V));
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase19()
{
    // --------------------------------------------------------------------
    // TESTING SWAP
    //
    // Concerns:
    //   1) Swapping containers does not swap allocators.
    //   2) Swapping containers with same allocator results in no allocation
    //      or deallocation operations.
    //   3) Swapping containers with different allocators does result in
    //      allocation and deallocation operations.
    //
    // Plan:
    //   Construct 'vec1' and 'vec2' with different test allocators.
    //   Add data to 'vec1'.  Remember allocation statistics.
    //   Swap 'vec1' and 'vec2'.
    //   Verify that contents were swapped.
    //   Verify that allocators for each are unchanged.
    //   Verify that allocation statistics changed for each test allocator.
    //   Create a 'vec3' with same allocator as 'vec2'.
    //   Swap 'vec2' and 'vec3'
    //   Verify that contents were swapped.
    //   Verify that allocation statistics did not change.
    //   Let 'vec3' got out of scope.
    //   Verify that memory was returned to allocator.
    //
    // Testing:
    //   swap(vector<T,A>& lhs, vector<T,A>& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP TEST"
                        "\n=========\n");

    bslma::TestAllocator testAlloc2(veryVeryVerbose);
    ASSERT(0 == testAlloc2.numBytesInUse());

    Vector_Imp<int> vec1;
    Vector_Imp<int> vec2(&testAlloc2);

    for (int i = 0; i < 1000; ++i) {
        vec1.push_back(i);
    }

    // Swap vectors with unequal allocators
    vec1.swap(vec2);
    ASSERT(0 == vec1.size());
    ASSERT(1000 == vec2.size());
    for (int i = 0; i < 1000; ++i) ASSERT(i == vec2[i]);
    ASSERT(bslma::Default::defaultAllocator() == vec1.get_allocator());
    ASSERT(&testAlloc2 == vec2.get_allocator());
    ASSERT(0 != testAlloc2.numBytesInUse());

    const bsls::Types::Int64 numAlloc2 = testAlloc2.numAllocations();
    const bsls::Types::Int64 numDealloc2 = testAlloc2.numDeallocations();
    const bsls::Types::Int64 inUse2 = testAlloc2.numBytesInUse();

    {
        Vector_Imp<int> vec3(&testAlloc2);
        ASSERT(testAlloc2.numBytesInUse() == inUse2);

        // Swap vectors with equal allocators
        vec3.swap(vec2);
        ASSERT(vec2.empty());
        ASSERT(1000 == vec3.size());
        for (int i = 0; i < 1000; ++i) ASSERT(i == vec3[i]);
        ASSERT(numAlloc2 == testAlloc2.numAllocations());
        ASSERT(numDealloc2 == testAlloc2.numDeallocations());
        ASSERT(inUse2 == testAlloc2.numBytesInUse());
    }
    // Destructor for vec3 should have freed memory
    ASSERT(0 == testAlloc2.numBytesInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase18()
{
    // --------------------------------------------------------------------
    // TESTING ERASE
    // We have the following concerns:
    //   1.)That the resulting value is correct.
    //   2) That erasing a suffix of the array never allocates, and thus never
    //      throws.  In particular, 'pop_back()' and 'erase(..., X.end())' do
    //      not throw.
    //   3) That erasing is exception neutral w.r.t. memory allocation.
    //   4) That no memory is leaked.
    //   5) That avoidable undefined behavior is trapped in appropriate build
    //      modes.  Avoidable undefined behavior might be:
    //      i) calling pop_back() on an empty container
    //      ii) calling 'erase(const_iterator)' with an invalid iterator, a
    //          non-dereferenceable iterator, or an iterator into a different
    //          container.
    //      iii) Calling 'erase(const_iterator, const_iterator)' with an
    //           invalid range, or a non-empty range that is not entirely
    //           contained within this vector, or an empty range where the
    //           iterator demarcating the range does is not 'this->end()' and
    //           does not otherwise point into this vector.
    //   6) The erase function invalidates (in appropriate build modes) all
    //      iterators into this container that refer to the erased elements, to
    //      any succeeding elements, to copies of the 'end' iterator, and no
    //      other iterators into this container.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a vector
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but not computing the number of allocations or checking on the
    //        value in case an exception is thrown (it is enough to verify that
    //        all the elements have been destroyed indirectly by making sure
    //        that there are no memory leaks).
    //   For concern 2, we verify that the number of allocations is as
    //   expected:
    //      - length of the tail (last element erased to last element) if the
    //        type uses a 'bslma' allocator, and is not moveable.
    //      - 0 otherwise.
    //
    // Testing:
    //   void pop_back();
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);
    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        TYPE_MOVEABLE  = bslmf::IsBitwiseMoveable<TYPE>::value,
        TYPE_ALLOC  =  bslma::UsesBslmaAllocator<TYPE>::value
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting 'pop_back' on non-empty vectors.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH       = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                const Obj& X = mX;

                mX.reserve(INIT_CAP);

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] =  VALUES[k % NUM_VALUES];
                }

                const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBefore: "); P_(BB); P(B);
                }

                mX.pop_back();

                const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAfter : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());
                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             INIT_CAP == X.capacity());

                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, k,
                                 VALUES[k % NUM_VALUES] == X[k]);
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, BB == AA);
                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             B - TYPE_ALLOC ==  A);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    LINE   = DATA[i].d_lineNum;
            const size_t LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t CAP = DATA[l].d_length;
                ASSERT(LENGTH <= CAP);

                Obj mX(LENGTH, DEFAULT_VALUE, &testAllocator);
                mX.reserve(CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(LENGTH); P(CAP);
                }

                bool exceptionCaught = false;
                try {
                    mX.pop_back();
                }
                catch (...) {
                    exceptionCaught = true;
                }
                LOOP_ASSERT(LINE, !exceptionCaught);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
#endif

    if (verbose) printf("\nTesting 'erase(pos)' on non-empty vectors.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                    const Obj& X = mX;

                    mX.reserve(INIT_CAP);

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase one element at "); P(POS);
                    }

                    const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore: "); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + POS);

                    const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tAfter : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 INIT_CAP == X.capacity());

                    for (m = 0; m < POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[(m + 1) % NUM_VALUES] == X[m]);
                    }

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                                   TYPE_ALLOC && !TYPE_MOVEABLE
                                                   ? LENGTH - POS
                                                   : 0;
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 BB + TYPE_ALLOCS == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 B - TYPE_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH - 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j < INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        testAllocator.setAllocationLimit(AL);

                        mX.erase(X.begin() + POS);  // test erase here

                        for (m = 0; m < POS; ++m) {
                            LOOP5_ASSERT(
                                      INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                      VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(
                                      INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                      VALUES[(m + 1) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\nTesting 'erase(first, last)'.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j <  INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const int NUM_ELEMENTS    = static_cast<int>(
                                                          END_POS - BEGIN_POS);
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                    const Obj& X = mX;

                    mX.reserve(INIT_CAP);

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);

                    const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, INIT_CAP == X.capacity());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                          TYPE_ALLOC && !TYPE_MOVEABLE &&
                                                                   NUM_ELEMENTS
                                          ? INIT_LENGTH - END_POS
                                          : 0;
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 BB + TYPE_ALLOCS == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, END_POS,
                                 B - NUM_ELEMENTS * TYPE_ALLOC ==  A);
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 1; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(INIT_LENGTH); P(INIT_CAP);
                }

                for (size_t j = 0; j <  INIT_LENGTH; ++j) {
                for (size_t k = j; k <= INIT_LENGTH; ++k) {
                    const size_t BEGIN_POS    = j;
                    const size_t END_POS      = k;
                    const size_t NUM_ELEMENTS = END_POS - BEGIN_POS;
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t m = 0;
                        for (m = 0; m < INIT_LENGTH; ++m) {
                            mX[m] =  VALUES[m % NUM_VALUES];
                        }

                        testAllocator.setAllocationLimit(AL);

                        mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);
                                                             // test erase here

                        for (m = 0; m < BEGIN_POS; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                                               VALUES[m % NUM_VALUES] == X[m]);
                        }
                        for (; m < LENGTH; ++m) {
                            LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                         END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase18Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ERASE
    //
    // Concerns:
    //   1 'pop_back' asserts on undefined behavior when the vector is empty,
    //   2 'erase' asserts on undefined behavior when iterators are not valid
    //   on the string being tested or they don't make a valid range.
    //
    // Plan:
    //   For concern (1), create an empty vector and call 'pop_back' which
    //   should assert.  Then 'push_back' a default-constructed element, and
    //   show a subsequent 'pop_back' no longer asserts.  Finally, call
    //   'pop_back' one more time on the now-empty container, and show that it
    //   asserts once again.
    //   For concern (2), create a non-empty vector and test 'erase' with
    //   different combinations of invalid iterators and iterator ranges.
    //
    // Testing:
    //   void pop_back();
    //   iterator erase(const_iterator p);
    //   iterator erase(const_iterator first, iterator last);
    // -----------------------------------------------------------------------

    bsls::AssertFailureHandlerGuard guard(&bsls::AssertTest::failTestDriver);

    if (veryVerbose) printf("\tnegative testing pop_back\n");

    {
        Obj mX;

        // pop_back on empty vector
        ASSERT_SAFE_FAIL(mX.pop_back());

        // set the vector 'mX' to a non-empty state, and demonstrate that it
        // does not assert when calling 'pop_back' until the vector is restored
        // to an empty state.
        TYPE value = TYPE();
        mX.push_back(value);

        ASSERT_SAFE_PASS(mX.pop_back());
        ASSERT_SAFE_FAIL(mX.pop_back());
    }

    if (veryVerbose) printf("\tnegative testing erase(iterator)\n");

    {
        Obj mX(g("ABCDE"));

        // position < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() - 1));

        // It is safe to call 'erase' on the boundaries of the range
        // [begin, end)
        ASSERT_SAFE_PASS(mX.erase(mX.begin()));
        ASSERT_SAFE_PASS(mX.erase(mX.end() - 1));

        // position >= end()
        ASSERT_SAFE_FAIL(mX.erase(mX.end()));
        ASSERT_SAFE_FAIL(mX.erase(mX.end() + 1));
    }

    if (veryVerbose) printf("\tnegative testing erase(iterator, iterator)\n");

    {
        Obj mX(g("ABCDE"));

        // first < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() - 1, mX.end()));

        // last > end()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin(), mX.end() + 1));

        // first > last
        ASSERT_SAFE_FAIL(mX.erase(mX.end(), mX.begin()));
        ASSERT_SAFE_FAIL(mX.erase(mX.begin() + 1, mX.begin()));
        ASSERT_SAFE_FAIL(mX.erase(mX.end(), mX.end() - 1));

        // first > end()
        ASSERT_SAFE_FAIL(mX.erase(mX.end() + 1, mX.end()));

        // last < begin()
        ASSERT_SAFE_FAIL(mX.erase(mX.begin(), mX.begin() - 1));

        ASSERT_SAFE_PASS(mX.erase(mX.begin(), mX.begin()));
        ASSERT_SAFE_PASS(mX.erase(mX.end(), mX.end()));
        ASSERT_SAFE_PASS(mX.erase(mX.begin()+1, mX.end()-1));
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase17()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    // We have the following concerns:
    //   1) That the resulting vector value is correct.
    //   2) That the 'insert' return (if any) value is a valid iterator, even
    //      when the vector underwent a reallocation.
    //   3) That the resulting capacity is correctly set up.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   7) The move 'push_back' and 'insert' move the value, capacity, and
    //      allocator correctly, and without performing any allocation.
    //   8) That inserting a 'const T& value' that is a reference to an element
    //      of the vector does not suffer from aliasing problems.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes and capacities
    //   containing default values, and insert a distinct 'value' at various
    //   positions, or a variable number of copies of this value.  Perform the
    //   above tests:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there a change in capacity, 0 otherwise
    //      - 1 if the type uses an allocator and the value is an alias.
    //
    //   For concerns 4 and 7, we test with a bitwise-moveable type that the
    //   only allocation for a move 'push_back' or 'insert' is the one for the
    //   vector reallocation (if capacity changes; all elements are moved), and
    //   for insertion the only reallocations should be for the new elements
    //   plus one if the vector undergoes a reallocation (capacity changes).
    //
    //   For concern 8, we insert an element of some vector where all the
    //   values are distinct into the same vector, taking care of the cases
    //   where the reference is before or after the position of insertion, and
    //   that the vector undergoes a reallocation or not (i.e., capacity
    //   changes or not).  We verify that the value is as expected, i.e.,
    //   identical to t it would be if the value had not been aliased.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& value);
    //   void insert(const_iterator pos, size_type n, const T& value);
    //   void push_back(T&& value);
    //   void insert(const_iterator pos, size_type n, T&& value);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        TYPE_MOVEABLE  = bslmf::IsBitwiseMoveable<TYPE>::value,
        TYPE_ALLOC  =  bslma::UsesBslmaAllocator<TYPE>::value
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting 'insert'.\n");

    if (verbose) printf("\tUsing a single 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                    const Obj& X = mX;
                    mX.reserve(INIT_CAP);

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                     ? 0
                                     : LENGTH <= INIT_CAP
                                     ? -1
                                     : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    iterator result = mX.insert(X.begin() + POS, VALUE);

                    const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
                    if (CAP != (size_t) -1) {
                        LOOP3_ASSERT(INIT_LINE, i, j, CAP == X.capacity());
                    }
                    LOOP3_ASSERT(INIT_LINE, i, j, X.begin() + POS == result);

                    for (k = 0; k < POS; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }
                    LOOP3_ASSERT(INIT_LINE, LENGTH, POS, VALUE == X[POS]);
                    for (++k; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[(k - 1) % NUM_VALUES] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                  TYPE_ALLOC && !TYPE_MOVEABLE
                                  ? (REALLOC ? INIT_LENGTH : INIT_LENGTH - POS)
                                  : 0;
                    const bsls::Types::Int64 EXP_ALLOCS =
                                            REALLOC + TYPE_ALLOCS + TYPE_ALLOC;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, TYPE_ALLOCS,
                                 BB + EXP_ALLOCS == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                 B + (0 == INIT_CAP) + TYPE_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const int    NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                         ? 0
                                         : LENGTH <= INIT_CAP
                                         ? -1
                                         : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(VALUE);
                        }

                        const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);

                        const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        if (static_cast<size_t>(-1) != CAP) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         CAP == X.capacity());
                        }

                        size_t m = 0;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (; k < POS + NUM_ELEMENTS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUE == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }

                        const int REALLOC = X.capacity() > INIT_CAP;

                        // Have to separate const's from vars in boolean
                        // operations or we get these really stupid warnings
                        // from g++.

                        const bsls::Types::Int64 TYPE_ALLOCS =
                                              !TYPE_ALLOC || TYPE_MOVEABLE
                                              ? 0
                                              : 0 == NUM_ELEMENTS
                                                ? 0
                                                : REALLOC ? INIT_LENGTH
                                                          : INIT_LENGTH - POS;

                        const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                     BB + EXP_ALLOCS == AA);
                        LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                          B + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC ==  A);
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE         = DATA[ti].d_lineNum;
                    const size_t NUM_ELEMENTS = DATA[ti].d_length;
                    const TYPE   VALUE        = VALUES[ti % NUM_VALUES];
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                            mX.reserve(INIT_CAP);
                            const Obj& X = mX;

                            const size_t CAP = (0 == INIT_CAP && 0 == LENGTH)
                                             ? 0
                                             : LENGTH <= INIT_CAP
                                             ? -1
                                             : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            testAllocator.setAllocationLimit(AL);

                            mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);
                                                         // test insertion here

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());
                            if ((size_t) -1 != CAP) {
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             CAP == X.capacity());
                            }

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             VALUE == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting aliasing concerns.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using distinct (cyclic) values.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    for (size_t h = 0; h < INIT_LENGTH; ++h) {
                        const size_t INDEX = h;

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                        mX.reserve(INIT_CAP);
                        const Obj& X = mX;

                        for (size_t k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] = VALUES[k % NUM_VALUES];
                        }

                        Obj mY(X); const Obj& Y = mY;  // control

                        if (veryVerbose) {
                            printf("\t\t\tInsert with "); P_(LENGTH);
                            printf(" at "); P_(POS);
                            printf(" using value at "); P_(INDEX);
                        }

                        mY.insert(Y.begin() + POS, X[INDEX]);  // control
                        mX.insert(X.begin() + POS, X[INDEX]);

                        if (veryVerbose) {
                            T_; T_; T_; T_; P(X);
                            T_; T_; T_; T_; P(Y);
                        }

                        LOOP5_ASSERT(INIT_LINE, i, INIT_CAP, POS, INDEX,
                                     X == Y);
                    }
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class ALLOC>
typename TestDriver<TYPE, ALLOC>::iterator
TestDriver<TYPE, ALLOC>::testEmplace(
                              typename TestDriver<TYPE, ALLOC>::Obj& object,
                              int&                                   numOfArgs,
                              const size_t                           position,
                              const TYPE&                            value)
{
          typename TestDriver<TYPE, ALLOC>::Obj& mX = object;
    const typename TestDriver<TYPE, ALLOC>::Obj&  X = mX;

    const size_t POS   = position;
    const TYPE   VALUE = value;

    iterator result;
    switch (numOfArgs)
    {
        case 5:
            result = mX.emplace(X.begin() + POS,
                                VALUE.value(),
                                VA2,
                                VA3,
                                VA4,
                                VA5);
            break;
        case 4:
            result = mX.emplace(X.begin() + POS, VALUE.value(), VA2, VA3, VA4);
            break;
        case 3:
            result = mX.emplace(X.begin() + POS, VALUE.value(), VA2, VA3);
            break;
        case 2:
            result = mX.emplace(X.begin() + POS, VALUE.value(), VA2);
            break;
        case 0:
            result = mX.emplace(X.begin() + POS);
            break;
        case 1:
        default:
            // If number of arguments is not in range '[0,14]', pass in the
            // default one 'char' argument.

            result = mX.emplace(X.begin() + POS,
                    VALUE.value());
            numOfArgs = 1;  // Reset the number of args.
            break;
    }
    return result;
}

template <class TYPE, class ALLOC>
void
TestDriver<TYPE, ALLOC>::testEmplaceBack(
                              typename TestDriver<TYPE, ALLOC>::Obj& object,
                              int&                                   numOfArgs,
                              const TYPE&                            value)
{
    typename TestDriver<TYPE, ALLOC>::Obj& mX = object;
    const TYPE VALUE = value;

    switch (numOfArgs)
    {
        case 5:
            mX.emplace_back(VALUE.value(), VA2, VA3, VA4, VA5);
            break;
        case 4:
            mX.emplace_back(VALUE.value(), VA2, VA3, VA4);
            break;
        case 3:
            mX.emplace_back(VALUE.value(), VA2, VA3);
            break;
        case 2:
            mX.emplace_back(VALUE.value(), VA2);
            break;
        case 0:
            mX.emplace_back();
            break;
        case 1:
        default:
            // If number of arguments is not in range '[0,14]', pass in the
            // default one 'char' argument.

            mX.emplace_back(VALUE.value());
            numOfArgs = 1;  // Reset the number of args.
            break;
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE, ALLOC>::testCase17Variadic(int numOfArgs)
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    // We have the following concerns:
    //   1) That the resulting vector value is correct.
    //   2) That the emplacement does correct in-place construction with
    //      variadic templates under C++11.
    //   3) That the 'emplace' return (if any) value is a valid iterator, even
    //      when the vector underwent a reallocation.
    //   4) That the resulting capacity is correctly set up.
    //   5) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   6) That emplacement is exception neutral w.r.t. memory allocation.
    //   7) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   8) The move 'emplace_back' and 'emplace' move the value, capacity, and
    //      allocator correctly, and without performing any allocation.
    //
    // Plan:
    //   For emplacement we will create objects of varying sizes and capacities
    //   containing default values, and emplace an element having a distinct
    //   'value' at various positions.  We will test the in-place construction
    //   by passing 0-14 arguments to the constructor (excluding allocator).
    //   Perform the above tests:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there a change in capacity, 0 otherwise
    //
    //   For concerns 5 and 8, we test with a bitwise-moveable type that the
    //   only allocation for a move 'emplace_back' or 'emplace' is the one for
    //   the vector reallocation (if capacity changes; all elements are moved),
    //   and for emplacement the only reallocations should be for the new
    //   elements plus one if the vector undergoes a reallocation (capacity
    //   changes).
    //
    // Testing:
    //   iterator emplace(const_iterator pos, Args&&... args);
    //   void emplace_back(Args&&... args);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    enum {
        TYPE_MOVEABLE  = bslmf::IsBitwiseMoveable <TYPE>::value,
        TYPE_ALLOC     = bslma::UsesBslmaAllocator<TYPE>::value
    };

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting 'emplace'.\n");

    if (verbose) printf("\tEmplace an element.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                    const Obj& X = mX;
                    mX.reserve(INIT_CAP);

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                     ? 0
                                     : LENGTH <= INIT_CAP
                                     ? -1
                                     : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                    }

                    iterator result = testEmplace(mX, numOfArgs, POS, VALUE);

                    const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
                    if (CAP != (size_t) -1) {
                        LOOP3_ASSERT(INIT_LINE, i, j, CAP == X.capacity());
                    }
                    LOOP3_ASSERT(INIT_LINE, i, j, X.begin() + POS == result);

                    for (k = 0; k < POS; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[k % NUM_VALUES] == X[k]);
                    }

                    if (numOfArgs) {
                        // There was at least one argument passed to 'emplace',
                        // the value of 'X[POS]' should have been set to
                        // expected value.

                        LOOP3_ASSERT(INIT_LINE, LENGTH, POS, VALUE == X[POS]);
                    }
                    else {
                        // No argument was passed to 'emplace', 'X[POS]' should
                        // have been set to default value.

                        LOOP3_ASSERT(INIT_LINE, LENGTH, POS,
                                     DEFAULT_VALUE == X[POS]);
                    }
                    LOOP2_ASSERT(numOfArgs, X[POS].numOfArgs(),
                                 numOfArgs == X[POS].numOfArgs());

                    for (++k; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, LENGTH, POS, k,
                                     VALUES[(k - 1) % NUM_VALUES] == X[k]);
                    }

                    const int REALLOC = X.capacity() > INIT_CAP;

                    const bsls::Types::Int64 TYPE_ALLOCS =
                                  TYPE_ALLOC && !TYPE_MOVEABLE
                                  ? (REALLOC ? INIT_LENGTH : INIT_LENGTH - POS)
                                  : 0;
                    const bsls::Types::Int64 EXP_ALLOCS =
                                            REALLOC + TYPE_ALLOCS + TYPE_ALLOC;

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, TYPE_ALLOCS,
                                 BB + EXP_ALLOCS == AA);
                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                 B + (0 == INIT_CAP) + TYPE_ALLOC ==  A);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tEmplace with exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];
            const size_t LENGTH      = INIT_LENGTH + 1;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                    const size_t POS = j;

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                            testAllocator) {
                        const bsls::Types::Int64 AL =
                            testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                        mX.reserve(INIT_CAP);
                        const Obj& X = mX;

                        const size_t CAP = (0 == INIT_CAP && 0 == LENGTH)
                            ? 0
                            : LENGTH <= INIT_CAP
                            ? -1
                            : ImpUtil::computeNewCapacity(
                                    LENGTH,
                                    INIT_CAP,
                                    X.max_size());

                        testAllocator.setAllocationLimit(AL);

                        testEmplace(mX, numOfArgs, POS, VALUE);

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
                        if ((size_t) -1 != CAP) {
                            LOOP3_ASSERT(INIT_LINE, i, j,
                                    CAP == X.capacity());
                        }

                        size_t k;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, i, j, k,
                                         DEFAULT_VALUE == X[k]);
                        }

                        if (numOfArgs) {
                            // There was at least one argument passed to
                            // 'emplace', the value of 'X[POS]' should have
                            // been set to expected value.

                            LOOP3_ASSERT(INIT_LINE, i, j, VALUE == X[POS]);
                        }
                        else {
                            // No argument was passed to 'emplace', 'X[POS]'
                            // should have been set to default value.

                            LOOP3_ASSERT(INIT_LINE, i, j,
                                         DEFAULT_VALUE == X[POS]);
                        }
                        LOOP2_ASSERT(numOfArgs, X[POS].numOfArgs(),
                                     numOfArgs == X[POS].numOfArgs());

                        for (++k; k < LENGTH; ++k) {
                            LOOP4_ASSERT(INIT_LINE, i, j, k,
                                         DEFAULT_VALUE == X[k]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\nTesting 'emplace_back'.\n");

    if (verbose) printf("\n\tEmplace an element to the back.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                    printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX(&testAllocator);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                testEmplaceBack(mX, numOfArgs, VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P_(X.capacity()); P(X);
            }

            testEmplaceBack(mX, numOfArgs, VALUES[li % NUM_VALUES]);

            const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            // Vector increases capacity only if the current length is zero or
            // a power of 2.  In addition, when the type allocates, an extra
            // allocation is used for the new element, and when the type is not
            // bitwise moveable, size() allocations are used during the move,
            // but an equal amount is destroyed thus the number of blocks in
            // use is unchanged.

            if (li == 0) {
                LOOP_ASSERT(li, BB + 1 + TYPE_ALLOC == AA);
                LOOP_ASSERT(li, B + 1 + TYPE_ALLOC == A);
            }
            else if((li & (li - 1)) == 0) {
                const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                                   TYPE_ALLOC * (1 + li * (1 - TYPE_MOVEABLE));
                LOOP_ASSERT(li, BB + 1 + TYPE_ALLOC_MOVES == AA);
                LOOP_ASSERT(li, B + 0 + TYPE_ALLOC == A);
            }
            else {
                LOOP_ASSERT(li, BB + 0 + TYPE_ALLOC == AA);
                LOOP_ASSERT(li, B + 0 + TYPE_ALLOC == A);
            }

            LOOP_ASSERT(li, li + 1 == X.size());

            if (numOfArgs) {
                for (size_t i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
                }
                LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
            }
            else {
                for (size_t i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, DEFAULT_VALUE == X[i]);
                }
                LOOP_ASSERT(li, DEFAULT_VALUE == X[li]);
            }
        }
    }

    if (verbose) printf("\tEmplace back with exceptions.\n");
    {
        // For each lengths li up to some modest limit:
        // 1) Create an object.
        // 2) 'emplace_back' { V0, V1, V2, V3, V4, V0, ... }  up to length li.
        // 3) Verify initial length and contents.
        // 4) Allow the object to leave scope.
        // 5) Make sure that the destructor cleans up.

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose) printf("\t\t\tOn an object of length " ZU ".\n", li);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

            Obj mX(&testAllocator);  const Obj& X = mX;              // 1.
            for (size_t i = 0; i < li; ++i) {                           // 2.
                ExceptionGuard<Obj> guard(&mX, X, L_);
                testEmplaceBack(mX, numOfArgs, VALUES[i % NUM_VALUES]);
                guard.release();
            }

            LOOP_ASSERT(li, li == X.size());                         // 3.
            if (numOfArgs) {
                for (size_t i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
                }
            }
            else {
                for (size_t i = 0; i < li; ++i) {
                    LOOP2_ASSERT(li, i, DEFAULT_VALUE == X[i]);
                }
            }

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                   // 4.
          LOOP_ASSERT(li, 0 == testAllocator.numBlocksInUse());      // 5.
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testCase17Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    // We have the following concerns:
    //   1) That the resulting vector value is correct.
    //   2) That the initial range is correctly imported and then moved if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3) That the resulting capacity is correctly set up if the initial
    //      'FWD_ITER' is a random-access iterator.
    //   4) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   5) That insertion is exception neutral w.r.t. memory allocation.
    //   6) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes with different
    //   'value' as argument.  Perform the above tests:
    //      - From the parameterized 'CONTAINER::const_iterator'.
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   In addition, the number of allocations should reflect proper internal
    //   memory management: the number of allocations should equal the sum of
    //      - NUM_ELEMENTS + (INIT_LENGTH - POS) if the type uses an allocator
    //        and is not bitwise-moveable,  0 otherwise
    //      - 1 if there is a change in capacity, 0 otherwise
    //      - 1 if the type uses an allocator and the value is an alias.
    //      -
    //   For concern 4, we test with a bitwise-moveable type that the only
    //   reallocations are for the new elements plus one if the vector
    //   undergoes a reallocation (capacity changes).
    //
    //   template <class InputIter>
    //    void insert(const_iterator pos, InputIter first, InputIter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_MOVEABLE  = bslmf::IsBitwiseMoveable<TYPE>::value;
    const int TYPE_ALLOC  =  bslma::UsesBslmaAllocator<TYPE>::value;

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    static const struct {
        int         d_lineNum;          // source line number
        int         d_length;           // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDE"                   }, // 5
        { L_,   "ABCDEAB"                 }, // 7
        { L_,   "ABCDEABC"                }, // 8
        { L_,   "ABCDEABCD"               }, // 9
        { L_,   "ABCDEABCDEABCDE"         }, // 15
        { L_,   "ABCDEABCDEABCDEA"        }, // 16
        { L_,   "ABCDEABCDEABCDEAB"       }  // 17
    };
    const int NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA;

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int    LINE         = U_DATA[ti].d_lineNum;
                    const char  *SPEC         = U_DATA[ti].d_spec;
                    const int    NUM_ELEMENTS = static_cast<int>(strlen(SPEC));
                    const size_t LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                        const Obj& X = mX;

                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                         ? 0
                                         : LENGTH <= INIT_CAP
                                         ? -1
                                         : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        mX.insert(X.begin() + POS, U.begin(), U.end());

                        const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());
                        if ((size_t) -1 != CAP) {
                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         CAP == X.capacity());
                        }

                        size_t m;
                        for (k = 0; k < POS; ++k) {
                            LOOP4_ASSERT(INIT_LINE, LINE, j, k,
                                         VALUES[k % NUM_VALUES] == X[k]);
                        }
                        for (m = 0; k < POS + NUM_ELEMENTS; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         U[m] == X[k]);
                        }
                        for (m = POS; k < LENGTH; ++k, ++m) {
                            LOOP5_ASSERT(INIT_LINE, LINE, j, k, m,
                                         VALUES[m % NUM_VALUES] == X[k]);
                        }

                        if (TYPE_MOVEABLE && INPUT_ITERATOR_TAG) {
                            const int REALLOC = X.capacity() > INIT_CAP
                                              ? NUM_ALLOCS[NUM_ELEMENTS]
                                              : 0;
                            const bsls::Types::Int64 TYPE_ALLOCS =
                                              NUM_ELEMENTS &&
                                                   TYPE_ALLOC && !TYPE_MOVEABLE
                                              ? (REALLOC ? INIT_LENGTH
                                                         : INIT_LENGTH - POS)
                                              : 0;
                            const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                            LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                         BB + EXP_ALLOCS <= AA);
                            LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                          B + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC <=  A);
                        } else {
                            const int REALLOC     = X.capacity() > INIT_CAP;
                            const bsls::Types::Int64 TYPE_ALLOCS =
                                              NUM_ELEMENTS &&
                                                   TYPE_ALLOC && !TYPE_MOVEABLE
                                              ? (REALLOC ? INIT_LENGTH
                                                         : INIT_LENGTH - POS)
                                              : 0;
                            const bsls::Types::Int64 EXP_ALLOCS  =
                                                     REALLOC + TYPE_ALLOCS +
                                                     NUM_ELEMENTS * TYPE_ALLOC;

                            LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                         BB + EXP_ALLOCS <= AA);
                            LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, j,
                                          B + (REALLOC && 0 == INIT_CAP) +
                                              NUM_ELEMENTS * TYPE_ALLOC <=  A);
                        }
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            for (int l = i; l < NUM_DATA; ++l) {
                const size_t INIT_CAP = DATA[l].d_length;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const size_t  NUM_ELEMENTS = strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                            mX.reserve(INIT_CAP);
                            const Obj& X = mX;

                            const size_t CAP = 0 == INIT_CAP && 0 == LENGTH
                                             ? 0
                                             : LENGTH <= INIT_CAP
                                             ? -1
                                             : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_CAP,
                                                                 X.max_size());

                            testAllocator.setAllocationLimit(AL);

                            mX.insert(X.begin() + POS, U.begin(), U.end());
                                                         // test insertion here

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());
                            if ((size_t) -1 != CAP) {
                                LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                             CAP == X.capacity());
                            }

                            size_t k;
                            for (k = 0; k < POS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                            for (; k < POS + NUM_ELEMENTS; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             U[k - POS] == X[k]);
                            }
                            for (; k < LENGTH; ++k) {
                                LOOP5_ASSERT(INIT_LINE, LINE, i, j, k,
                                             DEFAULT_VALUE == X[k]);
                            }
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
                ASSERT(0 == testAllocator.numMismatches());
                ASSERT(0 == testAllocator.numBlocksInUse());
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}


template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase17Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING INSERTION:
    //
    // Concerns:
    //   1 'insert' methods assert (in appropriate build modes) on undefined
    //     behavior when the iterator passed to specify the insertion point is
    //     not a valid iterator for this vector object.
    //
    //   2 An attempt to insert a range of elements specified by a pair of
    //     iterators should assert (in appropriate build modes) if the range is
    //     observably not valid i.e., a pair of random access iterators where
    //     'last' precedes 'first'.
    //
    // Plan:
    //   Construct a string object with some string data, and then call
    //   'insert' with a
    //   NULL C-string pointer and verify that it asserts.  Then call 'insert'
    //   with invalid iterators and verify that it asserts.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& val);
    //   iterator insert(const_iterator pos, size_type n, const T& val);
    //   template <class InputIter>
    //       void insert(const_iterator pos, InputIter first, InputIter last);
    // -----------------------------------------------------------------------

    const typename Obj::const_iterator badIterator =
                                               typename Obj::const_iterator();
    (void) badIterator;    // Quell 'unused' warnings in unsafe builds

    bsls::AssertFailureHandlerGuard guard(&bsls::AssertTest::failTestDriver);

    if (veryVerbose) printf("\tnegative testing insert(p, c)\n");

    {
        Obj mX(g("ABCDE"));     const Obj& X = mX;
        Obj mY(X);              const Obj& Y = mY;
        (void) Y;    // Quell 'unused' warnings in unsafe builds.

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, X[0]));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, X[0]));

        // arbitrary bad iterator
        ASSERT_SAFE_FAIL(mX.insert(badIterator, X[0]));

        // iterator to another container
        ASSERT_SAFE_FAIL(mX.insert(Y.begin(), X[0]));

        // begin() <= position < end()
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, X[0]));
        ASSERT_SAFE_PASS(mX.insert(X.end(), X[0]));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, n, c)\n");

    {
        Obj mX(g("ABCDE"));     const Obj& X = mX;
        Obj mY(X);              const Obj& Y = mY;
        (void) Y;    // Quell 'unused' warnings in unsafe builds.

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, 0, X[0]));
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, 2, X[0]));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, 0, X[0]));
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, 2, X[0]));

        // arbitrary bad iterator
        ASSERT_SAFE_FAIL(mX.insert(badIterator, 0, X[0]));
        ASSERT_SAFE_FAIL(mX.insert(badIterator, 2, X[0]));

        // iterator to another container
        ASSERT_SAFE_FAIL(mX.insert(Y.begin(), 0, X[0]));
        ASSERT_SAFE_FAIL(mX.insert(Y.begin(), 2, X[0]));

        // begin() <= position <= end()
        ASSERT_SAFE_PASS(mX.insert(X.begin() + 1, 0, X[0]));
        ASSERT_SAFE_PASS(mX.insert(X.end(), 2, X[0]));
    }

    if (veryVerbose) printf("\tnegative testing insert(p, first, last)\n");

    {
        Obj mX(g("ABCDE"));     const Obj& X = mX;
        Obj mY(g("ABE"));       const Obj& Y = mY;

        // position < begin()
        ASSERT_SAFE_FAIL(mX.insert(X.begin() - 1, Y.begin(), Y.end()));

        // position > end()
        ASSERT_SAFE_FAIL(mX.insert(X.end() + 1, Y.begin(), Y.end()));

        // first > last
        ASSERT_SAFE_FAIL(mX.insert(X.begin(), Y.end(), Y.begin()));
        ASSERT_SAFE_PASS(mX.insert(X.begin(), Y.end(), Y.end()));

        // two null pointers form a valid (empty) range
        const TYPE *nullPtr = 0;
        ASSERT_SAFE_PASS(mX.insert(X.begin(), nullPtr, nullPtr));

        // begin() <= position <= end() && first <= last
        ASSERT_SAFE_PASS(mX.insert(X.begin(), Y.begin(), Y.end()));
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase16()
{
    // --------------------------------------------------------------------
    // TESTING ITERATORS
    // Concerns:
    //   1) That 'begin' and 'end' return mutable iterators for a
    //      reference to a modifiable vector, and non-mutable iterators
    //      otherwise.
    //   2) That the range '[begin(), end())' equals the value of the vector.
    //   3) Same concerns with 'rbegin' and 'rend'.
    // In addition:
    //   4) That 'iterator' is a pointer to 'TYPE'.
    //   5) That 'const_iterator' is a pointer to 'const TYPE'.
    //   6) That 'reverse_iterator' and 'const_reverse_iterator' are
    //      implemented by the (fully-tested) 'bslstl::ReverseIterator' over a
    //      pointer to 'TYPE' or 'const TYPE'.
    //
    // Plan:
    //   For 1--3, for each value given by variety of specifications of
    //   different lengths, create a test vector with this value, and access
    //   each element in sequence and in reverse sequence, both as a modifiable
    //   reference (setting it to a default value, then back to its original
    //   value, and as a non-modifiable reference.
    //
    // For 4--6, use 'bsl::is_same' to assert the identity of iterator types.
    // Note that these concerns let us get away with other concerns such as
    // testing that 'iter[i]' and 'iter + i' advance 'iter' by the correct
    // number 'i' of positions, and other concern about traits, because
    // 'bslstl::IteratorTraits' and 'bslstl::ReverseIterator' have already been
    // fully tested in the 'bslstl_iterator' component.
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
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""                },
        { L_,  "A"               },
        { L_,  "ABC"             },
        { L_,  "ABCD"            },
        { L_,  "ABCDE"           },
        { L_,  "ABCDEAB"         },
        { L_,  "ABCDEABC"        },
        { L_,  "ABCDEABCD"       }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("Testing 'iterator', 'begin', and 'end',"
                        " and 'const' variants.\n");
    {
        ASSERT(1 == (bsl::is_same<iterator, TYPE *>::value));
        ASSERT(1 == (bsl::is_same<const_iterator, const TYPE *>::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&testAllocator);  const Obj& X = mX;
            mX = g(SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            size_t i = 0;
            for (iterator iter = mX.begin(); iter != mX.end(); ++iter, ++i) {
                LOOP_ASSERT(LINE, Y[i] == *iter);
                *iter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *iter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, LENGTH == i);

            LOOP_ASSERT(LINE, Y == X);

            i = 0;
            for (const_iterator iter = X.begin(); iter != X.end();
                                                                 ++iter, ++i) {
                LOOP2_ASSERT(LINE, i, Y[i] == *iter);
            }
            LOOP_ASSERT(LINE, LENGTH == i);
        }
    }

    if (verbose) printf("Testing 'reverse_iterator', 'rbegin', and 'rend',"
                        " and 'const' variants.\n");
    {
#ifdef BSLS_PLATFORM_CMP_SUN
#else
        ASSERT(1 == (bsl::is_same<reverse_iterator,
                                   bsl::reverse_iterator<TYPE *> >::value));
        ASSERT(1 == (bsl::is_same<const_reverse_iterator,
                                bsl::reverse_iterator<const TYPE *> >::value));
#endif

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const int     LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&testAllocator);  const Obj& X = mX;
            mX = g(SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            int i = LENGTH - 1;
            for (reverse_iterator riter = mX.rbegin(); riter != mX.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
                *riter = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == *riter);
                mX[i] = Y[i];
            }
            LOOP_ASSERT(LINE, -1 == i);

            LOOP_ASSERT(LINE, Y == X);

            i = LENGTH - 1;
            for (const_reverse_iterator riter = X.rbegin(); riter != X.rend();
                                                                ++riter, --i) {
                LOOP_ASSERT(LINE, Y[i] == *riter);
            }
            LOOP_ASSERT(LINE, -1 == i);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase15()
{
    // --------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    // Concerns:
    //   1) That 'v[x]', as well as 'v.front()' and 'v.back()', allow to modify
    //      its indexed element when 'v' is an lvalue, but must not modify its
    //      indexed element when it is an rvalue.
    //   2) That 'v.at(pos)' returns 'v[x]' or throws if 'pos == v.size())'.
    //   3) That 'v.front()' is identical to 'v[0]' and 'v.back()' the same as
    //      'v[v.size() - 1]'.
    //   4) That 'data()' returns the address of the first element, whether or
    //      not it is 'const', unless the vector is empty.
    //   5) That 'data()' returns a valid address when the vector is empty.
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test vector with this value, and access each element
    //   (front, back, at each position) both as a modifiable reference
    //   (setting it to a default value, then back to its original value, and
    //   as a non-modifiable reference.  Verify that 'at' throws
    //   'std::out_of_range' when accessing the past-the-end element.  Verify
    //   that the address of the referenced element returned from both
    //   'operator[]' and 'at' is that same as that return by adding the
    //   specified index to the pointer returned by 'data()'.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   T& at(size_type n);
    //   T& front();
    //   T& back();
    //   T *data();
    //   const T& operator[](size_type position) const;
    //   const T& at(size_type n) const;
    //   const T& front() const;
    //   const T& back() const;
    //   const T *data() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""                },
        { L_,  "A"               },
        { L_,  "ABC"             },
        { L_,  "ABCD"            },
        { L_,  "ABCDE"           },
        { L_,  "ABCDEAB"         },
        { L_,  "ABCDEABC"        },
        { L_,  "ABCDEABCD"       }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tWithout exception.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&testAllocator);  const Obj& X = mX;
            mX = g(SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            if (LENGTH) {
                TYPE element;
                makeElement(BSLS_UTIL_ADDRESSOF(element), SPEC[0]);
                const TYPE& ELEM = element;
                LOOP_ASSERT(LINE, ELEM == X.front());
                mX.front() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.front());
                mX[0] = Y[0];

                LOOP_ASSERT(LINE, X[LENGTH - 1] == X.back());
                mX.back() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.back());
                mX[LENGTH - 1] = Y[LENGTH - 1];
            }

            LOOP_ASSERT(LINE, Y == X);

            TYPE *const dataMptr = mX.data();
            const TYPE *const dataCptr = X.data();
            LOOP3_ASSERT(LINE, dataMptr, dataCptr, dataMptr == dataCptr);

            for (size_t j = 0; j < LENGTH; ++j) {
                TYPE element;
                makeElement(BSLS_UTIL_ADDRESSOF(element), SPEC[j]);
                const TYPE& ELEM = element;
                LOOP_ASSERT(LINE, ELEM == X[j]);
                mX[j] = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X[j]);
                LOOP_ASSERT(LINE, BSLS_UTIL_ADDRESSOF(X[j]) == (dataCptr + j));
                LOOP_ASSERT(LINE,
                            BSLS_UTIL_ADDRESSOF(mX[j]) == (dataMptr + j));
                mX.at(j) = Y[j];
                LOOP_ASSERT(LINE, ELEM == X.at(j));
                LOOP_ASSERT(LINE,
                            BSLS_UTIL_ADDRESSOF(X.at(j)) == (dataCptr + j));
                LOOP_ASSERT(LINE,
                            BSLS_UTIL_ADDRESSOF(mX.at(j)) == (dataMptr + j));
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
      if (verbose) printf("\tWith exception.\n");
      {
          for (int ti = 0; ti < NUM_DATA; ++ti) {
              const int     LINE   = DATA[ti].d_lineNum;
              const char   *SPEC   = DATA[ti].d_spec;
              const size_t  LENGTH = strlen(SPEC);

              Obj mX(&testAllocator);  const Obj& X = mX;
              mX = g(SPEC);

              Obj mY(X);  const Obj& Y = mY;  // control

              bool outOfRangeCaught = false;
              try {
                  mX.at(LENGTH) = DEFAULT_VALUE;
              }
              catch (std::out_of_range) {
                  outOfRangeCaught = true;
              }
              LOOP_ASSERT(LINE, Y == X);
              LOOP_ASSERT(LINE, outOfRangeCaught);
          }
      }
#endif
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase15Negative()
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING ELEMENT ACCESS
    // Concerns:
    //   For a vector 'v', the following const and non-const operations assert
    //   on undefined behavior:
    //   1 v[x] - when the index 'x' is out of range
    //   2 v.front() - when 'v' is empty
    //   3 v.back() - when 'v' is empty
    //
    // Plan:
    //   To test concerns (2) and (3), create an empty vector and verify that
    //   'front'/'back' methods assert correctly.  Then insert a single
    //   element into the vector and verify that the methods don't assert any
    //   more.  Then remove the element to make the vector empty again, and
    //   verify that the methods start asserting again.
    //
    //   To test concern (1), create a vector using a variety of specifications
    //   of different lengths, then scan the range of negative and positive
    //   indices for 'operator[]' and verify that 'operator[]' asserts when the
    //   index is out of range.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   const T& operator[](size_type position) const;
    //   T& front();
    //   T& back();
    //   const T& front() const;
    //   const T& back() const;
    // --------------------------------------------------------------------

    bsls::AssertFailureHandlerGuard guard(&bsls::AssertTest::failTestDriver);

    const TYPE DEFAULT_VALUE = TYPE();

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        //line  spec                                   length
        //----  ----                                    ------
        { L_,   ""                                   }, // 0
        { L_,   "A"                                  }, // 1
        { L_,   "AB"                                 }, // 2
        { L_,   "ABC"                                }, // 3
        { L_,   "ABCD"                               }, // 4
        { L_,   "ABCDE"                              }, // 5
        { L_,   "ABCDEA"                             }, // 6
        { L_,   "ABCDEAB"                            }, // 7
        { L_,   "ABCDEABC"                           }, // 8
        { L_,   "ABCDEABCD"                          }, // 9
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (veryVerbose) printf("\toperator[]\n");

    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (veryVeryVerbose) { T_ T_ P_(LINE) P_(SPEC) P(LENGTH); }

            Obj mX(g(SPEC));
            const Obj& X = mX;

            for (int i = -int(X.size()) - 1; i < int(X.size() * 2) + 2; ++i) {
                if (veryVeryVerbose) { T_ T_ T_ P(i); }

                if (i >= 0 && i < (int) X.size()) {
                    ASSERT_SAFE_PASS(X[i]);
                    ASSERT_SAFE_PASS(mX[i]);
                }
                else {
                    ASSERT_SAFE_FAIL(X[i]);
                    ASSERT_SAFE_FAIL(mX[i]);
                }
            }
        }
    }

    if (veryVerbose) printf("\tfront/back\n");

    {
        Obj mX;
        const Obj& X = mX;
        ASSERT_SAFE_FAIL(X.front());
        ASSERT_SAFE_FAIL(mX.front());
        ASSERT_SAFE_FAIL(X.back());
        ASSERT_SAFE_FAIL(mX.back());

        mX.push_back(DEFAULT_VALUE);
        ASSERT_SAFE_PASS(X.front());
        ASSERT_SAFE_PASS(mX.front());
        ASSERT_SAFE_PASS(X.back());
        ASSERT_SAFE_PASS(mX.back());

        mX.pop_back();
        ASSERT_SAFE_FAIL(X.front());
        ASSERT_SAFE_FAIL(mX.front());
        ASSERT_SAFE_FAIL(X.back());
        ASSERT_SAFE_FAIL(mX.back());
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase14()
{
    // --------------------------------------------------------------------
    // TESTING CAPACITY
    // Concerns:
    //   1) That 'v.reserve(n)' reserves sufficient capacity in 'v' to hold
    //      'n' elements without reallocation, but does not change value.
    //      In addition, if 'v.reserve(n)' allocates, it must allocate for a
    //      capacity of exactly 'n' bytes.
    //   2) That 'v.resize(n, val)' brings the new size to 'n', adding elements
    //      equal to 'val' if 'n' is larger than the current size.
    //   3) That existing elements are moved without copy-construction if the
    //      bitwise-moveable trait is present.
    //   4) That 'reserve' and 'resize' are exception-neutral with full
    //      guarantee of rollback.
    //   5) That the accessors such as 'capacity', 'empty', return the correct
    //      value.
    //
    // Plan:
    //   For vector 'v' having various initial capacities, call
    //   'v.reserve(n)' for various values of 'n'.  Verify that sufficient
    //   capacity is allocated by filling 'v' with 'n' elements.  Perform
    //   each test in the standard 'bslma' exception-testing macro block.
    //
    // Testing:
    //   void Vector_Imp<T>::reserve(size_type n);
    //   void resize(size_type n, T val = T());
    //   void shrink_to_fit();
    //   size_type max_size() const;
    //   size_type capacity() const;
    //   bool empty() const;
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;
    ASSERT(0 == testAllocator.numBytesInUse());

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<TYPE>::value;

    static const size_t EXTEND[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
    };
    const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

    static const size_t DATA[] = {
        0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tTesting 'max_size'.\n");
    {
        // This is the maximum value.  Any larger value would be cause for
        // potential bugs.

        Obj X;
        ASSERT(~(size_t)0 / sizeof(TYPE) >= X.max_size());
    }

    if (verbose) printf("\tTesting 'reserve', 'capacity' and 'empty'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const bsls::Types::Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);

              const bsls::Types::Int64 NUM_ALLOC_BEFORE =
                                                testAllocator.numAllocations();
              const size_t CAPACITY         = X.capacity();
              {
                  ExceptionGuard<Obj> guard(&mX, X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, CAP == X.size());
                  LOOP_ASSERT(ti, CAPACITY >= NE || NE == X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const bsls::Types::Int64 NUM_ALLOC_AFTER =
                                                testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAP || NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const bsls::Types::Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, CAP + DELTA == X.size());
              LOOP_ASSERT(ti, NUM_ALLOC_AFTER + TYPE_ALLOC * (int)DELTA ==
                                               testAllocator.numAllocations());

              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP = EXTEND[ei];

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const bsls::Types::Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              testAllocator.setAllocationLimit(AL);
              const bsls::Types::Int64 NUM_ALLOC_BEFORE =
                                                testAllocator.numAllocations();
              {
                  ExceptionGuard<Obj> guard(&mX, X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, 0  == X.size());
                  LOOP_ASSERT(ti, NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const bsls::Types::Int64 NUM_ALLOC_AFTER =
                                                testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAP || NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const bsls::Types::Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, NE);
              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NUM_ALLOC_AFTER + TYPE_ALLOC * (int)NE ==
                                               testAllocator.numAllocations());

              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    if (verbose) printf("\tTesting 'resize'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const bsls::Types::Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);
              const bsls::Types::Int64 NUM_ALLOC_BEFORE =
                                                testAllocator.numAllocations();
              ExceptionGuard<Obj> guard(&mX, X, L_);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const bsls::Types::Int64 NUM_ALLOC_AFTER =
                                                testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAP || TYPE_ALLOC ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = CAP; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const bsls::Types::Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        for (int ei = 0; ei < NUM_EXTEND; ++ei) {
            const size_t CAP   = EXTEND[ei];
            const size_t DELTA = NE > CAP ? NE - CAP : 0;

            if (veryVeryVerbose)
                printf("LINE = %d, ti = %d, ei = %d\n", L_, ti, ei);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
              const bsls::Types::Int64 AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              const bsls::Types::Int64 NUM_ALLOC_BEFORE =
                                                testAllocator.numAllocations();
              ExceptionGuard<Obj> guard(&mX, X, L_);

              testAllocator.setAllocationLimit(AL);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const bsls::Types::Int64 NUM_ALLOC_AFTER =
                                                testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAP || TYPE_ALLOC ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = 0; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const bsls::Types::Int64 AL2 = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
    }

    if (verbose) printf("\tTesting 'shrink_to_fit'.\n");

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const size_t NE = DATA[ti];

        if (veryVerbose)
            printf("LINE = %d, ti = %d\n", L_, ti);

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            const bsls::Types::Int64 AL = testAllocator.allocationLimit();
            testAllocator.setAllocationLimit(-1);

            Obj mX(Z);  const Obj& X = mX;
            LOOP_ASSERT(ti, X.empty());

            stretch(&mX, NE, VALUES[ti % NUM_VALUES]);
            LOOP_ASSERT(ti, NE == X.size());
            LOOP_ASSERT(ti, NE <= X.capacity());
            const size_t DELTA = X.capacity() - NE;
            LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

            testAllocator.setAllocationLimit(AL);
            const bsls::Types::Int64 NUM_ALLOC_BEFORE =
                testAllocator.numAllocations();
            ExceptionGuard<Obj> guard(&mX, X, L_);

            mX.shrink_to_fit();  // test here

            LOOP_ASSERT(ti, NE == X.size());
            LOOP_ASSERT(ti, NE == X.capacity());
            const bsls::Types::Int64 NUM_ALLOC_AFTER =
                testAllocator.numAllocations();

            LOOP2_ASSERT(ti, DELTA, DELTA ||
                    NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

            for (size_t j = 0; j < NE; ++j) {
                LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
            }
            guard.release();

            const bsls::Types::Int64 AL2 = testAllocator.allocationLimit();
            testAllocator.setAllocationLimit(-1);

            stretch(&mX, DELTA);
            LOOP_ASSERT(ti, NE + DELTA == X.size());
            testAllocator.setAllocationLimit(AL2);
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase13()
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    // The concerns are the same as for the constructor with the same
    // signature (case 12), except that the implementation is different,
    // and in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the assignment.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   assign(size_type n, const T& value);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    const TYPE          DEFAULT_VALUE = TYPE();
    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    if (verbose) printf("\nTesting initial-length assignment.\n");
    {
        static const struct {
            int         d_lineNum;          // source line number
            int         d_length;           // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0   },
            { L_,        1   },
            { L_,        2   },
            { L_,        3   },
            { L_,        4   },
            { L_,        5   },
            { L_,        6   },
            { L_,        7   },
            { L_,        8   },
            { L_,        9   },
            { L_,       11   },
            { L_,       12   },
            { L_,       14   },
            { L_,       15   },
            { L_,       16   },
            { L_,       17   }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\tUsing 'n' copies of 'value'.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                    printf("using default value.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                const Obj& X = mX;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    const size_t CAP = LENGTH <= INIT_LENGTH
                                     ? INIT_LENGTH
                                     : ImpUtil::computeNewCapacity(
                                                                 LENGTH,
                                                                 INIT_LENGTH,
                                                                 X.max_size());
                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    mX.assign(LENGTH, VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, CAP == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, VALUE == X[j]);
                    }
                }
            }
            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }

        if (verbose) printf("\tWith exceptions.\n");
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int    INIT_LINE   = DATA[i].d_lineNum;
                const size_t INIT_LENGTH = DATA[i].d_length;

                if (veryVerbose) {
                    printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    const size_t CAP = LENGTH <= INIT_LENGTH
                                     ? INIT_LENGTH
                                     : ImpUtil::computeNewCapacity(
                                                             LENGTH,
                                                             INIT_LENGTH,
                                                             Obj().max_size());
                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;

                        ExceptionGuard<Obj> guard(&mX, Obj(), L_);

                        testAllocator.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     CAP == X.capacity());

                        for (size_t j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(INIT_LINE, ti, i, j, VALUE == X[j]);
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERT(0 == testAllocator.numMismatches());
                    ASSERT(0 == testAllocator.numBlocksInUse());
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testCase13Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    // The concerns are the same as for the constructor with the same
    // signature (case 12), except that the implementation is different,
    // and in addition the previous value must be freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Note that we relax the concerns about memory consumption, since this
    //   is implemented as 'erase + insert', and insert will be tested more
    //   completely in test case 17.
    //
    // Testing:
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    const TYPE          DEFAULT_VALUE = TYPE();
    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;  // source line number
        int         d_length;   // expected length
    } DATA[] = {
        //line  length
        //----  ------
        { L_,        0   },
        { L_,        1   },
        { L_,        2   },
        { L_,        3   },
        { L_,        4   },
        { L_,        5   },
        { L_,        6   },
        { L_,        7   },
        { L_,        8   },
        { L_,        9   },
        { L_,       11   },
        { L_,       12   },
        { L_,       14   },
        { L_,       15   },
        { L_,       16   },
        { L_,       17   }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDE"                   }, // 5
        { L_,   "ABCDEAB"                 }, // 7
        { L_,   "ABCDEABC"                }, // 8
        { L_,   "ABCDEABCD"               }, // 9
        { L_,   "ABCDEABCDEABCDE"         }, // 15
        { L_,   "ABCDEABCDEABCDEA"        }, // 16
        { L_,   "ABCDEABCDEABCDEAB"       }  // 17
    };
    const int NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA;

    if (verbose) printf("\tUsing 'CONTAINER::const_iterator'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default value.\n");
            }

            Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES], &testAllocator);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                const size_t CAP = LENGTH <= INIT_LENGTH
                                 ? INIT_LENGTH
                                 : ImpUtil::computeNewCapacity(LENGTH,
                                                               INIT_LENGTH,
                                                               X.max_size());
                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                mX.assign(U.begin(), U.end());

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, CAP == X.capacity());

                Obj mY(g(SPEC)); const Obj& Y = mY;
                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                }
            }
        }
        ASSERT(0 == testAllocator.numMismatches());
        ASSERT(0 == testAllocator.numBlocksInUse());
    }

    if (verbose) printf("\tWith exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;

            if (veryVerbose) {
                printf("\t\tWith initial value of "); P_(INIT_LENGTH);
                printf("using default value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

                const size_t CAP = LENGTH <= INIT_LENGTH
                                 ? INIT_LENGTH
                                 : ImpUtil::computeNewCapacity(
                                                             LENGTH,
                                                             INIT_LENGTH,
                                                             Obj().max_size());
                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);  const Obj& X = mX;
                    ExceptionGuard<Obj> guard(&mX, Obj(), L_);

                    testAllocator.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    guard.release();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, CAP == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP5_ASSERT(INIT_LINE, LINE, i, ti, j, Y[j] == X[j]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(testAllocator.numMismatches(),
                            0 == testAllocator.numMismatches());
                LOOP_ASSERT(testAllocator.numBlocksInUse(),
                            0 == testAllocator.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testCase13Negative(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // NEGATIVE TESTING 'assign':
    //
    // Concerns:
    //   Assigning an invalid range to a vector should be trapped and asserted
    //   in appropriate build modes.
    //
    // Plan:
    //   Attempt to 'assign' a range to a 'vector' from another 'vector' object
    //   where the specified iterators are in the reversed order.
    //
    // Testing:
    //   template <class InputIter>
    //     assign(InputIter first, InputIter last);
    // --------------------------------------------------------------------

    bsls::AssertFailureHandlerGuard guard(&bsls::AssertTest::failTestDriver);

    const CONTAINER C(g("ABCDE"));

    const Obj X(C.begin(), C.end());

    Obj mY(X);

    if (verbose) {
        printf("\tUsing an empty range made up of null pointers\n");
    }
    // null pointers form a valid range
    const TYPE *nullPtr = 0;
    ASSERT_SAFE_PASS(mY.assign(nullPtr, nullPtr));

    if (verbose) {
        printf("\tUsing an empty range made up of stack pointers\n");
    }
    const TYPE null = TYPE();
    ASSERT_SAFE_PASS(mY.assign(BSLS_UTIL_ADDRESSOF(null),
                               BSLS_UTIL_ADDRESSOF(null)));


    if (verbose) {
        printf("\tUsing a reversed range of vector iterators\n");
    }
    // first > last
    ASSERT_SAFE_FAIL(mY.assign(X.end(), X.begin()));

    if (verbose) {
        printf("\tFinally test a valid range of vector iterators\n");
    }
    ASSERT_SAFE_PASS(mY.assign(X.end(), X.end()));
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase12()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTORS:
    //   We have the following concerns:
    //    1) The initial value is correct.
    //    2) The initial capacity is correctly set up.
    //    3) The constructor is exception neutral w.r.t. memory allocation.
    //    4) The internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    5) The move constructor moves value, capacity, and allocator
    //       correctly, and without performing any allocation.
    //
    // Plan:
    //   For the constructor we will create objects of varying sizes with
    //   different 'value' as argument.  Test first with the default value
    //   for type T, and then test with different values.  Perform the above
    //   tests:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   As for concern 5, we simply move-construct each value into a new
    //   vector and check that the value, capacity, and allocator are as
    //   expected, and that no allocation was performed.
    //
    // Testing:
    //   Vector_Imp(size_type n, const A& a = A());
    //   Vector_Imp(size_type n, const T& value, const A& a = A());
    //   Vector_Imp(vector<T,A>&& original);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE           DEFAULT_VALUE = TYPE();

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<TYPE>::value;

    if (verbose) printf("\nTesting initial-length ctor "
                        "with (default) initial value.\n");
    {
        static const struct {
            int         d_lineNum;          // source line number
            int         d_length;           // expected length
        } DATA[] = {
            //line  length
            //----  ------
            { L_,        0   },
            { L_,        1   },
            { L_,        2   },
            { L_,        3   },
            { L_,        4   },
            { L_,        5   },
            { L_,        6   },
            { L_,        7   },
            { L_,        8   },
            { L_,        9   },
            { L_,       11   },
            { L_,       12   },
            { L_,       14   },
            { L_,       15   },
            { L_,       16   },
            { L_,       17   },
            { L_,       31   },
            { L_,       32   },
            { L_,       33   },
            { L_,       63   },
            { L_,       64   },
            { L_,       65   }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\tWithout passing in an allocator, "
                            "using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                Obj mX(LENGTH);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWithout passing in an allocator, "
                            "using non-default values.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE       = DATA[ti].d_lineNum;
                const size_t LENGTH     = DATA[ti].d_length;
                const TYPE   VALUE      = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWith passing in an allocator, "
                            "using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\t\tBefore:"); P_(BB); P(B); }

                Obj mX(LENGTH, DEFAULT_VALUE, &testAllocator);
                const Obj& X = mX;

                const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\tAfter :"); P_(AA); P(A);
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                }

                if (LENGTH == 0) {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti,
                                 BB + 1 + (int)LENGTH * TYPE_ALLOC == AA);
                    LOOP2_ASSERT(LINE, ti,
                                 B + 1 + (int)LENGTH * TYPE_ALLOC ==  A);
                }
            }
        }

        if (verbose) printf("\tWith passing in an allocator, "
                            "using non-default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE       = DATA[ti].d_lineNum;
                const size_t LENGTH     = DATA[ti].d_length;
                const TYPE   VALUE      = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

                Obj mX(LENGTH, VALUE, &testAllocator);
                const Obj& X = mX;

                const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH == X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }

                if (LENGTH == 0) {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    LOOP2_ASSERT(LINE, ti,
                                 BB + 1 + (int)LENGTH * TYPE_ALLOC == AA);
                    LOOP2_ASSERT(LINE, ti,
                                 B + 1 + (int)LENGTH * TYPE_ALLOC ==  A);
                }
            }
        }

#if defined(BDE_BUILD_TARGET_EXC)
        if (verbose) printf("\tWith passing an allocator and checking for "
                            "allocation exceptions using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(LENGTH, DEFAULT_VALUE, &testAllocator);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

                if (LENGTH == 0) {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                    LOOP2_ASSERT(LINE, ti, BB + 1 + TYPE_ALLOCS == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0               ==  A);
                }
                LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
            }
        }

        if (verbose)
            printf("\tWith passing an allocator and checking for "
                   "allocation exceptions using non-default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(LENGTH, VALUE, &testAllocator);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                    LOOP2_ASSERT(LINE, ti, LENGTH == X.capacity());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
                const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

                if (LENGTH == 0) {
                    LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
                else {
                    // Because of exceptions, the number of allocations will be
                    // LENGTH trials which allocate the array for the vector,
                    // plus 1 + 2 + ... + LENGTH == LENGTH * (1 + LENGTH) / 2
                    // for the vector elements at each successive trial, plus
                    // one for the final trial which succeeds.

                    const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                    LOOP2_ASSERT(LINE, ti, BB + 1 + TYPE_ALLOCS == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0               ==  A);
                }

                LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
            }
        }
#endif  // BDE_BUILD_TARGET_EXC

        if (verbose) printf("\tAllocators hooked up properly when using "
                            "default value constructors.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                (void) LINE;

                if (verbose) {
                    printf("\t\tCreating object of "); P(LENGTH);
                }

                BSLS_TRY {
                    const bsls::Types::Int64 TB =
                                           defaultAllocator_p->numBytesInUse();
                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    ASSERT(0  == objectAllocator_p->numBytesInUse());

                    Obj x(LENGTH, DEFAULT_VALUE, objectAllocator_p);

                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    if (LENGTH) {
                        ASSERT(0 != objectAllocator_p->numBytesInUse());
                    }
                }
                BSLS_CATCH(...) {
                    break;
                }
                ASSERT(0 == globalAllocator_p->numBytesInUse());
                ASSERT(0 == objectAllocator_p->numBytesInUse());
            }
        }

        if (verbose) printf("\tAllocators hooked up properly when using "
                            "non-default value constructors.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];
                (void) LINE;

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                BSLS_TRY {
                    const bsls::Types::Int64 TB =
                                           defaultAllocator_p->numBytesInUse();
                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    ASSERT(0  == objectAllocator_p->numBytesInUse());

                    Obj x(LENGTH, VALUE, objectAllocator_p);

                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    if (LENGTH) {
                        ASSERT(0 != objectAllocator_p->numBytesInUse());
                    }
                }
                BSLS_CATCH(...) {

                    break;
                }

                ASSERT(0 == globalAllocator_p->numBytesInUse());
                ASSERT(0 == objectAllocator_p->numBytesInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testCase12Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING RANGE (TEMPLATE) CONSTRUCTORS:
    //   We have the following concerns:
    //    1) That the initial value is correct.
    //    2) That the initial range is correctly imported and then moved if the
    //       initial 'FWD_ITER' is an input iterator.
    //    3) That the initial capacity is correctly set up if the initial
    //       'FWD_ITER' is a random-access iterator.
    //    4) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    5) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    6) QoI: That passing an invalid range is detected and asserted, where
    //       possible, and in appropriate build modes only.
    //
    // Plan:
    //   We will create objects of varying sizes and capacities containing
    //   default values, and insert a range containing distinct values as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - capacity
    //      - element value at each index position { 0 .. length - 1 }.
    //   Finally, if the iterator category for 'CONTAINER' supports random
    //   access iterators, then we implement negative testing for passing
    //   reverse-ordered ranges.
    //
    // Testing:
    //   template <class InputIter>
    //     Vector_Imp(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const int TYPE_MOVEABLE  = bslmf::IsBitwiseMoveable<TYPE>::value;
    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<TYPE>::value;

    const int INPUT_ITERATOR_TAG =
        bsl::is_same<std::input_iterator_tag,
                      typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    static const struct {
        int         d_lineNum;          // source line number
        const char *d_spec;             // initial
    } DATA[] = {
        { L_,  ""                },
        { L_,  "A"               },
        { L_,  "AB"              },
        { L_,  "ABC"             },
        { L_,  "ABCD"            },
        { L_,  "ABCDE"           },
        { L_,  "ABCDEAB"         },
        { L_,  "ABCDEABC"        },
        { L_,  "ABCDEABCD"       }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\tWithout passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            Obj mY(g(SPEC));  const Obj& Y = mY;
            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) { printf("\t\tCreating object "); P(SPEC); }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));     const Obj& Y = mY;

            const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
            const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

            Obj mX(U.begin(), U.end(), &testAllocator);
            const Obj& X = mX;

            const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
            const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }

            if (LENGTH == 0) {
                LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
            }
            else if (INPUT_ITERATOR_TAG) {
                const bsls::Types::Int64 TYPE_ALLOCS = TYPE_ALLOC * LENGTH;
                if (TYPE_MOVEABLE) {
                    // Elements are create once, and then moved (no
                    // allocation), so 'TYPE_ALLOCS' is exactly the number of
                    // allocations triggered by elements.

                    LOOP2_ASSERT(LINE, ti, BB + 1 + TYPE_ALLOCS +
                                                     NUM_ALLOCS[LENGTH] == AA);
                    LOOP2_ASSERT(LINE, ti, B  + 1 + TYPE_ALLOCS ==  A);
                } else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 + TYPE_ALLOCS +
                                                     NUM_ALLOCS[LENGTH] <= AA);
                    LOOP2_ASSERT(LINE, ti, B  + 1 + TYPE_ALLOCS ==  A);
                }
            } else {
                LOOP2_ASSERT(LINE, ti,
                             BB + 1 + (int)LENGTH * TYPE_ALLOC == AA);
                LOOP2_ASSERT(LINE, ti,
                             B + 1 + (int)LENGTH * TYPE_ALLOC ==  A);
            }
        }
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf("using "); P(SPEC);
            }

            CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));        const Obj& Y = mY;

            const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
            const bsls::Types::Int64  B = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                Obj mX(U.begin(), U.end(), &testAllocator);

                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
                }

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
            const bsls::Types::Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            if (LENGTH == 0) {
                LOOP2_ASSERT(LINE, ti, BB + 0 == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
            }
            else {
                const bsls::Types::Int64 TYPE_ALLOCS =
                             TYPE_ALLOC * (LENGTH + LENGTH * (1 + LENGTH) / 2);
                if (INPUT_ITERATOR_TAG) {
                    LOOP2_ASSERT(LINE, ti, BB + 1 + TYPE_ALLOCS <= AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                } else {
                    LOOP2_ASSERT(LINE, ti, BB + 1 + TYPE_ALLOCS == AA);
                    LOOP2_ASSERT(LINE, ti,  B + 0 ==  A);
                }
            }

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }
#endif  // BDE_BUILD_TARGET_EXC

    const int RANDOM_ACCESS_ITERATOR_TAG =
          bsl::is_same<std::random_access_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;

    if (RANDOM_ACCESS_ITERATOR_TAG) {
        if (verbose) { printf("\nNegative testing\n"); }

        bsls::AssertTestHandlerGuard guard;

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_spec;             // initial
        } DATA[] = {
            { L_,  "A"               },
            { L_,  "AB"              },
            { L_,  "ABC"             },
            { L_,  "ABCD"            },
            { L_,  "ABCDE"           },
            { L_,  "ABCDEAB"         },
            { L_,  "ABCDEABC"        },
            { L_,  "ABCDEABCD"       }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("\tWithout passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
//              const int     LINE   = DATA[ti].d_lineNum;

                const char   *SPEC   = DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
                typename CONTAINER::const_iterator it1 = U.begin(); ++it1;
                ASSERT_SAFE_PASS(Obj mX(U.begin(), U.begin()));
                ASSERT_SAFE_FAIL(Obj mX(it1, U.begin()));
                ASSERT_SAFE_FAIL(Obj mX(U.end(), U.begin()));
                ASSERT_SAFE_PASS(Obj mX(U.end(), U.end()));

                // two null pointers form a valid (empty) range
                const TYPE *nullPtr = 0;
                ASSERT_SAFE_PASS(Obj mX(nullPtr, nullPtr));
            }
        }


        if (verbose) printf("\tWithout passing in an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
//              const int     LINE   = DATA[ti].d_lineNum;

                const char   *SPEC   = DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(SPEC);
                }

                CONTAINER mU(g(SPEC));  const CONTAINER& U = mU;
                typename CONTAINER::const_iterator it1 = U.begin(); ++it1;
                ASSERT_SAFE_PASS(Obj mX(U.begin(), U.begin(), &testAllocator));
                ASSERT_SAFE_FAIL(Obj mX(it1, U.begin(), &testAllocator));
                ASSERT_SAFE_FAIL(Obj mX(U.end(), U.begin(), &testAllocator));
                ASSERT_SAFE_PASS(Obj mX(U.end(), U.end(), &testAllocator));

                // two null pointers form a valid (empty) range
                const TYPE *nullPtr = 0;
                ASSERT_SAFE_PASS(Obj mX(nullPtr, nullPtr, &testAllocator));
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase11()
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // Concerns:
    //   o That the 'computeNewCapacity' class method does not overflow
    //   o That creating an empty vector does not allocate
    //   o That the allocator is passed through to elements
    //   o That the vector class has the 'bslma::UsesBslmaAllocator' trait.
    //
    // Plan:
    //   We first verify that the 'Vector_Imp' class has the traits, and
    //   that allocator
    //
    // Testing:
    //   TRAITS
    //
    // TBD When a new vector object Y is created from an old vector object
    //      X, then the standard states that Y should get its allocator by
    //      copying X's allocator (23.1, Point 8).  Our vector implementation
    //      does not follow this rule for 'bslma::Allocator'-based allocators.
    //      To verify this behavior for non-'bslma::Allocator', should test
    //      copy constructor using one and verify standard is followed.
    // --------------------------------------------------------------------

    if (verbose) printf("\nALLOCATOR TEST"
                        "\n==============\n");

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    if (verbose)
        printf("\nTesting 'bslma::UsesBslmaAllocator'.\n");

    ASSERT((bsl::is_convertible<bslma::Allocator *,
                                typename Obj::allocator_type>::value));
    ASSERT((bslma::UsesBslmaAllocator<Obj>::value));

    if (verbose)
        printf("\nTesting that empty vector does not allocate.\n");
    {
        Obj mX(&testAllocator);
        ASSERT(0 == testAllocator.numBytesInUse());
    }

    if (verbose)
        printf("\nTesting passing allocator through to elements.\n");

    ASSERT(( bslma::UsesBslmaAllocator<TYPE>::value));
    {
        Obj mX(1, VALUES[0], &testAllocator);  const Obj& X = mX;
        ASSERT(&testAllocator == X[0].allocator());
        ASSERT(2 == testAllocator.numBlocksInUse());
    }
    {
        Obj mX(&testAllocator);  const Obj& X = mX;
        mX.push_back(VALUES[0]);
        ASSERT(&testAllocator == X[0].allocator());
        ASSERT(2 == testAllocator.numBlocksInUse());
    }

    ASSERT(0 == testAllocator.numBytesInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase9()
{
    // --------------------------------------------------------------------
    // TESTING ASSIGNMENT OPERATOR:
    // We have the following concerns:
    //   1.  The value represented by any object can be assigned to any
    //         other object regardless of how either value is represented
    //         internally.
    //   2.  The 'rhs' value must not be affected by the operation.
    //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
    //       after the assignment.
    //   4.  Aliasing (x = x): The assignment operator must always work --
    //         even when the lhs and rhs are identically the same object.
    //   5.  The assignment operator must be neutral with respect to memory
    //       allocation exceptions.
    //   6.  The assignment's internal functionality varies
    //       according to which bitwise copy/move trait is applied.
    //
    // Plan:
    //   Specify a set S of unique object values with substantial and
    //   varied differences, ordered by increasing length.  For each value
    //   in S, construct an object x along with a sequence of similarly
    //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
    //   aspect of white-box state by altering each xi in a unique way.
    //   Let the union of all such objects be the set T.
    //
    //   To address concerns 1, 2, and 5, construct tests u = v for all
    //   (u, v) in T X T.  Using canonical controls UU and VV, assert
    //   before the assignment that UU == u, VV == v, and v == u if and only if
    //   VV == UU.  After the assignment, assert that VV == u, VV == v,
    //   and, for grins, that v == u.  Let v go out of scope and confirm
    //   that VV == u.  All of these tests are performed within the 'bslma'
    //   exception testing apparatus.  Since the execution time is lengthy
    //   with exceptions, every permutation is not performed when
    //   exceptions are tested.  Every permutation is also tested
    //   separately without exceptions.
    //
    //   As a separate exercise, we address 4 and 5 by constructing tests
    //   y = y for all y in T.  Using a canonical control X, we will verify
    //   that X == y before and after the assignment, again within
    //   the bslma exception testing apparatus.
    //
    //   To address concern 6, all these tests are performed on user
    //   defined types:
    //          With allocator, copyable
    //          With allocator, moveable
    //          With allocator, not moveable
    //
    // Testing:
    //   vector<T,A>& operator=(const vector<T,A>& rhs);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    // --------------------------------------------------------------------

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "Without Exceptions\n");
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEA",  // Try equal-size assignment of different values.
            "DEAB",
            "CBAEDCBA",
            "EDCBAEDCB",
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9
        };
        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        {
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = static_cast<int>(strlen(U_SPEC));

                if (verbose) {
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen <= uLen);
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);  // control
                // same lengths
                LOOP_ASSERT(ui, uLen == static_cast<int>(UU.size()));

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC); // control

                    const bool Z = ui == vi; // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            Obj mU(&testAllocator);
                            stretchRemoveAll(&mU, U_N, VALUES[0]);
                            const Obj& U = mU;
                            gg(&mU, U_SPEC);
                            {
                                Obj mV(&testAllocator);
                                stretchRemoveAll(&mV, V_N, VALUES[0]);
                                const Obj& V = mV;
                                gg(&mV, V_SPEC);
                    // v--------
                    static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                    if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                        printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                        --firstFew;
                    }
                    if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                        printf("\t| ... (ommitted from now on\n");
                        --firstFew;
                    }

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                    const int NUM_CTOR = numCopyCtorCalls;
                    const int NUM_DTOR = numDestructorCalls;
                    const size_t OLD_LENGTH = U.size();

                    mU = V; // test assignment here

                    ASSERT((numCopyCtorCalls - NUM_CTOR) <= (int)V.size());
                    ASSERT((numDestructorCalls - NUM_DTOR) <= (int)OLD_LENGTH);

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    // ---------v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nAssign cross product of values "
                        "with varied representations.\n"
                        "With Exceptions\n");
    {
        static const char *SPECS[] = { // len: 0-2, 4, 9,
            "",        "A",    "BC",     "DEAB",    "EDCBAEDCB",
            0
        }; // Null string required as last element.

        static const int EXTEND[] = {
            0, 1, 3, 5
        };
        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        int iterationModulus = 1;
        int iteration = 0;
        {
            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int         uLen   = static_cast<int>(strlen(U_SPEC));

                if (verbose) {
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);  // control
                // same lengths
                LOOP_ASSERT(ui, uLen == static_cast<int>(UU.size()));

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length " ZU ":\t",
                               vLen);
                        P(V_SPEC);
                    }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                            const int V_N = EXTEND[vj];

                            if (iteration % iterationModulus == 0) {
                                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                    //--------------^
                    const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(&testAllocator);
                    stretchRemoveAll(&mU, U_N, VALUES[0]);
                    const Obj& U = mU;
                    gg(&mU, U_SPEC);
                    {
                        Obj mV(&testAllocator);
                        stretchRemoveAll(&mV, V_N, VALUES[0]);
                        const Obj& V = mV;
                        gg(&mV, V_SPEC);

                        static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                        if (veryVeryVerbose || (veryVerbose && firstFew > 0)) {
                            printf("\t| "); P_(U_N); P_(V_N); P_(U); P(V);
                            --firstFew;
                        }
                        if (!veryVeryVerbose && veryVerbose && 0 == firstFew) {
                            printf("\t| ... (ommitted from now on\n");
                            --firstFew;
                        }

                        testAllocator.setAllocationLimit(AL);
                        {
                            mU = V; // test assignment here
                        }

                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                        LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    //--------------v
                                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                            }
                            ++iteration;
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting self assignment (Aliasing).");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0 // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
        };
        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];
            const int curLen = (int) strlen(SPEC);

            if (verbose) {
                printf("\tFor an object of length %d:\t", curLen);
                P(SPEC);
            }
            LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
            oldLen = curLen;

            // control
            const Obj X = g(SPEC);
            LOOP_ASSERT(ti, curLen == (int)X.size());  // same lengths

            for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const bsls::Types::Int64 AL =
                                               testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Obj mY(&testAllocator);
                    stretchRemoveAll(&mY, N, VALUES[0]);
                    const Obj& Y = mY;
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    {
                        ExceptionGuard<Obj> guard(&mY, Y, L_);
                        mY = Y; // test assignment here
                    }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase8()
{
    // --------------------------------------------------------------------
    // TESTING GENERATOR FUNCTION, g:
    //   Since 'g' is implemented almost entirely using 'gg', we need to verify
    //   only that the arguments are properly forwarded, that 'g' does not
    //   affect the test allocator, and that 'g' returns an object by value.
    //
    // Plan:
    //   For each SPEC in a short list of specifications, compare the object
    //   returned (by value) from the generator function, 'g(SPEC)' with the
    //   value of a newly constructed OBJECT configured using 'gg(&OBJECT,
    //   SPEC)'.  Compare the results of calling the allocator's
    //   'numBlocksTotal' and 'numBytesInUse' methods before and after calling
    //   'g' in order to demonstrate that 'g' has no effect on the test
    //   allocator.  Finally, use 'sizeof' to confirm that the (temporary)
    //   returned by 'g' differs in size from that returned by 'gg'.
    //
    // Testing:
    //   Vector_Imp g(const char *spec);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    static const char *SPECS[] = {
        "", "~", "A", "B", "C", "D", "E", "A~B~C~D~E", "ABCDE", "ABC~DE",
        0  // null string required as last element
    };

    if (verbose)
        printf("\nCompare values produced by 'g' and 'gg' "
               "for various inputs.\n");

    for (int ti = 0; SPECS[ti]; ++ti) {
        const char *SPEC = SPECS[ti];
        if (veryVerbose) { P_(ti);  P(SPEC); }

        Obj mX(&testAllocator);
        gg(&mX, SPEC);  const Obj& X = mX;

        if (veryVerbose) {
            printf("\t g = "); dbg_print(g(SPEC)); printf("\n");
            printf("\tgg = "); dbg_print(X); printf("\n");
        }
        const bsls::Types::Int64 TOTAL_BLOCKS_BEFORE =
                                                testAllocator.numBlocksTotal();
        const bsls::Types::Int64 IN_USE_BYTES_BEFORE =
                                                 testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, X == g(SPEC));
        const bsls::Types::Int64 TOTAL_BLOCKS_AFTER =
                                                testAllocator.numBlocksTotal();
        const bsls::Types::Int64 IN_USE_BYTES_AFTER =
                                                 testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");
    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

        Obj x(&testAllocator);                      // runtime tests
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

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase7()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    // We have the following concerns:
    //   1) The new object's value is the same as that of the original
    //        object (relying on the equality operator) and created with
    //        the correct capacity.
    //   2) All internal representations of a given value can be used to
    //        create a new object of equivalent value.
    //   3) The value of the original object is left unaffected.
    //   4) Subsequent changes in or destruction of the source object have
    //        no effect on the copy-constructed object.
    //   5) Subsequent changes ('push_back's) on the created object have no
    //        effect on the original and change the capacity of the new
    //        object correctly.
    //   6) The object has its internal memory management system hooked up
    //        properly so that *all* internally allocated memory draws
    //        from a user-supplied allocator whenever one is specified.
    //   7) The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //   Specify a set S of object values with substantial and varied
    //   differences, ordered by increasing length, to be used in the
    //   following tests.
    //
    //   For concerns 1 - 4, for each value in S, initialize objects w and
    //   x, copy construct y from x and use 'operator==' to verify that
    //   both x and y subsequently have the same value as w.  Let x go out
    //   of scope and again verify that w == y.
    //
    //   For concern 5, for each value in S initialize objects w and x,
    //   and copy construct y from x.  Change the state of y, by using the
    //   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //   that y differs from x and w, and verify that the capacity of y
    //   changes correctly.
    //
    //   To address concern 6, we will perform tests performed for concern 1:
    //     - While passing a testAllocator as a parameter to the new object
    //       and ascertaining that the new object gets its memory from the
    //       provided testAllocator.  Also perform test for concerns 2 and 5.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concern 7, perform tests for concern 1 performed
    //   in the presence of exceptions during memory allocations using a
    //   'bslma::TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   vector<T,A>(const vector<T,A>& original);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_MOVE  = bslmf::IsBitwiseMoveable<TYPE>::value
                         ? 0 : 1;  // if moveable, moves do not count as allocs
    const int TYPE_ALLOC =  bslma::UsesBslmaAllocator<TYPE>::value;

    if (verbose)
        printf("\nTesting parameters: TYPE_ALLOC = %d, TYPE_MOVE = %d.\n",
               TYPE_ALLOC, TYPE_MOVE);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEAB",
            "ABCDEABC",
            "ABCDEABCD",
            "ABCDEABCDEABCDE",
            "ABCDEABCDEABCDEA",
            "ABCDEABCDEABCDEAB",
            0  // null string required as last element
        };

        static const int EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9
        };

        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const size_t      LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = static_cast<int>(LENGTH);

            // Create control object w.
            Obj mW; gg(&mW, SPEC);
            const Obj& W = mW;

            LOOP_ASSERT(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Stretch capacity of x object by different amounts.

            for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                const int N = EXTEND[ei];
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                Obj *pX = new Obj(&testAllocator);
                Obj& mX = *pX;

                stretchRemoveAll(&mX, N, VALUES[0]);
                const Obj& X = mX;  gg(&mX, SPEC);

                if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

                {   // Testing concern 1.

                    if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                    const Obj Y0(X);

                    if (veryVerbose) {
                        printf("\tObj : "); P_(Y0); P(Y0.capacity());
                    }

                    LOOP2_ASSERT(SPEC, N, W == Y0);
                    LOOP2_ASSERT(SPEC, N, W == X);
                    LOOP2_ASSERT(SPEC, N, Y0.get_allocator() ==
                                           bslma::Default::defaultAllocator());
                    LOOP2_ASSERT(SPEC, N, LENGTH == Y0.capacity());
                }
                {   // Testing concern 5.

                    if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N+1; ++i) {
                        const size_t oldCap = Y1.capacity();
                        const size_t remSlots = Y1.capacity() - Y1.size();

                        stretch(&Y1, 1, VALUES[i % NUM_VALUES]);

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y1.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X != Y1);

                        if (oldCap == 0) {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == 1);
                        }
                        else if (remSlots == 0) {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == 2 * oldCap);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y1.capacity() == oldCap);
                        }
                    }
                }
                {   // Testing concern 5 with test allocator.

                    if (veryVerbose)
                        printf("\t\t\tInsert into created obj, "
                                "with test allocator:\n");

                    const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(X, &testAllocator);

                    const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    if (LENGTH == 0) {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                    }
                    else {
                        const  bsls::Types::Int64 TYPE_ALLOCS =
                                                        TYPE_ALLOC * X.size();
                        LOOP2_ASSERT(SPEC, N, BB + 1 + TYPE_ALLOCS == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 1 + TYPE_ALLOCS ==  A);
                    }

                    for (int i = 1; i < N+1; ++i) {

                        const size_t oldSize  = Y11.size();
                        const size_t oldCap   = Y11.capacity();
                        const size_t remSlots = Y11.capacity() - Y11.size();

                        const bsls::Types::Int64 CC =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64  C =
                                                testAllocator.numBlocksInUse();

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        const bsls::Types::Int64 DD =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64  D =
                                                testAllocator.numBlocksInUse();

                        // Blocks allocated should increase only when trying to
                        // add more than 'capacity'.  When adding the first
                        // element, 'numBlocksInUse' will increase by 1.  In
                        // all other conditions 'numBlocksInUse' should remain
                        // the same.

                        const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                                        TYPE_ALLOC * (1 + oldSize * TYPE_MOVE);
                        if (LENGTH == 0 && i == 1) {
                            LOOP3_ASSERT(SPEC, N, i,
                                         CC + 1 + TYPE_ALLOC_MOVES == DD);
                            LOOP3_ASSERT(SPEC, N, i,
                                         C + 1 + TYPE_ALLOC == D);
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y11.capacity() == 1);
                        }
                        else if (remSlots == 0){
                            LOOP3_ASSERT(SPEC, N, i,
                                         CC + 1 + TYPE_ALLOC_MOVES == DD);
                            LOOP3_ASSERT(SPEC, N, i,
                                         C + 0 + TYPE_ALLOC ==  D);
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y11.capacity() == 2 * oldCap);
                        }
                        else {
                            LOOP3_ASSERT(SPEC, N, i,
                                         CC + 0 + TYPE_ALLOC == DD);
                            LOOP3_ASSERT(SPEC, N, i,
                                         C + 0 + TYPE_ALLOC ==  D);
                            LOOP3_ASSERT(SPEC, N, i,
                                         Y11.capacity() == oldCap);
                        }

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append : ");
                            P_(i); P(Y11);
                        }

                        LOOP3_ASSERT(SPEC, N, i, Y11.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X != Y11);
                        LOOP3_ASSERT(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                }
#if defined(BDE_BUILD_TARGET_EXC)
                {   // Exception checking.

                    const bsls::Types::Int64 BB =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  B =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Obj Y2(X, &testAllocator);
                        if (veryVerbose) {
                            printf("\t\t\tException Case  :\n");
                            printf("\t\t\t\tObj : "); P(Y2);
                        }
                        LOOP2_ASSERT(SPEC, N, W == Y2);
                        LOOP2_ASSERT(SPEC, N, W == X);
                        LOOP2_ASSERT(SPEC, N,
                                     Y2.get_allocator() == X.get_allocator());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    const bsls::Types::Int64 AA =
                                                testAllocator.numBlocksTotal();
                    const bsls::Types::Int64  A =
                                                testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }

                    if (LENGTH == 0) {
                        LOOP2_ASSERT(SPEC, N, BB + 0 == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 0 ==  A);
                    }
                    else {
                        const bsls::Types::Int64 TYPE_ALLOCS = TYPE_ALLOC *
                                          (LENGTH + LENGTH * (1 + LENGTH) / 2);
                        LOOP4_ASSERT(SPEC, N, BB, AA,
                                     BB + 1 + TYPE_ALLOCS == AA);
                        LOOP2_ASSERT(SPEC, N,  B + 0 == A);
                    }
                }
#endif  // BDE_BUILD_TARGET_EXC
                {                            // with 'original' destroyed
                    Obj Y5(X);
                    if (veryVerbose) {
                        printf("\t\t\tWith Original deleted: \n");
                        printf("\t\t\t\tBefore Delete : "); P(Y5);
                    }

                    delete pX;

                    LOOP2_ASSERT(SPEC, N, W == Y5);

                    for (int i = 1; i < N+1; ++i) {
                        stretch(&Y5, 1, VALUES[i % NUM_VALUES]);
                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append to new obj : ");
                            P_(i);P(Y5);
                        }
                        LOOP3_ASSERT(SPEC, N, i, W != Y5);
                    }
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //   1) Objects constructed with the same values are returned as equal.
    //   2) Objects constructed such that they have same (logical) value but
    //      different internal representation (due to the lack or presence
    //      of an allocator, and/or different capacities) are always returned
    //      as equal.
    //   3) Unequal objects are always returned as unequal.
    //   4) Correctly selects the 'bitwiseEqualityComparable' traits.
    //
    // Plan:
    //   For concerns 1 and 3, Specify a set A of unique allocators including
    //   no allocator.  Specify a set S of unique object values having various
    //   minor or subtle differences, ordered by non-decreasing length.
    //   Verify the correctness of 'operator==' and 'operator!=' (returning
    //   either true or false) using all elements (u, ua, v, va) of the
    //   cross product S X A X S X A.
    //
    //   For concern 2 create two objects using all elements in S one at a
    //   time.  For the second object change its internal representation by
    //   extending it by different amounts in the set E, followed by erasing
    //   its contents using 'clear'.  Then recreate the original value and
    //   verify that the second object still return equal to the first.
    //
    //   For concern 4, we instantiate this test driver on a test type having
    //   allocators or not, and possessing the bitwise-equality-comparable
    //   trait or not.
    //
    // Testing:
    //   operator==(const vector<T,A>&, const vector<T,A>&);
    //   operator!=(const vector<T,A>&, const vector<T,A>&);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    bslma::Allocator *ALLOCATOR[] = {
        &testAllocator1,
        &testAllocator2
    };

    const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    static const char *SPECS[] = {
        "",
        "A",      "B",
        "AA",     "AB",     "BB",     "BA",
        "AAA",    "BAA",    "ABA",    "AAB",
        "AAAA",   "BAAA",   "ABAA",   "AABA",   "AAAB",
        "AAAAA",  "BAAAA",  "ABAAA",  "AABAA",  "AAABA",  "AAAAB",
        "AAAAAA", "BAAAAA", "AABAAA", "AAABAA", "AAAAAB",
        "AAAAAAA",          "BAAAAAA",          "AAAAABA",
        "AAAAAAAA",         "ABAAAAAA",         "AAAAABAA",
        "AAAAAAAAA",        "AABAAAAAA",        "AAAAABAAA",
        "AAAAAAAAAA",       "AAABAAAAAA",       "AAAAABAAAA",
        "AAAAAAAAAAA",      "AAAABAAAAAA",      "AAAAABAAAAA",
        "AAAAAAAAAAAA",     "AAAABAAAAAAA",     "AAAAABAAAAAA",
        "AAAAAAAAAAAAA",    "AAAABAAAAAAAA",    "AAAAABAAAAAAA",
        "AAAAAAAAAAAAAA",   "AAAABAAAAAAAAA",   "AAAAABAAAAAAAA",
        "AAAAAAAAAAAAAAA",  "AAAABAAAAAAAAAA",  "AAAAABAAAAAAAAA",
        0  // null string required as last element
    };

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {

        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
                // same lengths
                LOOP2_ASSERT(si, ai, LENGTH == static_cast<int>(U.size()));

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n",
                                                                  LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= LENGTH);//non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) { T_; T_;
                    P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {

                        const char *const V_SPEC = SPECS[sj];
                        Obj mV(ALLOCATOR[aj]);
                        const Obj& V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            T_; T_; P_(sj); P_(V_SPEC); P(V);
                        }

                        const bool isSame = si == sj;
                        LOOP2_ASSERT(si, sj,  isSame == (U == V));
                        LOOP2_ASSERT(si, sj, !isSame == (U != V));
                    }
                }
            }
        }
    }

    if (verbose) printf("\nCompare each pair of similar values (u, ua, v, va)"
                        " in S X A X S X A after perturbing.\n");
    {
        static const std::size_t EXTEND[] = {
            0, 1, 2, 3, 4, 5, 7, 8, 9, 15
        };

        const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = static_cast<int>(strlen(U_SPEC));

                Obj mU(ALLOCATOR[ai]); const Obj& U = mU;
                gg(&mU, U_SPEC);
                // same lengths
                LOOP_ASSERT(si, LENGTH == static_cast<int>(U.size()));

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n",
                                                                  LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= LENGTH);
                    oldLen = LENGTH;
                }

                if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }

                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {
                        //Perform perturbation
                        for (int e = 0; e < NUM_EXTEND; ++e) {

                            const char *const V_SPEC = SPECS[sj];
                            Obj mV(ALLOCATOR[aj]); const Obj& V = mV;
                            gg(&mV, V_SPEC);

                            stretchRemoveAll(&mV, EXTEND[e],
                                             VALUES[e % NUM_VALUES]);
                            gg(&mV, V_SPEC);

                            if (veryVerbose) {
                                T_; T_; P_(sj); P_(V_SPEC); P(V);
                            }

                            const bool isSame = si == sj;
                            LOOP2_ASSERT(si, sj,  isSame == (U == V));
                            LOOP2_ASSERT(si, sj, !isSame == (U != V));
                        }
                    }
                }
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase4()
{
    // --------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    // Concerns:
    //   1) The returned value for operator[] and function at() is correct
    //      as long as pos < size().
    //   2) The at() function throws out_of_range exception if
    //      pos >= size().
    //   3) Changing the internal representation to get the same (logical)
    //      final value, should not change the result of the element
    //      accessor functions.
    //   4) The internal memory management is correctly hooked up so that
    //      changes made to the state of the object via these accessors
    //      do change the state of the object.
    //
    // Plan:
    //   For 1 and 3 do the following:
    //   Specify a set S of representative object values ordered by
    //   increasing length.  For each value w in S, initialize a newly
    //   constructed object x with w using 'gg' and verify that each basic
    //   accessor returns the expected result.  Reinitialize and repeat
    //   the same test on an existing object y after perturbing y so as to
    //   achieve an internal state representation of w that is potentially
    //   different from that of x.
    //
    //   For 2, check that function at() throws a out_of_range exception
    //   when pos >= size().
    //
    //   For 4, For each value w in S, create a object x with w using
    //   'gg'.  Create another empty object y and make it 'resize' capacity
    //   equal to the size of x.  Now using the element accessor functions
    //   recreate the value of x in y.  Verify that x == y.
    //   Note - Using untested resize(int).
    //
    // Testing:
    //   reference operator[](size_type pos);
    //   const_reference operator[](size_type pos) const;
    //   reference at(size_type pos);
    //   const_reference at(size_type pos) const;
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    bslma::Allocator *ALLOCATOR[] = {
        &testAllocator,
        &testAllocator1,
        &testAllocator2
    };

    const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const size_t MAX_LENGTH = 32;

    static const struct {
        int         d_lineNum;                   // source line number
        const char *d_spec_p;                    // specification string
        int         d_length;                    // expected length
        char        d_elements[MAX_LENGTH + 1];  // expected element values
    } DATA[] = {
        //line  spec            length  elements
        //----  --------------  ------  ------------------------
        { L_,   "",                  0, { }                     },
        { L_,   "A",                 1, { VA }                  },
        { L_,   "B",                 1, { VB }                  },
        { L_,   "AB",                2, { VA, VB }              },
        { L_,   "BC",                2, { VB, VC }              },
        { L_,   "BCA",               3, { VB, VC, VA }          },
        { L_,   "CAB",               3, { VC, VA, VB }          },
        { L_,   "CDAB",              4, { VC, VD, VA, VB }      },
        { L_,   "DABC",              4, { VD, VA, VB, VC }      },
        { L_,   "ABCDE",             5, { VA, VB, VC, VD, VE }  },
        { L_,   "EDCBA",             5, { VE, VD, VC, VB, VA }  },
        { L_,   "ABCDEA",            6, { VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "ABCDEAB",           7, { VA, VB, VC, VD, VE,
                                          VA, VB }              },
        { L_,   "BACDEABC",          8, { VB, VA, VC, VD, VE,
                                          VA, VB, VC }          },
        { L_,   "CBADEABCD",         9, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD }      },
        { L_,   "CBADEABCDAB",      11, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABC",     12, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC }              },
        { L_,   "CBADEABCDABCDE",   14, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE }      },
        { L_,   "CBADEABCDABCDEA",  15, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA }  },
        { L_,   "CBADEABCDABCDEAB", 16, { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEA", 31,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA }                  },
        { L_,   "CBADEABCDABCDEABCBADEABCDABCDEAB", 32,
                                        { VC, VB, VA, VD, VE,
                                          VA, VB, VC, VD, VA,
                                          VB, VC, VD, VE, VA,
                                          VB, VC, VB, VA, VD,
                                          VE, VA, VB, VC, VD,
                                          VA, VB, VC, VD, VE,
                                          VA, VB }              }
    };

    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting const and non-const versions of "
                        "operator[] and function at() where pos < size().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const ELEMS  = DATA[ti].d_elements;

            Obj mExp;
            const Obj& EXP = gg(&mExp, ELEMS);   // expected spec

            ASSERT(LENGTH <= MAX_LENGTH);

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                Obj mX(ALLOCATOR[ai]);

                const Obj& X = gg(&mX, SPEC);    // canonical organization

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf( "\ton objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                size_t i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX.at(i));
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X.at(i));
                }

                for (; i < MAX_LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, 0 == ELEMS[i]);
                }

                // Check for perturbation.
                static const std::size_t EXTEND[] = {
                    0, 1, 2, 3, 4, 5, 7, 8, 9, 15
                };

                const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

                Obj mY(ALLOCATOR[ai]);

                const Obj& Y = gg(&mY, SPEC);

                {                             // Perform the perturbation
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {

                        stretchRemoveAll(&mY, EXTEND[ei],
                                         VALUES[ei % NUM_VALUES]);
                        gg(&mY, SPEC);

                        if (veryVerbose) { T_; T_; T_; P(Y); }

                        size_t j;
                        for (j = 0; j < LENGTH; ++j) {
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == mY[j]);
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == Y[j]);
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == mY.at(j));
                            LOOP4_ASSERT(LINE, ai, j, ei, EXP[j] == Y.at(j));
                        }

                        for (; j < MAX_LENGTH; ++j) {
                            LOOP4_ASSERT(LINE, ai, j, ei, 0 == ELEMS[j]);
                        }
                    }
                }
            }
        }
    }

    if (verbose) printf("\nTesting non-const versions of operator[] and "
                        "function at() modify state of object correctly.\n");
    {

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE    = DATA[ti].d_lineNum;
            const char *const SPEC    = DATA[ti].d_spec_p;
            const size_t      LENGTH  = DATA[ti].d_length;
            const char *const ELEMS   = DATA[ti].d_elements;

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                Obj mX(ALLOCATOR[ai]);

                const Obj& X = gg(&mX, SPEC);

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths

                if (veryVerbose) {
                    printf("\tOn objects of length " ZU ":\n", LENGTH);
                }

                if ((int)LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = static_cast<int>(LENGTH);
                }

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                Obj mY(ALLOCATOR[ai]); const Obj& Y = mY;
                Obj mZ(ALLOCATOR[ai]); const Obj& Z = mZ;

                mY.resize(LENGTH);
                mZ.resize(LENGTH);

                // Change state of Y and Z so its same as X

                for (size_t j = 0; j < LENGTH; j++) {
                    TYPE element;
                    makeElement(BSLS_UTIL_ADDRESSOF(element), ELEMS[j]);
                    const TYPE& ELEM = element;
                    mY[j]    = ELEM;
                    mZ.at(j) = ELEM;
                }

                if (veryVerbose) {
                    printf("\t\tNew object1: "); P(Y);
                    printf("\t\tNew object2: "); P(Z);
                }

                LOOP2_ASSERT(ti, ai, Y == X);
                LOOP2_ASSERT(ti, ai, Z == X);
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
      if (verbose) printf("\tTesting for out_of_range exceptions thrown"
                          " by at() when pos >= size().\n");
      {

          for (int ti = 0; ti < NUM_DATA ; ++ti) {
              const int         LINE    = DATA[ti].d_lineNum;
              const char *const SPEC    = DATA[ti].d_spec_p;
              const size_t      LENGTH  = DATA[ti].d_length;

              for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                  int exceptions, trials;

                  const int NUM_TRIALS = 2;

                  // Check exception behavior for non-const version of 'at()'.
                  // Checking the behavior for 'pos == size()' and
                  // 'pos > size()'.

                  for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                                 ; ++trials) {
                      try {
                          Obj mX(ALLOCATOR[ai]);
                          gg(&mX, SPEC);
                          mX.at(LENGTH + trials);
                      } catch (std::out_of_range) {
                          ++exceptions;
                          if (veryVerbose) {
                              printf("In out_of_range exception.\n");
                              P_(LINE); P(trials);
                          }
                          continue;
                      }
                  }

                  ASSERT(exceptions == trials);

                  // Check exception behavior for const version of at()
                  for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                                 ; ++trials) {

                      try {
                          Obj mX(ALLOCATOR[ai]);
                          const Obj& X = gg(&mX, SPEC);
                          X.at(LENGTH + trials);
                      } catch (std::out_of_range) {
                          ++exceptions;
                          if (veryVerbose) {
                              printf("In out_of_range exception." );
                              P_(LINE); P(trials);
                          }
                          continue;
                      }
                  }

                  ASSERT(exceptions == trials);
              }
          }
      }
#endif // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase3()
{
    // --------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions, we want to verify (1) that valid
    //   generator syntax produces expected results and (2) that invalid
    //   syntax is detected and reported.
    //
    // Plan:
    //   For each of an enumerated sequence of 'spec' values, ordered by
    //   increasing 'spec' length, use the primitive generator function
    //   'gg' to set the state of a newly created object.  Verify that 'gg'
    //   returns a valid reference to the modified argument object and,
    //   using basic accessors, that the value of the object is as
    //   expected.  Repeat the test for a longer 'spec' generated by
    //   prepending a string ending in a '~' character (denoting
    //   'clear').  Note that we are testing the parser only; the
    //   primary manipulators are already assumed to work.
    //
    //   For each of an enumerated sequence of 'spec' values, ordered by
    //   increasing 'spec' length, use the primitive generator function
    //   'ggg' to set the state of a newly created object.  Verify that
    //   'ggg' returns the expected value corresponding to the location of
    //   the first invalid value of the 'spec'.  Repeat the test for a
    //   longer 'spec' generated by prepending a string ending in a '~'
    //   character (denoting 'clear').  Note that we are testing the
    //   parser only; the primary manipulators are already assumed to work.
    //
    // Testing:
    //   vector<T,A>& gg(vector<T,A> *object, const char *spec);
    //   int ggg(vector<T,A> *object, const char *spec, int vF = 1);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        const int MAX_LENGTH = 10;
        static const struct {
            int         d_lineNum;               // source line number
            const char *d_spec_p;                // specification string
            int         d_length;                // expected length
            char        d_elements[MAX_LENGTH];  // expected element values
        } DATA[] = {
            //line  spec            length  elements
            //----  --------------  ------  ------------------------
            { L_,   "",             0,      { 0 }                   },

            { L_,   "A",            1,      { VA }                  },
            { L_,   "B",            1,      { VB }                  },
            { L_,   "~",            0,      { 0 }                   },

            { L_,   "CD",           2,      { VC, VD }              },
            { L_,   "E~",           0,      { 0 }                   },
            { L_,   "~E",           1,      { VE }                  },
            { L_,   "~~",           0,      { 0 }                   },

            { L_,   "ABC",          3,      { VA, VB, VC }          },
            { L_,   "~BC",          2,      { VB, VC }              },
            { L_,   "A~C",          1,      { VC }                  },
            { L_,   "AB~",          0,      { 0 }                   },
            { L_,   "~~C",          1,      { VC }                  },
            { L_,   "~B~",          0,      { 0 }                   },
            { L_,   "A~~",          0,      { 0 }                   },
            { L_,   "~~~",          0,      { 0 }                   },

            { L_,   "ABCD",         4,      { VA, VB, VC, VD }      },
            { L_,   "~BCD",         3,      { VB, VC, VD }          },
            { L_,   "A~CD",         2,      { VC, VD }              },
            { L_,   "AB~D",         1,      { VD }                  },
            { L_,   "ABC~",         0,      { 0 }                   },

            { L_,   "ABCDE",        5,      { VA, VB, VC, VD, VE }  },
            { L_,   "~BCDE",        4,      { VB, VC, VD, VE }      },
            { L_,   "AB~DE",        2,      { VD, VE }              },
            { L_,   "ABCD~",        0,      { 0 }                   },
            { L_,   "A~C~E",        1,      { VE }                  },
            { L_,   "~B~D~",        0,      { 0 }                   },

            { L_,   "~CBA~~ABCDE",  5,      { VA, VB, VC, VD, VE }  },

            { L_,   "ABCDE~CDEC~E", 1,      { VE }                  }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const ELEMS  = DATA[ti].d_elements;
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&testAllocator);
            const Obj& X = gg(&mX, SPEC);   // original spec

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj mY(&testAllocator);
            const Obj& Y = gg(&mY, buf);    // extended spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                printf("\t\tBigSpec = \"%s\"\n", buf);
                T_; T_; T_; P(X);
                T_; T_; T_; P(Y);
            }

            LOOP_ASSERT(LINE, LENGTH == X.size());
            LOOP_ASSERT(LINE, LENGTH == Y.size());
            for (size_t i = 0; i < LENGTH; ++i) {
                TYPE element;
                makeElement(BSLS_UTIL_ADDRESSOF(element), ELEMS[i]);
                const TYPE& ELEM = element;
                LOOP2_ASSERT(LINE, i, ELEM == X[i]);
                LOOP2_ASSERT(LINE, i, ELEM == Y[i]);
            }

        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec            index
            //----  -------------   -----
            { L_,   "",             -1,     }, // control

            { L_,   "~",            -1,     }, // control
            { L_,   " ",             0,     },
            { L_,   ".",             0,     },
            { L_,   "E",             -1,    }, // control
            { L_,   "I",             0,     },
            { L_,   "Z",             0,     },

            { L_,   "AE",           -1,     }, // control
            { L_,   "aE",            0,     },
            { L_,   "Ae",            1,     },
            { L_,   ".~",            0,     },
            { L_,   "~!",            1,     },
            { L_,   "  ",            0,     },

            { L_,   "ABC",          -1,     }, // control
            { L_,   " BC",           0,     },
            { L_,   "A C",           1,     },
            { L_,   "AB ",           2,     },
            { L_,   "?#:",           0,     },
            { L_,   "   ",           0,     },

            { L_,   "ABCDE",        -1,     }, // control
            { L_,   "aBCDE",         0,     },
            { L_,   "ABcDE",         2,     },
            { L_,   "ABCDe",         4,     },
            { L_,   "AbCdE",         1,     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int          LINE  = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const int         LENGTH = static_cast<int>(strlen(SPEC));

            Obj mX(&testAllocator);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                // LOOP_ASSERT(LINE, oldLen <= LENGTH);  // non-decreasing
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            LOOP_ASSERT(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase2()
{
    // --------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - push_back             (black-box)
    //      - clear                 (white-box)
    //   operate as expected.  We have the following specific concerns:
    //    1) The default constructor
    //        1a) creates the correct initial value.
    //        1b) does *not* allocate memory.
    //        1c) has the internal memory management system hooked up
    //              properly so that *all* internally allocated memory
    //              draws from the same user-supplied allocator whenever
    //              one is specified.
    //    2) The destructor properly deallocates all allocated memory to
    //         its corresponding allocator from any attainable state.
    //    3) 'push_back'
    //        3a) produces the expected value.
    //        3b) increases capacity as needed.
    //        3c) maintains valid internal state.
    //        3d) is exception neutral with respect to memory allocation.
    //    4) 'clear'
    //        4a) produces the expected value (empty).
    //        4b) properly destroys each contained element value.
    //        4c) maintains valid internal state.
    //        4d) does not allocate memory.
    //    5) The size based parameters of the class reflect the platform.
    //
    // Plan:
    //   To address concerns 1a - 1c, create an object using the default
    //   constructor:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concerns 3a - 3c, construct a series of independent
    //   objects, ordered by increasing length.  In each test, allow the
    //   object to leave scope without further modification, so that the
    //   destructor asserts internal object invariants appropriately.
    //   After the final insert operation in each test, use the (untested)
    //   basic accessors to cross-check the value of the object
    //   and the 'bslma::TestAllocator' to confirm whether a resize has
    //   occurred.
    //
    //   To address concerns 4a-4c, construct a similar test, replacing
    //   'push_back' with 'clear'; this time, however, use the test
    //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //
    //   To address concerns 2, 3d, 4d, create a small "area" test that
    //   exercises the construction and destruction of objects of various
    //   lengths and capacities in the presence of memory allocation
    //   exceptions.  Two separate tests will be performed.
    //
    //   Let S be the sequence of integers { 0 .. N - 1 }.
    //      (1) for each i in S, use the default constructor and 'push_back'
    //          to create an object of length i, confirm its value (using
    //           basic accessors), and let it leave scope.
    //      (2) for each (i, j) in S X S, use 'push_back' to create an
    //          object of length i, use 'clear' to clear its value
    //          and confirm (with 'length'), use insert to set the object
    //          to a value of length j, verify the value, and allow the
    //          object to leave scope.
    //
    //   The first test acts as a "control" in that 'clear' is not
    //   called; if only the second test produces an error, we know that
    //   'clear' is to blame.  We will rely on 'bslma::TestAllocator'
    //   and purify to address concern 2, and on the object invariant
    //   assertions in the destructor to address concerns 3d and 4d.
    //
    //   To address concern 5, the values will be explicitly compared to
    //   the expected values.  This will be done first so as to ensure all
    //   other tests are reliable and may depend upon the class's
    //   constants.
    //
    // Testing:
    //   vector<T,A>(const A& a = A());
    //   ~vector<T,A>();
    //   void push_back(const T&);
    //   void clear();
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_MOVE = bslmf::IsBitwiseMoveable<TYPE>::value
                            ? 0 : 1;
    const int TYPE_ALLOC  =  bslma::UsesBslmaAllocator<TYPE>::value;

    if (verbose)
        printf("\tTesting parameters: TYPE_ALLOC = %d, TYPE_MOVE = %d.\n",
               TYPE_ALLOC, TYPE_MOVE);

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting default ctor (thoroughly).\n");

    if (verbose) printf("\t\tWithout passing in an allocator.\n");
    {
        const Obj X;
        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());
    }

    if (verbose) printf("\t\tPassing in an allocator.\n");
    {
        const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
        const bsls::Types::Int64 A  = testAllocator.numBlocksInUse();

        const Obj X(&testAllocator);

        const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
        const bsls::Types::Int64 B  = testAllocator.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        ASSERT(AA + 0 == BB);
        ASSERT(A + 0  == B);
    }

    if (verbose) printf("\t\tIn place using a buffer allocator.\n");
    {
        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());

        Obj x(objectAllocator_p);

        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());
    }
    ASSERT(0 == globalAllocator_p->numBytesInUse());
    ASSERT(0 == defaultAllocator_p->numBytesInUse());
    ASSERT(0 == objectAllocator_p->numBytesInUse());

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tBEFORE: "); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            if(veryVerbose){
                printf("\t\t\tAFTER: "); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                    printf("\t\tOn an object of initial length " ZU ".\n",li);

            Obj mX(&testAllocator);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            // Vector increases capacity only if the current length is zero or
            // a power of 2.  In addition, when the type allocates, an extra
            // allocation is used for the new element, and when the type is not
            // bitwise moveable, size() allocations are used during the move,
            // but an equal amount is destroyed thus the number of blocks in
            // use is unchanged.

            if (li == 0) {
                LOOP_ASSERT(li, BB + 1 + TYPE_ALLOC == AA);
                LOOP_ASSERT(li, B + 1 + TYPE_ALLOC == A);
            }
            else if((li & (li - 1)) == 0) {
                const bsls::Types::Int64 TYPE_ALLOC_MOVES =
                                            TYPE_ALLOC * (1 + li * TYPE_MOVE);
                LOOP_ASSERT(li, BB + 1 + TYPE_ALLOC_MOVES == AA);
                LOOP_ASSERT(li, B + 0 + TYPE_ALLOC == A);
            }
            else {
                LOOP_ASSERT(li, BB + 0 + TYPE_ALLOC == AA);
                LOOP_ASSERT(li, B + 0 + TYPE_ALLOC == A);
            }

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }
            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                    printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            if(veryVerbose){
                printf("\t\t\tBEFORE ");
                P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            mX.clear();

            if(veryVerbose){
                printf("\t\t\tAFTER ");
                P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT ");
                P_(X.capacity()); P(X);
            }
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                    printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX(&testAllocator);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const bsls::Types::Int64 BB = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B);
                typename Obj::size_type Cap = X.capacity();
                P_(Cap);P(X);
            }

            mX.clear();

            const bsls::Types::Int64 AA = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const bsls::Types::Int64 CC = testAllocator.numBlocksTotal();
            const bsls::Types::Int64 C  = testAllocator.numBlocksInUse();

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            LOOP_ASSERT(li, BB == AA);
            LOOP_ASSERT(li, BB + (int)li * TYPE_ALLOC == CC);

            LOOP_ASSERT(li, B - 0 - (int)li * TYPE_ALLOC == A);
            LOOP_ASSERT(li, B - 0 == C);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'push_back' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an object
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the object to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose) printf("\t\t\tOn an object of length " ZU ".\n", li);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

            Obj mX(&testAllocator);  const Obj& X = mX;              // 1.
            for (size_t i = 0; i < li; ++i) {                           // 2.
                ExceptionGuard<Obj> guard(&mX, X, L_);
                mX.push_back(VALUES[i % NUM_VALUES]);
                guard.release();
            }

            LOOP_ASSERT(li, li == X.size());                         // 3.
            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                   // 4.
          LOOP_ASSERT(li, 0 == testAllocator.numBlocksInUse());      // 5.
        }
    }

    if (verbose) printf("\t\tWith 'push_back' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1) create an object
        //    2) insert V0 values up to a length of i
        //    3) verify initial length and contents
        //    4) clear contents from object
        //    5) verify length is 0
        //    6) insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7) verify new length and contents
        //    8) allow the object to leave scope
        //    9) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
            if (verbose)
                printf("\t\t\tOn an object of initial length " ZU ".\n", i);

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVerbose)
                    printf("\t\t\t\tAnd with final length " ZU ".\n", j);

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                size_t k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
                for (k = 0; k < i; ++k) {                           // 2.
                    ExceptionGuard<Obj> guard(&mX, X, L_);
                    mX.push_back(VALUES[0]);
                    guard.release();
                }

                LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                for (k = 0; k < i; ++k) {
                    LOOP3_ASSERT(i, j, k, VALUES[0] == X[k]);
                }

                mX.clear();                                         // 4.
                LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                for (k = 0; k < j; ++k) {                           // 6.
                    ExceptionGuard<Obj> guard(&mX, X, L_);
                    mX.push_back(VALUES[k % NUM_VALUES]);
                    guard.release();
                }

                LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                for (k = 0; k < j; ++k) {
                    LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == X[k]);
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END              // 8.
              LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());  // 9.
            }

        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase1()
{
    // --------------------------------------------------------------------
    // BREATHING TEST:
    //   We want to exercise basic value-semantic functionality.  In
    //   particular we want to demonstrate a base-line level of correct
    //   operation of the following methods and operators:
    //      - default and copy constructors (and also the destructor)
    //      - the assignment operator (including aliasing)
    //      - equality operators: 'operator==' and 'operator!='
    //      - primary manipulators: 'push_back' and 'clear' methods
    //      - basic accessors: 'size' and 'operator[]'
    //   In addition we would like to exercise objects with potentially
    //   different internal organizations representing the same value.
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators, basic
    //   accessors, equality operators, and the assignment operator.
    //   Invoke the primary manipulator [1&5], copy constructor [2&8], and
    //   assignment operator [9&10] in situations where the internal data
    //   (i) does *not* and (ii) *does* have to resize.  Try aliasing with
    //   assignment for a non-empty object [11] and allow the result to
    //   leave scope, enabling the destructor to assert internal object
    //   invariants.  Display object values frequently in verbose mode:
    //
    // 1) Create an object x1 (default ctor).       { x1: }
    // 2) Create a second object x2 (copy from x1). { x1: x2: }
    // 3) Append an element value A to x1).         { x1:A x2: }
    // 4) Append the same element value A to x2).   { x1:A x2:A }
    // 5) Append another element value B to x2).    { x1:A x2:AB }
    // 6) Remove all elements from x1.              { x1: x2:AB }
    // 7) Create a third object x3 (default ctor).  { x1: x2:AB x3: }
    // 8) Create a forth object x4 (copy of x2).    { x1: x2:AB x3: x4:AB }
    // 9) Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:AB }
    // 10) Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:AB x4:AB }
    // 11) Assign x4 = x4 (aliasing).               { x1: x2: x3:AB x4:AB }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(&testAllocator);  const Obj& X1 = mX1;
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

    if(veryVerbose){
        typename Obj::size_type capacity = X1.capacity();
        T_; T_;
        P(capacity);
    }

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 2) Create a second object x2 (copy from x1)."
                         "\t\t{ x1: x2: }\n");
    Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check the initial state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 3) Append an element value A to x1)."
                        "\t\t\t{ x1:A x2: }\n");
    mX1.push_back(VALUES[0]);
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(1 == X1.size());
    ASSERT(VALUES[0] == X1[0]);

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 4) Append the same element value A to x2)."
                         "\t\t{ x1:A x2:A }\n");
    mX2.push_back(VALUES[0]);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(1 == X2.size());
    ASSERT(VALUES[0] == X2[0]);

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5) Append another element value B to x2)."
                         "\t\t{ x1:A x2:AB }\n");
    mX2.push_back(VALUES[1]);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(2 == X2.size());
    ASSERT(VALUES[0] == X2[0]);
    ASSERT(VALUES[1] == X2[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(!(X2 == X1));          ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 6) Remove all elements from x1."
                         "\t\t\t{ x1: x2:AB }\n");
    mX1.clear();
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(0 == X1.size());

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 7) Create a third object x3 (default ctor)."
                         "\t\t{ x1: x2:AB x3: }\n");

    Obj mX3(&testAllocator);  const Obj& X3 = mX3;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(0 == X3.size());

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3.\n");
    ASSERT(  X3 == X1 );          ASSERT(!(X3 != X1));
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 8) Create a forth object x4 (copy of x2)."
                         "\t\t{ x1: x2:AB x3: x4:AB }\n");

    Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");

    ASSERT(2 == X4.size());
    ASSERT(VALUES[0] == X4[0]);
    ASSERT(VALUES[1] == X4[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );          ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));          ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:AB }\n");

    mX2 = X1;
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(0 == X2.size());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2, x3, x4.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));
    ASSERT(  X2 == X3 );          ASSERT(!(X2 != X3));
    ASSERT(!(X2 == X4));          ASSERT(  X2 != X4 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n10) Assign x3 = x4 (empty becomes non-empty)."
                         "\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX3 = X4;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(2 == X3.size());
    ASSERT(VALUES[0] == X3[0]);
    ASSERT(VALUES[1] == X3[1]);

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X3 == X1));          ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );          ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n11) Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:AB x4:AB }\n");

    mX4 = X4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");
    ASSERT(2 == X4.size());
    ASSERT(VALUES[0] == X4[0]);
    ASSERT(VALUES[1] == X4[1]);

    if (verbose)
        printf("\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Creating a Matrix Type
///- - - - - - - - - - - - - - - - -
// Suppose we want to define a value-semantic type representing a dynamically
// resizable two-dimensional matrix.
//
// First, we define the public interface for the 'MyMatrix' class template:
//..
template <class TYPE>
class MyMatrix {
    // This value-semantic type characterizes a two-dimensional matrix of
    // objects of the (template parameter) 'TYPE'.  The numbers of columns and
    // rows of the matrix can be specified at construction and, at any time,
    // via the 'reset', 'insertRow', and 'insertColumn' methods.  The value of
    // each element in the matrix can be set and accessed using the 'theValue',
    // and 'theModifiableValue' methods respectively.  A free operator,
    // 'operator*', is available to return the product of two specified
    // matrices.

  public:
    // PUBLIC TYPES
//..
// Here, we create a type alias, 'RowType', for an instantiation of
// 'bsl::vector' to represent a row of 'TYPE' objects in the matrix.  We create
// another type alias, 'MatrixType', for an instantiation of 'bsl::vector' to
// represent the entire matrix of 'TYPE' objects as a list of rows:
//..
    typedef bsl::vector<TYPE>    RowType;
        // This is an alias representing a row of values of the (template
        // parameter) 'TYPE'.

    typedef bsl::vector<RowType> MatrixType;
        // This is an alias representing a two-dimensional matrix of values of
        // the (template parameter) 'TYPE'.

  private:
    // DATA
    MatrixType d_matrix;      // matrix of values
    int        d_numColumns;  // number of columns

    // FRIENDS
    template <class T>
    friend bool operator==(const MyMatrix<T>&, const MyMatrix<T>&);

  public:
    // PUBLIC TYPES
    typedef typename MatrixType::const_iterator ConstRowIterator;

    // CREATORS
    MyMatrix(int               numRows,
             int               numColumns,
             bslma::Allocator *basicAllocator = 0);
        // Create a 'MyMatrix' object having the specified 'numRows' and the
        // specified 'numColumns'.  All elements of the (template parameter)
        // 'TYPE' in the matrix will have the default-constructed value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numRows' and
        // '0 <= numColumns'

    MyMatrix(const MyMatrix&   original,
             bslma::Allocator *basicAllocator = 0);
        // Create a 'MyMatrix' object having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~MyMatrix = default;
        // Destroy this object.

    // MANIPULATORS
    MyMatrix& operator=(const MyMatrix& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void clear();
        // Remove all rows and columns from this object.

    void insertRow(int rowIndex);
        // Insert, into this matrix, an row at the specified 'rowIndex'.  All
        // elements of the (template parameter) 'TYPE' in the row will have the
        // default-constructed value.  The behavior is undefined unless
        // '0 <= rowIndex <= numRows()'.

    void insertColumn(int columnIndex);
        // Insert, into this matrix, an column at the specified 'columnIndex'.
        // All elements of the (template parameter) 'TYPE' in the column will
        // have the default constructed value.  The behavior is undefined
        // unless '0 <= columnIndex <= numColumns()'.

    TYPE& theModifiableValue(int rowIndex, int columnIndex);
        // Return a reference providing modifiable access to the element at the
        // specified 'rowIndex' and the specified 'columnIndex' in this matrix.
        // The behavior is undefined unless '0 <= rowIndex < numRows()' and
        // '0 <= columnIndex < numColumns()'.

    // ACCESSORS
    int numRows() const;
        // Return the number of rows in this matrix.

    int numColumns() const;
        // Return the number of columns in this matrix.

    ConstRowIterator beginRow() const;
        // Return an iterator providing non-modifiable access to the 'RowType'
        // objects representing the first row in this matrix.

    ConstRowIterator endRow() const;
        // Return an iterator providing non-modifiable access to the 'RowType'
        // objects representing the past-the-end row in this matrix.

    const TYPE& theValue(int rowIndex, int columnIndex) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'rowIndex' and the specified 'columnIndex' in this
        // matrix.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()' and '0 <= columnIndex < numColumns()'.
};
//..
// Then we declare the free operator for 'MyMatrix':
//..
// FREE OPERATORS
template <class TYPE>
MyMatrix<TYPE> operator==(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyMatrix' objects have the same
    // value if they have the same number of rows and columns and every element
    // in both matrices compare equal.

template <class TYPE>
MyMatrix<TYPE> operator!=(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MyMatrix' objects do not have
    // the same value if they do not have the same number of rows and columns
    // or every element in both matrices do not compare equal.
//..
// Now, we define the methods of 'MyMatrix':
//..
// CREATORS
template <class TYPE>
MyMatrix<TYPE>::MyMatrix(int numRows,
                         int numColumns,
                         bslma::Allocator *basicAllocator)
: d_matrix(numRows, basicAllocator)
, d_numColumns(numColumns)
{
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(0 <= numColumns);

    for (typename MatrixType::iterator itr = d_matrix.begin();
         itr != d_matrix.end();
         ++itr) {
        itr->resize(d_numColumns);
    }
}

template <class TYPE>
MyMatrix<TYPE>::MyMatrix(const MyMatrix& original,
                         bslma::Allocator *basicAllocator)
: d_matrix(original.d_matrix, basicAllocator)
, d_numColumns(original.d_numColumns)
{
}
//..
// Notice that we pass the contained 'bsl::vector' ('d_matrix') the allocator
// specified at construction to supply memory.  If the (template parameter)
// 'TYPE' of the elements has the 'bslalg_TypeTraitUsesBslmaAllocator' trait,
// this allocator will be passed by the vector to the elements as well.
//..
// MANIPULATORS
template <class TYPE>
MyMatrix<TYPE>& MyMatrix<TYPE>::operator=(const MyMatrix& rhs)
{
    d_matrix = rhs.d_matrix;
    d_numColumns = rhs.d_numColumns;
}

template <class TYPE>
void MyMatrix<TYPE>::clear()
{
    d_matrix.clear();
    d_numColumns = 0;
}

template <class TYPE>
void MyMatrix<TYPE>::insertRow(int rowIndex)
{
    typename MatrixType::iterator itr =
        d_matrix.insert(d_matrix.begin() + rowIndex, RowType());
    itr->resize(d_numColumns);
}

template <class TYPE>
void MyMatrix<TYPE>::insertColumn(int columnIndex) {
    for (typename MatrixType::iterator itr = d_matrix.begin();
         itr != d_matrix.end();
         ++itr) {
        itr->insert(itr->begin() + columnIndex, TYPE());
    }
    ++d_numColumns;
}

template <class TYPE>
TYPE& MyMatrix<TYPE>::theModifiableValue(int rowIndex, int columnIndex)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT((typename MatrixType::size_type) rowIndex < d_matrix.size());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(columnIndex < d_numColumns);

    return d_matrix[rowIndex][columnIndex];
}

// ACCESSORS
template <class TYPE>
int MyMatrix<TYPE>::numRows() const
{
    return d_matrix.size();
}

template <class TYPE>
int MyMatrix<TYPE>::numColumns() const
{
    return d_numColumns;
}

template <class TYPE>
typename MyMatrix<TYPE>::ConstRowIterator MyMatrix<TYPE>::beginRow() const
{
    return d_matrix.begin();
}

template <class TYPE>
typename MyMatrix<TYPE>::ConstRowIterator MyMatrix<TYPE>::endRow() const
{
    return d_matrix.end();
}

template <class TYPE>
const TYPE& MyMatrix<TYPE>::theValue(int rowIndex, int columnIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT((typename MatrixType::size_type) rowIndex < d_matrix.size());
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(columnIndex < d_numColumns);

    return d_matrix[rowIndex][columnIndex];
}
//..
// Finally, we defines the free operators for 'MyMatrix':
//..
// FREE OPERATORS
template <class TYPE>
MyMatrix<TYPE> operator==(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs)
{
    return lhs.d_numColumns == rhs.d_numColumns &&
                                                  lhs.d_matrix == rhs.d_matrix;
}

template <class TYPE>
MyMatrix<TYPE> operator!=(const MyMatrix<TYPE>& lhs,
                          const MyMatrix<TYPE>& rhs)
{
    return !(lhs == rhs);
}
//..

}  // close unnamed namespace

//=============================================================================
//                         HYMAN'S TEST TYPES
//-----------------------------------------------------------------------------
struct A     { int x; A() : x('a') { } };
struct B : A { int y; B() : y('b') { } };

template <class T, size_t N>
struct HI : public bsl::iterator<bsl::random_access_iterator_tag, T>
{
    static const size_t SIDE = size_t(1) << N;
    static const size_t SIZE = SIDE * SIDE;

    T *p;
    size_t d;

    explicit HI(T *p = 0, size_t d = SIZE) : p(p), d(d) { }
    HI(const HI& o) : p(o.p), d(o.d) { }

    size_t htoi() const
    {
        size_t x = 0, y = 0, t = d;
        for (size_t s = 1; s < SIDE; s *= 2) {
            size_t rx = 1 & (t / 2);
            size_t ry = 1 & (t ^ rx);
            if (ry == 0) {
                if (rx == 1) {
                    x = s - 1 - x;
                    y = s - 1 - y;
                }
                size_t z = x;
                x = y;
                y = z;
            }
            x += s * rx;
            y += s * ry;
            t /= 4;
        }
        return y * SIDE + x;
    }

    T &operator*()  const { return p[htoi()];  }
    T *operator->() const { return p + htoi(); }

    HI& operator++() { ++d; return *this; }
    HI& operator--() { --d; return *this; }

    HI  operator++(int) { HI t(p, d); ++d; return t; }
    HI  operator--(int) { HI t(p, d); --d; return t; }

    HI& operator+=(ptrdiff_t n) { d += n; return *this; }
    HI& operator-=(ptrdiff_t n) { d -= n; return *this; }

    HI  operator+ (ptrdiff_t n) const { return HI(p, d + n); }
    HI  operator- (ptrdiff_t n) const { return HI(p, d - n); }

    ptrdiff_t operator-(const HI& o) const { return d - o.d; }

    T &operator[](ptrdiff_t n) const { return *(*this + n); }

    operator T*()   const { return p + htoi(); }
        // Conversion operator to confuse badly written traits code.
};

template <class T, size_t N>
inline
bool operator< (const HI<T, N>& l, const HI<T, N>& r)
{
    return (l.p < r.p) || (l.p == r.p && l.d < r.d);
}

template <class T, size_t N>
inline
bool operator>=(const HI<T, N>& l, const HI<T, N>& r)
{
    return !(l <  r);
}

template <class T, size_t N>
inline
bool operator> (const HI<T, N>& l, const HI<T, N>& r)
{
    return !(l <= r);
}

template <class T, size_t N>
inline
bool operator<=(const HI<T, N>& l, const HI<T, N>& r)
{
    return !(l >  r);
}

template <class T, size_t N>
inline
bool operator==(const HI<T, N>& l, const HI<T, N>& r)
{
    return !(l < r) && !(r < l);
}

template <class T, size_t N>
inline
bool operator!=(const HI<T, N>& l, const HI<T, N>& r)
{
    return !(l == r);
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose; // Suppressing the "unused variable" warning

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);
    objectAllocator_p = &objectAllocator;

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;

    // Global Test Allocator.
    bslma::TestAllocator  globalAllocator("Global Allocator",
                                          veryVeryVeryVerbose);
    bslma::Allocator *originalGlobalAllocator =
                          bslma::Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 27: {
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

        // Do some ad-hoc breathing test for the 'MyMatrix' type defined in the
        // usage example

        {
            bslma::TestAllocator oa("oa", veryVeryVeryVerbose);

            // 1 2
            //
            // 3 4

            MyMatrix<int> m1(1, 1, &oa);
            m1.theModifiableValue(0, 0) = 4;
            m1.insertRow(0);
            m1.theModifiableValue(0, 0) = 2;
            m1.insertColumn(0);
            m1.theModifiableValue(0, 0) = 1;
            m1.theModifiableValue(1, 0) = 3;

            ASSERT(1 == m1.theValue(0, 0));
            ASSERT(2 == m1.theValue(0, 1));
            ASSERT(3 == m1.theValue(1, 0));
            ASSERT(4 == m1.theValue(1, 1));
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING HYMAN'S TEST CASE 2
        //
        // Concerns
        //: 1 Can construct a vector from an iterator range where the iterator
        //:    type has an unfortunate implicit conversion to 'ELEMENT_TYPE *'.
        //: 2: Can insert into a vector from an iterator range where the
        //:    iterator type has an unfortunate implicit conversion to
        //:    'ELEMENT_TYPE *'.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        int d[4][4] = {
             0,  1,  2,  3,
             4,  5,  6,  7,
             8,  9, 10, 11,
            12, 13, 14, 15,
        };

        const HI<int, 2> b(&d[0][0], 0);
        const HI<int, 2> e(&d[0][0]);
        {
            bsl::vector<int> bh(b, e);

            HI<int, 2> iter = b;
            for (size_t i = 0; i < bh.size(); ++i, ++iter) {
                if (veryVerbose) printf("%u %u %u\n", i, bh[i], *iter);
                ASSERTV(i, bh[i] == *iter);
            }

            bh.assign(b, e);
            for (size_t i = 0; i < bh.size(); ++i, ++iter) {
                if (veryVerbose) printf("%u %u %u\n", i, bh[i], *iter);
                ASSERTV(i, bh[i] == *iter);
            }
        }

        {
            bsl::vector<int> bh;
            bh.insert(bh.begin(), b, e);
            HI<int, 2> iter = b;
            for (size_t i = 0; i < bh.size(); ++i, ++iter) {
                if (veryVerbose) printf("%u %u %u\n", i, bh[i], *iter);
                ASSERTV(i, bh[i] == *iter);
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING HYMAN'S TEST CASE 1
        //
        // Concerns
        // 1: A range of derived objects is correctly sliced when inserted into
        //    a vector of base objects.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        {
            bsl::vector<B> bB(10);
            bsl::vector<A> bA(bB.begin(), bB.end());
            for (unsigned i = 0; i < bA.size(); ++i) {
                ASSERTV(i, bA[i].x, bA[i].x == 'a');
            }

            bA.assign(bB.begin(), bB.end());
            for (unsigned i = 0; i < bA.size(); ++i) {
                ASSERTV(i, bA[i].x, bA[i].x == 'a');
            }
        }

        {
            bsl::vector<B> bB(10);
            bsl::vector<A> bA;
            bA.insert(bA.begin(), bB.begin(), bB.end());
            for (unsigned i = 0; i < bA.size(); ++i) {
                ASSERTV(i, bA[i].x, bA[i].x == 'a');
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING VECTORS OF FUNCTION POINTERS
        //   In DRQS 34693876, it was observed that function pointers cannot
        //   be cast into 'void *' pointers. A 'reinterpret_cast' is required
        //   in this case. This is handled in 'bslalg_arrayprimitives'.
        //
        // Diagnosis:
        //   Vector is specialized for ptr types, and the specialization
        //   assumes that any pointer type can be cast or copy c'ted into a
        //   'void *', but for function ptrs on g++, this is not the case.
        //   Had to fix bslalg_arrayprimitives to deal with this, this test
        //   verifies that the fix worked.  DRQS 34693876.
        //
        // Concerns:
        //: 1 A vector of function pointers can be constructed from a sequence
        //:   of function pointers described by iterators that may be pointers
        //:   or simpler input iterators.
        //: 2 A vector of function pointers can insert a sequence of function
        //:   pointers described by iterators that may be pointers or simpler
        //:   input iterators.
        //
        // Testing:
        //   DRQS 34693876
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING VECTORS OF FUNCTION POINTERS"
                            "\n====================================\n");

        const charFnPtr VALUES[] = { TestFunc<VA>, TestFunc<VB>, TestFunc<VC>,
                                     TestFunc<VD> };
        const int NUM_VALUES = sizeof VALUES / sizeof VALUES[0];

        typedef FixedArray<charFnPtr>::iterator ArrayIterator;

        FixedArray<charFnPtr> l;
        l.push_back(TestFunc<VA>);
        l.push_back(TestFunc<VB>);
        l.push_back(TestFunc<VC>);
        l.push_back(TestFunc<VD>);

        {
            // construct vector from a pair of iterators that are not pointers
            vector<charFnPtr> w(l.begin(), l.end());
            ASSERT(4 == w.size());

            // Check the elements of w.
            vector<charFnPtr>::iterator wit = w.begin();

            for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
                ASSERT(wit != w.end());
                ASSERT(*it == *wit);
                ++wit;
            }
            ASSERT(wit == w.end());

            // insert a range from a pair of pointers indicating an array
            w.insert(w.end(), &VALUES[0], &VALUES[0] + NUM_VALUES);

            // Check the elements of w.
            wit = w.begin();

            for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
                ASSERT(wit != w.end());
                ASSERT(*it == *wit);
                ++wit;
            }

            for (int i = 0; i != NUM_VALUES; ++i) {
                ASSERT(wit != w.end());
                ASSERT(VALUES[i] == *wit);
                ++wit;
            }
            ASSERT(wit == w.end());
        }

        {
            // construct vector from a pair of pointers indicating an array
            vector<charFnPtr> w(&VALUES[0], &VALUES[0] + NUM_VALUES);
            ASSERT(4 == w.size());

            // Check the elements of w.
            vector<charFnPtr>::iterator wit = w.begin();

            for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
                ASSERT(wit != w.end());
                ASSERT(*it == *wit);
                ++wit;
            }
            ASSERT(wit == w.end());

            // insert a range with iterators that are not pointers
            w.insert(w.end(), l.begin(), l.end());

            // Check the elements of w.
            wit = w.begin();

            for (int i = 0; i != NUM_VALUES; ++i) {
                ASSERT(wit != w.end());
                ASSERT(VALUES[i] == *wit);
                ++wit;
            }

            for (ArrayIterator it = l.begin(); it != l.end(); ++it) {
                ASSERT(wit != w.end());
                ASSERT(*it == *wit);
                ++wit;
            }
            ASSERT(wit == w.end());
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION PTR RANGE-INSERT BUGFIX
        //   In DRQS 31711031, it was observed that a c'tor insert range from
        //   an array of function ptrs broke 'g++'.  Reproduce the bug.
        //
        // Diagnosis:
        //   Vector is specialized for ptr types, and the specialization
        //   assumes that any pointer type can be cast or copy c'ted into a
        //   'void *', but for function ptrs on g++, this is not the case.
        //   Had to fix bslalg_arrayprimitives to deal with this, this test
        //   verifies that the fix worked.  DRQS 31711031.
        //
        // Concerns:
        //: 1 A vector of function pointers can insert a sequence of function
        //:   pointers described by iterators that may be pointers or simpler
        //:   input iterators.
        //
        // Testing:
        //   DRQS 31711031
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTION PTR RANGE-INSERT BUGFIX"
                            "\n========================================\n");

        typedef int (*FuncPtr)();
        static FuncPtr funcPtrs[] = { &myFunc<0>, &myFunc<1>, &myFunc<2>,
                                      &myFunc<3>, &myFunc<4>, &myFunc<5>,
                                      &myFunc<6>, &myFunc<7>, &myFunc<8>,
                                      &myFunc<9> };
        enum { ARRAY_SIZE = sizeof(funcPtrs) /  sizeof(*funcPtrs) };

        vector<FuncPtr> v(funcPtrs + 0, funcPtrs + ARRAY_SIZE);
        for (int i = 0; i < 10; ++i) {
            LOOP2_ASSERT(i, (*v[i])(), i == (*v[i])());
        }

        v.clear();

        const FuncPtr * const cFuncPtrs = funcPtrs;

        v.insert(v.begin(), cFuncPtrs, cFuncPtrs + 10);
        for (int i = 0; i < 10; ++i) {
            LOOP2_ASSERT(i, (*v[i])(), i == (*v[i])());
        }

        v.clear();

        v.insert(v.begin(),     funcPtrs + 5, funcPtrs + 10);
        v.insert(v.begin(),     funcPtrs + 0, funcPtrs + 2);
        v.insert(v.begin() + 2, funcPtrs + 2, funcPtrs + 5);
        for (int i = 0; i < 10; ++i) {
            LOOP2_ASSERT(i, (*v[i])(), i == (*v[i])());
        }

        const vector<FuncPtr>& cv = v;
        vector<FuncPtr> w(cv);
        for (int i = 0; i < 10; ++i) {
            LOOP2_ASSERT(i, (w[i])(), i == (*w[i])());
        }
        w.insert(w.begin() + 5, cv.begin(), cv.begin() + 10);
        for (int i = 0; i < 20; ++i) {
            const int match = i - (i < 5 ? 0 : i < 15 ? 5 : 10);
            LOOP2_ASSERT(i, (w[i])(), match == (*w[i])());
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING NON-STANDARD TYPES
        //
        // Testing:
        //   CONCERN: Vector support types with overloaded new/delete
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NON-STANDARD TYPES"
                            "\n==========================\n");

        if (verbose) printf("\nTesting overloaded new/delete type"
                            "\n==================================\n");

        TestDriver<bsltf::NonTypicalOverloadsTestType>::testCase22();
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTIONS
        //
        // Testing:
        //   CONCERN: std::length_error is used properly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting use of 'std::length_error'"
                            "\n==================================\n");

        TestDriver<T>::testCase21();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase21();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase21();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase21();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON FREE OPERATORS
        //
        // Testing:
        //   bool operator<(const vector<T,A>&, const vector<T,A>&);
        //   bool operator>(const vector<T,A>&, const vector<T,A>&);
        //   bool operator<=(const vector<T,A>&, const vector<T,A>&);
        //   bool operator>=(const vector<T,A>&, const vector<T,A>&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COMPARISON FREE OPERATORS"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase20();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase20();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase20();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase20();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase20();

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'swap'
        //
        // Testing:
        //   void swap(Vector_Imp&);
        //   void swap(vector<T,A>& lhs, vector<T,A>& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'swap'"
                            "\n==============\n");

        TestDriver<T>::testCase19();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase19();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase19();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase19();

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'erase' AND 'pop_back'
        //
        // Testing:
        //   iterator erase(const_iterator position);
        //   iterator erase(const_iterator first, const_iterator last);
        //   void pop_back();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'erase' AND 'pop_back'"
                            "\n==============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase18();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase18();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase18();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase18();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase18();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase18();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase18();

        if (verbose) printf("\nNegative testing 'erase' and 'pop_back'"
                            "\n=======================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18Negative();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase18Negative();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase18Negative();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase18Negative();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase18Negative();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase18Negative();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase18Negative();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase18Negative();

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        //
        // Testing:
        //   void push_back(T&& value);
        //   iterator insert(const_iterator position, const T& value);
        //   void insert(const_iterator pos, size_type n, const T& val);
        //   template <class InputIter>
        //    void insert(const_iterator pos, InputIter first, InputIter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase17();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase17();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase17();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase17();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase17();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase17();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase17();

        if (verbose) printf("\nTesting Value Emplacement"
                            "\n=======================\n");

        // Test variadic templates with 0-5 arguments.

        for (int i = 0;i < 6; ++i) {
            if (verbose) printf("\n... with 'TestType', %2d arguments.\n", i);
            TestDriver<T>::testCase17Variadic(i);
        }
        for (int i = 0;i < 6; ++i) {
            if (verbose)
                printf("\n... with 'TestTypeNoAlloc', %2d arguments.\n", i);
            TestDriver<TNA>::testCase17Variadic(i);
        }
        for (int i = 0;i < 6; ++i) {
            if (verbose)
                printf(
                      "\n... with 'BitwiseMoveableTestType', %2d arguments.\n",
                      i);
            TestDriver<BMT>::testCase17Variadic(i);
        }
        for (int i = 0;i < 6; ++i) {
            if (verbose)
                printf(
                      "\n... with 'BitwiseCopyableTestType', %2d arguments.\n",
                      i);
            TestDriver<BCT>::testCase17Variadic(i);
        }

        if (verbose) printf("\nTesting Range Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<T>::testCase17Range(CharList<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testCase17Range(CharArray<T>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<BMT>::testCase17Range(CharList<BMT>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BMT>::testCase17Range(CharArray<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<BCT>::testCase17Range(CharList<BCT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BCT>::testCase17Range(CharArray<BCT>());

        if (verbose) printf("\nNegative Testing Insertions"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase17Negative();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase17Negative();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase17Negative();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase17Negative();

       if (verbose) printf("\nTesting iterator vs. value type deduction"
                           "\n=========================================\n");

        {
            vector<size_t> vna;
            vna.insert(vna.end(), 13, 42);

            ASSERT(13 == vna.size());
            ASSERT(42 == vna.front());
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        //
        // Testing:
        //   iterator begin();
        //   iterator end();
        //   reverse_iterator rbegin();
        //   reverse_iterator rend();
        //   const_iterator begin() const;
        //   const_iterator end() const;
        //   const_reverse_iterator rbegin() const;
        //   const_reverse_iterator rend() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ITERATORS"
                            "\n=================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase16();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase16();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase16();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase16();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase16();

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        //
        // Testing:
        //   T& operator[](size_type position);
        //   T& at(size_type n);
        //   T& front();
        //   T& back();
        //   const T& front() const;
        //   const T& back() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ELEMENT ACCESS"
                            "\n======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase15();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase15();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase15();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase15();

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nNegative Testing Element Access"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15Negative();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase15Negative();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase15Negative();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase15Negative();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase15Negative();
#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'reserve' AND 'capacity'
        //
        // Testing:
        //   void reserve(size_type n);
        //   void resize(size_type n, T val);
        //   size_type max_size() const;
        //   size_type capacity() const;
        //   bool empty() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'reserve' AND 'capacity'"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase14();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase14();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase14();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase14();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase14();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT
        //
        // Testing:
        //   void assign(size_t n, const T& val);
        //   template<class InputIter>
        //     void assign(InputIter first, InputIter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT"
                            "\n==================\n");

        if (verbose) printf("\nTesting Initial-Length Assignment"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase13();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase13();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase13();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase13();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase13();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase13();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase13();

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase13Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase13Range(CharArray<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<T>::testCase13Range(CharList<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testCase13Range(CharArray<T>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BMT>::testCase13Range(CharList<BMT>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BMT>::testCase13Range(CharArray<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BCT>::testCase13Range(CharList<BCT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BCT>::testCase13Range(CharArray<BCT>());

        if (verbose) printf("\nNegative-testing Assignment"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase13Negative(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase13Negative(CharArray<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<T>::testCase13Negative(CharList<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testCase13Negative(CharArray<T>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BMT>::testCase13Negative(CharList<BMT>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BMT>::testCase13Negative(CharArray<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BCT>::testCase13Negative(CharList<BCT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BCT>::testCase13Negative(CharArray<BCT>());

        if (verbose) printf("\nTest iterator vs. value type deduction"
                            "\n======================================\n");

        {
            vector<size_t> vna;
            vna.assign(13, 42);
            ASSERT(13 == vna.size());
            ASSERT(42 == vna.front());
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //  Specific concerns for each of the constructors are described in the
        //  separate testing function template, with separate test functions
        //  for the Range-based constructor vs. the length/value constructors.
        //  An additional concern is that there may be an ambiguity between the
        //  constructor deducing a pair of 'INPUT_ITERATOR' arguments of the
        //  same type, and the range/value constructor where the vector's
        //  'value_type' is the same as its 'size_type'.  This ambiguity should
        //  be resolved in favor of calling the length/value constructor.
        //
        // Plan:
        //  We will separately test the length/value constructor from the
        //  iterator-range based constructor.  Specific plans are described in
        //  the test functions 'testCase12' and 'testCase12Range' respectively.
        //  In addition, there is a specific concern about an ambiguity with
        //  the constructor for 'vector<size_t>'.  As the only concern is for a
        //  specific constructor invocation, on a specific container, we test
        //  that combination at the end, having sufficiently proven the rest of
        //  the constructor behavior that a simple verification that the
        //  correct overload was called should suffice.
        //
        // Testing:
        //   vector<T,A>(size_type n, const T& val = T(), const A& a = A());
        //   template<class InputIter>
        //     vector<T,A>(InputIter first, InputIter last, const A& a = A());
        //   vector(vector<T,A>&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCTORS"
                            "\n====================\n");

        if (verbose) printf("\nTesting Initial-Length Constructor"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase12();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase12();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase12();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase12();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase12();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase12();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase12();

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase12Range(CharList<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase12Range(CharArray<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<T>::testCase12Range(CharList<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testCase12Range(CharArray<T>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BMT>::testCase12Range(CharList<BMT>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BMT>::testCase12Range(CharArray<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BCT>::testCase12Range(CharList<BCT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BCT>::testCase12Range(CharArray<BCT>());


        if (verbose) printf("\nTesting Initial-Range vs. -Length Ambiguity"
                            "\n===========================================\n");

        {
            const vector<size_t> vna(13, 42);
            ASSERT(13 == vna.size());
            ASSERT(42 == vna.front());

            bslma::TestAllocator at;
            const vector<size_t> vwa(8, 12, &at);
            ASSERT( 8 == vwa.size());
            ASSERT(12 == vwa.front());
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR-RELATED CONCERNS
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATOR-RELATED CONCERNS"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase11();

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STREAMING FUNCTIONALITY"
                            "\n===============================\n");

        if (verbose)
            printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   Now that we can generate many values for our test objects, and
        //   compare results of assignments, we can test the assignment
        //   operator.    This is achieved by the 'testCase9' class method of
        //   the test driver template, instantiated for the basic test type.
        //   See that function for a list of concerns and a test plan.
        //
        // Testing:
        //   Obj& operator=(const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ASSIGNMENT OPERATOR"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase9();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase9();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase9();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase9();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, 'g'
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.  Because the generator is used for various types
        //   in higher numbered test cases, we need to test it on all test
        //   types.  This is achieved by the 'testCase8' class method of the
        //   test driver template, instantiated for the basic test type.  See
        //   that function for a list of concerns and a test plan.
        //
        // Testing:
        //   Obj g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTION, 'g'"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase8();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase8();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase8();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase8();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase8();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase8();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase8();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase8();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Having now full confidence in 'operator==', we can use it
        //   to test that copy constructors preserve the notion of
        //   value.  This is achieved by the 'testCase7' class method of the
        //   test driver template, instantiated for the basic test type.  See
        //   that function for a list of concerns and a test plan.
        //
        // Testing:
        //   Vector_Imp(const Vector_Imp& original);
        //   Vector_Imp(const Vector_Imp& original, alloc);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING COPY CONSTRUCTOR"
                            "\n========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase7();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase7();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase7();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase7();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase7();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase7();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase7();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase7();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY COMPARISION OPERATORS
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value.  This is achieved by the 'testCase6' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Testing:
        //   operator==(const Obj&, const Obj&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING EQUALITY COMPARISION OPERATORS"
                            "\n======================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase6();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase6();

        if (verbose) printf("\n... with 'BitwiseEqComparableTestType'.\n");
        TestDriver<BET>::testCase6();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase6();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase6();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase6();

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
        // TESTING BASIC ACCESSORS
        //   Having implemented an effective generation mechanism, we now would
        //   like to test thoroughly the basic accessor functions
        //     - size() const
        //     - operator[](int) const
        //   Also, we want to ensure that various internal state
        //   representations for a given value produce identical results.  This
        //   is achieved by the 'testCase4' class method of the test driver
        //   template, instantiated for the basic test type.  See that function
        //   for a list of concerns and a test plan.
        //
        // Testing:
        //   int size() const;
        //   const int& operator[](int index) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BASIC ACCESSORS"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase4();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase4();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase4();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase4();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase4();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase4();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase4();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase4();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //   This is achieved by the 'testCase3' class method of the test
        //   driver template, instantiated for the basic test type.  See that
        //   function for a list of concerns and a test plan.
        //
        // Testing:
        //   void ggg(Obj *object, const char *spec);
        //   Obj& gg(Obj *object, const char *spec, );
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTIONS"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase3();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase3();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase3();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase3();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase3();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase3();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase3();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase3();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //   We want to ensure that the primary manipulators
        //      - push_back             (black-box)
        //      - clear                 (white-box)
        //   operate as expected.  This is achieved by the 'testCase2' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Testing:
        //   void push_back(T const& v);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING PRIMARY MANIPULATORS (BOOTSTRAP)"
                            "\n========================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase2();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase2();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase2();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase2();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase2();

        if (verbose) printf("\n... with 'int *'.\n");
        TestDriver<int *>::testCase2();

        if (verbose) printf("\n... with 'const char *'.\n");
        TestDriver<const char *>::testCase2();

        if (verbose) printf("\n... with function pointers.\n");
        TestDriver<char (*)()>::testCase2();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   We want to exercise basic value-semantic functionality.  This is
        //   achieved by the 'testCase1' class method of the test driver
        //   template, instantiated for a few basic test types.  See that
        //   function for a list of concerns and a test plan.  In addition, we
        //   want to make sure that we can use any standard-compliant
        //   allocator, including not necessarily rebound to the same type as
        //   the contained element, and that various manipulators and accessors
        //   work as expected in normal operation.
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("\nStandard value-semantic test.\n");

        if (verbose) printf("\n\t... with 'char' type.\n");
        TestDriver<char>::testCase1();

        if (verbose) printf("\n\t... with 'TestType'.\n");
        TestDriver<T>::testCase1();

        if (verbose) printf("\n\t... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase1();

        if (verbose) printf("\n\t... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase1();

        if (verbose) printf("\n\t... with 'BitwiseCopyableTestType' .\n");
        TestDriver<BCT>::testCase1();

        if (verbose) printf("\nAdditional tests: allocators.\n");

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        bsl::allocator<int> zza(&testAllocator);

        // Disabled: in order to use bslstl_Vector_Imp, we disabled this very
        // unfrequent usage for Vector_Imp (it will be flagged by
        // 'BSLMF_ASSERT'):
        //..
        // Vector_Imp<int, bsl::allocator<void*> > zz1, zz2(zza);
        //..

        if (verbose) printf("\nAdditional tests: misc.\n");

        Vector_Imp<char> myVec(5, 'a');
        Vector_Imp<char>::const_iterator citer;
        ASSERT(5 == myVec.size());
        for (citer = myVec.begin(); citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        myVec.insert(myVec.begin(), 'z');
        ASSERT(6 == myVec.size());
        ASSERT('z' == myVec[0]);
        for (citer = myVec.begin() + 1; citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        myVec.erase(myVec.begin() + 2, myVec.begin() + 4);
        ASSERT(4 == myVec.size());
        ASSERT('z' == myVec[0]);
        for (citer = myVec.begin() + 1; citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        Vector_Imp<Vector_Imp<char> > vv;
        vv.push_back(myVec);
        if (verbose) P(myVec);

        if (verbose) printf("\nAdditional tests: traits.\n");

#ifndef BSLS_PLATFORM_CMP_MSVC  // Temporarily does not work
        ASSERT(  (bslmf::IsBitwiseMoveable<vector<char> >::value));
        ASSERT(  (bslmf::IsBitwiseMoveable<vector<T> >::value));
        ASSERT(  (bslmf::IsBitwiseMoveable<vector<Vector_Imp<int> > >::value));
#endif

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   Provide benchmark for subsequent improvements to 'bslstl_vector'.
        //   The benchmark should measure the speed of various operations such
        //   as 'push_back', 'pop_back', 'erase', 'insert',
        //   etc.
        //
        // Plan:
        //   Using 'bsls_stopwatch', the run time of the various methods under
        //   test be tallied over various iterations.  These values should only
        //   be used as a comparison across various versions.  They are NOT
        //   meant to be used to compare which method runs faster since the
        //   loops they run in have various lengths.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nPERFORMANCE TEST"
                            "\n================\n");

        if (verbose) printf("\n... with 'char' type.\n");
        TestDriver<char>::testCaseM1();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCaseM1();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCaseM1();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCaseM1();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' .\n");
        TestDriver<BCT>::testCaseM1();

        if (verbose) printf("\nPERFORMANCE TEST RANGE"
                            "\n======================\n");

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<T>::testCaseM1Range(CharList<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testCaseM1Range(CharArray<T>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<BMT>::testCaseM1Range(CharList<BMT>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BMT>::testCaseM1Range(CharArray<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<BCT>::testCaseM1Range(CharList<BCT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BCT>::testCaseM1Range(CharArray<BCT>());

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    bslma::Default::setGlobalAllocator(originalGlobalAllocator);

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
