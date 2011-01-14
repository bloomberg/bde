// bslstl_list.t.cpp                                                  -*-C++-*-
#include <bslstp_list.h>

// #include <bslstl_allocator.h>
// #include <bslstl_iterator.h>
// #include <bslstl_forwarditerator.h>

#include <bslalg_typetraits.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatorexception.h>  // for testing only
#include <bslmf_assert.h>                  // for testing only
#include <bslmf_issame.h>                  // for testing only
#include <bsls_objectbuffer.h>
#include <bsls_alignmentutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_stopwatch.h>                // for testing only

#include <iterator>   // 'iterator_traits'
#include <stdexcept>  // 'length_error', 'out_of_range'

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
// dictated by the C++ standard.  In particular, the standard mandates "strong"
// exception safety (with full guarantee of rollback) along with throwing
// 'std::length_error' if about to request memory for more than 'max_size()'
// elements.  (Note: 'max_size' depends on the parameterized 'VALUE_TYPE'.)
// The general concerns are compliance, exception safety, and proper
// dispatching (for member function templates such as assign and insert).  In
// addition, it is a value-semantic type whose salient attributes are size and
// value of each element in sequence.  This container is implemented in the
// form of a class template, and thus its proper instantiation for several
// types is a concern.  Regarding the allocator template argument, we use
// mostly a 'bsl::allocator' together with a 'bslma_TestAllocator' mechanism,
// but we also verify the C++ standard.
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back' and 'clear' methods to be used by the generator functions
// 'g' and 'gg'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity).  Note that some
// manipulators must support aliasing, and those that perform memory allocation
// must be tested for exception neutrality via the 'bslma_testallocator'
// component.  After the mandatory sequence of cases (1--10) for value-semantic
// types (cases 5 and 10 are not implemented, as there is not output or
// streaming below stlport), we test each individual constructor, manipulator,
// and accessor in subsequent cases.
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     T            VALUE_TYPE (template argument, no default)
//     A            ALLOCATOR (template argument, default: bsl::allocator<T>)
//     list<T,A>    bsl::list<VALUE_TYPE,ALLOCATOR>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//-----------------------------------------------------------------------------
// class list<T,A> (list)
// =================================
// [11] TRAITS
//
// CREATORS:
// [ 2] list<T,A>(const A& a = A());
// [12] list<T,A>(size_type n, const A& a = A());
// [12] list<T,A>(size_type n, const T& val, const A& a = A());
// [12] template<class InputIter>
//        list<T,A>(InputIter first, InputIter last, const A& a = A());
// [ 7] list<T,A>(const list<T,A>& orig, const A& = A());
// [12] list(list<T,A>&& original);
// [ 2] ~list<T,A>();
//
/// MANIPULATORS:
// [13] template <class InputIter>
//        void assign(InputIter first, InputIter last);
// [13] void assign(size_type numElements, const T& val);
// [ 9] operator=(list<T,A>&);
// [15] reference operator[](size_type pos);
// [15] reference at(size_type pos);
// [16] iterator begin();
// [16] iterator end();
// [16] reverse_iterator rbegin();
// [16] reverse_iterator rend();
// [14] void resize(size_type n);
// [14] void resize(size_type n, const T& val);
// [14] void reserve(size_type n);
// [ 2] void clear();
// [15] reference front();
// [15] reference back();
// [20] template <class Args...>
//        iterator emplace(const_iterator pos, Args...);
// [ 2] void push_back(const T&);
// [17] void push_back(T&&);
// [18] void pop_back();
// [17] iterator insert(const_iterator pos, const T& val);
// [17] iterator insert(const_iterator pos, size_type n, const T& val);
// [17] template <class InputIter>
//        void insert(const_iterator pos, InputIter first, InputIter last);
// [18] iterator erase(const_iterator pos);
// [18] iterator erase(const_iterator first, const_iterator last);
// [19] void swap(list<T,A>&);
//
// ACCESSORS:
// [ 4] const_reference operator[](size_type pos) const;
// [ 4] const_reference at(size_type pos) const;
// [15] const_reference front() const;
// [15] const_reference back() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [??] bool empty() const;
// [16] const_iterator begin();
// [16] const_iterator end();
// [16] const_reverse_iterator rbegin();
// [16] const_reverse_iterator rend();
//
// FREE OPERATORS:
// [ 6] bool operator==(const list<T,A>&, const list<T,A>&);
// [ 6] bool operator!=(const list<T,A>&, const list<T,A>&);
// [22] bool operator<(const list<T,A>&, const list<T,A>&);
// [22] bool operator>(const list<T,A>&, const list<T,A>&);
// [22] bool operator<=(const list<T,A>&, const list<T,A>&);
// [22] bool operator>=(const list<T,A>&, const list<T,A>&);
// [19] void swap(list<T,A>&, list<T,A>&);
// [19] void swap(list<T,A>&, list<T,A>&&);
// [19] void swap(list<T,A>&&, list<T,A>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] ALLOCATOR-RELATED CONCERNS
// [18] USAGE EXAMPLE
// [21] CONCERN: 'std::length_error' is used properly
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(list<T,A> *object, const char *spec, int vF = 1);
// [ 3] list<T,A>& gg(list<T,A> *object, const char *spec);
// [ 8] list<T,A> g(const char *spec);

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
class TestType;
class TestTypeNoAlloc;
class TestTypeOtherAlloc;
class BitwiseMoveableTestType;
class BitwiseCopyableTestType;
template <class T> class OtherAllocator;

typedef TestType                      T;    // uses 'bslma' allocators
typedef TestTypeNoAlloc               TNA;  // does not use 'bslma' allocators
typedef TestTypeOtherAlloc            TOA;  // Uses non-'bslma' allocators
typedef BitwiseMoveableTestType       BMT;  // uses 'bslma' allocators
typedef BitwiseCopyableTestType       BCT;  // uses 'bslma' allocators

typedef OtherAllocator<TestType>           OAT;   // Non-'bslma' allocator
typedef OtherAllocator<TestTypeOtherAlloc> OATOA; // Non-'bslma' allocator

typedef bsls_Types::Int64             Int64;
typedef bsls_Types::Uint64            Uint64;

// TEST OBJECT (unless o/w specified)
typedef char                          Element;  // every TestType's value type
typedef list<T>                       Obj;

// CONSTANTS
const int MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

const char UNINITIALIZED_VALUE = '_';
const char DEFAULT_VALUE       = 'z';
const char VA = 'A';
const char VB = 'B';
const char VC = 'C';
const char VD = 'D';
const char VE = 'E';
    // All test types have character value type.

const int  LARGE_SIZE_VALUE = 10;
    // Declare a large value for insertions into the list.  Note this value
    // will cause multiple resizes during insertion into the list.

const int NUM_ALLOCS[] = {
    // Number of allocations (blocks) to create a list of the following size
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
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(Int64 val) {
    printf("%lld", val); fflush(stdout);
}
inline void dbg_print(size_t val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

// List-specific print function.
template <typename TYPE, typename ALLOC>
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
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl)
{
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

// Return the 'n'th iterator after 'it':
template <typename ITER>
inline ITER succ(ITER it, int n = 1)
{
    for (int i = 0; i < n; ++i)
        ++it;
    return it;
}

// Return the 'n'th element of container x, counting from 0. (I.e., if for
// n == 0, return x.front().)
template <typename C>
inline typename C::value_type& nthElem(C& x, int n)
{
    return *succ(x.begin(), n);
}

template <typename C>
inline const typename C::value_type& nthElem(const C& x, int n)
{
    return *succ(x.begin(), n);
}

//=============================================================================
//                       GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose;
static bslma_TestAllocator *globalAllocator_p,
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
        ASSERT(&original != this);
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
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

// TestType-specific print function.
void dbg_print(const TestTypeNoAlloc& rhs) {
    printf("%c", rhs.value());
    fflush(stdout);
}

                               // ====================
                               // class OtherAllocator
                               // ====================

bslma_TestAllocator OtherAllocatorDefaultImp;

template <typename T>
class OtherAllocator
{
    bslma_Allocator* d_implementation;

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

    template <typename U>
    struct rebind
    {
        typedef OtherAllocator<U> other;
    };

    // Constructors
    explicit OtherAllocator(bslma_Allocator* a) : d_implementation(a) { }
    OtherAllocator() : d_implementation(&OtherAllocatorDefaultImp) { }
    template <typename U> OtherAllocator(const OtherAllocator<U> other)
        : d_implementation(other.implementation()) { }

    // Manipulators
    T* allocate(size_t n, const void* = 0)
        { return static_cast<T*>(d_implementation->allocate(sizeof(T)*n)); }
    void deallocate(T* p, size_t) { d_implementation->deallocate(p); }
    void construct(pointer p, const T& v) { ::new((void*) p) T(v); }
    void destroy(pointer p) { p->~T(); }

    // Accessors
    size_t max_size() const { return ((size_t)-1) / sizeof(T); }
    bslma_Allocator* implementation() const { return d_implementation; }
};

template <typename T, typename U>
inline
bool operator==(const OtherAllocator<T>& a, const OtherAllocator<U>& b)
{
    return a.implementation() == b.implementation();
}

template <typename T, typename U>
inline
bool operator!=(const OtherAllocator<T>& a, const OtherAllocator<U>& b)
{
    return a.implementation() != b.implementation();
}

                               // ====================
                               // class SmallAllocator
                               // ====================

template <class T>
class SmallAllocator : public OtherAllocator<T>
{
  public:
    // Small allocator that can allocate only 10 elements.
    // Note: Allocation limit is not enforced.  Only the operation of
    // 'max_size' matters for this test.

    template <typename U>
    struct rebind
    {
        typedef SmallAllocator<U> other;
    };

    SmallAllocator() { }
    explicit SmallAllocator(bslma_Allocator* a) : OtherAllocator<T>(a) { }

    template <class U>
    SmallAllocator(const SmallAllocator<U>& other)
        : OtherAllocator<T>(other) { }

    static size_t max_size() { return 10; }
};

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

    bslma_Allocator* allocator() const {
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

                       // =============================
                       // class BitwiseMoveableTestType
                       // =============================

class BitwiseMoveableTestType : public TestType {
    // This test type is identical to 'TestType' except that it has the
    // bit-wise moveable trait.  All members are inherited.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(BitwiseMoveableTestType,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    explicit
    BitwiseMoveableTestType(bslma_Allocator *ba = 0)
    : TestType(ba)
    {
    }

    explicit
    BitwiseMoveableTestType(char c, bslma_Allocator *ba = 0)
    : TestType(c, ba)
    {
    }

    BitwiseMoveableTestType(const BitwiseMoveableTestType&  original,
                            bslma_Allocator                *ba = 0)
    : TestType(original, ba)
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
    BSLALG_DECLARE_NESTED_TRAITS(BitwiseCopyableTestType,
                                 bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    BitwiseCopyableTestType()
    : TestTypeNoAlloc()
    {
    }

    explicit
    BitwiseCopyableTestType(char c)
    : TestTypeNoAlloc(c)
    {
        ++numCharCtorCalls;
    }

    BitwiseCopyableTestType(const BitwiseCopyableTestType&  original)
    : TestTypeNoAlloc(original.value())
    {
    }
};

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
    template <typename T>
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
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(LimitAllocator,
                                 BloombergLP::bslalg_TypeTraits<AllocBase>);

    // CREATORS
    LimitAllocator()
    : d_limit(-1) {}

    LimitAllocator(BloombergLP::bslma_Allocator *mechanism)
    : AllocBase(mechanism), d_limit(-1) { }

    LimitAllocator(const ALLOC& rhs)
    : AllocBase((const AllocBase&) rhs), d_limit(-1) { }

    ~LimitAllocator() { }

    // MANIPULATORS
    void setMaxSize(size_type maxSize) { d_limit = maxSize; }

    // ACCESSORS
    size_type max_size() const { return d_limit; }
};

//=============================================================================
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A .. E] correspond to arbitrary (but unique) char
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
        bslmf_IsConvertible<bslma_Allocator*,ALLOC>::Type ObjHasBslmaAlloc;
        // TRUE_TYPE if ALLOC is a bslma allocator type

    typedef typename
        bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::Type
            TypeHasBslmaAlloc;
        // TRUE_TYPE if TYPE uses a bslma allocator

    enum { SCOPED_ALLOC = ObjHasBslmaAlloc::VALUE && TypeHasBslmaAlloc::VALUE};
        // true if both the container shares its allocator with its contained
        // elements.

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

    // TEST CASES

    static void testCase15();
        // Test element access.

    static void testCase14();
        // Test 'resize' and 'max_size'

    static void testCase13();
        // Test 'assign' members.

    template <class CONTAINER>
    static void testCase13Range(const CONTAINER&);
        // Test 'assign' member template.

    static void testCase12();
        // Test user-supplied constructors.

    template <class CONTAINER>
    static void testCase12Range(const CONTAINER&);
        // Test user-supplied constructor templates.

    static void testCase11(const char *t, const char *a);
    static void testCase11(bslmf_MetaInt<0>, const char *t, const char *a);
    static void testCase11(bslmf_MetaInt<1>, const char *t, const char *a);
        // Test allocator-related concerns.  The first overload is called from
        // the main test driver.  The second overload is dispatched when
        // 'ALLOC' is not a bslma-compliant allocator.  The third overload is
        // dispatched when 'ALLOC' is a bslma-compliant allocator. The
        // arguments 't' and 'a' are the names of the parameters 'TYPE' and
        // 'ALLOC', respectively.

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
        // Test basic accessors ('size', 'begin' and 'end').

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

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::getValues(const TYPE **valuesPtr)
{
    bslma_DefaultAllocatorGuard guard(&bslma_NewDeleteAllocator::singleton());

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
int TestDriver<TYPE,ALLOC>::ggg(Obj           *object,
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
            return i;  // Discontinue processing this spec.
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

                                 // ----------
                                 // TEST CASES
                                 // ----------


template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase15()
{
    // --------------------------------------------------------------------
    // TESTING ELEMENT ACCESS
    // Concerns:
    //   1) That 'v.front()' and 'v.back()', allow modifing the
    //      element when 'v' is modifiable, but must not modify its
    //      indexed element when it is 'const'.
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

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
                LOOP_ASSERT(LINE, TYPE(SPEC[0]) == X.front());
                mX.front() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.front());
                LOOP_ASSERT(LINE, Y != X);
                mX.front() = Y.front();
                LOOP_ASSERT(LINE, Y == X);

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
void TestDriver<TYPE,ALLOC>::testCase14()
{
    // --------------------------------------------------------------------
    // TESTING 'resize'
    //
    // Concerns:
    //   1) Resized list has the correct value.
    //   2) Resizing to the current size allocates and frees no memory.
    //   3) Resizing to a smaller size allocates no memory.
    //   4) Resizing to a larger size frees no memory.
    //   5) Resizing to a larger size propagates the allocator to elements
    //      appropriately.
    //   6) 'resize' is exception neutral.
    //
    // Plan:
    //   Using a set of input specs and result sizes, try each combination
    //   with and without specifying a value for the new elements.  Verify
    //   each of the above concerns for each combination.
    //
    // Testing:
    //   void resize(size_type sz);
    //   void resize(size_type sz, const T& c);
    // --------------------------------------------------------------------

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
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

        if (veryVerbose) { T_; P_(SPEC); }

        for (int newlen = 0; newlen < 20; ++newlen)
        {
            const int NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P_(NEWLEN); }

            BEGIN_BSLMA_EXCEPTION_TEST {
                const int AL = testAllocator.allocationLimit();
                testAllocator.setAllocationLimit(-1);

                Obj mX(Z);
                const Obj& X = gg(&mX, SPEC);
                Obj mU(X);  const Obj& U = mU;

                testAllocator.setAllocationLimit(AL);

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();

                mX.resize(NEWLEN);  // test here

                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P_(X);
                }

                LOOP2_ASSERT(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <= LENGTH) {
                    LOOP2_ASSERT(LINE, NEWLEN, BB == AA);
                }
                else {
                    LOOP2_ASSERT(LINE, NEWLEN,
                                 BB + (NEWLEN-LENGTH)*(1+SCOPED_ALLOC) == AA);
                }

                LOOP2_ASSERT(LINE, NEWLEN,
                             B + (NEWLEN - LENGTH) * (1 + SCOPED_ALLOC) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();
                for (int j = 0; j < LENGTH && j < NEWLEN; ++j, ++xi, ++yi) {
                    LOOP2_ASSERT(LINE, NEWLEN, *yi == *xi);
                }
                for (int j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    LOOP2_ASSERT(LINE, NEWLEN, DEFAULT_VALUE == *xi);
                }
                LOOP2_ASSERT(LINE, NEWLEN, xi == X.end());
            } END_BSLMA_EXCEPTION_TEST

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

        if (veryVerbose) { T_; P_(SPEC); }

        for (int newlen = 0; newlen < 20; ++newlen)
        {
            const int NEWLEN = newlen;
            if (veryVerbose) { T_; T_; P_(NEWLEN); }

            BEGIN_BSLMA_EXCEPTION_TEST {
                const int AL = testAllocator.allocationLimit();
                testAllocator.setAllocationLimit(-1);

                Obj mX(Z);
                const Obj& X = gg(&mX, SPEC);
                Obj mU(X);  const Obj& U = mU;

                testAllocator.setAllocationLimit(AL);

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();

                mX.resize(NEWLEN, VALUE);  // test here

                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P_(X);
                }

                LOOP2_ASSERT(LINE, NEWLEN, NEWLEN == X.size());
                if (NEWLEN <= LENGTH) {
                    LOOP2_ASSERT(LINE, NEWLEN, BB == AA);
                }
                else {
                    LOOP2_ASSERT(LINE, NEWLEN,
                                 BB + (NEWLEN-LENGTH)*(1+SCOPED_ALLOC) == AA);
                }

                LOOP2_ASSERT(LINE, NEWLEN,
                             B + (NEWLEN - LENGTH) * (1 + SCOPED_ALLOC) == A);

                const_iterator xi = X.begin();
                const_iterator yi = U.begin();
                for (int j = 0; j < LENGTH && j < NEWLEN; ++j, ++xi, ++yi) {
                    LOOP2_ASSERT(LINE, NEWLEN, *yi == *xi);
                }
                for (int j = LENGTH; j < NEWLEN; ++j, ++xi) {
                    LOOP2_ASSERT(LINE, NEWLEN, VALUE == *xi);
                }
                LOOP2_ASSERT(LINE, NEWLEN, xi == X.end());
            } END_BSLMA_EXCEPTION_TEST

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        } // end for newlen
    } // end testing resize(sz)
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase13()
{
    // --------------------------------------------------------------------
    // TESTING 'assign'
    //   We have the following concerns:
    //    1) The assigned value is correct.
    //    2) The 'assign' call is exception neutral w.r.t. memory allocation.
    //    3) The internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //
    // Plan:
    //   For the assignment we will create objects of varying sizes containing
    //   default values for type T, and then assign different 'value'.  Perform
    //   the above tests:
    //    - With various initial values before the assignment.
    //    - In the presence of exceptions during memory allocations using
    //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
    //   and use basic accessors and equality comparison to verify that
    //   assignment was succesfull.
    //
    // Testing:
    //   assign(size_type n, const T& value);
    // --------------------------------------------------------------------

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
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
                    printf("using default value.\n");
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
                    const int A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X);
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 A == 1 + LENGTH + SCOPED_ALLOC * LENGTH);

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
                    printf("using default value.\n");
                }

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BEGIN_BSLMA_EXCEPTION_TEST {
                        const int AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);
                        const Obj& X = mX;

                        testAllocator.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        const int A = testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X);
                        }

                        LOOP4_ASSERT(INIT_LINE,LINE,i,ti, LENGTH == X.size());
                        LOOP4_ASSERT(INIT_LINE,LINE,i,ti,
                                     A == 1 + LENGTH + SCOPED_ALLOC * LENGTH);

                        for (const_iterator j = X.begin(); j != X.end(); ++j) {
                            LOOP4_ASSERT(INIT_LINE,LINE, i, ti, VALUE == *j);
                        }
                    } END_BSLMA_EXCEPTION_TEST

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
    //   We have the following concerns:
    //    1) That the initial value is correct.
    //    2) That the initial range is correctly imported if the
    //       initial 'FWD_ITER' is an input iterator.
    //    3) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    4) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    5) The previous value is freed properly.
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
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
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
                printf("using default value.\n");
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
                const int A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; T_; P_(X);
                }

                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                             A == 1 + LENGTH + SCOPED_ALLOC * LENGTH);

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
                printf("using default value.\n");
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

                BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, Z);  const Obj& X = mX;

                    testAllocator.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    const int A = testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X);
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                 A == 1 + LENGTH + SCOPED_ALLOC * LENGTH);
                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, Y == X);
                } END_BSLMA_EXCEPTION_TEST

                LOOP_ASSERT(testAllocator.numMismatches(),
                            0 == testAllocator.numMismatches());
                LOOP_ASSERT(testAllocator.numBlocksInUse(),
                            0 == testAllocator.numBlocksInUse());
            }
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase12()
{
    // --------------------------------------------------------------------
    // TESTING CONSTRUCTORS:
    //   We have the following concerns:
    //    1) The initial value is correct.
    //    2) The constructor is exception neutral w.r.t. memory allocation.
    //    3) The internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
    //    4) TBD: The move constructor moves value and allocator
    //       correctly, and without performing any allocation.
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
    //
    // Testing:
    //   list(size_type n, const T& value = T(), const A& a = A());
    //   list(list&& original);
    // --------------------------------------------------------------------

    bslma_TestAllocator  testAllocator(veryVeryVerbose);

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
                    T_; T_; P_(X);
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, ALLOC() == X.get_allocator());

                for (size_t j = 0; j < LENGTH; ++j) {
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
                    printf("using "); P(VALUE);
                }

                Obj mX(LENGTH, VALUE);  const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X);
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, ALLOC() == X.get_allocator());

                for (size_t j = 0; j < LENGTH; ++j) {
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
                    printf("using "); P(VALUE);
                }

                const int BB = testAllocator.numBlocksTotal();
                const int  B = testAllocator.numBlocksInUse();

                Obj mX(LENGTH, VALUE, AL);
                const Obj& X = mX;

                const int AA = testAllocator.numBlocksTotal();
                const int  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    T_; T_; P_(X);
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, AL == X.get_allocator());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == nthElem(X,j));
                }

                LOOP2_ASSERT(LINE, ti, AA ==
                             BB + 1 + (int)LENGTH + (int)LENGTH * SCOPED_ALLOC);
                LOOP2_ASSERT(LINE, ti, A ==
                             B + 1 + (int)LENGTH + (int)LENGTH * SCOPED_ALLOC);
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
                    printf("using "); P(VALUE);
                }

                const int BB = testAllocator.numBlocksTotal();
                const int  B = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BEGIN_BSLMA_EXCEPTION_TEST {

                    Obj mX(LENGTH, VALUE, AL);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X);
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, VALUE == nthElem(X,j));
                    }

                } END_BSLMA_EXCEPTION_TEST

                const int AA = testAllocator.numBlocksTotal();
                const int  A = testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

                // The number of allocations, 'ALLOCS', needed for successful
                // construction of a list of length
                // 'LENGTH' is '1 + LENGTH + SCOPED_ALLOC * LENGTH', where
                // 'SCOPED_ALLOC' is 1 if the element type uses the container's
                // allocator and 0 otherwise.  Because we are retrying on each
                // exception, the number of allocations by the time we succeed
                // will be 'SUM(1 .. ALLOCS)', which is easily computed as
                // 'ALLOCS * (ALLOCS+1) / 2'.

                const int ALLOCS = 1 + LENGTH + SCOPED_ALLOC * LENGTH;
                const int TOTAL_ALLOCS = ALLOCS * (ALLOCS+1) / 2;
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
                (void)LINE;

                if (verbose) {
                    printf("\t\tCreating object of "); P(LENGTH);
                }

                {
                    const int TB = defaultAllocator_p->numBlocksInUse();
                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    ASSERT(0  == objectAllocator_p->numBlocksInUse());

                    const ALLOC AL(objectAllocator_p);
                    Obj x(LENGTH, DEFAULT_VALUE, AL);

                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    if (TypeHasBslmaAlloc::VALUE && !ObjHasBslmaAlloc::VALUE) {
                        // If TYPE uses bslma but Obj does not, then each
                        // element will allocate one block from the default
                        // allocator.
                        ASSERT(TB + LENGTH ==
                               defaultAllocator_p->numBlocksInUse());
                    }
                    else {
                        // Default allocator is not used
                        ASSERT(TB == defaultAllocator_p->numBlocksInUse());
                    }
                    ASSERT(1 + LENGTH + LENGTH*SCOPED_ALLOC ==
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
                (void)LINE;

                if (verbose) {
                    printf("\t\tCreating object of "); P_(LENGTH);
                    printf("using "); P(VALUE);
                }

                {
                    const int TB = defaultAllocator_p->numBlocksInUse();
                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    ASSERT(0  == objectAllocator_p->numBlocksInUse());

                    const ALLOC AL(objectAllocator_p);
                    Obj x(LENGTH, VALUE, AL);

                    ASSERT(0  == globalAllocator_p->numBlocksInUse());
                    if (TypeHasBslmaAlloc::VALUE && !ObjHasBslmaAlloc::VALUE) {
                        // If TYPE uses bslma but Obj does not, then each
                        // element will allocate one block from the default
                        // allocator.
                        ASSERT(TB + LENGTH ==
                               defaultAllocator_p->numBlocksInUse());
                    }
                    else {
                        // Default allocator is not used
                        ASSERT(TB == defaultAllocator_p->numBlocksInUse());
                    }
                    ASSERT(1 + LENGTH + LENGTH*SCOPED_ALLOC ==
                           objectAllocator_p->numBlocksInUse());
                }

                ASSERT(0 == globalAllocator_p->numBlocksInUse());
                ASSERT(0 == objectAllocator_p->numBlocksInUse());
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
    //    2) That the initial range is correctly imported if the
    //       initial 'FWD_ITER' is an input iterator.
    //    3) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    4) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    const int INPUT_ITERATOR_TAG =
          bslmf_IsSame<std::input_iterator_tag,
                       typename bsl::iterator_traits<
                         typename CONTAINER::const_iterator>::iterator_category
                      >::VALUE;

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

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;

            Obj mX(U.begin(), U.end());  const Obj& X = mX;

            if (veryVerbose) {
                T_; T_; P_(X);
            }

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

            const int BB = testAllocator.numBlocksTotal();
            const int  B = testAllocator.numBlocksInUse();

            ALLOC AL(&testAllocator);
            Obj mX(U.begin(), U.end(), AL); const Obj& X = mX;

            const int AA = testAllocator.numBlocksTotal();
            const int  A = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X);
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

            Obj mY(g(SPEC));  const Obj& Y = mY;
            LOOP2_ASSERT(LINE, ti, Y == X);

            LOOP2_ASSERT(LINE, ti,
                         BB + 1 + LENGTH + LENGTH * SCOPED_ALLOC == AA);
            LOOP2_ASSERT(LINE, ti,
                         B + 1 + LENGTH + LENGTH * SCOPED_ALLOC == A);
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
                printf("using "); P(SPEC);
            }

            CONTAINER mU(SPEC);  const CONTAINER& U = mU;
            Obj mY(g(SPEC));     const Obj& Y = mY;

            const int BB = testAllocator.numBlocksTotal();
            const int  B = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

            BEGIN_BSLMA_EXCEPTION_TEST {

                ALLOC AL(&testAllocator);
                Obj mX(U.begin(), U.end(), AL); const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X);
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
                LOOP2_ASSERT(LINE, ti, Y == X);

            } END_BSLMA_EXCEPTION_TEST

            const int AA = testAllocator.numBlocksTotal();
            const int  A = testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            // The number of allocations, 'ALLOCS', needed for successful
            // construction of a list of length 'LENGTH' is '1 + LENGTH +
            // SCOPED_ALLOC * LENGTH', where 'SCOPED_ALLOC' is 1 if the element
            // type uses the container's allocator and 0 otherwise.  Because
            // we are retrying on each exception, the number of allocations by
            // the time we succeed will be 'SUM(1 .. ALLOCS)', which is easily
            // computed as 'ALLOCS * (ALLOCS+1) / 2'.

            const int ALLOCS = 1 + LENGTH + SCOPED_ALLOC * LENGTH;
            const int TOTAL_ALLOCS = ALLOCS * (ALLOCS+1) / 2;
            LOOP2_ASSERT(LINE, ti, BB + TOTAL_ALLOCS == AA);
            LOOP2_ASSERT(LINE, ti, B + 0 == A);

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase11(bslmf_MetaInt<1>,
                                        const char *t, const char *a)
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS
    //
    // This template specialization is for containers that use bslma_Allocator.
    //
    // Concerns:
    //      1. The list class has the 'bslalg_TypeTraitUsesBslmaAllocator'
    //         trait.
    //      2. The allocator is not copied when the list is copy-constructed.
    //      3. The allocator is set with the extended copy-constructor.
    //      4. The allocator is passed through to elements if the elements
    //         also use bslma_Allocator.
    //      5. Creating an empty list allocates exactly one block.
    //
    // Testing:
    //   Allocator traits and allocator propagation
    // --------------------------------------------------------------------

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT(ObjHasBslmaAlloc::VALUE);

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void)NUM_VALUES;

    if (verbose)
        printf("\nTesting 'bslalg_TypeTraitUsesBslmaAllocator'.\n");

    LOOP2_ASSERT(t, a,
                 (bslalg_HasTrait<Obj,
                                  bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

    if (verbose)
        printf("\nTesting that empty list allocates one block.\n");
    {
        const int BB = testAllocator.numBlocksTotal();
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
        bslma_TestAllocator a2;
        Obj mZ(X,&a2);           const Obj& Z = mZ;

        // Allocator set to a2 (not copied)
        LOOP2_ASSERT(t, a, &testAllocator != Z.get_allocator());
        LOOP2_ASSERT(t, a, &a2            == Z.get_allocator());
    }

    if (verbose)
        printf("\nTesting passing allocator through to elements.\n");

    const int DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE)
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
            bslma_TestAllocator *const elemAlloc =
                dynamic_cast<bslma_TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &OtherAllocatorDefaultImp == elemAlloc);
            LOOP2_ASSERT(t, a, DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(&testAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma_TestAllocator *const elemAlloc =
                dynamic_cast<bslma_TestAllocator*>(X.back().allocator());
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
void TestDriver<TYPE,ALLOC>::testCase11(bslmf_MetaInt<0>,
                                        const char *t, const char *a)
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS FOR NON-BSLMA ALLOCATORS
    //
    // This template specialization is for containers that use non-bslma
    // Allocators.
    //
    // Concerns:
    //      1. The list class does not have the
    //         'bslalg_TypeTraitUsesBslmaAllocator' trait.
    //      2. The allocator is not passed through to elements
    //      3. The allocator is set with the extended copy-constructor.
    //      4. The allocator is copied when the list is copy-constructed.
    //      5. Creating an empty list allocates exactly one block.
    //
    // Testing:
    //   Allocator traits and propagation
    // --------------------------------------------------------------------

    // Compile-time assert that this is the correct specialization.
    BSLMF_ASSERT( !ObjHasBslmaAlloc::VALUE );

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    OtherAllocator<char> objAllocator(&testAllocator);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void)NUM_VALUES;

    if (verbose)
        printf("\nTesting 'bslalg_TypeTraitUsesBslmaAllocator'.\n");

    LOOP2_ASSERT(t, a,
                 (!bslalg_HasTrait<Obj,
                                  bslalg_TypeTraitUsesBslmaAllocator>::VALUE));

    if (verbose)
        printf("\nTesting that empty list allocates one block.\n");
    {
        const int BB = testAllocator.numBlocksTotal();
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
        bslma_TestAllocator  a2;
        OtherAllocator<char> objAlloc2(&a2);
        Obj mZ(X,objAlloc2);     const Obj& Z = mZ;

        // Allocator set to objAlloc2 (not copied)
        LOOP2_ASSERT(t, a, objAllocator   != Z.get_allocator());
        LOOP2_ASSERT(t, a, objAlloc2      == Z.get_allocator());
    }

    if (verbose)
        printf("\nTesting that allocator is not passed through to elements.\n");

    const int DD = OtherAllocatorDefaultImp.numBlocksInUse();
    if (bslalg_HasTrait<TYPE, bslalg_TypeTraitUsesBslmaAllocator>::VALUE)
    {
        // Elements in container should use default allocator while the
        // container itself uses 'testAllocator'.  Set the default allocator
        // here so that we can track its use.
        bslma_TestAllocator defAlloc(veryVeryVerbose);
        bslma_DefaultAllocatorGuard g(&defAlloc);

        {
            Obj mX(1, VALUES[0], objAllocator);  const Obj& X = mX;
            bslma_TestAllocator *const elemAlloc =
                dynamic_cast<bslma_TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &defAlloc == elemAlloc);
            LOOP2_ASSERT(t, a, 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma_TestAllocator *const elemAlloc =
                dynamic_cast<bslma_TestAllocator*>(X.back().allocator());
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
            bslma_TestAllocator *const elemAlloc =
                dynamic_cast<bslma_TestAllocator*>(X.back().allocator());
            LOOP2_ASSERT(t, a, objAllocator == X.get_allocator());
            LOOP2_ASSERT(t, a, &testAllocator != elemAlloc);
            LOOP2_ASSERT(t, a, 2 == testAllocator.numBlocksInUse());
            LOOP2_ASSERT(t, a, &OtherAllocatorDefaultImp == elemAlloc);
            LOOP2_ASSERT(t, a, DD + 1 == elemAlloc->numBlocksInUse());
        }
        {
            Obj mX(objAllocator);  const Obj& X = mX;
            mX.push_back(VALUES[0]);
            bslma_TestAllocator *const elemAlloc =
                dynamic_cast<bslma_TestAllocator*>(X.back().allocator());
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
void TestDriver<TYPE,ALLOC>::testCase11(const char *t, const char *a)
{
    // --------------------------------------------------------------------
    // TEST ALLOCATOR-RELATED CONCERNS FOR NON-BSLMA ALLOCATORS
    //
    // This template specialization is for containers that use non-bslma
    // Allocators.
    //
    // Concerns:
    //   For ALLOC that is a bslma_Allocator
    //      1. The list class has the 'bslalg_TypeTraitUsesBslmaAllocator'
    //         trait.
    //      2. The allocator is not copied when the list is copy-constructed.
    //      3. The allocator is set with the extended copy-constructor.
    //      4. The allocator is passed through to elements if the elements
    //         also use bslma_Allocator.
    //      5. Creating an empty list allocates exactly one block.
    //   For ALLOC that is not a bslma allocator
    //      1. The list class does not have the
    //         'bslalg_TypeTraitUsesBslmaAllocator' trait.
    //      2. The allocator is not passed through to elements
    //      3. The allocator is set with the extended copy-constructor.
    //      4. The allocator is copied when the list is copy-constructed.
    //      5. Creating an empty list allocates exactly one block.
    //
    // Testing:
    //   Allocator traits and propagation
    // --------------------------------------------------------------------

    // Dispatch to the appropriate function
    testCase11(ObjHasBslmaAlloc(), t, a);
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase9()
{
    // --------------------------------------------------------------------
    // TESTING ASSIGNMENT OPERATOR:
    // We have the following concerns:
    //   1.  The 'rhs' value must not be affected by the operation.
    //   2.  'rhs' going out of scope has no effect on the value of 'lhs'
    //       after the assignment.
    //   3.  After the assignment, no element of the 'lhs' has the same
    //       address an element of 'rhs' (except in the case of
    //       self-assignment).
    //   4.  Aliasing (x = x): The assignment operator must always work --
    //         even when the lhs and rhs are identically the same object.
    //   5.  The assignment operator must be neutral with respect to memory
    //       allocation exceptions.
    //   6.  The 'lhs' object must allocate all of its internal memory from
    //       its own allocator, even of 'rhs' uses a different allocator.
    //   7.  TBD: Move assignment moves the value but not the allocator.
    //       Memory is allocated only if the 'lhs' and 'rhs' allocators are
    //       different.
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
    //   list<T,A>& operator=(const list<T,A>& rhs);
    // --------------------------------------------------------------------

    bslma_TestAllocator testAllocator1(veryVeryVerbose);
    bslma_TestAllocator testAllocator2(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    (void)NUM_VALUES;

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
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == UU.size());   // same lengths

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const size_t      vLen   = strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length %d:\t", vLen);
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

                        const int BB1 = testAllocator1.numBlocksTotal();
                        const int B1  = testAllocator1.numBlocksInUse();
                        const int BB2 = testAllocator2.numBlocksTotal();
                        const int B2  = testAllocator2.numBlocksInUse();

                        mU = V; // test assignment here

                        const int AA1 = testAllocator1.numBlocksTotal();
                        const int A1  = testAllocator1.numBlocksInUse();
                        const int AA2 = testAllocator2.numBlocksTotal();
                        const int A2  = testAllocator2.numBlocksInUse();

                        // The assignment may construct as many as V.size()
                        // objects and may destroy as many as U.size()
                        // objects, but could (through the use of
                        // element-by-element assignment) construct and
                        // destroy fewer elements.
                        const int NUM_CTOR = numCopyCtorCalls - NUM_CTOR_BEFORE;
                        const int NUM_DTOR =
                            numDestructorCalls - NUM_DTOR_BEFORE;
                        ASSERT(NUM_CTOR <= (int)V.size());
                        ASSERT(NUM_DTOR <= (int)U_LEN_BEFORE);

                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  V == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, BB1 == AA1);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, B1  == A1 );
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                     BB2 + vLen * (1 + SCOPED_ALLOC) >= AA2);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                     B2 + (vLen-uLen) * (1+SCOPED_ALLOC) == A2);
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
            const size_t    uLen   = (int) strlen(U_SPEC);

            if (verbose) {
                printf("\tFor lhs objects of length %d:\t", uLen);
                P(U_SPEC);
            }

            const Obj UU = g(U_SPEC);  // control
            LOOP_ASSERT(ui, uLen == UU.size()); // same lengths

            // int vOldLen = -1;
            for (int vi = 0; SPECS[vi]; ++vi) {
                const char *const V_SPEC = SPECS[vi];
                const size_t    vLen   = (int) strlen(V_SPEC);

                if (veryVerbose) {
                    printf("\t\tFor rhs objects of length %d:\t",
                           vLen);
                    P(V_SPEC);
                }

                // control
                const Obj VV = g(V_SPEC);

                // Exception-test macros muse use 'testAllocator':
                bslma_TestAllocator& testAllocator = testAllocator2;
                BEGIN_BSLMA_EXCEPTION_TEST {
                    // We want to use the allocation limit only for the
                    // assignment operation, not for producing the initial
                    // objects.  Thus, we save the limit in AL and turn off
                    // the limit until we're ready to test assignment.
                    const int AL = testAllocator.allocationLimit();
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

                        const int BB2 = testAllocator2.numBlocksTotal();
                        const int B2  = testAllocator2.numBlocksInUse();

                        testAllocator2.setAllocationLimit(AL);
                        mU = V; // test assignment here

                        const int AA2 = testAllocator2.numBlocksTotal();
                        const int A2  = testAllocator2.numBlocksInUse();

                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  V == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                     BB2 + vLen * (1 + SCOPED_ALLOC) >= AA2);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,
                                     B2 + (vLen-uLen) * (1+SCOPED_ALLOC) == A2);
                    }
                    // 'mV' (and therefore 'V') now out of scope
                    LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                } END_BSLMA_EXCEPTION_TEST
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
            bslma_TestAllocator& testAllocator = testAllocator2;
            BEGIN_BSLMA_EXCEPTION_TEST {
                // We want to use the allocation limit only for the
                // assignment operation, not for producing the initial
                // objects.  Thus, we save the limit in AL and turn off
                // the limit until we're ready to test assignment.
                const int AL = testAllocator.allocationLimit();
                testAllocator2.setAllocationLimit(-1);

                ALLOC AL2(&testAllocator2);
                Obj mY(AL2);
                const Obj& Y = gg(&mY, SPEC);

                if (veryVerbose) { T_; T_; P(Y); }

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);

                const int B2 = testAllocator2.numBlocksInUse();

                testAllocator2.setAllocationLimit(AL);
                {
                    ExceptionGuard<Obj> guard(&mY, Y, L_);
                    mY = Y; // test assignment here
                }

                const int A2 = testAllocator2.numBlocksInUse();

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);
                LOOP_ASSERT(SPEC, B2 == A2);
            } END_BSLMA_EXCEPTION_TEST
        } // end for (ti)
    } // end self-assignment test
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
    //   list g(const char *spec);
    // --------------------------------------------------------------------

    bslma_TestAllocator testAllocator(veryVeryVerbose);

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
        const Obj& X = gg(&mX, SPEC);

        if (veryVerbose) {
            printf("\t g = "); dbg_print(g(SPEC)); printf("\n");
            printf("\tgg = "); dbg_print(X); printf("\n");
        }
        const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
        const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, X == g(SPEC));
        const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
        const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
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
    //        the correct allocator.
    //   2) The value of the original object is left unaffected.
    //   3) Subsequent changes in or destruction of the source object have
    //        no effect on the copy-constructed object.
    //   4) Subsequent changes ('push_back's) on the created object have no
    //        effect on the original.
    //   5) The object has its internal memory management system hooked up
    //        properly so that *all* internally allocated memory draws
    //        from a user-supplied allocator whenever one is specified.
    //   6) The function is exception neutral w.r.t. memory allocation.
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
    //    - Where the object is constructed with an object allocator, and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concern 6, perform tests for concern 1 performed
    //   in the presence of exceptions during memory allocations using a
    //   'bslma_TestAllocator' and varying its *allocation* *limit*.
    //
    // Testing:
    //   list<T,A>(const list<T,A>& original);
    // --------------------------------------------------------------------

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_MOVE  = bslalg_HasTrait<TYPE,
                                        bslalg_TypeTraitBitwiseMoveable>::VALUE
                         ? 0 : 1;  // if moveable, moves do not count as allocs
    const int SCOPED_ALLOC = bslalg_HasTrait<TYPE,
                                    bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose)
        printf("\nTesting parameters: SCOPED_ALLOC = %d, TYPE_MOVE = %d.\n",
               SCOPED_ALLOC, TYPE_MOVE);
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
            const size_t    LENGTH = (int) strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length %d:\n", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int)LENGTH); // strictly increasing
            oldLen = LENGTH;

            // Create control object w.
            Obj mW; gg(&mW, SPEC);
            const Obj& W = mW;

            LOOP_ASSERT(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj *pX = new Obj(&testAllocator);
            Obj& mX = *pX;

            const Obj& X = mX;  gg(&mX, SPEC);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1.

                if (veryVerbose) { printf("\t\t\tRegular Case :"); }

                const Obj Y0(X);

                if (veryVerbose) {
                    printf("\tObj : "); P_(Y0);
                }

                LOOP_ASSERT(SPEC, W == Y0);
                LOOP_ASSERT(SPEC, W == X);
                LOOP_ASSERT(SPEC, Y0.get_allocator() ==
                             bslma_Default::defaultAllocator());
            }
            {   // Testing concern 4.

                if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                        "without test allocator:\n");

                Obj Y1(X);

                Y1.push_back(VALUES[Y1.size() % NUM_VALUES]);

                LOOP_ASSERT(SPEC, Y1.size() == LENGTH + 1);
                LOOP_ASSERT(SPEC, W != Y1);
                LOOP_ASSERT(SPEC, X != Y1);
                LOOP_ASSERT(SPEC, W == X);
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                const int BB = testAllocator.numBlocksTotal();
                const int  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                Obj Y11(X, &testAllocator);

                const int AA = testAllocator.numBlocksTotal();
                const int  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    printf("\t\t\t\tBefore Append: "); P(Y11);
                }

                const int SCOPED_ALLOCS = SCOPED_ALLOC * LENGTH;
                LOOP_ASSERT(SPEC, BB + 1 + LENGTH + SCOPED_ALLOCS == AA);
                LOOP_ASSERT(SPEC,  B + 1 + LENGTH + SCOPED_ALLOCS ==  A);

                const int CC = testAllocator.numBlocksTotal();
                const int  C = testAllocator.numBlocksInUse();

                Y11.push_back(VALUES[LENGTH % NUM_VALUES]);

                const int DD = testAllocator.numBlocksTotal();
                const int  D = testAllocator.numBlocksInUse();

                // Allocations should increase by one node block for the list.
                // If TYPE uses an allocator, allocations hould increase by
                // one more block.
                LOOP_ASSERT(SPEC, CC + 1 + SCOPED_ALLOC == DD);
                LOOP_ASSERT(SPEC, C  + 1 + SCOPED_ALLOC == D );

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
            {   // Exception checking.

                const int BB = testAllocator.numBlocksTotal();
                const int  B = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                }

                size_t allocations = 0;
                BEGIN_BSLMA_EXCEPTION_TEST {
                    allocations += bslmaExceptionCounter;
                    const Obj Y2(X, &testAllocator);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    LOOP_ASSERT(SPEC, W == Y2);
                    LOOP_ASSERT(SPEC, W == X);
                    LOOP_ASSERT(SPEC, Y2.get_allocator() == X.get_allocator());
                } END_BSLMA_EXCEPTION_TEST

                const int AA = testAllocator.numBlocksTotal();
                const int  A = testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                }

                LOOP_ASSERT(SPEC, BB + allocations == AA);
                LOOP_ASSERT(SPEC,  B + 0 ==  A);
            }
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
                LOOP_ASSERT(SPEC, W != Y5);
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
    //   1) Objects constructed with the same values compare equal.
    //   2) Objects constructed such that they have same (logical) value but
    //      different internal representation (due to the lack or presence
    //      of an allocator) always compare as equal.
    //   3) Unequal objects are always compare as unequal.
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

    bslma_TestAllocator testAllocator1(veryVeryVerbose);
    bslma_TestAllocator testAllocator2(veryVeryVerbose);

    bslma_Allocator *ALLOCATOR[] = {
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
    int oldLen = -1;

    // Create first object
    for (int si = 0; SPECS[si]; ++si) {
        for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

            const char *const U_SPEC = SPECS[si];
            const size_t    LENGTH = strlen(U_SPEC);

            Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
            LOOP2_ASSERT(si, ai, LENGTH == U.size()); // same lengths

            if ((int)LENGTH != oldLen) {
                if (verbose)
                    printf( "\tUsing lhs objects of length %d.\n", LENGTH);
                LOOP_ASSERT(U_SPEC, oldLen <= (int)LENGTH);//non-decreasing
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
void TestDriver<TYPE,ALLOC>::testCase4()
{
    // --------------------------------------------------------------------
    // TESTING BASIC ACCESSORS:
    // Concerns:
    //   0) size() returns the list size.
    //   1) begin() == end() if and only if the list is empty
    //   2) Iterating from begin() to end() will visit every value in a
    //      list and only the values in that list.
    //   3) Iteration works for both const containers (using const_iterator)
    //      and non-const containers (using iterator).
    //   4) empty() returns true iff size() return 0
    //   5) The test function succ() increments an iterator by n.
    //   6) The test function nthElem() returns the nth element of a list
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
    // --------------------------------------------------------------------

    bslma_TestAllocator testAllocator(veryVeryVerbose);
    bslma_TestAllocator testAllocator1(veryVeryVerbose);
    bslma_TestAllocator testAllocator2(veryVeryVerbose);

    bslma_Allocator *ALLOCATOR[] = {
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
                        "begin() and end().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const size_t      LENGTH = DATA[ti].d_length;
            const char *const EXP    = DATA[ti].d_elements;

            ASSERT(LENGTH <= MAX_LENGTH);

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                Obj mX(ALLOCATOR[ai]);

                const Obj& X = gg(&mX, SPEC);    // canonical organization

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths
                LOOP2_ASSERT(ti, ai, (LENGTH == 0) == X.empty());

                if (veryVerbose) {
                    printf( "\ton objects of length %d:\n", LENGTH);
                }

                // non-decreasing
                LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                oldLen = LENGTH;

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                size_t i;
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
                Obj mX(ALLOCATOR[ai]);

                const Obj& X = gg(&mX, SPEC);

                LOOP2_ASSERT(ti, ai, LENGTH == X.size()); // same lengths
                LOOP2_ASSERT(ti, ai, (LENGTH == 0) == X.empty());

                if (veryVerbose) {
                    printf("\tOn objects of length %d:\n", LENGTH);
                }

                // non-decreasing
                LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                oldLen = LENGTH;

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                Obj mY(ALLOCATOR[ai]); const Obj& Y = mY;

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
    //   character (denoting 'clear').
    //
    // Testing:
    //   list<T,A>& gg(list<T,A> *object, const char *spec);
    //   int ggg(list<T,A> *object, const char *spec, int vF = 1);
    // --------------------------------------------------------------------

    bslma_TestAllocator testAllocator(veryVeryVerbose);

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
            const int          LINE   = DATA[ti].d_lineNum;
            const char *const SPEC    = DATA[ti].d_spec_p;
            const int         INDEX   = DATA[ti].d_index;
            const size_t      LENGTH  = (int)strlen(SPEC);

            Obj mX(&testAllocator);

            if ((int)LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
                // LOOP_ASSERT(LINE, oldLen <= (int)LENGTH);  // non-decreasing
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
    //        1b) allocates exactly one block.
    //        1c) has the internal memory management system hooked up
    //              properly so that *all* internally allocated memory
    //              draws from the same user-supplied allocator whenever
    //              one is specified.
    //    2) The destructor properly deallocates all allocated memory to
    //         its corresponding allocator from any attainable state.
    //    3) 'push_back'
    //        3a) produces the expected value.
    //        3b) maintains valid internal state.
    //        3c) is exception neutral with respect to memory allocation.
    //        3d) does not change the address of any other list elements
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
    //        a 'bslma_TestAllocator' and varying its *allocation* *limit*.
    //    - Where the object is constructed with an object allocator and
    //        neither of global and default allocator is used to supply memory.
    //
    //   To address concerns 3a - 3d, construct a series of independent
    //   objects, ordered by increasing length.  In each test, allow the
    //   object to leave scope without further modification, so that the
    //   destructor asserts internal object invariants appropriately.
    //   After the final insert operation in each test, use the (untested)
    //   basic accessors to cross-check the value of the object
    //   and the 'bslma_TestAllocator' to confirm whether memory allocaiton has
    //   occurred.
    //
    //   To address concerns 4a-4c, construct a similar test, replacing
    //   'push_back' with 'clear'; this time, however, use the test
    //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
    //
    //   To address concerns 2, 3d, 4d, create a small "area" test that
    //   exercises the construction and destruction of objects of various
    //   lengths in the presence of memory allocation exceptions.  Two
    //   separate tests will be performed:
    //
    //     Let S be the sequence of integers { 0 .. N - 1 }.
    //      (1) for each i in S, use the default constructor and 'push_back'
    //          to create an instance of length i, confirm its value (using
    //           basic accessors), and let it leave scope.
    //      (2) for each (i, j) in S X S, use 'push_back' to create an
    //          instance of length i, use 'clear' to clear its value
    //          and confirm (with 'length'), use insert to set the instance
    //          to a value of length j, verify the value, and allow the
    //          instance to leave scope.
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    const int TYPE_MOVE = bslalg_HasTrait<TYPE,
                                        bslalg_TypeTraitBitwiseMoveable>::VALUE
                            ? 0 : 1;
    const int SCOPED_ALLOC  = bslalg_HasTrait<TYPE,
                                    bslalg_TypeTraitUsesBslmaAllocator>::VALUE;

    if (verbose)
        printf("\tTesting parameters: SCOPED_ALLOC = %d, TYPE_MOVE = %d.\n",
               SCOPED_ALLOC, TYPE_MOVE);

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
        const int AA = testAllocator.numBlocksTotal();
        const int A  = testAllocator.numBlocksInUse();

        const Obj X(&testAllocator);

        const int BB = testAllocator.numBlocksTotal();
        const int B  = testAllocator.numBlocksInUse();

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());

        ASSERT(AA + 1 == BB);
        ASSERT(A + 1  == B);
    }

    if (verbose) printf("\t\tIn place using a buffer allocator.\n");
    {
        ASSERT(0 == globalAllocator_p->numBlocksInUse());
        ASSERT(0 == defaultAllocator_p->numBlocksInUse());
        ASSERT(0 == objectAllocator_p->numBlocksInUse());

        Obj x(objectAllocator_p);

        ASSERT(0 == globalAllocator_p->numBlocksInUse());
        ASSERT(0 == defaultAllocator_p->numBlocksInUse());
        ASSERT(1 == objectAllocator_p->numBlocksInUse());
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
                printf("\t\tOn an object of initial length %d.\n", li);

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
            for (int i = 0; i < li; ++it, ++i) {
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
                    printf("\t\tOn an object of initial length %d.\n", li);

            Obj mX(&testAllocator);  const Obj& X = mX;
            const TYPE *elemAddrs[NUM_TRIALS];

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
                elemAddrs[i] = &X.back();
            }

            LOOP_ASSERT(li, li == X.size());

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);
            elemAddrs[li] = &X.back();

            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P(X);
            }

            // When the type allocates, an extra allocation is used for the
            // new element, and when the type is not bitwise moveable, size()
            // allocations are used during the move, but an equal amount is
            // destroyed thus the number of blocks in use is unchanged.

            const int SCOPED_ALLOC_MOVES = SCOPED_ALLOC;
            LOOP_ASSERT(li, BB + 1 + SCOPED_ALLOC_MOVES == AA);
            LOOP_ASSERT(li, B + 1 + SCOPED_ALLOC == A);

            LOOP_ASSERT(li, li + 1 == X.size());

            const_iterator it = X.begin();
            for (int i = 0; i < li; ++it, ++i) {
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
                    printf("\t\tOn an object of initial length %d.\n", li);

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
            for (int i = 0; i < li; ++it, ++i) {
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
                    printf("\t\tOn an object of initial length %d.\n", li);

            Obj mX(&testAllocator);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const int BB = testAllocator.numBlocksTotal();
            const int B  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P(X);
            }

            mX.clear();

            const int AA = testAllocator.numBlocksTotal();
            const int A  = testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const int CC = testAllocator.numBlocksTotal();
            const int C  = testAllocator.numBlocksInUse();

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P(X);
            }

            LOOP_ASSERT(li, li == X.size());

            LOOP_ASSERT(li, BB == AA);
            LOOP_ASSERT(li, B - (li + int(li) * SCOPED_ALLOC) == A);

            LOOP_ASSERT(li, BB + (li + int(li) * SCOPED_ALLOC) == CC);
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
            if (verbose) printf("\t\t\tOn an object of length %d.\n", li);

          BEGIN_BSLMA_EXCEPTION_TEST {

            Obj mX(&testAllocator);  const Obj& X = mX;              // 1.
            const TYPE *elemAddrs[NUM_TRIALS];
            for (size_t i = 0; i < li; ++i) {                        // 2.
                ExceptionGuard<Obj> guard(&mX, X, L_);
                mX.push_back(VALUES[i % NUM_VALUES]);
                elemAddrs[i] = &X.back();
                guard.release();
            }

            LOOP_ASSERT(li, li == X.size());                         // 3.
            const_iterator it = X.begin();
            for (int i = 0; i < li; ++it, ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == *it);
                LOOP2_ASSERT(li, i, elemAddrs[i] == &*it);
            }

          } END_BSLMA_EXCEPTION_TEST                                 // 4.
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
                printf("\t\t\tOn an object of initial length %d.\n", i);

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVerbose)
                    printf("\t\t\t\tAnd with final length %d.\n", j);

              BEGIN_BSLMA_EXCEPTION_TEST {
                size_t k; // loop index

                Obj mX(&testAllocator);  const Obj& X = mX;         // 1.
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

              } END_BSLMA_EXCEPTION_TEST                            // 8.
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

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);
    const TYPE&         A = VALUES[0];
    const TYPE&         B = VALUES[1];
    const TYPE&         C = VALUES[2];
    (void)NUM_VALUES;

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
    bslma_TestAllocator objectAllocator("Object Allocator",
                                        veryVeryVeryVerbose);
    objectAllocator_p = &objectAllocator;

    // Default Test Allocator.
    bslma_TestAllocator defaultAllocator("Default Allocator",
                                         veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);
    defaultAllocator_p = &defaultAllocator;

    // Global Test Allocator.
    bslma_TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma_Allocator *originalGlobalAllocator =
                           bslma_Default::setGlobalAllocator(&globalAllocator);
    globalAllocator_p = &globalAllocator;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
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
        TestDriver<char>::testCase15();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase15();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING resize
        //
        // Concerns:
        //   For 'max_size', the reported value is no more than one less than
        //     the maximum allowed by the allocator.
        //   For 'max_size', the allocator's 'max_size' is honored.
        //   For 'resize', see 'testCase14' for a list of concerns and plan.
        //
        // Plan:
        //   Using the default allocator, test that 'max_size' returns a value
        //   no larger than all of memory divided by the size of one element.
        //   Repeat this test with 'char' and TestType' element types.  Using
        //   the 'SmallAllocator', test that 'max_size' returns the same value
        //   as 'SmallAllocator<T>::max_size()', except that a node of
        //   overhead is allowed to be subtracted from the result.
        //
        //   For 'resize', call 'testCase14()' with different combinations of
        //   element types and allocator types.  See 'testCase14' for details.
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

        if (verbose) printf("\n... with 'int' and 'SmallAllocator.\n");
        {
            list<int,SmallAllocator<int> > X;
            // SmallAlloc will return the same 'max_size' regardless of the
            // type on which it is instantiated.  Thus, it will report that it
            // can allocate the same number of nodes as 'int's.  (This
            // behavior is not typical for an allocator, but works for this
            // test.)  The 'list' should have no more than one node of
            // overhead.
            ASSERT(SmallAllocator<int>::max_size() >= X.max_size());
            ASSERT(SmallAllocator<int>::max_size() - 1 <= X.max_size());
        }

        if (verbose) printf("\nTesting 'resize'.\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase14();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase14();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testCase14();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testCase14();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testCase14();

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT
        //
        // Concerns:
        //   Specific concerns are listed in 'testCase13' and 'testCase13Range'
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
        TestDriver<char>::testCase13();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase13();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testCase13();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testCase13();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testCase13();

        if (verbose) printf("\nTesting Initial-Range Assignment"
                            "\n================================\n");

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary input iterator.\n");
        TestDriver<char>::testCase13Range(InputSeq<char>());

        if (verbose) printf("\n... with 'char' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<char>::testCase13Range(RandSeq<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<T>::testCase13Range(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "and arbitrary input iterator.\n");
        TestDriver<TOA>::testCase13Range(InputSeq<TOA>());

        if (verbose) printf("\n... with 'TestType', 'OtherAlloc', "
                            "and arbitrary input iterator.\n");
        TestDriver<T,OAT>::testCase13Range(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc', 'OtherAlloc', "
                            "and arbitrary input iterator.\n");
        TestDriver<TOA,OATOA>::testCase13Range(InputSeq<TOA>());

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   Specific concerns are listed in 'testCase12' and 'testCase12Range'
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
        TestDriver<char>::testCase12();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase12();

        if (verbose) printf("\n... with 'TestType and other allocator'.\n");
        TestDriver<T,OAT>::testCase12();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testCase12();

        if (verbose)
            printf("\n... with 'TestTypeOtherAlloc and other allocator'.\n");
        TestDriver<TOA,OATOA>::testCase12();

        if (verbose) printf("\nTesting Initial-Range Constructor"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char' "
                            "an arbitrary input iterator.\n");
        TestDriver<char>::testCase12Range(InputSeq<char>());

        if (verbose) printf("\n... with 'char' "
                            "an arbitrary random-access iterator.\n");
        TestDriver<char>::testCase12Range(RandSeq<char>());

        if (verbose) printf("\n... with 'TestType' "
                            "an arbitrary input iterator.\n");
        TestDriver<T>::testCase12Range(InputSeq<T>());

        if (verbose) printf("\n... with 'TestType' "
                            "an arbitrary random-access iterator.\n");
        TestDriver<T>::testCase12Range(RandSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "an arbitrary input iterator.\n");
        TestDriver<TOA>::testCase12Range(InputSeq<TOA>());

        if (verbose) printf("\n... with 'TestType', 'OtherAlloc', "
                            "an arbitrary input iterator.\n");
        TestDriver<T,OAT>::testCase12Range(InputSeq<T>());

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' "
                            "an arbitrary input iterator.\n");
        TestDriver<TOA,OATOA>::testCase12Range(InputSeq<TOA>());

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR-RELATED CONCERNS
        //
        // Plan:
        //   Call test driver case11 with the following combinations:
        //    1. Element type and list both use bslma_Allocator
        //    2. Element type uses bslma_Allocator, and list uses no-bslma
        //       allocator type.
        //    3. Element type uses non-bslma allocator type, and list uses
        //       bslma allocator type.
        //    4. Element type and list both use non-bslma allocator type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Allocator concerns"
                            "\n==================================\n");

        TestDriver<T>::testCase11("T", "bslma");

        TestDriver<T,OAT>::testCase11("T", "OAT");

        TestDriver<TOA>::testCase11("TOA", "bslma");

        TestDriver<TOA,OATOA>::testCase11("TOA", "OATOA");

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
        //   Now that we can generate many values for our test objects, and
        //   compare results of assignments, we can test the assignment
        //   operator.    This is achieved by the 'testCase9' class method of
        //   the test driver template, instantiated for the basic test type.
        //   See that function for a list of concerns and a test plan.
        //
        // Testing:
        //   Obj& operator=(const Obj& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase9();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase9();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc'.\n");
        TestDriver<TOA>::testCase9();

        if (verbose) printf("\n... with 'TestType' and 'OtherAlloc'.\n");
        TestDriver<T,OAT>::testCase9();

        if (verbose) printf("\n... with 'TestTypeOtherAlloc' and"
                            " 'OtherAlloc'.\n");
        TestDriver<TOA,OATOA>::testCase9();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
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

        if (verbose) printf("\nTesting Generator Function g"
                            "\n============================\n");

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

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Having now full confidence in 'operator==', we can use it
        //   to test that copy constructors preserve the notion of
        //   value.  This is achieved by the 'testCase7' class method of the
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
        TestDriver<char>::testCase7();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase7();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase7();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase7();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase7();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
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
        //   operator!=(const Obj&, const Obj&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase6();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase6();

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
        TestDriver<char>::testCase4();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase4();

        if (verbose) printf("\n... with 'TestTypeNoAlloc'.\n");
        TestDriver<TNA>::testCase4();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase4();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase4();

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

        if (verbose) printf("\nTesting Generator Functions"
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

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
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

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

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

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
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

        bslma_TestAllocator testAllocator(veryVeryVerbose);

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

#ifndef BSLS_PLATFORM__CMP_MSVC  // Temporarily does not work
        ASSERT(  (bslalg_HasTrait<list<char>,
                  bslalg_TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<list<T>,
                  bslalg_TypeTraitBitwiseMoveable>::VALUE));
        ASSERT(  (bslalg_HasTrait<list<list<int> >,
                  bslalg_TypeTraitBitwiseMoveable>::VALUE));
#endif

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    bslma_Default::setGlobalAllocator(originalGlobalAllocator);

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
