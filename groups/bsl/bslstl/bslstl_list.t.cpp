// bslstl_list.t.cpp                                                  -*-C++-*-
#include <bslstl_list.h>
#include <bslstl_iterator.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatorexception.h>  // for testing only

#include <bslmf_assert.h>                  // for testing only
#include <bslmf_issame.h>                  // for testing only

#include <bsls_alignmentutil.h>
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>                // for testing only
#include <bsls_types.h>

#include <bsltf_nonassignabletesttype.h>   // for testing only

#include <stdexcept>  // 'length_error', 'out_of_range'
#include <algorithm>  // 'next_permutation'
#include <functional> // 'less'

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstddef>

using namespace BloombergLP;
using namespace std;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The object under testing is a container whose interface and contract is
// dictated by the C++ standard.  The general concerns are compliance,
// exception safety (including the strong guarantee for insert and emplace),
// and proper dispatching (for member function templates such as assign and
// insert).  In addition, it is a value-semantic type whose salient attributes
// are size and value of each element in sequence.  This container is
// implemented in the form of a class template, and thus its proper
// instantiation for several types is a concern.  Regarding the allocator
// template argument, we use mostly a 'bsl::allocator' together with a
// 'bslma_TestAllocator' mechanism, but we also verify the C++ standard.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' and 'clear' methods to be used by the generator functions 'g'
// and 'gg'.  Note that some manipulators must support aliasing, and those
// that perform memory allocation must be tested for exception neutrality via
// the 'bslma_testallocator' component.  After the mandatory sequence of cases
// (1--10) for value-semantic types (cases 5 and 10 are not implemented, as
// there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     T            VALUE (template argument, no default)
//     A            ALLOCATOR (template argument, default: bsl::allocator<T>)
//     list<T,A>    bsl::list<VALUE_TYPE,ALLOCATOR>
//     list         list<T,A>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//
// The of tests below is grouped as per the definition of list in the C++
// standard (construct, iterators, capacity...) rather than the canonical
// grouping of members per BDE convention (CREATORS, MANIPULATORS, ACCESSORS).
// ----------------------------------------------------------------------------
// class list<T,A> (list)
// ============================================================================
//
// TYPES:
// [21] reference
// [21] const_reference
// [16] iterator
// [16] const_iterator
// [21] size_type
// [21] difference_type
// [21] value_type
// [21] allocator_type
// [21] pointer
// [21] const_pointer
// [16] reverse_iterator
// [16] const_reverse_iterator
// [22] TYPE TRAITS
//
// CREATORS:
// [ 2] list(const A& a = A());
// [12] list(size_type n, const T& val = T(), const A& a = A());
// [12] template<class InputIter>
//        list(InputIter first, InputIter last, const A& a = A());
// [ 7] list(const list& orig, const A& = A());
// [ 2] ~list();
//
/// MANIPULATORS:
// [ 9] operator=(list&);
// [13] template <class InputIter>
//        void assign(InputIter first, InputIter last);
// [13] void assign(size_type numElements, const T& val);
// [16] iterator begin();
// [16] iterator end();
// [16] reverse_iterator rbegin();
// [16] reverse_iterator rend();
// [14] void resize(size_type n);
// [14] void resize(size_type n, const T& val);
// [15] reference front();
// [15] reference back();
// [17] void push_front(const T&);
// [18] void pop_front();
// [17] void push_back(const T&);
// [18] void pop_back();
// [17] iterator insert(const_iterator pos, const T& val);
// [17] iterator insert(const_iterator pos, size_type n, const T& val);
// [17] template <class InputIter>
//        void insert(const_iterator pos, InputIter first, InputIter last);
// [18] iterator erase(const_iterator pos);
// [18] iterator erase(const_iterator first, const_iterator last);
// [19] void swap(list&);
// [ 2] void clear();
// [24] void splice(iterator pos, list& other);
// [24] void splice(iterator pos, list& other, iterator i);
// [24] void splice(iterator pos, list& other, iterator first, iterator last);
// [25] void remove(const T& val);
// [25] template <class PRED> void remove_if(PRED p);
// [26] void unique();
// [26] template <class BINPRED> void unique(BINPRED p);
// [27] void merge(list& other);
// [27] template <class COMP> void merge(list& other, COMP c);
// [28] void sort();
// [28] template <class COMP> void sort(COMP c);
// [23] void reverse();
//
// ACCESSORS:
// [11] A get_allocator() const;
// [16] const_iterator begin();
// [16] const_iterator end();
// [16] const_reverse_iterator rbegin();
// [16] const_reverse_iterator rend();
// [ 4] bool empty() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [15] const_reference front() const;
// [15] const_reference back() const;
//
// FREE OPERATORS:
// [ 6] bool operator==(const list<T,A>&, const list<T,A>&);
// [ 6] bool operator!=(const list<T,A>&, const list<T,A>&);
// [20] bool operator<(const list<T,A>&, const list<T,A>&);
// [20] bool operator>(const list<T,A>&, const list<T,A>&);
// [20] bool operator<=(const list<T,A>&, const list<T,A>&);
// [20] bool operator>=(const list<T,A>&, const list<T,A>&);
// [19] void swap(list<T,A>&, list<T,A>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] ALLOCATOR-RELATED CONCERNS
// [29] USAGE EXAMPLE
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(list<T,A> *object, const char *spec, int vF = 1);
// [ 3] list<T,A>& gg(list<T,A> *object, const char *spec);
// [ 8] list<T,A> g(const char *spec);
// [ 4] iterator succ(iterator);
// [ 4] const_iterator succ(iterator) const;
// [ 4] T& nthElem(list& x, int n);
// [ 4] const T& nthElem(list& x, int n) const;
// [ 4] bool is_mutable(T&);
// [ 4] bool is_mutable(const T&);
//-----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

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
class TestTypeOtherAlloc;
template <class T> class OtherAllocator;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators
typedef TestTypeOtherAlloc            TOA;  // Uses non-'bslma' allocators

typedef OtherAllocator<TestType>           OAT;   // Non-'bslma' allocator
typedef OtherAllocator<TestTypeOtherAlloc> OATOA; // Non-'bslma' allocator

typedef bsls::Types::Int64             Int64;
typedef bsls::Types::Uint64            Uint64;

// TEST OBJECT (unless o/w specified)
typedef char                          Element;  // every TestType's value type
typedef list<T>                       Obj;

// CONSTANTS
const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';
const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
const char VF = 'F';
const char VG = 'G';
const char VH = 'H';
    // All test types have character value type.

const int  LARGE_SIZE_VALUE = 10;
    // Declare a large value for insertions into the list.

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%hd", val); fflush(stdout); }
inline void dbg_print(unsigned short val) {printf("%hu", val); fflush(stdout);}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%lu", val); fflush(stdout); }
inline void dbg_print(unsigned long val) { printf("%lu", val); fflush(stdout);}
inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

// List-specific print function.
template <class TYPE, class ALLOC>
void dbg_print(const list<TYPE,ALLOC>& v)
{
    if (v.empty()) {
        printf("<empty>");
    }
    else {
        typedef typename list<TYPE,ALLOC>::const_iterator iter;
        for (iter i = v.begin(); i != v.end(); ++i)
        {
            dbg_print(*i);
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

// Return the 'n'th iterator after 'it':
template <class ITER>
inline ITER succ(ITER it, int n = 1)
{
    for (int i = 0; i < n; ++i)
        ++it;
    return it;
}

// Return the 'n'th element of container x, counting from 0.  (I.e., if for
// n == 0, return x.front().)
template <class C>
inline typename C::value_type& nthElem(C& x, int n)
{
    return *succ(x.begin(), n);
}

template <class C>
inline const typename C::value_type& nthElem(const C& x, int n)
{
    return *succ(x.begin(), n);
}

template <class T>
inline bool is_mutable(T& /* x */) { return true; }
    // Return 'true'.  Preferred match if 'x' is a modifiable lvalue.

template <class T>
inline bool is_mutable(const T& /* x */) { return false; }
    // Return 'false'.  Preferred match if 'x' is an rvalue or const lvalue.

template <class T>
inline T as_rvalue(const T& x) { return x; }
    // Return a copy of 'x' as an rvalue, even if called on a (possibly
    // const-qualified) lvalue or reference argument.

template <class T>
inline char value_of(const T& x) { return static_cast<char>(x); }
    // Return the char value corresponding to 'x'.  Specialized for each test
    // type.

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
    void resetValue(const VALUE_TYPE& value, int line) {
        d_lineNum = line;
        d_value = value;
    }

    void release() {
        d_object_p = 0;
    }
};
                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

    char            *d_data_p;
    bslma::Allocator *d_allocator_p;

    void init(const TestType& x) {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *x.d_data_p;
    }

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestType, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    TestType(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestType(char c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCopyCtorCalls;
        ASSERT(&original != this);
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }

    TestType(int a1, const TestType& x, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ASSERT(1 == a1);
        init(x);
    }

    TestType(int a1, int a2, const TestType& x, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        init(x);
    }

    TestType(int a1, int a2, int a3, const TestType& x,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        init(x);
    }

    TestType(int a1, int a2, int a3, int a4, const TestType& x,
             bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ASSERT(1 == a1);
        ASSERT(2 == a2);
        ASSERT(3 == a3);
        ASSERT(4 == a4);
        init(x);
    }

    ~TestType() {
        ++numDestructorCalls;
        ASSERT(d_data_p != 0);
        *d_data_p = UNINITIALIZED_VALUE;
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
        d_allocator_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
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

    bslma::Allocator *allocator() const {
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

// TestType-specific value_of function.
template <>
inline char value_of<TestType>(const TestType& x)
{
    return x.value();
}

// Specializations of std::less and std::equal_to should never be called.
// Certain algorithms have variants that call either a predicate function or
// operator<.  A non-compliant implementation may implement the latter variant
// by calling the former variant using std::less because most of the time,
// they are identical.  Unfortunately, the standard does not require that
// std::less do the same thing as operator<.  The same problem occurs with
// std::equal_to and operator==.  These specializations suppress the default
// definitions of std::less and std::equal_t and intercept stray calls by
// non-compliant implementations.

namespace std {
    template <>
    struct less<TestType> : binary_function<TestType, TestType, bool> {
        bool operator()(const TestType& a, const TestType& b) const {
            ASSERT(!"less<TestType> should not be called");
            return a < b;
        }
    };

    template <>
    struct equal_to<TestType> : binary_function<TestType, TestType, bool> {
        bool operator()(const TestType& a, const TestType& b) const {
            ASSERT(!"equal_to<TestType> should not be called");
            return a == b;
        }
    };
}  // close namespace std

                       // =====================
                       // class TestTypeNoAlloc
                       // =====================

class TestTypeNoAlloc {
    // This test type has footprint and interface identical to 'TestType'.  It
    // also counts the number of default and copy constructions, assignments,
    // and destructions.

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

// TestType-specific print function.
void dbg_print(const TestTypeNoAlloc& rhs) {
    printf("%c", rhs.value());
    fflush(stdout);
}

// TestTypeTypeNoAlloc-specific value_of function.
template <>
inline char value_of<TestTypeNoAlloc>(const TestTypeNoAlloc& x)
{
    return x.value();
}

                               // ====================
                               // class OtherAllocator
                               // ====================

bslma::TestAllocator OtherAllocatorDefaultImp;

template <class T>
class OtherAllocator
{
    bslma::Allocator* d_implementation;

  public:
    // An STL allocator type other than 'bsl::allocator'.  Like
    // 'bsl::allocator', it is constructed with a 'bslma_Allocator' pointer,
    // but it is not implicitly convertible from 'bslma_Allocator*'.

    // TYPES
    typedef T          value_type;
    typedef T         *pointer;
    typedef const T   *const_pointer;
    typedef T&         reference;
    typedef const T&   const_reference;
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;

    template <class U>
    struct rebind
    {
        typedef OtherAllocator<U> other;
    };

    // Constructors
    explicit OtherAllocator(bslma::Allocator* a) : d_implementation(a) { }
    OtherAllocator() : d_implementation(&OtherAllocatorDefaultImp) { }
    template <class U> OtherAllocator(const OtherAllocator<U>& other)
        : d_implementation(other.implementation()) { }

    // Manipulators
    T* allocate(size_t n, const void* = 0)
        { return static_cast<T*>(d_implementation->allocate(sizeof(T)*n)); }
    void deallocate(T* p, size_t) { d_implementation->deallocate(p); }
    void construct(pointer p, const T& v) { ::new((void*) p) T(v); }
    void destroy(pointer p) { p->~T(); }

    // Accessors
    size_t max_size() const { return ((size_t)-1) / sizeof(T); }
    bslma::Allocator* implementation() const { return d_implementation; }
};

template <class T, class U>
inline
bool operator==(const OtherAllocator<T>& a, const OtherAllocator<U>& b)
{
    return a.implementation() == b.implementation();
}

template <class T, class U>
inline
bool operator!=(const OtherAllocator<T>& a, const OtherAllocator<U>& b)
{
    return a.implementation() != b.implementation();
}

                               // ========================
                               // class TestTypeOtherAlloc
                               // ========================

class TestTypeOtherAlloc {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestTypeOtherAlloc'.

    char                 *d_data_p;
    OtherAllocator<char>  d_allocator;

  public:
    // TYPES
    typedef OtherAllocator<char> allocator_type;

    // CREATORS
    explicit
    TestTypeOtherAlloc(allocator_type a = allocator_type())
    : d_data_p(0)
    , d_allocator(a)
    {
        ++numDefaultCtorCalls;
        d_data_p  = d_allocator.allocate(1);
        *d_data_p = DEFAULT_VALUE;
    }

    explicit
    TestTypeOtherAlloc(char c, allocator_type a = allocator_type())
    : d_data_p(0)
    , d_allocator(a)
    {
        ++numCharCtorCalls;
        d_data_p  = d_allocator.allocate(1);
        *d_data_p = c;
    }

    TestTypeOtherAlloc(const TestTypeOtherAlloc& original,
                       allocator_type            a = allocator_type())
    : d_data_p(0)
    , d_allocator(a)
    {
        ++numCopyCtorCalls;
        if (&original != this) {
            d_data_p  = d_allocator.allocate(1);
            *d_data_p = *original.d_data_p;
        }
    }

    ~TestTypeOtherAlloc() {
        ++numDestructorCalls;
        *d_data_p = UNINITIALIZED_VALUE;
        d_allocator.deallocate(d_data_p, 1);
        d_data_p = 0;
    }

    // MANIPULATORS
    TestTypeOtherAlloc& operator=(const TestTypeOtherAlloc& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
            char *newData  = d_allocator.allocate(1);
            *d_data_p = UNINITIALIZED_VALUE;
            d_allocator.deallocate(d_data_p, 1);
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

    allocator_type get_allocator() const {
        return d_allocator;
    }

    bslma::Allocator* allocator() const {
        return d_allocator.implementation();
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
bool operator==(const TestTypeOtherAlloc& lhs, const TestTypeOtherAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() == rhs.value();
}

bool operator<(const TestTypeOtherAlloc& lhs, const TestTypeOtherAlloc& rhs)
{
    ASSERT(isalpha(lhs.value()));
    ASSERT(isalpha(rhs.value()));

    return lhs.value() < rhs.value();
}

// TestTypeOtherAlloc-specific print function.
void dbg_print(const TestTypeOtherAlloc& rhs) {
    printf("%c", rhs.value());
    fflush(stdout);
}

// TestTypeOtherAlloc-specific value_of function.
template <>
inline char value_of<TestTypeOtherAlloc>(const TestTypeOtherAlloc& x)
{
    return x.value();
}

                              // =============
                              // class RandSeq
                              // =============

template <class TYPE>
class RandSeq {
    // Random-access sequence.  This class is a simple wrapper around an array
    // offering a random-access iterator access via the 'begin' and 'end'
    // accessors.  The iterator is specifically a *random-access* iterator and
    // its value type is the parameterized 'TYPE'.

    // DATA
    TYPE*  d_value;
    size_t d_len;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    RandSeq() : d_value(0), d_len(0) {}
    RandSeq(const char* value);
    ~RandSeq();

    // ACCESSORS
    const TYPE& operator[](size_t index) const;
    const_iterator begin() const;
    const_iterator end() const;
};

// CREATORS
template <class TYPE>
RandSeq<TYPE>::RandSeq(const char* value)
    : d_value(0), d_len(std::strlen(value))
{
    if (d_len)
    {
        d_value = (TYPE*) ::operator new(d_len * sizeof(TYPE));
        for (size_t i = 0; i < d_len; ++i) {
            ::new((void*) &d_value[i]) TYPE(value[i]);
        }
    }
}

template <class TYPE>
RandSeq<TYPE>::~RandSeq()
{
    for (size_t i = 0; i < d_len; ++i) {
        d_value[i].~TYPE();
    }

    ::operator delete(d_value);
}

// ACCESSORS
template <class TYPE>
const TYPE& RandSeq<TYPE>::operator[](size_t index) const {
    return d_value[index];
}

template <class TYPE>
typename RandSeq<TYPE>::const_iterator RandSeq<TYPE>::begin() const {
    return const_iterator(d_value);
}

template <class TYPE>
typename RandSeq<TYPE>::const_iterator RandSeq<TYPE>::end() const {
    return const_iterator(d_value + d_len);
}

                               // ==============
                               // class InputSeq
                               // ==============

template <class TYPE> class InputSeq;
template <class TYPE> class InputSeqConstIterator;

template <class TYPE>
bool operator==(InputSeqConstIterator<TYPE> a, InputSeqConstIterator<TYPE> b);

template <class TYPE>
class InputSeqConstIterator {
    // Const iterator type for the InputSeq container type.  This iterator is
    // specifically an *input* iterator.  Iteration is not intended to be
    // restarted from a copy of an iterator.  If one iterator is a copy of
    // another, then incrementing either one will invalidate the other one.
    // Use of an invalidated iterator is checked for.

    typedef typename RandSeq<TYPE>::const_iterator BaseIterType;
    typedef bsl::iterator_traits<BaseIterType>     BaseIterTraits;

    // Input iterators are not value-semantic types.  In particular, if two
    // iterators compare equal and both are incremented, they need not
    // continue to compare equal.  We allow only one "active" iterator on a
    // given sequence.  We keep track of the active iterator by keeping a
    // master iterator in the container.  The 'd_master' member of valid
    // iterator will point to the master and its 'd_imp' member will have the
    // same value as the master.  If a valid iterator is copied by copy
    // construction or assignment, then both the original and copy are valid.
    // However, as soon as one is incremented, the 'd_imp' of other one will
    // no longer match the master, making it invalid.  As a special case, an
    // iterator with a null master is valid but not incrementable.  These
    // special iterators are used for the 'end' iterator and for the return of
    // the post-increment operator.
    BaseIterType *d_master;
    BaseIterType  d_imp;

    InputSeqConstIterator(BaseIterType *m, BaseIterType i)
        : d_master(m), d_imp(i) { }
        // Construct an iterator using 'm' as the address of the master
        // iterator and 'i' as the initial iterator position.  If 'm' is null,
        // then the resulting iterator is valid, but may not be incremented.
        // If 'm' is non-null and '*m' does not equal 'i', then the resulting
        // iterator is invalid -- it may not be dereferenced or incremented,
        // but it may be assigned a new value.

    friend class InputSeq<TYPE>;
    template <class T>
    friend bool operator==(InputSeqConstIterator<T> a,
                           InputSeqConstIterator<T> b);

  public:
    typedef std::input_iterator_tag                  iterator_category;
    typedef typename BaseIterTraits::value_type      value_type;
    typedef typename BaseIterTraits::difference_type difference_type;
    typedef typename BaseIterTraits::pointer         pointer;
    typedef typename BaseIterTraits::reference       reference;

    // Use compiler-generated copy constructor, assignment, and destructor:
    // InputSeqConstIterator(const InputSeqConstIterator&);
    // InputSeqConstIterator& operator=(const InputSeqConstIterator&);
    // ~InputSeqConstIterator();

    // MANIPULATORS
    InputSeqConstIterator& operator++() {
        ASSERT(d_master && d_imp == *d_master);  // test if incrementable
        if (!(d_master && d_imp == *d_master)) {
            // Continue test despite error by creating self-mastered iterator.
            // This assignment also prevents cascade errors.
            d_master = &d_imp;
        }
        d_imp = ++*d_master;
        return *this;
    }

    const InputSeqConstIterator operator++(int)
        { InputSeqConstIterator ret(0,d_imp); ++*this; return ret; }

    // ACCESSORS
    reference operator*() const {
        ASSERT( ! d_master || d_imp == *d_master); // test if valid
        return *d_imp;
    }
    pointer operator->() const { return &this->operator*(); }
};

template <class TYPE>
class InputSeq {
    // This array class is a simple wrapper on a 'char' array offering an
    // input iterator access via the 'begin' and 'end' accessors.  The
    // iterator is specifically an *input* iterator and its value type
    // is the parameterized 'TYPE'.

    // DATA
    RandSeq<TYPE>                                  d_value;
    mutable typename RandSeq<TYPE>::const_iterator d_masterIter;

  public:
    // TYPES
    typedef InputSeqConstIterator<TYPE> const_iterator;

    // CREATORS
    InputSeq() {}
    InputSeq(const char* value);

    // ACCESSORS
    const_iterator begin() const;
    const_iterator end() const;

    const TYPE operator[](int idx) const { return d_value[idx]; }
};

// CREATORS
template <class TYPE>
InputSeq<TYPE>::InputSeq(const char* value)
  : d_value(value)
{
}

// ACCESSORS
template <class TYPE>
inline
InputSeqConstIterator<TYPE> InputSeq<TYPE>::begin() const {
    d_masterIter = d_value.begin();
    return InputSeqConstIterator<TYPE>(&d_masterIter, d_masterIter);
}

template <class TYPE>
inline
InputSeqConstIterator<TYPE> InputSeq<TYPE>::end() const {
    return InputSeqConstIterator<TYPE>(0, d_value.end());
}

template <class TYPE>
inline
bool operator==(InputSeqConstIterator<TYPE> a, InputSeqConstIterator<TYPE> b)
{
    return (a.d_imp == b.d_imp &&
            (a.d_master == b.d_master || 0 == a.d_master || 0 == b.d_master));
}

template <class TYPE>
inline
bool operator!=(InputSeqConstIterator<TYPE> a, InputSeqConstIterator<TYPE> b)
{
    return ! (a == b);
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
        // It is better not to inherit the rebind template, or else
        // rebind<X>::other would be ALLOC::rebind<OTHER_TYPE>::other
        // instead of LimitAlloc<X>.

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

    // Templatize to make this a better match than the next constructor.
    template <class BSLMA_ALLOC>
    explicit LimitAllocator(BSLMA_ALLOC *mechanism)
        : AllocBase(mechanism), d_limit(-1) { }

    template <class U_ALLOC>
    LimitAllocator(const U_ALLOC& rhs)
        : AllocBase(rhs), d_limit(rhs.max_size()) { }

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

namespace bslma {

template <class ALLOCATOR>
struct UsesBslmaAllocator<LimitAllocator<ALLOCATOR> >
    : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma
}  // close enterprise namespace

                              // ====================
                              // class PointerWrapper
                              // ====================

template <class T> class PointerWrapper;

template <class T>
class PointerWrapper<const T>
{
    // Wrapper around a raw pointer to const T.  Convertible both ways.

  protected:
    T* d_imp;

  public:
    PointerWrapper() { }
    PointerWrapper(const T* p) : d_imp(const_cast<T*>(p)) { }

    operator const T* ()  const { return d_imp; }
    const T* operator->() const { return d_imp; }
    const T& operator*()  const { return *d_imp; }
};

template <class T>
class PointerWrapper : public PointerWrapper<const T>
{
    // Wrapper around a raw pointer to mutable T.  Convertible both ways.

  public:
    PointerWrapper() { }
    PointerWrapper(T* p) { this->d_imp = p; }

    operator T* ()  const { return this->d_imp; }
    T* operator->() const { return this->d_imp; }
    T& operator*()  const { return *this->d_imp; }
};

                              // ====================
                              // class SmallAllocator
                              // ====================

template <class T>
class SmallAllocator : public bsl::allocator<T> {
    // Allocator type with small size and difference types and non-raw pointer
    // types.  Used to test that these types are used in the interface to
    // the container.

    // PRIVATE TYPES
    typedef bsl::allocator<T> AllocBase;

  public:
    // TYPES
    typedef typename AllocBase::value_type        value_type;
    // typedef typename AllocBase::pointer           pointer;
    // typedef typename AllocBase::const_pointer     const_pointer;
    typedef PointerWrapper<T>                     pointer;
    typedef PointerWrapper<const T>               const_pointer;
    typedef typename AllocBase::reference         reference;
    typedef typename AllocBase::const_reference   const_reference;
    typedef unsigned short                        size_type;
    typedef short                                 difference_type;

    template <class OTHER_TYPE> struct rebind {
        typedef SmallAllocator<OTHER_TYPE> other;
    };

    // CREATORS
    SmallAllocator() { }

    explicit SmallAllocator(bslma::Allocator *mechanism)
        : AllocBase(mechanism) { }

    template <class U>
    SmallAllocator(const SmallAllocator<U>& rhs) : AllocBase(rhs) { }

    ~SmallAllocator() { }
};

                              // ================
                              // class IntWrapper
                              // ================

class IntWrapper
{
    // Simple wrapper object implicitly convertible from 'int'.

    int d_val;

  public:
    IntWrapper(int v = 0) : d_val(v) { }

    int value() const { return d_val; }
};

inline
bool operator==(IntWrapper a, IntWrapper b)
{
    return a.value() == b.value();
}

inline
bool operator!=(IntWrapper a, IntWrapper b)
{
    return ! (a == b);
}

enum TestEnum { TWO = 2, NINETYNINE = 99 };

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Filter "Twinkle Star"
/// - - - - - - - - - - - - - - - -
// Suppose an observatory needs to analyze the results of a sky survey.  The
// raw data is a text file of star observations where each star is represented
// by a tuple of three numbers: (x, y, b), where x and y represent the angular
// coordinates of the star in the sky and b represents its brightness on a
// scale of 0 to 100.  A star having brightness 75 or higher is of particular
// interest, which is named "twinkle star".
//
// Our first example will read such a data file as described above, filter out
// the dim stars (brightness less than 75), and count the "twinkle star"s left
// in the list.  Our test data set has been selected such that there are 10
// stars in the set, of which 4 are sufficiently bright as to pass our filter.
//
// First, we define the class 'Star' that encapsulates a single tuple, and
// provides accessors functions 'x', 'y', and 'brightness', file I/O functions
// 'read' and 'write', and free operators '==', '!=', and '<':
//..
#include <cstdio>
using namespace std;

class Star
    // This class represents a star as seen through a digital telescope.
{
    // DATA
    double d_x, d_y;     // coordinates

    int    d_brightness; // brightness on a scale of 0 to 100

  public:
    // CREATORS
    Star()
        // Create a 'Star' object located at coordinates '(0, 0)' having
        // '0' brightness.
    : d_x(0), d_y(0), d_brightness(0)
    {
    }

    Star(double x, double y, int b)
        // Create a 'Star' object located at the specified coordinates
        // '(x, y)' having the specified 'b' brightness.
    : d_x(x), d_y(y), d_brightness(b)
    {
    }

    // Compiler-generated copy construction, assignment, and destructor
    // Star(const Star&) = default;
    // Star& operator=(const Star&) = default;
    // ~Star() = default;

    // MANIPULATORS
    bool read(FILE *input);
        // Read x, y, and brightness from the specified 'input' file.
        // Return 'true' if the read succeeded and 'false' otherwise.

    void write(FILE *output) const;
        // Write x, y, and brightness to the specified 'output' file
        // followed by a newline.

    // ACCESSORS
    double x() const
        // Return the x coordinate of this 'Star' object.
    {
        return d_x;
    }

    double y() const
        // Return the y coordinate of this 'Star' object.
    {
        return d_y;
    }

    int brightness() const
        // Return the brightness of this 'Star' object.
    {
        return d_brightness;
    }
};

// FREE FUNCTIONS
bool operator==(const Star& lhs, const Star& rhs);
bool operator!=(const Star& lhs, const Star& rhs);
bool operator< (const Star& lhs, const Star& rhs);
//..
// Then, we define a 'readData' method that reads a file of data points and
// appends each onto a list.  The stars are stored in the data file in
// ascending sorted order by x and y coordinates.
//..
void readData(list<Star> *starList, FILE *input)
{
    Star s;
    while (s.read(input)) {
        starList->push_back(s);
    }
}
//..
// Now, we define the 'filter' method, which is responsible for removing stars
// with a brightness of less than 75 from the data set.  It does this by
// iterating over the list and erasing any element that does not pass the
// filter.  The list object features a fast 'erase' member function.  The
// return value of 'erase' is an iterator to the element immediately following
// the erased element:
//..
void filter(list<Star> *starList)
{
    static const int threshold = 75;

    list<Star>::iterator i = starList->begin();
    while (i != starList->end()) {
        if (i->brightness() < threshold) {
            i = starList->erase(i);  // Erase and advance to next element.
        }
        else {
            ++i;  // Advance to next element without erasing
        }
    }
}
//..
// Finally, we use the methods defined in above steps to put together our
// program to find twinkle stars:
//..
int usageExample1(int verbose)
{
    FILE *input = fopen("star_data1.txt", "r");  // Open input file.
    ASSERT(input);

    list<Star> starList;                         // Define a list of stars.
    ASSERT(starList.empty());                    // A list should be empty
                                                 // after default
                                                 // construction.

    readData(&starList, input);                  // Read input to the list.
    ASSERT(10 == starList.size());               // Verify correct reading.
    fclose(input);                               // Close input file.

    filter(&starList);                           // Pick twinkle stars.
    ASSERT(4 == starList.size());                // Verify correct filter.

    // Print out twinkle stars.
    if (verbose) {
        for (list<Star>::const_iterator i = starList.begin();
                i != starList.end(); ++i) {
            i->write(stdout);
        }
    }
    return 0;
}
//..

///Example 2: Combine Two Star Surveys
///- - - - - - - - - - - - - - - - - -
// Now we want to combine the results from two star surveys into a single
// list, using the same 'Star' class defined in the first usage example.
//
// First, we begin by reading both lists and filtering them.  (Our test data is
// selected so that the second data file contains 8 starts of which 3 are
// sufficiently bright as to pass our filter:
//..
int usageExample2(int verbose)
{
    FILE *input = fopen("star_data1.txt", "r");  // Open first input file.
    ASSERT(input);

    list<Star> starList1;                        // Define first star list.
    ASSERT(starList1.empty());

    readData(&starList1, input);                 // Read input into list.
    ASSERT(10 == starList1.size());
    fclose(input);                               // Close first input file.

    input = fopen("star_data2.txt", "r");        // Open second input file.
    ASSERT(input);

    list<Star> starList2;                        // Define second list.
    ASSERT(starList2.empty());

    readData(&starList2, input);                 // Read input into list.
    ASSERT(8 == starList2.size());
    fclose(input);                               // Close input file.

    filter(&starList1);                          // Pick twinkle stars from
                                                 // the first star list.
    ASSERT(4 == starList1.size());

    filter(&starList2);                          // Pick twinkle stars from
                                                 // the second star list.
    ASSERT(3 == starList2.size());
//..
// Then, we combine the two lists, 'starList1' and 'starList2'.  One way to do
// this is to simply insert the second list at the end of the first:
//..
    list<Star> tmp1(starList1);  // Make a copy of the first list
    list<Star> tmp2(starList2);  // Make a copy of the second list
    tmp1.insert(tmp1.end(), tmp2.begin(), tmp2.end());
    ASSERT(7 == tmp1.size());    // Verify combined size.
    ASSERT(3 == tmp2.size());    // 'tmp2' should be unchanged.
//..
// Next, let's have a closer look of the above code and see if we can improve
// the combination performance.  The above 'insert' method appends a copy of
// each element in 'tmp2' onto the end of 'tmp1'.  This copy is unnecessary
// because we have no need for 'tmp2' after the lists have been combined.  A
// faster and less-memory-intensive technique is to use the 'splice' function,
// which *moves* rather than *copies* elements from one list to another:
//..
    tmp1 = starList1;
    tmp2 = starList2;
    tmp1.splice(tmp1.begin(), tmp2);
    ASSERT(7 == tmp1.size());    // Verify combined size.
    ASSERT(0 == tmp2.size());    // 'tmp2' should be emptied by the splice.
//..
// Notice that, while the original lists were sorted in ascending order
// (because the data files were originally sorted), the combined list is no
// longer sorted.  To fix it, we sort 'tmp1' using the 'sort' member function:
//..
    tmp1.sort();
//..
// Then, we suggest a third, and also the best approach to combine two lists,
// which is to take advantage of the fact that the lists were originally
// sorted, using the 'merge' function:
//..
    starList1.merge(starList2);     // Merge 'starList2' into 'starList1'.
    ASSERT(7 == starList1.size());  // Verify combined size.
    ASSERT(0 == starList2.size());  // starList2 should be emptied by the
                                    // merge.
//..
// Now, since the two star surveys may overlap, we want to eliminate
// duplicates.  We accomplish this by using the 'unique' member function:
//..
    starList1.unique();             // Eliminate duplicates in 'starList1'.
    ASSERT(6 == starList1.size());  // Verify size after elimination.
//..
// Finally, we print the result:
//..
    if (verbose) {
        for (list<Star>::const_iterator i = starList1.begin();
             i != starList1.end(); ++i) {
            i->write(stdout);
        }
    }
    return 0;
}
//..
// For completeness, the implementations of the 'read', 'write', and comparison
// functions for class 'Star' are shown below:
//..
bool Star::read(FILE *input)
{
    int ret = fscanf(input, "%lf %lf %d", &d_x, &d_y, &d_brightness);
    return 3 == ret;
}

void Star::write(FILE *output) const
{
    fprintf(output, "%f %f %d\n", d_x, d_y, d_brightness);
}

bool operator==(const Star& lhs, const Star& rhs)
{
    return lhs.x() == rhs.x()
        && lhs.y() == rhs.y()
        && lhs.brightness() == rhs.brightness();
}

bool operator!=(const Star& lhs, const Star& rhs)
{
    return ! (lhs == rhs);
}

bool operator<(const Star& lhs, const Star& rhs)
{
    if (lhs.x() < rhs.x())
        return true;                                                  // RETURN
    else if (rhs.x() < lhs.x())
        return false;                                                 // RETURN
    else if (lhs.y() < rhs.y())
        return true;                                                  // RETURN
    else if (rhs.y() < lhs.y())
        return true;                                                  // RETURN
    else
        return lhs.brightness() < rhs.brightness();                   // RETURN
}
//..

// 10 data points with 4 stars at or above 75 brightness
const char STAR_DATA1[] =
    "-1.21 +0.11 76\n"
    "-1.05 +0.70 39\n"
    "-0.89 +1.03 93\n"
    "-0.61 +0.35 71\n"
    "-0.48 +1.19 61\n"
    "-0.01 +0.85 43\n"
    "+0.37 -0.01 90\n"
    "+0.37 +0.90 78\n"
    "+0.70 +0.48 54\n"
    "+0.91 +1.35 38\n";

// 8 data points with 3 stars at or above 75 brightness
// 1 point overlaps STAR_DATA1
const char STAR_DATA2[] =
    "-1.40 -0.48 74\n"
    "-0.95 -1.00 40\n"
    "-0.91 -0.21 51\n"
    "-0.51 -0.23 88\n"
    "-0.16 -0.55 30\n"
    "+0.37 -0.01 90\n"
    "+0.48 -0.57 66\n"
    "+0.93 -0.35 75\n";

//
//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters '[A .. E]' correspond to arbitrary (but unique) char
    // values to be appended to the 'bsl::list<T>' object.  A tilde ('~')
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
    typedef list<TYPE,ALLOC>  Obj;
        // Type under testing.

    typedef typename Obj::iterator                iterator;
    typedef typename Obj::const_iterator          const_iterator;
    typedef typename Obj::reverse_iterator        reverse_iterator;
    typedef typename Obj::const_reverse_iterator  const_reverse_iterator;
        // Shorthand.

    typedef typename
        bsl::is_convertible<bslma::Allocator*,ALLOC>::type ObjHasBslmaAlloc;
        // true_type if ALLOC is a bslma allocator type

    typedef typename bslma::UsesBslmaAllocator<TYPE>::type TypeHasBslmaAlloc;
        // true_type if TYPE uses a bslma allocator

    enum { SCOPED_ALLOC = ObjHasBslmaAlloc::value && TypeHasBslmaAlloc::value};
        // true if both the container shares its allocator with its contained
        // elements.

    // Unary predicate matching elements of a specified value
    struct VPred {
        const TYPE& d_match;
        explicit VPred(const TYPE& v) : d_match(v) { }
        bool operator()(const TYPE& x) const { return x == d_match; }
    };

    // Binary predicate returning true if the arguments values, expressed as
    // integers, have the same low bit value.  Thus, 'A' and 'C' compare
    // equal; 'B' and 'D' compare equal.  If an allocator is supplied to the
    // predicate constructor, then each call will cause an allocate/deallocate
    // sequence, which might throw an exception and is thus useful for
    // exception testing.
    struct LowBitEQ {
        bslma::Allocator* d_alloc;
        explicit LowBitEQ(bslma::Allocator* a = 0) : d_alloc(a) { }
        bool operator()(const TYPE& a, const TYPE& b) const {
            if (d_alloc) {
                void* p = d_alloc->allocate(1);
                d_alloc->deallocate(p);
            }
            char a1 = value_of(a);
            char b1 = value_of(b);
            return 0 == ((a1 ^ b1) & 1);
        }
    };

    // Binary predicate returning true if b < a
    class GreaterThan : private binary_function<TYPE, TYPE, bool> {
        int* d_countPtr;         // Pointer to count of times invoked
        int* d_invocationLimit;  // Number of invocations before throwing

      public:
        GreaterThan(int *count = 0) : d_countPtr(count) {
            d_invocationLimit = 0;
        }
        void setInvocationLimit(int *limit) { d_invocationLimit = limit; }
        bool operator()(const TYPE& a, const TYPE& b) {
            if (d_countPtr) ++*d_countPtr;
#ifdef BDE_BUILD_TARGET_EXC
            if (d_invocationLimit) {
                if (0 == *d_invocationLimit) throw --*d_invocationLimit;
                else if (0 < *d_invocationLimit) --*d_invocationLimit;
            }
#endif // BDE_BUILD_TARGET_EXC
            return b < a;
        }
    };

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
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static list<TYPE> gV(const char *spec);
        // Return, by value, a new list corresponding to the specified
        // 'spec'.

    static bool checkIntegrity(const Obj& object, int length);
    static bool checkIntegrity(const Obj& object, size_t length);
        // Check the integrity of the specified 'object' by verifying that
        // iterating over the list both forwards and backwards yields 'length'
        // positions and that 'object.size()' equals 'length'.  This simple
        // test should catch most instances of data structure corruption in a
        // doubly-linked-list implementation of list.

    static Int64 expectedBlocks(Int64 n);
        // Return the number of blocks expected to be used by a list of length
        // 'n'.

    static Int64 deltaBlocks(Int64 n);
        // Return the change in the number of blocks used by a list whose
        // length has changed by 'n' elements.  Note: 'n' may be negative.

    // TEST CASES
    static void testSort();
        // Test 'sort'

    static void testMerge();
        // Test 'merge'

    static void testUnique();
        // Test 'unique'

    static void testRemove();
        // Test 'remove' and 'remove_if'

    static void testSplice();
        // Test 'splice'

    static void testReverse();
        // Test 'reverse'

    static void testTypeTraits(bool uses_bslma, bool bitwise_moveable);
        // Test type traits

    static void testComparisonOps();
        // Test comparison free operators.

    static void testSwap();
        // Test 'swap' member and global swap.

    static void testErase();
        // Test 'erase' and 'pop_back'.

    static void testInsert();
        // Test 'insert' members, and move 'push_back' and 'insert' members.

    static void testEmplace();
        // Test 'emplace', 'emplace_front', and 'emplace_back' members.

    template <class CONTAINER>
    static void testInsertRange(const CONTAINER&);
        // Test 'insert' member template.

    static void testIterators();
        // Test iterators.

    static void testElementAccess();
        // Test element access.

    static void testResize();
        // Test 'resize' and 'max_size'

    static void testAssign();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testAssignRange(const CONTAINER&);
        // Test 'assign' member template.

    static void testConstructor();
        // Test user-supplied constructors.

    template <class CONTAINER>
    static void testConstructorRange(const CONTAINER&);
        // Test user-supplied constructor templates.

    static void testAllocator(const char *t, const char *a);
    static void testAllocator(bsl::false_type, const char *t, const char *a);
    static void testAllocator(bsl::true_type, const char *t, const char *a);
        // Test allocator-related concerns.  The first overload is called from
        // the main test driver.  The second overload is dispatched when
        // 'ALLOC' is not a bslma-compliant allocator.  The third overload is
        // dispatched when 'ALLOC' is a bslma-compliant allocator.  The
        // arguments 't' and 'a' are the names of the parameters 'TYPE' and
        // 'ALLOC', respectively.

    static void testAssignmentOp();
        // Test assignment operator ('operator=').

    static void testGeneratorG();
        // Test generator function 'g'.

    static void testCopyCtor();
        // Test copy constructor.

    static void testEqualityOp();
        // Test equality operator ('operator==').

    static void testBasicAccessors();
        // Test basic accessors ('size', 'begin' and 'end').

    static void testGeneratorGG();
        // Test generator functions 'ggg' and 'gg'.

    static void testPrimaryManipulators();
        // Test primary manipulators ('push_back' and 'clear').

    static void breathingTest();
        // Breathing test.  This test *exercises* basic functionality but
        // *test* nothing.
};

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::getValues(const TYPE **valuesPtr)
{
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());

    static TYPE values[8]; // avoid DEFAULT_VALUE and UNINITIALIZED_VALUE
    values[0] = TYPE(VA);
    values[1] = TYPE(VB);
    values[2] = TYPE(VC);
    values[3] = TYPE(VD);
    values[4] = TYPE(VE);
    values[5] = TYPE(VF);
    values[6] = TYPE(VG);
    values[7] = TYPE(VH);

    const int NUM_VALUES = 8;

    *valuesPtr = values;
    return NUM_VALUES;
}

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::ggg(Obj           *object,
                                const char    *spec,
                                int            verboseFlag)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    enum { SUCCESS = -1 };
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'H') {
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
list<TYPE,ALLOC>& TestDriver<TYPE,ALLOC>::gg(Obj        *object,
                                             const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class ALLOC>
list<TYPE,ALLOC> TestDriver<TYPE,ALLOC>::g(const char *spec)
{
    ALLOC AL;
    Obj object(AL);
    return gg(&object, spec);
}

template <class TYPE, class ALLOC>
list<TYPE>  TestDriver<TYPE,ALLOC>::gV(const char *spec)
{
    const TYPE *VALUES;
    getValues(&VALUES);
    list<TYPE> result;
    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'H') {
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
bool TestDriver<TYPE,ALLOC>::checkIntegrity(const Obj& object, int length)
{
    const const_iterator start  = object.begin();
    const const_iterator finish = object.end();
    const_iterator it;
    int count = 0;
    static const int MAX_SAVE_ITERS = 20;
    const_iterator save_iters[MAX_SAVE_ITERS];
    static const char DEFAULT_CVALUE = value_of(TYPE());

    // Iterate over the list.  Terminate the loop at the shorter of 'it ==
    // finish' or 'count == length'.  These should be the same, but data
    // structure corruption such as circular links or skipped nodes could make
    // them different.
    for (it = start; it != finish && count < length; ++it, ++count) {
        // Dereference the iterator and verify that the value is within the
        // expected range.
        char v = value_of(*it);
        if (v != DEFAULT_CVALUE && (v < VA || VH < v)) return false;  // RETURN
        if (count < MAX_SAVE_ITERS) {
            save_iters[count] = it;
        }
    }

    // Verify that 'count' reached 'length' at the same time that 'it' reached
    // 'finish'
    if (it != finish || count != length)
        return false;                                                 // RETURN

    // Iterate over the list in reverse.  Verify that we see the same iterator
    // values on this traversal as we did in the forward direction.
    while (it != start && count > 0) {
        --it;
        --count;
        if (count < MAX_SAVE_ITERS && it != save_iters[count])
            return false;                                             // RETURN
    }

    if (it != start || count != 0)
        return false;                                                 // RETURN

    // If got here, then the only integrity test left is to verify that size()
    // returns the actual length of the list.
    return length == (int) object.size();
}

template <class TYPE, class ALLOC>
bool TestDriver<TYPE,ALLOC>::checkIntegrity(const Obj& object, size_t length)
{
    ASSERT(length <= INT_MAX);
    return checkIntegrity(object, static_cast<int>(length));          // RETURN
}

template <class TYPE, class ALLOC>
inline
Int64 TestDriver<TYPE,ALLOC>::deltaBlocks(Int64 n)
{
    // One block per element plus one additional block per element if the
    // element uses the list's allocator ('SCOPED_ALLOC' == 1).
    return n + n * SCOPED_ALLOC;
}

template <class TYPE, class ALLOC>
inline
Int64 TestDriver<TYPE,ALLOC>::expectedBlocks(Int64 n)
{
    // One block for the sentinel node + block allocations.
    return 1 + deltaBlocks(n);
}

                                 // ----------
                                 // TEST CASES
                                 // ----------

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testSort()
{
    // --------------------------------------------------------------------
    // TESTING SORT
    //
    // Concerns:
    //   1. Sorts correctly in the presence of equivalent (duplicate) elements.
    //   2. Sorts correctly if the input is already sorted or sorted in
    //      reverse.
    //   3. No memory is allocated or deallocated during the sort.
    //   4. No constructors, destructors, or assignment of elements takes
    //      place.
    //   5. Iterators to all elements remain valid.
    //   6. The predicate version of 'sort' can be used to sort using a
    //      different comparison criterion.
    //   7. The non-predicate version of 'sort' does not use 'std::less'.
    //   8. The sort is stable -- equivalent elements remain in the same order
    //      as in the original list.
    //   9. The number of calls to the comparison operation is no larger than
    //      'N*log2(N)', where 'N' is the number of elements.
    //   10. If the comparison function throws an exception, no memory is
    //       leaked.  (The order of the elements is indeterminate.)
    //
    // Test plan:
    //   Create a series of list specifications of different lengths, some
    //   containing duplicates, triplicates, and multiple sets of duplicates
    //   and triplicates.  Generate every permutation of elements within each
    //   specification.  Create a list from the permutation, and store an
    //   iterator to each list element.  Sort the list.  Verify that the
    //   resultant list is a sorted version of the original.  Verify that
    //   iterating over each element in the sorted list results in an iterator
    //   that existed in the original list and that, for equivalent elements,
    //   the iterators appear in the same order.  Test allocations,
    //   constructor counts, destructor counts, and assignment counts before
    //   and after the sort and verify that they haven't changed.
    //   (Constructor, destructor, and assignment counts are meaningful only
    //   if 'TYPE' is 'TestType', but will are accessible and will remain
    //   unchanged anyway for other types.)  To address concern 7,
    //   std::less<TestType> is specialized to detect being called
    //   inappropriately.  To address concern 6, repeat the test using a
    //   predicate that sorts in reverse order.  To address concern 9, the
    //   predicate counts the number of invocations.  To address concern 10,
    //   the predicate operator is instrumented to throw an exception after a
    //   specific number of iterations.  Using a sample string, set the
    //   comparison operator to throw at different counts and verify, after
    //   each exception, that no memory is leaked, that the list is valid, and
    //   that every element in the list is represented by a saved iterator.
    //
    // Testing:
    //   void sort();
    //   template <class COMP> void sort(COMP c);
    // --------------------------------------------------------------------

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) VALUES;
    (void) NUM_VALUES;

    bslma::TestAllocator testAllocator;
    ALLOC Z(&testAllocator);

    const int MAX_SPEC_LEN = 10;

    // NOTE: The elements within each of these specifications must be sorted
    // so that 'next_permutation' can do the right thing.  Since we will be
    // testing every permutation, there is no worry about having the elements
    // int too predicatable an order.
    const char *const SPECS[] = {
        // Length 0 or 1: 1 permutation each
        "",
        "A",
        // Length 2: max 2 permutations each
        "AA", "AB",
        // Length 3: max 3! = 6 permutations each
        "AAA", "AAB", "ABB", "ABC",
        // Length 4: max 4! = 24 permutations each
        "AAAA", "AAAB", "AABB", "ABBB", "AABC", "ABBC", "ABCC", "ABCD",
        // Length 5: max 5! = 120 permutations each
        "AAAAA", "AAAAB", "AAABB", "AABBB", "ABBBB",
        "AAABC", "AABBC", "AABCC", "ABBBC", "ABBCC", "ABCCC", "ABCDE",
        // Length 8: max 8! = 40320 permutations each
        "ABCDEFGH", "AABCDEFG", "ABCDEFGG", "AABCDEFF", "ABCDDEFG",
        "AABCCDEE", "AAABBCDE",
        // Length 10, with no more than 8 unique elements:
        // max 10!/2!2! = 907200 permutations
//        "AABCDEFFGH"
    };

    const int NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]);

    // Log2 of integers from 0 to 16, rounded up.
    // (Log2(0) is undefined, but 0 works for our purposes.)
    const int LOG2[] = { 0, 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

    if (verbose) printf("\nTesting sort()\n");

    for (int i = 0; i < NUM_SPECS; ++i) {
        const char* const S_SPEC = SPECS[i];  // Sorted spec.
        const int LENGTH = static_cast<int>(strlen(S_SPEC));
        ASSERT(MAX_SPEC_LEN >= LENGTH);

        char spec[MAX_SPEC_LEN + 1];
        strcpy(spec, S_SPEC);

        // Expected result
        Obj mExp;   const Obj& EXP = gg(&mExp, S_SPEC);

        // Test each permutation
        do {
            const char *const SPEC = spec;
            if (veryVeryVerbose) P(SPEC);

            Obj mX(Z);  const Obj& X = gg(&mX, SPEC);

            const_iterator save_iters[MAX_SPEC_LEN + 1];
            const_iterator xi = X.begin();
            for (int j = 0; j < LENGTH; ++j, ++xi) {
                save_iters[j] = xi;
            }
            save_iters[LENGTH] = xi;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numCharCtorCalls    +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            mX.sort();

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numCharCtorCalls    +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            LOOP_ASSERT(SPEC, checkIntegrity(X, LENGTH));
            LOOP_ASSERT(SPEC, X            == EXP);
            LOOP_ASSERT(SPEC, AA           == BB);
            LOOP_ASSERT(SPEC, A            == B);
            LOOP_ASSERT(SPEC, CTORS_AFTER  == CTORS_BEFORE);
            LOOP_ASSERT(SPEC, DTORS_AFTER  == DTORS_BEFORE);
            LOOP_ASSERT(SPEC, ASSIGN_AFTER == ASSIGN_BEFORE);

            xi = X.begin();
            for (int j = 0; j < LENGTH; ++j) {

                // Find index of iterator in saved iterator array
                const_iterator* p = find(save_iters, save_iters + LENGTH, xi);
                long save_idx = p - save_iters;
                LOOP2_ASSERT(SPEC, j, LENGTH >= save_idx);

                // Verify stable sort.  Iterate through equivalent values and
                // verify that the sorted list produces iterators in the same
                // order as in the saved iterator array.  As each iterator is
                // matched, it is removed from 'save_iters' so as to ensure
                // that no iterator appears more than once (which would
                // represent a serious data structure corruption).
                char val = SPEC[save_idx];
                for (long k = save_idx; SPEC[k] == val; ++k, ++xi, ++j) {
                    LOOP2_ASSERT(SPEC, k, xi == save_iters[k]);
                    save_iters[k] = X.end();  // Avoid matching iterator twice
                } // end for k
            } // end for j
        } while (next_permutation(spec, spec + LENGTH));
    } // end for i

    if (verbose) printf("\nTesting template<COMP> sort(COMP)\n");

    for (int i = 0; i < NUM_SPECS; ++i) {
        const int LENGTH = static_cast<int>(strlen(SPECS[i]));
        ASSERT(MAX_SPEC_LEN >= LENGTH);

        // Copy SPECS[i] in reverse order
        char s_spec[MAX_SPEC_LEN + 1];
        for (int j = 0; j < LENGTH; ++j) {
            s_spec[j] = SPECS[i][LENGTH - j - 1];
        }
        s_spec[LENGTH] = '\0';
        const char* const S_SPEC = s_spec;    // (reverse) sorted spec.

        char spec[MAX_SPEC_LEN + 1];
        strcpy(spec, S_SPEC);

        // Expected result
        Obj mExp;   const Obj& EXP = gg(&mExp, S_SPEC);

        // Test each permutation
        do {
            const char *const SPEC = spec;

            Obj mX(Z);  const Obj& X = gg(&mX, SPEC);

            const_iterator save_iters[MAX_SPEC_LEN + 1];
            const_iterator xi = X.begin();
            for (int j = 0; j < LENGTH; ++j, ++xi) {
                save_iters[j] = xi;
            }
            save_iters[LENGTH] = xi;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numCharCtorCalls    +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            int predicateCalls = 0;  // Count of calls to predicate

            if (veryVeryVeryVerbose) { printf("\tBefore: "); P_(X); }

            mX.sort(GreaterThan(&predicateCalls));

            if (veryVeryVeryVerbose) { printf("After: "); P(X); }

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numCharCtorCalls    +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            LOOP_ASSERT(SPEC, checkIntegrity(X, LENGTH));
            LOOP_ASSERT(SPEC, X            == EXP);
            LOOP_ASSERT(SPEC, AA           == BB);
            LOOP_ASSERT(SPEC, A            == B);
            LOOP_ASSERT(SPEC, CTORS_AFTER  == CTORS_BEFORE);
            LOOP_ASSERT(SPEC, DTORS_AFTER  == DTORS_BEFORE);
            LOOP_ASSERT(SPEC, ASSIGN_AFTER == ASSIGN_BEFORE);

            // Verify complexity requirement.
            LOOP_ASSERT(SPEC, predicateCalls <= LENGTH * LOG2[LENGTH]);
            LOOP_ASSERT(SPEC, predicateCalls >= LENGTH - 1);

            xi = X.begin();
            for (int j = 0; j < LENGTH; ++j) {

                // Find index of iterator in saved iterator array
                const_iterator* p = find(save_iters, save_iters + LENGTH, xi);
                ptrdiff_t save_idx = p - save_iters;
                LOOP2_ASSERT(SPEC, j, LENGTH >= save_idx);

                // Verify stable sort.  Iterate through equivalent values and
                // verify that the sorted list produces iterators in the same
                // order as in the saved iterator array.  As each iterator is
                // matched, it is removed from 'save_iters' so as to ensure
                // that no iterator appears more than once (which would
                // represent a serious data structure corruption).
                char val = SPEC[save_idx];
                for (long k = save_idx; SPEC[k] == val; ++k, ++xi, ++j) {
                    LOOP2_ASSERT(SPEC, k, xi == save_iters[k]);
                    save_iters[k] = X.end();  // Avoid matching iterator twice
                } // end for k
            } // end for j
        } while (next_permutation(spec, spec + LENGTH, std::greater<char>()));
    } // end for i

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nTesting exception safety\n");

    // Choose a longish string of random values
    const char EH_SPEC[] = "CBHADBAGCBFFADHE";
    enum {
        EH_SPEC_LEN = sizeof(EH_SPEC) - 1
    };

    bool caught_ex = true;
    for (int threshold = 0; caught_ex; ++threshold) {
        caught_ex = false;

        Obj mX(Z);  const Obj& X = gg(&mX, EH_SPEC);

        const_iterator save_iters[EH_SPEC_LEN + 1];
        const_iterator xi = X.begin();
        for (int j = 0; j < EH_SPEC_LEN; ++j, ++xi) {
            save_iters[j] = xi;
        }
        save_iters[EH_SPEC_LEN] = xi;

        const Int64 BB = testAllocator.numBlocksTotal();
        const Int64 B  = testAllocator.numBlocksInUse();
        const int CTORS_BEFORE = (numDefaultCtorCalls +
                                  numCharCtorCalls    +
                                  numCopyCtorCalls);
        const int ASSIGN_BEFORE = numAssignmentCalls;
        const int DTORS_BEFORE  = numDestructorCalls;

        GreaterThan gt;  // Create a predicate object
        int limit = threshold;
        gt.setInvocationLimit(&limit);
        try {
            mX.sort(gt);
        }
        catch (int e) {
            LOOP_ASSERT(threshold, -1 == e);
            caught_ex = true;
        }
        catch (...) {
            LOOP_ASSERT(threshold, !"Caught unexpected exception");
            caught_ex = true;
        }

        if (veryVeryVeryVerbose) {
            T_; P_(threshold); P_(caught_ex);
            printf("Result: "); P(X);
        }

        const Int64 AA = testAllocator.numBlocksTotal();
        const Int64 A  = testAllocator.numBlocksInUse();
        const int CTORS_AFTER = (numDefaultCtorCalls +
                                 numCharCtorCalls    +
                                 numCopyCtorCalls);
        const int ASSIGN_AFTER = numAssignmentCalls;
        const int DTORS_AFTER  = numDestructorCalls;

        if (caught_ex) {
            // Should not call predicate more than N*Log2(N) times.
            LOOP_ASSERT(threshold,
                        threshold < EH_SPEC_LEN * LOG2[EH_SPEC_LEN]);
        }
        else {
            // Must have called predicate successfully at least N-1 times.
            LOOP_ASSERT(threshold, threshold >= EH_SPEC_LEN -1);
        }

        LOOP_ASSERT(threshold, (int) X.size() == EH_SPEC_LEN);
        LOOP_ASSERT(threshold, checkIntegrity(X, X.size()));

        LOOP_ASSERT(threshold, AA           == BB);
        LOOP_ASSERT(threshold, CTORS_AFTER  == CTORS_BEFORE);
        LOOP_ASSERT(threshold, ASSIGN_AFTER == ASSIGN_BEFORE);

        if ((int) X.size() == EH_SPEC_LEN) {
            // To avoid cascade errors The following tests are skipped if the
            // length changed.  Otherwise they would all fail, giving no
            // additional information.

            LOOP_ASSERT(threshold, A            == B);
            LOOP_ASSERT(threshold, DTORS_AFTER  == DTORS_BEFORE);
        }

        // Verify that all iterators in list were already in the list before
        // the sort (and before the exception).  The order of elements is
        // unspecified in the case of an exception, and is thus not tested.
        for (xi = X.begin(); xi != X.end(); ++xi) {
            // Find index of iterator in saved iterator array
            const_iterator* p = find(save_iters, save_iters + EH_SPEC_LEN, xi);
            ptrdiff_t save_idx = p - save_iters;
            const char VAL = EH_SPEC[save_idx];

            LOOP_ASSERT(threshold, save_idx < EH_SPEC_LEN);
            if (save_idx < EH_SPEC_LEN) {
                LOOP_ASSERT(threshold, value_of(*xi) == VAL);
            }
        } // End for (xi)
    } // End for (threshold)
#endif // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testMerge()
{
    // --------------------------------------------------------------------
    // TESTING MERGE
    //
    // Concerns:
    //   1. Merging produces correct results with and without duplicate
    //      elements within and between the lists to be merged.
    //   2. The argument to merge is empty after the merge.
    //   3. No memory is allocated or deallocated during the merge.
    //   4. No constructors, destructors, or assignment of elements takes
    //      place.
    //   5. Iterators to all elements remain valid.
    //   6. The predicate version of 'merge' can be used to merge using a
    //      different comparison criterion.
    //   7. The non-predicate version of 'merge' does not use 'std::less'.
    //   8. Merging a list with itself has no effect.
    //   9. If the comparison function throws an exception, no memory is
    //      leaked and all elements remain in one list or the other.
    //
    // Test plan:
    //   Create two lists from the cross-product of two small sets of
    //   specifications.  The elements in the lists are chosen so that every
    //   combination of duplicate and non-duplicate elements, both within and
    //   between lists, is represented.  Save the iterators to all elements of
    //   both lists and record the memory usage before the merge.  Merge one
    //   list into the other.  Verify that the merged value is correct, that
    //   all of the pre-merge iterators are still valid, and that the
    //   non-merged list is now empty.  To address concern 6, sort the initial
    //   specifications using the reverse sort order, then use a custom
    //   "greater-than" predicate to merge the lists and verify the same
    //   values as for the non-predicate case.  To address concern 7,
    //   std::less<TestType> is specialized to detect being called
    //   inappropriately.  To address concern 8, merge each list with itself
    //   and verify that no memory is allocated or deallocated and that all
    //   iterators remain valid.
    //
    // Testing:
    //   void merge(list& other);
    //   template <class COMP> void merge(list& other, COMP c);
    // --------------------------------------------------------------------

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    bslma::TestAllocator testAllocator;
    ALLOC Z(&testAllocator);

    class SortedSpecGen
    {
        // Generate every possible specification up to 5 elements long such
        // that no element has a value less than the previous element.  Using
        // 6 of the possible 8 values for each element, there are a total of
        // 462 combinations.
      public:
        enum { MAX_SPEC_LEN = 5 };
      private:
        int  d_len;
        char d_spec[MAX_SPEC_LEN + 1];
        mutable char d_reverse_spec[MAX_SPEC_LEN + 1];

        enum { MAX_ELEMENT = 'F' };

      public:
        SortedSpecGen() : d_len(0) { d_spec[0] = '\0'; }

        // Return true if this object holds a valid spec
        operator bool() const { return d_len <= MAX_SPEC_LEN; }

        // Advance to the next specification
        SortedSpecGen& operator++() {

            // Find the last element with value < MAX_ELEMENT.  Note that
            // with 'MAX_ELEMENT' set to 'F', we are using only 6 of the
            // possible 8 values for each element yielding a total of 462
            // combinations.  For more combinations (and a slower test),
            // extend 'MAX_ELEMENT' to 'G' or (max) 'H'.
            char max_elem_str[2] = { MAX_ELEMENT, 0 };
            int i = static_cast<int>(strcspn(d_spec, max_elem_str)) - 1;

            // If nothing was found, then 'd_spec' is all MAX_ELEMENTs.
            // Increment length and start over with all 'A's.
            if (i < 0) {
                ++d_len;
                if (MAX_SPEC_LEN < d_len) return *this;               // RETURN
                memset(d_spec, 'A', d_len);
                d_spec[d_len] = '\0';
                return *this;                                         // RETURN
            }

            // d_spec[i] < MAX_ELEMENT.  Increment the element at 'i' and fill
            // the remainder of the spec with the same value.
            char x = static_cast<char>(d_spec[i] + 1);
            memset(d_spec + i, x, d_len - i);
            return *this;
        }

        int len() const { return d_len; }
        const char* spec() const { return d_spec; }
        const char* reverse_spec() const {
            for (int i = 0; i < d_len; ++i) {
                d_reverse_spec[d_len - i - 1] = d_spec[i];
            }
            d_reverse_spec[d_len] = '\0';
            return d_reverse_spec;
        }
    };  // End class SortedSpecGen

    const int MAX_SPEC_LEN = SortedSpecGen::MAX_SPEC_LEN;

    if (verbose) printf("\nTesting void merge(list& other);\n");

    for (SortedSpecGen xgen; xgen; ++xgen) {
        for (SortedSpecGen ygen; ygen; ++ygen) {

            const char* const X_SPEC     = xgen.spec();
            const int         X_SPEC_LEN = xgen.len();
            const char* const Y_SPEC     = ygen.spec();
            const int         Y_SPEC_LEN = ygen.len();

            Obj mX(Z); const Obj& X = gg(&mX, X_SPEC);
            Obj mY(Z); const Obj& Y = gg(&mY, Y_SPEC);

            const_iterator xiters[MAX_SPEC_LEN + 1];
            const_iterator yiters[MAX_SPEC_LEN + 1];

            // Save the iterators before merge
            int xi = 0;
            for (const_iterator it = X.begin(); it != X.end(); ++it, ++xi) {
                xiters[xi] = it;
            }
            xiters[xi] = X.end();
            int yi = 0;
            for (const_iterator it = Y.begin(); it != Y.end(); ++it, ++yi) {
                yiters[yi] = it;
            }
            yiters[yi] = Y.end();

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numCharCtorCalls    +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            // Self merge (noop)
            mX.merge(mX);
            LOOP2_ASSERT(X_SPEC, Y_SPEC, (int) X.size() == X_SPEC_LEN);
            LOOP2_ASSERT(X_SPEC, Y_SPEC, (int) Y.size() == Y_SPEC_LEN);

            if (veryVeryVerbose) {
                T_; printf("Before: "); P_(X); P_(Y);
            }

            mX.merge(mY); // Test merge here

            if (veryVeryVerbose) {
                T_; printf("After: "); P_(X); P(Y);
            }

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numCharCtorCalls    +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            // Test result size
            LOOP2_ASSERT(X_SPEC, Y_SPEC,
                         (int) X.size() == X_SPEC_LEN + Y_SPEC_LEN);
            LOOP2_ASSERT(X_SPEC, Y_SPEC, Y.size() == 0);

            // Test merged results and iterators
            int idx = 0;
            xi = yi = 0;
            for (const_iterator it = X.begin(); it != X.end(); ++it, ++idx)
            {
                if (it == xiters[xi]) {
                    if (yi < Y_SPEC_LEN) {
                        // Verify that merge criterion was met
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     ! (Y_SPEC[yi] < X_SPEC[xi]));
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     VALUES[X_SPEC[xi] - 'A'] == *it)
                    }
                    ++xi;
                }
                else if (it == yiters[yi]) {
                    if (xi < X_SPEC_LEN) {
                        // Verify that merge criterion was met.
                        // C++98 required that items from X precede equivalent
                        // items from Y.  C++0x seemed to remove this
                        // requirement, but we should adhere to it anyway.
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     Y_SPEC[yi] < X_SPEC[xi]);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     VALUES[Y_SPEC[yi] - 'A'] == *it)
                    }
                    ++yi;
                }
                else {
                    // A stable merge requires that the iterator must match
                    // the next iterator on the save x or y list.
                    LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, !"Invalid merge");
                }
            }
            // Test end iterators
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, xiters[xi] == X.end());
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, yiters[yi] == Y.end());
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, (xi + yi) == (int) X.size());

            // Test allocations and deallocations
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, AA == BB);
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,  A ==  B);

            // Test that no constructors, destructors or assignments were
            // called.
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, CTORS_AFTER  == CTORS_BEFORE);
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, DTORS_AFTER  == DTORS_BEFORE);
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, ASSIGN_AFTER ==ASSIGN_BEFORE);
        } // end for (ygen)
    } // end for (xgen)

    if (verbose) printf("\nTesting void merge(list& other, COMP c);\n");

    for (SortedSpecGen xgen; xgen; ++xgen) {
        for (SortedSpecGen ygen; ygen; ++ygen) {

            const char* const X_SPEC     = xgen.reverse_spec();
            const int         X_SPEC_LEN = xgen.len();
            const char* const Y_SPEC     = ygen.reverse_spec();
            const int         Y_SPEC_LEN = ygen.len();

            Obj mX(Z); const Obj& X = gg(&mX, X_SPEC);
            Obj mY(Z); const Obj& Y = gg(&mY, Y_SPEC);

            const_iterator xiters[MAX_SPEC_LEN + 1];
            const_iterator yiters[MAX_SPEC_LEN + 1];

            // Save the iterators before merge
            int xi = 0;
            for (const_iterator it = X.begin(); it != X.end(); ++it, ++xi) {
                xiters[xi] = it;
            }
            xiters[xi] = X.end();
            int yi = 0;
            for (const_iterator it = Y.begin(); it != Y.end(); ++it, ++yi) {
                yiters[yi] = it;
            }
            yiters[yi] = Y.end();

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numCharCtorCalls    +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            // Self merge (noop)
            mX.merge(mX, GreaterThan());
            LOOP2_ASSERT(X_SPEC, Y_SPEC, (int) X.size() == X_SPEC_LEN);
            LOOP2_ASSERT(X_SPEC, Y_SPEC, (int) Y.size() == Y_SPEC_LEN);

            if (veryVeryVerbose) {
                T_; printf("Before: "); P_(X); P_(Y);
            }

            mX.merge(mY, GreaterThan()); // Test merge here

            if (veryVeryVerbose) {
                T_; printf("After: "); P_(X); P(Y);
            }

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numCharCtorCalls    +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            // Test result size
            LOOP2_ASSERT(X_SPEC, Y_SPEC,
                         (int) X.size() == X_SPEC_LEN + Y_SPEC_LEN);
            LOOP2_ASSERT(X_SPEC, Y_SPEC, Y.size() == 0);

            // Test merged results and iterators
            int idx = 0;
            xi = yi = 0;
            for (const_iterator it = X.begin(); it != X.end(); ++it, ++idx)
            {
                if (it == xiters[xi]) {
                    if (yi < Y_SPEC_LEN) {
                        // Verify that merge criterion was met
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     ! (Y_SPEC[yi] > X_SPEC[xi]));
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     VALUES[X_SPEC[xi] - 'A'] == *it)
                    }
                    ++xi;
                }
                else if (it == yiters[yi]) {
                    if (xi < X_SPEC_LEN) {
                        // Verify that merge criterion was met.
                        // C++98 required that items from X precede equivalent
                        // items from Y.  C++0x seemed to remove this
                        // requirement, but we should adhere to it anyway.
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     Y_SPEC[yi] > X_SPEC[xi]);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,
                                     VALUES[Y_SPEC[yi] - 'A'] == *it)
                    }
                    ++yi;
                }
                else {
                    // A stable merge requires that the iterator must match
                    // the next iterator on the save x or y list.
                    LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, !"Invalid merge");
                }
            }
            // Test end iterators
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, xiters[xi] == X.end());
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, yiters[yi] == Y.end());
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, (xi + yi) == (int) X.size());

            // Test allocations and deallocations
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, AA == BB);
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi,  A ==  B);

            // Test that no constructors, destructors or assignments were
            // called.
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, CTORS_AFTER  == CTORS_BEFORE);
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, DTORS_AFTER  == DTORS_BEFORE);
            LOOP4_ASSERT(X_SPEC, Y_SPEC, xi, yi, ASSIGN_AFTER ==ASSIGN_BEFORE);
        } // end for (ygen)
    } // end for (xgen)

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nTesting exception safety\n");
    {
        const char X_SPEC[] = "HGFEDCBA";
        const char Y_SPEC[] = "GGEECCBA";
        enum {
            X_SPEC_LEN = sizeof(X_SPEC) - 1,
            Y_SPEC_LEN = sizeof(Y_SPEC) - 1,
            MERGED_SPEC_LEN = X_SPEC_LEN + Y_SPEC_LEN
        };

        bool caught_ex = true;
        for (int threshold = 0; caught_ex; ++threshold) {
            caught_ex = false;

            Obj mX(Z);  const Obj& X = gg(&mX, X_SPEC);
            Obj mY(Z);  const Obj& Y = gg(&mY, Y_SPEC);

            const_iterator save_iters[MERGED_SPEC_LEN + 1];
            int j = 0;
            for (const_iterator xi = X.begin(); xi != X.end(); ++xi, ++j) {
                save_iters[j] = xi;
            }
            for (const_iterator yi = Y.begin(); yi != Y.end(); ++yi, ++j) {
                save_iters[j] = yi;
            }
            save_iters[MERGED_SPEC_LEN] = Y.end();
            LOOP_ASSERT(threshold, MERGED_SPEC_LEN == j);

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();
            const int CTORS_BEFORE = (numDefaultCtorCalls +
                                      numCharCtorCalls    +
                                      numCopyCtorCalls);
            const int ASSIGN_BEFORE = numAssignmentCalls;
            const int DTORS_BEFORE  = numDestructorCalls;

            GreaterThan gt;  // Create a predicate object
            int limit = threshold;
            gt.setInvocationLimit(&limit);
            try {
                mX.merge(mY, gt);
            }
            catch (int e) {
                LOOP_ASSERT(threshold, -1 == e);
                caught_ex = true;
            }
            catch (...) {
                LOOP_ASSERT(threshold, !"Caught unexpected exception");
                caught_ex = true;
            }

            if (veryVeryVeryVerbose) {
                T_; P_(threshold); P_(caught_ex);
                printf("Result: "); P_(X); P(Y);
            }

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();
            const int CTORS_AFTER = (numDefaultCtorCalls +
                                     numCharCtorCalls    +
                                     numCopyCtorCalls);
            const int ASSIGN_AFTER = numAssignmentCalls;
            const int DTORS_AFTER  = numDestructorCalls;

            LOOP_ASSERT(threshold,
                        (int)(X.size() + Y.size()) == MERGED_SPEC_LEN);
            LOOP_ASSERT(threshold, checkIntegrity(X, X.size()));
            LOOP_ASSERT(threshold, checkIntegrity(Y, Y.size()));

            LOOP_ASSERT(threshold, AA           == BB);
            LOOP_ASSERT(threshold, ASSIGN_AFTER == ASSIGN_BEFORE);
            LOOP_ASSERT(threshold, CTORS_AFTER  == CTORS_BEFORE);

            if ((int) (X.size() + Y.size()) == MERGED_SPEC_LEN) {
                // To avoid cascade errors The following tests are skipped if
                // the total length changed.  Otherwise they would all fail,
                // giving no additional information.
                LOOP_ASSERT(threshold, A            == B);
                LOOP_ASSERT(threshold, DTORS_AFTER  == DTORS_BEFORE);
            }

            // Verify that all iterators in the lists were already in the
            // lists before the merge (and before the exception).  The order
            // of elements is unspecified in the case of an exception, and is
            // thus not tested.
            char prev_val = 'Z';
            for (const_iterator xi = X.begin(); xi != X.end(); ++xi) {
                // Find index of iterator in saved iterator array
                const_iterator* p = find(save_iters,
                                         save_iters + MERGED_SPEC_LEN, xi);
                ptrdiff_t save_idx = p - save_iters;
                const char VAL = (save_idx < X_SPEC_LEN) ?
                                  X_SPEC[save_idx] :
                                  Y_SPEC[save_idx - X_SPEC_LEN];

                LOOP_ASSERT(threshold, save_idx < MERGED_SPEC_LEN);
                if (save_idx < MERGED_SPEC_LEN) {
                    LOOP_ASSERT(threshold, value_of(*xi) == VAL);
                    // Verify that the values are still in descending value,
                    // even though the standard does not seem to require it.
                    LOOP_ASSERT(threshold, VAL <= prev_val);
                }
                save_iters[save_idx] = Y.end(); // Prevent duplicate matches
            } // End for (xi)
            prev_val = 'Z';
            for (const_iterator yi = Y.begin(); yi != Y.end(); ++yi) {
                // Find index of iterator in saved iterator array
                const_iterator* p = find(save_iters,
                                         save_iters + MERGED_SPEC_LEN, yi);
                ptrdiff_t save_idx = p - save_iters;
                const char VAL = (save_idx < X_SPEC_LEN) ?
                                  X_SPEC[save_idx] :
                                  Y_SPEC[save_idx - X_SPEC_LEN];

                LOOP_ASSERT(threshold, save_idx < MERGED_SPEC_LEN);
                if (save_idx < MERGED_SPEC_LEN) {
                    LOOP_ASSERT(threshold, value_of(*yi) == VAL);
                    // Verify that the values are still in descending value,
                    // even though the standard does not seem to require it.
                    LOOP_ASSERT(threshold, VAL <= prev_val);
                }
                save_iters[save_idx] = Y.end(); // Prevent duplicate matches
            } // End for (yi)
        } // End for (threshold)
    }
#endif // BDE_BUILD_TARGET_EXC
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testUnique()
{
    // --------------------------------------------------------------------
    // TESTING UNIQUE
    //
    // Concerns:
    //   1. The predicate and non-predicate versions of 'unique' have
    //      essentially the same characteristics.
    //   2. Can remove elements from any or all positions in the list except
    //      the first.
    //   3. Destructors are called for removed elements and memory is deleted
    //      for removed elements.
    //   4. No constructors, destructors, or assignment operators are called
    //      on the remaining (non-removed) elements.
    //   5. No memory is allocated.
    //   6. Iterators to non-removed elements, including the 'end()' iterator,
    //      remain valid after removal.
    //   7. The non-removed elements retain their relative order.
    //   8. The 'unique' operation is exception-neutral, if the equality
    //      operator or predicate throw an exception.
    //   9. The non-predicate version calls operator==(T,T) directly; it does
    //      not call std::equal_to<T>::operator()(T,T).
    //
    // Plan:
    //   For concern 1, perform the same tests for both the predicate and
    //   non-predicate versions of 'unique.  Generate lists of various lengths
    //   up to 10 elements, filling the lists with different sequences of
    //   values such that every combination of matching and non-matching
    //   subsequences is generated.  (For the predicate version, matching
    //   elements need to be equal only in their low bit).  For each
    //   combination, make a copy of all of the iterators to non-repeated
    //   elements, then call 'unique'.  Validate that: The number of new
    //   destructor calls matches the number of elements removed, reduction of
    //   memory blocks in use is correct for the number elements removed, the
    //   number of new allocations is zero, the number of new constructor
    //   calls is zero, and the iterating over the remaining elements produces
    //   a sequence of values and iterators matching those saved before the
    //   'unique' operation.  For concern 8, perform the tests in an
    //   exception-testing framework, using a special feature of the
    //   'LowBitEQ' predicate to cause exceptions to be thrown.  For concern
    //   9, std::equal_to<TestType> is specialized to detect being called
    //   inappropriately.
    //
    // Testing:
    //   void unique();
    //   template <class BINPRED> void unique(BINPRED p);
    // --------------------------------------------------------------------

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    // For this test, it is important that 'NUM_VALUES' be even.
    // If 'getValues' returns an odd number, ignore the last value.
    const int           NUM_VALUES = getValues(&values) & 0xfffe;

    bslma::TestAllocator testAllocator; // For exception testing only
    bslma::TestAllocator objAllocator;  // For object allocation testing
    ALLOC Z(&objAllocator);

    const int LENGTHS[] = { 0, 1, 2, 3, 4, 5, 10 };
    const int NUM_LENGTHS = sizeof(LENGTHS) / sizeof(LENGTHS[0]);
    const int MAX_LENGTH = 10;

    enum {
        OP_FIRST,
        OP_UNIQUE = OP_FIRST, // void unique();
        OP_UNIQUE_PRED,       // template <class BINPRED> void unique(BINPRED);
        OP_LAST
    };

    for (int op = OP_FIRST; op < OP_LAST; ++op) {

        // The 'perturb_bit' is a bit mask that can be perturbed in a value and
        // still compare equal to the original according to the predicate.
        char perturb_bit;

        switch (op) {
          case OP_UNIQUE:
            if (verbose) printf("\nTesting unique()\n");
            perturb_bit = 0;
            break;
          case OP_UNIQUE_PRED:
            if (verbose) printf("\nTesting unique(BINPRED p)\n");
            perturb_bit = 2;
            break;
        }

        for (int i = 0; i < NUM_LENGTHS; ++i) {
            const int LEN  = LENGTHS[i];

            ASSERT(MAX_LENGTH >= LEN);

            // 'mask' contains a bit for each element in the list.  For each
            // '1' bit, the element should match the preceding element's value
            // according to the predicate.  Bit 0 (the first position) is
            // skipped, since it has no preceding value.
            for (unsigned mask = 0; mask < (unsigned) (1 << LEN); mask += 2) {

              BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                const_iterator save_iters[MAX_LENGTH + 1];
                int res_len = 0;  // To compute expected result length
                Obj mX(Z);    const Obj& X = mX;            // test objected
                Obj res_exp;  const Obj& RES_EXP = res_exp; // expected result

                int val_idx  = 0;
                if (LEN > 0) {
                    mX.push_back(VALUES[0]);
                    res_exp.push_back(VALUES[0]);
                    ++res_len;
                    save_iters[0] = X.begin();
                }
                for (unsigned bit = 2; bit < (unsigned)(1 << LEN); bit <<= 1) {
                    if ((mask & bit)) {
                        // Set the new value to the previous value, but
                        // (possibly) perturbed in such a way that it they
                        // still compare equal according to the predicate.
                        val_idx = val_idx ^ perturb_bit;
                        mX.push_back(VALUES[val_idx]);
                    }
                    else {
                        // Increment val_idx, modulo NUM_VALUES
                        val_idx = (val_idx + 1) % NUM_VALUES;
                        mX.push_back(VALUES[val_idx]);
                        res_exp.push_back(VALUES[val_idx]);

                        // Save iterators to non-repeated elements
                        save_iters[res_len++] = --X.end();
                    }
                }
                LOOP3_ASSERT(op, X, RES_EXP, (int) X.size() == LEN);
                LOOP3_ASSERT(op, X, RES_EXP, (int) RES_EXP.size() == res_len);
                save_iters[res_len] = X.end();

                const Int64 BB = objAllocator.numBlocksTotal();
                const Int64 B  = objAllocator.numBlocksInUse();
                const int CTORS_BEFORE = (numDefaultCtorCalls +
                                          numCharCtorCalls    +
                                          numCopyCtorCalls);
                const int ASSIGN_BEFORE = numAssignmentCalls;
                const int DTORS_BEFORE  = numDestructorCalls;

                if (veryVeryVerbose) { T_; printf("Before: "); P_(X); }

                switch (op) {
                  case OP_UNIQUE:
                    mX.unique();
                    break;
                  case OP_UNIQUE_PRED: {
                    mX.unique(LowBitEQ(&testAllocator));
                  } break;
                }

                if (veryVeryVerbose) { printf("After: "); P(X); }

                const Int64 AA = objAllocator.numBlocksTotal();
                const Int64 A  = objAllocator.numBlocksInUse();
                const int CTORS_AFTER = (numDefaultCtorCalls +
                                         numCharCtorCalls    +
                                         numCopyCtorCalls);
                const int ASSIGN_AFTER = numAssignmentCalls;
                const int DTORS_AFTER  = numDestructorCalls;

                // Test result value
                LOOP3_ASSERT(op, X, RES_EXP, checkIntegrity(X, res_len));
                LOOP3_ASSERT(op, X, RES_EXP, (int) X.size() == res_len);
                LOOP3_ASSERT(op, X, RES_EXP, X == RES_EXP);

                // Test that iterators are still valid
                int idx = 0;
                for (const_iterator it = X.begin(); it != X.end(); ++it, ++idx)
                {
                    LOOP4_ASSERT(op, X, RES_EXP, idx, save_iters[idx] == it);
                }
                // Test end iterator
                LOOP4_ASSERT(op, X, RES_EXP, idx, save_iters[idx] == X.end());

                // Test allocations and deallocations
                LOOP3_ASSERT(op, X, RES_EXP, AA == BB);
                LOOP3_ASSERT(op, X, RES_EXP,
                             deltaBlocks(LEN - res_len) == B - A);

                // If 'TYPE' is 'TestType', then test that no constructors or
                // assignments were called and the expected number of
                // destructors were called.
                if (bsl::is_same<TYPE, TestType>::value) {
                    LOOP3_ASSERT(op, X, RES_EXP, CTORS_AFTER  == CTORS_BEFORE);
                    LOOP3_ASSERT(op, X, RES_EXP,
                                 ASSIGN_AFTER == ASSIGN_BEFORE);
                    LOOP3_ASSERT(op, X, RES_EXP,
                                 DTORS_AFTER == DTORS_BEFORE + (LEN-res_len));
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // end for (mask)
        } // end for (i)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testRemove()
{
    // --------------------------------------------------------------------
    // TESTING REMOVE
    //
    // Concerns:
    //   1. 'remove' and 'remove_if' have essentially the same characteristics.
    //   2. Will remove 0..N elements from an N-element list.
    //   3. Can remove elements from any or all positions in the list
    //   4. Destructors are called for removed elements and memory is deleted
    //      for removed elements.
    //   5. No constructors, destructors, or assignment operators are called
    //      on the remaining (non-removed) elements.
    //   6. No memory is allocated.
    //   7. Iterators to non-removed elements, including the 'end()' iterator,
    //      remain valid after removal.
    //   8. The non-'E' elements retain their relative order.
    //
    // Plan:
    //   For concern 1, perform the same tests for both 'remove' and
    //   'remove_if'.  Generate lists from a small set of specifications from
    //   empty to 10 elements, none of which contain the value 'E'.  Replace 0
    //   to 'LENGTH' elements with the value 'E', in every possible
    //   combination.  For each specification and combination, make a copy of
    //   all of the iterators to non-'E' elements, then call 'remove' or
    //   'remove_if'.  Validate that: The number of new destructor call
    //   matches the number of elements removed, reduction of memory blocks in
    //   use is correct for the number elements removed, the number of new
    //   allocations is zero, the number of new constructor calls is zero, and
    //   the iterating over the remaining elements produces a sequence of
    //   values and iterators matching those saved before the remove
    //   operation.
    //
    // Testing:
    //   void remove(const T& val);
    //   template <class PRED> void remove_if(PRED p);
    // --------------------------------------------------------------------

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    const TYPE&         E          = VALUES[4];  // Element with value 'E'
    (void) NUM_VALUES;

    bslma::TestAllocator testAllocator;
    ALLOC Z(&testAllocator);

    // Specifications from 0 to 10 elements long, none of which is the value
    // 'E'.
    const char* const SPECS[] = {
        "", "A", "AB", "ABA", "ABCD", "AAAA", "ABCDF", "ABCDFGHDAB"
    };

    const int NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]);
    const int MAX_SPEC_LEN = 10;

    enum {
        OP_FIRST,
        OP_REMOVE = OP_FIRST, // remove(const T& val);
        OP_REMOVE_IF,         // template <class PRED> void remove_if(PRED p);
        OP_LAST
    };

    for (int op = OP_FIRST; op < OP_LAST; ++op) {

        if (verbose) {
            switch (op) {
              case OP_REMOVE:
                printf("\nTesting remove(const T& val)\n");
                break;
              case OP_REMOVE_IF:
                printf("\nTesting remove(PRED p)\n");
                break;
            }
        }

        for (int i = 0; i < NUM_SPECS; ++i) {
            const char* const SPEC = SPECS[i];
            const int         LEN  = static_cast<int>(std::strlen(SPEC));

            ASSERT(MAX_SPEC_LEN >= LEN);

            // 'mask' contains a bit for each element in the list.  For each
            // '1' bit, the element is replaced by the value 'E'
            for (unsigned mask = 0; mask < (unsigned) (1 << LEN); ++mask) {

                Obj mX(Z);   const Obj& X = gg(&mX, SPEC);

                const_iterator save_iters[MAX_SPEC_LEN + 1];
                char res_spec[MAX_SPEC_LEN + 1]; // expected result spec
                int res_len = 0;  // To compute expected result length

                // Replace each element in 'mX' for which 'mask' has a '1'
                // bit with the value 'E'.
                iterator it = mX.begin();
                int idx = 0;
                for (unsigned bit = 1; bit < (unsigned) (1 << LEN);
                     bit <<= 1, ++it, ++idx) {
                    if ((mask & bit)) {
                        *it = E;
                    }
                    else {
                        save_iters[res_len] = it;
                        res_spec[res_len] = SPEC[idx];
                        ++res_len;
                    }
                }
                LOOP2_ASSERT(SPEC, mask, X.end() == it);
                save_iters[res_len] = X.end();
                res_spec[res_len] = '\0';

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64 B  = testAllocator.numBlocksInUse();
                const int CTORS_BEFORE = (numDefaultCtorCalls +
                                          numCharCtorCalls    +
                                          numCopyCtorCalls);
                const int ASSIGN_BEFORE = numAssignmentCalls;
                const int DTORS_BEFORE  = numDestructorCalls;

                if (veryVeryVerbose) { T_; printf("Before: "); P_(X); }

                switch (op) {
                  case OP_REMOVE:    mX.remove(E);           break;
                  case OP_REMOVE_IF: mX.remove_if(VPred(E)); break;
                }

                if (veryVeryVerbose) { printf("After: "); P(X); }

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64 A  = testAllocator.numBlocksInUse();
                const int CTORS_AFTER = (numDefaultCtorCalls +
                                         numCharCtorCalls    +
                                         numCopyCtorCalls);
                const int ASSIGN_AFTER = numAssignmentCalls;
                const int DTORS_AFTER  = numDestructorCalls;

                // Test result value
                LOOP3_ASSERT(SPEC, res_spec, X, checkIntegrity(X, res_len));
                LOOP3_ASSERT(SPEC, res_spec, X, (int) X.size() == res_len);
                LOOP3_ASSERT(SPEC, res_spec, X, X == g(res_spec));

                // Test that iterators are still valid
                const_iterator cit = X.begin();
                for (idx = 0; idx < (int) X.size(); ++idx, ++cit) {
                    LOOP3_ASSERT(SPEC, res_spec, idx, save_iters[idx] == cit);
                }
                // Test end iterator
                LOOP3_ASSERT(SPEC, res_spec, idx, save_iters[idx] == cit);

                // Test allocations and deallocations
                LOOP2_ASSERT(SPEC, res_spec, AA == BB);
                LOOP2_ASSERT(SPEC, res_spec,
                             deltaBlocks(LEN - res_len) == B - A);

                // If 'TYPE' is 'TestType', then test that no constructors or
                // assignments were called and the expected number of
                // destructors were called.
                if (bsl::is_same<TYPE, TestType>::value) {
                    LOOP2_ASSERT(SPEC, res_spec, CTORS_AFTER  == CTORS_BEFORE);
                    LOOP2_ASSERT(SPEC, res_spec,
                                 ASSIGN_AFTER == ASSIGN_BEFORE);
                    LOOP2_ASSERT(SPEC, res_spec,
                                 DTORS_AFTER == DTORS_BEFORE + (LEN-res_len));
                }

            } // end for (mask)
        } // end for (i)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testSplice()
{
    // --------------------------------------------------------------------
    // TESTING SPLICE
    //
    // Concerns:
    //   1. Can splice into any position within target list.
    //   2. Can splice from any position within source list.
    //   3. No iterators or pointers are invalidated.
    //   4. No allocations or deallocations occur.
    //   5. No constructor calls, destructor calls, or assignments occur.
    //
    // Test plan:
    //   Perform a small area test with source and target lists of 0 to 5
    //   elements each, splicing into every target position from every
    //   source position and every source length.  Keep track of the
    //   original iterators and element addresses from each list and
    //   verify that they remain valid and point to the correct element in
    //   the post-splice lists.  Query the number of allocations,
    //   deallocations, constructor calls, destructor calls, and
    //   assignment operator calls before and after each splice and verify
    //   that they do not change.
    //
    // Testing:
    //   void splice(iterator pos, list& other);
    //   void splice(iterator pos, list& other, iterator i);
    //   void splice(iterator pos, list& other,
    //               iterator first, iterator last);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator;
    ALLOC Z(&testAllocator);

    const char* const SPECS[] = {
        "", "A", "AB", "ABC", "ABCD", "ABCDE"
    };

    const int NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]);
    const int MAX_SPEC_LEN = 5;

    enum {
        OP_FIRST,
        OP_SPLICE_ALL = OP_FIRST, // splice(pos, other)
        OP_SPLICE_1,              // splice(pos, other, i)
        OP_SPLICE_RANGE,          // splice(pos, other, first, last)
        OP_LAST
    };

    for (int op = OP_FIRST; op < OP_LAST; ++op) {

        switch (op) {
            case OP_SPLICE_ALL:
                if (verbose) printf("\nTesting splice(pos, other)\n");
                break;
            case OP_SPLICE_1:
                if (verbose) printf("\nTesting splice(pos, other, i)\n");
                break;
            case OP_SPLICE_RANGE:
                if (verbose) printf("\nTesting splice(pos, other, "
                                    "first, last)\n");
                break;
        }

        for (int i = 0; i < NUM_SPECS * NUM_SPECS; ++i) {
            const char* const X_SPEC = SPECS[i / NUM_SPECS ];
            const int         X_LEN  = static_cast<int>(std::strlen(X_SPEC));
            const char* const Y_SPEC = SPECS[i % NUM_SPECS ];
            const int         Y_LEN  = static_cast<int>(std::strlen(Y_SPEC));

            if (veryVerbose) { P_(X_SPEC); P(Y_SPEC); }
            LOOP_ASSERT(X_SPEC, X_LEN <= MAX_SPEC_LEN);
            LOOP_ASSERT(Y_SPEC, Y_LEN <= MAX_SPEC_LEN);

            int max_y_pos = MAX_SPEC_LEN;
            int min_y_count = 0, max_y_count = MAX_SPEC_LEN;

            switch (op) {
                case OP_SPLICE_ALL:   min_y_count = Y_LEN;           break;
                case OP_SPLICE_1:     min_y_count = max_y_count = 1; break;
                case OP_SPLICE_RANGE:                                break;
            }

            if (max_y_pos + min_y_count > Y_LEN)
                max_y_pos = Y_LEN - min_y_count;

            for (int x_pos = 0; x_pos <= X_LEN; ++x_pos) {
                for (int y_pos = 0; y_pos <= max_y_pos; ++y_pos) {
                    for (int y_count = min_y_count;
                         y_count <= Y_LEN-y_pos && y_count <= max_y_count;
                         ++y_count)
                    {
                        Obj mX(Z);
                        const Obj& X = gg(&mX, X_SPEC);

                        Obj mY(Z);
                        const Obj& Y = gg(&mY, Y_SPEC);

                        if (veryVeryVerbose) {
                            T_; P_(x_pos); P_(y_pos); P(y_count);
                            T_; T_; printf("Before: "); P_(X); P(Y);
                        }

                        // iterators and pointers to elements -- BEFORE
                        iterator    BX_iters[MAX_SPEC_LEN + 1];
                        const TYPE* BX_ptrs[MAX_SPEC_LEN];
                        iterator    BY_iters[MAX_SPEC_LEN + 1];
                        const TYPE* BY_ptrs[MAX_SPEC_LEN];

                        // iterators and pointers to elements -- AFTER
                        iterator    AX_iters[2*MAX_SPEC_LEN + 1];
                        const TYPE* AX_ptrs[2*MAX_SPEC_LEN];
                        iterator    AY_iters[MAX_SPEC_LEN + 1];
                        const TYPE* AY_ptrs[MAX_SPEC_LEN];

                        // Save iterators and pointers into BEFORE arrays
                        iterator xi = mX.begin();
                        for (int j = 0; j < X_LEN; ++j, ++xi) {
                            BX_iters[j] = xi;
                            BX_ptrs[j] = &*xi;
                        }
                        BX_iters[X_LEN] = xi;

                        iterator yi = mY.begin();
                        for (int j = 0; j < Y_LEN; ++j, ++yi) {
                            BY_iters[j] = yi;
                            BY_ptrs[j] = &*yi;
                        }
                        BY_iters[Y_LEN] = yi;

                        // Compute iterators and pointers AFTER splice
                        for (int j = 0; j < x_pos; ++j) {
                            AX_iters[j] = BX_iters[j];
                            AX_ptrs[j] = BX_ptrs[j];
                        }
                        for (int j = 0; j < y_pos; ++j) {
                            AY_iters[j] = BY_iters[j];
                            AY_ptrs[j] = BY_ptrs[j];
                        }
                        for (int j = 0; j < y_count; ++j) {
                            AX_iters[x_pos + j] = BY_iters[y_pos + j];
                            AX_ptrs[x_pos + j] = BY_ptrs[y_pos + j];
                        }
                        for (int j = x_pos; j < X_LEN; ++j) {
                            AX_iters[j + y_count] = BX_iters[j];
                            AX_ptrs[j + y_count]  = BX_ptrs[j];
                        }
                        AX_iters[X_LEN + y_count] = BX_iters[X_LEN];
                        for (int j = y_pos + y_count; j < Y_LEN; ++j) {
                            AY_iters[j - y_count] = BY_iters[j];
                            AY_ptrs[j - y_count]  = BY_ptrs[j];
                        }
                        AY_iters[Y_LEN - y_count] = BY_iters[Y_LEN];

                        const Int64 BB = testAllocator.numBlocksTotal();
                        const Int64 B  = testAllocator.numBlocksInUse();
                        const int CTORS_BEFORE = (numDefaultCtorCalls +
                                                  numCharCtorCalls    +
                                                  numCopyCtorCalls);
                        const int ASSIGN_BEFORE = numAssignmentCalls;
                        const int DTORS_BEFORE  = numDestructorCalls;

                        switch (op) {
                            case OP_SPLICE_ALL:
                                ASSERT(0 == y_pos);
                                ASSERT(Y_LEN == y_count);
                                mX.splice(BX_iters[x_pos], mY);
                                break;
                            case OP_SPLICE_1:
                                ASSERT(1 == y_count);
                                mX.splice(BX_iters[x_pos], mY,
                                          BY_iters[y_pos]);
                                break;
                            case OP_SPLICE_RANGE:
                                mX.splice(BX_iters[x_pos], mY,
                                          BY_iters[y_pos],
                                          BY_iters[y_pos + y_count]);
                                break;
                        }

                        if (veryVeryVerbose) {
                            T_; T_; printf("After: "); P_(X); P(Y);
                        }

                        const Int64 AA = testAllocator.numBlocksTotal();
                        const Int64 A  = testAllocator.numBlocksInUse();
                        const int CTORS_AFTER = (numDefaultCtorCalls +
                                                 numCharCtorCalls    +
                                                 numCopyCtorCalls);
                        const int ASSIGN_AFTER = numAssignmentCalls;
                        const int DTORS_AFTER  = numDestructorCalls;

                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos, AA == BB);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos, A  == B );
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     CTORS_AFTER  == CTORS_BEFORE);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     ASSIGN_AFTER == ASSIGN_BEFORE);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     DTORS_AFTER  == DTORS_BEFORE);

                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     checkIntegrity(X, X_LEN + y_count));

                        xi = mX.begin();
                        for (int j = 0; j < X_LEN + y_count; ++j, ++xi) {
                            LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                         AX_iters[j] == xi);
                            LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                         AX_ptrs[j] == &*xi);
                        }
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     X.end() == xi);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     AX_iters[X_LEN + y_count] == xi);

                        yi = mY.begin();
                        for (int j = 0; j < Y_LEN - y_count; ++j, ++yi) {
                            LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                         AY_iters[j] == yi);
                            LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                         AY_ptrs[j] == &*yi);
                        }
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     Y.end() == yi);
                        LOOP4_ASSERT(X_SPEC, Y_SPEC, x_pos, y_pos,
                                     AY_iters[Y_LEN - y_count] == yi);

                    } // end for (y_count)
                } // end for (y_pos)
            } // end for (x_pos)
        } // end for (i)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testReverse()
{
    // --------------------------------------------------------------------
    // TESTING REVERSE
    //
    // Concerns:
    //   1. Reversing a list produced the correct result with 0, 1, 2,
    //      or more elements.
    //   2. Reversing a list with duplicate elements works as expected.
    //   3. No constructors, destructors, or assignment operators of
    //      contained elements are called.
    //   4. No memory is allocated or deallocated.
    //
    // Plan:
    //   Create a list from a variety of specifications, including empty
    //   lists, lists of different lengths, and lists with consecutive or
    //   non-consecutive duplicate elements and call 'reverse' on the
    //   list.  For concerns 1 and 2, verify that calling 'reverse'
    //   produces the expected result.  For concern 3, compare the counts
    //   of 'TestType' constructors and destructors before and after
    //   calling 'reverse' and verify that they do not change.  For
    //   concern 4, use a test allocator and compare the counts of total
    //   blocks allocated and blocks in use before and after calling
    //   'reverse' and verify that the counts do not change.
    //
    // Testing:
    //   void reverse();
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    ALLOC Z(&testAllocator);

    struct {
        int         d_line;
        const char* d_spec_before;
        const char* d_spec_after;
    } const DATA[] = {
        { L_, "",           ""              },
        { L_, "A",          "A"             },
        { L_, "AB",         "BA"            },
        { L_, "ABC",        "CBA"           },
        { L_, "ABCD",       "DCBA"          },
        { L_, "ABBC",       "CBBA"          },
        { L_, "ABCA",       "ACBA"          },
        { L_, "AAAA",       "AAAA"          },
        { L_, "ABCDEABCD",  "DCBAEDCBA"     },
    };

    const int NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

    for (int i = 0; i < NUM_DATA; ++i) {
        const int     LINE            = DATA[i].d_line;
        const char   *SPEC_BEFORE     = DATA[i].d_spec_before;
        const char   *SPEC_AFTER      = DATA[i].d_spec_after;
        const size_t  LENGTH          = strlen(SPEC_BEFORE);

        Obj mX(Z);
        const Obj& X = gg(&mX, SPEC_BEFORE);

        Obj mExp;
        const Obj& EXP = gg(&mExp, SPEC_AFTER);

        const Int64 BB = testAllocator.numBlocksTotal();
        const Int64 B  = testAllocator.numBlocksInUse();
        const int CTORS_BEFORE = (numDefaultCtorCalls +
                                  numCharCtorCalls    +
                                  numCopyCtorCalls);
        const int ASSIGN_BEFORE = numAssignmentCalls;
        const int DTORS_BEFORE  = numDestructorCalls;

        mX.reverse();

        const Int64 AA = testAllocator.numBlocksTotal();
        const Int64 A  = testAllocator.numBlocksInUse();
        const int CTORS_AFTER = (numDefaultCtorCalls +
                                 numCharCtorCalls    +
                                 numCopyCtorCalls);
        const int ASSIGN_AFTER = numAssignmentCalls;
        const int DTORS_AFTER  = numDestructorCalls;

        LOOP_ASSERT(LINE, checkIntegrity(X, LENGTH));
        LOOP_ASSERT(LINE, EXP == X);
        LOOP_ASSERT(LINE, AA == BB);
        LOOP_ASSERT(LINE, A  == B );
        LOOP_ASSERT(LINE, CTORS_AFTER  == CTORS_BEFORE);
        LOOP_ASSERT(LINE, ASSIGN_AFTER == ASSIGN_BEFORE);
        LOOP_ASSERT(LINE, DTORS_AFTER  == DTORS_BEFORE);
    } // end for (i)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testTypeTraits(bool uses_bslma,
                                            bool bitwise_moveable)
{
    // --------------------------------------------------------------------
    // TESTING TYPE TRAITS
    //
    // Concerns:
    //   1. That the list has the 'bslalg::HasStlIterators' trait.
    //   2. Iff instantiated with 'bsl::allocator', then list has the
    //      'bslma::UsesBslmaAllocator' trait.
    //   3. Iff instantiated with an allocator that is bitwise moveable, then
    //      the list has the 'bslmf::IsBitwiseMoveable' trait.
    //
    // Plan:
    //   Test each of the above three traits and compare their value to the
    //   expected value as expressed in the 'uses_bslma' and
    //   'bitwise_moveable' arguments to this function.
    //
    // Testing:
    //   bslalg::HasStlIterators
    //   bslma::UsesBslmaAllocator
    //   bslmf::IsBitwiseMoveable
    // --------------------------------------------------------------------

    ASSERT(bslalg::HasStlIterators<Obj>::value);

    LOOP_ASSERT(uses_bslma,
                uses_bslma == bslma::UsesBslmaAllocator<Obj>::value);

    LOOP_ASSERT(bitwise_moveable,
                bitwise_moveable == bslmf::IsBitwiseMoveable<Obj>::value);
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testComparisonOps()
{
    // --------------------------------------------------------------------
    // TESTING COMPARISON FREE OPERATORS
    //
    // Concerns:
    //   1. 'operator<' returns the lexicographic comparison on two lists.
    //   2. 'operator>', 'operator<=', and 'operator>=' are correctly tied to
    //      'operator<'.
    //   3. That traits get selected properly.
    //
    // Plan:
    //   For a variety of lists of different sizes and different values, test
    //   that the comparison returns as expected.
    //
    // Testing:
    //   bool operator<(const list<T,A>&, const list<T,A>&);
    //   bool operator>(const list<T,A>&, const list<T,A>&);
    //   bool operator<=(const list<T,A>&, const list<T,A>&);
    //   bool operator>=(const list<T,A>&, const list<T,A>&);
    // ------------------------------------------------------------------------

    // NOTE: These specs must be sorted in lexicographical order
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
    };

    const int NUM_SPECS = sizeof(SPECS) / sizeof(SPECS[0]);

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, v) in S x S \n.");
    {
        // Create first object
        for (int si = 0; si < NUM_SPECS; ++si) {
            const char *const U_SPEC = SPECS[si];

            Obj mU(g(U_SPEC));  const Obj& U = mU;

            if (veryVerbose) {
                T_; T_; P_(U_SPEC); P(U);
            }

            // Create second object
            for (int sj = 0; sj < NUM_SPECS; ++sj) {
                const char *const V_SPEC = SPECS[sj];

                Obj mV(g(V_SPEC));  const Obj& V = mV;

                if (veryVerbose) {
                    T_; T_; T_; P_(V_SPEC); P(V);
                }

                const bool isLT = si <  sj;
                const bool isLE = si <= sj;
                const bool isGT = si >  sj;
                const bool isGE = si >= sj;

                LOOP2_ASSERT(si, sj, isLT == (U <  V));
                LOOP2_ASSERT(si, sj, isLE == (U <= V));
                LOOP2_ASSERT(si, sj, isGT == (U >  V));
                LOOP2_ASSERT(si, sj, isGE == (U >= V));
                LOOP2_ASSERT(si, sj, (U <  V) == !(U >= V));
                LOOP2_ASSERT(si, sj, (U >  V) == !(U <= V));
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testSwap()
{
    // --------------------------------------------------------------------
    // TESTING SWAP
    //
    // Concerns:
    //   1. Swapping containers does not swap allocators.
    //   2. Swapping containers with same allocator results in no allocation
    //      or deallocation operations.
    //   3. Swapping containers with the same allocator causes iterators to
    //      remain valid but to refer to the opposite container.
    //   4. DEPRECATED: Swapping containers with different allocator instances
    //      will have the same memory usage copy-constructing each container
    //      and destroying the original.
    //   5. DEPRECATED: An exception thrown while swapping containers with
    //      different allocator instances will leave the containers unchanged.
    //
    // Plan:
    //   Construct 'lst1' and 'lst2' with same test allocator.
    //   Add data to each list.  Remember allocation statistics and iterators.
    //   Verify that contents were swapped.
    //   Verify that allocator is unchanged.
    //   Verify that no memory was allocated or deallocated.
    //   Verify that each iterator now refers to the same element in the other
    //      container.
    //   For concerns 4 and 5, construct two containers with different
    //   allocators and swap them within an exception test harness.  Verify
    //   the expected memory usage and verify that an exception leaves the
    //   containers unchanged.
    //
    // Testing:
    //   swap(list& rhs);                           // member
    //   bsl::swap(list<T,A>& lhs, list<T,A>& rhs); // free function
    // ------------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);
    const ALLOC Z(&testAllocator);
    const ALLOC Z2(&testAllocator2);

    const size_t        MAX_LEN    = 15;

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting member swap\n");
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int     XLINE   = DATA[ti].d_lineNum;
        const char   *XSPEC   = DATA[ti].d_spec;
        const size_t  XLENGTH = strlen(XSPEC);
        LOOP_ASSERT(XLINE, MAX_LEN >= XLENGTH);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int     YLINE   = DATA[tj].d_lineNum;
            const char   *YSPEC   = DATA[tj].d_spec;
            const size_t  YLENGTH = strlen(YSPEC);
            LOOP_ASSERT(YLINE, MAX_LEN >= YLENGTH);

            // Create two objects to be swapped.
            Obj mX(Z);  const Obj& X = gg(&mX, XSPEC);
            Obj mY(Z);  const Obj& Y = gg(&mY, YSPEC);

            // Save iterators
            const_iterator xiters[MAX_LEN + 1];
            const_iterator yiters[MAX_LEN + 1];

            const_iterator it = X.begin();
            for (size_t i = 0; i < XLENGTH + 1; ++i, ++it) {
                xiters[i] = it;
            }
            it = Y.begin();
            for (size_t i = 0; i < YLENGTH + 1; ++i, ++it) {
                yiters[i] = it;
            }

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            mX.swap(mY);  // Test here

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            // Test the contents have swapped.  Allocator is unchanged.
            LOOP2_ASSERT(XLINE, YLINE, g(YSPEC) == X);
            LOOP2_ASSERT(XLINE, YLINE, g(XSPEC) == Y);
            LOOP2_ASSERT(XLINE, YLINE, Z == X.get_allocator());
            LOOP2_ASSERT(XLINE, YLINE, Z == Y.get_allocator());

            // Test that iterators have swapped.  NOTE: the end iterator is
            // included in this test.  This test is correct for our current
            // implementation, but the standard does not require that the end
            // iterator be swapped.
            it = X.begin();
            for (size_t i = 0; i < YLENGTH + 1; ++i, ++it) {
                LOOP3_ASSERT(XLINE, YLINE, i, it == yiters[i]);
            }
            it = Y.begin();
            for (size_t i = 0; i < XLENGTH + 1; ++i, ++it) {
                LOOP3_ASSERT(XLINE, YLINE, i, it == xiters[i]);
            }

            // No allocations or deallocations should have occurred.
            LOOP2_ASSERT(XLINE, YLINE, BB == AA);
            LOOP2_ASSERT(XLINE, YLINE, B  == A );
        } // end for tj
    } // end for ti

    if (verbose) printf("\nTesting free swap\n");
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int     XLINE   = DATA[ti].d_lineNum;
        const char   *XSPEC   = DATA[ti].d_spec;
        const size_t  XLENGTH = strlen(XSPEC);
        LOOP_ASSERT(XLINE, MAX_LEN >= XLENGTH);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int     YLINE   = DATA[tj].d_lineNum;
            const char   *YSPEC   = DATA[tj].d_spec;
            const size_t  YLENGTH = strlen(YSPEC);
            LOOP_ASSERT(YLINE, MAX_LEN >= YLENGTH);

            // Create two objects to be swapped.
            Obj mX(Z);  const Obj& X = gg(&mX, XSPEC);
            Obj mY(Z);  const Obj& Y = gg(&mY, YSPEC);

            // Save iterators
            const_iterator xiters[MAX_LEN + 1];
            const_iterator yiters[MAX_LEN + 1];

            const_iterator it = X.begin();
            for (size_t i = 0; i < XLENGTH + 1; ++i, ++it) {
                xiters[i] = it;
            }
            it = Y.begin();
            for (size_t i = 0; i < YLENGTH + 1; ++i, ++it) {
                yiters[i] = it;
            }

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            swap(mX, mY);  // Test here

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            // Test the contents have swapped.  Allocator is unchanged.
            LOOP2_ASSERT(XLINE, YLINE, g(YSPEC) == X);
            LOOP2_ASSERT(XLINE, YLINE, g(XSPEC) == Y);
            LOOP2_ASSERT(XLINE, YLINE, Z == X.get_allocator());
            LOOP2_ASSERT(XLINE, YLINE, Z == Y.get_allocator());

            // Test that iterators have swapped.  NOTE: the end iterator is
            // included in this test.  This test is correct for our current
            // implementation, but the standard does not require that the end
            // iterator be swapped.
            it = X.begin();
            for (size_t i = 0; i < YLENGTH + 1; ++i, ++it) {
                LOOP3_ASSERT(XLINE, YLINE, i, it == yiters[i]);
            }
            it = Y.begin();
            for (size_t i = 0; i < XLENGTH + 1; ++i, ++it) {
                LOOP3_ASSERT(XLINE, YLINE, i, it == xiters[i]);
            }

            // No allocations or deallocations should have occurred.
            LOOP2_ASSERT(XLINE, YLINE, BB == AA);
            LOOP2_ASSERT(XLINE, YLINE, B  == A );
        } // end for tj
    } // end for ti

    if (verbose) printf("\nTesting member swap with unequal allocators\n");
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int     XLINE   = DATA[ti].d_lineNum;
        const char   *XSPEC   = DATA[ti].d_spec;
        const size_t  XLENGTH = strlen(XSPEC);
        LOOP_ASSERT(XLINE, MAX_LEN >= XLENGTH);

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int     YLINE   = DATA[tj].d_lineNum;
            const char   *YSPEC   = DATA[tj].d_spec;
            const size_t  YLENGTH = strlen(YSPEC);
            LOOP_ASSERT(YLINE, MAX_LEN >= YLENGTH);

            // Create two objects to be swapped.
            Obj mX(Z);  const Obj& X = gg(&mX, XSPEC);
            Obj mY(Z2); const Obj& Y = gg(&mY, YSPEC);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                ExceptionGuard<Obj> gx(&mX, X, XLINE);
                ExceptionGuard<Obj> gy(&mY, Y, YLINE);

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64 B  = testAllocator.numBlocksInUse();
                const Int64 BB2 = testAllocator2.numBlocksTotal();
                const Int64 B2  = testAllocator2.numBlocksInUse();

                mX.swap(mY);  // Test here

                gx.release();
                gy.release();

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64 A  = testAllocator.numBlocksInUse();
                const Int64 AA2 = testAllocator2.numBlocksTotal();
                const Int64 A2  = testAllocator2.numBlocksInUse();

                // Test the contents have swapped.  Allocator is unchanged.
                LOOP2_ASSERT(XLINE, YLINE, g(YSPEC) == X);
                LOOP2_ASSERT(XLINE, YLINE, g(XSPEC) == Y);
                LOOP2_ASSERT(XLINE, YLINE, Z  == X.get_allocator());
                LOOP2_ASSERT(XLINE, YLINE, Z2 == Y.get_allocator());

                // Total allocations increased by enough to build copies
                // of each list
                LOOP2_ASSERT(XLINE, YLINE, BB +expectedBlocks(YLENGTH) == AA );
                LOOP2_ASSERT(XLINE, YLINE, BB2+expectedBlocks(XLENGTH) == AA2);

                // Blocks in use have effectively swapped
                ptrdiff_t difference =
                                     static_cast<ptrdiff_t>(YLENGTH - XLENGTH);
                LOOP2_ASSERT(XLINE, YLINE, A - B   == deltaBlocks(difference));
                LOOP2_ASSERT(XLINE, YLINE, A2 - B2 == -(A - B));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        } // end for tj
    } // end for ti
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testErase()
{
    // --------------------------------------------------------------------
    // TESTING ERASE
    //
    // Concerns:
    //   1. That the resulting value is correct.
    //   2. That erase operations do not allocate memory.
    //   3. That no memory is leaked.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a list
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - That the total allocations do not increase.
    //      - That the in-use allocations diminish by the correct amount.
    //
    // Testing:
    //   void pop_back();
    //   void pop_front();
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

    const int           MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_FIRST,
        TEST_ERASE1 = TEST_FIRST,     // erase(pos);
        TEST_ERASE_RANGE,             // erase(first, last);
        TEST_POP_BACK,                // pop_back();
        TEST_POP_FRONT,               // pop_front();
        TEST_LAST
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // Iterate through the operations
    for (int op = TEST_FIRST; op < TEST_LAST; ++op) {

        const char* opname = "<unknown>";

        switch (op) {
            case TEST_ERASE1:      opname = "erase(iterator)";           break;
            case TEST_ERASE_RANGE: opname = "erase(iterator, iterator)"; break;
            case TEST_POP_BACK:    opname = "pop_back()";                break;
            case TEST_POP_FRONT:   opname = "pop_front()";               break;
        }

        if (verbose) printf("\ntesting %s\n", opname);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const int     LENGTH = static_cast<int>(strlen(SPEC));

            LOOP_ASSERT(LENGTH, LENGTH <= MAX_LEN);

            int pos_first, pos_last;  // possible start positions

            switch (op) {
                case TEST_ERASE1:
                    pos_first = 0;
                    pos_last  = LENGTH - 1;
                    break;
                case TEST_ERASE_RANGE:
                    pos_first = 0;
                    pos_last  = LENGTH;
                    break;
                case TEST_POP_BACK:
                    pos_first = LENGTH - 1;
                    pos_last  = LENGTH - 1;
                    break;
                case TEST_POP_FRONT:
                    pos_first = 0;
                    pos_last  = 0;
                    break;
            } // end switch

            for (int posidx = pos_first; posidx <= pos_last; ++posidx) {

                int erase_min, erase_max;      // num elements to be erased
                if (TEST_ERASE_RANGE == op) {
                    erase_min = 0;
                    erase_max = LENGTH - posidx;
                }
                else {
                    erase_min = 1;
                    erase_max = 1;
                    if (LENGTH < 1) continue;
                }

                for (int n = erase_min; n <= erase_max; ++n) {

                    Obj mX(Z);
                    const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Save original iterators (including end iterator)
                    // C++0x allows erasing using const_iterator
                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (int i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    // C++0x allows erasing using const_iterator
                    const_iterator pos = orig_iters[posidx];
                    iterator ret;

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64 B  = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; P_(SPEC); P_(posidx); P(n);
                    }

                    switch (op) {
                        case TEST_ERASE1: {
                            ret = mX.erase(pos);
                        } break;
                        case TEST_ERASE_RANGE: {
                            // C++0x allows erasing using const_iterator
                            const_iterator end_range = succ(pos, n);
                            ret = mX.erase(pos, end_range);
                        } break;
                        case TEST_POP_BACK: {
                            mX.pop_back();
                            ret = mX.end();
                        } break;
                        case TEST_POP_FRONT: {
                            mX.pop_front();
                            ret = mX.begin();
                        } break;
                    } // end switch

                    // Should never have an exception, so should always get
                    // here.

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64 A  = testAllocator.numBlocksInUse();

                    // Test important values
                    LOOP3_ASSERT(LINE, op, posidx,
                                 checkIntegrity(X, LENGTH - n));
                    LOOP3_ASSERT(LINE, op, posidx,
                                 LENGTH - n == (int) X.size());
                    LOOP3_ASSERT(LINE, op, posidx, BB == AA);
                    LOOP3_ASSERT(LINE, op, posidx, B + deltaBlocks(-n) == A);

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (int i = 0; i < LENGTH; ++i, ++yi) {
                        if (i < posidx) {
                            // Test that part before erasure is unchanged
                            LOOP4_ASSERT(LINE, op, posidx, i, *yi == *cit);
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         orig_iters[i] == cit);
                            ++cit;
                        }
                        else if (i < posidx + n) {
                            // skip erased values
                            continue;
                        }
                        else {
                            // Test that part after erasure is unchanged
                            LOOP4_ASSERT(LINE, op, posidx, i, *yi == *cit);
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         orig_iters[i] == cit);
                            ++cit;
                        }
                    }
                    // Test end iterator
                    LOOP3_ASSERT(LINE, op, posidx, X.end() == cit);
                    LOOP3_ASSERT(LINE, op, posidx, Y.end() == yi);
                    LOOP3_ASSERT(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } // end for (n)

                LOOP3_ASSERT(LINE, op, posidx,
                             0 == testAllocator.numBlocksInUse());
            } // end for (posidx)
        } // end for (ti)
    } // end for (op)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testInsert()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    //
    // Concerns
    //   1. That the resulting list value is correct.
    //   2. That the 'insert' return (if any) value is a valid iterator to the
    //      first inserted element or to the insertion position if no elements
    //      are inserted.
    //   3. That insertion of one element has the strong exception guarantee.
    //   4. That insertion is exception neutral w.r.t. memory allocation.
    //   5. The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   6. That inserting a 'const T& value' that is a reference to an element
    //      of the list does not suffer from aliasing problems.
    //   7. That no iterators are invalidated by the insertion.
    //   8. That inserting 'n' copies of value 'v' selects the correct
    //      overload when 'n' and 'v' are identical arithmetic types (i.e.,
    //      the iterator-range overload is not selected).
    //   9. That inserting 'n' copies of value 'v' selects the correct
    //      overload when 'v' is a pointer type and 'n' is a null pointer
    //      literal ,'0'. (i.e., the iterator-range overload is not selected).
    //
    // Plan:
    //   Create objects of various sizes and insert a distinct value one or
    //   more times into each possible position.  For concerns 1, 2 & 5, verify
    //   that the return value and modified list are as expected.  For concerns
    //   3 & 4 perform the test using the exception-testing infrastructure and
    //   verify the value and memory changes.  For concern 6, we select the
    //   value to insert from the middle of the list, thus testing insertion
    //   before, at, and after the aliased element.  For concern 7, save
    //   copies of the iterators before and after the insertion point and
    //   verify that they point to the same (valid) elements after the
    //   insertion by iterating to the same point in the resulting list and
    //   comparing the new iterators to the old ones.  For concerns 8 and 9,
    //   insert 2 elements of integral or pointer types into lists and verify
    //   that it compiles and that the resultant list contains the expected
    //   values.
    //
    // Testing:
    //   iterator insert(const_iterator pos, const T& value);
    //   iterator insert(const_iterator pos, size_type n, const T& value);
    //   void push_back(const T& value);
    //   void push_front(const T& value);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    const int           MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_FIRST,
        TEST_INSERT_N0 = TEST_FIRST,  // insert(pos, 0, value);
        TEST_INSERT_N1,               // insert(pos, 1, value);
        TEST_INSERT_N2,               // insert(pos, 2, value);
        TEST_INSERT_N3,               // insert(pos, 3, value);
        TEST_INSERT,                  // insert(pos, value);
        TEST_PUSH_BACK,               // push_back(value);
        TEST_PUSH_FRONT,              // push_front(value);
        TEST_LAST
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // Iterate through the operations
    for (int op = TEST_FIRST; op < TEST_LAST; ++op) {

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const int     LENGTH = static_cast<int>(strlen(SPEC));

            LOOP_ASSERT(LENGTH, LENGTH <= MAX_LEN);

            for (int posidx = 0; posidx <= LENGTH; ++posidx) {

                if (TEST_PUSH_BACK == op && LENGTH != posidx) {
                    continue;  // Can push_back only at end
                }
                else if (TEST_PUSH_FRONT == op && 0 != posidx) {
                    continue;  // Can push_front only at beginning
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(Z);
                    const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Choose a value to insert that is deliberately aliasing
                    // a list element.
                    const TYPE& NEW_ELEM_REF(LENGTH ?
                                             nthElem(X,LENGTH/2) :
                                             VALUES[0]);
                    const TYPE NEW_ELEM_VALUE(NEW_ELEM_REF);

                    // Save original iterators (including end iterator)
                    // C++0x allows insertion using const_iterator
                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (int i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    testAllocator.setAllocationLimit(AL);

                    int n = 0;
                    // C++0x allows insertion using const_iterator
                    const_iterator pos = orig_iters[posidx];
                    iterator ret;
                    ExceptionGuard<Obj> guard(&mX, X, LINE);

                    const Int64 B = testAllocator.numBlocksInUse();

                    switch (op) {
                        case TEST_INSERT: {
                            ret = mX.insert(pos, NEW_ELEM_REF);
                            n = 1;
                        } break;
                        case TEST_PUSH_BACK: {
                            mX.push_back(NEW_ELEM_REF);
                            ret = --mX.end();
                            n = 1;
                        } break;
                        case TEST_PUSH_FRONT: {
                            mX.push_front(NEW_ELEM_REF);
                            ret = mX.begin();
                            n = 1;
                        } break;
                        default: {
                            n = op - TEST_INSERT_N0;
                            if (n > 1)
                                // strong guarantee only for 0 or 1 insertion
                                guard.release();
                            ret = mX.insert(pos, n, NEW_ELEM_REF);
                        }
                    } // end switch
                    guard.release();

                    // If got here, then there was no exception

                    const Int64 A = testAllocator.numBlocksInUse();

                    // Test important values
                    LOOP3_ASSERT(LINE, op, posidx,
                                 checkIntegrity(X, LENGTH + n));
                    LOOP3_ASSERT(LINE, op, posidx,
                                 LENGTH + n == (int) X.size());
                    LOOP3_ASSERT(LINE, op, posidx, B + deltaBlocks(n) == A);

                    // Test return value from 'insert'
                    LOOP3_ASSERT(LINE, op, posidx,
                                 bsl::distance(mX.begin(), ret) == posidx);

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (int i = 0; i < (int) X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            LOOP4_ASSERT(LINE, op, posidx, i, *yi++ == *cit);
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         orig_iters[i] == cit);
                        }
                        else if (i < posidx + n) {
                            // Test inserted values
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         NEW_ELEM_VALUE == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            LOOP4_ASSERT(LINE, op, posidx, i, *yi++ == *cit);
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         orig_iters[i - n] == cit);
                        }
                    }
                    // Test end iterator
                    LOOP3_ASSERT(LINE, op, posidx, X.end() == cit);
                    LOOP3_ASSERT(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, op, posidx,
                             0 == testAllocator.numBlocksInUse());
            } // end for (posidx)
        } // end for (ti)
    } // end for (op)

    if (verbose) printf("\nTesting overloading disambiguation\n");
    {
        // 'n' and 'v' are identical arithmetic types.  Make sure overload
        // resolution doesn't try to call the iterator-range 'insert'.
        {
            list<size_t, ALLOC> x;
            list<size_t, ALLOC>& X = x;
            size_t n = 2, v = 99;

            x.insert(X.begin(), n, v);
            ASSERT(X.size()  == n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
        {
            list<IntWrapper, ALLOC> x;
            list<IntWrapper, ALLOC>& X = x;
            unsigned char n = 2, v = 99;

            x.insert(X.begin(), n, v);
            ASSERT(X.size()  == n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
        {
            list<IntWrapper, ALLOC> x;
            list<IntWrapper, ALLOC>& X = x;
            size_t n = 2;
            int v = 99;

            x.insert(X.begin(), n, v);
            ASSERT(X.size()  == n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
        {
            // TBD: the below code block causes warnings.
            //list<IntWrapper, ALLOC> x;
            //list<IntWrapper, ALLOC>& X = x;
            //float n = 2, v = 99;

            //x.insert(X.begin(), n, v);
            //ASSERT(X.size()  == n);
            //ASSERT(X.front() == v);
            //ASSERT(X.back()  == v);
        }

        {
            list<IntWrapper, ALLOC> x;
            list<IntWrapper, ALLOC>& X = x;
            TestEnum n = TWO, v = NINETYNINE;

            x.insert(X.begin(), n, v);
            ASSERT(X.size()  == (size_t)n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }

        // 'n' is an 'int' and 'v' is a zero 'int' literal (which is also a
        // null pointer literal).  Make sure that it is correctly treated as a
        // pointer.
        {
            list<char*, ALLOC> x;
            list<char*, ALLOC>& X = x;
            int   n = 2;
            char *v = 0;

            x.insert(X.begin(), n, 0);  // Literal null, acts like an int.
            ASSERT(X.size()  == (size_t)n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testEmplace()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION USING EMPLACE:
    //
    // Concerns
    //   1. That the resulting list value is correct.
    //   2. That the 'emplace' return (if any) value is a valid iterator to the
    //      first inserted element or to the insertion position if no elements
    //      are inserted.
    //   3. That 'emplace' has the strong exception guarantee.
    //   4. That 'emplace' is exception neutral w.r.t. memory allocation.
    //   5. The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   6. That inserting a 'const T& value' that is a reference to an element
    //      of the list does not suffer from aliasing problems.
    //   7. That no iterators are invalidated by the insertion.
    //   8. That 'emplace' passes 0 to 5 arguments to the 'T' constructor.
    //
    // Plan:
    //   Create objects of various sizes and insert a distinct value into each
    //   possible position.  For concerns 1, 2 & 5, verify that the return
    //   value and modified list are as expected.  For concerns 3 & 4 perform
    //   the test using the exception-testing infrastructure and verify the
    //   value and memory changes.  For concern 6, we select the value to
    //   insert from the middle of the list, thus testing insertion before,
    //   at, and after the aliased element.  For concern 7, save copies of the
    //   iterators before and after the insertion point and verify that they
    //   point to the same (valid) elements after the insertion by iterating
    //   to the same point in the resulting list and comparing the new
    //   iterators to the old ones.  For concern 8, test each 'emplace' call
    //   with 0 to 5 arguments.  The test types are designed to ignore all but
    //   the last argument, but verify that the preceding arguments are the
    //   values '1', '2', '3', and '4'.
    //
    // Testing:
    //   iterator emplace(const_iterator pos, Args&&... args);
    //   void emplace_back(Args&&... args);
    //   void emplace_front(Args&&... args);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

    const TYPE         *values        = 0;
    const TYPE *const&  VALUES        = values;
    const int           NUM_VALUES    = getValues(&values);
    const TYPE          DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);
    (void) NUM_VALUES;

    const int           MAX_LEN    = 15;

    // Operations to test
    enum {
        TEST_FIRST,
        TEST_EMPLACE_A0 = TEST_FIRST, // emplace(pos);
        TEST_EMPLACE_A1,              // emplace(pos, a1);
        TEST_EMPLACE_A2,              // emplace(pos, a1, a2);
        TEST_EMPLACE_A3,              // emplace(pos, a1, a2, a3);
        TEST_EMPLACE_A4,              // emplace(pos, a1, a2, a3, a4);
        TEST_EMPLACE_A5,              // emplace(pos, a1, a2, a3, a4, a5);
        TEST_EMPLACE_FRONT_A0,        // emplace_front(pos);
        TEST_EMPLACE_FRONT_A1,        // emplace_front(pos, a1);
        TEST_EMPLACE_FRONT_A2,        // emplace_front(pos, a1, a2);
        TEST_EMPLACE_FRONT_A3,        // emplace_front(pos, a1, a2, a3);
        TEST_EMPLACE_FRONT_A4,        // emplace_front(pos, a1, a2, a3, a4);
        TEST_EMPLACE_FRONT_A5,        // emplace_front(pos, a1, a2, a3, a4,a5);
        TEST_EMPLACE_BACK_A0,         // emplace_back(pos);
        TEST_EMPLACE_BACK_A1,         // emplace_back(pos, a1);
        TEST_EMPLACE_BACK_A2,         // emplace_back(pos, a1, a2);
        TEST_EMPLACE_BACK_A3,         // emplace_back(pos, a1, a2, a3);
        TEST_EMPLACE_BACK_A4,         // emplace_back(pos, a1, a2, a3, a4);
        TEST_EMPLACE_BACK_A5,         // emplace_back(pos, a1, a2, a3, a4, a5);
        TEST_LAST
    };

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // Iterate through the operations
    for (int op = TEST_FIRST; op < TEST_LAST; ++op) {

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const int     LENGTH = static_cast<int>(strlen(SPEC));

            LOOP_ASSERT(LENGTH, LENGTH <= MAX_LEN);

            for (int posidx = 0; posidx <= LENGTH; ++posidx) {

                if (TEST_EMPLACE_BACK_A0 <= op &&
                    op <= TEST_EMPLACE_BACK_A5 && LENGTH != posidx) {
                    continue;  // Can emplace_back only at end
                }
                else if (TEST_EMPLACE_FRONT_A0 <= op &&
                         op <= TEST_EMPLACE_FRONT_A5 && 0 != posidx) {
                    continue;  // Can emplace_front only at beginning
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(Z);
                    const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Choose a value to insert that is deliberately aliasing
                    // a list element.
                    bool useDefault = (TEST_EMPLACE_A0       ||
                                       TEST_EMPLACE_FRONT_A0 ||
                                       TEST_EMPLACE_BACK_A0);
                    const TYPE& NEW_ELEM_REF(useDefault ?
                                             DEFAULT_VALUE :
                                             LENGTH ?
                                             nthElem(X,LENGTH/2) :
                                             VALUES[0]);
                    const TYPE NEW_ELEM_VALUE(NEW_ELEM_REF);

                    // Save original iterators (including end iterator)
                    // C++0x allows insertion using const_iterator
                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (int i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    testAllocator.setAllocationLimit(AL);

                    // C++0x allows insertion using const_iterator
                    const_iterator pos = orig_iters[posidx];
                    iterator ret;
                    ExceptionGuard<Obj> guard(&mX, X, LINE);

                    const Int64 BB = testAllocator.numBlocksTotal();
                    const Int64 B  = testAllocator.numBlocksInUse();

                    switch (op) {
                        case TEST_EMPLACE_A0: {
                            ret = mX.emplace(pos);
                        } break;
                        case TEST_EMPLACE_A1: {
                            ret = mX.emplace(pos, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A2: {
                            ret = mX.emplace(pos, 1, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A3: {
                            ret = mX.emplace(pos, 1, 2, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A4: {
                            ret = mX.emplace(pos, 1, 2, 3, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_A5: {
                            ret = mX.emplace(pos, 1, 2, 3, 4, NEW_ELEM_REF);
                        } break;
                        case TEST_EMPLACE_FRONT_A0: {
                            mX.emplace_front();
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A1: {
                            mX.emplace_front(NEW_ELEM_REF);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A2: {
                            mX.emplace_front(1, NEW_ELEM_REF);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A3: {
                            mX.emplace_front(1, 2, NEW_ELEM_REF);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A4: {
                            mX.emplace_front(1, 2, 3, NEW_ELEM_REF);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_FRONT_A5: {
                            mX.emplace_front(1, 2, 3, 4, NEW_ELEM_REF);
                            ret = mX.begin();
                        } break;
                        case TEST_EMPLACE_BACK_A0: {
                            mX.emplace_back();
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A1: {
                            mX.emplace_back(NEW_ELEM_REF);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A2: {
                            mX.emplace_back(1, NEW_ELEM_REF);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A3: {
                            mX.emplace_back(1, 2, NEW_ELEM_REF);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A4: {
                            mX.emplace_back(1, 2, 3, NEW_ELEM_REF);
                            ret = --mX.end();
                        } break;
                        case TEST_EMPLACE_BACK_A5: {
                            mX.emplace_back(1, 2, 3, 4, NEW_ELEM_REF);
                            ret = --mX.end();
                        } break;
                        default: {
                            ASSERT(0 && "No such operation");
                        }
                    } // end switch
                    guard.release();

                    // If got here, then there was no exception

                    const Int64 AA = testAllocator.numBlocksTotal();
                    const Int64 A  = testAllocator.numBlocksInUse();

                    // Test important values
                    LOOP3_ASSERT(LINE, op, posidx,
                                 checkIntegrity(X, LENGTH + 1));
                    LOOP3_ASSERT(LINE, op, posidx,
                                 LENGTH + 1 == (int) X.size());
                    LOOP3_ASSERT(LINE, op, posidx, BB + deltaBlocks(1) == AA);
                    LOOP3_ASSERT(LINE, op, posidx, B + deltaBlocks(1) == A);

                    if (TEST_EMPLACE_A0 <= op && op <= TEST_EMPLACE_A5) {
                        // Test return value from emplace
                        LOOP3_ASSERT(LINE, op, posidx,
                                     bsl::distance(mX.begin(), ret) == posidx);
                    }

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (int i = 0; i < (int) X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            LOOP4_ASSERT(LINE, op, posidx, i, *yi++ == *cit);
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         orig_iters[i] == cit);
                        }
                        else if (i == posidx) {
                            // Test inserted value
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         NEW_ELEM_VALUE == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            LOOP4_ASSERT(LINE, op, posidx, i, *yi++ == *cit);
                            LOOP4_ASSERT(LINE, op, posidx, i,
                                         orig_iters[i - 1] == cit);
                        }
                    }
                    // Test end iterator
                    LOOP3_ASSERT(LINE, op, posidx, X.end() == cit);
                    LOOP3_ASSERT(LINE, op, posidx, orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, op, posidx,
                             0 == testAllocator.numBlocksInUse());
            } // end for (posidx)
        } // end for (ti)
    } // end for (op)
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testInsertRange(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING RANGE INSERTION:
    //
    // Concerns
    //   1. That the resulting list value is correct.
    //   2. That the 'insert' return (if any) value is a valid iterator to the
    //      first inserted element or to the insertion position if no elements
    //      are inserted.
    //   3. That insertion of one element has the strong exception guarantee.
    //   4. That insertion is exception neutral w.r.t. memory allocation.
    //   5. The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   6. That no iterators are invalidated by the insertion.
    //
    // Plan:
    //   Create objects of various sizes and insert a range of 0 to 3 values
    //   at each possible position.  For concerns 1, 2 & 5, verify
    //   that the return value and modified list are as expected.  For concerns
    //   3 & 4 perform the test using the exception-testing infrastructure and
    //   verify the value and memory changes.  For concern 7, save
    //   copies of the iterators before and after the insertion point and
    //   verify that they point to the same (valid) elements after the
    //   insertion by iterating to the same point in the resulting list and
    //   comparing the new iterators to the old ones.
    //
    // Testing:
    //   template <class InputIter>
    //    void insert(const_iterator pos, InputIter first, InputIter last);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) VALUES;
    (void) NUM_VALUES;

    const int           MAX_LEN    = 15;

    // Starting data
    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "A"                       }, // 1
        { L_,   "AB"                      }, // 2
        { L_,   "ABC"                     }, // 3
        { L_,   "ABCD"                    }, // 4
        { L_,   "ABCDA"                   }, // 5
        { L_,   "ABCDABCDABCDABC"         }, // 15
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // Data to insert
    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } U_DATA[] = {
        //line  spec                            length
        //----  ----                            ------
        { L_,   ""                        }, // 0
        { L_,   "E"                       }, // 1
        { L_,   "EA"                      }, // 2
        { L_,   "EBA"                     }, // 3
    };
    const int NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int     LINE   = DATA[ti].d_lineNum;
        const char   *SPEC   = DATA[ti].d_spec;
        const int     LENGTH = static_cast<int>(strlen(SPEC));

        LOOP_ASSERT(LENGTH, LENGTH <= MAX_LEN);

        for (int posidx = 0; posidx <= LENGTH; ++posidx) {

            for (int ui = 0; ui < NUM_U_DATA; ++ui) {

                const int     U_LINE   = U_DATA[ui].d_lineNum;
                const char   *U_SPEC   = U_DATA[ui].d_spec;
                const size_t  N        = strlen(U_SPEC);

                CONTAINER mU(U_SPEC);  const CONTAINER& U = mU;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(Z);
                    const Obj& X = gg(&mX, SPEC);

                    const Obj Y(X);  // Control

                    // Save original iterators (including end iterator)
                    // C++0x allows insertion using const_iterator
                    const_iterator orig_iters[MAX_LEN + 1];
                    const_iterator it = X.begin();
                    for (int i = 0; i < LENGTH + 1; ++i, ++it) {
                        orig_iters[i] = it;
                    }

                    testAllocator.setAllocationLimit(AL);

                    // C++0x allows insertion using const_iterator
                    const_iterator pos = orig_iters[posidx];
                    iterator ret;
                    ExceptionGuard<Obj> guard(&mX, X, LINE);

                    const Int64 B = testAllocator.numBlocksInUse();

                    if (N > 1) {
                        // strong guarantee only for 0 or 1 insertions
                        guard.release();
                    }

                    ret = mX.insert(pos, U.begin(), U.end());
                    guard.release();

                    // If got here, then there was no exception

                    const Int64 A = testAllocator.numBlocksInUse();

                    // Test important values
                    LOOP3_ASSERT(LINE, posidx, U_LINE,
                                 checkIntegrity(X, LENGTH + N));
                    LOOP3_ASSERT(LINE, posidx, U_LINE, LENGTH + N == X.size());
                    LOOP3_ASSERT(LINE, posidx, U_LINE,
                                 B + deltaBlocks(N) == A);
                    LOOP3_ASSERT(LINE, posidx, U_LINE,
                                 bsl::distance(mX.begin(), ret) == posidx);

                    const_iterator cit = X.begin();
                    const_iterator yi = Y.begin();
                    for (int i = 0; i < (int) X.size(); ++i, ++cit) {
                        if (i < posidx) {
                            // Test that part before insertion is unchanged
                            LOOP4_ASSERT(LINE, posidx, U_LINE,
                                         i, *yi++ == *cit);
                            LOOP4_ASSERT(LINE, posidx, U_LINE, i,
                                         orig_iters[i] == cit);
                        }
                        else if (i < posidx + (int) N) {
                            // Test inserted values
                            LOOP4_ASSERT(LINE, posidx, U_LINE, i,
                                         U[i - posidx] == *cit);
                        }
                        else {
                            // Test that part after insertion is unchanged
                            LOOP4_ASSERT(LINE, posidx, U_LINE, i,
                                         *yi++ == *cit);
                            LOOP4_ASSERT(LINE, posidx, U_LINE, i,
                                         orig_iters[i - N] == cit);
                        }
                    }
                    // Test end iterator
                    LOOP3_ASSERT(LINE, posidx, U_LINE, X.end() == cit);
                    LOOP3_ASSERT(LINE, posidx, U_LINE,
                                 orig_iters[LENGTH] == cit);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, posidx, U_LINE,
                             0 == testAllocator.numBlocksInUse());
            } // end for (ui)
        } // end for (posidx)
    } // end for (ti)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testIterators()
{
    // --------------------------------------------------------------------
    // TESTING ITERATORS
    //
    // Concerns:
    //   1. That 'iterator' and 'const_iterator' are bi-directional iterators.
    //   2. That 'iterator' and 'const_iterator' are CopyConstructible,
    //      Assignable, and EqualityComparable, that 'iterator' is
    //      convertible to 'const_iterator', and that 'reverse_iterator' is
    //      constructible from 'iterator'.
    //   3. That 'begin' and 'end' return mutable iterators for a
    //      reference to a modifiable list, and non-mutable iterators
    //      otherwise.
    //   4. That the iterators can be dereferenced using 'operator*' or
    //      'operator->', yielding a reference or pointer with the correct
    //      constness.
    //   5. That the range '[begin(), end())' equals the value of the list.
    //   6. That iterators can be pre-incremented, post-incremented,
    //      pre-decremented, and post-decremented.
    //   7. Same concerns with 'rbegin', 'rend', 'reverse_iterator', and
    //      'const_reverse_iterator'.
    //
    // Plan:
    //   For concerns 1, 3, 4, and 7 create a one-element list and verify the
    //   static properties of 'iterator', 'const_iterator',
    //   ''reverse_iterator', and 'const_reverse_iterator'.
    //
    //   For concerns 1, 2, 5, 6, and 7, for each value given by variety of
    //   specifications of different lengths, create a test list with this
    //   value, and access each element in sequence and in reverse sequence,
    //   both as a modifiable reference (setting it to a default value, then
    //   back to its original value), and as a non-modifiable reference.  At
    //   each step in the traversal, save the current iterator using both copy
    //   construction and assignment and, in a nested second loop, traverse
    //   the whole list in reverse order, testing that the nested-traversal
    //   iterator matches the saved iterator iff they refer to the same
    //   element.
    //
    // Testing:
    //   type iterator
    //   type reverse_iterator
    //   type const_iterator
    //   type const_reverse_iterator
    //   iterator begin();
    //   iterator end();
    //   reverse_iterator rbegin();
    //   reverse_iterator rend();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //   const_reverse_iterator rbegin() const;
    //   const_reverse_iterator rend() const;
    // --------------------------------------------------------------------

    typedef typename Obj::size_type size_type;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE();
    (void) DEFAULT_VALUE;

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

    if (verbose) printf("Testing 'iterator', 'reverse_iterator', "
                        "'const_iterator', and 'const_reverse_iterator'\n");
    {
        Obj mX(2); const Obj& X = mX;

        const iterator               iter   = mX.begin();
        const const_iterator         citer  =  X.begin();
        const reverse_iterator       riter  = mX.rbegin();
        const const_reverse_iterator criter =  X.rbegin();

        // Check iterator category
        ASSERT((bsl::is_same<typename iterator::iterator_category,
                             bidirectional_iterator_tag>::value));
        ASSERT((bsl::is_same<typename reverse_iterator::iterator_category,
                             bidirectional_iterator_tag>::value));
        ASSERT((bsl::is_same<typename const_iterator::iterator_category,
                             bidirectional_iterator_tag>::value));
        ASSERT((bsl::is_same<typename const_reverse_iterator::iterator_category
                            ,bidirectional_iterator_tag>::value));

        // Test mutability
        ASSERT(  is_mutable(*mX.begin()));
        ASSERT(! is_mutable(* X.begin()));
        ASSERT(  is_mutable(*mX.rbegin()));
        ASSERT(! is_mutable(* X.rbegin()));
        ASSERT(  is_mutable(*--mX.end()));
        ASSERT(! is_mutable(*-- X.end()));
        ASSERT(  is_mutable(*--mX.rend()));
        ASSERT(! is_mutable(*-- X.rend()));

        ASSERT(  is_mutable(*iter));
        ASSERT(! is_mutable(*citer));
        ASSERT(  is_mutable(*riter));
        ASSERT(! is_mutable(*criter));

        // Test dereferencing
        ASSERT(&*iter   == &X.front());
        ASSERT(&*citer  == &X.front());
        ASSERT(&*riter  == &X.back());
        ASSERT(&*criter == &X.back());

        // Test operator->()
        ASSERT(iter.operator->()   == &X.front());
        ASSERT(citer.operator->()  == &X.front());
        ASSERT(riter.operator->()  == &X.back());
        ASSERT(criter.operator->() == &X.back());
    }

    if (verbose) printf("Testing 'begin', and 'end', 'rbegin', 'rend', "
                        " and their 'const' variants.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            Obj mX(&testAllocator);  const Obj& X = gg(&mX, SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            iterator               iter   = mX.begin();
            const_iterator         citer  =  X.begin();
            reverse_iterator       riter  = mX.rend();
            const_reverse_iterator criter =  X.rend();

            for (size_type i = 0; i < LENGTH; ++i)
            {
                LOOP2_ASSERT(LINE, i, TYPE(SPEC[i]) == *iter);
                LOOP2_ASSERT(LINE, i, TYPE(SPEC[i]) == *citer);

                LOOP2_ASSERT(LINE, i, iter    != mX.end());
                LOOP2_ASSERT(LINE, i, citer   !=  X.end());
                LOOP2_ASSERT(LINE, i, riter   != mX.rbegin());
                LOOP2_ASSERT(LINE, i, criter  !=  X.rbegin());
                LOOP2_ASSERT(LINE, i, citer   == iter);
                LOOP2_ASSERT(LINE, i, &*citer == &*iter);
                // C++0x allows comparison of dissimilar reverse_iterators.
                //LOOP2_ASSERT(LINE, i, criter  == riter);

                // Decrement reverse iterator before dereferencing
                --riter;
                --criter;

                // Reverse iterator refers to same element as iterator
                LOOP2_ASSERT(LINE, i, TYPE(SPEC[i]) == *riter);
                LOOP2_ASSERT(LINE, i, TYPE(SPEC[i]) == *criter);
                LOOP2_ASSERT(LINE, i, &*iter == &*riter);
                LOOP2_ASSERT(LINE, i, &*iter == &*criter);

                // iterator copy ctor and assignment
                iterator iter2(iter);     // iterator copy ctor
                iterator iter3(mY.end());
                iter3 = iter;             // iterator assignment

                // const_iterator copy ctor, assignment, and conversion
                const_iterator citer2(citer);   // const_iterator copy ctor
                const_iterator citer3(Y.end());
                citer3 = citer;                 // const_iterator assignment
                const_iterator citer4 = iter;   // conversion ctor
                const_iterator citer5(Y.end());
                citer5 = iter;                  // conversion assignment

                // reverse_iterator conversion, copy ctor, and assignment
                reverse_iterator       riter1(iter);     // conversion ctor
                reverse_iterator       riter2(riter);    // copy ctor
                reverse_iterator       riter3(mY.end());
                riter3 = riter;                          // assignment

                // const_reverse_iterator conversion, copy ctor, and assignment
                const_reverse_iterator criter1(citer);   // rev conversion ctor
                const_reverse_iterator criter3(criter);  // copy ctor
                const_reverse_iterator criter2(riter);   // const conversion
                const_reverse_iterator criter4(Y.end());
                criter4 = criter;                        // assignment
                const_reverse_iterator criter5(Y.end());
                criter5 = riter;                         // const assignment

                // Test equivalences: All of the iterators except for riter1
                // and criter1 refer to the same element
                LOOP2_ASSERT(LINE, i, iter     == iter2);
                LOOP2_ASSERT(LINE, i, iter     == iter3);
                LOOP2_ASSERT(LINE, i, citer    == citer2);
                LOOP2_ASSERT(LINE, i, citer    == citer3);
                LOOP2_ASSERT(LINE, i, citer    == citer4);
                LOOP2_ASSERT(LINE, i, citer    == citer5);
                LOOP2_ASSERT(LINE, i, riter    == riter2);
                LOOP2_ASSERT(LINE, i, riter    == riter3);
                // C++0x allows comparison of dissimilar reverse_iterators.
                //LOOP2_ASSERT(LINE, i, riter    == criter);

                LOOP2_ASSERT(LINE, i, criter   == criter2);
                LOOP2_ASSERT(LINE, i, criter   == criter3);
                LOOP2_ASSERT(LINE, i, criter   == criter4);
                LOOP2_ASSERT(LINE, i, criter   == criter5);

                LOOP2_ASSERT(LINE, i, &*iter   == &*iter2);
                LOOP2_ASSERT(LINE, i, &*citer  == &*citer2);
                LOOP2_ASSERT(LINE, i, &*riter  == &*riter2);
                LOOP2_ASSERT(LINE, i, &*criter == &*criter2);

                // Forward-reverse equivalences
                LOOP2_ASSERT(LINE, i, citer == criter1.base());
                LOOP2_ASSERT(LINE, i, iter  == riter1.base());
                LOOP2_ASSERT(LINE, i, &*iter  == &*--riter1);
                LOOP2_ASSERT(LINE, i, &*citer == &*--criter1);

                // Iterate backwards over the list
                iterator               iback   = mX.end();
                const_iterator         ciback  =  X.end();
                reverse_iterator       riback  = mX.rbegin();
                const_reverse_iterator criback =  X.rbegin();
                for (size_type j = LENGTH; j > 0; ) {
                    --j;
                    --iback;
                    --ciback;

                    LOOP3_ASSERT(LINE, i, j, &*iback == &*ciback);
                    LOOP3_ASSERT(LINE, i, j, &*iback == &*riback);
                    LOOP3_ASSERT(LINE, i, j, &*iback == &*criback);

                    const bool is_eq  = (j == i);
                    const bool is_neq = (j != i);

                    LOOP3_ASSERT(LINE, i, j, is_eq  == (iback   == iter2));
                    LOOP3_ASSERT(LINE, i, j, is_neq == (iback   != iter2));
                    LOOP3_ASSERT(LINE, i, j, is_eq  == (ciback  == citer2));
                    LOOP3_ASSERT(LINE, i, j, is_neq == (ciback  != citer2));
                    LOOP3_ASSERT(LINE, i, j, is_eq  == (riback  == riter2));
                    LOOP3_ASSERT(LINE, i, j, is_neq == (riback  != riter2));
                    LOOP3_ASSERT(LINE, i, j, is_eq  == (criback == criter2));
                    LOOP3_ASSERT(LINE, i, j, is_neq == (criback != criter2));

                    LOOP3_ASSERT(LINE, i, j, is_eq == (&*iback == &*iter2));
                    LOOP3_ASSERT(LINE, i, j, is_eq == (&*iback == &*citer2));
                    LOOP3_ASSERT(LINE, i, j, is_eq == (&*iback == &*riter2));
                    LOOP3_ASSERT(LINE, i, j, is_eq == (&*iback == &*criter2));

                    ++riback;
                    ++criback;
                    // iback and ciback have already been decremented
                }

                LOOP2_ASSERT(LINE, i, X.begin() == iback);
                LOOP2_ASSERT(LINE, i, X.begin() == ciback);
                // C++0x allows comparison of dissimilar reverse_iterators.
                //LOOP2_ASSERT(LINE, i, X.rend()  == riback);
                LOOP2_ASSERT(LINE, i, X.rend()  == criback);

                // Test result of pre and post-increment
                LOOP2_ASSERT(LINE, i, iter2++   ==   iter3);
                LOOP2_ASSERT(LINE, i, iter2     == ++iter3);
                LOOP2_ASSERT(LINE, i, iter2     ==   iter3);
                LOOP2_ASSERT(LINE, i, citer2++  ==   citer3);
                LOOP2_ASSERT(LINE, i, citer2    == ++citer3);
                LOOP2_ASSERT(LINE, i, citer2    ==   citer3);
                LOOP2_ASSERT(LINE, i, riter2++  ==   riter3);
                LOOP2_ASSERT(LINE, i, riter2    == ++riter3);
                LOOP2_ASSERT(LINE, i, riter2    ==   riter3);
                LOOP2_ASSERT(LINE, i, criter2++ ==   criter3);
                LOOP2_ASSERT(LINE, i, criter2   == ++criter3);
                LOOP2_ASSERT(LINE, i, criter2   ==   criter3);

                ++iter;
                ++citer;
                // riter and criter have already been decremented
            } // end for i

            LOOP_ASSERT(LINE, X.end()    == iter);
            LOOP_ASSERT(LINE, X.end()    == citer);
            // C++0x allows comparison of dissimilar reverse_iterators.
            //LOOP_ASSERT(LINE, X.rbegin() == riter);
            LOOP_ASSERT(LINE, X.rbegin() == criter);
        } // end for each spec
    } // end for 'begin', 'end', etc.
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testElementAccess()
{
    // --------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    //
    // Concerns:
    //   1. That 'v.front()' and 'v.back()', allow modifying the
    //      element when 'v' is modifiable, but must not modify the
    //      element when it is 'const'.
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test list with this value, and access the first and
    //   last elements (front, back) both as a modifiable reference (setting
    //   it to a default value, then back to its original value), and as a
    //   non-modifiable reference.
    //
    // Testing:
    //   T& front();
    //   T& back();
    //   const T& front() const;
    //   const T& back() const;
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

            Obj mX(&testAllocator);
            const Obj& X = gg(&mX,SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            if (LENGTH) {
                LOOP_ASSERT(LINE,   is_mutable(mX.front()));
                LOOP_ASSERT(LINE, ! is_mutable(X.front()));
                LOOP_ASSERT(LINE, TYPE(SPEC[0]) == X.front());
                mX.front() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.front());
                LOOP_ASSERT(LINE, Y != X);
                mX.front() = Y.front();
                LOOP_ASSERT(LINE, Y == X);

                LOOP_ASSERT(LINE,   is_mutable(mX.back()));
                LOOP_ASSERT(LINE, ! is_mutable(X.back()));
                LOOP_ASSERT(LINE, TYPE(SPEC[LENGTH - 1]) == X.back());
                mX.back() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.back());
                LOOP_ASSERT(LINE, Y != X);
                mX.back() = Y.back();
                LOOP_ASSERT(LINE, Y == X);
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testResize()
{
    // --------------------------------------------------------------------
    // TESTING 'resize'
    //
    // Concerns:
    //   1. Resized list has the correct value.
    //   2. Resizing to the current size allocates and frees no memory.
    //   3. Resizing to a smaller size allocates no memory.
    //   4. Resizing to a larger size frees no memory.
    //   5. Resizing to a larger size propagates the allocator to elements
    //      appropriately.
    //   6. 'resize' is exception neutral.
    //
    // Plan:
    //   Using a set of input specs and result sizes, try each combination
    //   with and without specifying a value for the new elements.  Verify
    //   each of the above concerns for each combination.
    //
    // Testing:
    //   void resize(size_type n);
    //   void resize(size_type n, const T& val);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    ALLOC                Z(&testAllocator);

    const TYPE           DEFAULT_VALUE = TYPE();
    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

    static const struct {
        int         d_lineNum;  // source line number
        const char *d_spec;     // container spec
    } DATA[] = {
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
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    if (verbose) printf("\nTesting resize(sz).\n");
    for (int i = 0; i < NUM_DATA; ++i)
    {
        const int     LINE   = DATA[i].d_lineNum;
        const char   *SPEC   = DATA[i].d_spec;
        const size_t  LENGTH = strlen(SPEC);

        if (veryVerbose) { T_; P(SPEC); }

        for (size_t newlen = 0; newlen < 20; ++newlen)
        {
            const size_t NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P(NEWLEN); }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                const Int64 AL = testAllocator.allocationLimit();
                testAllocator.setAllocationLimit(-1);

                Obj mX(Z);
                const Obj& X = gg(&mX, SPEC);
                Obj mU(X);  const Obj& U = mU;

                testAllocator.setAllocationLimit(AL);

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64 B  = testAllocator.numBlocksInUse();

                mX.resize(NEWLEN);  // test here

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64 A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                LOOP2_ASSERT(LINE, NEWLEN, checkIntegrity(X, NEWLEN));
                LOOP2_ASSERT(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <= LENGTH) {
                    LOOP2_ASSERT(LINE, NEWLEN, BB == AA);
                }
                else {
                    LOOP2_ASSERT(LINE, NEWLEN,
                                 BB + deltaBlocks(NEWLEN - LENGTH) == AA);
                }

                ptrdiff_t difference = static_cast<ptrdiff_t>(NEWLEN - LENGTH);
                LOOP2_ASSERT(LINE, NEWLEN, B + deltaBlocks(difference) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();
                for (size_t j = 0;
                     j <  LENGTH && j < NEWLEN;
                     ++j, ++xi, ++yi) {
                    LOOP2_ASSERT(LINE, NEWLEN, *yi == *xi);
                }
                for (size_t j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    LOOP2_ASSERT(LINE, NEWLEN, DEFAULT_VALUE == *xi);
                }
                LOOP2_ASSERT(LINE, NEWLEN, xi == X.end());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        } // end for newlen
    } // end testing resize(sz)

    if (verbose) printf("\nTesting resize(sz, c).\n");
    for (int i = 0; i < NUM_DATA; ++i)
    {
        const int     LINE   = DATA[i].d_lineNum;
        const char   *SPEC   = DATA[i].d_spec;
        const size_t  LENGTH = strlen(SPEC);
        const TYPE    VALUE  = VALUES[i % NUM_VALUES];

        if (veryVerbose) { T_; P(SPEC); }

        for (size_t newlen = 0; newlen < 20; ++newlen)
        {
            const size_t NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P(NEWLEN); }

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                const Int64 AL = testAllocator.allocationLimit();
                testAllocator.setAllocationLimit(-1);

                Obj mX(Z);
                const Obj& X = gg(&mX, SPEC);
                Obj mU(X);  const Obj& U = mU;

                testAllocator.setAllocationLimit(AL);

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64 B  = testAllocator.numBlocksInUse();

                mX.resize(NEWLEN, VALUE);  // test here

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64 A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                LOOP2_ASSERT(LINE, NEWLEN, checkIntegrity(X, NEWLEN));
                LOOP2_ASSERT(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <=  LENGTH) {
                    LOOP2_ASSERT(LINE, NEWLEN, BB == AA);
                }
                else {
                    LOOP2_ASSERT(LINE, NEWLEN,
                                 BB + deltaBlocks(NEWLEN - LENGTH) == AA);
                }

                ptrdiff_t difference = static_cast<ptrdiff_t>(NEWLEN - LENGTH);
                LOOP2_ASSERT(LINE, NEWLEN, B + deltaBlocks(difference) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();
                for (size_t j = 0;
                     j < LENGTH && j < NEWLEN;
                     ++j, ++xi, ++yi) {
                    LOOP2_ASSERT(LINE, NEWLEN, *yi == *xi);
                }
                for (size_t j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    LOOP2_ASSERT(LINE, NEWLEN, VALUE == *xi);
                }
                LOOP2_ASSERT(LINE, NEWLEN, xi == X.end());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        } // end for newlen
    } // end testing resize(sz)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testAssign()
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   1. The assigned value is correct.
    //   2. The 'assign' call is exception neutral w.r.t. memory allocation.
    //   3. The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the assignment.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors and equality comparison to verify that
    //   assignment was successful.
    //
    // Testing:
    //   assign(size_type n, const T& value);
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    ALLOC                Z(&testAllocator);

    const TYPE           DEFAULT_VALUE = TYPE();
    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

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
                    printf(" using default value.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                const Obj& X = mX;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    mX.assign(LENGTH, VALUE);
                    const Int64 A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P(X);
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 checkIntegrity(X, LENGTH));
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 A == expectedBlocks(LENGTH));

                    for (const_iterator j = X.begin(); j != X.end(); ++j) {
                        LOOP4_ASSERT(INIT_LINE,LINE, i, ti, VALUE == *j);
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
                    printf(" using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const Int64 AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;

                        testAllocator.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        const Int64 A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            T_; T_; T_; P(X);
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     checkIntegrity(X, LENGTH));
                        LOOP4_ASSERT(INIT_LINE,LINE,i,ti, LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE,LINE,i,ti,
                                     A == expectedBlocks(LENGTH));

                        for (const_iterator j = X.begin(); j != X.end(); ++j) {
                            LOOP4_ASSERT(INIT_LINE,LINE, i, ti, VALUE == *j);
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
void TestDriver<TYPE,ALLOC>::testAssignRange(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //
    // Concerns:
    //   1. That the initial value is correct.
    //   2. That the initial range is correctly imported if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3. That the constructor is exception neutral w.r.t. memory
    //      allocation.
    //   4. That the internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   5. The previous value is freed properly.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value' as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
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
    ALLOC                Z(&testAllocator);

    const TYPE           DEFAULT_VALUE = TYPE();
    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

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
                printf(" using default value.\n");
            }

            Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES], Z);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(SPEC);  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                mX.assign(U.begin(), U.end());
                const Int64 A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                             checkIntegrity(X, LENGTH));
                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                             A == expectedBlocks(LENGTH));

                Obj mY(g(SPEC)); const Obj& Y = mY;
                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, Y == X);
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
                printf(" using default value.\n");
            }

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(SPEC);  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);  const Obj& X = mX;

                    testAllocator.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    const Int64 A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P(X);
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 checkIntegrity(X, LENGTH));
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 A == expectedBlocks(LENGTH));
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, Y == X);
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
void TestDriver<TYPE,ALLOC>::testConstructor()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTORS:
    //
    // Concerns:
    //   1. The initial value is correct.
    //   2. The constructor is exception neutral w.r.t. memory allocation.
    //   3. The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   4. TBD: The C++0x move constructor moves value and allocator
    //      correctly, and without performing any allocation.
    //   5. Constructing a list with 'n' copies of value 'v' selects the
    //      correct overload when 'n' and 'v' are identical arithmetic types
    //      (i.e., the iterator-range overload is not selected).
    //   6. Constructing a list with 'n' copies of value 'v' selects the
    //      correct overload when 'v' is a pointer type and 'n' is a null
    //      pointer literal ,'0'. (i.e., the iterator-range overload is not
    //      selected).
    //
    // Plan:
    //   For the constructor we will create objects of varying sizes with
    //   different 'value' as argument.  Test first with the default value
    //   for type T, and then test with different values.  Perform the above
    //   tests:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //   and use basic accessors to verify
    //    - size
    //    - allocator
    //    - element value at each iterator position { begin() .. end() }.
    //   As for concern 4, we simply move-construct each value into a new
    //   list and check that the value, and allocator are as
    //   expected, and that no allocation was performed.
    //   For concerns 5 and 6, construct a list with 2 elements of arithmetic
    //   or pointer types and verify that it compiles and that the resultant
    //   list contains the expected values.
    //
    // Testing:
    //   list(size_type n, const T& value = T(), const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);

    const TYPE           DEFAULT_VALUE = TYPE();

    const TYPE          *values     = 0;
    const TYPE *const&   VALUES     = values;
    const int            NUM_VALUES = getValues(&values);

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

        if (verbose) printf("\tWithout passing in a value or allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                Obj mX(LENGTH);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P(X);
                }

                LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, ALLOC() == X.get_allocator());

                for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                    LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == nthElem(X,j));
                }
            }
        }

        if (verbose) printf("\tWith passing in a value, without passing in "
                            "an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE       = DATA[ti].d_lineNum;
                const size_t LENGTH     = DATA[ti].d_length;
                const TYPE   VALUE      = VALUES[ti % NUM_VALUES];

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf(" using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P(X);
                }

                LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, ALLOC() == X.get_allocator());

                for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == nthElem(X,j));
                }
            }
        }

        if (verbose) printf("\tWith passing in a value and an allocator.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE       = DATA[ti].d_lineNum;
                const size_t LENGTH     = DATA[ti].d_length;
                const TYPE   VALUE      = VALUES[ti % NUM_VALUES];

                const ALLOC AL(&testAllocator);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf(" using "); P(VALUE);
                }

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                Obj mX(LENGTH, VALUE, AL);
                const Obj& X = mX;

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; P(X);
                }

                LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, AL == X.get_allocator());

                for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == nthElem(X,j));
                }

                LOOP2_ASSERT(LINE, ti, BB + expectedBlocks(LENGTH) == AA);
                LOOP2_ASSERT(LINE, ti, B + expectedBlocks(LENGTH) == A);
            }
        }

        if (verbose)
            printf("\tWith passing a value and an allocator and checking for "
                   "allocation exceptions.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;
                const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                const ALLOC AL(&testAllocator);

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf(" using "); P(VALUE);
                }

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(LENGTH, VALUE, AL);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P(X);
                    }

                    LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                    for (int j = 0; j < static_cast<int>(LENGTH); ++j) {
                        LOOP3_ASSERT(LINE, ti, j, VALUE == nthElem(X,j));
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

                // The number of allocations, 'ALLOCS', needed for successful
                // construction of a list of length 'LENGTH' is
                // 'expectedBlocks(LENGTH)', Because we are retrying on each
                // exception, the number of allocations by the time we succeed
                // will be 'SUM(1 .. ALLOCS)', which is easily computed as
                // 'ALLOCS * (ALLOCS+1) / 2'.

                const Int64 ALLOCS = expectedBlocks(LENGTH);
#ifdef BDE_BUILD_TARGET_EXC
                const Int64 TOTAL_ALLOCS = ALLOCS * (ALLOCS+1) / 2;
#else
                const Int64 TOTAL_ALLOCS = ALLOCS;
#endif
                LOOP2_ASSERT(LINE, ti, BB + TOTAL_ALLOCS == AA);
                LOOP2_ASSERT(LINE, ti,  B + 0            ==  A);

                LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
            }
        }

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

                {
                    const Int64 TB = defaultAllocator_p->numBlocksInUse();
                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    ASSERT(0  == objectAllocator_p->numBlocksInUse());

                    const ALLOC AL(objectAllocator_p);
                    Obj x(LENGTH, DEFAULT_VALUE, AL);

                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    if (TypeHasBslmaAlloc::value && !ObjHasBslmaAlloc::value) {
                        // If TYPE uses bslma but Obj does not, then each
                        // element will allocate one block from the default
                        // allocator.
                        ASSERT(TB + (int) LENGTH ==
                               defaultAllocator_p->numBlocksInUse());
                    }
                    else {
                        // Default allocator is not used
                        ASSERT(TB == defaultAllocator_p->numBlocksInUse());
                    }
                    ASSERT(expectedBlocks(LENGTH) ==
                           objectAllocator_p->numBlocksInUse());
                }

                ASSERT(0 == globalAllocator_p->numBlocksInUse());
                ASSERT(0 == objectAllocator_p->numBlocksInUse());
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
                    printf(" using "); P(VALUE);
                }

                {
                    const Int64 TB = defaultAllocator_p->numBlocksInUse();
                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    ASSERT(0  == objectAllocator_p->numBlocksInUse());

                    const ALLOC AL(objectAllocator_p);
                    Obj x(LENGTH, VALUE, AL);

                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    if (TypeHasBslmaAlloc::value && !ObjHasBslmaAlloc::value) {
                        // If TYPE uses bslma but Obj does not, then each
                        // element will allocate one block from the default
                        // allocator.
                        ASSERT(TB + (int) LENGTH ==
                               defaultAllocator_p->numBlocksInUse());
                    }
                    else {
                        // Default allocator is not used
                        ASSERT(TB == defaultAllocator_p->numBlocksInUse());
                    }
                    ASSERT(expectedBlocks(LENGTH) ==
                           objectAllocator_p->numBlocksInUse());
                }

                ASSERT(0 == globalAllocator_p->numBlocksInUse());
                ASSERT(0 == objectAllocator_p->numBlocksInUse());
            }
        }
    }

    if (verbose) printf("\nTesting overloading disambiguation\n");
    {
        // 'n' and 'v' are identical aritmetic types.  Make sure overload
        // resolution doesn't try to call the iterator-range 'insert'.
        {
            size_t n = 2, v = 99;
            list<size_t, ALLOC> x(n, v);
            list<size_t, ALLOC>& X = x;

            ASSERT(X.size()  == n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
        {
            unsigned char n = 2, v = 99;
            list<IntWrapper, ALLOC> x(n, v);
            list<IntWrapper, ALLOC>& X = x;

            ASSERT(X.size()  == n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
        {
            size_t n = 2;
            int v = 99;
            list<IntWrapper, ALLOC> x(n, v);
            list<IntWrapper, ALLOC>& X = x;

            ASSERT(X.size()  == n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
        {
            // TBD: the below code block causes warnings.
            //float n = 2, v = 99;
            //list<IntWrapper, ALLOC> x(n, v);
            //list<IntWrapper, ALLOC>& X = x;

            //ASSERT(X.size()  == n);
            //ASSERT(X.front() == v);
            //ASSERT(X.back()  == v);
        }

        {
            TestEnum n = TWO, v = NINETYNINE;
            list<IntWrapper, ALLOC> x(n, v);
            list<IntWrapper, ALLOC>& X = x;

            ASSERT(X.size()  == (size_t)n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }

        // 'n' is an 'int' and 'v' is a zero 'int' literal (which is also a
        // null pointer literal).  Make sure that it is correctly treated as a
        // pointer.
        {
            int   n = 2;
            char *v = 0;
            list<char*, ALLOC> x(n, 0);  // Literal null, acts like an int.
            list<char*, ALLOC>& X = x;

            ASSERT(X.size()  == (size_t)n);
            ASSERT(X.front() == v);
            ASSERT(X.back()  == v);
        }
    }
}

template <class TYPE, class ALLOC>
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testConstructorRange(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING RANGE (TEMPLATE) CONSTRUCTORS:
    //
    // Concerns:
    //   1. That the initial value is correct.
    //   2. That the initial range is correctly imported if the
    //      initial 'FWD_ITER' is an input iterator.
    //   3. That the constructor is exception neutral w.r.t. memory
    //      allocation.
    //   4. That the internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   We will create objects of varying sizes containing
    //   default values, and insert a range containing distinct values as
    //   argument.  Perform the above tests:
    //    - From the parameterized 'CONTAINER::const_iterator'.
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors to verify
    //      - size
    //      - element value at each index position { 0 .. length - 1 }.
    //
    // Testing:
    //   template <class InputIter>
    //     list(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const int INPUT_ITERATOR_TAG =
          bsl::is_same<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::value;
    (void) INPUT_ITERATOR_TAG;

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
                printf(" using "); P(SPEC);
            }

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P(X);
            }

            LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

            Obj mY(g(SPEC));  const Obj& Y = mY;
            LOOP2_ASSERT(LINE, ti, Y == X);
        }
    }

    if (verbose) printf("\tWith passing in an allocator.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) { printf("\t\tCreating object "); P(SPEC); }

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64  B = testAllocator.numBlocksInUse();

            ALLOC AL(&testAllocator);
            Obj mX(U.begin(), U.end(), AL); const Obj& X = mX;

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P(X);
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

            Obj mY(g(SPEC));  const Obj& Y = mY;
            LOOP2_ASSERT(LINE, ti, Y == X);

            LOOP2_ASSERT(LINE, ti, BB + expectedBlocks(LENGTH) == AA);
            LOOP2_ASSERT(LINE, ti, B + expectedBlocks(LENGTH) == A);
        }
    }

    if (verbose) printf("\tWith passing an allocator and checking for "
                        "allocation exceptions.\n");
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const size_t  LENGTH = strlen(SPEC);

            if (verbose) {
                printf("\t\tCreating object of "); P_(LENGTH);
                printf(" using "); P(SPEC);
            }

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;
            Obj mY(g(SPEC));     const Obj& Y = mY;

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64  B = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                ALLOC AL(&testAllocator);
                Obj mX(U.begin(), U.end(), AL); const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P(X);
                }

                LOOP2_ASSERT(LINE, ti, checkIntegrity(X, LENGTH));
                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64  A = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            // The number of allocations, 'ALLOCS', needed for successful
            // construction of a list of length 'LENGTH' is
            // 'expectedBlocks(LENGTH)'.  Because we are retrying on each
            // exception, the number of allocations by the time we succeed
            // will be 'SUM(1 .. ALLOCS)', which is easily computed as 'ALLOCS
            // * (ALLOCS+1) / 2'.

            const Int64 ALLOCS = expectedBlocks(LENGTH);
#ifdef BDE_BUILD_TARGET_EXC
            const Int64 TOTAL_ALLOCS = ALLOCS * (ALLOCS+1) / 2;
#else
            const Int64 TOTAL_ALLOCS = ALLOCS;
#endif
            LOOP2_ASSERT(LINE, ti, BB + TOTAL_ALLOCS == AA);
            LOOP2_ASSERT(LINE, ti, B + 0 == A);

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testAllocator(bsl::true_type,
                                           const char *t, const char *a)
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // This template specialization is for containers that use bslma_Allocator.
    //
    // Concerns:
    //   1. The list class has the 'bslma::UsesBslmaAllocator'
    //      trait.
    //   2. The allocator is not copied when the list is copy-constructed.
    //   3. The allocator is set with the extended copy-constructor.
    //   4. The allocator is passed through to elements if the elements
    //      also use bslma_Allocator.
    //   5. Creating an empty list allocates exactly one block.
    //   6. 'get_allocator' returns the allocator used to construct the
    //      list object.
    //
    // Testing:
    //   allocator_type get_allocator() const;
    //   Allocator traits
    //   Allocator propagation
    // --------------------------------------------------------------------

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT(ObjHasBslmaAlloc::value);

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    if (verbose)
        printf("\nTesting 'bslma::UsesBslmaAllocator'.\n");

    LOOP2_ASSERT(t, a, bslma::UsesBslmaAllocator<Obj>::value);

    if (verbose)
        printf("\nTesting that empty list allocates one block.\n");
    {
        const Int64 BB = testAllocator.numBlocksTotal();
        Obj mX(&testAllocator);
        LOOP2_ASSERT(t, a, BB + 1 == testAllocator.numBlocksTotal());
        LOOP2_ASSERT(t, a, 1 == testAllocator.numBlocksInUse());
    }

    if (verbose)
        printf("\nTesting that allocator propagation for "
               "copy construction.\n");
    {
        // Normal copy constructor
        Obj mX(&testAllocator);  const Obj& X = mX;
        Obj mY(X);               const Obj& Y = mY;

        // Allocator not copied
        LOOP2_ASSERT(t, a, &testAllocator == X.get_allocator());
        LOOP2_ASSERT(t, a, &testAllocator != Y.get_allocator());

        // Extended copy constructor
        bslma::TestAllocator a2;
        Obj mZ(X,&a2);           const Obj& Z = mZ;

        // Allocator set to a2 (not copied)
        LOOP2_ASSERT(t, a, &testAllocator != Z.get_allocator());
        LOOP2_ASSERT(t, a, &a2            == Z.get_allocator());
    }

    if (verbose)
        printf("\nTesting passing allocator through to elements.\n");

    const Int64 DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslma::UsesBslmaAllocator<TYPE>::value)
    {
        {
            Obj mX(1, VALUES[0], &testAllocator);  const Obj& X = mX;
            LOOP2_ASSERT(t, a, &testAllocator == X.back().allocator());
            LOOP2_ASSERT(t, a, 3 == testAllocator.numBlocksInUse());
        }
        {
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            LOOP2_ASSERT(t, a, &testAllocator == X.back().allocator());
            LOOP2_ASSERT(t, a, 3 == testAllocator.numBlocksInUse());
        }
    }
    else
    {
        {
            Obj mX(1, VALUES[0], &testAllocator);  const Obj& X = mX;
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &OtherAllocatorDefaultImp == elemAlloc);
            LOOP2_ASSERT(t, a, DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &OtherAllocatorDefaultImp == elemAlloc);
            LOOP2_ASSERT(t, a, DD + 1 == elemAlloc->numBlocksInUse());
        }
    }

    LOOP2_ASSERT(t, a, 0 == testAllocator.numBytesInUse());
    LOOP2_ASSERT(t, a, DD == OtherAllocatorDefaultImp.numBytesInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testAllocator(bsl::false_type,
                                           const char *t, const char *a)
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS FOR NON-BSLMA ALLOCATORS
    //
    // This template specialization is for containers that use non-bslma
    // Allocators.
    //
    // Concerns:
    //   1. The list class does not have the
    //      'bslma::UsesBslmaAllocator' trait.
    //   2. The allocator is not passed through to elements
    //   3. The allocator is set with the extended copy-constructor.
    //   4. The allocator is copied when the list is copy-constructed.
    //   5. Creating an empty list allocates exactly one block.
    //   6. 'get_allocator' returns the allocator used to construct the
    //      list object.
    //
    // Testing:
    //   allocator_type get_allocator() const;
    //   Allocator traits
    //   Allocator propagation
    //   Allocator traits and propagation
    // --------------------------------------------------------------------

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT( !ObjHasBslmaAlloc::value );

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    OtherAllocator<char> objAllocator(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) NUM_VALUES;

    if (verbose)
        printf("\nTesting 'bslma::UsesBslmaAllocator'.\n");

    LOOP2_ASSERT(t, a, ! bslma::UsesBslmaAllocator<Obj>::value);

    if (verbose)
        printf("\nTesting that empty list allocates one block.\n");
    {
        const Int64 BB = testAllocator.numBlocksTotal();
        Obj mX(objAllocator);
        LOOP2_ASSERT(t, a, BB + 1 == testAllocator.numBlocksTotal());
        LOOP2_ASSERT(t, a, 1 == testAllocator.numBlocksInUse());
    }

    if (verbose)
        printf("\nTesting that allocator propagation for "
               "copy construction.\n");
    {
        // Normal copy constructor
        Obj mX(objAllocator);    const Obj& X = mX;
        Obj mY(X);               const Obj& Y = mY;

        // Allocator copied
        LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
        LOOP2_ASSERT(t, a, objAllocator == Y.get_allocator());

        // Extended copy constructor
        bslma::TestAllocator  a2;
        OtherAllocator<char> objAlloc2(&a2);
        Obj mZ(X,objAlloc2);     const Obj& Z = mZ;

        // Allocator set to objAlloc2 (not copied)
        LOOP2_ASSERT(t, a, objAllocator   != Z.get_allocator());
        LOOP2_ASSERT(t, a, objAlloc2      == Z.get_allocator());
    }

    if (verbose)
       printf("\nTesting that allocator is not passed through to elements.\n");

    const Int64 DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslma::UsesBslmaAllocator<TYPE>::value)
    {
        // Elements in container should use default allocator while the
        // container itself uses 'testAllocator'.  Set the default allocator
        // here so that we can track its use.
        bslma::TestAllocator defAlloc(veryVeryVerbose);
        bslma::DefaultAllocatorGuard g(&defAlloc);

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &defAlloc == elemAlloc);
            LOOP2_ASSERT(t, a, 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &defAlloc == elemAlloc);
            LOOP2_ASSERT(t, a, 1 == elemAlloc->numBlocksInUse());
        }

        LOOP2_ASSERT(t, a, 0 == defAlloc.numBlocksInUse());
    }
    else
    {
        // Elements in container should use 'OtherAllocator::d_defaultImp'
        // while the container itself uses 'testAllocator'.

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &OtherAllocatorDefaultImp == elemAlloc);
            LOOP2_ASSERT(t, a, DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma::TestAllocator *const elemAlloc =
                dynamic_cast<bslma::TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &OtherAllocatorDefaultImp == elemAlloc);
            LOOP2_ASSERT(t, a, DD + 1 == elemAlloc->numBlocksInUse());
        }
    }

    LOOP2_ASSERT(t, a, 0 == testAllocator.numBytesInUse());
    LOOP2_ASSERT(t, a, DD == OtherAllocatorDefaultImp.numBytesInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testAllocator(const char *t, const char *a)
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // This template specialization is for containers that use non-bslma
    // Allocators.
    //
    // Concerns:
    //
    //   For ALLOC that is a bslma_Allocator
    //   1. The list class has the 'bslma::UsesBslmaAllocator'
    //      trait.
    //   2. The allocator is not copied when the list is copy-constructed.
    //   3. The allocator is set with the extended copy-constructor.
    //   4. The allocator is passed through to elements if the elements
    //      also use bslma_Allocator.
    //   5. Creating an empty list allocates exactly one block.
    //
    //   For ALLOC that is not a bslma allocator
    //   1. The list class does not have the
    //      'bslma::UsesBslmaAllocator' trait.
    //   2. The allocator is not passed through to elements
    //   3. The allocator is set with the extended copy-constructor.
    //   4. The allocator is copied when the list is copy-constructed.
    //   5. Creating an empty list allocates exactly one block.
    //
    // Testing:
    //   Allocator traits and propagation
    // --------------------------------------------------------------------

    // Dispatch to the appropriate function
    testAllocator(ObjHasBslmaAlloc(), t, a);
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testAssignmentOp()
{
    // --------------------------------------------------------------------
    // TESTING ASSIGNMENT OPERATOR:
    //
    // Concerns:
    //   1. The 'rhs' value must not be affected by the operation.
    //   2. 'rhs' going out of scope has no effect on the value of 'lhs'
    //      after the assignment.
    //   3. After the assignment, no element of the 'lhs' has the same
    //      address an element of 'rhs' (except in the case of
    //      self-assignment).
    //   4. Aliasing (x = x): The assignment operator must always work --
    //        even when the lhs and rhs are identically the same object.
    //   5. The assignment operator must be neutral with respect to memory
    //      allocation exceptions.
    //   6. The 'lhs' object must allocate all of its internal memory from
    //      its own allocator, even of 'rhs' uses a different allocator.
    //   7. TBD: C++0x move assignment moves the value but not the allocator.
    //      Memory is allocated only if the 'lhs' and 'rhs' allocators are
    //      different.
    //
    // Plan:
    //   Specify a set S of unique object values with substantial and varied
    //   differences, ordered by increasing length.  For each value in S,
    //   construct an object x along with a sequence of similarly constructed
    //   duplicates x1, x2, ..., xN. The elements within each object in S are
    //   unique so that re-ordering elements cannot preserve equality.
    //   Attempt to affect every aspect of white-box state by altering each xi
    //   in a unique way.  Let the union of all such objects be the set T.
    //
    //   To address concerns 2, 3, and 5, construct tests u = v for all (u, v)
    //   in T X T.  Using canonical controls UU and VV, assert before the
    //   assignment that UU == u, VV == v, and v == u if and only if VV == UU.
    //   After the assignment, assert that VV == u, VV == v, and, for grins,
    //   that v == u.  Verify that each element in v has a different address
    //   from the corresponding element in v.  Let v go out of scope and
    //   confirm that VV == u.  All of these tests are performed within the
    //   'bslma' exception testing apparatus.  Since the execution time is
    //   lengthy with exceptions, not every permutation is performed when
    //   exceptions are tested.  Every permutation is also tested separately
    //   without exceptions.
    //
    //   As a separate exercise, we address 4 and 5 by constructing tests
    //   y = y for all y in T.  Using a canonical control X, we will verify
    //   that X == y before and after the assignment, again within
    //   the bslma exception testing apparatus.
    //
    //   To address concern 5, all these tests are performed on user
    //   defined types:
    //          With allocator, copyable
    //          With allocator, moveable
    //          With allocator, not moveable
    //
    // Testing:
    //   list& operator=(const list& rhs);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void) VALUES;
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
            "DEA",  // Try equal-size assignment of different values
            "DEAB",
            "BACEDEDC",
            0 // null string required as last element
        };

        {
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const size_t      uLen   = strlen(U_SPEC);

                if (verbose) {
                    printf("\tFor lhs objects of length " ZU ":\t", uLen);
                    P(U_SPEC);
                }

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

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

                    ALLOC AL2(&testAllocator2);
                    Obj mU(AL2);
                    const Obj& U = gg(&mU, U_SPEC);
                    {
                        ALLOC AL1(&testAllocator1);
                        Obj mV(AL1);
                        const Obj& V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, Z==(V==U));

                        const int NUM_CTOR_BEFORE = numCopyCtorCalls;
                        const int NUM_DTOR_BEFORE = numDestructorCalls;
                        const size_t U_LEN_BEFORE = U.size();

                        const Int64 BB1 = testAllocator1.numBlocksTotal();
                        const Int64 B1  = testAllocator1.numBlocksInUse();
                        const Int64 BB2 = testAllocator2.numBlocksTotal();
                        const Int64 B2  = testAllocator2.numBlocksInUse();

                        mU = V; // test assignment here

                        const Int64 AA1 = testAllocator1.numBlocksTotal();
                        const Int64 A1  = testAllocator1.numBlocksInUse();
                        const Int64 AA2 = testAllocator2.numBlocksTotal();
                        const Int64 A2  = testAllocator2.numBlocksInUse();

                        // The assignment may construct as many as V.size()
                        // objects and may destroy as many as U.size()
                        // objects, but could (through the use of
                        // element-by-element assignment) construct and
                        // destroy fewer elements.
                        const int NUM_CTOR =
                            numCopyCtorCalls - NUM_CTOR_BEFORE;
                        const int NUM_DTOR =
                            numDestructorCalls - NUM_DTOR_BEFORE;
                        ASSERT(NUM_CTOR <= (int)V.size());
                        ASSERT(NUM_DTOR <= (int)U_LEN_BEFORE);

                        LOOP2_ASSERT(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  V == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, BB1 == AA1);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, B1  == A1 );
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                     BB2 + deltaBlocks(vLen) >= AA2);
                        ptrdiff_t difference =
                                           static_cast<ptrdiff_t>(vLen - uLen);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);
                        for (const_iterator iu = U.begin(), iv = V.begin();
                             iu != U.end(); ++iu, ++iv) {
                            // Verify that U and V have no elements in common
                            LOOP3_ASSERT(U_SPEC, V_SPEC, *iu, &*iv != &*iu);
                        }
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                } // end for (vi)
            } // end for (ui)
        }
    }

    if (verbose) printf("\nAssign cross product of values With Exceptions\n");
    {
        static const char *SPECS[] = { // len: 0-2, 4, 4, 9,
            "",        "A",    "BC",     "DEAB",    "CBEA",    "BACEDEDCB",
            0
        }; // Null string required as last element.

        for (int ui = 0; SPECS[ui]; ++ui) {
            const char *const U_SPEC = SPECS[ui];
            const size_t      uLen   = strlen(U_SPEC);

            if (verbose) {
                printf("\tFor lhs objects of length " ZU ":\t", uLen);
                P(U_SPEC);
            }

            const Obj UU = g(U_SPEC);  // control
            LOOP_ASSERT(ui, uLen == UU.size()); // same lengths

            // int vOldLen = -1;
            for (int vi = 0; SPECS[vi]; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t      vLen   = strlen(V_SPEC);

                if (veryVerbose) {
                    printf("\t\tFor rhs objects of length " ZU ":\t", vLen);
                    P(V_SPEC);
                }

                // control
                const Obj VV = g(V_SPEC);

                // Exception-test macros muse use 'testAllocator':
                bslma::TestAllocator& testAllocator = testAllocator2;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off
                    // the limit until we're ready to test assignment.
                    const Int64 AL = testAllocator.allocationLimit();
                    testAllocator2.setAllocationLimit(-1);

                    ALLOC AL2(&testAllocator2);
                    Obj mU(AL2);
                    const Obj& U = gg(&mU, U_SPEC);
                    {
                        ALLOC AL1(&testAllocator1);
                        Obj mV(AL1);
                        const Obj& V = gg(&mV, V_SPEC);

                        if (veryVerbose) {
                            printf("\t| "); P_(U); P(V);
                        }

                        const Int64 BB2 = testAllocator2.numBlocksTotal();
                        const Int64 B2  = testAllocator2.numBlocksInUse();

                        testAllocator2.setAllocationLimit(AL);
                        mU = V; // test assignment here

                        const Int64 AA2 = testAllocator2.numBlocksTotal();
                        const Int64 A2  = testAllocator2.numBlocksInUse();

                        LOOP2_ASSERT(U_SPEC, V_SPEC, checkIntegrity(U, vLen));
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  V == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                     BB2 + deltaBlocks(vLen) >= AA2);
                        ptrdiff_t difference =
                                             static_cast<ptrdiff_t>(vLen-uLen);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                           B2 + deltaBlocks(difference) == A2);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            } // end for (vi)
        } // end for (ui)
    } // end exception test

    if (verbose) printf("\nTesting self assignment (Aliasing).");
    {
        static const char *SPECS[] = {
            "",      "A",      "BC",     "CDE",    "DEAB",   "EABCD",
            "ABCDEAB",         "ABCDEABC",         "ABCDEABCD",
            "ABCDEABCDEABCDE", "ABCDEABCDEABCDEA", "ABCDEABCDEABCDEAB",
            0 // null string required as last element
        };

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

            // Exception-test macros muse use 'testAllocator':
            bslma::TestAllocator& testAllocator = testAllocator2;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                // We want to use the allocation limit only for the
                // assignment operation, not for producing the initial
                // objects.  Thus, we save the limit in AL and turn off
                // the limit until we're ready to test assignment.
                const Int64 AL = testAllocator.allocationLimit();
                testAllocator2.setAllocationLimit(-1);

                ALLOC AL2(&testAllocator2);
                Obj mY(AL2);
                const Obj& Y = gg(&mY, SPEC);

                if (veryVerbose) { T_; T_; P(Y); }

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);

                const Int64 B2 = testAllocator2.numBlocksInUse();

                testAllocator2.setAllocationLimit(AL);
                {
                    ExceptionGuard<Obj> guard(&mY, Y, L_);
                    mY = Y; // test assignment here
                }

                const Int64 A2 = testAllocator2.numBlocksInUse();

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);
                LOOP_ASSERT(SPEC, B2 == A2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        } // end for (ti)
    } // end self-assignment test
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testGeneratorG()
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
    //   list g(const char *spec);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

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

        Obj mX(Z);
        const Obj& X = gg(&mX, SPEC);

        if (veryVerbose) {
            printf("\t g = "); dbg_print(g(SPEC)); printf("\n");
            printf("\tgg = "); dbg_print(X); printf("\n");
        }
        const Int64 TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
        const Int64 IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, X == g(SPEC));
        const Int64 TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
        const Int64 IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");
    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

        Obj x(Z);                      // runtime tests
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
void TestDriver<TYPE,ALLOC>::testCopyCtor()
{
    // --------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //
    // Concerns:
    //   1. The new object's value is the same as that of the original
    //      object (relying on the equality operator) and created with
    //      the correct allocator.
    //   2. The value of the original object is left unaffected.
    //   3. Subsequent changes in or destruction of the source object have
    //      no effect on the copy-constructed object.
    //   4. Subsequent changes ('push_back's) on the created object have no
    //      effect on the original.
    //   5. The object has its internal memory management system hooked up
    //      properly so that *all* internally allocated memory draws
    //      from a user-supplied allocator whenever one is specified.
    //   6. The function is exception neutral w.r.t. memory allocation.
    //
    // Plan:
    //   Specify a set S of object values with substantial and varied
    //   differences, ordered by increasing length, to be used in the
    //   following tests.
    //
    //   For concerns 1 - 3, for each value in S, initialize objects w and
    //   x, copy construct y from x and use 'operator==' to verify that
    //   both x and y subsequently have the same value as w.  Let x go out
    //   of scope and again verify that w == y.
    //
    //   For concern 4, for each value in S initialize objects w and x,
    //   and copy construct y from x.  Change the state of y, by using the
    //   *primary* *manipulator* 'push_back'.  Using the 'operator!=' verify
    //   that y differs from x and w.
    //
    //   To address concern 5, we will perform tests performed for concern 1:
    //     - While passing a testAllocator as a parameter to the new object
    //       and ascertaining that the new object gets its memory from the
    //       provided testAllocator.  Also perform test for concerns 2 and 4.
    //     - Where the object is constructed with an object allocator, and
    //       neither of global and default allocator is used to supply memory.
    //
    //   To address concern 6, perform tests for concern 1 performed
    //   in the presence of exceptions during memory allocations using a
    //   'bslma_TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   list(const list& original, const A& = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    if (verbose)
        printf("\nTesting parameters: SCOPED_ALLOC = %d.\n", SCOPED_ALLOC);
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

            Obj *pX = new Obj(Z);
            Obj& mX = *pX;

            const Obj& X = mX;  gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                const Obj Y0(X);

                if (veryVerbose) {
                    printf("\tObj : "); P(Y0);
                }

                LOOP_ASSERT(SPEC, checkIntegrity(Y0, LENGTH));
                LOOP_ASSERT(SPEC, W == Y0);
                LOOP_ASSERT(SPEC, W == X);
                if (ObjHasBslmaAlloc::value) {
                    LOOP_ASSERT(SPEC, Y0.get_allocator() == ALLOC());
                }
                else {
                    LOOP_ASSERT(SPEC, Y0.get_allocator() == X.get_allocator());
                }
            }
            {   // Testing concern 4.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                Y1.push_back(VALUES[Y1.size() % NUM_VALUES]);

                LOOP_ASSERT(SPEC, checkIntegrity(Y1, LENGTH + 1));
                LOOP_ASSERT(SPEC, Y1.size() == LENGTH + 1);
                LOOP_ASSERT(SPEC, W != Y1);
                LOOP_ASSERT(SPEC, X != Y1);
                LOOP_ASSERT(SPEC, W == X);
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, Z);

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                LOOP_ASSERT(SPEC, BB + expectedBlocks(LENGTH) == AA);
                LOOP_ASSERT(SPEC,  B + expectedBlocks(LENGTH) ==  A);

                const Int64 CC = testAllocator.numBlocksTotal();
                const Int64  C = testAllocator.numBlocksInUse();

                Y11.push_back(VALUES[LENGTH % NUM_VALUES]);

                const Int64 DD = testAllocator.numBlocksTotal();
                const Int64  D = testAllocator.numBlocksInUse();

                // Allocations should increase by one node block for the list.
                // If TYPE uses an allocator, allocations should increase by
                // one more block.
                LOOP_ASSERT(SPEC, CC + deltaBlocks(1) == DD);
                LOOP_ASSERT(SPEC, C  + deltaBlocks(1) == D );

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append : ");
                    P(Y11);
                }

                LOOP_ASSERT(SPEC, Y11.size() == LENGTH + 1);
                LOOP_ASSERT(SPEC, W != Y11);
                LOOP_ASSERT(SPEC, X != Y11);
                LOOP_ASSERT(SPEC, Y11.get_allocator() == X.get_allocator());
                LOOP_ASSERT(SPEC, X == W);
            }
#ifdef BDE_BUILD_TARGET_EXC
            {   // Exception checking.

                const Int64 BB = testAllocator.numBlocksTotal();
                const Int64  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    allocations += bslmaExceptionCounter;
                    const Obj Y2(X, Z);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    LOOP_ASSERT(SPEC, checkIntegrity(Y2, LENGTH));
                    LOOP_ASSERT(SPEC, W == Y2);
                    LOOP_ASSERT(SPEC, W == X);
                    LOOP_ASSERT(SPEC, Y2.get_allocator() == X.get_allocator());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const Int64 AA = testAllocator.numBlocksTotal();
                const Int64  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                LOOP_ASSERT(SPEC, BB + (int) allocations == AA);
                LOOP_ASSERT(SPEC,  B + 0 ==  A);
            }
#endif // BDE_BUILD_TARGET_EXC
            {                            // with 'original' destroyed
                Obj Y5(X);
                if (veryVerbose) {
                    printf("\t\t\tWith Original deleted: \n");
                    printf("\t\t\t\tBefore Delete : "); P(Y5);
                }

                delete pX;

                LOOP_ASSERT(SPEC, W == Y5);

                Y5.push_back(VALUES[LENGTH % NUM_VALUES]);
                if (veryVerbose) {
                    printf("\t\t\t\tAfter Append to new obj : ");
                    P(Y5);
                }
                LOOP_ASSERT(SPEC, checkIntegrity(Y5, LENGTH + 1));
                LOOP_ASSERT(SPEC, W != Y5);
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testEqualityOp()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    //
    // Concerns:
    //   1. Objects constructed with the same values compare equal.
    //   2. Objects constructed such that they have same (logical) value but
    //      different internal representation (due to the lack or presence
    //      of an allocator) always compare as equal.
    //   3. Unequal objects are always compare as unequal.
    //
    // Plan:
    //   Specify a set A of unique allocators including no allocator.  Specify
    //   a set S of unique object values having various minor or subtle
    //   differences, ordered by non-decreasing length.  Verify the
    //   correctness of 'operator==' and 'operator!=' (returning either true
    //   or false) using all elements (u, ua, v, va) of the cross product S X
    //   A X S X A.
    //
    // Testing:
    //   operator==(const list<T,A>&, const list<T,A>&);
    //   operator!=(const list<T,A>&, const list<T,A>&);
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
    (void) VALUES;
    (void) NUM_VALUES;

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
    int oldLen = -1;

    // Create first object
    for (int si = 0; SPECS[si]; ++si) {
        for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

            const char *const U_SPEC = SPECS[si];
            const int         LENGTH = static_cast<int>(strlen(U_SPEC));

            Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
            LOOP2_ASSERT(si, ai,
                         LENGTH == static_cast<int>(U.size())); // same lengths

            if ((int)LENGTH != oldLen) {
                if (verbose)
                    printf("\tUsing lhs objects of length %d.\n", LENGTH);
                LOOP_ASSERT(U_SPEC, oldLen <= LENGTH);  // non-decreasing
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
                } // end for (each allocator for V)
            } // end for (each spec for V)
        } // end for (each allocator for U)
    } // end for (each spec for U)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testBasicAccessors()
{
    // --------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    //
    // Concerns:
    //   0. size() returns the list size.
    //   1. begin() == end() if and only if the list is empty
    //   2. Iterating from begin() to end() will visit every value in a
    //      list and only the values in that list.
    //   3. Iteration works for both const containers (using const_iterator)
    //      and non-const containers (using iterator).
    //   4. empty() returns true iff size() return 0
    //   5. The test function 'succ' increments an iterator by n.
    //   6. The test function 'nthElem' returns the nth element of a list.
    //   7. The test function 'is_mutable' returns true iff its argument is a
    //      mutable lvalue.
    //   NOTE: This is not a thorough test of iterators.  This test is only
    //   sufficient for using iterators to access the contents of a list in
    //   order.
    //
    // Plan:
    //   Specify a set S of representative object values ordered by
    //   increasing length.  For each value w in S, initialize a newly
    //   constructed object x with w using 'gg' and verify that each basic
    //   accessor returns the expected result.
    //
    // Testing:
    //   int size() const;
    //   bool empty() const;
    //   iterator begin();
    //   iterator end();
    //   const_iterator begin() const;
    //   const_iterator end() const;
    //
    //   iterator succ(iterator);
    //   const_iterator succ(iterator) const;
    //   T& nthElem(list& x, int n);
    //   const T& nthElem(list& x, int n) const;
    //   bool is_mutable(T& val);
    //   bool is_mutable(const T& val);
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
    (void) NUM_VALUES;

    const int MAX_LENGTH = 32;

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

    if (verbose) printf("\nTesting is_mutable.\n");
    {
        TYPE mV;
        const TYPE CV = TYPE();
        TYPE& mVref = mV;
        const TYPE& cmVref = mV;

        ASSERT(  is_mutable(mV));
        ASSERT(! is_mutable(CV));
        ASSERT(  is_mutable(mVref));
        ASSERT(! is_mutable(cmVref));
        ASSERT(! is_mutable(as_rvalue(VALUES[0]))); // rvalue is not mutable
    }

    if (verbose) printf("\nTesting const and non-const versions of "
                        "begin() and end().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         LENGTH = DATA[ti].d_length;
            const char *const EXP    = DATA[ti].d_elements;

            ASSERT(LENGTH <= MAX_LENGTH);

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                const ALLOC AL(ALLOCATOR[ai]);
                Obj mX(AL);

                const Obj& X = gg(&mX, SPEC);    // canonical organization

                LOOP2_ASSERT(ti, ai,
                         LENGTH == static_cast<int>(X.size())); // same lengths
                LOOP2_ASSERT(ti, ai, (LENGTH == 0) == X.empty());

                if (veryVerbose) {
                    printf( "\ton objects of length %d:\n", LENGTH);
                }

                // non-decreasing
                LOOP2_ASSERT(LINE, ai, oldLen <= LENGTH);
                oldLen = LENGTH;

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                int i;
                iterator imX;
                const_iterator iX;
                for (i = 0, imX = mX.begin(), iX = X.begin(); i < LENGTH;
                     ++i, ++imX, ++iX) {
                    LOOP3_ASSERT(LINE, ai, i, iX  != X.end());
                    LOOP3_ASSERT(LINE, ai, i, imX != mX.end());
                    LOOP3_ASSERT(LINE, ai, i, imX == iX);
                    LOOP3_ASSERT(LINE, ai, i, TYPE(EXP[i]) == *iX);
                    LOOP3_ASSERT(LINE, ai, i, TYPE(EXP[i]) == *imX);
                    LOOP3_ASSERT(LINE, ai, i, succ(X.begin(),i) == iX);
                    LOOP3_ASSERT(LINE, ai, i, succ(mX.begin(),i) == imX);
                    LOOP3_ASSERT(LINE, ai, i, &nthElem(X,i) == &*iX);
                    LOOP3_ASSERT(LINE, ai, i, &nthElem(mX,i) == &*imX);
                }
                LOOP3_ASSERT(LINE, ai, i, iX  == X.end());
                LOOP3_ASSERT(LINE, ai, i, imX == mX.end());

                // Sanity check that the test driver doesn't have unused data:
                for (; i < MAX_LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, 0 == EXP[i]);
                }
            }
        }
    }

    if (verbose) printf("\nTesting non-const iterators "
                        "modify state of object correctly.\n");
    {

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE         = DATA[ti].d_lineNum;
            const char *const SPEC = DATA[ti].d_spec_p;
            const size_t    LENGTH  = DATA[ti].d_length;
            const char *const e = DATA[ti].d_elements;

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                const ALLOC AL(ALLOCATOR[ai]);
                Obj mX(AL);

                const Obj& X = gg(&mX, SPEC);

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths
                LOOP2_ASSERT(ti, ai, (LENGTH == 0) == X.empty());

                if (veryVerbose) {
                    printf("\tOn objects of length " ZU ":\n", LENGTH);
                }

                // non-decreasing
                LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                oldLen = static_cast<int>(LENGTH);

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                Obj mY(AL); const Obj& Y = mY;

                for (size_t j = 0; j < LENGTH; j++) {
                    mY.push_back(TYPE());
                }

                // Change state of Y so its same as X
                size_t j = 0;
                for (iterator imY = mY.begin(); imY != mY.end(); ++imY, ++j) {
                    *imY = TYPE(e[j]);
                }
                LOOP2_ASSERT(ti, ai, LENGTH == j);

                if (veryVerbose) {
                    printf("\t\tNew object1: "); P(Y);
                }

                const_iterator iX;
                const_iterator iY;
                for (j = 0, iX = X.begin(), iY = Y.begin(); iX != X.end();
                     ++j, ++iX, ++iY)
                {
                    LOOP3_ASSERT(ti, ai, j, *iY == *iX);
                }
                LOOP2_ASSERT(ti, ai, iY == Y.end());

                // Just for kicks, use the (untested) operator==
                LOOP2_ASSERT(ti, ai, Y == X);
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testGeneratorGG()
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
    //   character (denoting 'clear').
    //
    // Testing:
    //   list<T,A>& gg(list<T,A> *object, const char *spec);
    //   int ggg(list<T,A> *object, const char *spec, int vF = 1);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

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
            const size_t    LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;
            const int         curLen = (int)strlen(SPEC);

            Obj mX(Z);
            const Obj& X = gg(&mX, SPEC);   // original spec

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj mY(Z);
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
            const_iterator ix = X.begin();
            const_iterator iy = Y.begin();
            for (size_t i = 0; i < LENGTH; ++i, ++ix, ++iy) {
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == *ix);
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == *iy);
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
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(Z);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                // LOOP_ASSERT(LINE, oldLen <= (int)LENGTH);  // non-decreasing
                oldLen = static_cast<int>(LENGTH);
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int result = ggg(&mX, SPEC, veryVerbose);

            LOOP_ASSERT(LINE, INDEX == result);
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testPrimaryManipulators()
{
    // --------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //
    // Concerns:
    //   The basic concern is that the default constructor, the destructor,
    //   and, under normal conditions (i.e., no aliasing), the primary
    //   manipulators
    //      - push_back             (black-box)
    //      - clear                 (white-box)
    //   operate as expected.  Specifically:
    //   1. The default constructor
    //      1a) creates the correct initial value.
    //      1b) allocates exactly one block.
    //   2. The destructor properly deallocates all allocated memory to
    //      its corresponding allocator from any attainable state.
    //   3. 'push_back'
    //      3a) produces the expected value.
    //      3b) maintains valid internal state.
    //      3c) preserves the strong exception guarantee and is
    //          exception-neutral wrt memory allocation.
    //      3d) does not change the address of any other list elements
    //      3e) has the internal memory management system hooked up
    //          properly so that *all* internally allocated memory
    //          draws from the same user-supplied allocator whenever
    //          one is specified.
    //   4. 'clear'
    //      4a) produces the expected value (empty).
    //      4b) properly destroys each contained element value.
    //      4c) maintains valid internal state.
    //      4d) does not allocate memory.
    //      4e) deallocates all element memory
    //   5. The size-based parameters of the class reflect the platform.
    //
    // Plan:
    //   To address concerns 1a - 1c, create an object using the default
    //   constructor:
    //    - With and without passing in an allocator.
    //    - In the presence of exceptions during memory allocations using
    //      a 'bslma_TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator and
    //      neither of global and default allocator is used to supply memory.
    //
    //   To address concerns 3a - 3e, construct a series of independent
    //   objects, ordered by increasing length.  In each test, allow the
    //   object to leave scope without further modification, so that the
    //   destructor asserts internal object invariants appropriately.
    //   After the final insert operation in each test, use the (untested)
    //   basic accessors to cross-check the value of the object
    //   and the 'bslma_TestAllocator' to confirm whether memory allocation has
    //   occurred.
    //
    //   To address concerns 4a-4e, construct a similar test, replacing
    //   'push_back' with 'clear'; this time, however, use the test
    //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //
    //   To address concerns 2, 3d, 4d, create a small "area" test that
    //   exercises the construction and destruction of objects of various
    //   lengths in the presence of memory allocation exceptions.  Two
    //   separate tests will be performed:
    //
    //     Let S be the sequence of integers { 0 .. N - 1 }.
    //     (1) for each i in S, use the default constructor and 'push_back'
    //         to create an instance of length i, confirm its value (using
    //         basic accessors), and let it leave scope.
    //     (2) for each (i, j) in S X S, use 'push_back' to create an
    //         instance of length i, use 'clear' to clear its value
    //         and confirm (with 'length'), use insert to set the instance
    //         to a value of length j, verify the value, and allow the
    //         instance to leave scope.
    //
    //   The first test acts as a "control" in that 'clear' is not
    //   called; if only the second test produces an error, we know that
    //   'clear' is to blame.  We will rely on 'bslma_TestAllocator'
    //   and purify to address concern 2, and on the object invariant
    //   assertions in the destructor to address concerns 3d and 4d.
    //
    //   To address concern 5, the values will be explicitly compared to
    //   the expected values.  This will be done first so as to ensure all
    //   other tests are reliable and may depend upon the class's
    //   constants.
    //
    // Testing:
    //   list<T,A>(const A& a = A());
    //   ~list<T,A>();
    //   void push_back(const T&);
    //   void clear();
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    const ALLOC Z(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    if (verbose)
        printf("\tTesting parameters: SCOPED_ALLOC = %d.\n", SCOPED_ALLOC);

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
        const Int64 AA = testAllocator.numBlocksTotal();
        const Int64 A  = testAllocator.numBlocksInUse();

        ALLOC AL(&testAllocator);
        const Obj X(AL);

        const Int64 BB = testAllocator.numBlocksTotal();
        const Int64 B  = testAllocator.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        ASSERT(AA + expectedBlocks(0) == BB);
        ASSERT(A + expectedBlocks(0)  == B);
    }

    if (verbose) printf("\t\tIn place using a buffer allocator.\n");
    {
        ASSERT(0 == globalAllocator_p->numBlocksInUse());
        ASSERT(0 == defaultAllocator_p->numBlocksInUse());
        ASSERT(0 == objectAllocator_p->numBlocksInUse());

        ALLOC AL(objectAllocator_p);
        Obj x(AL);

        ASSERT(0 == globalAllocator_p->numBlocksInUse());
        ASSERT(0 == defaultAllocator_p->numBlocksInUse());
        ASSERT(expectedBlocks(0) == objectAllocator_p->numBlocksInUse());
    }
    ASSERT(0 == globalAllocator_p->numBlocksInUse());
    ASSERT(0 == defaultAllocator_p->numBlocksInUse());
    ASSERT(0 == objectAllocator_p->numBlocksInUse());

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            Obj mX;  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
                elemAddrs[i] = &X.back();
            }

            LOOP_ASSERT(li, li == X.size());

            if(veryVerbose){
                printf("\t\t\tBEFORE: "); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);
            elemAddrs[li] = &X.back();

            if(veryVerbose){
                printf("\t\t\tAFTER: "); P(X);
            }

            LOOP_ASSERT(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (int i = 0; i < (int) li; ++it, ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == *it);
                LOOP2_ASSERT(li, i, elemAddrs[i] == &*it);
            }

            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == *it);
            LOOP_ASSERT(li, elemAddrs[li] == &*it);
        }
    }

    // --------------------------------------------------------------------

    if (verbose)
        printf("\n\tTesting 'push_back' (bootstrap) with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose)
                printf("\t\tOn an object of initial length " ZU ".\n", li);

            ALLOC AL(&testAllocator);
            Obj mX(AL);  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
                elemAddrs[i] = &X.back();
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);
            elemAddrs[li] = &X.back();

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P(X);
            }

            LOOP_ASSERT(li, BB + deltaBlocks(1) == AA);
            LOOP_ASSERT(li, B + deltaBlocks(1) == A);

            LOOP_ASSERT(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (int i = 0; i < (int) li; ++it, ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == *it);
                LOOP2_ASSERT(li, i, elemAddrs[i] == &*it);
            }

            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == *it);
            LOOP_ASSERT(li, elemAddrs[li] == &*it);
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
                printf("\t\t\tBEFORE "); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            mX.clear();

            if(veryVerbose){
                printf("\t\t\tAFTER "); P(X);
            }

            LOOP_ASSERT(li, 0 == X.size());

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT "); P(X);
            }

            LOOP_ASSERT(li, li == X.size());
            const_iterator it = X.begin();
            for (int i = 0; i < (int) li; ++it, ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == *it);
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

            ALLOC AL(&testAllocator);
            Obj mX(AL);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 BB = testAllocator.numBlocksTotal();
            const Int64 B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            mX.clear();

            const Int64 AA = testAllocator.numBlocksTotal();
            const Int64 A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const Int64 CC = testAllocator.numBlocksTotal();
            const Int64 C  = testAllocator.numBlocksInUse();

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            LOOP_ASSERT(li, BB == AA);
            LOOP_ASSERT(li, B - deltaBlocks(li) == A);

            LOOP_ASSERT(li, BB + deltaBlocks(li) == CC);
            LOOP_ASSERT(li, B == C);
        }
    }

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'push_back' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1) create an instance
        //    2) insert { V0, V1, V2, V3, V4, V0, ... }  up to length li
        //    3) verify initial length and contents
        //    4) allow the instance to leave scope
        //    5) make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose)
                printf("\t\t\tOn an object of length " ZU ".\n", li);

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

            Obj mX(Z);  const Obj& X = mX;                           // 1.
            const TYPE *elemAddrs[NUM_TRIALS];
            for (size_t i = 0; i < li; ++i) {                        // 2.
                ExceptionGuard<Obj> guard(&mX, X, L_);
                mX.push_back(VALUES[i % NUM_VALUES]);
                elemAddrs[i] = &X.back();
                guard.release();
            }

            LOOP_ASSERT(li, li == X.size());                         // 3.
            const_iterator it = X.begin();
            for (int i = 0; i < (int) li; ++it, ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == *it);
                LOOP2_ASSERT(li, i, elemAddrs[i] == &*it);
            }

          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END                   // 4.
          LOOP_ASSERT(li, 0 == testAllocator.numBlocksInUse());      // 5.
        }
    }

    if (verbose) printf("\t\tWith 'push_back' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1) create an instance
        //    2) insert V0 values up to a length of i
        //    3) verify initial length and contents
        //    4) clear contents from instance
        //    5) verify length is 0
        //    6) insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7) verify new length and contents
        //    8) allow the instance to leave scope
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

                Obj mX(Z);  const Obj& X = mX;                      // 1.
                const TYPE *elemAddrs[NUM_TRIALS];
                for (k = 0; k < i; ++k) {                           // 2.
                    ExceptionGuard<Obj> guard(&mX, X, L_);
                    mX.push_back(VALUES[0]);
                    elemAddrs[k] = &X.back();
                    guard.release();
                }

                LOOP2_ASSERT(i, j, i == X.size());                  // 3.
                const_iterator it = X.begin();
                for (k = 0; k < i; ++k, ++it) {
                    LOOP3_ASSERT(i, j, k, VALUES[0] == *it);
                    LOOP3_ASSERT(i, j, k, elemAddrs[k] == &*it);
                }

                mX.clear();                                         // 4.
                LOOP2_ASSERT(i, j, 0 == X.size());                  // 5.

                for (k = 0; k < j; ++k) {                           // 6.
                    ExceptionGuard<Obj> guard(&mX, X, L_);
                    mX.push_back(VALUES[k % NUM_VALUES]);
                    elemAddrs[k] = &X.back();
                    guard.release();
                }

                LOOP2_ASSERT(i, j, j == X.size());                  // 7.
                it = X.begin();
                for (k = 0; k < j; ++k, ++it) {
                    LOOP3_ASSERT(i, j, k, VALUES[k % NUM_VALUES] == *it);
                    LOOP3_ASSERT(i, j, k, elemAddrs[k] == &*it);
                }

              } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END              // 8.
              LOOP_ASSERT(i, 0 == testAllocator.numBlocksInUse());  // 9.
            }

        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::breathingTest()
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
    //
    // Plan:
    //   Create four objects using both the default and copy constructors.
    //   Exercise these objects using primary manipulators, basic accessors,
    //   equality operators, and the assignment operator.  Try aliasing with
    //   assignment for a non-empty instance [11] and allow the result to
    //   leave scope, enabling the destructor to assert internal object
    //   invariants.  Display object values frequently in verbose mode:
    //
    // 1) Create an object x1 (default ctor).       { x1: }
    // 2) Create a second object x2 (copy from x1). { x1: x2: }
    // 3) Prepend an element value A to x1).        { x1:A x2: }
    // 4) Append the same element value A to x2).   { x1:A x2:A }
    // 5) Prepend/Append two values B & C to x2).   { x1:A x2:BAC }
    // 6) Remove all elements from x1.              { x1: x2:BAC }
    // 7) Create a third object x3 (default ctor).  { x1: x2:BAC x3: }
    // 8) Create a forth object x4 (copy of x2).    { x1: x2:BAC x3: x4:BAC }
    // 9) Assign x2 = x1 (non-empty becomes empty). { x1: x2: x3: x4:BAC }
    // 10) Assign x3 = x4 (empty becomes non-empty).{ x1: x2: x3:BAC x4:BAC }
    // 11) Assign x4 = x4 (aliasing).               { x1: x2: x3:BAC x4:BAC }
    //
    // Testing:
    //   This "test" *exercises* basic functionality.
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    const TYPE&         A = VALUES[0];
    const TYPE&         B = VALUES[1];
    const TYPE&         C = VALUES[2];
    (void) NUM_VALUES;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 1) Create an object x1 (default ctor)."
                        "\t\t\t{ x1: }\n");

    Obj mX1(&testAllocator);  const Obj& X1 = mX1;
    if (verbose) { T_;  P(X1); }

    if (verbose) printf("\ta) Check initial state of x1.\n");

    ASSERT(0 == X1.size());

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
    if (verbose) printf("\n 3) Prepend an element value A to x1)."
                        "\t\t\t{ x1:A x2: }\n");
    mX1.push_front(A);
    if (verbose) { T_;  P(X1); }

    if (verbose) printf(
        "\ta) Check new state of x1.\n");
    ASSERT(1 == X1.size());
    ASSERT(A == X1.front()); ASSERT(A == X1.back());

    if (verbose) printf(
        "\tb) Try equality operators: x1 <op> x1, x2.\n");
    ASSERT(  X1 == X1 );          ASSERT(!(X1 != X1));
    ASSERT(!(X1 == X2));          ASSERT(  X1 != X2 );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 4) Append the same element value A to x2)."
                         "\t\t{ x1:A x2:A }\n");
    mX2.push_back(A);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(1 == X2.size());
    ASSERT(A == X2.front()); ASSERT(A == X2.back());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(  X2 == X1 );          ASSERT(!(X2 != X1));
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 5) Prepend another element value B and append "
                        "another element c to x2).\t\t{ x1:A x2:BAC }\n");
    mX2.push_front(B);
    mX2.push_back(C);
    if (verbose) { T_;  P(X2); }

    if (verbose) printf(
        "\ta) Check new state of x2.\n");
    ASSERT(3 == X2.size());
    ASSERT(B == X2.front());
    ASSERT(A == nthElem(X2, 1));
    ASSERT(C == X2.back());

    if (verbose) printf(
        "\tb) Try equality operators: x2 <op> x1, x2.\n");
    ASSERT(!(X2 == X1));          ASSERT(  X2 != X1 );
    ASSERT(  X2 == X2 );          ASSERT(!(X2 != X2));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 6) Remove all elements from x1."
                         "\t\t\t{ x1: x2:BAC }\n");
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
                         "\t\t{ x1: x2:BAC x3: }\n");

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
                         "\t\t{ x1: x2:BAC x3: x4:BAC }\n");

    Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");

    ASSERT(3 == X4.size());
    ASSERT(B == nthElem(X4, 0));
    ASSERT(A == nthElem(X4, 1));
    ASSERT(C == nthElem(X4, 2));

    if (verbose) printf(
        "\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(  X4 == X2 );          ASSERT(!(X4 != X2));
    ASSERT(!(X4 == X3));          ASSERT(  X4 != X3 );
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n 9) Assign x2 = x1 (non-empty becomes empty)."
                         "\t\t{ x1: x2: x3: x4:BAC }\n");

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
                         "\t\t{ x1: x2: x3:BAC x4:BAC }\n");

    mX3 = X4;
    if (verbose) { T_;  P(X3); }

    if (verbose) printf(
        "\ta) Check new state of x3.\n");
    ASSERT(3 == X3.size());
    ASSERT(B == nthElem(X3, 0));
    ASSERT(A == nthElem(X3, 1));
    ASSERT(C == nthElem(X3, 2));

    if (verbose) printf(
        "\tb) Try equality operators: x3 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X3 == X1));          ASSERT(  X3 != X1 );
    ASSERT(!(X3 == X2));          ASSERT(  X3 != X2 );
    ASSERT(  X3 == X3 );          ASSERT(!(X3 != X3));
    ASSERT(  X3 == X4 );          ASSERT(!(X3 != X4));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (verbose) printf("\n11) Assign x4 = x4 (aliasing)."
                         "\t\t\t\t{ x1: x2: x3:BAC x4:BAC }\n");

    mX4 = X4;
    if (verbose) { T_;  P(X4); }

    if (verbose) printf(
        "\ta) Check new state of x4.\n");
    ASSERT(3 == X4.size());
    ASSERT(B == nthElem(X4, 0));
    ASSERT(A == nthElem(X4, 1));
    ASSERT(C == nthElem(X4, 2));

    if (verbose)
        printf("\tb) Try equality operators: x4 <op> x1, x2, x3, x4.\n");
    ASSERT(!(X4 == X1));          ASSERT(  X4 != X1 );
    ASSERT(!(X4 == X2));          ASSERT(  X4 != X2 );
    ASSERT(  X4 == X3 );          ASSERT(!(X4 != X3));
    ASSERT(  X4 == X4 );          ASSERT(!(X4 != X4));
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

    // As part of our overall allocator testing strategy, we will create
    // three test allocators.

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
      case 29: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   1. The usage examples in the header file compile.
        //   2. The usage examples in the header produce correct results
        //
        // Test plan:
        //   Copy the usage examples from the header into this test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        // Create test data files
        FILE* output = fopen("star_data1.txt", "w");
        ASSERT(output);
        fprintf(output, "%s", STAR_DATA1);
        fclose(output);

        output = fopen("star_data2.txt", "w");
        ASSERT(output);
        fprintf(output, "%s", STAR_DATA2);
        fclose(output);

        if (verbose) printf("\nusageExample1\n");
        usageExample1(veryVerbose);

        if (verbose) printf("\nusageExample2\n");
        usageExample2(veryVerbose);

        // Erase output files.

        remove("star_data1.txt");
        remove("star_data2.txt");

      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING SORT
        //
        // Concerns and plan:
        //   See testSort for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void sort();
        //   template <class COMP> void sort(COMP c);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting SORT"
                            "\n============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testSort();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testSort();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testSort();

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING MERGE
        //
        // Concerns and plan:
        //   See testMerge for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void merge(list& other);
        //   template <class COMP> void merge(list& other, COMP c);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting MERGE"
                            "\n=============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testMerge();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testMerge();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testMerge();

      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING UNIQUE
        //
        // Concerns and plan:
        //   See testUnique for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void unique();
        //   template <class BINPRED> void unique(BINPRED p);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting  UNIQUE"
                            "\n===============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testUnique();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testUnique();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testUnique();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testUnique();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testUnique();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testUnique();

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING REMOVE
        //
        // Concerns and plan:
        //   See testRemove for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void remove(const T& val);
        //   template <class PRED> void remove_if(PRED p);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting  REMOVE"
                            "\n===============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testRemove();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testRemove();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testRemove();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testRemove();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testRemove();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testRemove();

      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING SPLICE
        //
        // Concerns and plan:
        //   See testSplice for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void splice(iterator pos, list& other);
        //   void splice(iterator pos, list& other, iterator i);
        //   void splice(iterator pos, list& other,
        //               iterator first, iterator last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting  SPLICE"
                            "\n===============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testSplice();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testSplice();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testSplice();

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING REVERSE
        //
        // Concerns and plan:
        //   See testReverse for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void reverse();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting  REVERSE"
                            "\n================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testReverse();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testReverse();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testReverse();

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //
        // Concerns and plan:
        //   See testTypeTraits for a list of specific concerns and a test
        //   plan.
        //
        // Testing:
        //   bslalg::HasStlIterators
        //   bslma::UsesBslmaAllocator
        //   bslmf::IsBitwiseMoveable
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting TYPE TRAITS"
                            "\n===================\n");

        // The default allocator is both a bslma allocator and is bitwise
        // moveable.
        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testTypeTraits(/* uses_bslma */ true,
                                         /* bitwise_moveable */ true);

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testTypeTraits(/* uses_bslma */ true,
                                      /* bitwise_moveable */ true);

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testTypeTraits(/* uses_bslma */ true,
                                        /* bitwise_moveable */ true);

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testTypeTraits(/* uses_bslma */ true,
                                        /* bitwise_moveable */ true);

        // OtherAlloc allocator is neither a bslma allocator nor bitwise
        // moveable.
        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testTypeTraits(/* uses_bslma */ false,
                                          /* bitwise_moveable */ false);

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testTypeTraits(/* uses_bslma */ false,
                                              /* bitwise_moveable */ false);

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING TYPEDEFS
        //
        // Concerns:
        //   1. That all of the required typedefs are defined.
        //   2. That the typedefs are identical to the corresponding typedefs
        //      from the allocator.
        //
        // Plan:
        //   Instantiate 'list<T,Alloc>' with at least two types of 'Alloc'.
        //   Verify that each typedef in 'list<T,bAlloc>' matches the
        //   corresponding typedef in 'Alloc'.  Note that the iterator types
        //   were tested in test case 16 and so are not tested here.
        //
        // Testing:
        //   reference
        //   const_reference
        //   size_type
        //   difference_type
        //   value_type
        //   allocator_type
        //   pointer
        //   const_pointer
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting typedefs"
                            "\n================\n");

        if (verbose) printf("\nWith 'bsl::allocator'\n");
        {
            typedef bsl::allocator<T> Alloc;
            typedef list<T,Alloc>     Obj;

            ASSERT((bsl::is_same<Alloc::reference,
                                 Obj::reference>::value));
            ASSERT((bsl::is_same<Alloc::const_reference,
                                 Obj::const_reference>::value));

            ASSERT((bsl::is_same<Alloc::pointer,
                                 Obj::pointer>::value));
            ASSERT((bsl::is_same<Alloc::const_pointer,
                                 Obj::const_pointer>::value));

            ASSERT((bsl::is_same<Alloc::size_type,
                                 Obj::size_type>::value));
            ASSERT((bsl::is_same<Alloc::difference_type,
                                 Obj::difference_type>::value));

            ASSERT((bsl::is_same<T, Obj::value_type>::value));
            ASSERT((bsl::is_same<Alloc, Obj::allocator_type>::value));
        }

        if (verbose) printf("\nWith 'SmallAllocator'\n");
        {
            typedef SmallAllocator<T> Alloc;
            typedef list<T,Alloc>     Obj;

            ASSERT((bsl::is_same<Alloc::reference,
                                 Obj::reference>::value));
            ASSERT((bsl::is_same<Alloc::const_reference,
                                 Obj::const_reference>::value));

            ASSERT((bsl::is_same<Alloc::pointer,
                                 Obj::pointer>::value));
            ASSERT((bsl::is_same<Alloc::const_pointer,
                                 Obj::const_pointer>::value));

            ASSERT((bsl::is_same<Alloc::size_type,
                                 Obj::size_type>::value));
            ASSERT((bsl::is_same<Alloc::difference_type,
                                 Obj::difference_type>::value));

            ASSERT((bsl::is_same<T, Obj::value_type>::value));
            ASSERT((bsl::is_same<Alloc, Obj::allocator_type>::value));
        }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON FREE OPERATORS
        //
        // Concerns and plan:
        //   See testComparisonOps for a list of specific concerns and a test
        //   plan.
        //
        // Testing:
        //   bool operator<(const list<T,A>& lhs, const list<T,A>& rhs);
        //   bool operator>(const list<T,A>& lhs, const list<T,A>& rhs);
        //   bool operator<=(const list<T,A>& lhs, const list<T,A>& rhs);
        //   bool operator>=(const list<T,A>& lhs, const list<T,A>& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting comparison free operators"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testComparisonOps();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testComparisonOps();

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Concerns and plan:
        //   See testSwap for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void swap(list&);
        //   void swap(list<T,A>&  lhs, list<T,A>&  rhs);
        //   void swap(list<T,A>&& lhs, list<T,A>&  rhs);
        //   void swap(list<T,A>&  lhs, list<T,A>&& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'swap'"
                            "\n==============\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testSwap();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testSwap();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testSwap();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testSwap();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testSwap();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testSwap();

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING ERASE
        //
        // Concerns and plan:
        //   See testErase for a list of specific concerns and a test plan.
        //
        // Testing:
        //   iterator erase(const_iterator position);
        //   iterator erase(const_iterator first, const_iterator last);
        //   void pop_back();
        //   void pop_front();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'erase' and 'pop_back'"
                            "\n==============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testErase();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testErase();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testErase();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testErase();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testErase();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testErase();

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        //
        // Concerns and plan:
        //   See testInsert for a list of specific concerns and a test plan.
        //
        // Testing:
        //   void push_back(const T& value);
        //   void push_front(const T& value);
        //   iterator insert(const_iterator position, const T& value);
        //   void insert(const_iterator pos, size_type n, const T& val);
        //   template <class InputIter>
        //    void insert(const_iterator pos, InputIter first, InputIter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testInsert();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testInsert();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testInsert();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testInsert();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testInsert();

        if (verbose) printf("\nTesting Emplace Insertion"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testEmplace();

        // if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        // TestDriver<TOA>::testEmplace();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testEmplace();

        // if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
        //                     " 'OtherAlloc'.\n");
        // TestDriver<TOA,OATOA>::testEmplace();

        if (verbose) printf("\nTesting Range Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary forward iterator.\n");
        TestDriver<T>::testInsertRange(InputSeq<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testInsertRange(RandSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver<TOA>::testInsertRange(InputSeq<TOA>());

        if (verbose) printf("\n... with 'TestType', 'OtherAlloc', "
                            "and arbitrary input iterator.\n");
        TestDriver<T,OAT>::testInsertRange(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc', 'OtherAlloc', "
                            "and arbitrary input iterator.\n");
        TestDriver<TOA,OATOA>::testInsertRange(InputSeq<TOA>());

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ITERATORS
        //
        // Concerns and plan:
        //   See testIterators for a list of specific concerns and a test plan.
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

        if (verbose) printf("\nTesting Iterators"
                            "\n=================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testIterators();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testIterators();

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ELEMENT ACCESS
        //
        // Testing:
        //   T& front();
        //   T& back();
        //   const T& front() const;
        //   const T& back() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Element Access"
                            "\n======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testElementAccess();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testElementAccess();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING resize AND max_size
        //
        // Concerns:
        //   1. For 'max_size', the reported value is no more than one less
        //      than the maximum allowed by the allocator.
        //   2. For 'max_size', the allocator's 'max_size' is honored.
        //   3. For 'resize', see 'testResize' for a list of concerns and plan.
        //
        // Plan:
        //   Using the default allocator, test that 'max_size' returns a value
        //   no larger than all of memory divided by the size of one element.
        //   Repeat this test with 'char' and TestType' element types.  Using
        //   the 'LimitAllocator', test that 'max_size' returns the same value
        //   as 'LimitAllocator<T>::max_size()', except that a node of
        //   overhead is allowed to be subtracted from the result.
        //
        //   For 'resize', call 'testResize()' with different combinations of
        //   element types and allocator types.  See 'testResize' for details.
        //
        // Testing:
        //   void resize(size_type sz);
        //   void resize(size_type sz, T val);
        //   size_type max_size() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'resize' and 'max_size'"
                            "\n===============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        {
            list<char> X;
            ASSERT(~(size_t)0 / sizeof(char) >= X.max_size());
        }

        if (verbose) printf("\n... with 'TestType'.\n");
        {
            list<TestType> X;
            ASSERT(~(size_t)0 / sizeof(TestType) >= X.max_size());
        }

        if (verbose) printf("\n... with 'int' and 'LimitAllocator.\n");
        {
            const int LIMIT = 10;
            typedef LimitAllocator<bsl::allocator<int> > LimA;
            LimA a;
            a.setMaxSize(LIMIT);

            list<int,LimA> X(a);
            // LimitAllocator will return the same 'max_size' regardless of the
            // type on which it is instantiated.  Thus, it will report that it
            // can allocate the same number of nodes as 'int's.  (This
            // behavior is not typical for an allocator, but works for this
            // test.)  The 'list' should have no more than one node of
            // overhead.
            ASSERT(LIMIT     >= (int) X.max_size());
            ASSERT(LIMIT - 1 <= (int) X.max_size());
        }

        if (verbose) printf("\nTesting 'resize'.\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testResize();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testResize();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testResize();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testResize();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testResize();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT
        //
        // Concerns:
        //   Specific concerns are listed in 'testAssign' and 'testAssignRange'
        //
        // Test plan:
        //   Test with combinations of:
        //    - Element type 'char', 'TestType', or 'TestTypeOtherAlloc'
        //    - Allocator type 'bsl::allocator' or 'OtherAllocator'
        //    - Random-access iterator range or input iterator range.
        //
        // Testing:
        //   void assign(size_t n, const T& val);
        //   template<class InputIter>
        //     void assign(InputIter first, InputIter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Initial-Length Assignment"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testAssign();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testAssign();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testAssign();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testAssign();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testAssign();

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testAssignRange(InputSeq<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testAssignRange(RandSeq<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<T>::testAssignRange(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver<TOA>::testAssignRange(InputSeq<TOA>());

        if (verbose) printf("\n... with 'TestType', 'OtherAlloc', "
                            "and arbitrary input iterator.\n");
        TestDriver<T,OAT>::testAssignRange(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc', 'OtherAlloc', "
                            "and arbitrary input iterator.\n");
        TestDriver<TOA,OATOA>::testAssignRange(InputSeq<TOA>());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   Specific concerns are listed in 'testConstructor' and
        //   'testConstructorRange'
        //
        // Test plan:
        //   Test with combinations of:
        //    - Element type 'char', 'TestType', or 'TestTypeOtherAlloc'
        //    - Allocator type 'bsl::allocator' or 'OtherAllocator'
        //    - Random-access iterator range or input iterator range.
        //
        // Testing:
        //   list<T,A>(size_type n, const T& val = T(), const A& a = A());
        //   template<class InputIter>
        //     list<T,A>(InputIter first, InputIter last, const A& a = A());
        //   list(list<T,A>&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Initial-Length Constructor"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testConstructor();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testConstructor();

        if (verbose) printf("\n... with 'TestType and other allocator'.\n");
        TestDriver<T,OAT>::testConstructor();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testConstructor();

        if (verbose)
            printf("\n... with 'TestTypeOtherAlloc and other allocator'.\n");
        TestDriver<TOA,OATOA>::testConstructor();

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char' "
                            "an arbitrary input iterator.\n");
        TestDriver<char>::testConstructorRange(InputSeq<char>());

        if (verbose) printf("\n... with 'char' "
                            "an arbitrary random-access iterator.\n");
        TestDriver<char>::testConstructorRange(RandSeq<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "an arbitrary input iterator.\n");
        TestDriver<T>::testConstructorRange(InputSeq<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "an arbitrary random-access iterator.\n");
        TestDriver<T>::testConstructorRange(RandSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "an arbitrary input iterator.\n");
        TestDriver<TOA>::testConstructorRange(InputSeq<TOA>());

        if (verbose) printf("\n... with 'TestType', 'OtherAlloc', "
                            "an arbitrary input iterator.\n");
        TestDriver<T,OAT>::testConstructorRange(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "an arbitrary input iterator.\n");
        TestDriver<TOA,OATOA>::testConstructorRange(InputSeq<TOA>());

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR-RELATED CONCERNS
        //
        // Plan:
        //   Call test driver case11 with the following combinations:
        //   1. Element type and list both use bslma_Allocator
        //   2. Element type uses bslma_Allocator, and list uses no-bslma
        //      allocator type.
        //   3. Element type uses non-bslma allocator type, and list uses
        //      bslma allocator type.
        //   4. Element type and list both use non-bslma allocator type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Allocator concerns"
                            "\n==========================\n");

        TestDriver<T>::testAllocator("T", "bslma");

        TestDriver<T,OAT>::testAllocator("T", "OAT");

        TestDriver<TOA>::testAllocator("TOA", "bslma");

        TestDriver<TOA,OATOA>::testAllocator("TOA", "OATOA");

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Streaming Functionality"
                            "\n===============================\n");

        if (verbose)
            printf("There is no streaming for this component.\n");

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns and plan:
        //   Now that we can generate many values for our test objects, and
        //   compare results of assignments, we can test the assignment
        //   operator.  This is achieved by the 'testAssignmentOp' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Testing:
        //   Obj& operator=(const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testAssignmentOp();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testAssignmentOp();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testAssignmentOp();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testAssignmentOp();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testAssignmentOp();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
        //
        // Concerns and plan:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.  Because the generator is used for various types
        //   in higher numbered test cases, we need to test it on all test
        //   types.  This is achieved by the 'testGeneratorG' class method of
        //   the test driver template, instantiated for the basic test type.
        //   See that function for a list of concerns and a test plan.
        //
        // Testing:
        //   Obj g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Generator Function g"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testGeneratorG();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testGeneratorG();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testGeneratorG();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testGeneratorG();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testGeneratorG();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testGeneratorG();

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns and plan:
        //   Having now full confidence in 'operator==', we can use it
        //   to test that copy constructors preserve the notion of
        //   value.  This is achieved by the 'testCopyCtor' class method of the
        //   test driver template, instantiated for the basic test type.  See
        //   that function for a list of concerns and a test plan.
        //
        // Testing:
        //   list(const list& original);
        //   list(const list& original, alloc);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCopyCtor();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCopyCtor();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCopyCtor();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testCopyCtor();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testCopyCtor();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testCopyCtor();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns and plan:
        //   Since 'operators==' is implemented in terms of basic accessors,
        //   it is sufficient to verify only that a difference in value of any
        //   one basic accessor for any two given objects implies inequality.
        //   However, to test that no other internal state information is
        //   being considered, we want also to verify that 'operator==' reports
        //   true when applied to any two objects whose internal
        //   representations may be different yet still represent the same
        //   (logical) value.  This is achieved by the 'testEqualityOp' class
        //   method of the test driver template, instantiated for the basic
        //   test type.  See that function for a list of concerns and a test
        //   plan.
        //
        // Testing:
        //   operator==(const Obj&, const Obj&);
        //   operator!=(const Obj&, const Obj&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testEqualityOp();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testEqualityOp();

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
        // TESTING BASIC ACCESSORS:
        //
        // Plan:
        //   Having implemented an effective generation mechanism, we now would
        //   like to test thoroughly the basic accessor functions
        //     - size() const
        //     - begin()
        //     - end()
        //     - begin() const
        //     - end() const
        //   Also, we want to test the test utility functions:
        //     - succ(const_iterator)
        //     - nthElem(Obj, int)
        //
        // Testing:
        //   int size() const;
        //   iterator begin();
        //   iterator end();
        //   const_iterator begin() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testBasicAccessors();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testBasicAccessors();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testBasicAccessors();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testBasicAccessors();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testBasicAccessors();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testBasicAccessors();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS
        //
        // Concerns and plan:
        //   This is achieved by the 'testGeneratorGG' class method of the test
        //   driver template, instantiated for the basic test type.  See that
        //   function for a list of concerns and a test plan.
        //
        // Testing:
        //   void ggg(Obj *object, const char *spec);
        //   Obj& gg(Obj *object, const char *spec, );
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Generator Functions"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testGeneratorGG();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testGeneratorGG();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testGeneratorGG();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testGeneratorGG();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testGeneratorGG();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testGeneratorGG();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        // Concerns and plan:
        //   We want to ensure that the primary manipulators
        //      - push_back             (black-box)
        //      - clear                 (white-box)
        //   operate as expected.  This is achieved by the
        //   'testPrimaryManipulators' class method of the test driver
        //   template, instantiated for the basic test type.  See that
        //   function for a list of concerns and a test plan.
        //
        // Testing:
        //   void push_back(T const& v);
        //   void clear();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testPrimaryManipulators();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testPrimaryManipulators();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testPrimaryManipulators();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testPrimaryManipulators();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testPrimaryManipulators();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testPrimaryManipulators();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //   We want to exercise basic value-semantic functionality.  This is
        //   achieved by the 'breathingTest' class method of the test driver
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
        TestDriver<char>::breathingTest();

        if (verbose) printf("\n\t... with 'TestType'.\n");
        TestDriver<T>::breathingTest();

        if (verbose) printf("\n\t... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::breathingTest();

        if (verbose) printf("\nAdditional tests: allocators.\n");

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        bsl::allocator<int> zza(&testAllocator);

        // Disabled: we disabled this very infrequent usage for list (it will
        // be flagged by 'BSLMF_ASSERT'), which is illegal as of C++0x anyway:
        //..
        // list<int, bsl::allocator<void*> > zz1, zz2(zza);
        //..

        if (verbose) printf("\nAdditional tests: misc.\n");

        list<char> myLst(5, 'a');
        list<char>::const_iterator citer;
        ASSERT(5 == myLst.size());
        for (citer = myLst.begin(); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myLst);

        myLst.insert(myLst.begin(), 'z');
        ASSERT(6 == myLst.size());
        ASSERT('z' == myLst.front());
        for (citer = ++myLst.begin(); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myLst);

        myLst.erase(succ(myLst.begin(),2), succ(myLst.begin(),4));
        ASSERT(4 == myLst.size());
        ASSERT('z' == myLst.front());
        for (citer = succ(myLst.begin()); citer != myLst.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myLst);

        list<list<char> > vv;
        vv.push_front(myLst);
        if (verbose) P(myLst);

        if (verbose) printf("\nAdditional tests: traits.\n");

        ASSERT(  bslmf::IsBitwiseMoveable<list<char> >::value);
        ASSERT(  bslmf::IsBitwiseMoveable<list<T> >::value);
        ASSERT(  bslmf::IsBitwiseMoveable<list<list<int> > >::value);

        ASSERT(! bsl::is_trivially_copyable<list<char> >::value);
        ASSERT(! bsl::is_trivially_copyable<list<T> >::value);
        ASSERT(! bsl::is_trivially_copyable<list<list<int> > >::value);

      } break;
      case -1: {
        // This test is used to verify various list constructors do not require
        // copy-assignable value type as its (template parameter) type 'VALUE'.

        bsltf::NonAssignableTestType value(1);
        list<bsltf::NonAssignableTestType> firstList(20, value);
        list<bsltf::NonAssignableTestType> secondList(firstList);
        list<bsltf::NonAssignableTestType> thirdList(firstList.begin(),
                                                     firstList.end());
      }
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
