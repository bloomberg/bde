// bslstl_deque.t.cpp                                                 -*-C++-*-

#include <bslstl_deque.h>
#include <bslstl_iterator.h>
#include <bslstl_forwarditerator.h>
#include <bslstl_randomaccessiterator.h>
#include <bslstl_string.h>                 // for testing only
#include <bslstl_vector.h>                 // for testing only

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_testallocator.h>           // for testing only
#include <bslma_testallocatorexception.h>  // for testing only
#include <bslmf_ispointer.h>               // for testing only
#include <bslmf_issame.h>                  // for testing only
#include <bsls_alignmentutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>                    // for testing only
#include <bsls_stopwatch.h>                // for testing only
#include <bsls_util.h>

#include <iterator>

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cctype>
#include <stdexcept>
#include <new>  // bad_alloc

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
// mostly a 'bsl::allocator' together with a 'bslma::TestAllocator' mechanism,
// but we also verify that the C++ standard  Finally, this component implements
// some new features proposed by C++0x, such as move semantics (see
// 'bslstl_move' component) and in-place construction ('emplace').
//
// This test plan follows the standard approach for components implementing
// value-semantic containers.  We have chosen as *primary* *manipulators* the
// 'push_back', 'pop_back' and 'clear' methods to be used by the generator
// functions 'g' and 'gg'.  Note that this choice is conditional on the
// implementation of 'clear', which clears all blocks and then sets 'd_start'
// to 'd_finish'.  Additional helper functions are provided to facilitate
// perturbation of internal state (e.g., capacity and start iterator within
// start block).  Note that some manipulators must support aliasing, and
// those that perform memory allocation must be tested for exception neutrality
// via the 'bslma::TestAllocator' component.  After the mandatory sequence of
// cases (1--10) for value-semantic types (cases 5 and 10 are not implemented,
// as there is not output or streaming below bslstl), we test each individual
// constructor, manipulator, and accessor in subsequent cases.  Move semantics
// are tested within relevant test case (e.g., [12] for move constructor, and
// [15] for 'push_back').
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     T            VALUE_TYPE (template argument, no default)
//     A            ALLOCATOR (template argument, default: bsl::allocator<T>)
//     deque<T,A>  bslstl_Deque<VALUE_TYPE,ALLOCATOR>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//-----------------------------------------------------------------------------
// class bslstl_Deque<T,A> (deque)
// ============================================================================
// [11] TRAITS
//
// CREATORS:
// [ 2] deque<T,A>(const A& a = A());
// [12] deque<T,A>(size_type n, const A& a = A());
// [12] deque<T,A>(size_type n, const T& val, const A& a = A());
// [12] template<class InputIter>
//        deque<T,A>(InputIter first, InputIter last, const A& a = A());
// [ 7] deque<T,A>(const deque<T,A>& orig, const A& = A());
// [12] deque(deque<T,A>&& original);
// [ 2] ~deque<T,A>();
//
/// MANIPULATORS:
// [13] template <class InputIter>
//        void assign(InputIter first, InputIter last);
// [13] void assign(size_type numElements, const T& val);
// [ 9] operator=(deque<T,A>&);
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
// [17] void push_front(const T&);
// [17] void push_back(const T&);
// [19] void pop_front();
// [19] void pop_back();
// [18] iterator insert(const_iterator pos, const T& val);
// [18] iterator insert(const_iterator pos, size_type n, const T& val);
// [18] template <class InputIter>
//        void insert(const_iterator pos, InputIter first, InputIter last);
// [19] iterator erase(const_iterator pos);
// [19] iterator erase(const_iterator first, const_iterator last);
// [20] void swap(deque<T,A>&);
//
// ACCESSORS:
// [ 4] const_reference operator[](size_type pos) const;
// [ 4] const_reference at(size_type pos) const;
// [15] const_reference front() const;
// [15] const_reference back() const;
// [ 4] size_type size() const;
// [14] size_type max_size() const;
// [14] size_type capacity() const;
// [14] bool empty() const;
// [16] const_iterator begin();
// [16] const_iterator end();
// [16] const_reverse_iterator rbegin();
// [16] const_reverse_iterator rend();
//
// FREE OPERATORS:
// [ 6] bool operator==(const deque<T,A>&, const deque<T,A>&);
// [ 6] bool operator!=(const deque<T,A>&, const deque<T,A>&);
// [21] bool operator<(const deque<T,A>&, const deque<T,A>&);
// [21] bool operator>(const deque<T,A>&, const deque<T,A>&);
// [21] bool operator<=(const deque<T,A>&, const deque<T,A>&);
// [21] bool operator>=(const deque<T,A>&, const deque<T,A>&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] ALLOCATOR-RELATED CONCERNS
// [18] USAGE EXAMPLE
// [22] CONCERN: 'std::length_error' is used properly
//
// TEST APPARATUS: GENERATOR FUNCTIONS
// [ 3] int ggg(deque<T,A> *object, const char *spec, int vF = 1);
// [ 3] deque<T,A>& gg(deque<T,A> *object, const char *spec);
// [ 8] deque<T,A> g(const char *spec);

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
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
class SmallTestTypeNoAlloc;
class MediumTestTypeNoAlloc;
class LargeTestTypeNoAlloc;
class BitwiseMoveableTestType;
class BitwiseCopyableTestType;

typedef TestType                      T;    // uses 'bslma' allocators
typedef SmallTestTypeNoAlloc          S;    // does not use 'bslma' allocators
typedef MediumTestTypeNoAlloc         M;    // does not use 'bslma' allocators
typedef LargeTestTypeNoAlloc          L;    // does not use 'bslma' allocators
typedef BitwiseMoveableTestType       BMT;  // uses 'bslma' allocators
typedef BitwiseCopyableTestType       BCT;  // uses 'bslma' allocators

typedef bsls::Types::Int64            Int64;
typedef bsls::Types::Uint64           Uint64;

// TEST OBJECT (unless o/w specified)
typedef char                          Element;  // every TestType's value type
typedef bsl::deque<T>                 Obj;

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

const int NUM_ALLOCS[] = {
    // Number of allocations (blocks) to create a deque of the following size
    // by using 'push_back' repeatedly (without initial reserve):
    //
    // 0    1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17
    // --   --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --
       0,   1,  2,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6
};

const int NUM_PADDING = 2;
    // BLOCK_ARRAY_PADDING as defined in 'bslalg_dequeimputil'.

const int NUM_INTERNAL_STATE_TEST = 10;
    // Number of different internal states to check.

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline
void dbg_print(char c)
{
    printf("%c", c);
    fflush(stdout);
}
inline
void dbg_print(unsigned char c)
{
    printf("%c", c);
    fflush(stdout);
}
inline
void dbg_print(signed char c)
{
    printf("%c", c);
    fflush(stdout);
}
inline
void dbg_print(short val)
{
    printf("%d", (int)val);
    fflush(stdout);
}
inline
void dbg_print(unsigned short val)
{
    printf("%d", (int)val);
    fflush(stdout);
}
inline
void dbg_print(int val)
{
    printf("%d", val);
    fflush(stdout);
}
inline
void dbg_print(bsls::Types::Int64 val)
{
    printf("%lld", val);
    fflush(stdout);
}
#if defined(BSLS_PLATFORM_OS_AIX)
inline
void dbg_print(unsigned int val)
{
    printf("%d", val);
    fflush(stdout);
}
#endif
inline
void dbg_print(size_t val)
{
    printf("%llu", (Uint64) val);
    fflush(stdout);
}
inline
void dbg_print(float val)
{
    printf("'%f'", (double)val);
    fflush(stdout);
}
inline
void dbg_print(double val)
{
    printf("'%f'", val);
    fflush(stdout);
}

inline
void dbg_print(const char* s)
{
    printf("\"%s\"", s);
    fflush(stdout);
}

// Deque-specific print function.
template <class TYPE, class ALLOC>
void dbg_print(const bsl::deque<TYPE,ALLOC>& v)
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

                              // ===============
                              // Usage Example 1
                              // ===============

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Using a 'deque' to Implement a Laundry Queue
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a class to maintain a process queue of names of
// customers who are dropping off their laundry at a drop-off laundry service.
// We can accomplish this by defining a new class characterizing a
// laundry-process queue that uses 'bsl::deque' in its implementation.
//
// The process queue provides two methods, 'push' and 'expeditedPush', for
// inserting names of customers onto the queue.  When calling the 'push'
// method, the customer's name will be inserted at the end of the queue -- his
// laundry will be done after the laundry of customers previously on the queue.
// The 'expeditedPush' method is reserved for customers who have bribed the
// merchant for expedited service.  When calling the 'expeditedPush' method,
// the customer's name will be inserted onto the front of the queue -- his
// laundry will be done before customers previously on the queue.
//
// When the workers are ready to do some laundry, they call the 'next' method
// of the queue, which returns the name of the customer whose laundry is to be
// done next.  For brevity of the usage example, we do not show how customers
// are track while or after their laundry is being done.
//
// In addition, the laundry queue also provides the 'find' method, which
// returns a 'bool' to indicate whether a given customer is still in the queue.
//
// First, we declare a class 'LaundryQueue' based on a deque, to store names of
// customers at a drop-off laundry:

class LaundryQueue {
    // This 'class' keeps track of customers enqueued to have their laundry
    // done by a laundromat.

    // DATA
    bsl::deque<bsl::string> d_queue;

  public:
    // CREATORS
    explicit
    LaundryQueue(bslma::Allocator *basicAllocator = 0);
        // Create a 'LaundryQueue' object using the specified 'basicAllocator'.
        // If 'basicAllocator' is not provided, use the default allocator.

    // MANIPULATORS
    void push(const bsl::string& customerName);
        // Add the specified 'customerName' to the back of the laundry queue.

    void expeditedPush(const bsl::string& customerName);
        // Add the specified 'customerName' to the laundry queue at the
        // front.

    bsl::string next();
        // Return the name from the front of the queue, removing it from
        // the queue.  If the queue is empty, return '(* empty *)' which is not
        // a valid name for a customer.

    // ACCESSORS
    bool find(const bsl::string& customerName);
        // Return 'true' if 'customerName' is in the queue, and 'false'
        // otherwise.
};

// Then, we define the implementation of the methods of 'LaundryQueue'

// CREATORS
LaundryQueue::LaundryQueue(bslma::Allocator *basicAllocator)
: d_queue(basicAllocator)
{
    // Note that the allocator is propagated to the underlying 'deque', which
    // will use the default allocator is '0 == basicAllocator'.
}

// MANIPULATORS
void LaundryQueue::push(const bsl::string& customerName)
{
    d_queue.push_back(customerName);     // note constant time
}

void LaundryQueue::expeditedPush(const bsl::string& customerName)
{
    d_queue.push_front(customerName);    // note constant time
}

bsl::string LaundryQueue::next()
{
    if (d_queue.empty()) {
        return "(* empty *)";                                         // RETURN
    }

    bsl::string ret = d_queue.front();   // note constant time

    d_queue.pop_front();                 // note constant time

    return ret;
}

// ACCESSORS
bool LaundryQueue::find(const bsl::string& customerName)
{
    // Note 'd_queue.empty() || d_queue[0] == d_queue.front()'

    for (size_t i = 0; i < d_queue.size(); ++i) {
        if (customerName == d_queue[i]) {    // note '[]' is constant time
            return true;                                              // RETURN
        }
    }

    return false;
}

                              // ===============
                              // Usage Example 2
                              // ===============

// It was decided to eliminate this second usage example from the .h file as it
// was too long, it is being kept as a test case, and perhaps here it may
// inform someone trying to familiarize themselves with the component.

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 2: Showing Properties of a Deque and its Iterators
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'deque' (pronounced 'deck') is a *D*ouble *E*nded *QUE*ue.  One can
// efficiently push or pop elements to the front or end of the queue.  It has
// iterators and reverse iterators which are quite symmetrical.
//
// First, we define a function 'checkInvariants' which will check properties
// that should always be true for any deque in a valid state.:
//..
template <class TYPE>
void checkInvariants(const deque<TYPE>& d)
{
    const size_t SZ = d.size();

    {
        // The following tests will be valid for any deque in a valid state.

        ASSERT((0 == SZ) == d.empty());

        ASSERT(d.end() - d.begin() == d.rend() - d.rbegin());
        ASSERT(d.end() - d.begin() >= 0);
        ASSERT(SZ == (size_t) (d.end() - d.begin()));

        for (size_t u = 0; u < SZ; ++u) {
            ASSERT(&*(d.begin()    + u) == &d[         u]);
            ASSERT(&*(d.end()  - 1 - u) == &d[SZ - 1 - u]);
            ASSERT(&*(d.rbegin()   + u) == &d[SZ - 1 - u]);
            ASSERT(&*(d.rend() - 1 - u) == &d[         u]);
        }
    }

    if (d.empty()) {
        return;                                                       // RETURN
    }

    {
        // The rest of the tests are only valid if the deque contains at least
        // one element, but are always valid in that case.

        ASSERT(&d[0]                   == &d.front());
        ASSERT(&d[SZ - 1]              == &d.back());

        ASSERT(&*d.begin()             == &d.front());
        ASSERT(&*d.rbegin()            == &d.back());

        ASSERT(&*(d.begin()  + SZ - 1) == &d.back());
        ASSERT(&*(d.rbegin() + SZ - 1) == &d.front());

        ASSERT(&*(d.end()         - 1) == &d.back());
        ASSERT(&*(d.rend()        - 1) == &d.front());

        ASSERT(&*(d.end()    - SZ)     == &d.front());
        ASSERT(&*(d.rend()   - SZ)     == &d.back());
    }
}
//..


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

    ~ExceptionGuard()
    {
        if (d_object_p) {
            const int LINE = d_lineNum;
            LOOP_ASSERT(LINE, d_value == *d_object_p);
        }
    }

    // MANIPULATORS
    void resetValue(const VALUE_TYPE& value, int line)
    {
        d_lineNum = line;
        d_value = value;
    }

    void release()
    {
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

  private:
    char             *d_data_p;
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

        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }

    ~TestType()
    {
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

    void setDatum(char c)
    {
        *d_data_p = c;
    }

    // ACCESSORS
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
void dbg_print(const TestType& rhs)
{
    printf("%c", rhs.value());
    fflush(stdout);
}

                       // ==========================
                       // class SmallTestTypeNoAlloc
                       // ==========================

class SmallTestTypeNoAlloc {
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
    SmallTestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    SmallTestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
        ++numCharCtorCalls;
    }

    SmallTestTypeNoAlloc(const SmallTestTypeNoAlloc&  original)
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

// SmallTestType-specific print function.
void dbg_print(const SmallTestTypeNoAlloc& rhs)
{
    printf("%c", rhs.value());
    fflush(stdout);
}

                       // ===========================
                       // class MediumTestTypeNoAlloc
                       // ===========================

class MediumTestTypeNoAlloc {
    // This test type has footprint identical to 'TestType', but a large
    // footprint, so that there are just a few objects in a deque's block.  It
    // also counts the number of default and copy constructions, assignments,
    // and destructions.  It does not allocate, and thus could have the
    // bit-wise copyable trait, but we defer this to the
    // 'BitwiseCopyableMediumTestType'.

    // DATA
    union {
        char  d_char;
        char  d_fill[MEDIUM_TEST_TYPE_SIZE];
    } d_u;

  public:
    // CREATORS
    MediumTestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    MediumTestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
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

// MediumTestType-specific print function.
void dbg_print(const MediumTestTypeNoAlloc& rhs)
{
    printf("%c", rhs.value());
    fflush(stdout);
}

                       // ==========================
                       // class LargeTestTypeNoAlloc
                       // ==========================

class LargeTestTypeNoAlloc {
    // This test type has footprint identical to 'TestType', but a much larger
    // footprint, so that there are just a single object in a deque's block.
    // It also counts the number of default and copy constructions,
    // assignments, and destructions.  It does not allocate, and thus could
    // have the bit-wise copyable trait, but we defer this to the
    // 'BitwiseCopyableLargeTestType'.

    // DATA
    union {
        char  d_char;
        char  d_fill[LARGE_TEST_TYPE_SIZE];
    } d_u;

  public:
    // CREATORS
    LargeTestTypeNoAlloc()
    {
        d_u.d_char = DEFAULT_VALUE;
        ++numDefaultCtorCalls;
    }

    explicit
    LargeTestTypeNoAlloc(char c)
    {
        d_u.d_char = c;
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

// LargeTestType-specific print function.
void dbg_print(const LargeTestTypeNoAlloc& rhs)
{
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
};

                       // =============================
                       // class BitwiseCopyableTestType
                       // =============================

class BitwiseCopyableTestType : public SmallTestTypeNoAlloc {
    // This test type is identical to 'TestTypeNoAlloc' except that it has the
    // bit-wise copyable and bit-wise equality comparable traits.  All members
    // are inherited.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyableTestType,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(BitwiseCopyableTestType,
                                   bslmf::IsBitwiseEqualityComparable);

    // CREATORS
    BitwiseCopyableTestType()
    : SmallTestTypeNoAlloc()
    {
    }

    explicit
    BitwiseCopyableTestType(char c)
    : SmallTestTypeNoAlloc(c)
    {
        ++numCharCtorCalls;
    }

    BitwiseCopyableTestType(const BitwiseCopyableTestType&  original)
    : SmallTestTypeNoAlloc(original.value())
    {
    }
};

                               // ==============
                               // class CharList
                               // ==============

template <class TYPE>
class CharList {
    // This array class is a simple wrapper on a 'char' array offering an
    // input iterator access via the 'begin' and 'end' accessors.  The
    // iterator is specifically an *input* iterator and its value type
    // is the parameterized 'TYPE'.

    // DATA
    Vector_Imp<TYPE> d_value;

  public:
    // TYPES
    typedef bslstl::ForwardIterator<const TYPE, const TYPE*>    const_iterator;
        // Input iterator.

    // CREATORS
    CharList() {}
    explicit CharList(const Vector_Imp<TYPE>& value);

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
    // parameterized 'TYPE'.

    // DATA
    Vector_Imp<TYPE>  d_value;

  public:
    // TYPES
    typedef const TYPE *const_iterator;
        // Random-access iterator.

    // CREATORS
    CharArray() {}
    explicit CharArray(const Vector_Imp<TYPE>& value);

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

  public:
    // TYPES
    typedef typename ALLOC::value_type        value_type;
    typedef typename ALLOC::pointer           pointer;
    typedef typename ALLOC::const_pointer     const_pointer;
    typedef typename ALLOC::reference         reference;
    typedef typename ALLOC::const_reference   const_reference;
    typedef typename ALLOC::size_type         size_type;
    typedef typename ALLOC::difference_type   difference_type;

    template <class OTHER_TYPE>
    struct rebind {
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

    explicit
    LimitAllocator(bslma::Allocator *mechanism)
    : AllocBase(mechanism), d_limit(-1) { }

    explicit
    LimitAllocator(const ALLOC& alloc)
    : AllocBase((const AllocBase&) alloc), d_limit(-1) { }

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
//                       TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
struct TestDriver {
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A .. E] correspond to arbitrary (but unique) char
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
    //-------------------------------------------------------------------------

    // CONSTANTS
    enum {START_POS  = -1 * NUM_PADDING * NOMINAL_BLOCK_BYTES /
                                                           (int) sizeof (TYPE),
        // The integer offset to the first element pointed by the first block
        // pointer of a newly created empty 'bsl::deque'.

          FINISH_POS = ((NUM_PADDING + 1) * NOMINAL_BLOCK_BYTES /
                                                      (int) sizeof (TYPE)) - 1,
        // The integer offset to the last element pointed by the last block
        // pointer of a newly created empty 'bsl::deque'.

          INCREMENT = FINISH_POS - START_POS >= NUM_INTERNAL_STATE_TEST
                    ? (FINISH_POS - START_POS) / NUM_INTERNAL_STATE_TEST
                    : 1
        // The integer increment that will be used when permutating through the
        // various internal states.
    };

    // TYPES
    typedef bsl::deque<TYPE,ALLOC>  Obj;
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
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

    static Vector_Imp<TYPE> gV(const char *spec);
        // Return, by value, a new vector corresponding to the specified
        // 'spec'.

    static Obj g(const char *spec);
        // Return, by value, a new object corresponding to the specified
        // 'spec'.

    static void setInternalState(Obj *object,
                                 int  n);
        // Using 'push_back', 'push_front', 'pop_back' and 'pop_front', set the
        // internal state of the specified 'object' by moving the start and
        // finish iterator by the specified 'n' positions.

    static void stretch(Obj *object, int size, const TYPE& value = TYPE());
        // Using only primary manipulators, extend the length of the specified
        // 'object' by the specified size by adding copies of the specified
        // 'value'.  The resulting value is not specified.  The behavior is
        // undefined unless 0 <= size.

    static void stretchRemoveAll(Obj         *object,
                                 int          size,
                                 const TYPE&  value = TYPE());
        // Using only primary manipulators, extend the capacity of the
        // specified 'object' to (at least) the specified size by adding
        // copies of the optionally specified 'value'; then remove all
        // elements leaving 'object' empty.  The behavior is undefined unless
        // '0 <= size'.

    // TEST CASES

    template <class CONTAINER>
    static void testCaseM1Range(const CONTAINER&);
        // Performance test for operators that take a range of inputs.

    static void testCaseM1();
        // Performance test.

    static void testCase22();
        // Test proper use of 'std::length_error'.

    static void testCase21();
        // Test comparison free operators.

    static void testCase20();
        // Test 'swap' member.

    static void testCase19();
        // Test 'erase', 'pop_back' and 'pop_front'.

    static void testCase18();
        // Test 'insert' members, and move 'push_back' and 'insert' members.

    template <class CONTAINER>
    static void testCase18Range(const CONTAINER&);
        // Test 'insert' member template.

    static void testCase17();
        // Test 'push_back' and 'push_front' members.

    static void testCase16();
        // Test iterators.

    static void testCase15();
        // Test element access.

    static void testCase14();
        // Test reserve and capacity-related methods.

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

template <class TYPE, class ALLOC>
int TestDriver<TYPE,ALLOC>::getValues(const TYPE **valuesPtr)
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
        else if ('<' == spec[i]) {
            object->pop_back();
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
bsl::deque<TYPE,ALLOC>& TestDriver<TYPE,ALLOC>::gg(Obj        *object,
                                                   const char *spec)
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

template <class TYPE, class ALLOC>
bsl::deque<TYPE,ALLOC> TestDriver<TYPE,ALLOC>::g(const char *spec)
{
    Obj object((bslma::Allocator *)0);
    return gg(&object, spec);
}

template <class TYPE, class ALLOC>
Vector_Imp<TYPE>  TestDriver<TYPE,ALLOC>::gV(const char    *spec)
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
void TestDriver<TYPE,ALLOC>::setInternalState(Obj         *object,
                                              int          n)
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
        for(; n < 0; ++n) {
            object->push_front(value);
            object->pop_back();
        }
    }
    ASSERT(0 == object->size());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::stretch(Obj *object, int size, const TYPE& value)
{
    ASSERT(object);
    ASSERT(0 <= size);
    for (int i = 0; i < size; ++i) {
        object->push_back(value);
    }
    ASSERT(object->size() >= (std::size_t)size);
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::stretchRemoveAll(Obj         *object,
                                              int          size,
                                              const TYPE&  value)
{
    ASSERT(object);
    ASSERT(0 <= size);
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
    const int           NUM_DEQUE   = 300;

    const char         *SPECREF     = "ABCDE";
    const int           SPECREF_LEN = (int) strlen(SPECREF);
    char                SPEC[LENGTH + 1];

    for (int i = 0; i < LENGTH; ++i) {
        SPEC[i] = SPECREF[i % SPECREF_LEN];
    }
    SPEC[LENGTH] = '\0';
    CONTAINER mU(gV(SPEC)); const CONTAINER& U = mU;

    void * addr = bslma::Default::defaultAllocator()->allocate(
                                        NUM_DEQUE * LENGTH * 2 * sizeof(TYPE));
    bslma::Default::defaultAllocator()->deallocate(addr);

    printf("\tC1) Constructor:\n");
    {
        // bsl::deque(f, l)
        double time = 0.;
        Obj *deques[NUM_DEQUE];

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE; ++i) {
            deques[i] = new Obj(U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tbsl::deque(f,l):\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE; ++i) {
            delete deques[i];
        }
    }

    printf("\tA1) Assign:\n");
    {
        // assign(f, l)
        double time = 0.;
        Obj *deques[NUM_DEQUE];

        ASSERT(LENGTH >= NUM_DEQUE);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_DEQUE) / 2, j = 0;
                 i < (LENGTH + NUM_DEQUE) / 2;
                 ++i, ++j) {
            deques[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE; ++i) {
            deques[i]->assign(U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tassign(f,l):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE; ++i) {
            delete deques[i];
        }
    }

    printf("\tI1) Insert (at front):\n");
    {
        // insert(p, f, l)
        double time = 0.;
        Obj *deques[NUM_DEQUE];

        ASSERT(LENGTH >= NUM_DEQUE);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_DEQUE) / 2, j = 0;
                 i < (LENGTH + NUM_DEQUE) / 2;
                 ++i, ++j) {
            deques[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE; ++i) {
            deques[i]->insert(deques[i]->begin(), U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,f,l):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE; ++i) {
            delete deques[i];
        }
    }

    printf("\tI2) Insert (at back):\n");
    {
        // insert(p, f, l)
        double time = 0.;
        Obj *deques[NUM_DEQUE];

        ASSERT(LENGTH >= NUM_DEQUE);
        // Spread out the initial lengths.
        for (int i = (LENGTH - NUM_DEQUE) / 2, j = 0;
                 i < (LENGTH + NUM_DEQUE) / 2;
                 ++i, ++j) {
            deques[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE; ++i) {
            deques[i]->insert(deques[i]->end(), U.begin(), U.end());
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,f,l):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE; ++i) {
            delete deques[i];
        }
    }

    printf("\tI3) Insert (everywhere):\n");
    {
        // insert(p, f, l)
        double time = 0.;
        Obj    *deques[NUM_DEQUE];

        ASSERT(LENGTH >= NUM_DEQUE);
        // Spread out the initial lengths.
        for (int i = 0; i < NUM_DEQUE; ++i) {
            // NOTE: Uses NUM_DEQUE for length.
            deques[i] = new Obj(NUM_DEQUE, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE; ++i) {
            deques[i]->insert(deques[i]->begin() + i, U.begin(), U.end());
        }
        time += t.elapsedTime();

        printf("\t\tinsert(p,f,l):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE; ++i) {
            delete deques[i];
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
    const int           LENGTH_S    = 1000;
    const int           LENGTH_L    = 5000;
    const int           NUM_DEQUE_S = 100;
    const int           NUM_DEQUE_L = 1000;

//    void * addr = bslma::Default::defaultAllocator()->allocate(
//                                  NUM_DEQUE_L * LENGTH_L * 2 * sizeof(TYPE));
//    bslma::Default::defaultAllocator()->deallocate(addr);

    // C1) CONSTRUCTORS
    printf("\tC1) Constructors:\n");
    {
        // bsl::deque()
        double time = 0.;

        t.reset(); t.start();
        Obj *deques = new Obj[NUM_DEQUE_L];
        time = t.elapsedTime();

        printf("\t\tbsl::deque():\t\t%1.6fs\n", time);
        delete[] deques;
    }
    {
        // bsl::deque(n)
        double time = 0.;
        Obj *deques[NUM_DEQUE_L];

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            deques[i] = new Obj(LENGTH_S);
        }
        time = t.elapsedTime();

        printf("\t\tbsl::deque(n):\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            delete deques[i];
        }
    }
    {
        // bsl::deque(n,v)
        double time = 0.;
        Obj *deques[NUM_DEQUE_L];

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            deques[i] = new Obj(LENGTH_S, VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tbsl::deque(n,v):\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            delete deques[i];
        }
    }
    // C2) COPY CONSTRUCTOR
    printf("\tC2) Copy Constructors:\n");
    {
        // bsl::deque(bsl::deque)
        double time = 0.;
        Obj  deq(LENGTH_L);
        Obj *deques[NUM_DEQUE_S];

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            deques[i] = new Obj(deq);
        }
        time = t.elapsedTime();

        printf("\t\tbsl::deque(d):\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // A1) COPY ASSIGNMENT
    printf("\tA1) Copy Assginment:\n");
    {
        // operator=(bsl::deque)
        double time = 0;
        Obj deq(LENGTH_L / 2);
        Obj *deques[NUM_DEQUE_L];

        ASSERT(LENGTH_L >= NUM_DEQUE_L);
        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_DEQUE_L) / 2, j = 0;
                 i < (LENGTH_L + NUM_DEQUE_L) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            *deques[i] = deq;
        }
        time = t.elapsedTime();

        printf("\t\toperator=(d):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            ASSERT(*deques[i] == deq);
            delete deques[i];
        }
    }

    // A2) ASSIGN
    printf("\tA2) Assign:\n");
    {
        // assign(n)
        double time = 0;
        Obj *deques[NUM_DEQUE_L];

        ASSERT(LENGTH_L >= NUM_DEQUE_L);
        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_DEQUE_L) / 2, j = 0;
                 i < (LENGTH_L + NUM_DEQUE_L) / 2;
                 ++i, ++j) {
            deques[j]  = new Obj(i, VALUES[i % NUM_VALUES]);
        }
        const TYPE val    = TYPE();
        const int  length = LENGTH_L / 2;

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            deques[i]->assign(length, val);
        }
        time = t.elapsedTime();

        printf("\t\tassign(n):\t\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_L; ++i) {
            delete deques[i];
        }
    }

    // P1) PUSH_BACK
    printf("\tP1) Push_back:\n");
    {
        // push_back(v)
        double time = 0;
        Obj *deques[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            // Push in 1000 objects.
            for (int j = 0; j < LENGTH_L; ++j) {
                deques[i]->push_back(VALUES[j % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tpush_back(v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // P1) PUSH_FRONT
    printf("\tP2) Push_front:\n");
    {
        // push_front(v)
        double time = 0;
        Obj *deques[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            // Push in 1000 objects.
            for (int j = 0; j < LENGTH_L; ++j) {
                deques[i]->push_front(VALUES[j % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tpush_front(v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // P1) POP_BACK
    printf("\tP3) Pop_back:\n");
    {
        // pop_back()
        double time = 0;
        Obj    *deques[NUM_DEQUE_S];
        size_t  sizes[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            // Pop out all objects.
            for (unsigned int j = 0; j < sizes[i]; ++j) {
                deques[i]->pop_back();
            }
        }
        time = t.elapsedTime();

        printf("\t\tpop_back():\t\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // P1) POP_FRONT
    printf("\tP4) Pop_front:\n");
    {
        // pop_front()
        double  time = 0;
        Obj    *deques[NUM_DEQUE_S];
        size_t  sizes[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_L - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_L + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            // Pop out all objects.
            for (unsigned int j = 0; j < sizes[i]; ++j) {
                deques[i]->pop_front();
            }
        }
        time = t.elapsedTime();

        printf("\t\tpop_front():\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // I1) INSERT (FRONT)
    printf("\tI1) Insert (at front):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *deques[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            for (int j = 0; j < LENGTH_L; ++j) {
                deques[i]->insert(deques[i]->begin(), VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }
    {
        // insert(n,p,v)
        double time = 0;
        Obj  *deques[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            deques[i]->insert(deques[i]->begin(), LENGTH_L,
                              VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // I2) INSERT (BACK)
    printf("\tI2) Insert (at back):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *deques[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            for (int j = 0; j < LENGTH_L; ++j) {
                deques[i]->insert(deques[i]->end(), VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    {
        // insert(n,p,v)
        double time = 0;
        Obj  *deques[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            deques[i]->insert(deques[i]->end(), LENGTH_L,
                              VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // I3) INSERT
    printf("\tI3) Insert (everywhere):\n");
    {
        // insert(p,v)
        double  time = 0;
        Obj    *deques[NUM_DEQUE_S];
        size_t  sizes[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j]  = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            for (unsigned int j = 0; j < sizes[i]; ++j) {
                deques[i]->insert(deques[i]->begin() + j,
                                  VALUES[i % NUM_VALUES]);
            }
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    {
        // insert(n,p,v)
        double time = 0;
        Obj    *deques[NUM_DEQUE_S];
        size_t  sizes[NUM_DEQUE_S];

        // Spread out the initial lengths.
        for (int i = (LENGTH_S - NUM_DEQUE_S) / 2, j = 0;
                 i < (LENGTH_S + NUM_DEQUE_S) / 2;
                 ++i, ++j) {
            deques[j] = new Obj(i, VALUES[i % NUM_VALUES]);
            sizes[j] = i;
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            // Insert in middle.
            deques[i]->insert(deques[i]->begin() + sizes[i] / 2, LENGTH_S,
                              VALUES[i % NUM_VALUES]);
        }
        time = t.elapsedTime();

        printf("\t\tinsert(p,n,v):\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }

    // E1) ERASE
    printf("\tE1) Erase:\n");
    {
        // erase(p)
        double  time = 0;
        Obj    *deques[NUM_DEQUE_S];

        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            deques[i] = new Obj(LENGTH_S * 2, VALUES[i % NUM_VALUES]);
        }

        t.reset(); t.start();
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            for (int j = 0; j < LENGTH_S; ++j) {
                deques[i]->erase(deques[i]->begin() + j);
            }
        }
        time = t.elapsedTime();

        printf("\t\terase(p):\t\t\t%1.6fs\n", time);
        for (int i = 0; i < NUM_DEQUE_S; ++i) {
            delete deques[i];
        }
    }
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase22()
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
    //   3) That the value of the deque is unchanged if an exception is
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
    typedef bsl::deque<TYPE,LimitAllocator<ALLOC> > LimitObj;

    LimitObj mY(LENGTH, DEFAULT_VALUE);  // does not throw
    const LimitObj& Y = mY;

#ifdef BDE_BUILD_TARGET_EXC
    if (verbose) printf("\nConstructor 'deque(n, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
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

    if (verbose) printf("\nConstructor 'deque(n, T x, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(LENGTH, DEFAULT_VALUE, a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
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

    if (verbose) printf("\nConstructor 'deque<Iter>(f, l, a = A())'.\n");

    for (int limit = LENGTH - 2; limit <= LENGTH + 2; ++limit) {
        bool exceptionCaught = false;
        a.setMaxSize(limit);

        if (veryVerbose)
            printf("\tWith max_size() equal to limit = %d\n", limit);

        try {
            LimitObj mX(Y.begin(), Y.end(), a);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
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
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
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
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\tCaught std::exception(%s).\n", e.what());
                }
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
                case 0: printf("\tWith push_back(c).\n");          break;
                case 1: printf("\tWith insert(p, T x).\n");        break;
                case 2: printf("\tWith insert(p, n, T x).\n");     break;
                case 3: printf("\tWith insert<Iter>(p, f, l).\n"); break;
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
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
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
    const Obj X;

    if (verbose) printf("\nTesting requests for '(size_t) -1' elements with "
                        "default allocator.\n");

    if (verbose) printf("\nConstructor 'deque(n, T x, a = A())'"
                        " and 'max_size()' equal to %llu.\n",
                        (Uint64) X.max_size());
    {
        bool exceptionCaught = false;

        if (veryVerbose) {
            size_t minus2 = (size_t) -2;
            printf("\tWith max_size() equal to %llu.\n", (Uint64) minus2);
        }

        try {
            Obj mX(-1, DEFAULT_VALUE);  // test here
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            ASSERT(0);
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
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
                        " 'max_size()' equal to %llu.\n",
                        (Uint64) X.max_size());

    for (int capacityMethod = 0; capacityMethod <= 2; ++capacityMethod)
    {
        bool exceptionCaught = false;

        if (verbose) {
            switch (capacityMethod) {
                case 0: printf("\tWith reserve(n).\n");        break;
                case 1: printf("\tWith resize(n).\n");         break;
                case 2: printf("\tWith resize(n, T x).\n");    break;
                default: ASSERT(0);
            };
        }

        try {
            Obj mX;

            switch (capacityMethod) {
                case 0:  mX.reserve((size_t)-1);                  break;
                case 1:  mX.resize((size_t)-1);                   break;
                case 2:  mX.resize((size_t)-1, DEFAULT_VALUE);    break;
                default: ASSERT(0);
            };
        }
        catch (const std::length_error& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::length_error(\"%s\").\n", e.what());
            }
            exceptionCaught = true;
        }
        catch (std::exception& e) {
            if (veryVerbose) {
                printf("\t\tCaught std::exception(%s).\n", e.what());
            }
            ASSERT(0);
        }
        catch (...) {
            if (veryVerbose) {
                printf("\t\tCaught unknown exception.\n");
            }
            ASSERT(0);
        }
        ASSERT(exceptionCaught);
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBytesInUse());

    if (verbose) printf("\nTesting requests for 'X.max_size() + n' elements "
                        "with 'insert' and 'max_size()' equal to %llu.\n",
                        (Uint64) X.max_size());

    for (int insertMethod = 0; insertMethod <= 1; insertMethod += 2) {

        if (verbose) {
            switch (insertMethod) {
                case 0: printf("\tWith insert(pos, n, C c).\n");        break;
                case 1: printf("\tWith insert(p, n, C c).\n");          break;
                default: ASSERT(0);
            };
        }

        for (int limit = 1; limit <= 5; ++limit) {
            bool exceptionCaught = false;

            if (veryVerbose)
                printf("\t\tCreating string of length 'max_size()' plus %d.\n",
                       limit);

            try {
                Obj mX(PADDING, DEFAULT_VALUE, a);  const Obj& X = mX;

                const size_t LENGTH = X.max_size() - PADDING + limit;
                switch (insertMethod) {
                    case 0: {
                                mX.insert(mX.begin(), LENGTH, DEFAULT_VALUE);
                    } break;
                    default: ASSERT(0);
                };
            }
            catch (const std::length_error& e) {
                if (veryVerbose) {
                    printf("\t\t\tCaught std::length_error(\"%s\").\n",
                           e.what());
                }
                exceptionCaught = true;
            }
            catch (std::exception& e) {
                ASSERT(0);
                if (veryVerbose) {
                    printf("\t\t\tCaught std::exception(%s).\n", e.what());
                }
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
#endif

}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase21()
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
    //   For a variety of deques of different sizes and different values, test
    //   that the comparison returns as expected.  Note that capacity is not of
    //   concern here, the implementation specifically uses only 'begin()',
    //   'end()', and 'size()'.
    //
    // Testing:
    //   bool operator<(const deque<T,A>&, const deque<T,A>&);
    //   bool operator>(const deque<T,A>&, const deque<T,A>&);
    //   bool operator<=(const deque<T,A>&, const deque<T,A>&);
    //   bool operator>=(const deque<T,A>&, const deque<T,A>&);
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
void TestDriver<TYPE,ALLOC>::testCase20()
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
    //   swap(deque<T,A>& lhs, deque<T,A>& rhs);
    // ------------------------------------------------------------------------

        if (verbose) printf("\nSWAP TEST"
                            "\n=========\n");

        bslma::TestAllocator testAlloc2("alloc2", veryVeryVerbose);
                        ASSERT(0 == testAlloc2.numBytesInUse());

        bsl::deque<int> vec1;
        bsl::deque<int> vec2(&testAlloc2);

        for (int i = 0; i < 1000; ++i) {
            vec1.push_back(i);
        }

        const int numAlloc1 = (int) testAlloc2.numAllocations();
        const int numDealloc1 = (int) testAlloc2.numDeallocations();
        const int inUse1 = (int) testAlloc2.numBytesInUse();

        // Swap deques with unequal allocators
        vec1.swap(vec2);
                        ASSERT(0 == vec1.size());
                        ASSERT(1000 == vec2.size());
                        for (int i = 0; i < 1000; ++i) ASSERT(i == vec2[i]);
                        ASSERT(bslma::Default::defaultAllocator() ==
                               vec1.get_allocator());
                        ASSERT(&testAlloc2 == vec2.get_allocator());
                        // Possible extra memory used
                        ASSERT(inUse1 <= testAlloc2.numBytesInUse());
                        // Extra number of allocations for creating temp deque
                        ASSERT(numAlloc1 < testAlloc2.numAllocations());
                        ASSERT(numDealloc1 < testAlloc2.numDeallocations());

        {
            bsl::deque<int> vec3(&testAlloc2);
            const int numAlloc3 = (int) testAlloc2.numAllocations();
            const int numDealloc3 = (int) testAlloc2.numDeallocations();
            const int inUse3 = (int) testAlloc2.numBytesInUse();

            // Swap deques with equal allocators
            vec3.swap(vec2);
            ASSERT(vec2.empty());
            ASSERT(1000 == vec3.size());
            for (int i = 0; i < 1000; ++i) ASSERT(i == vec3[i]);
            ASSERT(numAlloc3 == testAlloc2.numAllocations());
            ASSERT(numDealloc3 == testAlloc2.numDeallocations());
            ASSERT(inUse3 == testAlloc2.numBytesInUse());
        }
        // Destructor for vec3 should have freed memory
        // Moreover, vec2 is now back to default-constructed state
        ASSERT(inUse1 == testAlloc2.numBytesInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase19()
{
    // --------------------------------------------------------------------
    // TESTING ERASE
    // We have the following concerns:
    //   1) That the resulting value is correct.
    //   2) That erasing a suffix of the array never allocates, and thus never
    //      throws.  In particular, 'pop_back()' and 'erase(..., X.end())' do
    //      not throw.
    //   3) That erasing is exception neutral w.r.t. memory allocation.
    //   4) That no memory is leaked.
    //
    // Plan:
    //   For the 'erase' methods, the concerns are simply to cover the full
    //   range of possible indices and numbers of elements.  We build a deque
    //   with a variable size and capacity, and remove a variable element or
    //   number of elements from it, and verify that size, capacity, and value
    //   are as expected:
    //      - Without exceptions, and computing the number of allocations.
    //      - In the presence of exceptions during memory allocations using a
    //        'bslma::TestAllocator' and varying its *allocation* *limit*, but
    //        not computing the number of allocations or checking on the value
    //        in case an exception is thrown (it is enough to verify that all
    //        the elements have been destroyed indirectly by making sure that
    //        there are no memory leaks).
    //   For concern 2, we verify that the number of allocations is as
    //   expected:
    //      - length of the tail (last element erased to last element) if the
    //        type uses a 'bslma' allocator, and is not moveable.
    //      - 0 otherwise.
    //
    // Testing:
    //   void pop_back();
    //   void pop_front();
    //   iterator erase(const_iterator pos);
    //   iterator erase(const_iterator first, const_iterator last);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

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

    if (verbose) printf("\nTesting 'pop_back' on non-empty deques.\n");
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

                Obj mX(INIT_LENGTH, &testAllocator);  const Obj& X = mX;
                mX.reserve(INIT_CAP);

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] =  VALUES[k % NUM_VALUES];
                }

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBefore: "); P_(BB); P(B);
                }

                mX.pop_back();

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAfter : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());
                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, i, l, k,
                                 VALUES[k % NUM_VALUES] == X[k]);
                }
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

                Obj mX(LENGTH, &testAllocator);
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

    if (verbose) printf("\nTesting 'pop_front' on non-empty deques.\n");
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

                Obj mX(INIT_LENGTH, &testAllocator);  const Obj& X = mX;
                mX.reserve(INIT_CAP);

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    mX[k] =  VALUES[k % NUM_VALUES];
                }

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tBefore: "); P_(BB); P(B);
                }

                mX.pop_front();

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\tAfter : "); P_(AA); P(A);
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                             LENGTH == X.size());

                for (k = 0; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, i, l, k,
                                 VALUES[(k+1) % NUM_VALUES] == X[k]);
                }
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

                Obj mX(LENGTH, &testAllocator);
                mX.reserve(CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial "); P_(LENGTH); P(CAP);
                }

                bool exceptionCaught = false;
                try {
                    mX.pop_front();
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

    if (verbose) printf("\nTesting 'erase(pos)' on non-empty deques.\n");
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

                    Obj mX(INIT_LENGTH, &testAllocator);  const Obj& X = mX;

                    mX.reserve(INIT_CAP);

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase one element at "); P(POS);
                    }

                    const int BB = (int) testAllocator.numBlocksTotal();
                    const int  B = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore: "); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + POS);

                    const int AA = (int) testAllocator.numBlocksTotal();
                    const int  A = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tAfter : "); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS,
                                 LENGTH == X.size());

                    for (m = 0; m < POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP, POS, m,
                                     VALUES[(m + 1) % NUM_VALUES] == X[m]);
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
                        const int AL = (int) testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, &testAllocator); const Obj& X = mX;
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
                    const int    NUM_ELEMENTS = (int) (END_POS - BEGIN_POS);
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    Obj mX(INIT_LENGTH, &testAllocator); const Obj& X = mX;
                    mX.reserve(INIT_CAP);

                    size_t m = 0;
                    for (m = 0; m < INIT_LENGTH; ++m) {
                        mX[m] =  VALUES[m % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    const int BB = (int) testAllocator.numBlocksTotal();
                    const int  B = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\tBefore:"); P_(BB); P(B);
                    }

                    mX.erase(X.begin() + BEGIN_POS, X.begin() + END_POS);

                    const int AA = (int) testAllocator.numBlocksTotal();
                    const int  A = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, INIT_LENGTH, INIT_CAP,
                                 NUM_ELEMENTS, LENGTH == X.size());

                    for (m = 0; m < BEGIN_POS; ++m) {
                        LOOP5_ASSERT(INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                                     VALUES[m % NUM_VALUES] == X[m]);
                    }
                    for (; m < LENGTH; ++m) {
                        LOOP5_ASSERT(
                              INIT_LINE, LENGTH, BEGIN_POS, END_POS, m,
                              VALUES[(m + NUM_ELEMENTS) % NUM_VALUES] == X[m]);
                    }
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
                    const int    NUM_ELEMENTS = (int) (END_POS - BEGIN_POS);
                    const size_t LENGTH       = INIT_LENGTH - NUM_ELEMENTS;

                    if (veryVerbose) {
                        printf("\t\t\tErase elements between ");
                        P_(BEGIN_POS); P(END_POS);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const int AL = (int) testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, &testAllocator); const Obj& X = mX;
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
void TestDriver<TYPE,ALLOC>::testCase18()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    // We have the following concerns:
    //   1) That the resulting deque value is correct.
    //   2) That the 'insert' return (if any) value is a valid iterator, even
    //      when the deque underwent a reallocation.
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
    //      of the deque does not suffer from aliasing problems.
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
    //   deque reallocation (if capacity changes; all elements are moved), and
    //   for insertion the only reallocations should be for the new elements
    //   plus one if the deque undergoes a reallocation (capacity changes).
    //
    //   For concern 8, we insert an element of some deque where all the
    //   values are distinct into the same deque, taking care of the cases
    //   where the reference is before or after the position of insertion, and
    //   that the deque undergoes a reallocation or not (i.e., capacity
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

                    Obj mX(INIT_LENGTH, &testAllocator);  const Obj& X = mX;
                    mX.reserve(INIT_CAP);

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        mX[k] =  VALUES[k % NUM_VALUES];
                    }

                    if (veryVerbose) {
                        printf("\t\t\tInsert with "); P_(LENGTH);
                        printf(" at "); P_(POS);
                        printf(" using "); P(VALUE);
                    }

                    const int BB = (int) testAllocator.numBlocksTotal();
                    const int  B = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    iterator result = mX.insert(X.begin() + POS, VALUE);

                    const int AA = (int) testAllocator.numBlocksTotal();
                    const int  A = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter :"); P_(AA); P(A);
                        T_; T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, j, LENGTH == X.size());
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

                        Obj mX(INIT_LENGTH, &testAllocator); const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(VALUE);
                        }

                        const int BB = (int) testAllocator.numBlocksTotal();
                        const int  B = (int) testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P(B);
                        }

                        mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);

                        const int AA = (int) testAllocator.numBlocksTotal();
                        const int  A = (int) testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P(A);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());

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
                            const int AL =
                                         (int) testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                            mX.reserve(INIT_CAP);
                            const Obj& X = mX;

                            testAllocator.setAllocationLimit(AL);

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P_(X.capacity()); P(POS);
                            }

                            mX.insert(X.begin() + POS, NUM_ELEMENTS, VALUE);
                                                         // test insertion here

                            if (veryVerbose) {
                                T_; T_; T_; printf("After: "); P_(X);
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());

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
                            printf("\n");
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
template <class CONTAINER>
void TestDriver<TYPE,ALLOC>::testCase18Range(const CONTAINER&)
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    // We have the following concerns:
    //   1) That the resulting deque value is correct.
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
    //   reallocations are for the new elements plus one if the deque
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
                    const int     LINE         = U_DATA[ti].d_lineNum;
                    const char   *SPEC         = U_DATA[ti].d_spec;
                    const int     NUM_ELEMENTS = (int) strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        Obj mX(INIT_LENGTH, &testAllocator); const Obj& X = mX;
                        mX.reserve(INIT_CAP);

                        size_t k;
                        for (k = 0; k < INIT_LENGTH; ++k) {
                            mX[k] =  VALUES[k % NUM_VALUES];
                        }

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        const int BB = (int) testAllocator.numBlocksTotal();
                        const int  B = (int) testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tBefore:"); P_(BB); P_(B); P(mX);
                        }
                        mX.insert(X.begin() + POS, U.begin(), U.end());

                        const int AA = (int) testAllocator.numBlocksTotal();
                        const int  A = (int) testAllocator.numBlocksInUse();

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter :"); P_(AA); P_(A); P(mX);
                            T_; T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                     LENGTH == X.size());

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
                    const int     NUM_ELEMENTS = (int) strlen(SPEC);
                    const size_t  LENGTH       = INIT_LENGTH + NUM_ELEMENTS;

                    CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                    for (size_t j = 0; j <= INIT_LENGTH; ++j) {
                        const size_t POS = j;

                        if (veryVerbose) {
                            printf("\t\t\tInsert "); P_(NUM_ELEMENTS);
                            printf("at "); P_(POS);
                            printf("using "); P(SPEC);
                        }

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                            const int AL =
                                         (int) testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);

                            Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                            mX.reserve(INIT_CAP);
                            const Obj& X = mX;

                            testAllocator.setAllocationLimit(AL);

                            if (veryVerbose) {
                                printf("\t\t\tBefore "); P(mX);
                            }

                            mX.insert(X.begin() + POS, U.begin(), U.end());
                                                         // test insertion here

                            if (veryVerbose) {
                                printf("\t\t\tAfter "); P(mX);
                            }

                            if (veryVerbose) {
                                T_; T_; T_; P_(X); P(X.capacity());
                            }

                            LOOP4_ASSERT(INIT_LINE, LINE, i, j,
                                         LENGTH == X.size());

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
void TestDriver<TYPE,ALLOC>::testCase17()
{
    // --------------------------------------------------------------------
    // TESTING INSERTION:
    // We have the following concerns:
    //   1) That the resulting deque value is correct.
    //   2) That the resulting capacity is correctly set up.
    //   3) That insertion is exception neutral w.r.t. memory allocation.
    //   4) The internal memory management system is hooked up properly
    //      so that *all* internally allocated memory draws from a
    //      user-supplied allocator whenever one is specified.
    //   5) The initial internal state of the deque will not affect the
    //      correctness of the operation.
    //
    // Plan:
    //   For insertion we will create objects of varying sizes and capacities
    //   containing default values, and insert a distinct 'value' using
    //   'push_back' and 'push_front'.  Perform the above test:
    //      - Without exceptions, and compute the number of allocations.
    //      - In the presence of exceptions during memory allocations using
    //        a 'bslma::TestAllocator' and varying its *allocation* *limit*,
    //        but do not compute the number of allocations.
    //   and use basic accessors to verify the resulting
    //      - size
    //      - element value at each index position { 0 .. length - 1 }.
    //
    //   For concern 5, 'setInternalState' will be used to permutate the
    //   internal state of the deque.
    //
    // Testing:
    //   void push_back(T&& value);
    //   void push_front(T&& value);
    // -----------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE DEFAULT_VALUE = TYPE(::DEFAULT_VALUE);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

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

    if (verbose) printf("\nTesting 'push_back'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (size_t j = INIT_LENGTH; j <= LENGTH; ++j) {
                const size_t INIT_CAP = j;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                mX.reserve(INIT_CAP);
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\t\t'push_back' using "); P(VALUE);
                }

                TYPE mV(VALUE);

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore:"); P_(BB); P(B);
                }

                mX.push_back(mV);

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter :"); P_(AA); P(A);
                    T_; T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, i, INIT_CAP, LENGTH == X.size());

                size_t k = 0;
                for (k = 0; k < INIT_LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_CAP, LENGTH, k,
                                 DEFAULT_VALUE == X[k]);
                }
                LOOP3_ASSERT(INIT_LINE, INIT_CAP, INIT_LENGTH,
                             VALUE == X[INIT_LENGTH]);
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\nTesting 'push_front'.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const TYPE   VALUE        = VALUES[i % NUM_VALUES];
            const size_t LENGTH       = INIT_LENGTH + 1;

            for (size_t j = INIT_LENGTH; j <= LENGTH; ++j) {
                const size_t INIT_CAP = j;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                mX.reserve(INIT_CAP);
                const Obj& X = mX;

                if (veryVerbose) {
                    printf("\t\t\t'push_front' using "); P(VALUE);
                }

                TYPE mV(VALUE);

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tBefore:"); P_(BB); P(B);
                }

                mX.push_front(mV);

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter :"); P_(AA); P(A);
                    T_; T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP3_ASSERT(INIT_LINE, i, INIT_CAP, LENGTH == X.size());

                size_t k = 0;
                LOOP3_ASSERT(INIT_LINE, INIT_CAP, INIT_LENGTH,
                             VALUE == X[k]);
                for (k = 1; k < LENGTH; ++k) {
                    LOOP4_ASSERT(INIT_LINE, INIT_CAP, LENGTH, k,
                                 DEFAULT_VALUE == X[k]);
                }
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting 'push_back' with exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];

            for (size_t l = INIT_LENGTH; l < LENGTH; ++l) {
                const size_t INIT_CAP = l;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const int AL = (int) testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                    mX.reserve(INIT_CAP);
                    const Obj& X = mX;

                    testAllocator.setAllocationLimit(AL);

                    if (veryVerbose) {
                        T_; T_; T_; printf("Before: "); P_(X); P(X.capacity());
                    }

                    mX.push_back(VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; printf("After: "); P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, l, LENGTH == X.size());

                    size_t k;
                    for (k = 0; k < INIT_LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, i, l, k,
                                     DEFAULT_VALUE == X[k]);
                    }
                    LOOP3_ASSERT(INIT_LINE, i, l, VALUE == X[k]);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\tTesting 'push_front' with exceptions.\n");
    {
        for (int i = 0; i < NUM_DATA; ++i) {
            const int    INIT_LINE   = DATA[i].d_lineNum;
            const size_t INIT_LENGTH = DATA[i].d_length;
            const size_t LENGTH      = INIT_LENGTH + 1;
            const TYPE   VALUE       = VALUES[i % NUM_VALUES];

            for (size_t l = INIT_LENGTH; l <= LENGTH; ++l) {
                const size_t INIT_CAP = l;
                ASSERT(INIT_LENGTH <= INIT_CAP);

                if (veryVerbose) {
                    printf("\t\tWith initial value of ");
                    P_(INIT_LENGTH); P_(INIT_CAP);
                    printf("using default value.\n");
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const int AL = (int) testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, DEFAULT_VALUE, &testAllocator);
                    mX.reserve(INIT_CAP);
                    const Obj& X = mX;

                    testAllocator.setAllocationLimit(AL);

                    if (veryVerbose) {
                        T_; T_; T_; printf("Before: "); P_(X); P(X.capacity());
                    }

                    mX.push_front(VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; printf("After: "); P_(X); P(X.capacity());
                    }

                    LOOP3_ASSERT(INIT_LINE, i, l, LENGTH == X.size());

                    size_t k = 0;
                    LOOP3_ASSERT(INIT_LINE, i, l, VALUE == X[k]);
                    for (k = 1; k < LENGTH; ++k) {
                        LOOP4_ASSERT(INIT_LINE, i, l, k,
                                     DEFAULT_VALUE == X[k]);
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
}

template <class TYPE, class ALLOC>
void TestDriver<TYPE,ALLOC>::testCase16()
{
    // --------------------------------------------------------------------
    // TESTING ITERATORS
    // Concerns:
    //   1) That 'begin' and 'end' return mutable iterators for a
    //      reference to a modifiable deque, and non-mutable iterators
    //      otherwise.
    //   2) That the range '[begin(), end())' equals the value of the deque.
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
    //   different lengths, create a test deque with this value, and access
    //   each element in sequence and in reverse sequence, both as a modifiable
    //   reference (setting it to a default value, then back to its original
    //   value, and as a non-modifiable reference.
    //
    //   For 4--6, use 'bsl::is_same' to assert the identity of iterator
    //   types.  Note that these concerns let us get away with other concerns
    //   such as testing that 'iter[i]' and 'iter + i' advance 'iter' by the
    //   correct number 'i' of positions, and other concern about traits,
    //   because 'bslstl::IteratorTraits' and 'bslstl::ReverseIterator' have
    //   already been fully tested in the 'bslstl_iterator' component.
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
    const int  BLOCK_LENGTH  = Deque_BlockLengthCalcUtil<TYPE>::BLOCK_LENGTH;

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
        ASSERT(1 == (bsl::is_same<iterator,
                        bslstl::RandomAccessIterator<TYPE,
                            bslalg::DequeIterator<TYPE, BLOCK_LENGTH>
                                                                  > >::value));
        ASSERT(1 == (bsl::is_same<const_iterator,
                     bslstl::RandomAccessIterator<const TYPE,
                        bslalg::DequeIterator<TYPE, BLOCK_LENGTH>
                                                                  > >::value));

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
        ASSERT(1 == (bsl::is_same<reverse_iterator,
                                   bsl::reverse_iterator<iterator> >::value));
        ASSERT(1 == (bsl::is_same<const_reverse_iterator,
                              bsl::reverse_iterator<const_iterator> >::value));

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int     LINE   = DATA[ti].d_lineNum;
            const char   *SPEC   = DATA[ti].d_spec;
            const int     LENGTH = (int) strlen(SPEC);

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
    //
    // Plan:
    //   For each value given by variety of specifications of different
    //   lengths, create a test deque with this value, and access each element
    //   (front, back, at each position) both as a modifiable reference
    //   (setting it to a default value, then back to its original value, and
    //   as a non-modifiable reference.  Verify that 'at' throws
    //   'std::out_of_range' when accessing the past-the-end element.
    //
    // Testing:
    //   T& operator[](size_type position);
    //   T& at(size_type n);
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

            Obj mX(&testAllocator);  const Obj& X = mX;
            mX = g(SPEC);

            Obj mY(X);  const Obj& Y = mY;  // control

            if (verbose) { P_(LINE); P(SPEC); }

            if (LENGTH) {
                LOOP_ASSERT(LINE, TYPE(SPEC[0]) == X.front());
                mX.front() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.front());
                mX[0] = Y[0];

                LOOP_ASSERT(LINE, X[LENGTH - 1] == X.back());
                mX.back() = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X.back());
                mX[LENGTH - 1] = Y[LENGTH - 1];
            }

            LOOP_ASSERT(LINE, Y == X);

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X[j]);
                mX[j] = DEFAULT_VALUE;
                LOOP_ASSERT(LINE, DEFAULT_VALUE == X[j]);
                mX.at(j) = Y[j];
                LOOP_ASSERT(LINE, TYPE(SPEC[j]) == X.at(j));
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
            catch (const std::out_of_range& ex) {
                outOfRangeCaught = true;
            }
            LOOP_ASSERT(LINE, Y == X);
            LOOP_ASSERT(LINE, outOfRangeCaught);
        }
    }
#endif
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
    //   For deque 'v' having various initial capacities, call
    //   'v.reserve(n)' for various values of 'n'.  Verify that sufficient
    //   capacity is allocated by filling 'v' with 'n' elements.  Perform
    //   each test in the standard 'bdema' exception-testing macro block.
    //
    // Testing:
    //   void bsl::deque<T>::reserve(size_type n);
    //   void resize(size_type n, T val = T());
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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value;

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
              const int AL = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, (int) CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);

              const int NUM_ALLOC_BEFORE = (int)testAllocator.numAllocations();
              {
                  ExceptionGuard<Obj> guard(&mX, X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, CAP == X.size());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const int NUM_ALLOC_AFTER = (int) testAllocator.numAllocations();
              LOOP_ASSERT(ti, NE > CAP || NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);

              const int AL2 = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, (int) DELTA);
              LOOP_ASSERT(ti, CAP + DELTA == X.size());

              // Extra allocation for new block
              const int BLOCK_LENGTH =
                                     NOMINAL_BLOCK_BYTES > (int) sizeof(TYPE) ?
                                     NOMINAL_BLOCK_BYTES / (int) sizeof(TYPE) :
                                     16;

              const int INITIAL_BLOCK_COUNT =
                            (int) (CAP + (BLOCK_LENGTH - 1)/ 2) / BLOCK_LENGTH;
              const int AFTER_BLOCK_COUNT =
                   (int) (DELTA + CAP + (BLOCK_LENGTH - 1) / 2) / BLOCK_LENGTH;

              LOOP_ASSERT(ti, NUM_ALLOC_AFTER + TYPE_ALLOC * (int)DELTA +
                                    AFTER_BLOCK_COUNT - INITIAL_BLOCK_COUNT ==
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
              const int AL = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, (int) CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              testAllocator.setAllocationLimit(AL);
              {
                  ExceptionGuard<Obj> guard(&mX, X, L_);

                  mX.reserve(NE);
                  LOOP_ASSERT(ti, 0  == X.size());
                  LOOP_ASSERT(ti, NE <= X.capacity());
              }
              // Note: assert mX unchanged via the exception guard destructor.

              const int AL2 = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, (int) NE);
              LOOP_ASSERT(ti, NE == X.size());

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
              const int AL = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;
              LOOP_ASSERT(ti, X.empty());

              stretch(&mX, (int) CAP);
              LOOP_ASSERT(ti, CAP == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());
              LOOP_ASSERT(ti, !(bool)X.size() == X.empty());

              testAllocator.setAllocationLimit(AL);
              const int NUM_ALLOC_BEFORE = (int)testAllocator.numAllocations();
              ExceptionGuard<Obj> guard(&mX, X, L_);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const int NUM_ALLOC_AFTER = (int) testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAP || TYPE_ALLOC ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = CAP; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const int AL2 = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, (int) DELTA);
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
              const int AL = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              Obj mX(Z);  const Obj& X = mX;

              stretchRemoveAll(&mX, (int) CAP);
              LOOP_ASSERT(ti, X.empty());
              LOOP_ASSERT(ti, 0   == X.size());
              LOOP_ASSERT(ti, CAP <= X.capacity());

              const int NUM_ALLOC_BEFORE =(int) testAllocator.numAllocations();
              ExceptionGuard<Obj> guard(&mX, X, L_);

              testAllocator.setAllocationLimit(AL);

              mX.resize(NE, VALUES[ti % NUM_VALUES]);  // test here

              LOOP_ASSERT(ti, NE == X.size());
              LOOP_ASSERT(ti, NE <= X.capacity());
              const int NUM_ALLOC_AFTER = (int) testAllocator.numAllocations();

              LOOP_ASSERT(ti, NE > CAP || TYPE_ALLOC ||
                                          NUM_ALLOC_BEFORE == NUM_ALLOC_AFTER);
              for (size_t j = 0; j < NE; ++j) {
                  LOOP2_ASSERT(ti, j, VALUES[ti % NUM_VALUES] == X[j]);
              }
              guard.release();

              const int AL2 = (int) testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);

              stretch(&mX, (int) DELTA);
              LOOP_ASSERT(ti, NE + DELTA == X.size());
              testAllocator.setAllocationLimit(AL2);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(0 == testAllocator.numMismatches());
            ASSERT(0 == testAllocator.numBlocksInUse());
        }
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

                Obj mX(INIT_LENGTH, &testAllocator);  const Obj& X = mX;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int    LINE   = DATA[ti].d_lineNum;
                    const size_t LENGTH = DATA[ti].d_length;
                    const TYPE   VALUE  = VALUES[ti % NUM_VALUES];

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    mX.assign(LENGTH, VALUE);

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

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

                    if (veryVerbose) {
                        printf("\t\tAssign "); P_(LENGTH);
                        printf(" using "); P(VALUE);
                    }

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        const int AL = (int) testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);

                        Obj mX(INIT_LENGTH, Z);  const Obj& X = mX;
                        ExceptionGuard<Obj> guard(&mX, Obj(), L_);

                        testAllocator.setAllocationLimit(AL);

                        mX.assign(LENGTH, VALUE);  // test here
                        guard.release();

                        if (veryVerbose) {
                            T_; T_; T_; P_(X); P(X.capacity());
                        }

                        LOOP4_ASSERT(INIT_LINE, LINE, i, ti,
                                     LENGTH == X.size());

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
    //     assign(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

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

            Obj mX(INIT_LENGTH, VALUES[i % NUM_VALUES], &testAllocator);
            const Obj& X = mX;

            for (int ti = 0; ti < NUM_U_DATA; ++ti) {
                const int     LINE   = U_DATA[ti].d_lineNum;
                const char   *SPEC   = U_DATA[ti].d_spec;
                const size_t  LENGTH = strlen(SPEC);

                CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                mX.assign(U.begin(), U.end());

                if (veryVerbose) {
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

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

                CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

                if (veryVerbose) {
                    printf("\t\tAssign "); P_(LENGTH);
                    printf(" using "); P(SPEC);
                }

                Obj mY(g(SPEC)); const Obj& Y = mY;

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const int AL = (int) testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    Obj mX(INIT_LENGTH, Z);  const Obj& X = mX;
                    ExceptionGuard<Obj> guard(&mX, Obj(), L_);

                    testAllocator.setAllocationLimit(AL);

                    mX.assign(U.begin(), U.end());  // test here
                    guard.release();

                    if (veryVerbose) {
                        T_; T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP4_ASSERT(INIT_LINE, LINE, i, ti, LENGTH == X.size());

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
    //   deque and check that the value, capacity, and allocator are as
    //   expected, and that no allocation was performed.
    //
    // Testing:
    //   bsl::deque(size_type n, const A& a = A());
    //   bsl::deque(size_type n, const T& value, const A& a = A());
    //   bsl::deque(deque<T,A>&& original);
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

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\t\tBefore:"); P_(BB); P(B); }

                Obj mX(LENGTH, DEFAULT_VALUE, &testAllocator);
                const Obj& X = mX;

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) {
                    printf("\t\t\tAfter :"); P_(AA); P(A);
                    T_; T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
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

                Obj mX(LENGTH, VALUE, &testAllocator);
                const Obj& X = mX;

                if (veryVerbose) {
                    T_; T_; P_(X); P(X.capacity());
                }

                LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                for (size_t j = 0; j < LENGTH; ++j) {
                    LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                }
            }
        }

        if (verbose) printf("\tWith passing an allocator and checking for "
                            "allocation exceptions using default value.\n");
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE   = DATA[ti].d_lineNum;
                const size_t LENGTH = DATA[ti].d_length;

                if (verbose) { printf("\t\tCreating object of "); P(LENGTH); }

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(LENGTH, DEFAULT_VALUE, &testAllocator);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, DEFAULT_VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}
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

                const int BB = (int) testAllocator.numBlocksTotal();
                const int  B = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tBefore: "); P_(BB); P(B);}

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

                    Obj mX(LENGTH, VALUE, &testAllocator);
                    const Obj& X = mX;

                    if (veryVerbose) {
                        T_; T_; P_(X); P(X.capacity());
                    }

                    LOOP2_ASSERT(LINE, ti, LENGTH == X.size());

                    for (size_t j = 0; j < LENGTH; ++j) {
                        LOOP3_ASSERT(LINE, ti, j, VALUE == X[j]);
                    }

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                const int AA = (int) testAllocator.numBlocksTotal();
                const int  A = (int) testAllocator.numBlocksInUse();

                if (veryVerbose) { printf("\t\tAFTER : "); P_(AA); P(A);}

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

                BSLS_TRY {
                    const int TB = (int) defaultAllocator_p->numBytesInUse();
                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    ASSERT(0  == objectAllocator_p->numBytesInUse());

                    Obj x(LENGTH, DEFAULT_VALUE, objectAllocator_p);

                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    if (LENGTH != 0) {
                        ASSERT(0 != objectAllocator_p->numBytesInUse());
                    }
                }
                BSLS_CATCH(const std::bad_alloc&) {
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
                    const int TB = (int) defaultAllocator_p->numBytesInUse();
                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    ASSERT(0  == objectAllocator_p->numBytesInUse());

                    Obj x(LENGTH, VALUE, objectAllocator_p);

                    ASSERT(0  == globalAllocator_p->numBytesInUse());
                    ASSERT(TB == defaultAllocator_p->numBytesInUse());
                    if (LENGTH != 0) {
                        ASSERT(0 != objectAllocator_p->numBytesInUse());
                    }
                }
                BSLS_CATCH(const std::bad_alloc&) {
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
    //    2) That the initial capacity is correctly set up if the initial
    //       'FWD_ITER' is a random-access iterator.
    //    3) That the constructor is exception neutral w.r.t. memory
    //       allocation.
    //    4) That the internal memory management system is hooked up properly
    //       so that *all* internally allocated memory draws from a
    //       user-supplied allocator whenever one is specified.
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
    //
    // Testing:
    //   template <class InputIter>
    //     bsl::deque(InputIter first, InputIter last, const A& a = A());
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

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

            CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;

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

            CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));     const Obj& Y = mY;

            const int BB = (int) testAllocator.numBlocksTotal();
            const int  B = (int) testAllocator.numBlocksInUse();

            Obj mX(U.begin(), U.end(), &testAllocator);
            const Obj& X = mX;

            const int AA = (int) testAllocator.numBlocksTotal();
            const int  A = (int) testAllocator.numBlocksInUse();

            if (veryVerbose) {
                T_; T_; P_(X); P(X.capacity());
                T_; T_; P_(AA - BB); P(A - B);
            }

            LOOP2_ASSERT(LINE, ti, LENGTH == X.size());
            LOOP2_ASSERT(LINE, ti, LENGTH <= X.capacity());

            for (size_t j = 0; j < LENGTH; ++j) {
                LOOP3_ASSERT(LINE, ti, j, Y[j] == X[j]);
            }
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

            CONTAINER mU(gV(SPEC));  const CONTAINER& U = mU;
            Obj mY(g(SPEC));        const Obj& Y = mY;

            const int BB = (int) testAllocator.numBlocksTotal();
            const int  B = (int) testAllocator.numBlocksInUse();

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

            const int AA = (int) testAllocator.numBlocksTotal();
            const int  A = (int) testAllocator.numBlocksInUse();

            if (veryVerbose) { printf("\t\tAfter : "); P_(AA); P(A);}

            LOOP2_ASSERT(LINE, ti, 0 == testAllocator.numBlocksInUse());
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
    //   o That creating an empty deque does not allocate
    //   o That the allocator is passed through to elements
    //   o That the deque class has the 'bslma::UsesBslmaAllocator'
    //
    // Plan:
    //   We first verify that the 'bsl::deque' class has the traits, and
    //   that allocator
    //
    // Testing:
    //   TRAITS
    //
    // TBD When a new deque object Y is created from an old deque object
    //      X, then the standard states that Y should get its allocator by
    //      copying X's allocator (23.1, Point 8).  The STLport deque
    //      implementation does not follow this rule for bslma::Allocator
    //      based allocators.  To verify this behavior for non
    //      bslma::Allocator, should test, copy constructor using one
    //      and verify standard is followed.
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

    ASSERT((bslma::UsesBslmaAllocator<Obj>::value));

    if (verbose)
        printf("\nTesting that empty deque does allocate.\n");
    {
        Obj mX(&testAllocator);
        ASSERT(0 < testAllocator.numBytesInUse());
    }

    if (verbose)
        printf("\nTesting passing allocator through to elements.\n");

    ASSERT((bslma::UsesBslmaAllocator<TYPE>::value));
    {
        Obj mX(1, VALUES[0], &testAllocator);  const Obj& X = mX;
        ASSERT(&testAllocator == X[0].allocator());
    }
    {
        Obj mX(&testAllocator);  const Obj& X = mX;
        mX.push_back(VALUES[0]);
        ASSERT(&testAllocator == X[0].allocator());
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
    //   6.  The copy constructor's internal functionality varies
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
    //   deque<T,A>& operator=(const deque<T,A>& rhs);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
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
                const int         uLen   = (int) strlen(U_SPEC);

                if (verbose) {
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == (int) UU.size());  // same lengths

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int         vLen   = (int) strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length %d:\t", vLen);
                        P(V_SPEC);
                    }

                    const Obj VV = g(V_SPEC);  // control

                    const bool Z = ui == vi;  // flag indicating same values

                    for (int uj = START_POS; uj <= FINISH_POS;
                                                             uj += INCREMENT) {
                        const int U_N = uj;
                        for (int vj = START_POS; vj <= FINISH_POS;
                                                             vj += INCREMENT) {
                            const int V_N = vj;

                            Obj mU(&testAllocator);
                            setInternalState(&mU, U_N);
                            const Obj& U = mU;
                            gg(&mU, U_SPEC);
                            {
                                Obj mV(&testAllocator);
                                setInternalState(&mV, V_N);
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

                    mU = V;  // test assignment here

                    ASSERT((numCopyCtorCalls - NUM_CTOR) <= (int)V.size());
                    ASSERT((numDestructorCalls - NUM_DTOR) <=
                                                 (int)(V.size() + OLD_LENGTH));

                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                    LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                    // ---------v
                            }
                            //  'mV' (and therefore 'V') now out of scope
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
        static const char *SPECS[] = {  // len: 0-2, 4, 9,
            "",        "A",    "BC",     "DEAB",    "EDCBAEDCB",
            0
        };  // null string required as last element

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
                const int         uLen   = (int) strlen(U_SPEC);

                if (verbose) {
                    printf("\tFor lhs objects of length %d:\t", uLen);
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen < uLen);
                uOldLen = uLen;

                const Obj UU = g(U_SPEC);  // control
                LOOP_ASSERT(ui, uLen == (int) UU.size());  // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int         vLen   = (int) strlen(V_SPEC);

                    if (veryVerbose) {
                        printf("\t\tFor rhs objects of length %d:\t", vLen);
                        P(V_SPEC);
                    }

                    // control
                    const Obj VV = g(V_SPEC);

                    for (int uj = START_POS; uj <= FINISH_POS;
                                                             uj += INCREMENT) {
                        const int U_N = uj;
                        for (int vj = START_POS; vj <= FINISH_POS;
                                                             vj += INCREMENT) {
                            const int V_N = vj;

                            if (iteration % iterationModulus == 0) {
                                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(
                                                               testAllocator) {
                    //--------------^
                    const int AL = (int) testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Obj mU(&testAllocator);
                    setInternalState(&mU, U_N);
                    const Obj& U = mU;
                    gg(&mU, U_SPEC);
                    {
                        Obj mV(&testAllocator);
                        setInternalState(&mV, V_N);
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
                            mU = V;  // test assignment here
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

            for (int tj = START_POS; tj <= FINISH_POS; tj += INCREMENT) {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const int AL = (int) testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = tj;
                    Obj mY(&testAllocator);
                    setInternalState(&mY, N);
                    const Obj& Y = mY;
                    gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    {
                        ExceptionGuard<Obj> guard(&mY, Y, L_);
                        mY = Y;  // test assignment here
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
    //   bsl::deque g(const char *spec);
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
        const int TOTAL_BLOCKS_BEFORE = (int) testAllocator.numBlocksTotal();
        const int IN_USE_BYTES_BEFORE = (int) testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, X == g(SPEC));
        const int TOTAL_BLOCKS_AFTER = (int) testAllocator.numBlocksTotal();
        const int IN_USE_BYTES_AFTER = (int) testAllocator.numBytesInUse();
        LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
        LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
    }

    if (verbose) printf("\nConfirm return-by-value.\n");
    {
        const char *SPEC = "ABCDE";

        // compile-time fact
        ASSERT(sizeof(Obj) == sizeof g(SPEC));

        Obj x(&testAllocator);  // runtime tests
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
    //   deque<T,A>(const deque<T,A>& original);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    // if moveable, moves do not count as allocs
    const int TYPE_MOVE  = ! bslmf::IsBitwiseMoveable<TYPE>::value;
    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<TYPE>::value;

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

        int oldLen = -1;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC   = SPECS[ti];
            const int         LENGTH = (int) strlen(SPEC);

            if (verbose) {
                printf("\nFor an object of length %d:\n", LENGTH);
                P(SPEC);
            }

            LOOP_ASSERT(SPEC, oldLen < (int) LENGTH); // strictly increasing
            oldLen = LENGTH;

            // Create control object w.
            Obj mW; gg(&mW, SPEC);
            const Obj& W = mW;

            LOOP_ASSERT(ti, LENGTH == (int) W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            // Permutate through the initial internal representation.
            for (int ei = START_POS; ei <= FINISH_POS; ei += INCREMENT) {

                const int N = ei;
                if (veryVerbose) { printf("\t\tExtend By  : "); P(N); }

                Obj *pX = new Obj(&testAllocator);
                Obj& mX = *pX;

                setInternalState(&mX, N);
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
                }
                {   // Testing concern 5.

                    if (veryVerbose) printf("\t\t\tInsert into created obj, "
                                            "without test allocator:\n");

                    Obj Y1(X);

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Insert: "); P(Y1);
                    }

                    for (int i = 1; i < N+1; ++i) {

                        stretch(&Y1, 1, VALUES[i % NUM_VALUES]);

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Insert : ");
                            P_(Y1.capacity()); P_(i); P(Y1);
                        }

                        LOOP3_ASSERT(SPEC, N, i, (int)Y1.size() == LENGTH + i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y1);
                        LOOP3_ASSERT(SPEC, N, i, X != Y1);
                    }
                }
                {   // Testing concern 5 with test allocator.

                    if (veryVerbose)
                        printf("\t\t\tInsert into created obj, "
                                "with test allocator:\n");

                    const int BB = (int) testAllocator.numBlocksTotal();
                    const int  B = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tBefore Creation: "); P_(BB); P(B);
                    }

                    Obj Y11(X, &testAllocator);

                    const int AA = (int) testAllocator.numBlocksTotal();
                    const int  A = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                        printf("\t\t\t\tBefore Append: "); P(Y11);
                    }

                    for (int i = 1; i < N+1; ++i) {

                        stretch(&Y11, 1, VALUES[i % NUM_VALUES]);

                        // Blocks allocated should increase only when
                        // trying to add more than capacity.  When adding
                        // the first element numBlocksInUse will increase
                        // by 1.  In all other conditions numBlocksInUse
                        // should remain the same.

                        if (veryVerbose) {
                            printf("\t\t\t\tAfter Append : ");
                            P_(i); P(Y11);
                        }

                        LOOP3_ASSERT(SPEC, N, i, (int) Y11.size() == LENGTH+i);
                        LOOP3_ASSERT(SPEC, N, i, W != Y11);
                        LOOP3_ASSERT(SPEC, N, i, X != Y11);
                        LOOP3_ASSERT(SPEC, N, i,
                                     Y11.get_allocator() == X.get_allocator());
                    }
                }
                {   // Exception checking.

                    const int BB = (int) testAllocator.numBlocksTotal();
                    const int  B = (int) testAllocator.numBlocksInUse();

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

                    const int AA = (int) testAllocator.numBlocksTotal();
                    const int  A = (int) testAllocator.numBlocksInUse();

                    if (veryVerbose) {
                        printf("\t\t\t\tAfter Creation: "); P_(AA); P(A);
                    }
                }
                {   // with 'original' destroyed
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
    //   operator==(const deque<T,A>&, const deque<T,A>&);
    //   operator!=(const deque<T,A>&, const deque<T,A>&);
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator1(veryVeryVerbose);
    bslma::TestAllocator testAllocator2(veryVeryVerbose);

    bslma::Allocator *ALLOCATOR[] = {
        &testAllocator1,
        &testAllocator2
    };

    const int NUM_ALLOCATOR = sizeof ALLOCATOR / sizeof *ALLOCATOR;

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
                const int         LENGTH = (int) strlen(U_SPEC);

                Obj mU(ALLOCATOR[ai]); const Obj& U = gg(&mU, U_SPEC);
                LOOP2_ASSERT(si, ai, LENGTH == (int) U.size()); // same lengths

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n", LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= LENGTH);  //non-decreasing
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
        int oldLen = -1;

        // Create first object
        for (int si = 0; SPECS[si]; ++si) {
            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {

                const char *const U_SPEC = SPECS[si];
                const int         LENGTH = (int) strlen(U_SPEC);

                Obj mU(ALLOCATOR[ai]); const Obj& U = mU;
                gg(&mU, U_SPEC);
                LOOP_ASSERT(si, LENGTH == (int) U.size());  // same lengths

                if (LENGTH != oldLen) {
                    if (verbose)
                        printf( "\tUsing lhs objects of length %d.\n", LENGTH);
                    LOOP_ASSERT(U_SPEC, oldLen <= (int)LENGTH);
                    oldLen = LENGTH;
                }

                if (veryVerbose) { P_(si); P_(U_SPEC); P(U); }
                // Create second object
                for (int sj = 0; SPECS[sj]; ++sj) {
                    for (int aj = 0; aj < NUM_ALLOCATOR; ++aj) {
                        //Perform perturbation
                        for (int e = START_POS; e <= FINISH_POS; e+=INCREMENT){

                            const char *const V_SPEC = SPECS[sj];
                            Obj mV(ALLOCATOR[aj]); const Obj& V = mV;

                            setInternalState(&mV, e);
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

    if (verbose) printf("\nTesting const and non-const versions of "
                        "operator[] and function at() where pos < size().\n");
    {
        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_lineNum;
            const char *const SPEC   = DATA[ti].d_spec_p;
            const int         LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;

            Obj mExp;
            const Obj& EXP = gg(&mExp, e);   // expected spec

            ASSERT(LENGTH <= MAX_LENGTH);

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                Obj mX(ALLOCATOR[ai]);

                const Obj& X = gg(&mX, SPEC);  // canonical organization

                LOOP2_ASSERT(ti, ai, LENGTH == (int) X.size()); // same lengths

                if (veryVerbose) {
                    printf( "\ton objects of length %d:\n", LENGTH);
                }

                if (LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                int i;
                for (i = 0; i < LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X[i]);
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == mX.at(i));
                    LOOP3_ASSERT(LINE, ai, i, EXP[i] == X.at(i));
                }

                for (; i < MAX_LENGTH; ++i) {
                    LOOP3_ASSERT(LINE, ai, i, 0 == e[i]);
                }

                // Perform the perturbation.
                for (int i = START_POS; i <= FINISH_POS; i += INCREMENT) {

                    Obj mY(ALLOCATOR[ai]);
                    setInternalState(&mY, i);

                    const Obj& Y = gg(&mY, SPEC);

                    if (veryVerbose) { T_; T_; T_; P_(i); P(Y); }

                    int j;
                    for (j = 0; j < LENGTH; ++j) {
                        LOOP4_ASSERT(LINE, ai, j, i, EXP[j] == mY[j]);
                        LOOP4_ASSERT(LINE, ai, j, i, EXP[j] == Y[j]);
                        LOOP4_ASSERT(LINE, ai, j, i, EXP[j] == mY.at(j));
                        LOOP4_ASSERT(LINE, ai, j, i, EXP[j] == Y.at(j));
                    }

                    for (; j < MAX_LENGTH; ++j) {
                        LOOP4_ASSERT(LINE, ai, j, i, 0 == e[j]);
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
            const int         LINE  = DATA[ti].d_lineNum;
            const char *const SPEC  = DATA[ti].d_spec_p;
            const int       LENGTH  = DATA[ti].d_length;
            const char *const e     = DATA[ti].d_elements;

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                Obj mX(ALLOCATOR[ai]);

                const Obj& X = gg(&mX, SPEC);

                LOOP2_ASSERT(ti, ai, LENGTH == (int) X.size()); // same lengths

                if (veryVerbose) {
                    printf("\tOn objects of length %d:\n", LENGTH);
                }

                if (LENGTH != oldLen) {
                    LOOP2_ASSERT(LINE, ai, oldLen <= (int)LENGTH);
                          // non-decreasing
                    oldLen = LENGTH;
                }

                if (veryVerbose) printf( "\t\tSpec = \"%s\"\n", SPEC);

                if (veryVerbose) {
                    T_; T_; T_; P(X);
                }

                Obj mY(ALLOCATOR[ai]); const Obj& Y = mY;
                Obj mZ(ALLOCATOR[ai]); const Obj& Z = mZ;

                mY.resize(LENGTH);
                mZ.resize(LENGTH);

                // Change state of Y and Z so its same as X.

                for (int j = 0; j < LENGTH; j++) {
                    mY[j] = TYPE(e[j]);
                    mZ.at(j) = TYPE(e[j]);
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
            const int         LENGTH  = DATA[ti].d_length;

            for (int ai = 0; ai < NUM_ALLOCATOR; ++ai) {
                int exceptions, trials;

                const int NUM_TRIALS = 2;

                // Check exception behavior for non-const version of at()
                // Checking the behavior for pos == size() and
                // pos > size().

                for (exceptions = 0, trials = 0; trials < NUM_TRIALS
                                               ; ++trials) {
                    try {
                        Obj mX(ALLOCATOR[ai]);
                        gg(&mX, SPEC);
                        mX.at(LENGTH + trials);
                    } catch (const std::out_of_range& ex) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("In out_of_range exception.\n");
                            printf("Exception: %s\n", ex.what());
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
                    } catch (const std::out_of_range& ex) {
                        ++exceptions;
                        if (veryVerbose) {
                            printf("In out_of_range exception.\n" );
                            printf("Exception: %s\n", ex.what());
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
    //   deque<T,A>& gg(deque<T,A> *object, const char *spec);
    //   int ggg(deque<T,A> *object, const char *spec, int vF = 1);
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
            const size_t    LENGTH = DATA[ti].d_length;
            const char *const e      = DATA[ti].d_elements;
            const int         curLen = (int)strlen(SPEC);

            Obj mX(&testAllocator);
            const Obj& X = gg(&mX, SPEC);  // original spec

            static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
            char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

            Obj mY(&testAllocator);
            const Obj& Y = gg(&mY, buf);  // extended spec

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
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == X[i]);
                LOOP2_ASSERT(LINE, i, TYPE(e[i]) == Y[i]);
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
            const int         LENGTH  = (int) strlen(SPEC);

            Obj mX(&testAllocator);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length %d:\n", LENGTH);
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
    //          object of length i, use 'clear' to clear its value and
    //          confirm (with 'length'), use insert to set the object to a
    //          value of length j, verify the value, and allow the object to
    //          leave scope.
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
    //   deque<T,A>(const A& a = A());
    //   ~deque<T,A>();
    //   void push_back(const T&);
    //   void clear();
    // --------------------------------------------------------------------

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    const TYPE         *values     = 0;
    const TYPE *const&  VALUES     = values;
    const int           NUM_VALUES = getValues(&values);

    // If bitwise moveable, then move does not count as an alloc
    const int TYPE_MOVE = ! bslmf::IsBitwiseMoveable<TYPE>::value;
    const int TYPE_ALLOC  = bslma::UsesBslmaAllocator<TYPE>::value;

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
        const Obj X(&testAllocator);

        if (veryVerbose) { T_; T_; P(X); }
        ASSERT(0 == X.size());
    }

    if (verbose) printf("\t\tIn place using a test allocator.\n");
    {
        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 == objectAllocator_p->numBytesInUse());

        Obj x(objectAllocator_p);

        ASSERT(0 == globalAllocator_p->numBytesInUse());
        ASSERT(0 == defaultAllocator_p->numBytesInUse());
        ASSERT(0 != objectAllocator_p->numBytesInUse());
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
            if (verbose) {
                printf("\t\tOn an object of initial length %llu.\n",
                                                                  (Uint64) li);
            }

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
            if (verbose) {
                printf("\t\tOn an object of initial length %llu.\n",
                                                                  (Uint64) li);
            }

            Obj mX(&testAllocator);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const int BB = (int) testAllocator.numBlocksTotal();
            const int B  = (int) testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); P_(X.capacity()); P(X);
            }

            mX.push_back(VALUES[li % NUM_VALUES]);

            const int AA = (int) testAllocator.numBlocksTotal();
            const int A  = (int) testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\t AFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            // Deque increases capacity only if the current length is zero or
            // a power of 2.  In addition, when the type allocates, an extra
            // allocation is used for the new element, and when the type is not
            // bitwise moveable, size() allocations are used during the move,
            // but an equal amount is destroyed thus the number of blocks in
            // use is unchanged.

            LOOP_ASSERT(li, li + 1 == X.size());

            for (size_t i = 0; i < li; ++i) {
                LOOP2_ASSERT(li, i, VALUES[i % NUM_VALUES] == X[i]);
            }
            LOOP_ASSERT(li, VALUES[li % NUM_VALUES] == X[li]);
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' without allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) {
                printf("\t\tOn an object of initial length %llu.\n",
                                                                  (Uint64) li);
            }

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
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting 'clear' with allocator.\n");
    {
        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;

        for (size_t li = 0; li < NUM_TRIALS; ++li) {
            if (verbose) {
                printf("\t\tOn an object of initial length %llu.\n",
                                                                  (Uint64) li);
            }

            Obj mX(&testAllocator);  const Obj& X = mX;

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const int BB = (int) testAllocator.numBlocksTotal();
            const int B  = (int) testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tBEFORE: ");
                P_(BB); P_(B); int Cap = (int) X.capacity();P_(Cap);P(X);
            }

            mX.clear();

            const int AA = (int) testAllocator.numBlocksTotal();
            const int A  = (int) testAllocator.numBlocksInUse();

            if (veryVerbose) {
                printf("\t\t\tAFTER: ");
                P_(AA); P_(A); P_(X.capacity()); P(X);
            }

            for (size_t i = 0; i < li; ++i) {
                mX.push_back(VALUES[i % NUM_VALUES]);
            }

            LOOP_ASSERT(li, li == X.size());

            const int CC = (int) testAllocator.numBlocksTotal();
            const int C  = (int) testAllocator.numBlocksInUse();

            if(veryVerbose){
                printf("\t\t\tAFTER SECOND INSERT: ");
                P_(CC); P_(C); P_(X.capacity()); P(X);
            }

            LOOP_ASSERT(li, li == X.size());
        }
    }
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    // --------------------------------------------------------------------

    if (verbose) printf("\n\tTesting the destructor and exception neutrality "
                        "with allocator.\n");

    if (verbose) printf("\t\tWith 'push_back' only\n");
    {
        // For each lengths li up to some modest limit:
        //    1. create an object
        //    2. insert { V0, V1, V2, V3, V4, V0, ... }  up to length 'li'
        //    3. verify initial length and contents
        //    4. allow the object to leave scope
        //    5. make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t li = 0; li < NUM_TRIALS; ++li) { // i is the length
            if (verbose) {
                printf("\t\t\tOn an object of length %llu.\n", (Uint64) li);
            }

          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {

            Obj mX(&testAllocator);  const Obj& X = mX;              // 1.
            for (size_t i = 0; i < li; ++i) {                        // 2.
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
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());

    if (verbose) printf("\t\tWith 'push_back' and 'clear'\n");
    {
        // For each pair of lengths (i, j) up to some modest limit:
        //    1. create an instance
        //    2. insert V0 values up to a length of i
        //    3. verify initial length and contents
        //    4. clear contents from instance
        //    5. verify length is 0
        //    6. insert { V0, V1, V2, V3, V4, V0, ... }  up to length j
        //    7. verify new length and contents
        //    8. allow the instance to leave scope
        //    9. make sure that the destructor cleans up

        const size_t NUM_TRIALS = LARGE_SIZE_VALUE;
        for (size_t i = 0; i < NUM_TRIALS; ++i) { // i is first length
            if (verbose) {
                printf("\t\t\tOn an object of initial length %llu.\n",
                                                                   (Uint64) i);
            }

            for (size_t j = 0; j < NUM_TRIALS; ++j) { // j is second length
                if (veryVerbose) {
                    printf("\t\t\t\tAnd with final length %llu.\n",
                                                                   (Uint64) j);
                }

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
    ASSERT(0 == testAllocator.numMismatches());
    ASSERT(0 == testAllocator.numBlocksInUse());
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
    //   assignment for a non-empty instance [11] and allow the result to
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
        int capacity = (int) X1.capacity();
        T_; T_;
        P(capacity);
    }

    if (verbose) printf("\tb) Try equality operators: x1 <op> x1.\n");
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
      case 26: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //
        // Concern:
        //   Demonstrate properties of deque with an abstract demonstration
        //   showing numerous properties of the container.
        // --------------------------------------------------------------------

        if (verbose) printf("\ndeque Usage Example\n"
                              "===================\n");
//..
// Then, we define a type 'MyDeque' which is a 'deque' containing 'int's, and
// a forward iterator for it:
//..
        typedef bsl::deque<int> MyDeque;
        typedef MyDeque::iterator It;
//..
// Next, we observe that an iterator to a 'deque', unlike an iterator to a
// 'vector', is not a pointer:
//..
        ASSERT(! (bsl::is_same<int *, It>::value));
        ASSERT(! bsl::is_pointer<It>::value);
//..
// Then, we create an allocator to use for the 'deque', and some test data to
// load into it:
//..
        bslma::TestAllocator ta;

        int DATA1[] = {   0,  2 , 4,  6,  8 };
        int DATA2[] = { -10, -8, -6, -4, -2 };
        enum { NUM_DATA =  sizeof DATA1 / sizeof *DATA1 };
        ASSERT(NUM_DATA == sizeof DATA2 / sizeof *DATA2 );
//..
// Next, we create a 'deque' 'd' and initialize it at construction to have the
// 5 elements of 'DATA1', and check our invariants for it.
//..
        MyDeque d(DATA1 + 0, DATA1 + NUM_DATA, &ta);

        checkInvariants(d);
//..
// Then, we verify that it has as many elements as we expect:
//..
        ASSERT(NUM_DATA == d.size());
        ASSERT(NUM_DATA == d.end()  - d.begin());
        ASSERT(NUM_DATA == d.rend() - d.rbegin());
//..
// Next, we examine the first and last elements in a number of ways:
//..
        ASSERT(0 == d.front());
        ASSERT(0 == d[0]);
        ASSERT(0 == *d.begin());
        ASSERT(0 == *(d.rbegin() + NUM_DATA - 1));

        ASSERT(8 == d.back());
        ASSERT(8 == d[NUM_DATA - 1]);
        ASSERT(8 == *d.rbegin());
        ASSERT(8 == *(d.begin()  + NUM_DATA - 1));
//..
// Then, we verify the expected value of all the elements:
//..
        for (int i = 0; i < NUM_DATA; ++i) {
            ASSERT(2 * i == d[i]);
            ASSERT(2 * i == *(d.begin() + i));
        }
//..
// Next we create a second empty 'deque' and check invariants on it:
//..
        MyDeque d2(&ta);

        checkInvariants(d2);
//..
// Then, we assign the value of the first 'deque' to the second, check
// invariants, and verify they are equal:
//..
        d2 = d;
        checkInvariants(d2);
        ASSERT(d == d2);
//..
// Next, we assert the elements of 'DATA2' at the beginning of the second
// 'deque', and check invariants:
//..
        d2.insert(d2.begin(), DATA2 + 0, DATA2 + NUM_DATA);

        checkInvariants(d2);
//..
// Then, the two 'deque's should now be unequal.  Record whether 'd2' is now
// less than 'd':
//..
        ASSERT(d != d2);
        const bool lesserFlag = d2 < d;
        ASSERT((d2 >= d) != lesserFlag);
        ASSERT((d2 >  d) != lesserFlag);
//..
// Next, we swap 'd2' and 'd', check invariants of both 'deque's after the
// swap, and verify that the equality comparisons and size are as expected:
//..
        d.swap(d2);

        checkInvariants(d);
        checkInvariants(d2);

        ASSERT(d != d2);
        ASSERT((d <  d2) == lesserFlag);
        ASSERT((d >= d2) != lesserFlag);
        ASSERT((d >  d2) != lesserFlag);

        ASSERT(2 * NUM_DATA == d.size());
//..
// Note that had we had any iterators to 'd2', both the 'insert' and the 'swap'
// would have invalidated them.
//
// Then, we verify that 'front' and 'back' are as expected:
//..
        ASSERT(-10 == d.front());
        ASSERT(-10 == d[0]);

        ASSERT(  8 == d.back());
        ASSERT(  8 == d[2 * NUM_DATA - 1]);
//..
// Next, we verify all elements in the 'deque':
//..
        for (int i = 0; i < (int) d.size(); ++i) {
            ASSERT(-10 + 2 * i == d[i]);
            ASSERT(-10 + 2 * i == *(d.begin() + i));
            ASSERT(  8 - 2 * i == *(d.end() - 1 - i));
            ASSERT(  8 - 2 * i == *(d.rbegin() + i));
        }
//..
// Then, we push more elements to the front & back:
//
// Note that if we had any iterators to 'd', these pushes would invalidate them
//..
        d.push_back(10);
        d.push_front(-12);
//..
// Next, using iterators, we verify the sequence of all elements in 'd'
//..
        const It endMinus1 = d.end() - 1;
        for (It it = d.begin(); it < endMinus1; ++it) {
            ASSERT(*it + 2 == *(it + 1));
        }
//..
// Then, we create an iterator and set it to point to the element containing
// '0', and take a reference to the same element:
//..
        It zeroIt = d.begin();
        while (*zeroIt < 0) {
            ++zeroIt;
        }
        ASSERT(0 == *zeroIt);

        ASSERT((bsl::is_same<int&, MyDeque::reference>::value));

        MyDeque::reference zeroRef = *zeroIt;

        ASSERT(0 == zeroRef);
//..
// Next, create an iterator and a reference pointing to 'd.back()':
//..
        It backIt = d.begin() + d.size() - 1;

        ASSERT(10 == *backIt);

        int& backRef = *backIt;
//..
// Then, we pop the front element off 'd'.  Note that 'pop_front' and
// 'pop_back' do not invalidate iterators or references to surviving elements:
//..
        d.pop_front();

        checkInvariants(d);
//..
// Now, we 'erase' all elements before the element containing '0'.  Note that
// 'erase' at either end of the deque does not invalidate iterators or
// references to surviving elements
//..
        d.erase(d.begin(), zeroIt);

        checkInvariants(d);
//..
// Finally, we use our (still valid) iterators and references to verify that
// the contents of 'd' are as expected:
//..
        ASSERT( 0 == *zeroIt);
        ASSERT( 0 == zeroRef);

        ASSERT(10 == *backIt);
        ASSERT(10 == backRef);

        ASSERT(&*zeroIt == &d.front());
        ASSERT(&zeroRef == &d.front());

        ASSERT(&*backIt == &d.back());
        ASSERT(&backRef == &d.back());

        ASSERT( 0 == d.front());
        ASSERT( 0 == d[0]);
        ASSERT(10 == d.back());
        ASSERT(10 == d[5]);

        for (unsigned u = 0; u < d.size(); ++u) {
            ASSERT(2 * (int) u == d[u]);
        }
//..
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //
        // Concern:
        //   Demonstrate a context in which a 'deque' might be useful.
        // --------------------------------------------------------------------

        if (verbose) printf("\nUsage Example 1\n"
                              "===============\n");

        // Then, we define (and default construct) our laundry queue:

        bslma::TestAllocator ta;
        LaundryQueue q(&ta);

        // Next, we add a few customers:

        q.push("Steve Firm");
        q.push("Sally Johnson");
        q.push("Joe Sampson");

        // Then, the following customer bribes the merchant and gets pushed
        // to the front of the queue:

        q.expeditedPush("Dirty Dan");

        // Next, a couple of more regular customers are pushed:

        q.push("Wally Walters");
        q.push("Fred Flintstone");

        // Then, we see who is now next up to have their laundry done, and
        // verify that it is "Dirty Dan":

        bsl::string nxt = q.next();
        ASSERT("Dirty Dan" == nxt);

        // Next, we verify that "Dirty Dan" is no longer in the queue:

        ASSERT(! q.find("Dirty Dan"));

        // Then, we verify that "Sally Johnson" is still in the queue:

        ASSERT(  q.find("Sally Johnson"));

        // Now, we iterate, printing out the names of people whose laundry
        // remains to be done:

        while (true) {
            bsl::string customerName = q.next();
            if ("(* empty *)" == customerName) {
                break;
            }

            printf("Next: %s\n", customerName.c_str());
        }

        // Finally, we observe that these names are printed, in the following
        // order:

        // Next: Steve Firm
        // Next: Sally Johnson
        // Next: Joe Sampson
        // Next: Wally Walters
        // Next: Fred Flintstone
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING EXCEPTIONS
        //
        // Testing:
        //   CONCERN: bsl::length_error is used properly
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting use of 'bsl::length_error'"
                            "\n==================================\n");

        TestDriver<T>::testCase22();

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON FREE OPERATORS
        //
        // Testing:
        //   bool operator<(const deque<T,A>& lhs, const deque<T,A>& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting comparison free operators"
                            "\n=================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase21();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase21();

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING SWAP
        //
        // Testing:
        //   void swap(bsl::deque&);
        //   void swap(deque<T,A>&  lhs, deque<T,A>&  rhs);
        //   void swap(deque<T,A>&& lhs, deque<T,A>&  rhs);
        //   void swap(deque<T,A>&  lhs, deque<T,A>&& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'swap'"
                            "\n==============\n");

        TestDriver<T>::testCase20();

      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING ERASE
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
        TestDriver<char>::testCase19();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase19();

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase19();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase19();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase19();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase19();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase19();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING RANGE ARRAY INSERTION
        //
        // Testing:
        //   template <class InputIter>
        //    void insert(const_iterator pos, InputIter first, InputIter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Range Array Insertion"
                            "\n=============================\n");

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<T>::testCase18Range(CharArray<T>());

        if (verbose) printf("\n... with 'MediumTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<M>::testCase18Range(CharArray<M>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BMT>::testCase18Range(CharArray<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary random-access iterator.\n");
        TestDriver<BCT>::testCase18Range(CharArray<BCT>());
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING RANGE LIST INSERTION
        //
        // Testing:
        //   template <class InputIter>
        //    void insert(const_iterator pos, InputIter first, InputIter last);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Range List Insertion"
                            "\n============================\n");

        if (verbose) printf("\n... with 'TestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<T>::testCase18Range(CharList<T>());

        if (verbose) printf("\n... with 'MediumTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<M>::testCase18Range(CharList<M>());

        if (verbose) printf("\n... with 'BitwiseMoveableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BMT>::testCase18Range(CharList<BMT>());

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' "
                            "and arbitrary input iterator.\n");
        TestDriver<BCT>::testCase18Range(CharList<BCT>());
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING RANGE INSERTION
        //
        // Testing:
        //   iterator insert(const_iterator position, const T& value);
        //   void insert(const_iterator pos, size_type n, const T& val);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase18();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase18();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase18();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase18();
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING INSERTION
        //
        // Testing:
        //   void push_back(const T& value);
        //   void push_front(const T& value);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Value Insertion"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char' type.\n");
        TestDriver<char>::testCase17();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase17();

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase17();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase17();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase17();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase17();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType' .\n");
        TestDriver<BCT>::testCase17();

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

        if (verbose) printf("\nTesting Iterators"
                            "\n=================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase16();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase16();

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

        if (verbose) printf("\nTesting Element Access"
                            "\n======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase15();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase15();

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY
        //
        // Testing:
        //   void reserve(size_type n);
        //   void resize(size_type n, T val);
        //   size_type max_size() const;
        //   size_type capacity() const;
        //   bool empty() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Reserve and Capacity"
                            "\n============================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase14();

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

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Testing:
        //   deque<T,A>(size_type n, const T& val = T(), const A& a = A());
        //   template<class InputIter>
        //     deque<T,A>(InputIter first, InputIter last, const A& a = A());
        //   deque(deque<T,A>&& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Initial-Length Constructor"
                            "\n==================================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase12();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase12();

        if (verbose) printf("\n... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase15();

        if (verbose) printf("\n... with 'BitwiseCopyableTestType'.\n");
        TestDriver<BCT>::testCase15();

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

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR-RELATED CONCERNS
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Allocator concerns"
                            "\n==================================\n");

        TestDriver<T>::testCase11();

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

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase8();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase8();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase8();

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
        //   bsl::deque(const bsl::deque& original);
        //   bsl::deque(const bsl::deque& original, alloc);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase7();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase7();

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase7();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase7();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase7();

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

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        if (verbose) printf("\n... with 'char'.\n");
        TestDriver<char>::testCase4();

        if (verbose) printf("\n... with 'TestType'.\n");
        TestDriver<T>::testCase4();

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase4();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase4();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase4();

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

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase3();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase3();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase3();

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

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase2();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase2();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase2();

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

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCase1();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCase1();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCase1();

        if (verbose) printf("\n\t... with 'BitwiseMoveableTestType'.\n");
        TestDriver<BMT>::testCase1();

        if (verbose) printf("\n\t... with 'BitwiseCopyableTestType' .\n");
        TestDriver<BCT>::testCase1();

        if (verbose) printf("\nAdditional tests: allocators.\n");

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        bsl::allocator<int> zza(&testAllocator);

        // Disabled: in order to use bsl::deque, we disabled this very
        // unfrequent usage for bsl::deque (it will be flagged by
        // 'BSLMF_ASSERT'):
        //..
        // bsl::deque<int, bsl::allocator<void *> > zz1, zz2(zza);
        //..

        if (verbose) printf("\nAdditional tests: misc.\n");

        bsl::deque<char> myVec(5, 'a');
        bsl::deque<char>::const_iterator citer;
        ASSERT(5 == myVec.size());
        ASSERT(5 == myVec.end() - myVec.begin());
        ASSERT(5 == myVec.cend() - myVec.cbegin());
        for (citer = myVec.begin(); citer != myVec.end(); ++citer) {
            LOOP_ASSERT(*citer, 'a' == *citer);
        }
        if (verbose) P(myVec);

        myVec.insert(myVec.begin(), 'z');
        ASSERT(6 == myVec.size());
        ASSERT(6 == myVec.end() - myVec.begin());
        ASSERT(6 == myVec.cend() - myVec.cbegin());
        ASSERT('z' == myVec[0]);
        for (citer = myVec.begin() + 1; citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        myVec.erase(myVec.begin() + 2, myVec.begin() + 4);
        ASSERT(4 == myVec.size());
        ASSERT(4 == myVec.end() - myVec.begin());
        ASSERT(4 == myVec.cend() - myVec.cbegin());
        ASSERT('z' == myVec[0]);
        for (citer = myVec.begin() + 1; citer != myVec.end(); ++citer) {
            ASSERT('a' == *citer);
        }
        if (verbose) P(myVec);

        bsl::deque<bsl::deque<char> > vv;
        vv.push_back(myVec);
        if (verbose) P(myVec);

        if (verbose) printf("\nAdditional tests: traits.\n");

        ASSERT(bslmf::IsBitwiseMoveable<bsl::deque<char> >::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::deque<T> >::value);
        ASSERT(bslmf::IsBitwiseMoveable<bsl::deque<bsl::deque<int> > >::value);

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        //
        // Concerns:
        //   Provide benchmark for subsequent improvements to 'bsl::deque'.
        //   The benchmark should measure the speed of various operations such
        //   as 'push_back', 'push_front', 'pop_back', 'pop_front', 'insert',
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

        if (verbose) printf("\n... with 'SmallTestTypeNoAlloc'.\n");
        TestDriver<S>::testCaseM1();

        if (verbose) printf("\n... with 'MediumTestTypeNoAlloc'.\n");
        TestDriver<M>::testCaseM1();

        if (verbose) printf("\n... with 'LargeTestTypeNoAlloc'.\n");
        TestDriver<L>::testCaseM1();

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
