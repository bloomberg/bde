// bslstl_function.t.cpp                                              -*-C++-*-
#include "bslstl_function.h"

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_removeconst.h>
#include <bsls_bsltestutil.h>
#include <bsls_macrorepeat.h>
#include <bsls_types.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The 'bsl::function' class template is an in-core value-semantic class that
// generalizes the concept of an invocable object.  A 'function' object wraps
// a pointer to function, pointer to member function, function object, or
// reference to function object.  It can also be "empty", i.e., wrap no
// object.  The salient attribute of a 'function' object is the invocable
// object that it wraps (if any).  Because the invocable object is type-erased
// on construction, the run-time type of the invocable object is effectively a
// salient attribute as well.  A 'function' object also has a type-erased
// allocator, which is not a salient attribute.
//
// A 'function' object is effectively immutable, except that it can be
// assigned to.  A wrapped functor can also be modified in the process of
// being invoked.  As an immutable type, the only primitive manipulators are
// the constructors and assignment operators.  The primitive accessors are
// 'operator bool', the invocation operators, the 'allocator' function, and
// the 'target' and 'target_type' functions.  Black-box testing consists of
// constructing 'function' objects with a variety of invocables and testing
// that invocation works correctly and that 'operator bool' and the other
// accessors return the expected values.  The biggest complication in this
// testing is verifying the large number of combinations of invocables and
// invocation prototypes (0 to 20 arguments, with and without return types).
//
// White-box testing looks at the memory allocation pattern. Both the
// invocable and the allocator are type erased and stored in allocated memory.
// The implementation uses the small-object optimization to store one or both
// of these type-erased objects in the footprint of the 'function' object
// itself, when possible.  The allocation pattern is complicated by the fact
// that on copy construction and assignment, the invocable is copied but the
// allocator is not.  Thus, whether or not the small-object optimization is
// applied to the allocator can be different in the original object vs. the
// copy.
//
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

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
// inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
// inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

bool         verbose = false;
bool     veryVerbose = false;
bool veryVeryVerbose = false;
//=============================================================================
//                  EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

class ExceptionLimit
{
    // This class acts as a counter that, when decremented to zero, causes an
    // exception to be thrown.  It is used in exception testing to verify the
    // behavior of the test when an exception is thrown on, e.g., a move or
    // copy operation, after some number of successful operations.  The
    // exception-throwing behavior is disabled by setting the counter to -1.

    int         d_counter;
    const char *d_name;

  public:
    ExceptionLimit(const char *name) : d_counter(-1), d_name(name) { }

    ExceptionLimit& operator=(int newCount) {
        d_counter = newCount;
        return *this;
    }

    ExceptionLimit& operator--() {
        // Decrement 'counter'.  Throw 'this' on transition from zero to
        // negative.
#ifdef BDE_BUILD_TARGET_EXC
        if (d_counter >= 0 && --d_counter < 0) {
            throw this;
        }
#endif
        return *this;
    }

    int value() const { return d_counter; }
    const char *what() const { return d_name; }
};

// These macros are used to test for exception neutrality in the case where
// either an optionally-specified allocator throws an exception or an
// optionally-specified exception limit is reached.  If both the allocator and
// the exception limit are non-null, the exception test is run twice, once
// testing the allocator limit and a second time testing the other exception
// limit.  If an allocator is specified, it must be a 'bslma::TestAllocator*'.
//
// The simple usage of these macros is as follows (curly braces required):
//..
//  bslma::TestAllocator alloc;
//  EXCEPTION_TEST_BEGIN(&alloc, &moveLimit) {
//      EXCEPTION_TEST_TRY {
//          code-that-might-throw;
//          verify-post-conditions;
//      } EXCEPTION_TEST_ENDTRY;
//  } EXCEPTION_TEST_END;
//..
// If there are local variables that need to be constructed each time through
// the exception loop with the exception trigger turned off, they should be
// defined between the 'EXCEPTION_TEST_BEGIN' and 'EXCEPTION_TEST_TRY':
//..
//  bslma::TestAllocator alloc;
//  EXCEPTION_TEST_BEGIN(&alloc, NULL) { // Allocator limit only
//      Obj localVariable(alloc);     // Won't throw a test exception
//      EXCEPTION_TEST_TRY {
//          code-that-might-throw;    // Might throw a test exception
//          verify-post-conditions;
//      } EXCEPTION_TEST_ENDTRY;
//  } EXCEPTION_TEST_END;
//..
// Sometimes, we need to test postconditions on failure.  For example, if
// assignment fails with an exception, both the lhs and rhs should be
// unchanged.  For these cases, the usage is expanded to include
// 'EXCEPTION_TEST_CATCH':
//..
//  bslma::TestAllocator alloc;
//  EXCEPTION_TEST_BEGIN(alloc, NULL) {
//      Obj localVariable(alloc);
//      EXCEPTION_TEST_TRY {
//          code-that-might-throw;
//          verify-success-post-conditions;
//      } EXCEPTION_TEST_CATCH {
//          verify-failure-post-conditions;
//      } EXCEPTION_TEST_ENDTRY;
//  } EXCEPTION_TEST_END;
//..

#ifdef BDE_BUILD_TARGET_EXC

inline
void dumpExTest(const char *s, int bslmaExceptionCounter,
                const char *exLimitName, int exLimitCounter)
{
    if (! veryVeryVerbose) return;                                    // RETURN

    if (bslmaExceptionCounter >= 0) {
        printf("\t***   %s: alloc limit = %d ***\n", s, bslmaExceptionCounter);
    }
    else if (exLimitCounter >= 0) {
        printf("\t***   %s: %s = %d ***\n", s, exLimitName, exLimitCounter);
    }
}

// Run an exception test repeatedly using up to two exception counters: If a
// test allocator is specified, then its built-in exception limit is used.  If
// the address an exception-limit is specified, then it is used to set the
// exception limit.  If both are specified, The test is performed first by
// running through the allocator exception limit, then by running through the
// specified 'exceptionLimit'.  If both are null, then the test is run without
// forcing any exceptions.
#define EXCEPTION_TEST_BEGIN(testAllocator, exceptionLimit) do {              \
    if (veryVeryVerbose) printf("\t*** EXCEPTION_TEST_BEGIN ***\n");     \
    bslma::TestAllocator *testAlloc = (testAllocator);                        \
    ExceptionLimit *exLimit = (exceptionLimit);                               \
    int bslmaExceptionCounter = testAlloc ? 0 : -1;                           \
    int exLimitCounter = (exLimit && ! testAlloc) ? 0 : -1;                   \
    const char *const limitName = exLimit ? exLimit->what() : "(ignored)";    \
    do {                                                                      \
        bool exceptionCaught = false;                                         \
        if (testAlloc) testAlloc->setAllocationLimit(-1);                     \
        if (exLimit) *exLimit = -1;

#define EXCEPTION_TEST_TRY                                                    \
        if (testAlloc) testAlloc->setAllocationLimit(bslmaExceptionCounter);  \
        if (exLimit) *exLimit = exLimitCounter;                               \
        try

#define EXCEPTION_TEST_CATCH                                                  \
        catch (...) {                                                         \
            dumpExTest("EXCEPTION CAUGHT",                                    \
                       bslmaExceptionCounter, limitName, exLimitCounter);     \
            exceptionCaught = true;                                           \
            if (testAlloc) testAlloc->setAllocationLimit(-1);                 \
            if (exLimit) *exLimit = -1;                                       \
        }                                                                     \
        if (exceptionCaught) try

#define EXCEPTION_TEST_ENDTRY                                                 \
        catch (...) {                                                         \
            dumpExTest("EXCEPTION CAUGHT",                                    \
                       bslmaExceptionCounter, limitName, exLimitCounter);     \
            exceptionCaught = true;                                           \
        }                                                                     \
        if (exceptionCaught) {                                                \
            if (bslmaExceptionCounter >= 0) {                                 \
                ++bslmaExceptionCounter;                                      \
            } else if (exLimit) {                                             \
                ++exLimitCounter;                                             \
            }                                                                 \
        } else {                                                              \
            dumpExTest("SUCCEEDED WITH NO EXCEPTION",                         \
                       bslmaExceptionCounter, limitName, exLimitCounter);     \
            if (exLimitCounter >= 0 || NULL == exLimit) {                     \
                break;                                                        \
            }                                                                 \
            bslmaExceptionCounter = -1;                                       \
            exLimitCounter = 0;                                               \
        }

#define EXCEPTION_TEST_END                                                    \
    } while (true);                                                           \
    if (testAlloc) testAlloc->setAllocationLimit(-1);                         \
    if (exLimit) *exLimit = -1;                                               \
    if (veryVeryVerbose) printf("\t*** EXCEPTION_TEST_END ***\n");       \
} while (false)

#else // if ! BDE_BUILD_TARGET_EXC

#define EXCEPTION_TEST_BEGIN(testAllocator, exceptionLimit) do { \
    (void) (testAllocator); (void) (exceptionLimit);
#define EXCEPTION_TEST_TRY do { if (true)
#define EXCEPTION_TEST_CATCH else
#define EXCEPTION_TEST_ENDTRY } while (false)
#define EXCEPTION_TEST_END } while (false)

#endif // BDE_BUILD_TARGET_EXC


//=============================================================================
//                  GLOBAL DEFINITIONS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

// Size type used by test allocator.
typedef bsls::Types::Int64 AllocSizeType;

// Use this test allocator when another allocator is not specified.
bslma::TestAllocator globalTestAllocator("globalTestAllocator");

template <class FUNC>
bsl::Function_NothrowWrapper<FUNC> ntWrap(const FUNC& f)
    // Wrap the specified functor, 'f' in a nothrow wrapper.
{
    return f;
}

#define NTUNWRAP_T(FUNC) bsl::Function_NothrowWrapperUtil<FUNC>::UnwrappedType

template <class FUNC>
typename NTUNWRAP_T(FUNC) const& ntUnwrap(const FUNC& f)
    // If the specified 'f' is a nothrow wrapper, then return the invokable
    // object wrapped in 'f'; otherwise return 'f' unchanged.
{
    return bsl::Function_NothrowWrapperUtil<FUNC>::unwrap(f);
}

// template <class FUNC>
// typename NTUNWRAP_T(FUNC)& ntUnwrap(FUNC& f)
//     // If the specified 'f' is a nothrow wrapper, then return the invokable
//     // object wrapped in 'f'; otherwise return 'f' unchanged.
// {
//     return bsl::Function_NothrowWrapperUtil<FUNC>::unwrap(f);
// }

template <class TYPE>
class SmartPtr
{
    // A simple class with the interface of a smart pointer.

    TYPE *d_pointer;

  public:
    typedef TYPE value_type;

    SmartPtr(TYPE *p = NULL) : d_pointer(p) { }

    TYPE& operator*() const { return *d_pointer; }
    TYPE* operator->() const { return d_pointer; }
};

#define INT_ARGN(n) int arg ## n
#define ARGN(n) arg ## n

#define SUMMING_FUNC(n)                                       \
    int sum ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) {      \
        return BSLS_MACROREPEAT_SEP(n, ARGN, +) + 0x4000;     \
    }

// Increment '*val' by one.  Used to test a void return type.
void increment(int *val)
{
    ++*val;
}

// Create 11 functions with 0 to 10 integer arguments, returning the sum of the
// arguments + 0x4000.
SUMMING_FUNC(0)
BSLS_MACROREPEAT(10, SUMMING_FUNC)

class ConvertibleToInt
{
    // Class of objects implicitly convertible to 'int'

    int d_value;

  public:
    explicit ConvertibleToInt(int v) : d_value(v) { }

    operator int() const { return d_value; }
};

class IntWrapper
{
    // Simple wrapper around an 'int' that supplies member functions (whose
    // address can be taken) for testing 'bsl::function'.

    int d_value;

  public:
    IntWrapper(int i = 0) : d_value(i) { } // Convertible from 'int'
    IntWrapper(ConvertibleToInt i) : d_value(i) { }

    void incrementBy1() { ++d_value; }

    int value() const { return d_value; }

#define ADD_FUNC(n)                                                  \
    int add ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) const {       \
        return value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);           \
    }

#define INCREMENT_FUNC(n)                                            \
    int increment ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) {       \
        return d_value += BSLS_MACROREPEAT_SEP(n, ARGN, +);          \
    }

#define VOID_INCREMENT_FUNC(n)                                       \
    void voidIncrement ## n (BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) {  \
        d_value += BSLS_MACROREPEAT_SEP(n, ARGN, +);                 \
    }

    // Const function with 0 to 9 arguments.  Return value() plus the sum of
    // all arguments.
    int add0() const { return d_value; }
    BSLS_MACROREPEAT(9, ADD_FUNC)

    // Mutable function with 0 to 9 arguments.  Increment the value by the sum
    // of all arguments.  'increment0()' is a no-op.
    int increment0() { return d_value; }
    BSLS_MACROREPEAT(9, INCREMENT_FUNC)

    void voidIncrement0() { }
    BSLS_MACROREPEAT(9, VOID_INCREMENT_FUNC)

    int sub1(int arg) {
        return value() - arg;
    }
};

inline bool operator==(const IntWrapper& a, const IntWrapper& b)
{
    return a.value() == b.value();
}

inline bool operator!=(const IntWrapper& a, const IntWrapper& b)
{
    return a.value() != b.value();
}

class IntWrapperDerived : public IntWrapper
{
    // Derived class of 'IntWrapper'

  public:
    IntWrapperDerived(int v) : IntWrapper(v) { }
};

int *getAddress(int& r) { return &r; }
const int *getConstAddress(const int& r) { return &r; }

class CountCopies
{
    // Counts the number of times an object has been copy-constructed or
    // move-constructed.

    int d_numCopies;

  public:
    CountCopies() : d_numCopies(0) { }
    CountCopies(const CountCopies& other) : d_numCopies(other.d_numCopies+1) {}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Move constructor does not bump count
    CountCopies(CountCopies&& other) : d_numCopies(other.d_numCopies) {}
#endif

    CountCopies& operator=(const CountCopies& rhs)
        { d_numCopies = rhs.d_numCopies + 1; return *this; }

    int numCopies() const { return d_numCopies; }
};

// Return the number of times the specified 'cc' object has been copied.  Note
// that, since 'cc' is passed by value, there is a copy or move on every call.
// Therefore, the return value should never be less than 1.
int numCopies(CountCopies cc) { return cc.numCopies(); }

typedef bsl::Function_SmallObjectOptimization::InplaceBuffer SmallObjectBuffer;

// Simple functions
int simpleFunc(const IntWrapper& iw, int v)
{
    return iw.value() + v;
}

int simpleFunc2(const IntWrapper& iw, int v)
{
    return iw.value() - v;
}

class FunctorBase
{
    // Keep count of the number of functors in existance
    static int s_count;

  public:
    FunctorBase() { ++s_count; }
    FunctorBase(const FunctorBase&) { ++s_count; }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    FunctorBase(FunctorBase&&) { ++s_count; }
#endif
    ~FunctorBase() { --s_count; ASSERT(s_count >= 0); }

    static int count() { return s_count; }
};

int FunctorBase::s_count = 0;

class FunctorMonitor
{
    // An instance of this class can be used to check for a change in the
    // number of functor objects that have been created or destroyed during a
    // specific operation.

    int d_line;     // source line number where 'FunctorMonitor' was created
    int d_snapshot; // Number of 'FunctorBase' objects in existance at the
                    // time when this 'FunctorMonitor' was created.

  public:
    FunctorMonitor(int line) { reset(line); }
    ~FunctorMonitor() {
        if (! isSameCount()) {
            printf("FunctorBase::count(): %d\td_snapshot : %d\n",
                   FunctorBase::count(), d_snapshot);
            aSsErT(1,"isSameCount() at destruction of FunctorMonitor", d_line);
        }
    }

    void reset(int line) {
        d_line     = line;
        d_snapshot = FunctorBase::count();
    }

    bool isSameCount() const { return FunctorBase::count() == d_snapshot; }
};

// Limits the number of copies before a copy operation throws.
ExceptionLimit copyLimit("copy limit");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// Limits the number of moves before a move operation throws.
ExceptionLimit moveLimit("move limit");
#else
ExceptionLimit& moveLimit = copyLimit;
#endif

// Simple functor with no state
struct EmptyFunctor : FunctorBase
{
    // Stateless functor
    // - No allocator
    // - Bitwise movable
    // - Nothrow move constructible

    EmptyFunctor(const EmptyFunctor&) : FunctorBase()
        { --copyLimit; memset(this, 0xdd, sizeof(*this)); }

#if defined BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT && \
    defined BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Nothrow moveable
    EmptyFunctor(EmptyFunctor&&) noexcept
        { memset(this, 0xdd, sizeof(*this)); }
#endif

    enum { IS_STATELESS = true };

    explicit EmptyFunctor(int /* ignored */ = 0)
        // Constructor writes '0xee' over its memory.  Even an empty class has
        // a footprint and any code that optimizes the object away must be
        // careful not to overwrite arbitrary memory by calling the "empty"
        // constructor.
        { memset(this, 0xee, sizeof(*this)); }

    ~EmptyFunctor()
        // Destructor writes '0xbb' over its memory.  Even an empty class has a
        // footprint and any code that optimizes the object away must be
        // careful not to overwrite arbitrary memory by calling the "empty"
        // destructor.
        { memset(this, 0xbb, sizeof(*this)); }

#define OP_PAREN(n)                                                           \
    int operator()(const IntWrapper& iw, BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) \
    {                                                                         \
        return iw.value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);                 \
    }

    // Invocation operator with 0 to 10 arguments.  The first argument (if any)
    // is a const reference to 'IntWrapper', although it can be passed an
    // 'int'.  The remaining arguments are of type 'int'.
    int operator()() { return 0; }
    int operator()(const IntWrapper& iw) { return iw.value(); }
    BSLS_MACROREPEAT(9, OP_PAREN)

#undef OP_PAREN

    // Invocation operator that returns void.
    void operator()(const char*) { }

    int value() const { return 0; }
};

inline
bool operator==(const EmptyFunctor&, const EmptyFunctor&)
{
    return true;
}

inline
bool operator!=(const EmptyFunctor&, const EmptyFunctor&)
{
    return false;
}

class SmallFunctor : public FunctorBase
{
    // Small stateful functor.
    // - No allocator
    // - Bitwise movable
    // - Throwing move construtor

    int d_value;  // Arbitrary state to distinguish one instance from another

    enum { k_MOVED_FROM_VAL = 0x100000 };

  public:
    // BITWISE MOVEABLE
    BSLMF_NESTED_TRAIT_DECLARATION(SmallFunctor, bslmf::IsBitwiseMoveable);

    enum { IS_STATELESS = false };

    explicit SmallFunctor(int v) : d_value(v) { }
    SmallFunctor(const SmallFunctor& other)
      : FunctorBase(), d_value(other.d_value)
        { --copyLimit; }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Move-constructor deliberately modifies 'other'.
    // This move constructor is not called in destructive move settings.
    SmallFunctor(SmallFunctor&& other) : d_value(other.d_value)
        { --moveLimit; other.d_value = k_MOVED_FROM_VAL; }
#endif

    ~SmallFunctor() { memset(this, 0xbb, sizeof(*this)); }

#define OP_PAREN(n)                                                           \
    int operator()(const IntWrapper& iw, BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) \
    {                                                                         \
        return d_value += iw.value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);      \
    }

    // Invocation operator with 0 to 10 arguments.  The first argument (if any)
    // is a const reference to 'IntWrapper', although it can be passed an
    // 'int'.  The remaining arguments are of type 'int'.
    int operator()() { return d_value; }
    int operator()(const IntWrapper& iw) { return d_value += iw.value(); }
    BSLS_MACROREPEAT(9, OP_PAREN)

#undef OP_PAREN

    // Invocation operator that sets the functor's value and returns void.
    // To ensure unambiguous overloading resolution, the argument is passed as
    // a null-terminated string, not as an integer.
    void operator()(const char* s) { d_value = atoi(s); }

    int value() const { return d_value; }
};

inline
bool operator==(const SmallFunctor& a, const SmallFunctor& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const SmallFunctor& a, const SmallFunctor& b)
{
    return a.value() != b.value();
}

class MediumFunctor : public SmallFunctor
{
    // Functor that barely fits into the small object buffer.
    // - No allocator
    // - Bitwise movable
    // - Throwing move construtor

    char d_padding[sizeof(SmallObjectBuffer) - sizeof(SmallFunctor)];

  public:
    // BITWISE MOVEABLE
    BSLMF_NESTED_TRAIT_DECLARATION(MediumFunctor, bslmf::IsBitwiseMoveable);

    explicit MediumFunctor(int v) : SmallFunctor(v)
        { memset(d_padding, 0xee, sizeof(d_padding)); }

    MediumFunctor(const MediumFunctor& other) : SmallFunctor(other)
        { memset(d_padding, 0xee, sizeof(d_padding)); }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // This move constructor is not called in destructive move settings.
    MediumFunctor(MediumFunctor&& other)
      : SmallFunctor(bslmf::MovableRefUtil::move(other))
    {
        memset(d_padding, 0xee, sizeof(d_padding));
    }
#endif

    ~MediumFunctor() { memset(this, 0xbb, sizeof(*this)); }
};

inline
bool operator==(const MediumFunctor& a, const MediumFunctor& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const MediumFunctor& a, const MediumFunctor& b)
{
    return a.value() != b.value();
}

class LargeFunctor : public SmallFunctor
{
    // Functor that is barely too large to fit into the small object buffer.
    // - No allocator
    // - Bitwise movable
    // - Throwing move construtor

    char d_padding[sizeof(SmallObjectBuffer) - sizeof(SmallFunctor) + 1];

  public:
    // BITWISE MOVEABLE
    BSLMF_NESTED_TRAIT_DECLARATION(LargeFunctor, bslmf::IsBitwiseMoveable);

    explicit LargeFunctor(int v) : SmallFunctor(v)
        { memset(d_padding, 0xee, sizeof(d_padding)); }

    LargeFunctor(const LargeFunctor& other) : SmallFunctor(other)
        { memset(d_padding, 0xee, sizeof(d_padding)); }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // This move constructor is not called in destructive move settings.
    LargeFunctor(LargeFunctor&& other)
      : SmallFunctor(bslmf::MovableRefUtil::move(other))
    {
        memset(d_padding, 0xee, sizeof(d_padding));
    }
#endif

    ~LargeFunctor() { memset(this, 0xbb, sizeof(*this)); }
};

inline
bool operator==(const LargeFunctor& a, const LargeFunctor& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const LargeFunctor& a, const LargeFunctor& b)
{
    return a.value() != b.value();
}

class NTSmallFunctor
{
    // A small functor that is not bitwise movable, but does have a nothrow
    // move constructor
    // - No allocator
    // - Not bitwise movable (except in C++-03 mode)
    // - Nothrow move constructible

    enum { k_MOVED_FROM_VAL = 0x100000 };

    // Value that is XOR'ed with 'this' address.  If an object of this type
    // subjected to a bitwise move, the encoded value will be wrong and the
    // test driver will detect the error.
    int d_encodedValue;

    int xorWithSelf(int v) const {
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
        union aliaser { const void *d_ptr; int d_int; };
        aliaser a;
        a.d_ptr = this;
        return v ^ a.d_int;
#else
        // Don't encode if 'noexcept' is not supported because this class will
        // be bitwise movable in that case.
        return v;
#endif
    }

  public:
    explicit NTSmallFunctor(int v) : d_encodedValue(xorWithSelf(v)) { }

    NTSmallFunctor(const NTSmallFunctor& other)
        : d_encodedValue(xorWithSelf(other.value())) { --copyLimit; }

#if defined BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT && \
    defined BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Nothrow move and copy constructible
    NTSmallFunctor(NTSmallFunctor&& other) noexcept
      : d_encodedValue(xorWithSelf(other.value())) {
        other.d_encodedValue = other.xorWithSelf(k_MOVED_FROM_VAL);
    }
#else
    // Bitwise moveable -- use if 'noexcept' is not supported.
    BSLMF_NESTED_TRAIT_DECLARATION(NTSmallFunctor,
                                   bslmf::IsBitwiseMoveable);
#endif

    ~NTSmallFunctor() {
        memset(this, 0xbb, sizeof(*this));
    }

    int operator()(const IntWrapper& iw, int v) {
        d_encodedValue = xorWithSelf(value() + iw.value() + v);
        return value();
    }

    int value() const { return xorWithSelf(d_encodedValue); }
};

inline
bool operator==(const NTSmallFunctor& a, const NTSmallFunctor& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const NTSmallFunctor& a, const NTSmallFunctor& b)
{
    return a.value() != b.value();
}

class ThrowingEmptyFunctor : public EmptyFunctor
{
    // An empty functor whose move constructor might throw.
    // - No allocator
    // - Not bitwise movable
    // - Throwing move constructor

  public:
    explicit ThrowingEmptyFunctor(int v = 0) : EmptyFunctor(v) { }

    // Throwing copy constructor.
    ThrowingEmptyFunctor(const ThrowingEmptyFunctor& other)
        : EmptyFunctor(other) { }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Throwing move constructor.  Note that the constructor for the
    // 'EmptyFunctor' base does not increment 'moveLimit'.
    ThrowingEmptyFunctor(ThrowingEmptyFunctor&& other)
        : EmptyFunctor((--moveLimit, bslmf::MovableRefUtil::move(other))) { }
#endif

    ~ThrowingEmptyFunctor() { memset(this, 0xbb, sizeof(*this)); }
};

inline
bool operator==(const ThrowingEmptyFunctor&, const ThrowingEmptyFunctor&)
{
    return true;
}

inline
bool operator!=(const ThrowingEmptyFunctor&, const ThrowingEmptyFunctor&)
{
    return false;
}

namespace BloombergLP {
namespace bslmf {

template <>
struct IsBitwiseMoveable<ThrowingEmptyFunctor> : false_type
{
};

} // close namespace bslmf
} // close namespace BloombergLP

class ThrowingSmallFunctor : public FunctorBase
{
    // A small functor that is not bitwise movable, and whose move constructor
    // may throw.
    // - No allocator
    // - Not bitwise movable
    // - Throwing move constructor

    enum { k_MOVED_FROM_VAL = 0x100000 };

    // Value that is XOR'ed with 'this' address.  If an object of this type
    // subjected to a bitwise move, the encoded value will be wrong and the
    // test driver will detect the error.
    int d_encodedValue;

    int xorWithSelf(int v) const {
        union aliaser { const void *d_ptr; int d_int; };
        aliaser a;
        a.d_ptr = this;
        return v ^ a.d_int;
    }

  public:
    explicit ThrowingSmallFunctor(int v) : d_encodedValue(xorWithSelf(v)) { }

    // Throwing copy constructor
    ThrowingSmallFunctor(const ThrowingSmallFunctor& other)
      : FunctorBase(), d_encodedValue(xorWithSelf(other.value()))
        { --copyLimit; }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Throwing move constructor
    ThrowingSmallFunctor(ThrowingSmallFunctor&& other)
      : FunctorBase(), d_encodedValue(xorWithSelf(other.value()))
        { --moveLimit; }
#endif

    ~ThrowingSmallFunctor() {
        memset(this, 0xbb, sizeof(*this));
    }

    int operator()(const IntWrapper& iw, int v) {
        d_encodedValue = xorWithSelf(value() + iw.value() + v);
        return value();
    }

    int value() const { return xorWithSelf(d_encodedValue); }
};

inline
bool operator==(const ThrowingSmallFunctor& a, const ThrowingSmallFunctor& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const ThrowingSmallFunctor& a, const ThrowingSmallFunctor& b)
{
    return a.value() != b.value();
}

class SmallFunctorWithAlloc : public SmallFunctor
{
    // Small functor with allocator.
    // - Uses allocator
    // - Bitwise moveable
    // - Throwing move constructor

    bslma::Allocator *d_alloc_p;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(SmallFunctorWithAlloc,
                                   bslma::UsesBslmaAllocator);

    // BITWISE MOVEABLE
    BSLMF_NESTED_TRAIT_DECLARATION(SmallFunctorWithAlloc,
                                   bslmf::IsBitwiseMoveable);

    enum { IS_STATELESS = false };

    SmallFunctorWithAlloc(int v, bslma::Allocator *alloc = 0)
        : SmallFunctor(v), d_alloc_p(alloc) { }

    SmallFunctorWithAlloc(const SmallFunctorWithAlloc&  other,
                          bslma::Allocator             *alloc = 0)
        : SmallFunctor(other), d_alloc_p(bslma::Default::allocator(alloc)) { }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Move constructor propagates allocator
    SmallFunctorWithAlloc(SmallFunctorWithAlloc&& other)
        : SmallFunctor(bslmf::MovableRefUtil::move(other))
        , d_alloc_p(other.d_alloc_p) { }
    SmallFunctorWithAlloc(SmallFunctorWithAlloc&&  other,
                          bslma::Allocator        *alloc)
        : SmallFunctor(bslmf::MovableRefUtil::move(other))
        , d_alloc_p(alloc) { }
#endif

    ~SmallFunctorWithAlloc()
        { memset(this, 0xbb, sizeof(*this)); }

    bslma::Allocator *allocator() const { return d_alloc_p; }
};

inline
bool operator==(const SmallFunctorWithAlloc& a, const SmallFunctorWithAlloc& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const SmallFunctorWithAlloc& a, const SmallFunctorWithAlloc& b)
{
    return a.value() != b.value();
}

class NTSmallFunctorWithAlloc : public NTSmallFunctor
{
    // A small functor with allocator that is not bitwise movable, but does
    // have a nothrow move constructor.  It is unusual to have an object like
    // this, which uses an allocator but cannot throw on move, but we must
    // test it, anyway.
    // - Uses allocator
    // - Not bitwise moveable
    // - Nothrow move constructible

    bslma::Allocator *d_alloc_p;

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(NTSmallFunctorWithAlloc,
                                   bslma::UsesBslmaAllocator);

    explicit NTSmallFunctorWithAlloc(int v, bslma::Allocator *alloc = 0)
        : NTSmallFunctor(v), d_alloc_p(alloc) { }

    NTSmallFunctorWithAlloc(const NTSmallFunctorWithAlloc&  other,
                            bslma::Allocator               *alloc = 0)
        : NTSmallFunctor(other), d_alloc_p(alloc) { }

#if defined BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT && \
    defined BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Nothrow move and copy constructible
    NTSmallFunctorWithAlloc(NTSmallFunctorWithAlloc&& other) noexcept
      : NTSmallFunctor(bslmf::MovableRefUtil::move(other))
      , d_alloc_p(other.d_alloc_p)
    {
    }
    NTSmallFunctorWithAlloc(NTSmallFunctorWithAlloc&&  other,
                            bslma::Allocator          *alloc)
      : NTSmallFunctor(bslmf::MovableRefUtil::move(other))
      , d_alloc_p(alloc) { if (alloc != d_alloc_p) --moveLimit; }
#else
    // Bitwise moveable -- use if 'noexcept' is not supported.
    BSLMF_NESTED_TRAIT_DECLARATION(NTSmallFunctorWithAlloc,
                                   bslmf::IsBitwiseMoveable);
#endif

    ~NTSmallFunctorWithAlloc() { }

    bslma::Allocator *allocator() const { return d_alloc_p; }
};

inline
bool operator==(const NTSmallFunctorWithAlloc& a,
                const NTSmallFunctorWithAlloc& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const NTSmallFunctorWithAlloc& a,
                const NTSmallFunctorWithAlloc& b)
{
    return a.value() != b.value();
}

class LargeFunctorWithAlloc : public SmallFunctorWithAlloc
{
    // Functor with allocator that is barely too large to fit into the small
    // object buffer.
    // - Uses allocator
    // - Bitwise movable
    // - Throwing move constructor

    char d_padding[sizeof(SmallObjectBuffer)-sizeof(SmallFunctorWithAlloc)+1];

  public:
    // BITWISE MOVEABLE
    BSLMF_NESTED_TRAIT_DECLARATION(LargeFunctorWithAlloc,
                                   bslmf::IsBitwiseMoveable);

    BSLMF_NESTED_TRAIT_DECLARATION(LargeFunctorWithAlloc,
                                   bslma::UsesBslmaAllocator);

    LargeFunctorWithAlloc(int v, bslma::Allocator *alloc = 0)
        : SmallFunctorWithAlloc(v, alloc)
        { memset(d_padding, 0xee, sizeof(d_padding)); }

    LargeFunctorWithAlloc(const LargeFunctorWithAlloc&  other,
                          bslma::Allocator             *alloc = 0)
      : SmallFunctorWithAlloc(other, alloc)
        { memset(d_padding, 0xee, sizeof(d_padding)); }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Move constructor propagates allocator
    LargeFunctorWithAlloc(LargeFunctorWithAlloc&& other)
      : SmallFunctorWithAlloc(bslmf::MovableRefUtil::move(other))
        { memset(d_padding, 0xee, sizeof(d_padding)); }
    LargeFunctorWithAlloc(LargeFunctorWithAlloc&&  other,
                          bslma::Allocator        *alloc)
      : SmallFunctorWithAlloc(bslmf::MovableRefUtil::move(other), alloc)
        { memset(d_padding, 0xee, sizeof(d_padding)); }
#endif

    ~LargeFunctorWithAlloc()
        { memset(this, 0xbb, sizeof(*this)); }
};

inline
bool operator==(const LargeFunctorWithAlloc& a, const LargeFunctorWithAlloc& b)
{
    return a.value() == b.value();
}

inline
bool operator!=(const LargeFunctorWithAlloc& a, const LargeFunctorWithAlloc& b)
{
    return a.value() != b.value();
}

// Common function type used in most tests
typedef bsl::function<int(const IntWrapper&, int)> Obj;

// Using the curiously-recurring template pattern with a template template
// parameter, we can implement the boiler-plate part of an STL-style
// allocator.
template <class TYPE, template <class T> class ALLOC>
class STLAllocatorBase
{
    // Define the boilerplate for the specified 'ALLOC' allocator.

  public:
    typedef TYPE        value_type;
    typedef TYPE       *pointer;
    typedef const TYPE *const_pointer;
    typedef unsigned    size_type;
    typedef int         difference_type;

    template <class U>
    struct rebind {
        typedef ALLOC<U> other;
    };

    static size_type max_size() { return UINT_MAX / sizeof(TYPE); }

    void construct(pointer p, const TYPE& value)
        { new((void *)p) TYPE(value); }

    void destroy(pointer p) { p->~TYPE(); }
};

// Test allocator used by all instances of 'EmptySTLAllocator'.  This value is
// changed globally whenever an 'EmptySTLAllocator' is constructed from a
// 'bslma::TestAllocator*'.
bslma::TestAllocator *emptySTLAllocSource = NULL;

template <class TYPE>
class EmptySTLAllocator : public STLAllocatorBase<TYPE, EmptySTLAllocator>
{

  public:
    explicit EmptySTLAllocator(bslma::TestAllocator *ta)
        { emptySTLAllocSource = ta; }

    template <class U>
    EmptySTLAllocator(const EmptySTLAllocator<U>&) { }

    TYPE *allocate(size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) emptySTLAllocSource->allocate(n * sizeof(TYPE));
    }

    void deallocate(TYPE *p, size_t) {
        emptySTLAllocSource->deallocate(p);
    }
};

template <class TYPE1, class TYPE2>
inline
bool operator==(const EmptySTLAllocator<TYPE1>&,
                const EmptySTLAllocator<TYPE2>&)
{
    return true;
}

template <class TYPE1, class TYPE2>
inline
bool operator!=(const EmptySTLAllocator<TYPE1>&,
                const EmptySTLAllocator<TYPE2>&)
{
    return false;
}

// Test allocator used by all instances of 'EmptySTLAllocator2'.  This value
// is changed globally whenever an 'EmptySTLAllocator2' is constructed from a
// 'bslma::TestAllocator*'.
bslma::TestAllocator *emptySTLAlloc2Source = NULL;

template <class TYPE>
class EmptySTLAllocator2 : public STLAllocatorBase<TYPE, EmptySTLAllocator2>
{
    // 'EmptySTLAllocator' but uses a different 'emptySTLAllocSource' and so
    // avoid conflicts.

  public:
    explicit EmptySTLAllocator2(bslma::TestAllocator *ta)
        { emptySTLAlloc2Source = ta; }

    template <class U>
    EmptySTLAllocator2(const EmptySTLAllocator2<U>&) { }

    TYPE *allocate(size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) emptySTLAlloc2Source->allocate(n * sizeof(TYPE));
    }

    void deallocate(TYPE *p, size_t) {
        emptySTLAlloc2Source->deallocate(p);
    }
};

template <class TYPE1, class TYPE2>
inline
bool operator==(const EmptySTLAllocator2<TYPE1>&,
                const EmptySTLAllocator2<TYPE2>&)
{
    return true;
}

template <class TYPE1, class TYPE2>
inline
bool operator!=(const EmptySTLAllocator2<TYPE1>&,
                const EmptySTLAllocator2<TYPE2>&)
{
    return false;
}

template <class TYPE>
class StatefulAllocatorBase
{
    bslma::TestAllocator *d_mechanism;

  public:
    explicit StatefulAllocatorBase(bslma::TestAllocator *mechanism)
        : d_mechanism(mechanism) { }

    TYPE *allocate(size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) d_mechanism->allocate(n * sizeof(TYPE));
    }

    void deallocate(TYPE *p, size_t) {
        d_mechanism->deallocate(p);
    }

    bslma::TestAllocator *mechanism() const { return d_mechanism; }
};

template <class TYPE1, class TYPE2>
inline
bool operator==(const StatefulAllocatorBase<TYPE1>& a,
                const StatefulAllocatorBase<TYPE2>& b)
{
    return a.mechanism() == b.mechanism();
}

template <class TYPE1, class TYPE2>
inline
bool operator!=(const StatefulAllocatorBase<TYPE1>& a,
                const StatefulAllocatorBase<TYPE2>& b)
{
    return a.mechanism() != b.mechanism();
}

template <class TYPE>
class StatefulAllocator :
    public StatefulAllocatorBase<TYPE>,
    public STLAllocatorBase<TYPE, StatefulAllocator>
{
    // A small, stateful, STL allocator type.

  public:
    explicit StatefulAllocator(bslma::TestAllocator *mechanism)
        : StatefulAllocatorBase<TYPE>(mechanism) { }

    template <class U>
    StatefulAllocator(const StatefulAllocator<U>& other)
        : StatefulAllocatorBase<TYPE>(other.mechanism()) { }
};

template <class TYPE>
class StatefulAllocator2 :
    public StatefulAllocatorBase<TYPE>,
    public STLAllocatorBase<TYPE, StatefulAllocator2>
{
    // Another stateful STL allocator type. Just for completeness, this
    // allocator is too large to fit in the small object buffer, although no
    // type-erased allocator should ever be put into the small object buffer.

    typedef
      typename bslma::AllocatorAdaptor<StatefulAllocator<TYPE> >::Type Adaptor;

    char d_padding[sizeof(SmallObjectBuffer)];

  public:
    explicit StatefulAllocator2(bslma::TestAllocator *mechanism)
        : StatefulAllocatorBase<TYPE>(mechanism) { }

    template <class U>
    StatefulAllocator2(const StatefulAllocator2<U>& other)
        : StatefulAllocatorBase<TYPE>(other.mechanism()) { }
};

inline bool isConstPtr(void *) { return false; }
inline bool isConstPtr(const void *) { return true; }

template <class T>
inline bool isNullPtrImp(const T& p, bsl::true_type /* is pointer */) {
    return 0 == p;
}

template <class T>
inline bool isNullPtrImp(const T&, bsl::false_type /* is pointer */) {
    return false;
}

template <class T>
inline bool isNullPtr(const T& p) {
    static const bool IS_POINTER =
        bsl::is_pointer<T>::value ||
        bslmf::IsFunctionPointer<T>::value ||
        bslmf::IsMemberFunctionPointer<T>::value;

    return isNullPtrImp(p, bsl::integral_constant<bool, IS_POINTER>());
}

inline bool isNullPtr(const bsl::nullptr_t&) {
    return true;
}

template <class T>
class ValueGeneratorBase {
    // Generates values ot type 'T' for test driver
    typedef typename bsl::remove_const<T>::type MutableT;
    MutableT d_value;

  public:
    enum { INIT_VALUE = 0x2001 };

    ValueGeneratorBase() : d_value(INIT_VALUE) { }

    T& reset() { return (d_value = MutableT(INIT_VALUE)); }
    int value() const { return d_value.value(); }
};

template <class T>
struct ValueGenerator : ValueGeneratorBase<T> {
    // Generate and check values for values of type 'T'.  This primary
    // template is used when 'T' is an rvalue (i.e., not a reference or
    // pointer type.)

    // Since rvalue is passed by value, it is not modified by function calls.
    // The expected value is therefore ignored when checking the value.
    T obj() { return this->reset(); }
    bool check(int /* exp */) const
        { return this->value() == ValueGeneratorBase<T>::INIT_VALUE; }
};

template <class T>
struct ValueGenerator<T&> : ValueGeneratorBase<T> {
    // Specialization for lvalues of type 'T&'
    T& obj() { return this->reset(); }
    bool check(int exp) const { return this->value() == exp; }
};

template <class T>
struct ValueGenerator<T*> : ValueGeneratorBase<T> {
    // Specialization for pointers to 'T'
    T* obj() { return &this->reset(); }
    bool check(int exp) const { return this->value() == exp; }
};

template <class T>
struct ValueGenerator<SmartPtr<T> > : ValueGeneratorBase<T> {
    // Specialization for smart pointers to 'T'
    SmartPtr<T> obj() { return SmartPtr<T>(&this->reset()); }
    bool check(int exp) const { return this->value() == exp; }
};

// Special marker for moved-from comparisons
bsls::ObjectBuffer<Obj> movedFromMarkerBuf;
const Obj& movedFromMarker = movedFromMarkerBuf.object();

//=============================================================================
//                  TEST FUNCTIONS
//-----------------------------------------------------------------------------

template <class T, class RET, class ARG>
void testPtrToMemFunc(const char *prototypeStr)
    // Test invocation of pointer to member function wrapper.
    // Tests using non-const member functions 'IntWrapper::increment[0-9]'
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    const ARG a1(0x0002);
    const ARG a2(0x0004);
    const ARG a3(0x0008);
    const ARG a4(0x0010);
    const ARG a5(0x0020);
    const ARG a6(0x0040);
    const ARG a7(0x0080);
    const ARG a8(0x0100);
    const ARG a9(0x0200);

    ValueGenerator<T> gen;

    bsl::function<RET(T)> f1(&IntWrapper::increment0);
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));

    bsl::function<RET(T, ARG)> f2(&IntWrapper::increment1);
    ASSERT(0x2003 == f2(gen.obj(), a1));
    ASSERT(gen.check(0x2003));

    bsl::function<RET(T, ARG, ARG)> f3(&IntWrapper::increment2);
    ASSERT(0x2007 == f3(gen.obj(), a1, a2));
    ASSERT(gen.check(0x2007));

    bsl::function<RET(T, ARG, ARG, ARG)> f4(&IntWrapper::increment3);
    ASSERT(0x200f == f4(gen.obj(), a1, a2, a3));
    ASSERT(gen.check(0x200f));

    bsl::function<RET(T, ARG, ARG, ARG, ARG)> f5(&IntWrapper::increment4);
    ASSERT(0x201f == f5(gen.obj(), a1, a2, a3, a4));
    ASSERT(gen.check(0x201f));

    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG)> f6(&IntWrapper::increment5);
    ASSERT(0x203f == f6(gen.obj(), a1, a2, a3, a4, a5));
    ASSERT(gen.check(0x203f));

    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f7(&IntWrapper::increment6);
    ASSERT(0x207f == f7(gen.obj(), a1, a2, a3, a4, a5, a6));
    ASSERT(gen.check(0x207f));

    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f8(&IntWrapper::increment7);
    ASSERT(0x20ff == f8(gen.obj(), a1, a2, a3, a4, a5, a6, a7));
    ASSERT(gen.check(0x20ff));

    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG)> f9(&IntWrapper::increment8);
    ASSERT(0x21ff == f9(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8));
    ASSERT(gen.check(0x21ff));

    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> f10(&IntWrapper::increment9);
    ASSERT(0x23ff == f10(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(gen.check(0x23ff));

    // Test using Function_NothrowWrapper
    bsl::function<RET(T, ARG, ARG)> ntf3(ntWrap(&IntWrapper::increment2));
    ASSERT(0x2007 == ntf3(gen.obj(), a1, a2));
    ASSERT(gen.check(0x2007));
}

template <class T, class RET, class ARG>
void testPtrToConstMemFunc(const char *prototypeStr)
    // Test invocation of pointer to const member function wrapper.  Tests
    // using const member functions 'IntWrapper::add[0-9]'.  To save compile
    // time, since 'testPtrToMemFunc' already tests every possible
    // argument-list length, we test only a small number of possible
    // argument-list lengths (specifically 0, 1, and 9 arguments) here.
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    const ARG a1(0x0002);
    const ARG a2(0x0004);
    const ARG a3(0x0008);
    const ARG a4(0x0010);
    const ARG a5(0x0020);
    const ARG a6(0x0040);
    const ARG a7(0x0080);
    const ARG a8(0x0100);
    const ARG a9(0x0200);

    ValueGenerator<T> gen;

    bsl::function<RET(T)> f1(&IntWrapper::add0);
    ASSERT(0x2001 == f1(gen.obj()));
    ASSERT(gen.check(0x2001));

    bsl::function<RET(T, ARG)> f2(&IntWrapper::add1);
    ASSERT(0x2003 == f2(gen.obj(), a1));
    ASSERT(gen.check(0x2001));

    // No need to test 3 through 9 arguments.  That mechanism has already been
    // tested via the 'testPtrToMemFunc' function.

    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> f10(&IntWrapper::add9);
    ASSERT(0x23ff == f10(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(gen.check(0x2001));

    // Test using Function_NothrowWrapper
    bsl::function<RET(T, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> ntf10(&IntWrapper::add9);
    ASSERT(0x23ff == ntf10(gen.obj(), a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(gen.check(0x2001));
}

template <class FUNCTOR, class OBJ>
bool checkValue(const OBJ& obj, int exp)
    // Return true if the functor wrapped within the specified 'obj' has a
    // value that matches the specified 'exp' value and false otherwise.
{
    if (FUNCTOR::IS_STATELESS)
        return true;  // No state to check                            // RETURN

    const FUNCTOR& target = *obj.template target<FUNCTOR>();
    return exp == target.value();
}

template <class FUNCTOR, class RET, class ARG>
void testFunctor(const char *prototypeStr)
    // Test invocation of a 'bsl::function' wrapping a functor object.
{
    if (veryVeryVerbose) printf("\t%s\n", prototypeStr);

    const bool isStateless = FUNCTOR::IS_STATELESS;
    const int  initState = isStateless ? 0 : 0x3000;

    FUNCTOR ftor(initState);

    const ARG a0(0x0001);
    const ARG a1(0x0002);
    const ARG a2(0x0004);
    const ARG a3(0x0008);
    const ARG a4(0x0010);
    const ARG a5(0x0020);
    const ARG a6(0x0040);
    const ARG a7(0x0080);
    const ARG a8(0x0100);
    const ARG a9(0x0200);

    bsl::function<RET()> f0(ftor);
    ASSERT(initState + 0x0000 == f0());
    ASSERT(checkValue<FUNCTOR>(f0, initState + 0x0000));

    bsl::function<RET(ARG)> f1(ftor);
    ASSERT(initState + 0x0001 == f1(a0));
    ASSERT(checkValue<FUNCTOR>(f1, initState + 0x0001));

    bsl::function<RET(ARG, ARG)> f2(ftor);
    ASSERT(initState + 0x0003 == f2(a0, a1));
    ASSERT(checkValue<FUNCTOR>(f2, initState + 0x0003));

    bsl::function<RET(ARG, ARG, ARG)> f3(ftor);
    ASSERT(initState + 0x0007 == f3(a0, a1, a2));
    ASSERT(checkValue<FUNCTOR>(f3, initState + 0x0007));

    bsl::function<RET(ARG, ARG, ARG, ARG)> f4(ftor);
    ASSERT(initState + 0x000f == f4(a0, a1, a2, a3));
    ASSERT(checkValue<FUNCTOR>(f4, initState + 0x000f));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG)> f5(ftor);
    ASSERT(initState + 0x001f == f5(a0, a1, a2, a3, a4));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG)> f6(ftor);
    ASSERT(initState + 0x003f == f6(a0, a1, a2, a3, a4, a5));
    ASSERT(checkValue<FUNCTOR>(f6, initState + 0x003f));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f7(ftor);
    ASSERT(initState + 0x007f == f7(a0, a1, a2, a3, a4, a5, a6));
    ASSERT(checkValue<FUNCTOR>(f7, initState + 0x007f));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG)> f8(ftor);
    ASSERT(initState + 0x00ff == f8(a0, a1, a2, a3, a4, a5, a6, a7));
    ASSERT(checkValue<FUNCTOR>(f8, initState + 0x00ff));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG)> f9(ftor);
    ASSERT(initState + 0x01ff == f9(a0, a1, a2, a3, a4, a5, a6, a7, a8));
    ASSERT(checkValue<FUNCTOR>(f9, initState + 0x01ff));

    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG, ARG)> f10(ftor);
    ASSERT(initState + 0x03ff == f10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(checkValue<FUNCTOR>(f10, initState + 0x03ff));

    // Test invocation of const 'bsl::function' & verify side-effects
    const bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                            ARG, ARG, ARG)> cf10(ftor);
    ASSERT(initState + 0x03ff == cf10(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9));
    ASSERT(checkValue<FUNCTOR>(cf10, initState + 0x03ff));

    // Test 'Function_NothrowWrapper'
    bsl::function<RET(ARG, ARG, ARG, ARG, ARG, ARG, ARG,
                      ARG, ARG)> ntf9(ntWrap(ftor));
    ASSERT(initState + 0x01ff == ntf9(a0, a1, a2, a3, a4, a5, a6, a7, a8));
    ASSERT(checkValue<FUNCTOR>(ntf9, initState + 0x01ff));

    // None of the above invocations should have changed the original of
    // 'ftor'
    ASSERT(initState == ftor.value());

    // Discard return value of functor that returns non-void
    bsl::function<void(ARG)> fvi(ftor);
    fvi(a0);
    ASSERT(checkValue<FUNCTOR>(fvi, initState + 0x0001));

    // Invoke a functor with a void return value
    bsl::function<void(const char *)> fvr(ftor);
    fvr("1234");
    ASSERT(checkValue<FUNCTOR>(fvr, 1234)); // Wrapped copy is modified
    ASSERT(ftor.value() == initState);      // Original is unchanged
}

template <class ALLOC>
struct CheckAlloc
{
    typedef typename bslma::AllocatorAdaptor<ALLOC>::Type Adaptor;
    static const size_t k_SIZE = (bsl::is_empty<ALLOC>::value ?
                                  0 : sizeof(Adaptor));
    static const size_t k_MAX_OVERHEAD =
        2 * sizeof(bsls::AlignmentUtil::MaxAlignedType);

    template <class BSLMA_ALLOC>
    static bool areEqualAlloc(const ALLOC& a, BSLMA_ALLOC *const &b)
        // Return true if the specified allocator 'b' is a pointer to an
        // adaptor that wraps a copy of the specified allocator 'a' and false
        // otherwise.
    {
        Adaptor *adaptor = dynamic_cast<Adaptor*>(b);
        return adaptor != NULL && a == adaptor->adaptedAllocator();
    }

    template <class ALLOC2>
    static bool areEqualAlloc(const ALLOC& /* a */, const ALLOC2& /* b */)
        // Return false, indicating that the specified allocator 'b' is of a
        // different STL allocator type than the specified allocator 'a'.
    {
        return false;
    }

    static bool areEqualAlloc(const ALLOC& a, const ALLOC& b)
        // Return true if the specified allocator 'b' is an STL allocator of
        // the same type as the specified allocator 'a' and compares equal to
        // 'a' and false otherwise.
    {
        return a == b;
    }
};

template <class T>
struct CheckAlloc<bsl::allocator<T> >
{
    static const size_t k_SIZE = 0;
    static const size_t k_MAX_OVERHEAD = 0;

    template <class BSLMA_ALLOC>
    static bool areEqualAlloc(const bsl::allocator<T>& a,BSLMA_ALLOC *const &b)
        // Return true if the mechanism pointer for the specified allocator
        // 'b' is equal to the specified 'bslma::Allocator' pointer 'a' and
        // false otherwise.
    {
        return a.mechanism() == b;
    }

    template <class ALLOC2>
    static bool areEqualAlloc(const bsl::allocator<T>& a, const ALLOC2& b)
    {
        return CheckAlloc<ALLOC2>::areEqualAlloc(b, a.mechanism());
    }
};

template <class BA>
struct CheckAlloc<BA*>
{
    static const size_t k_SIZE = 0;
    static const size_t k_MAX_OVERHEAD = 0;

    template <class BSLMA_ALLOC>
    static bool areEqualAlloc(bslma::Allocator* a, BSLMA_ALLOC *const &b)
        // Return true if the specified 'bslma::allocator' pointer 'b' is
        // equal to the specified 'bslma::Allocator' pointer 'a' and false
        // otherwise.
    {
        return a == b;
    }

    template <class ALLOC2>
    static bool areEqualAlloc(bslma::Allocator* a, const ALLOC2& b)
    {
        return CheckAlloc<ALLOC2>::areEqualAlloc(b, a);
    }
};

template <class ALLOC1, class ALLOC2>
bool areEqualAlloc(const ALLOC1& a, const ALLOC2& b)
    // Return true if the specified allocator 'a' has a type compatible with
    // the specified allocator 'b' and the two allocators, after simplifying,
    // compare equal.
{
    return CheckAlloc<ALLOC1>::areEqualAlloc(a, b);
}

template <class ALLOC1, class TP>
bool areEqualAlloc(const ALLOC1& a, class bsl::allocator<TP>& b)
    // Return true if the specified allocator 'a' has compatible a type with
    // the specified allocator 'b' and the two allocators, after simplifying,
    // compare equal.
{
    return CheckAlloc<ALLOC1>(a).areEqualAlloc(b.mechanism());
}

template <class FUNC>
inline
bool allocPropagationCheckImp(const Obj& f, bsl::true_type /*usesBslmaAlloc*/)
    // Return true (good) if the allocator for the specified 'f' function
    // object is stored as the allocator in the wrapped functor of type 'FUNC'
    // within 'f'; otherwise return false (failed to propagate allocator
    // correctly).  The behavior is undefined unless 'f' wraps an
    // allocator-aware object of type 'FUNC' which supplies an 'allocator()'
    // member that returns a stored allocator pointer.  This overload is used
    // when 'bslma::UsesBslmaAllocator<FUNC>' derives from 'true_type'.
{
    const FUNC *target = f.target<FUNC>();
    return f.allocator() == target->allocator();
}

template <class FUNC>
inline
bool allocPropagationCheckImp(const Obj&, bsl::false_type /*usesBslmaAlloc*/)
    // Return true (good).  This overload is used when
    // 'bslma::UsesBslmaAllocator<FUNC>' derives from 'false_type', meaning
    // that 'FUNC' does not use an allocator.
{
    return true;
}

template <class FUNC>
inline
bool allocPropagationCheck(const Obj& f)
    // Return true if 'f.allocator()' for the specified 'f' functor returns
    // the equivalent of the specified 'alloc' or if 'f' does not use a
    // 'bslma::Allocator*' at all; otherwise return false.
{
    return allocPropagationCheckImp<FUNC>(f,
                                           bslma::UsesBslmaAllocator<FUNC>());
}

enum WhatIsInplace {
    e_INPLACE_BOTH,      // Both function and allocator are in place
    e_INPLACE_FUNC_ONLY, // Function is in place, allocator is out of place
    e_OUTOFPLACE_BOTH    // Both function and allocator are out of place
};

template <class ALLOC, class FUNC_ARG>
void testFuncWithAlloc(int line, FUNC_ARG func_arg, WhatIsInplace inplace)
{
    // Get the real functor (if it's wrapped).
    typedef typename NTUNWRAP_T(FUNC_ARG) FUNC;
    const FUNC& func = ntUnwrap(func_arg);

    const size_t inplaceFuncSize = (bsl::is_empty<FUNC>::value ? 0 :
                                    isNullPtr(func)            ? 0 :
                                    sizeof(FUNC));
    const size_t allocSize = CheckAlloc<ALLOC>::k_SIZE;
    const size_t maxOverhead = CheckAlloc<ALLOC>::k_MAX_OVERHEAD;

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

    AllocSizeType numBlocksUsed, minBytesUsed, maxBytesUsed;

    switch (inplace) {
      case e_INPLACE_BOTH: {
        LOOP_ASSERT(line, inplaceFuncSize <= sizeof(SmallObjectBuffer));
        LOOP_ASSERT(line, 0 == allocSize);
        numBlocksUsed = 0;
        minBytesUsed = 0;
        maxBytesUsed = 0;
      } break;

      case e_INPLACE_FUNC_ONLY: {
        LOOP_ASSERT(line, inplaceFuncSize <= sizeof(SmallObjectBuffer));
        LOOP_ASSERT(line, allocSize > 0);
        numBlocksUsed = 1;
        minBytesUsed = allocSize;
        maxBytesUsed = minBytesUsed + maxOverhead;
      } break;

      case e_OUTOFPLACE_BOTH: {
        numBlocksUsed = 1;
        minBytesUsed = sizeof(FUNC) + allocSize;
        maxBytesUsed = minBytesUsed + maxOverhead;
      } break;
    } // end switch

    bslma::TestAllocator ta;
    globalAllocMonitor.reset();
    FunctorMonitor funcMonitor(L_);
    EXCEPTION_TEST_BEGIN(&ta, &moveLimit) {
        EXCEPTION_TEST_TRY {
            funcMonitor.reset(L_);
            ALLOC alloc(&ta);
            Obj f(bsl::allocator_arg, alloc, func_arg);
            LOOP_ASSERT(line, isNullPtr(func) == !f);
            LOOP_ASSERT(line,
                        CheckAlloc<ALLOC>::areEqualAlloc(alloc,f.allocator()));
            if (f) {
                LOOP_ASSERT(line, typeid(func) == f.target_type());
                FUNC *target_p = f.target<FUNC>();
                LOOP_ASSERT(line, target_p);
                if (target_p) {
                    LOOP_ASSERT(line, func == *target_p);
                    LOOP_ASSERT(line, allocPropagationCheck<FUNC>(f));
                    LOOP_ASSERT(line, 0x4005 == f(IntWrapper(0x4000), 5));
                }
            }
            LOOP_ASSERT(line, numBlocksUsed == ta.numBlocksInUse());
            LOOP_ASSERT(line, minBytesUsed <= ta.numBytesInUse() &&
                        ta.numBytesInUse() <= maxBytesUsed);
            LOOP_ASSERT(line, globalAllocMonitor.isInUseSame());
        } EXCEPTION_TEST_CATCH {
            // Exception neutral: All memory has been released.
            LOOP_ASSERT(line, 0 == ta.numBlocksInUse());
            LOOP_ASSERT(line, globalAllocMonitor.isInUseSame());
            LOOP_ASSERT(line, funcMonitor.isSameCount());
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    LOOP_ASSERT(line, 0 == ta.numBlocksInUse());
    LOOP_ASSERT(line, globalAllocMonitor.isInUseSame());
}

template <class ALLOC, class FUNC_ARG>
void testCopyCtorWithAlloc(FUNC_ARG    func,
                           const Obj&  original,
                           const char *originalAllocName,
                           const char *copyAllocName)
{
    // Get the real functor type (in case it's wrapped).
    typedef typename NTUNWRAP_T(FUNC_ARG) FUNC;

    if (veryVeryVerbose)
        printf("\tAlloc: orig = %s, copy = %s\n", originalAllocName,
               copyAllocName);

    bool copyAllocNone = (0 == strcmp(copyAllocName, "none"));

    AllocSizeType numBlocksUsed, numBytesUsed;

    bslma::TestAllocator copyTa;
    ALLOC copyAlloc(&copyTa);

    // Re-create 'original' using 'copyAlloc' so that we can measure memory
    // usage.
    {
        Obj original2(bsl::allocator_arg, copyAlloc, func);
        numBlocksUsed = copyTa.numBlocksInUse();
        numBytesUsed = copyTa.numBytesInUse();
    }
    ASSERT(copyTa.numBlocksInUse() == 0);
    ASSERT(copyTa.numBytesInUse()  == 0);

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);
    FunctorMonitor funcMonitor(L_);

    EXCEPTION_TEST_BEGIN(&copyTa, &copyLimit) {
        EXCEPTION_TEST_TRY {
            // We want to select one of two constructors at run time, so
            // instead of declaring a 'function' object directly, we create a
            // buffer that can hold a 'function' object and construct the
            // 'function' later, using the desired constructor.
            union {
                char                                d_bytes[sizeof(Obj)];
                bsls::AlignmentUtil::MaxAlignedType d_align;
            } copyBuf;

            funcMonitor.reset(L_);
            if (copyAllocNone) {
                // copyAllocName is "none".  Choose normal copy constructor
                // with no allocator, but install 'copyTa' as the allocator
                // indirectly by setting the default allocator.
                bslma::DefaultAllocatorGuard guard(&copyTa);
                ::new(copyBuf.d_bytes) Obj(original);
            }
            else {
                // Use extended copy constructor.
                ::new(copyBuf.d_bytes) Obj(bsl::allocator_arg, copyAlloc,
                                           original);
            }

            // 'copyBuf' now holds the copy-constructed 'function'.
            Obj& copy = *reinterpret_cast<Obj*>(copyBuf.d_bytes);

            ASSERT(copy.target_type() == original.target_type());
            ASSERT(CheckAlloc<ALLOC>::areEqualAlloc(copyAlloc,
                                                    copy.allocator()));
            ASSERT(allocPropagationCheck<FUNC>(copy));
            ASSERT(! copy == ! original);

            if (copy) {

                // Check for faithful copy of functor.
                // Address of original and copy must be different.
                ASSERT(*copy.target<FUNC>() == *original.target<FUNC>());
                ASSERT(copy.target<FUNC>() != original.target<FUNC>());

                // Invoke
                ASSERT(copy(IntWrapper(0x4000), 9) ==
                       original(IntWrapper(0x4000), 9));

                // Invocation performed identical operations on original and on
                // copy.  Check that equality relationship was not disturbed.
                ASSERT(*copy.target<FUNC>() == *original.target<FUNC>());
            }

            ASSERT(copyTa.numBlocksInUse() == numBlocksUsed);
            ASSERT(copyTa.numBytesInUse()  == numBytesUsed);
            ASSERT(globalAllocMonitor.isInUseSame());

            copy.~Obj();
        }
        EXCEPTION_TEST_CATCH {
            // Exception neutral: All memory has been released.
            ASSERT(0 == copyTa.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());
            ASSERT(funcMonitor.isSameCount());
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    ASSERT(copyTa.numBlocksInUse() == 0);
    ASSERT(copyTa.numBytesInUse()  == 0);
    ASSERT(globalAllocMonitor.isInUseSame());
}

template <class ORIGINAL_ALLOC, class FUNC>
void testCopyCtor(FUNC func, const char *originalAllocName)
{
    // Construct the original 'function'
    bslma::TestAllocator originalTa;
    ORIGINAL_ALLOC originalAlloc(&originalTa);
    Obj original(bsl::allocator_arg, originalAlloc, func);

    // Snapshot allocator to ensure that nothing else is allocated from here.
    bslma::TestAllocatorMonitor originalAllocMonitor(&originalTa);

#define TEST(A)                                                               \
    testCopyCtorWithAlloc<A>(func, original, originalAllocName, #A)

    // Special case, if allocator for copy is named "none", then
    // 'testCopyCtorWithAlloc' uses the normal copy constructor instead of the
    // allocator-extended copy constructor.
    testCopyCtorWithAlloc<bslma::Allocator *>(func, original,
                                              originalAllocName, "none");

    // Test with different allocator types for copy
    TEST(bslma::TestAllocator *  );
    TEST(bsl::allocator<char>    );
    TEST(EmptySTLAllocator2<char>);
    TEST(StatefulAllocator<char> );
    TEST(StatefulAllocator2<char>);

    ASSERT(originalAllocMonitor.isInUseSame());

#undef TEST

}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class ALLOC, class FUNC_ARG>
void testMoveCtorWithSameAlloc(FUNC_ARG func, bool extended,
                               const char *allocName)
{
    typedef typename NTUNWRAP_T(FUNC_ARG) FUNC;

    if (veryVeryVerbose) {
        if (extended) {
            printf("\tsource and dest using same alloc: %s\n", allocName);
        }
        else {
            printf("\tdest copies source alloc: %s\n", allocName);
        }
    }

    bool isEmpty = isNullPtr(ntUnwrap(func));
    bool usesSmallObjectOptimization = false;

    // Construct a 'FUNC' object in a moved-from state.
    FUNC_ARG movedFromFunc(func);
    {
        FUNC_ARG movedToFunc(bslmf::MovableRefUtil::move(movedFromFunc));
        ASSERT(ntUnwrap(movedToFunc) == ntUnwrap(func));
    }

    bslma::TestAllocator ta;
    ALLOC alloc(&ta);

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

    // For source and destination using the same allocator, create a pair of
    // objects with the identical memory footprint of the post-move (possibly
    // empty) source and post-move destination and measure the total memory
    // usage.
    AllocSizeType destNumBlocksUsed, destNumBytesUsed;
    {
        Obj postMoveDest(bsl::allocator_arg, alloc, func);
        char *ctarget = reinterpret_cast<char*>(postMoveDest.target<FUNC>());
        if (reinterpret_cast<char*>(&postMoveDest) <= ctarget &&
            ctarget < reinterpret_cast<char*>(&postMoveDest + 1)) {
            // Wrapped functor falls within the footprint of the function
            // object.  Thus, the small object optimization is in use.  After
            // the move, the source 'function' wraps a 'FUNC' object in a
            // moved-from state.
            usesSmallObjectOptimization = true;
            Obj postMoveSource(bsl::allocator_arg, alloc, movedFromFunc);
            destNumBlocksUsed = ta.numBlocksInUse();
            destNumBytesUsed = ta.numBytesInUse();
        }
        else {
            // Functor is allocated out-of-place, so ownership of it moves
            // rather than being move-construted.  The result of the move is
            // that the source is empty and the destination hold the functor.
            Obj postMoveSource(bsl::allocator_arg, alloc);
            destNumBlocksUsed = ta.numBlocksInUse();
            destNumBytesUsed = ta.numBytesInUse();
        }
    }

    ASSERT(ta.numBlocksInUse() == 0);
    ASSERT(ta.numBytesInUse()  == 0);
    ASSERT(globalAllocMonitor.isInUseSame());

    globalAllocMonitor.reset();

    {
        // We want to select one of two constructors at run time, so instead
        // of declaring a 'function' object directly, we create a buffer that
        // can hold a 'function' object and construct the 'function' later
        // using the desired constructor.
        bsls::ObjectBuffer<Obj> destBuf;

        Obj source(bsl::allocator_arg, alloc, func);
        ASSERT(isEmpty == ! source);
        FUNC *sourceTarget = isEmpty ? NULL : source.target<FUNC>();

        if (extended) {
            // Use extended move constructor.
            ::new(&destBuf) Obj(bsl::allocator_arg,
                                alloc,
                                bslmf::MovableRefUtil::move(source));
        }
        else {
            // Use normal move constructor
            ::new(&destBuf) Obj(bslmf::MovableRefUtil::move(source));
        }

        // 'destBuf' now holds the move-constructed 'function'.
        Obj& dest = destBuf.object();

        ASSERT(! source);
        ASSERT(CheckAlloc<ALLOC>::areEqualAlloc(alloc, source.allocator()));
        ASSERT(CheckAlloc<ALLOC>::areEqualAlloc(alloc, dest.allocator()));
        ASSERT(allocPropagationCheck<FUNC>(dest));
        if (! usesSmallObjectOptimization) {
            // Function was moved by pointer so the address of the wrapped
            // function will not have changed.
            ASSERT(dest.target<FUNC>() == sourceTarget);
        }

        ASSERT(CheckAlloc<ALLOC>::areEqualAlloc(alloc, dest.allocator()));
        ASSERT(isEmpty == ! dest);

        if (dest) {

            // Check for faithful move of functor
            ASSERT(dest.target_type() == typeid(ntUnwrap(func)));
            ASSERT(*dest.target<FUNC>() == ntUnwrap(func));

            // Invoke
            Obj temp(func);
            ASSERT(dest(IntWrapper(0x4000), 7) ==
                   temp(IntWrapper(0x4000), 7));

            // Invocation performed identical operations on func and on dest.
            // Check that equality relationship was not disturbed.
            ASSERT(*dest.target<FUNC>() == *temp.target<FUNC>());
        }

        ASSERT(ta.numBlocksInUse() == destNumBlocksUsed);
        ASSERT(ta.numBytesInUse()  == destNumBytesUsed);
        ASSERT(globalAllocMonitor.isInUseSame());

        dest.~Obj();
    }
    ASSERT(ta.numBlocksInUse() == 0);
    ASSERT(ta.numBytesInUse()  == 0);
    ASSERT(globalAllocMonitor.isInUseSame());
}

template <class SRC_ALLOC, class DEST_ALLOC, class FUNC_ARG>
void testMoveCtorWithDifferentAlloc(FUNC_ARG    func,
                                    const char *sourceAllocName,
                                    const char *destAllocName)
{
    typedef typename NTUNWRAP_T(FUNC_ARG) FUNC;

    if (veryVeryVerbose)
        printf("\tAlloc: source = %s, dest = %s\n", sourceAllocName,
               destAllocName);

    bool isEmpty = isNullPtr(ntUnwrap(func));

    bslma::TestAllocator sourceTa;
    SRC_ALLOC sourceAlloc(&sourceTa);

    bslma::TestAllocator destTa;
    DEST_ALLOC destAlloc(&destTa);

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

    // Create a pair of objects identical to the post-move source and
    // post-move destination and measure the total memory usage.
    AllocSizeType sourceNumBlocksUsed, sourceNumBytesUsed;
    AllocSizeType destNumBlocksUsed, destNumBytesUsed;
    {
        // Since allocator is different, move operation is identical to copy.
        Obj postMoveSource(bsl::allocator_arg, sourceAlloc, func);
        sourceNumBlocksUsed = sourceTa.numBlocksInUse();
        sourceNumBytesUsed = sourceTa.numBytesInUse();

        Obj postMoveDest(bsl::allocator_arg, destAlloc, func);
        destNumBlocksUsed = destTa.numBlocksInUse();
        destNumBytesUsed = destTa.numBytesInUse();
    }
    ASSERT(destTa.numBlocksInUse() == 0);
    ASSERT(destTa.numBytesInUse()  == 0);
    ASSERT(globalAllocMonitor.isInUseSame());

    globalAllocMonitor.reset();

    {
        // Create source.
        Obj source(bsl::allocator_arg, sourceAlloc, func);
        FunctorMonitor funcMonitor(L_);

        EXCEPTION_TEST_BEGIN(&destTa, &moveLimit) {
            EXCEPTION_TEST_TRY {
                funcMonitor.reset(L_);

                // move-construct dest using extended move constructor.
                Obj dest(bsl::allocator_arg,
                         destAlloc,
                         bslmf::MovableRefUtil::move(source));

                ASSERT(CheckAlloc<SRC_ALLOC>::areEqualAlloc(sourceAlloc,
                                                          source.allocator()));

                ASSERT(isEmpty == ! dest);
                ASSERT(CheckAlloc<DEST_ALLOC>::areEqualAlloc(destAlloc,
                                                            dest.allocator()));

                ASSERT(allocPropagationCheck<FUNC>(dest));

                if (dest) {

                    // Check for faithful move of functor
                    ASSERT(dest.target_type() == typeid(FUNC));
                    ASSERT(*dest.target<FUNC>() == ntUnwrap(func));

                    // Invoke
                    Obj temp(func);
                    ASSERT(dest(IntWrapper(0x4000), 7) ==
                           temp(IntWrapper(0x4000), 7));

                    // Invocation performed identical operations on func and
                    // on dest.  Check that equality relationship was not
                    // disturbed.
                    ASSERT(*dest.target<FUNC>() == *temp.target<FUNC>());
                }

                ASSERT(sourceTa.numBlocksInUse() == sourceNumBlocksUsed);
                ASSERT(sourceTa.numBytesInUse()  == sourceNumBytesUsed);
                ASSERT(destTa.numBlocksInUse() == destNumBlocksUsed);
                ASSERT(destTa.numBytesInUse()  == destNumBytesUsed);
                ASSERT(globalAllocMonitor.isInUseSame());
            }
            EXCEPTION_TEST_CATCH {
                ASSERT(destTa.numBlocksInUse() == 0);
                ASSERT(destTa.numBytesInUse()  == 0);
                ASSERT(funcMonitor.isSameCount());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;
    }
    ASSERT(sourceTa.numBlocksInUse() == 0);
    ASSERT(sourceTa.numBytesInUse()  == 0);
    ASSERT(destTa.numBlocksInUse() == 0);
    ASSERT(destTa.numBytesInUse()  == 0);
    ASSERT(globalAllocMonitor.isInUseSame());
}

template <class SOURCE_ALLOC, class FUNC>
void testMoveCtor(FUNC func, const char *sourceAllocName)
{
    // Test normal move constructor.
    testMoveCtorWithSameAlloc<SOURCE_ALLOC>(func, false, sourceAllocName);

    // Test extended move constructor with same allocator for source and
    // destination.
    testMoveCtorWithSameAlloc<SOURCE_ALLOC>(func, true, sourceAllocName);

    // Test with different allocator types for source and destination
#define TEST(A)                                                               \
    testMoveCtorWithDifferentAlloc<SOURCE_ALLOC, A>(func, sourceAllocName, #A)

    TEST(bslma::TestAllocator *  );
    TEST(bsl::allocator<char>    );
    TEST(EmptySTLAllocator2<char>);
    TEST(StatefulAllocator<char> );
    TEST(StatefulAllocator2<char>);

#undef TEST

}

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class FUNC>
bool AreEqualFunctions(const Obj& inA, const Obj& inB)
    // Given a known invocable type specified by 'FUNC', return true if the
    // specified 'inA' wraps an invocable of type 'FUNC' with the same value as
    // the invocable wrapped by 'inB' (or they are both empty); otherwise
    // return false.  As a special case, if 'inB' is the special object
    // 'movedFromMarker', then return 'true' if 'inA' is empty or wraps an
    // invocable of type 'FUNC' that holds the moved-from value of 'FUNC';
    // otherwise return false.  This function also asserts that 'inA' and 'inB'
    // have correct allocator propagation from the 'function' object to the
    // wrapped functor.
{
    if (inA) ASSERT(allocPropagationCheck<FUNC>(inA));
    if (inB) ASSERT(allocPropagationCheck<FUNC>(inB));

    if (&inA == &inB) {
        return true;                                                  // RETURN
    }

    if (&inB == &movedFromMarker) {
        if (! inA) {
            return true;  // Empty is a valid moved-from state        // RETURN
        }
        else if (typeid(FUNC) != inA.target_type()) {
            return false;                                             // RETURN
        }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        // Create a 'FUNC' object in a moved-from state.  Note that all of our
        // test functors as well as function pointers can be initialized from
        // integer zero.
        FUNC movedFromFunc(0);
        {
            // Put 'movedFromFunc' into the moved-to state.
            // Disable move and copy limits during this operation.
            int savedMoveLimit = moveLimit.value();
            moveLimit = -1;
            int savedCopyLimit = copyLimit.value();
            copyLimit = -1;
            FUNC movedToFunc(bslmf::MovableRefUtil::move(movedFromFunc));
            ASSERT(movedToFunc == FUNC(0));
            copyLimit = savedCopyLimit;
            moveLimit = savedMoveLimit;
        }

        const FUNC *targetA = inA.target<FUNC>();
        ASSERT(targetA);
        return *targetA == movedFromFunc;                             // RETURN
#else
        return false;                                                 // RETURN
#endif
    }

    if (! inA || ! inB) {
        // One or both are empty
        if (static_cast<bool>(inA) == static_cast<bool>(inB)) {
            return true;  // Both are empty                           // RETURN
        }
        else {
            return false; // One is empty and the other is not        // RETURN
        }
    }

    if (typeid(FUNC) != inA.target_type() ||
        typeid(FUNC) != inB.target_type()) {
        return false; // One or both wrap invocables of the wrong type.
    }

    // Get the targets
    const FUNC *targetA = inA.target<FUNC>();
    const FUNC *targetB = inB.target<FUNC>();

    // Neither 'function' is empty, so neither should have a NULL target.
    ASSERT(targetA && targetB);
    ASSERT(targetA != targetB);  // Different objects have different targets

    return *targetA == *targetB;
}

template <class ALLOC>
void testSwap(const Obj& inA,
              const Obj& inB,
              bool (*    areEqualA_p)(const Obj&, const Obj&),
              bool (*    areEqualB_p)(const Obj&, const Obj&),
              int        lineA,
              int        lineB,
              bool       skipExcTest)
    // Test 'function::swap'.
{
    bslma::TestAllocator testAlloc;
    ALLOC alloc(&testAlloc);

    // Make copies of inA and inB.
    Obj a( bsl::allocator_arg, alloc, inA);
    Obj a2(bsl::allocator_arg, alloc, inA);
    Obj b( bsl::allocator_arg, alloc, inB);
    Obj b2(bsl::allocator_arg, alloc, inB);

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);
    bslma::TestAllocatorMonitor testAllocMonitor(&testAlloc);

    // swap() should not call any potentially-throwing operations; set
    // allocation limit and move limit to detect such operations unless
    // 'skipExcTest' is true.
    testAlloc.setAllocationLimit(skipExcTest ? -1 : 0);
    moveLimit = skipExcTest ? -1 : 0;
    a.swap(b);
    moveLimit = -1;
    testAlloc.setAllocationLimit(-1);
    LOOP2_ASSERT(lineA, lineB, areEqualA_p(a2, b));
    LOOP2_ASSERT(lineA, lineB, areEqualB_p(b2, a));
    LOOP2_ASSERT(lineA, lineB, CheckAlloc<ALLOC>::areEqualAlloc(alloc,
                                                            a.allocator()));
    LOOP2_ASSERT(lineA, lineB, CheckAlloc<ALLOC>::areEqualAlloc(alloc,
                                                            b.allocator()));
    LOOP2_ASSERT(lineA, lineB, globalAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAllocMonitor.isInUseSame());
    if (a2) {
        LOOP2_ASSERT(lineA, lineB, a2(1, 2) == b(1, 2));
    }
    if (b2) {
        LOOP2_ASSERT(lineA, lineB, b2(1, 2) == a(1, 2));
    }

    // Swap back using namespace-scope swap.  swap() should not call any
    // potentially-throwing operations; set allocation limit and move limit to
    // detect such operations unless 'skipExcTest' is true.
    testAlloc.setAllocationLimit(skipExcTest ? -1 : 0);
    moveLimit = skipExcTest ? -1 : 0;
    swap(b, a);
    moveLimit = -1;
    testAlloc.setAllocationLimit(-1);
    LOOP2_ASSERT(lineA, lineB, areEqualA_p(a2, a));
    LOOP2_ASSERT(lineA, lineB, areEqualB_p(b2, b));
    LOOP2_ASSERT(lineA, lineB, CheckAlloc<ALLOC>::areEqualAlloc(alloc,
                                                            a.allocator()));
    LOOP2_ASSERT(lineA, lineB, CheckAlloc<ALLOC>::areEqualAlloc(alloc,
                                                            b.allocator()));
    LOOP2_ASSERT(lineA, lineB, globalAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAllocMonitor.isInUseSame());
    if (a2) {
        LOOP2_ASSERT(lineA, lineB, a2(2, 3) == a(2, 3));
    }
    if (b2) {
        LOOP2_ASSERT(lineA, lineB, b2(2, 3) == b(2, 3));
    }
}

template <class ALLOC_A, class ALLOC_B>
void testAssignment(const Obj& inA,
                    const Obj& inB,
                    bool (*    areEqualA_p)(const Obj&, const Obj&),
                    bool (*    areEqualB_p)(const Obj&, const Obj&),
                    int        lineA,
                    int        lineB,
                    bool       skipMvExcTest)
    // Test copy and move assignment
{
    (void) skipMvExcTest; // Avoid unused arg warning in C++03 mode

    bslma::TestAllocator testAlloc1;
    bslma::TestAllocator testAlloc2;
    ALLOC_A allocA1(&testAlloc1);
    ALLOC_B allocB2(&testAlloc2);

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);
    bslma::TestAllocatorMonitor testAlloc1Monitor(&testAlloc1);
    bslma::TestAllocatorMonitor testAlloc2Monitor(&testAlloc2);

    // Test copy assignment
    EXCEPTION_TEST_BEGIN(&testAlloc1, &copyLimit) {

        FunctorMonitor funcMonitor(L_);

        // Make copies of inA and inB.
        AllocSizeType preA1Bytes = testAlloc1.numBytesInUse();
        Obj a(bsl::allocator_arg, allocA1, inA);
        AllocSizeType aBytesBefore =
            testAlloc1.numBytesInUse() - preA1Bytes;
        Obj b(bsl::allocator_arg, allocB2, inB);

        // 'exp' should look like 'a' after the assignment
        preA1Bytes = testAlloc1.numBytesInUse();
        Obj exp(bsl::allocator_arg, allocA1, inB);
        AllocSizeType expBytes = testAlloc1.numBytesInUse() - preA1Bytes;

        preA1Bytes = testAlloc1.numBytesInUse();
        AllocSizeType preB2Bytes = testAlloc2.numBytesInUse();
        AllocSizeType preB2Total = testAlloc2.numBytesTotal();
        EXCEPTION_TEST_TRY {
            if (skipMvExcTest) {
                testAlloc1.setAllocationLimit(-1);
                copyLimit = -1;
            }
            a = b;  ///////// COPY ASSIGNMENT //////////
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(a, inB));
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(b, inB)); // b is unchanged
            LOOP2_ASSERT(lineA, lineB,
                         CheckAlloc<ALLOC_A>::areEqualAlloc(allocA1,
                                                            a.allocator()));
            LOOP2_ASSERT(lineA, lineB,
                         CheckAlloc<ALLOC_B>::areEqualAlloc(allocB2,
                                                            b.allocator()));
            // Verify that memory allocator usage in a's allocator is the same
            // as destroying a and recreating it with b's functor.
            LOOP2_ASSERT(lineA, lineB,
                         postA1Bytes == preA1Bytes - aBytesBefore + expBytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
            if (a) {
                LOOP2_ASSERT(lineA, lineB, a(1, 2) == exp(1, 2));
            }
            if (b) {
                LOOP2_ASSERT(lineA, lineB, b(1, 2) == inB(1, 2));
            }
        }
        EXCEPTION_TEST_CATCH {
            // a and b are unchanged.
            LOOP2_ASSERT(lineA, lineB, areEqualA_p(a, inA));
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(b, inB));

            // No memory was leaked
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, postA1Bytes == preA1Bytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    LOOP2_ASSERT(lineA, lineB, globalAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc1Monitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc2Monitor.isInUseSame());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // Test move assignment with unequal allocators
    globalAllocMonitor.reset(&globalTestAllocator);
    testAlloc1Monitor.reset(&testAlloc1);
    testAlloc2Monitor.reset(&testAlloc2);

    EXCEPTION_TEST_BEGIN(&testAlloc1, &moveLimit) {

        FunctorMonitor funcMonitor(L_);

        // Make copies of inA and inB.
        AllocSizeType preA1Bytes = testAlloc1.numBytesInUse();
        Obj a( bsl::allocator_arg, allocA1, inA);
        AllocSizeType aBytesBefore =
            testAlloc1.numBytesInUse() - preA1Bytes;
        Obj b( bsl::allocator_arg, allocB2, inB);

        // 'exp' should look like 'a' after the assignment
        preA1Bytes = testAlloc1.numBytesInUse();
        Obj exp(bsl::allocator_arg, allocA1, inB);
        AllocSizeType expBytes = testAlloc1.numBytesInUse() - preA1Bytes;

        preA1Bytes = testAlloc1.numBytesInUse();
        AllocSizeType preB2Bytes = testAlloc2.numBytesInUse();
        AllocSizeType preB2Total = testAlloc2.numBytesTotal();
        EXCEPTION_TEST_TRY {
            if (skipMvExcTest) {
                testAlloc1.setAllocationLimit(-1);
                moveLimit = -1;
            }
            a = bslmf::MovableRefUtil::move(b);  ///// MOVE ASSIGNMENT //////
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(a, inB));
            // 'b' should be either in the moved-from state or unchanged.
            LOOP2_ASSERT(lineA, lineB, (areEqualB_p(b, movedFromMarker) ||
                                        areEqualB_p(b, inB)));
            LOOP2_ASSERT(lineA, lineB,
                         CheckAlloc<ALLOC_A>::areEqualAlloc(allocA1,
                                                            a.allocator()));
            LOOP2_ASSERT(lineA, lineB,
                         CheckAlloc<ALLOC_B>::areEqualAlloc(allocB2,
                                                            b.allocator()));
            // Verify that memory allocator usage in a's allocator is the same
            // as destroying a and recreating it with b's functor.
            LOOP2_ASSERT(lineA, lineB,
                         postA1Bytes == preA1Bytes - aBytesBefore + expBytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
            if (a) {
                LOOP2_ASSERT(lineA, lineB, a(1, 2) == exp(1, 2));
            }
        }
        EXCEPTION_TEST_CATCH {
            // a and b are unchanged.
            LOOP2_ASSERT(lineA, lineB, areEqualA_p(a, inA));
            LOOP2_ASSERT(lineA, lineB, areEqualB_p(b, inB));

            // No memory was leaked
            AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
            AllocSizeType postB2Bytes = testAlloc2.numBytesInUse();
            AllocSizeType postB2Total = testAlloc2.numBytesTotal();
            LOOP2_ASSERT(lineA, lineB, postA1Bytes == preA1Bytes);
            // No allocations or deallcations from testAlloc2
            LOOP2_ASSERT(lineA, lineB, postB2Bytes == preB2Bytes);
            LOOP2_ASSERT(lineA, lineB, postB2Total == preB2Total);
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;
    LOOP2_ASSERT(lineA, lineB, globalAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc1Monitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc2Monitor.isInUseSame());

    // Test move assignment with equal allocators
    globalAllocMonitor.reset(&globalTestAllocator);
    testAlloc1Monitor.reset(&testAlloc1);

    // Construct another allocator, 'allocB1' from the same source as 'allocA1'
    // but with type 'ALLOC_B'.  In order to test move assignment with equal
    // allocators, 'ALLOC_A' and 'ALLOC_B' need not be the same type, but they
    // must be compatible, and hence comparable.  If they are not comparable,
    // then skip this part of the test.
    ALLOC_B allocB1(&testAlloc1);
    if (areEqualAlloc(allocA1, allocB1)) {
        EXCEPTION_TEST_BEGIN(&testAlloc1, &moveLimit) {

            FunctorMonitor funcMonitor(L_);

            // Make copies of inA and inB.
            Obj a( bsl::allocator_arg, allocA1, inA);
            Obj b( bsl::allocator_arg, allocB1, inB);

            // 'exp' should look like 'a' after the assignment
            // Obj exp(bsl::allocator_arg, allocA1, inB);

            // preA1Bytes = testAlloc1.numBytesInUse();
            // Obj emptyObj(bsl::allocator_arg, allocA1);
            // AllocSizeType emptyBytes =
            //     testAlloc1.numBytesInUse() - preA1Bytes;

            EXCEPTION_TEST_TRY {
                if (skipMvExcTest) {
                    testAlloc1.setAllocationLimit(-1);
                    moveLimit = -1;
                }
                // Move assignment with equal allocators is the same as swap
                AllocSizeType preA1Bytes = testAlloc1.numBytesInUse();
                AllocSizeType preA1Total = testAlloc1.numBytesTotal();
                a = bslmf::MovableRefUtil::move(b);  ///// move ASSIGNMENT ////
                AllocSizeType postA1Bytes = testAlloc1.numBytesInUse();
                AllocSizeType postA1Total = testAlloc1.numBytesTotal();
                LOOP2_ASSERT(lineA, lineB, areEqualB_p(a, inB));
                LOOP2_ASSERT(lineA, lineB,
                             CheckAlloc<ALLOC_A>::areEqualAlloc(allocA1,
                                                               a.allocator()));
                LOOP2_ASSERT(lineA, lineB,
                             CheckAlloc<ALLOC_B>::areEqualAlloc(allocB1,
                                                               b.allocator()));
                // Verify that no memory was allocated or deallocated.
                LOOP2_ASSERT(lineA, lineB, postA1Bytes == preA1Bytes);
                LOOP2_ASSERT(lineA, lineB, postA1Total == preA1Total);
                if (a) {
                    LOOP2_ASSERT(lineA, lineB, a(1, 2) == inB(1, 2));
                }
                if (b) {
                    LOOP2_ASSERT(lineA, lineB, b(1, 2) == inA(1, 2));
                }
            }
            EXCEPTION_TEST_CATCH {
                LOOP2_ASSERT(lineA, lineB,
                             0 && "Exception should not be thrown");
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;
    }
    LOOP2_ASSERT(lineA, lineB, globalAllocMonitor.isInUseSame());
    LOOP2_ASSERT(lineA, lineB, testAlloc1Monitor.isInUseSame());

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
}

template <class ALLOC>
void testAssignNullptr(const Obj& func, int line)
{
    bslma::TestAllocator testAlloc;
    ALLOC alloc(&testAlloc);

    // Measure the number of blocks allocated for an empty 'function' using
    // the same allocator.
    AllocSizeType expNumBlocks;
    {
        Obj emptyObj(bsl::allocator_arg, alloc);
        expNumBlocks = testAlloc.numBlocksInUse();
    }

    ASSERT(0 == testAlloc.numBlocksInUse());

    FunctorMonitor funcMonitor(L_);

    // Make copy of function with specified allocator
    Obj funcCopy(bsl::allocator_arg, alloc, func);

    AllocSizeType expTotalBlocks = testAlloc.numBlocksTotal();

    testAlloc.setAllocationLimit(0);    // Disable new allocations
    moveLimit = 0;                      // Disable throwing-functor moves
    copyLimit = 0;                      // Disable throwing-functor copies
    funcCopy = bsl::nullptr_t(); ///////// Assignment from nullptr ////////
    copyLimit = -1;
    moveLimit = -1;
    testAlloc.setAllocationLimit(-1);

    LOOP_ASSERT(line, ! funcCopy);
    LOOP_ASSERT(line, areEqualAlloc(alloc, funcCopy.allocator()));
    LOOP_ASSERT(line, funcMonitor.isSameCount());
    // Test memory usage:
    //  * No new memory was allocated (total did not increase)
    //  * Memory blocks used matches expected use for empty function.
    //    (Bytes used might be larger in some cases.)
    LOOP_ASSERT(line, expTotalBlocks == testAlloc.numBlocksTotal());
    LOOP_ASSERT(line, expNumBlocks   == testAlloc.numBlocksInUse());
}

template <class ALLOC, class FUNC_ARG>
void testAssignFromFunctor(const Obj&   lhsIn,
                           FUNC_ARG     rhsIn,
                           const char  *allocName,
                           const char  *lhsFuncName,
                           const char  *rhsFuncName,
                           bool         skipExcTest)
{
    typedef typename NTUNWRAP_T(FUNC_ARG) FUNC;

    (void) skipExcTest; // Avoid unused arg warning in C++03 mode.

    if (veryVeryVerbose) {
        printf("\tObj lhs(allocator_arg, %s, %s); rhs = %s;\n",
               allocName, lhsFuncName, rhsFuncName);
    }

    bslma::TestAllocator ta;
    ALLOC alloc(&ta);

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);
    bslma::TestAllocatorMonitor testAllocMonitor(&ta);
    FunctorMonitor funcMonitor(L_);

    // Test copy-assignment from non-const functor
    EXCEPTION_TEST_BEGIN(&ta, &copyLimit) {
        // Make copy of lhsIn using desired allocator.  Measure memory usage.
        AllocSizeType preBytes = ta.numBytesInUse();
        Obj lhs(bsl::allocator_arg, alloc, lhsIn);
        AllocSizeType lhsBytesBefore = ta.numBytesInUse() - preBytes;

        // Make copy of 'rhsIn'.  The copy is a non-const lvalue, but its
        // value should be unchanged by the assignment.
        FUNC_ARG rhs(rhsIn);

        // 'exp' is what 'lsh' should look like after the assignment.
        preBytes = ta.numBytesInUse();
        Obj exp(bsl::allocator_arg, alloc, rhsIn);
        AllocSizeType expBytes = ta.numBytesInUse() - preBytes;

        preBytes = ta.numBytesInUse();
        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                ta.setAllocationLimit(-1);
                copyLimit = -1;
            }
            lhs = rhs;  ///////// COPY-ASSIGNMENT FROM FUNC_ARG //////////

            // The number of bytes used by the lhs after the assignment is
            // equal to the number of bytes used before the assignment plus
            // the delta caused by the assignment.  Note that the delta might
            // be negative.
            AllocSizeType lhsBytesAfter = (lhsBytesBefore +
                                           ta.numBytesInUse() -
                                           preBytes);
            if (exp) {
                // Non-empty expected result
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName, lhs);
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs.target_type() == typeid(FUNC));
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             *lhs.target<FUNC>() == ntUnwrap(rhsIn));
            }
            else {
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName, ! lhs);
            }
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         CheckAlloc<ALLOC>::areEqualAlloc(alloc,
                                                          lhs.allocator()));
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         lhsBytesAfter == expBytes);

            // verify that rhs is unchanged
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         ntUnwrap(rhs) == ntUnwrap(rhsIn));

            if (lhs && exp) {
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs(1, 2) == exp(1, 2));
            }

            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         allocPropagationCheck<FUNC>(lhs));
        }
        EXCEPTION_TEST_CATCH {
            // verify that both lhs and rhs are unchanged
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         ntUnwrap(rhs) == ntUnwrap(rhsIn));
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         lhs.target_type() == lhsIn.target_type());
            if (lhs && lhsIn) {
                Obj lhsInCopy(lhsIn);
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs(1, 2) == lhsInCopy(1, 2));
            }
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 globalAllocMonitor.isInUseSame());
    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 testAllocMonitor.isInUseSame());
    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 funcMonitor.isSameCount());

    // Test copy-assignment from const functor
    EXCEPTION_TEST_BEGIN(&ta, &copyLimit) {
        // Make copy of lhsIn using desired allocator.  Measure memory usage.
        Obj lhs(bsl::allocator_arg, alloc, lhsIn);

        // Make copy of 'rhsIn'.  The copy is a non-const lvalue, but its
        // value should be unchanged by the assignment.
        FUNC_ARG rhs(rhsIn); const FUNC_ARG& RHS = rhs;

        // 'exp' is what 'lsh' should look like after the assignment.
        Obj exp(bsl::allocator_arg, alloc, rhsIn);

        EXCEPTION_TEST_TRY {
            if (skipExcTest) {
                ta.setAllocationLimit(-1);
                copyLimit = -1;
            }
            // Prove that assignment can be called with const rhs.
            lhs = RHS;  // Assignment from const rhs

            // Basic test that assignment worked.
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName, ! lhs == ! exp);
            if (lhs && exp) {
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs(1, 2) == exp(1, 2));
            }

            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         allocPropagationCheck<FUNC>(lhs));
        }
        EXCEPTION_TEST_CATCH {
            // verify that both lhs and rhs are unchanged
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         ntUnwrap(rhs) == ntUnwrap(rhsIn));
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         lhs.target_type() == lhsIn.target_type());
            if (lhs && lhsIn) {
                Obj lhsInCopy(lhsIn);
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs(1, 2) == lhsInCopy(1, 2));
            }
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 globalAllocMonitor.isInUseSame());
    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 testAllocMonitor.isInUseSame());
    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 funcMonitor.isSameCount());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

    // Construct a 'FUNC' object duplicating 'rhsIn' in a moved-from state.
    FUNC movedFromRhs(ntUnwrap(rhsIn));
    {
        FUNC movedToFunc(bslmf::MovableRefUtil::move(movedFromRhs));
        (void) movedToFunc;
    }

    FunctorMonitor funcMonitor2(L_);

    // Test move-assignment from functor
    EXCEPTION_TEST_BEGIN(&ta, skipExcTest ? NULL : &moveLimit) {
        // Make copy of lhsIn using desired allocator.  Measure memory usage.
        AllocSizeType preBytes = ta.numBytesInUse();
        Obj lhs(bsl::allocator_arg, alloc, lhsIn);
        AllocSizeType lhsBytesBefore = ta.numBytesInUse() - preBytes;

        // Copy 'rhsIn' so as to not change 'rhsIn'
        FUNC_ARG rhs(rhsIn);

        // 'exp' is what 'lsh' should look like after the assignment
        preBytes = ta.numBytesInUse();
        Obj exp(bsl::allocator_arg, alloc, rhsIn);
        AllocSizeType expBytes = ta.numBytesInUse() - preBytes;

        preBytes = ta.numBytesInUse();
        EXCEPTION_TEST_TRY {
            lhs = bslmf::MovableRefUtil::move(rhs);
                                      ///// MOVE-ASSIGNMENT FROM FUNC_ARG /////

            // The number of bytes used by the lhs after the assignment is
            // equal to the number of bytes used before the assignment plus the
            // delta caused by the assignment.  Note that the delta might be
            // negative.
            AllocSizeType lhsBytesAfter = (lhsBytesBefore +
                                           ta.numBytesInUse() -
                                           preBytes);
            if (exp) {
                // Non-empty expected result
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName, lhs);
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs.target_type() == typeid(FUNC));
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             *lhs.target<FUNC>() == ntUnwrap(rhsIn));
            }
            else {
                // Empty expected result
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName, ! lhs);
            }
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         CheckAlloc<ALLOC>::areEqualAlloc(alloc,
                                                          lhs.allocator()));
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         lhsBytesAfter == expBytes);

            // verify that rhs is in moved-from state
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         ntUnwrap(rhs) == movedFromRhs);

            if (lhs && exp) {
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs(1, 2) == exp(1, 2));
            }

            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         allocPropagationCheck<FUNC>(lhs));
        }
        EXCEPTION_TEST_CATCH {
            // verify that both lhs and rhs are unchanged
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         ntUnwrap(rhs) == ntUnwrap(rhsIn));
            LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                         lhs.target_type() == lhsIn.target_type());
            if (lhs && lhsIn) {
                Obj lhsInCopy(lhsIn);
                LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                             lhs(1, 2) == lhsInCopy(1, 2));
            }
        } EXCEPTION_TEST_ENDTRY;
    } EXCEPTION_TEST_END;

    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 globalAllocMonitor.isInUseSame());
    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 testAllocMonitor.isInUseSame());
    LOOP3_ASSERT(allocName, lhsFuncName, rhsFuncName,
                 funcMonitor2.isSameCount());

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

}

//=============================================================================
//                  USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
                     verbose = argc > 2;
                 veryVerbose = argc > 3;
             veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    // Use a test allocator as the global allocator.  The global allocator is
    // used only for global singleton objects, so it's use within this test
    // would always be an error.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    // Use a test allocator for the default allocator. This allocator is used
    // when no other allocator is supplied.
    bslma::Default::setDefaultAllocator(&globalTestAllocator);

    // Confirm no static initialization locked the default allocator
    ASSERT(&globalTestAllocator == bslma::Default::defaultAllocator());

    // Top-level monitor to make sure that every functor constructor is
    // matched with a destructor.
    FunctorMonitor topFuncMonitor(L_);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.

      case 16: {
        // --------------------------------------------------------------------
        // COMPARISON TO NULLPTR
        //
        // Concerns:
        //: 1 If 'function' f is empty, then 'f == nullptr' is true,
        //:   'nullptr == f' is true, 'f != nullptr' is false and
        //:   'nullptr != f' is false.
        //: 2 If 'function' f is not empty, then 'f == nullptr' is false,
        //:   'nullptr == f' is false, 'f != nullptr' is true and
        //:   'nullptr != f' is true.
        //
        // Plan:
        //: 1 Construct an empty 'function' object 'e' and a non-empty
        //:   function object 'f'.
        //: 2 For concern 1, verify that  'e == nullptr' is true,
        //:   'nullptr == e' is true, 'e != nullptr' is false and
        //:   'nullptr != e' is false.
        //: 3 For concern 2, verify that 'f == nullptr' is false,
        //:   'nullptr == f' is false, 'f != nullptr' is true and
        //:   'nullptr != f' is true.
        //
        // Testing:
        //      bool operator==(const function& f, nullptr_t);
        //      bool operator==(nullptr_t, const function& f);
        //      bool operator!=(const function& f, nullptr_t);
        //      bool operator!=(nullptr_t, const function& f);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOMPARISON TO NULLPTR"
                            "\n=====================\n");

        Obj e;
        Obj f(&simpleFunc);

        ASSERT(  e == bsl::nullptr_t() );
        ASSERT(  bsl::nullptr_t() == e );
        ASSERT(!(e != bsl::nullptr_t()));
        ASSERT(!(bsl::nullptr_t() != e));

        ASSERT(!(f == bsl::nullptr_t()));
        ASSERT(!(bsl::nullptr_t() == f));
        ASSERT(  f != bsl::nullptr_t() );
        ASSERT(  bsl::nullptr_t() != f );

        // Just for grins, let's make sure that everything becomes reversed if
        // we swap 'e' and 'f'.
        e.swap(f);

        ASSERT(!(e == bsl::nullptr_t()));
        ASSERT(!(bsl::nullptr_t() == e));
        ASSERT(  e != bsl::nullptr_t() );
        ASSERT(  bsl::nullptr_t() != e );

        ASSERT(  f == bsl::nullptr_t() );
        ASSERT(  bsl::nullptr_t() == f );
        ASSERT(!(f != bsl::nullptr_t()));
        ASSERT(!(bsl::nullptr_t() != f));

      } break;

      case 15: {
        // --------------------------------------------------------------------
        // ASSIGNMENT FROM FUNCTOR
        //
        // Concerns:
        //: 1 Assigning a 'function' object the value of a functor,
        //:   pointer-to-function, or pointer-to-member function results in
        //:   the lhs having the same value as if it were constructed with
        //:   that functor, pointer-to-functor or pointer-to-member.
        //: 2 The functor previously wrapped by the lhs is destroyed.
        //: 3 If the rhs is an rvalue, the assignment will put it into a
        //:   moved-from state.
        //: 4 After the assignment, the allocator of the lhs is unchanged.
        //: 5 If assignment is from a functor that takes an allocator, the
        //:   copy of that functor in the lhs uses the same allocator.
        //: 6 The change in memory allocation is the same as if the lhs were
        //:   destroyed then re-constructed with its original allocator and
        //:   with the specified functor.
        //: 7 The above concerns apply to the entire range of functor types
        //:   and allocator types for the lhs and functor types for the rhs,
        //:   including for functors in nothrow wrappers.
        //: 8 If an exception is thrown, both lhs and rhs are unchanged.
        //
        // Plan:
        //: 1 For concern 1, assign from functor to a 'function' object and
        //:   verify that the 'target_type' and 'target' of the 'function'
        //:   matches the functor.
        //: 2 For concern 2, use an original functor type that tracks number
        //:   instances in existance.  Verify that the assignment from a
        //:   different functor type reducers the number of such functors in
        //:   existance.
        //: 3 For concern 3, test the assignment using both lvalue and rvalues
        //:   for the rhs.  In the case of rvalues, verify that the state of
        //:   the rhs after the assignment is matches the moved-from state for
        //:   that type.
        //: 4 For concern 4, check the allocator of the lhs after assignment
        //:   and verify that it is equivalent to the allocator before the
        //:   assignment.
        //: 5 For concern 5, verify that the lhs after the assignment uses
        //:   allocator propagation.
        //: 6 For concern 6, measure the memory use for constructing the lhs
        //:   and the memory use for constructing a 'function' from the rhs.
        //:   After assigning from the rhs to the lhs, verify that the memory
        //:   change is the difference between these memory values.
        //: 7 For concern 7, construct an array of 'function' objects created
        //:   with different functor types.  Package the steps above into a
        //:   template function 'testAssignFromFunctor', which is
        //:   parameterized by allocator type and functor type.  The test
        //:   function will make a copy of the input 'function' using the
        //:   desired allocator and use that as the lhs for the steps above.
        //:   Instantiate 'testAssignFromFunctor' with each combination of our
        //:   test allocator and test functor types.  Call each instantiation
        //:   with each functor in the data array.
        //: 8 For concern 8, test assignments in within the exception-test
        //:   framework and verify that, on exception, both operands retain
        //:   their original values.
        //
        // Testing:
        //      function& operator=(FUNC&& f);
        // --------------------------------------------------------------------

        if (verbose) printf("\nASSIGNMENT FROM FUNCTOR"
                            "\n=======================\n");

        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for one dimension of test

            int               d_line;           // Line number
            Obj               d_function;       // function object to assign
            const char       *d_funcName;       // function object name
            bool              d_skipExcTest;    // skip exception test
        };

#define TEST_ITEM(F, V)                          \
        { L_, Obj(F(V)), #F "(" #V ")", false }

#define NTTST_ITM(F, V)                          \
        { L_, Obj(ntWrap(F(V))), "ntWrap(" #F "(" #V "))", true }

        TestData data[] = {
            TEST_ITEM(SimpleFuncPtr_t             , nullFuncPtr       ),
            TEST_ITEM(SimpleMemFuncPtr_t          , nullMemFuncPtr    ),
            TEST_ITEM(SimpleFuncPtr_t             , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t          , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor                , 0                 ),
            TEST_ITEM(SmallFunctor                , 0x2000            ),
            TEST_ITEM(MediumFunctor               , 0x4000            ),
            TEST_ITEM(LargeFunctor                , 0x6000            ),
            TEST_ITEM(NTSmallFunctor              , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor        , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor        , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc       , 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc     , 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc       , 0x1000            ),

            NTTST_ITM(SimpleMemFuncPtr_t          , &IntWrapper::add1 ),
            NTTST_ITM(EmptyFunctor                , 0                 ),
            NTTST_ITM(SmallFunctor                , 0x2000            ),
            NTTST_ITM(LargeFunctor                , 0x6000            ),
            NTTST_ITM(NTSmallFunctor              , 0x3000            ),
            NTTST_ITM(ThrowingSmallFunctor        , 0x7000            ),
            NTTST_ITM(SmallFunctorWithAlloc       , 0x2000            ),
        };

        const int dataSize = sizeof(data) / sizeof(data[0]);

#undef TEST_ITEM
#undef NTTST_ITM

#define TEST(A, f) testAssignFromFunctor<A>(lhs, f, #A, funcName, #f, \
                                            skipExcTest)

        for (int i = 0; i < dataSize; ++i) {
            // const int line          = data[i].d_line;
            const Obj& lhs             = data[i].d_function;
            const char* funcName       = data[i].d_funcName;
            bool skipExcTest           = data[i].d_skipExcTest;

            if (veryVerbose) printf("Assign %s = nullFuncPtr\n", funcName);
            TEST(bslma::TestAllocator *  , nullFuncPtr          );
            TEST(bsl::allocator<char>    , nullFuncPtr          );
            TEST(EmptySTLAllocator<char> , nullFuncPtr          );
            TEST(StatefulAllocator<char> , nullFuncPtr          );
            TEST(StatefulAllocator2<char>, nullFuncPtr          );

            if (veryVerbose) printf("Assign %s = nullMemFuncPtr\n", funcName);
            TEST(bslma::TestAllocator *  , nullMemFuncPtr       );
            TEST(bsl::allocator<char>    , nullMemFuncPtr       );
            TEST(EmptySTLAllocator<char> , nullMemFuncPtr       );
            TEST(StatefulAllocator<char> , nullMemFuncPtr       );
            TEST(StatefulAllocator2<char>, nullMemFuncPtr       );

            if (veryVerbose) printf("Assign %s = simpleFunc\n", funcName);
            TEST(bslma::TestAllocator *  , simpleFunc           );
            TEST(bsl::allocator<char>    , simpleFunc           );
            TEST(EmptySTLAllocator<char> , simpleFunc           );
            TEST(StatefulAllocator<char> , simpleFunc           );
            TEST(StatefulAllocator2<char>, simpleFunc           );

            if (veryVerbose) printf("Assign %s = &IntWrapper::add1\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , &IntWrapper::add1    );
            TEST(bsl::allocator<char>    , &IntWrapper::add1    );
            TEST(EmptySTLAllocator<char> , &IntWrapper::add1    );
            TEST(StatefulAllocator<char> , &IntWrapper::add1    );
            TEST(StatefulAllocator2<char>, &IntWrapper::add1    );

            if (veryVerbose) printf("Assign %s = EmptyFunctor()\n", funcName);
            TEST(bslma::TestAllocator *  , EmptyFunctor()       );
            TEST(bsl::allocator<char>    , EmptyFunctor()       );
            TEST(EmptySTLAllocator<char> , EmptyFunctor()       );
            TEST(StatefulAllocator<char> , EmptyFunctor()       );
            TEST(StatefulAllocator2<char>, EmptyFunctor()       );

            if (veryVerbose) printf("Assign %s = SmallFunctor(0x2000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , SmallFunctor(0x2000) );
            TEST(bsl::allocator<char>    , SmallFunctor(0x2000) );
            TEST(EmptySTLAllocator<char> , SmallFunctor(0x2000) );
            TEST(StatefulAllocator<char> , SmallFunctor(0x2000) );
            TEST(StatefulAllocator2<char>, SmallFunctor(0x2000) );

            if (veryVerbose) printf("Assign %s = MediumFunctor(0x4000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , MediumFunctor(0x4000));
            TEST(bsl::allocator<char>    , MediumFunctor(0x4000));
            TEST(EmptySTLAllocator<char> , MediumFunctor(0x4000));
            TEST(StatefulAllocator<char> , MediumFunctor(0x4000));
            TEST(StatefulAllocator2<char>, MediumFunctor(0x4000));

            if (veryVerbose) printf("Assign %s = LargeFunctor(0x6000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , LargeFunctor(0x6000) );
            TEST(bsl::allocator<char>    , LargeFunctor(0x6000) );
            TEST(EmptySTLAllocator<char> , LargeFunctor(0x6000) );
            TEST(StatefulAllocator<char> , LargeFunctor(0x6000) );
            TEST(StatefulAllocator2<char>, LargeFunctor(0x6000) );

            if (veryVerbose) printf("Assign %s = NTSmallFunctor(0x3000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , NTSmallFunctor(0x3000));
            TEST(bsl::allocator<char>    , NTSmallFunctor(0x3000));
            TEST(EmptySTLAllocator<char> , NTSmallFunctor(0x3000));
            TEST(StatefulAllocator<char> , NTSmallFunctor(0x3000));
            TEST(StatefulAllocator2<char>, NTSmallFunctor(0x3000));

            if (veryVerbose) printf("Assign %s = ThrowingSmallFunctor(0x7000)"
                                    "\n", funcName);
            TEST(bslma::TestAllocator *  , ThrowingSmallFunctor(0x7000));
            TEST(bsl::allocator<char>    , ThrowingSmallFunctor(0x7000));
            TEST(EmptySTLAllocator<char> , ThrowingSmallFunctor(0x7000));
            TEST(StatefulAllocator<char> , ThrowingSmallFunctor(0x7000));
            TEST(StatefulAllocator2<char>, ThrowingSmallFunctor(0x7000));

            if (veryVerbose) printf("Assign %s = ThrowingEmptyFunctor()\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , ThrowingEmptyFunctor()     );
            TEST(StatefulAllocator2<char>, ThrowingEmptyFunctor()     );

            if (veryVerbose) printf("Assign %s = "
                                    "SmallFunctorWithAlloc(0x2000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , SmallFunctorWithAlloc(0x2000) );
            TEST(bsl::allocator<char>    , SmallFunctorWithAlloc(0x2000) );
            TEST(EmptySTLAllocator<char> , SmallFunctorWithAlloc(0x2000) );
            TEST(StatefulAllocator<char> , SmallFunctorWithAlloc(0x2000) );
            TEST(StatefulAllocator2<char>, SmallFunctorWithAlloc(0x2000) );

            if (veryVerbose) printf("Assign %s = "
                                    "NTSmallFunctorWithAlloc(0x2000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , NTSmallFunctorWithAlloc(0x2000) );
            TEST(bsl::allocator<char>    , NTSmallFunctorWithAlloc(0x2000) );
            TEST(EmptySTLAllocator<char> , NTSmallFunctorWithAlloc(0x2000) );
            TEST(StatefulAllocator<char> , NTSmallFunctorWithAlloc(0x2000) );
            TEST(StatefulAllocator2<char>, NTSmallFunctorWithAlloc(0x2000) );

            if (veryVerbose) printf("Assign %s = "
                                    "LargeFunctorWithAlloc(0x2000)\n",
                                    funcName);
            TEST(bslma::TestAllocator *  , LargeFunctorWithAlloc(0x2000) );
            TEST(bsl::allocator<char>    , LargeFunctorWithAlloc(0x2000) );
            TEST(EmptySTLAllocator<char> , LargeFunctorWithAlloc(0x2000) );
            TEST(StatefulAllocator<char> , LargeFunctorWithAlloc(0x2000) );
            TEST(StatefulAllocator2<char>, LargeFunctorWithAlloc(0x2000) );

// Test with nothrow wrapper on right side
#define NTW_TEST(A, f) testAssignFromFunctor<A>(lhs, ntWrap(f), #A, funcName, \
                                                "ntWrap(" #f ")", true)

            if (veryVerbose) printf("Assign from nothrow wrapper\n");
            NTW_TEST(bslma::TestAllocator *  , &simpleFunc          );
            NTW_TEST(bsl::allocator<char>    , EmptyFunctor()       );
            NTW_TEST(EmptySTLAllocator<char> , SmallFunctor(0x2000) );
            NTW_TEST(StatefulAllocator<char> , MediumFunctor(0x4000));
            NTW_TEST(StatefulAllocator2<char>, LargeFunctor(0x6000) );
            NTW_TEST(bslma::TestAllocator *  , ThrowingSmallFunctor(0x7000) );
            NTW_TEST(bslma::TestAllocator *  , ThrowingEmptyFunctor()       );
            NTW_TEST(bsl::allocator<char>    , SmallFunctorWithAlloc(0x2000));

        } // end for (each array item)

#undef TEST

      } break;

      case 14: {
        // --------------------------------------------------------------------
        // ASSIGNMENT FROM 'nullptr'
        //
        // Concerns:
        //: 1 Assigning a 'function' object the value 'nullptr' results in an
        //:   empty 'function'.
        //: 2 The allocator of the assigned 'function' does not change.
        //: 3 No memory is allocated by the assignment, though memory might be
        //:   freed. The number of blocks used after the assignment should
        //:   match the number of blocks used by an empty functor with the
        //:   same allocator (though the number of bytes might increase).
        //: 4 No potentially-throwing operations are invoked.
        //: 5 The above concerns apply to 'funcition' objects constructed with
        //:   every category of allocator and every category of wrapped
        //:   functor, including functors in a nothrow wrapper.
        //
        // Plan:
        //: 1 For concern 1, create a 'function', 'f', and assign 'f =
        //:   nullptr'. Verify that 'f.empty()' is true after the assignment.
        //: 2 For concern 2, verify that the allocator is the same before and
        //:   after the assignment.
        //: 3 For concern 3, test the blocks used from the allocator
        //:   before and after the assignment.  Verify that the total used
        //:   does not increase and that the number of blocks used after the
        //:   assignment matches the number of blocks used for an empty
        //:   function using the same allocator.
        //: 4 For concern 4, construct the initial 'function' using a functor
        //:   that throws on move or copy and set the allocator to throw on
        //:   the first allocation request.  Verify that the assignment works
        //:   without throwing.
        //: 5 For concern 5, encapsulate the above steps into a function
        //:   template, 'testAssignNullptr', parameterized on allocator type
        //:   and taking a 'function' argument.  Create an array of 'function'
        //:   objects, each of which is constructed with a functor from a
        //:   different category (small, large, throwing, non-throwing, etc.).
        //:   Invoke 'testAssignNullptr' with each category of allocator
        //:   ('bslma::Allocator*', 'bsl::allocator', empty STL-allocator,
        //:   stateful STL-allocator, etc.) for each element of the array of
        //:   'function'.
        //
        // Testing:
        //      function<RET(ARGS...)>& operator=(nullptr_t);
        // --------------------------------------------------------------------

        if (verbose) printf("\nASSIGNMENT FROM 'nullptr'"
                            "\n=========================\n");

        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for one dimension of test

            int               d_line;           // Line number
            Obj               d_function;       // function object to swap
            const char       *d_funcName;       // function object name
        };

#define TEST_ITEM(F, V)                          \
        { L_, Obj(F(V)), #F "(" #V ")" }

#define NTTST_ITM(F, V)                          \
        { L_, Obj(ntWrap(F(V))), "ntWrap(" #F "(" #V "))" }

        TestData data[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x2000            ),
            TEST_ITEM(MediumFunctor          , 0x4000            ),
            TEST_ITEM(LargeFunctor           , 0x6000            ),
            TEST_ITEM(NTSmallFunctor         , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x1000            ),

            NTTST_ITM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            NTTST_ITM(EmptyFunctor           , 0                 ),
            NTTST_ITM(SmallFunctor           , 0x2000            ),
            NTTST_ITM(LargeFunctor           , 0x6000            ),
            NTTST_ITM(NTSmallFunctor         , 0x3000            ),
            NTTST_ITM(ThrowingSmallFunctor   , 0x7000            ),
            NTTST_ITM(SmallFunctorWithAlloc  , 0x2000            ),
        };

#undef TEST_ITEM
#undef NTTST_ITM

        int dataSize = sizeof(data) / sizeof(TestData);

#define TEST(ALLOC) do {                                                    \
            if (veryVeryVerbose) printf("\tAllocator type = %s\n", #ALLOC); \
            testAssignNullptr<ALLOC>(func, line);                           \
        } while (false)

        for (int i = 0; i < dataSize; ++i) {
            const int line             = data[i].d_line;
            const Obj& func            = data[i].d_function;
            const char* funcName       = data[i].d_funcName;

            if (veryVerbose) printf("Assign %s = nullptr\n", funcName);

            TEST(bslma::TestAllocator *  );
            TEST(bsl::allocator<char>    );
            TEST(EmptySTLAllocator<char> );
            TEST(StatefulAllocator<char> );
            TEST(StatefulAllocator2<char>);

        } // end for (each function in data array)

#undef TEST

      } break;

      case 13: {
        // --------------------------------------------------------------------
        // COPY AND MOVE ASSIGNMENT
        //
        // Concerns:
        //: 1 The rhs of an assignment wraps a functor equal to the lhs before
        //:   the assignment for both copy assignment and move assignment.
        //: 2 The allocator of the lhs is not changed by the assignment and
        //:   its wrapped functor's allocator (if any) continues to reflect
        //:   proper allocator propagation.
        //: 3 The rhs of a copy assignment is not changed.
        //: 4 For move assignment, if the lhs and rhs allocators are same type
        //:   and compare equal, no memory is allocated and the rhs is swapped
        //:   with the lhs.
        //:   to be an empty 'function'.
        //: 5 If the lhs and rhs allocators do not compare equal, the memory
        //:   allocation behavior of move assignment is identical to that of
        //:   copy assignment and the rhs is either unchanged or is changed to
        //:   the moved-from state.
        //: 6 For move assignment, if the lhs and rhs allocators compare
        //:   equal, then no exception is thrown, even if the wrapped functor
        //:   has a throwing move constructor.
        //: 7 Memory formerly belonging to the lhs is released by both copy
        //:   and move assignment.
        //: 8 The above concerns apply for each of the different types of
        //:   wrapped functors.
        //: 9 The above concerns apply to functions constructed with allocator
        //:   constructor arguments that are pointers to type derived from
        //:   'bslma::Allocator' and stateful STL-style allocators. (It is not
        //:   necessary to separately test 'bsl::allocator' instantiations or
        //:   stateless STL-style allocators, as these are represented
        //:   internally as pointers to 'bslma::Allocator') Note that the
        //:   allocators in the lhs and rhs might be different.
        //: 10 If an exception is thrown during an assignment, both operands
        //:   of the assignment are unchanged.
        //: 11 The above concerns apply if the functor used to construct
        //:   either operand of the assignment was wrapped in a nothrow
        //:   wrapper.
        //
        // Plan:
        //: 1 For concern 1, construct a pair of 'function' objects 'a' and
        //:   'b'.  Copy-construct 'a1' and 'b1' from 'a' and 'b' respectively
        //:   and assign 'a1 = b1'.  Verify that the target type and target
        //:   value for 'a1' after the assignment matches the target type and
        //:   target of 'b'.  Make another pair of copies, 'a2' and 'b2' and
        //:   repeat the test for move assignment.
        //: 2 For concern 2, verify that allocators of 'a1' and 'a2' after the
        //:   assignments in step 1 match the allocator used to construct them
        //:   and that the wrapped functors' allocators (if any) continue to
        //:   reflect proper allocator propagation.
        //: 3 For concern 3, verify that the target type and target of 'b1'
        //:   matches the target type and target of 'b'.
        //: 4 For concern 4, check the allocators of 'a2' and 'b2' before the
        //:   assignment and, if they are equal, verify that no allocations
        //:   or deallocations are performed using either that allocator or
        //:   the global allocator as a result of the move assignment.
        //: 5 For concern 5, check the allocators of 'a2' and 'b2' before the
        //:   assignment and, if they are not equal, verify that copying 'b2'
        //:   to 'a2' by move assignment results in a memory footprint
        //:   equivalent to destroying 'a1' and move-constructing 'b' using
        //:   'a1's allocator.  Verify that the function wrapped 'b1' is put
        //:   into either in a moved-from state or left unchanged.
        //: 6 For concern 6, when 'a2' and 'b2' have the same allocator, turn
        //:   on instrumentation in the allocators and the functors that would
        //:   cause them to throw exceptions.  Verify that the move assignment
        //:   succeeds even with this instrumentation active.
        //: 7 For concern 7, track the amount of
        //:   memory used to construct 'a1', 'b1', 'a2', and 'b2'.  Verify
        //:   that, after the assignment, the memory used matches the memory
        //:   that would be used by two copies of 'b1' and one or two copies
        //:   of 'b2' (depending on whether or not 'a2' and 'b2' have the same
        //:   allocator).
        //: 8 For concerns 8 and 9, package the above steps into a function
        //:   template, 'testAssign', which is instantiated on two allocator
        //:   types and takes two 'function' object arguments as well as
        //:   comparison functions for each functor type.  The comparison
        //:   function are used to test if the targets compare equal and to
        //:   assert that they have proper allocator-propagation.  In
        //:   'testAssign', 'a' is copied into 'a1' and 'a2' using the first
        //:   allocator and 'b' is copied into 'b1' and 'b2' using the second
        //:   allocator argument.  Create two arrays where each array element
        //:   contains a function object and a pointer to a function that can
        //:   compare that function for equality.  Each function object is
        //:   constructed with a different invocable and the comparison
        //:   function is instantiated with the type of that invocable.  Loop
        //:   through the 4-way cross product of the two arrays and two sets
        //:   of allocator types and call 'testAssign' to perform the test on
        //:   each combination.
        //: 9 For concern 10, test assignments in within the exception-test
        //:   framework and verify that, on exception, both operands retain
        //:   their original values.
        //: 10 For concern 11, add functors with nothrow wrappers to the array
        //:   in step 8, above. Suppress exception tests for move assignment
        //:   when nothrow wrappers are used, since throwing an exception from
        //:   a wrapper would terminate the program.
        //
        // Testing
        //      function& operator=(const function&);
        //      function& operator=(function&&);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY AND MOVE ASSIGNMENT"
                            "\n========================\n");

        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;
        typedef bool (*AreEqualFuncPtr_t)(const Obj&, const Obj&);

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for one dimension of test

            int               d_line;           // Line number
            Obj               d_function;       // function object to swap
            const char       *d_funcName;       // function object name
            AreEqualFuncPtr_t d_areEqualFunc_p; // comparison function
            bool              d_skipMvExcTest;  // Skip exc test on move
        };

#define TEST_ITEM(F, V)                                                       \
        { L_, Obj(F(V)), #F "(" #V ")", &AreEqualFunctions<F>, false },       \
        { L_, Obj(ntWrap(F(V))), "ntWrap(" #F "(" #V "))",                    \
                &AreEqualFunctions<F>, true }

        TestData dataA[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x2000            ),
            TEST_ITEM(MediumFunctor          , 0x4000            ),
            TEST_ITEM(LargeFunctor           , 0x6000            ),
            TEST_ITEM(NTSmallFunctor         , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x1000            ),
        };

        int dataASize = sizeof(dataA) / sizeof(TestData);

        TestData dataB[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc2       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::sub1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x3000            ),
            TEST_ITEM(MediumFunctor          , 0x5000            ),
            TEST_ITEM(LargeFunctor           , 0x7000            ),
            TEST_ITEM(NTSmallFunctor         , 0x4000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x6000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x1000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x1000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x2000            ),
        };

#undef TEST_ITEM

#define TEST(ALLOC1, ALLOC2) do {                                       \
            if (veryVeryVerbose) printf("\tAllocator types = %s, %s\n", \
                                        #ALLOC1, #ALLOC2);              \
            testAssignment<ALLOC1, ALLOC2>(funcA, funcB, areEqualA,     \
                                           areEqualB, lineA, lineB,     \
                                           skipMvExcTest);              \
        } while (false)

        int dataBSize = sizeof(dataB) / sizeof(TestData);

        for (int i = 0; i < dataASize; ++i) {
            const int lineA             = dataA[i].d_line;
            const Obj& funcA            = dataA[i].d_function;
            const char* funcAName       = dataA[i].d_funcName;
            AreEqualFuncPtr_t areEqualA = dataA[i].d_areEqualFunc_p;
            for (int j = 0; j < dataBSize; ++j) {
                const int lineB             = dataB[j].d_line;
                const Obj& funcB            = dataB[j].d_function;
                const char* funcBName       = dataB[j].d_funcName;
                AreEqualFuncPtr_t areEqualB = dataB[j].d_areEqualFunc_p;
                bool skipMvExcTest          = (dataA[i].d_skipMvExcTest ||
                                               dataB[j].d_skipMvExcTest);

                if (veryVerbose) printf("Assign %s = %s\n",
                                        funcAName, funcBName);

                TEST(bslma::TestAllocator *  , bslma::TestAllocator *  );
                TEST(bsl::allocator<char>    , EmptySTLAllocator<char> );
                TEST(bsl::allocator<char>    , StatefulAllocator2<char>);
                TEST(EmptySTLAllocator<char> , EmptySTLAllocator2<char>);
                TEST(StatefulAllocator<char> , StatefulAllocator2<char>);
                TEST(StatefulAllocator2<char>, StatefulAllocator2<char>);
                TEST(StatefulAllocator<char> , bslma::TestAllocator *  );

            } // End for (each item in dataB)
        } // End for (each item in dataA)

#undef TEST

      } break;

      case 12: {
        // --------------------------------------------------------------------
        // SWAP
        //
        // Concerns:
        //: 1 Swapping two 'function' objects has the same affect as
        //:   constructing the same objects but with the constructor arguments
        //:   to one substituted for the constructor arguments to the other.
        //: 2 Memory consumption, both from allocators and from the global
        //:   heap, is unchanged by the swap operation.
        //: 3 The above concerns apply for each of the different types of
        //:   wrapped functors.
        //: 4 The above concerns apply to 'function's constructed with
        //:   allocator constructor arguments that are pointers to type
        //:   derived from 'bslma::Allocator', 'bsl::allocator'
        //:   instantiations, stateless STL-style allocators, and stateful
        //:   STL-style allocators.  Note that the allocators to both objects
        //:   must compare equal in order for them to be swapped.
        //: 5 The above concerns apply to 'function's constructed with nothrow
        //:   wrappers.
        //: 6 The namespace-scope function, 'bsl::swap' invokes
        //:   'bsl::function<F>::swap' when invoked with two 'function'
        //:   objects.
        //
        // Plan:
        //: 1 For concern 1, create two different 'function' objects, 'a',
        //:   'b', wrapping invocables of type 'FA' and 'FB', respectively.
        //:   Construct another pair of function objects 'a2' and 'b2', using
        //:   the same arguments as 'a' and b', respectively.  Swap 'a' with
        //:   'b'.  Verify that, after the swap, 'a.target_type() ==
        //:   b2.target_type()', '*a.target<FB>() == *b2.target<FB>()',
        //:   'b.target_type() == a2.target_type()', '*b.target<FB>() ==
        //:   *a2.target<FB>()'. Also verify that the allocators of both
        //:   objects are compare equal to their original values and that the
        //:   wrapped functors' allocators (if any) continue to reflect proper
        //:   allocator propagation.  (Since the allocators of 'a' and 'b'
        //:   were the same before the swap, it is unimportant whether the
        //:   allocators are swapped or not.)
        //: 2 For concern 2, check the memory in use by the allocator and by
        //:   the global heap after constructing 'a', 'b', 'a2', and 'b2' and
        //:   verify that the amount of memory in use after the swap is the
        //:   same as before the swap.
        //: 3 For concerns 3 and 4, package the above steps into a function
        //:   template, 'testSwap', which is instantiated on an allocator type
        //:   and takes two 'function' object arguments as well as comparison
        //:   functions for each functor type. The comparison function are
        //:   used to test if the targets compare equal and to assert that
        //:   they have proper allocator-propagation.  'testSwap' copies the
        //:   input arguments using the specified allocator type before
        //:   swapping them.  Create two arrays where each array element
        //:   contains a function object and a pointer to a function that can
        //:   compare that function for equality.  Each function object is
        //:   constructed with a different invocable and the comparison
        //:   function is instantiated with the type of that invocable.  Loop
        //:   through the 3-way cross product of the two arrays and the
        //:   different allocator categories and call 'testSwap' to perform
        //:   the test.
        //: 4 For concern 5, add a nothrow-wrapped version of each 'function'
        //:   in the arrays described in step 3. Suppress exception tests when
        //:   nothrow wrappers are used, since throwing an exception from a
        //:   wrapper would terminate the program.
        //: 5 For concern 6, reverse the call to member 'swap' in step 1 by
        //:   using free function 'bsl::swap'.
        //
        // Testing
        //      void swap(function& other);
        //      void bsl::swap(function<RET(ARGS...)>& a,
        //                     function<RET(ARGS...)>& b);
        // --------------------------------------------------------------------

        if (verbose) printf("\nSWAP"
                            "\n====\n");

        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;
        typedef bool (*AreEqualFuncPtr_t)(const Obj&, const Obj&);

        // Null function pointers
        static const SimpleFuncPtr_t    nullFuncPtr    = 0;
        static const SimpleMemFuncPtr_t nullMemFuncPtr = 0;

        struct TestData {
            // Data for one dimension of test

            int               d_line;           // Line number
            Obj               d_function;       // function object to swap
            const char       *d_funcName;       // function object name
            AreEqualFuncPtr_t d_areEqualFunc_p; // comparison function
            bool              d_skipExcTest;    // skip exception test
        };

#define TEST_ITEM(F, V)                                                 \
        { L_, Obj(F(V)), #F "(" #V ")", &AreEqualFunctions<F>, false }, \
        { L_, Obj(ntWrap(F(V))), "ntWrap(" #F "(" #V "))",              \
                &AreEqualFunctions<F>, true }

        TestData dataA[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc        ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::add1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x2000            ),
            TEST_ITEM(MediumFunctor          , 0x4000            ),
            TEST_ITEM(LargeFunctor           , 0x6000            ),
            TEST_ITEM(NTSmallFunctor         , 0x3000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x7000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x1000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x1000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x2000            ),
        };

        int dataASize = sizeof(dataA) / sizeof(TestData);

        TestData dataB[] = {
            TEST_ITEM(SimpleFuncPtr_t        , nullFuncPtr       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , nullMemFuncPtr    ),
            TEST_ITEM(SimpleFuncPtr_t        , simpleFunc2       ),
            TEST_ITEM(SimpleMemFuncPtr_t     , &IntWrapper::sub1 ),
            TEST_ITEM(EmptyFunctor           , 0                 ),
            TEST_ITEM(SmallFunctor           , 0x3000            ),
            TEST_ITEM(MediumFunctor          , 0x5000            ),
            TEST_ITEM(LargeFunctor           , 0x7000            ),
            TEST_ITEM(NTSmallFunctor         , 0x4000            ),
            TEST_ITEM(ThrowingSmallFunctor   , 0x6000            ),
            TEST_ITEM(ThrowingEmptyFunctor   , 0                 ),
            TEST_ITEM(SmallFunctorWithAlloc  , 0x2000            ),
            TEST_ITEM(NTSmallFunctorWithAlloc, 0x2000            ),
            TEST_ITEM(LargeFunctorWithAlloc  , 0x1000            ),
        };

#undef TEST_ITEM

#define TEST(ALLOC) do {                                                      \
         if (veryVeryVerbose) printf("\tAllocator type = %s\n", #ALLOC);      \
         testSwap<ALLOC>(funcA, funcB, areEqualA, areEqualB, lineA, lineB,    \
                         skipExcTest);                                        \
     } while (false)

        int dataBSize = sizeof(dataB) / sizeof(TestData);

        for (int i = 0; i < dataASize; ++i) {
            const int lineA             = dataA[i].d_line;
            const Obj& funcA            = dataA[i].d_function;
            const char* funcAName       = dataA[i].d_funcName;
            AreEqualFuncPtr_t areEqualA = dataA[i].d_areEqualFunc_p;
            for (int j = 0; j < dataBSize; ++j) {
                const int lineB             = dataB[j].d_line;
                const Obj& funcB            = dataB[j].d_function;
                const char* funcBName       = dataB[j].d_funcName;
                AreEqualFuncPtr_t areEqualB = dataB[j].d_areEqualFunc_p;
                bool skipExcTest            = (dataA[i].d_skipExcTest ||
                                               dataB[j].d_skipExcTest);

                if (veryVerbose) printf("swap(%s, %s)\n",funcAName,funcBName);

                TEST(bslma::TestAllocator *  );
                TEST(bsl::allocator<char>    );
                TEST(EmptySTLAllocator<char> );
                TEST(StatefulAllocator<char> );
                TEST(StatefulAllocator2<char>);

            } // end for (each item in dataB)
        } // end for (each item in dataA)

#undef TEST

      } break;

      case 11: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTORS
        //
        // Concerns:
        //: 1 Moving an empty 'function' object (with either the move
        //:   constructor or allocator-extended move constructor) yields an
        //:   empty destination 'function' object.
        //: 2 Moving a non-empty 'function' yields a non-empty destination
        //:   'function' and an empty source object.
        //: 3 The 'target_type' and 'target' attributes of the source
        //:   before the move match the corresponding attributes of the
        //:   destination after the move.
        //: 4 If the source 'function' could be invoked before the move, then
        //:   the destination can be invoked and will yield the same results.
        //: 5 If the move constructor is invoked (without an allocator), then
        //:   the source and destination after the move will use the original
        //:   source allocator or a copy of it.
        //: 6 If the allocator-extended move constructor is invoked
        //:   then the destination will use the specified allocator.
        //: 7 If 'FUNC' takes a 'bslma::Allocator*', then the wrapped functor
        //:   will use the same allocator as the 'function' object (i.e., the
        //:   allocator is propagated).
        //: 8 The net memory consumption of the source and destination after
        //:   the move is equal to the memory consumption of the source before
        //:   the move plus up to one block (if the source allocator is
        //:   type-erased on the heap).
        //: 9 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes with or without throwing move constructors.
        //: 10 The above concerns apply to allocator arguments that are
        //:   pointers to type derived from 'bslma::Allocator',
        //:   'bsl::allocator' instantiations, stateless STL-style allocators,
        //:   and stateful STL-style allocators.  The original and copy can
        //:   use different allocators, in the case of the extended move
        //:   constructor.
        //: 11 If the functor move-constructor or the allocator throws an
        //:   exception, then no resources are leaked.
        //: 12 The above concerns apply if the original 'function' object is
        //:   constructed with a nothrow wrapper.  Specifically, if the
        //:   nothrow wrapper results in an otherwise ineligible functor
        //:   becoming eligible for the small object optimization, then the
        //:   copy will also use the small object optimization.
        //
        // Plan:
        //: 1 For concern 1 move an empty 'function' using both the move
        //:   constructor and extended move constructor and verify that the
        //:   destination in each case is an empty 'function'.
        //: 2 For concern 2 move a non-empty function and verify that the
        //:   destination is not empty and that the source becomes empty.
        //: 3 For concern 3, for all 'function' objects moved in this test, //
        //:   create a copy of the source object before the move then preform
        //:   the move construction.  Verify that the 'target_type' of the
        //:   copy of the source compares equal to the 'target_type' of the
        //:   destination.  For non-empty 'function' objects, also verify that
        //:   the 'target' attributes of the copy of the source and the
        //:   destination point to objects that compare equal to each other.
        //: 4 For concern 4, invoke each non-empty destination and verify that
        //:   the result is the same as invoking a copy of the source before
        //:   the move.
        //: 5 For concern 5, move-construct a 'function' and verify that
        //:   'allocator()' invoked on both the source and the destination
        //:   after the move returns same the value as invoking 'allocator()'
        //:   on the source before the move or a clone of that value.
        //: 6 For concern 6, use the extended move constructor and verify that
        //:   the allocator for the destination matches the allocator passed
        //:   into the constructor and that the source allocator is unchanged
        //:   before and after the move operation.
        //: 7 For concern 7, perform the above steps using a small and a large
        //:   'FUNC' type that take a 'bslma::Allocator*' as well as with
        //:   functors that don't take a 'bslma::Allocator*'.  In the former
        //:   case, verify that the functor wrapped within the
        //:   newly-constructed 'function' object uses the same allocator as
        //:   the newly-created object itself.
        //: 8 For concern 8, measure the memory used to construct the source
        //:   object and the additional memory consumed in move-constructing
        //:   the destination object.  If the source allocator fits in
        //:   the small-object buffer, verify that the net memory consumption
        //:   did not change during the move; otherwise, verify that the net
        //:   memory consumption increased by one block.
        //: 9 For concerns 9 and 10, package all of the previous plan steps
        //:   into a function template 'testMoveCtor', instantiated with a
        //:   functor and allocator.  This test template will create an
        //:   original 'function' object using the passed-in functor and
        //:   allocator and -- using a fresh copy each time -- move it with
        //:   the move constructor and with several invocations of the
        //:   extended move constructor, passing in allocators of all of the
        //:   types described in concern 10.  Invoke 'testMoveCtor' with many
        //:   combinations of functor and allocator types so that every
        //:   category combination is represented.
        //: 10 For concern 11, performed the above steps within the exception
        //:   test framework and verify that, on exception, memory allocation
        //:   does not change and no functor objects are leaked.
        //: 11 For concern 12, repeat the above steps with a representative
        //:   set of 'FUNC' types. The test for memory allocation (step 8)
        //:   will have the effect of proving that the the original and the
        //:   moved-to object either both use the small-object optimization or
        //:   neither uses it.
        //
        // Testing:
        //      function(function&& other)
        //      function(allocator_arg_t, const ALLOC& alloc, function&& other)
        // --------------------------------------------------------------------

        if (verbose) printf("\nMOVE CONSTRUCTORS"
                            "\n=================\n");

        typedef bsls::AlignmentUtil::MaxAlignedType MaxAlignedType;

        // Test that the small-object buffer meets the minimum size constraints.

        LOOP3_ASSERT(sizeof(SmallObjectBuffer), sizeof(void*),
                     sizeof(MaxAlignedType),
                     sizeof(SmallObjectBuffer) >= 6 * sizeof(void*));

        // Test our assumptions about the platform specific small-object buffer 
        // size.  Note that this test is platform specific.

#if BSLS_PLATFORM_OS_DARWIN && BSLS_PLATFORM_CPU_32_BIT
        // Max aligned type is 16 bytes on OSX, clang, 32 bit builds.

        LOOP3_ASSERT(sizeof(SmallObjectBuffer), sizeof(void*),
                     sizeof(MaxAlignedType),
                     sizeof(SmallObjectBuffer) == 8 * sizeof(void*));
#else
        LOOP3_ASSERT(sizeof(SmallObjectBuffer), sizeof(void*),
                     sizeof(MaxAlignedType),
                     sizeof(SmallObjectBuffer) == 6 * sizeof(void*));
#endif
        LOOP4_ASSERT(sizeof(Obj),
                     sizeof(void*),
                     sizeof(SmallObjectBuffer),
                     sizeof(MaxAlignedType),
                     sizeof(Obj) == sizeof(SmallObjectBuffer)
                                    + 4 * sizeof(void*));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Null functors
        int (*nullFuncPtr)(IntWrapper, int) = 0;
        int (IntWrapper::*nullMemFuncPtr)(int) const = 0;

#define TEST(A, f) testMoveCtor<A>(f, #A)

        if (veryVerbose) printf("FUNC is nullFuncPtr\n");
        TEST(bslma::TestAllocator *,   nullFuncPtr          );
        TEST(bsl::allocator<char>,     nullFuncPtr          );
        TEST(EmptySTLAllocator<char>,  nullFuncPtr          );
        TEST(StatefulAllocator<char>,  nullFuncPtr          );
        TEST(StatefulAllocator2<char>, nullFuncPtr          );

        if (veryVerbose) printf("FUNC is nullMemFuncPtr\n");
        TEST(bslma::TestAllocator *,   nullMemFuncPtr       );
        TEST(bsl::allocator<char>,     nullMemFuncPtr       );
        TEST(EmptySTLAllocator<char>,  nullMemFuncPtr       );
        TEST(StatefulAllocator<char>,  nullMemFuncPtr       );
        TEST(StatefulAllocator2<char>, nullMemFuncPtr       );

        if (veryVerbose) printf("FUNC is simpleFunc\n");
        TEST(bslma::TestAllocator *,   simpleFunc           );
        TEST(bsl::allocator<char>,     simpleFunc           );
        TEST(EmptySTLAllocator<char>,  simpleFunc           );
        TEST(StatefulAllocator<char>,  simpleFunc           );
        TEST(StatefulAllocator2<char>, simpleFunc           );

        if (veryVerbose) printf("FUNC is &IntWrapper::add1\n");
        TEST(bslma::TestAllocator *,   &IntWrapper::add1    );
        TEST(bsl::allocator<char>,     &IntWrapper::add1    );
        TEST(EmptySTLAllocator<char>,  &IntWrapper::add1    );
        TEST(StatefulAllocator<char>,  &IntWrapper::add1    );
        TEST(StatefulAllocator2<char>, &IntWrapper::add1    );

        if (veryVerbose) printf("FUNC is EmptyFunctor()\n");
        TEST(bslma::TestAllocator *,   EmptyFunctor()       );
        TEST(bsl::allocator<char>,     EmptyFunctor()       );
        TEST(EmptySTLAllocator<char>,  EmptyFunctor()       );
        TEST(StatefulAllocator<char>,  EmptyFunctor()       );
        TEST(StatefulAllocator2<char>, EmptyFunctor()       );

        if (veryVerbose) printf("FUNC is SmallFunctor(0x2000)\n");
        TEST(bslma::TestAllocator *,   SmallFunctor(0x2000) );
        TEST(bsl::allocator<char>,     SmallFunctor(0x2000) );
        TEST(EmptySTLAllocator<char>,  SmallFunctor(0x2000) );
        TEST(StatefulAllocator<char>,  SmallFunctor(0x2000) );
        TEST(StatefulAllocator2<char>, SmallFunctor(0x2000) );

        if (veryVerbose) printf("FUNC is MediumFunctor(0x4000)\n");
        TEST(bslma::TestAllocator *,   MediumFunctor(0x4000));
        TEST(bsl::allocator<char>,     MediumFunctor(0x4000));
        TEST(EmptySTLAllocator<char>,  MediumFunctor(0x4000));
        TEST(StatefulAllocator<char>,  MediumFunctor(0x4000));
        TEST(StatefulAllocator2<char>, MediumFunctor(0x4000));

        if (veryVerbose) printf("FUNC is LargeFunctor(0x6000)\n");
        TEST(bslma::TestAllocator *,   LargeFunctor(0x6000) );
        TEST(bsl::allocator<char>,     LargeFunctor(0x6000) );
        TEST(EmptySTLAllocator<char>,  LargeFunctor(0x6000) );
        TEST(StatefulAllocator<char>,  LargeFunctor(0x6000) );
        TEST(StatefulAllocator2<char>, LargeFunctor(0x6000) );

        if (veryVerbose) printf("FUNC is NTSmallFunctor(0x3000)\n");
        TEST(bslma::TestAllocator *  , NTSmallFunctor(0x3000));
        TEST(bsl::allocator<char>    , NTSmallFunctor(0x3000));
        TEST(EmptySTLAllocator<char> , NTSmallFunctor(0x3000));
        TEST(StatefulAllocator<char> , NTSmallFunctor(0x3000));
        TEST(StatefulAllocator2<char>, NTSmallFunctor(0x3000));

        if (veryVerbose) printf("FUNC is ThrowingSmallFunctor(0x7000)\n");
        TEST(bslma::TestAllocator *  , ThrowingSmallFunctor(0x7000));
        TEST(bsl::allocator<char>    , ThrowingSmallFunctor(0x7000));
        TEST(EmptySTLAllocator<char> , ThrowingSmallFunctor(0x7000));
        TEST(StatefulAllocator<char> , ThrowingSmallFunctor(0x7000));
        TEST(StatefulAllocator2<char>, ThrowingSmallFunctor(0x7000));

        if (veryVerbose) printf("FUNC is ThrowingEmptyFunctor()\n");
        TEST(bslma::TestAllocator *  , ThrowingEmptyFunctor()     );
        TEST(StatefulAllocator2<char> , ThrowingEmptyFunctor()     );

        bslma::TestAllocator xa;

        if (veryVerbose) printf("FUNC is SmallFunctorWithAlloc(0)\n");
        TEST(bslma::TestAllocator *  , SmallFunctorWithAlloc(0, &xa));
        TEST(bsl::allocator<char>    , SmallFunctorWithAlloc(0, &xa));
        TEST(EmptySTLAllocator<char> , SmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator<char> , SmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator2<char>, SmallFunctorWithAlloc(0, &xa));

        if (veryVerbose) printf("FUNC is NTSmallFunctorWithAlloc(0)\n");
        TEST(bslma::TestAllocator *  , NTSmallFunctorWithAlloc(0, &xa));
        TEST(bsl::allocator<char>    , NTSmallFunctorWithAlloc(0, &xa));
        TEST(EmptySTLAllocator<char> , NTSmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator<char> , NTSmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator2<char>, NTSmallFunctorWithAlloc(0, &xa));

        if (veryVerbose) printf("FUNC is LargeFunctorWithAlloc(0)\n");
        TEST(bslma::TestAllocator *  , LargeFunctorWithAlloc(0, &xa));
        TEST(bsl::allocator<char>    , LargeFunctorWithAlloc(0, &xa));
        TEST(EmptySTLAllocator<char> , LargeFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator<char> , LargeFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator2<char>, LargeFunctorWithAlloc(0, &xa));

        if (veryVerbose) printf("FUNC is a nothrow wrapper\n");
        TEST(StatefulAllocator2<char>, ntWrap(nullFuncPtr)          );
        TEST(bslma::TestAllocator *,   ntWrap(&simpleFunc)          );
        TEST(StatefulAllocator<char>,  ntWrap(EmptyFunctor())       );
        TEST(bsl::allocator<char>,     ntWrap(SmallFunctor(0x2000)) );
        TEST(StatefulAllocator<char>,  ntWrap(SmallFunctor(0x2000)) );
        TEST(EmptySTLAllocator<char>,  ntWrap(MediumFunctor(0x4000)));
        TEST(StatefulAllocator2<char>, ntWrap(MediumFunctor(0x4000)));
        TEST(bslma::TestAllocator *,   ntWrap(LargeFunctor(0x6000)) );
        TEST(bslma::TestAllocator *  , ntWrap(NTSmallFunctor(0x3000)));
        TEST(bslma::TestAllocator *  , ntWrap(ThrowingSmallFunctor(0x7000)));
        TEST(bslma::TestAllocator *  , ntWrap(ThrowingEmptyFunctor())     );
        TEST(bsl::allocator<char>    , ntWrap(SmallFunctorWithAlloc(0, &xa)));
        TEST(StatefulAllocator<char> , ntWrap(LargeFunctorWithAlloc(0, &xa)));

#undef TEST

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

      } break;

      case 10: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTORS
        //
        // Concerns:
        //: 1 Copying an empty 'function' object (with either the copy
        //:   constructor or allocator-extended copy constructor) yields an
        //:   empty 'function' object.
        //: 2 Copying a non-empty 'function' yields a non-empty 'function'
        //:   object.
        //: 3 The 'target_type' and 'target' attributes of the original
        //:   'function' match the corresponding attributes of the copy.
        //: 4 If the original 'function' can be invoked, then its copy can
        //:   also be invoked and will yield the same results.
        //: 5 If the copy constructor is invoked (without an allocator), then
        //:   the copy will use the value of
        //:   'bslma::Default::defaultAllocator()' at the time of the copy.
        //: 6 If the allocator-extended copy constructor is invoked
        //:   then the copy will use the specified allocator.
        //: 7 If 'FUNC' takes a 'bslma::Allocator*', then the 'function'
        //:   allocator is propagated to the wrapped functor.
        //: 8 The memory allocated by this constructor is the same as if the
        //:   copy were created like the original, except using the specified
        //:   allocator (or default allocator if none specified).
        //: 9 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes with or without throwing move constructors.
        //: 10 The above concerns apply to allocators arguments that are
        //:   pointers to type derived from 'bslma::Allocator',
        //:   'bsl::allocator' instantiations, stateless STL-style allocators,
        //:   and stateful STL-style allocators.  The
        //:   original and copy can use different allocators.
        //: 11 If the functor copy-constructor or the allocator throws an
        //:   exception, then no resources are leaked.
        //: 12 The above concerns apply if the original 'function' object is
        //:   constructed with a nothrow wrapper.  Specifically, if the
        //:   nothrow wrapper results in an otherwise ineligible functor
        //:   becoming eligible for the small object optimization, then the
        //:   copy will also use the small object optimization.
        //
        // Plan:
        //: 1 For concern 1 copy an empty 'function' using both the copy
        //:   constructor and extended copy constructor and verify that the
        //:   result in each case is an empty 'function'.
        //: 2 For concern 2 copy a non-empty function and verify that the
        //:   result is not empty.
        //: 3 For concern 3, for all 'function' objects copied in this test,
        //:   verify that the 'target_type' of the original compares equal to
        //:   the 'target_type' of the copy.  For non-empty 'function'
        //:   objects, also verify that the 'target' attributes of the
        //:   original and the copy point to objects that compare equal to
        //:   each other.
        //: 4 For concern 4, invoke each non-empty copy and verify that the
        //:   result is the same as invoking the original.
        //: 5 For concern 5, copy-construct a 'function' and verify that
        //:   invoking 'allocator()' on the copy returns
        //:   'bslma::Default::defaultAllocator()'.  Change the default
        //:   allocator temporarily and verify that 'allocator()' still
        //:   returns the default allocator at the time of the copy.
        //: 6 For concern 6, use the extended copy constructor and verify that
        //:   the allocator for the copy matches the allocator passed into the
        //:   constructor (as was done for the previous test case).
        //: 7 For concern 7, perform the above steps using a small and a large
        //:   'FUNC' type that take a 'bslma::Allocator*' as well as with
        //:   functors that don't take a 'bslma::Allocator*.  In the former
        //:   case, verify that the functor wrapped within the
        //:   newly-constructed 'function' object uses the same allocator as
        //:   the newly-created object itself.
        //: 8 For concern 8, construct a function object 'f1' using a specific
        //:   'func' argument and allocator 'a1'.  Construct a second function
        //:   object 'f1' using the same 'func' argument and an allocator
        //:   'a2'.  Using the extended copy constructor, create a copy of
        //:   'f1' using allocator 'a2'.  Verify that the memory allocations
        //:   during this construction match those in constructing 'f2'.
        //: 9 For concerns 9 and 10, package all of the previous plan steps
        //:   into a function template 'testCopyCtor', instantiated with a
        //:   functor and allocator.  This test template will create an
        //:   original 'function' object using the passed-in functor and
        //:   allocator and copy it with the copy constructor and with several
        //:   invocations of the extended copy constructor, passing in
        //:   allocators of all of the types described in concern 10.  Invoke
        //:   'testCopyCtor' with many combinations of functor and allocator
        //:   types so that every category combination is represented.
        //: 10 For concern 11, performed the above steps within the exception
        //:   test framework and verify that, on exception, memory allocation
        //:   does not change and no functor objects are leaked.
        //: 11 For concern 12, repeat the above steps with a representative
        //:   set of 'FUNC' types. The test for memory allocation (step 8)
        //:   will have the effect of proving that the the original and the
        //:   moved-to object either both use the small-object optimization or
        //:   neither uses it.
        //
        // Testing:
        //      function(const function& other);
        //      function(allocator_arg_t, const ALLOC& alloc,
        //               const function& other);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTORS"
                            "\n=================\n");

        // Null functors
        int (*nullFuncPtr)(IntWrapper, int) = 0;
        int (IntWrapper::*nullMemFuncPtr)(int) const = 0;

#define TEST(A, f) testCopyCtor<A>(f, #A)

        if (veryVerbose) printf("FUNC is nullFuncPtr\n");
        TEST(bslma::TestAllocator *,   nullFuncPtr          );
        TEST(bsl::allocator<char>,     nullFuncPtr          );
        TEST(EmptySTLAllocator<char>,  nullFuncPtr          );
        TEST(StatefulAllocator<char>,  nullFuncPtr          );
        TEST(StatefulAllocator2<char>, nullFuncPtr          );

        if (veryVerbose) printf("FUNC is nullMemFuncPtr\n");
        TEST(bslma::TestAllocator *,   nullMemFuncPtr       );
        TEST(bsl::allocator<char>,     nullMemFuncPtr       );
        TEST(EmptySTLAllocator<char>,  nullMemFuncPtr       );
        TEST(StatefulAllocator<char>,  nullMemFuncPtr       );
        TEST(StatefulAllocator2<char>, nullMemFuncPtr       );

        if (veryVerbose) printf("FUNC is simpleFunc\n");
        TEST(bslma::TestAllocator *,   simpleFunc           );
        TEST(bsl::allocator<char>,     simpleFunc           );
        TEST(EmptySTLAllocator<char>,  simpleFunc           );
        TEST(StatefulAllocator<char>,  simpleFunc           );
        TEST(StatefulAllocator2<char>, simpleFunc           );

        if (veryVerbose) printf("FUNC is &IntWrapper::add1\n");
        TEST(bslma::TestAllocator *,   &IntWrapper::add1    );
        TEST(bsl::allocator<char>,     &IntWrapper::add1    );
        TEST(EmptySTLAllocator<char>,  &IntWrapper::add1    );
        TEST(StatefulAllocator<char>,  &IntWrapper::add1    );
        TEST(StatefulAllocator2<char>, &IntWrapper::add1    );

        if (veryVerbose) printf("FUNC is EmptyFunctor()\n");
        TEST(bslma::TestAllocator *,   EmptyFunctor()       );
        TEST(bsl::allocator<char>,     EmptyFunctor()       );
        TEST(EmptySTLAllocator<char>,  EmptyFunctor()       );
        TEST(StatefulAllocator<char>,  EmptyFunctor()       );
        TEST(StatefulAllocator2<char>, EmptyFunctor()       );

        if (veryVerbose) printf("FUNC is SmallFunctor(0x2000)\n");
        TEST(bslma::TestAllocator *,   SmallFunctor(0x2000) );
        TEST(bsl::allocator<char>,     SmallFunctor(0x2000) );
        TEST(EmptySTLAllocator<char>,  SmallFunctor(0x2000) );
        TEST(StatefulAllocator<char>,  SmallFunctor(0x2000) );
        TEST(StatefulAllocator2<char>, SmallFunctor(0x2000) );

        if (veryVerbose) printf("FUNC is MediumFunctor(0x4000)\n");
        TEST(bslma::TestAllocator *,   MediumFunctor(0x4000));
        TEST(bsl::allocator<char>,     MediumFunctor(0x4000));
        TEST(EmptySTLAllocator<char>,  MediumFunctor(0x4000));
        TEST(StatefulAllocator<char>,  MediumFunctor(0x4000));
        TEST(StatefulAllocator2<char>, MediumFunctor(0x4000));

        if (veryVerbose) printf("FUNC is LargeFunctor(0x6000)\n");
        TEST(bslma::TestAllocator *,   LargeFunctor(0x6000) );
        TEST(bsl::allocator<char>,     LargeFunctor(0x6000) );
        TEST(EmptySTLAllocator<char>,  LargeFunctor(0x6000) );
        TEST(StatefulAllocator<char>,  LargeFunctor(0x6000) );
        TEST(StatefulAllocator2<char>, LargeFunctor(0x6000) );

        if (veryVerbose) printf("FUNC is NTSmallFunctor(0x3000)\n");
        TEST(bslma::TestAllocator *  , NTSmallFunctor(0x3000));
        TEST(bsl::allocator<char>    , NTSmallFunctor(0x3000));
        TEST(EmptySTLAllocator<char> , NTSmallFunctor(0x3000));
        TEST(StatefulAllocator<char> , NTSmallFunctor(0x3000));
        TEST(StatefulAllocator2<char>, NTSmallFunctor(0x3000));

        if (veryVerbose) printf("FUNC is ThrowingSmallFunctor(0x7000)\n");
        TEST(bslma::TestAllocator *  , ThrowingSmallFunctor(0x7000));
        TEST(bsl::allocator<char>    , ThrowingSmallFunctor(0x7000));
        TEST(EmptySTLAllocator<char> , ThrowingSmallFunctor(0x7000));
        TEST(StatefulAllocator<char> , ThrowingSmallFunctor(0x7000));
        TEST(StatefulAllocator2<char>, ThrowingSmallFunctor(0x7000));

        if (veryVerbose) printf("FUNC is ThrowingEmptyFunctor()\n");
        TEST(bslma::TestAllocator *  , ThrowingEmptyFunctor()     );
        TEST(StatefulAllocator2<char>, ThrowingEmptyFunctor()     );

        bslma::TestAllocator xa;

        if (veryVerbose) printf("FUNC is SmallFunctorWithAlloc(0)\n");
        TEST(bslma::TestAllocator *  , SmallFunctorWithAlloc(0, &xa));
        TEST(bsl::allocator<char>    , SmallFunctorWithAlloc(0, &xa));
        TEST(EmptySTLAllocator<char> , SmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator<char> , SmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator2<char>, SmallFunctorWithAlloc(0, &xa));

        if (veryVerbose) printf("FUNC is NTSmallFunctorWithAlloc(0)\n");
        TEST(bslma::TestAllocator *  , NTSmallFunctorWithAlloc(0, &xa));
        TEST(bsl::allocator<char>    , NTSmallFunctorWithAlloc(0, &xa));
        TEST(EmptySTLAllocator<char> , NTSmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator<char> , NTSmallFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator2<char>, NTSmallFunctorWithAlloc(0, &xa));

        if (veryVerbose) printf("FUNC is LargeFunctorWithAlloc(0)\n");
        TEST(bslma::TestAllocator *  , LargeFunctorWithAlloc(0, &xa));
        TEST(bsl::allocator<char>    , LargeFunctorWithAlloc(0, &xa));
        TEST(EmptySTLAllocator<char> , LargeFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator<char> , LargeFunctorWithAlloc(0, &xa));
        TEST(StatefulAllocator2<char>, LargeFunctorWithAlloc(0, &xa));

        if (veryVerbose) printf("FUNC is a nothrow wrapper\n");
        TEST(StatefulAllocator2<char>, ntWrap(nullFuncPtr)          );
        TEST(bslma::TestAllocator *,   ntWrap(&simpleFunc)          );
        TEST(StatefulAllocator<char>,  ntWrap(EmptyFunctor())       );
        TEST(bsl::allocator<char>,     ntWrap(SmallFunctor(0x2000)) );
        TEST(StatefulAllocator<char>,  ntWrap(SmallFunctor(0x2000)) );
        TEST(EmptySTLAllocator<char>,  ntWrap(MediumFunctor(0x4000)));
        TEST(StatefulAllocator2<char>, ntWrap(MediumFunctor(0x4000)));
        TEST(bslma::TestAllocator *,   ntWrap(LargeFunctor(0x6000)) );
        TEST(bslma::TestAllocator *  , ntWrap(NTSmallFunctor(0x3000)));
        TEST(bslma::TestAllocator *  , ntWrap(ThrowingSmallFunctor(0x7000)));
        TEST(bslma::TestAllocator *  , ntWrap(ThrowingEmptyFunctor())     );
        TEST(bsl::allocator<char>    , ntWrap(SmallFunctorWithAlloc(0, &xa)));
        TEST(StatefulAllocator<char> , ntWrap(LargeFunctorWithAlloc(0, &xa)));

#undef TEST

      } break;

      case 9: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR 'function(allocator_arg_t,const ALLOC& alloc,FUNC func)'
        //
        // Concerns:
        //: 1 Constructing a 'function' using this constructor yields an empty
        //:   'function' object if 'func' is a null pointer to function or
        //:   null pointer to member function.
        //: 2 Constructing a 'function' using this constructor yields an
        //:   invocable 'function' if 'func' is not a null pointer.  Note that
        //:   invocation is only to ensure that that the function was
        //:   constructed successfully.  It is not necessary to thoroughly test
        //:   all argument-list combinations.
        //: 3 The 'target_type()' accessor will return 'type_id(FUNC)' and the
        //:   'target<FUNC>()' accessor will return a pointer to a functor
        //:   that compares equal to 'func'.
        //: 4 If 'alloc' is a pointer to a 'bslma::Allocator' object, then the
        //:   'allocator' accessor will return that pointer.
        //: 5 If 'alloc' is a 'bsl::allocator' object, then the 'allocator'
        //:   accessor will return 'alloc.mechanism()'.
        //: 6 If 'alloc' is an STL-style allocator with no state, then the
        //:   'allocator' accessor will return a pointer to an object of type
        //:   'bslma::AllocatorAdaptor<ALLOC>'.  That same pointer will be
        //:   returned by any 'function' created with that allocator type.
        //: 7 If 'alloc' is an STL-style allocator with state, then the
        //:   'allocator' accessor will return a pointer to an object of type
        //:   'bslma::AllocatorAdaptor<ALLOC>' which wraps a copy of 'alloc'.
        //: 8 If 'alloc' is other than an STL-style allocator with state and
        //:   'FUNC' is eligible for the small object optimization, no
        //:   memory is allocated by this constructor.
        //: 9 If 'alloc' is other than an STL-style allocator with state and
        //:   'FUNC' is not eligible for the small object optimization, one
        //:   block of memory of sufficient size to hold 'FUNC' is allocated
        //:   from the allocator by this constructor.
        //: 10 If 'alloc' is an STL-style allocator with state, then one block
        //:   of memory is allocated from 'alloc' itself.
        //: 11 In step 10, if 'FUNC' is eligible for the small object
        //:   optimization, then the allocated memory is only large enough to
        //:   hold the allocator adaptor.
        //: 12 In step 10, if 'FUNC' is not eligible for the small
        //:   object optimization, then the allocated memory is large enough
        //:   to hold both 'func' and the allocator adaptor.
        //: 13 If memory is allocated, the destructor frees it.
        //: 14 If 'FUNC' takes a 'bslma::Allocator*', then the 'function'
        //:   allocator is propagated to the wrapped functor.
        //: 15 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes, with or without throwing move constructors.
        //: 16 If memory allocation or functor construction fails with an
        //:   exception, then no resources are leaked.
        //: 17 If 'FUNC' is wrapped in a nothrow wrapper, all of the above
        //:   concerns hold. For concerns that hinge on the presence or
        //:   absence of the small object optimization, any sufficiently-small
        //:   'FUNC' is eligible for the optimization when wrapped ina nothrow
        //:   wrapper, even if it would otherwise throw.
        //
        // Plan:
        //: 1 For concern 1, construct 'function' objects using a null pointer
        //:   to function and a null pointer to member function and verify
        //:   that each returns false when converted to a Boolean value.
        //: 2 For concern 2, invoke every non-empty 'function' constructed by
        //:   this test and verify that it produces the expected results.
        //: 3 For concern 3, verify, for each 'function' constructed, that
        //:   'tareget_type' and 'target<FUNC>' return the expected values.
        //: 4 For concern 4, construct a 'function' object with the address of
        //:   a 'bslma:TestAllocator'.  Verify that 'allocator' returns the
        //:   address of the test allocator.
        //: 5 For concern 5, construct a 'bsl::allocator' wrapping a test
        //:   allocator.  Construct a 'function' object with the
        //:   'bsl::allocator' object.  Verify that 'allocator' returns the
        //:   address of the test allocator (i.e., the 'mechanism()' of the
        //:   'bsl::allocator'.
        //: 6 For concern 6, define a stateless STL-style allocator class and
        //:   use an instance of that class to construct a 'function' object.
        //:   Verify that 'allocator' returns a pointer that can dynamically
        //:   cast to a 'bslma::AllocatorAdaptor' wrapping the STL-style
        //:   allocator.  Verify that multiple 'function' objects instantiated
        //:   with multiple instantiations of the same STL-style allocator
        //:   return the same result from calling the 'allocator' method.
        //: 7 For concern 7, define a stateful STL-style allocator class and
        //:   use an instance of that class to construct a 'function' object.
        //:   Verify that 'allocator' returns a pointer that can dynamically
        //:   cast to a 'bslma::AllocatorAdaptor' wrapping the STL-style
        //:   allocator and that the allocator wrapped by the adaptor is equal
        //:   to the original STL-style allocator.
        //: 8 For concern 8, test the results of steps 2-4 to verify that when
        //:   'func' is eligible for the small object optimization, no memory
        //:   is allocated either from the global allocator or from the
        //:   allocator used to construct the 'function' object.
        //: 9 For concern 9, test the results of steps 2-4 to verify that when
        //:   'func' is not eligible the small object optimization, one block
        //:   of memory of sufficient size to hold 'FUNC' is allocated from
        //:   the allocator.
        //: 10 For concern 10, perform step 7 using stateful STL allocators
        //:   and verify that exactly one block was allocated
        //:   from the allocator used to construct the 'function' and that no
        //:   memory was allocated from the global allocator.
        //: 11 For concern 11, look at the memory allocation from step 10 and
        //:   verify that, when 'FUNC' is eligible for the small object
        //:   optimization, that the allocated memory is only large enough to
        //:   hold the allocator adaptor.
        //: 12 For concern 12, look at the memory allocation from step 10 and
        //:   verify that, when 'FUNC' is not eligible for the small object
        //:   optimization, that the allocated memory is large enough to hold
        //:   both 'FUNC' and the allocator adaptor.
        //: 13 For concern 13, check at the end of each step, when the
        //:   'function' object is destroyed, that all memory is returned to
        //:   the allocator.
        //: 14 For concern 14, perform the above steps using a small and a
        //:   large 'FUNC' type that take a 'bslma::Allocator*' as well as
        //:   with functors that don't take a 'bslma::Allocator*.  After
        //:   construction, verify that the wrapped functor uses the same
        //:   allocator as the 'function' object in the former case.
        //: 15 For concern 15, wrap the common parts of the above steps into a
        //:   function template, 'testFuncWithAlloc', which takes 'ALLOC' and
        //:   'FUNC' template parameters.  Instantiate this template with each
        //:   of the allocator types described in the previous step in
        //:   combination with each of the following invokable types: pointer
        //:   to function, pointer to member function, and functor types of
        //:   of all varieties.
        //: 16 For concern 16, construct the 'function' within the exception
        //:   test framework.  On exception, verify that any allocated memory
        //:   has been released and that no 'FUNC' objects have been leaked.
        //: 17 For concern 17, perform the above tests on an interesting
        //:   selection of functor types wrapped in a nothrow wrapper.
        //
        // Testing
        //      function(allocator_arg_t, const ALLOC& alloc, FUNC func);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR 'function(allocator_arg_t,"
                            "const ALLOC& alloc,FUNC func)'"
                            "\n======================================"
                            "==============================\n");

        // Null functors
        int (*nullFuncPtr)(IntWrapper, int) = 0;
        int (IntWrapper::*nullMemFuncPtr)(int) const = 0;

#define TEST(A, f, E)                                           \
    if (veryVeryVerbose) printf("\tALLOC is %s\n", #A);         \
    testFuncWithAlloc<A>(L_, f, E)

        if (veryVerbose) printf("FUNC is nullptr\n");
        TEST(bslma::TestAllocator *  , bsl::nullptr_t() , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , bsl::nullptr_t() , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , bsl::nullptr_t() , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , bsl::nullptr_t() , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, bsl::nullptr_t() , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is nullFuncPtr\n");
        TEST(bslma::TestAllocator *  , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , nullFuncPtr      , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, nullFuncPtr      , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is nullMemFuncPtr\n");
        TEST(bslma::TestAllocator *  , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , nullMemFuncPtr   , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, nullMemFuncPtr   , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is simpleFunc\n");
        TEST(bslma::TestAllocator *  , simpleFunc       , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , simpleFunc       , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , simpleFunc       , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , simpleFunc       , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, simpleFunc       , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is &IntWrapper::add1\n");
        TEST(bslma::TestAllocator *  , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , &IntWrapper::add1, e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, &IntWrapper::add1, e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is EmptyFunctor()\n");
        TEST(bslma::TestAllocator *  , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , EmptyFunctor()   , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, EmptyFunctor()   , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is SmallFunctor(0)\n");
        TEST(bslma::TestAllocator *  , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , SmallFunctor(0)  , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, SmallFunctor(0)  , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is MediumFunctor(0)\n");
        TEST(bslma::TestAllocator *  , MediumFunctor(0) , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , MediumFunctor(0) , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , MediumFunctor(0) , e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , MediumFunctor(0) , e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, MediumFunctor(0) , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is LargeFunctor(0)\n");
        TEST(bslma::TestAllocator *  , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(bsl::allocator<char>    , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator<char> , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator2<char>, LargeFunctor(0)  , e_OUTOFPLACE_BOTH);

        if (veryVerbose) printf("FUNC is NTSmallFunctor(0)\n");
        TEST(bslma::TestAllocator *  , NTSmallFunctor(0), e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , NTSmallFunctor(0), e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , NTSmallFunctor(0), e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , NTSmallFunctor(0), e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, NTSmallFunctor(0), e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is ThrowingSmallFunctor(0)\n");
        TEST(bslma::TestAllocator *  , ThrowingSmallFunctor(0),
                                                            e_OUTOFPLACE_BOTH);
        TEST(bsl::allocator<char>    , ThrowingSmallFunctor(0),
                                                            e_OUTOFPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , ThrowingSmallFunctor(0),
                                                            e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator<char> , ThrowingSmallFunctor(0),
                                                            e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator2<char>, ThrowingSmallFunctor(0),
                                                            e_OUTOFPLACE_BOTH);

        if (veryVerbose) printf("FUNC is ThrowingEmptyFunctor(0)\n");
        TEST(bslma::TestAllocator *  , ThrowingEmptyFunctor(0),
                                                            e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator2<char>, ThrowingEmptyFunctor(0),
                                                            e_OUTOFPLACE_BOTH);

        bslma::TestAllocator xa;

        if (veryVerbose) printf("FUNC is SmallFunctorWithAlloc(0)\n");
#define SmFnAlloc SmallFunctorWithAlloc
        TEST(bslma::TestAllocator *  , SmFnAlloc(0, &xa), e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , SmFnAlloc(0, &xa), e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , SmFnAlloc(0, &xa), e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , SmFnAlloc(0, &xa), e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, SmFnAlloc(0, &xa), e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is NTSmallFunctorWithAlloc(0)\n");
#define NTSmFnAlc NTSmallFunctorWithAlloc
        TEST(bslma::TestAllocator *  , NTSmFnAlc(0, &xa), e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , NTSmFnAlc(0, &xa), e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , NTSmFnAlc(0, &xa), e_INPLACE_BOTH);
        TEST(StatefulAllocator<char> , NTSmFnAlc(0, &xa), e_INPLACE_FUNC_ONLY);
        TEST(StatefulAllocator2<char>, NTSmFnAlc(0, &xa), e_INPLACE_FUNC_ONLY);

        if (veryVerbose) printf("FUNC is LargeFunctorWithAlloc(0)\n");
#define LgFnAlloc LargeFunctorWithAlloc
        TEST(bslma::TestAllocator *  , LgFnAlloc(0, &xa)  , e_OUTOFPLACE_BOTH);
        TEST(bsl::allocator<char>    , LgFnAlloc(0, &xa)  , e_OUTOFPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , LgFnAlloc(0, &xa)  , e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator<char> , LgFnAlloc(0, &xa)  , e_OUTOFPLACE_BOTH);
        TEST(StatefulAllocator2<char>, LgFnAlloc(0, &xa)  , e_OUTOFPLACE_BOTH);

#undef TEST

// Repeat selected tests, wrapping functor in 'Function_NothrowWrapper'
#define WTST(A, f, E)                                           \
    if (veryVeryVerbose) printf("\tALLOC is %s, FUNC is ntWrap(%s)\n",#A,#f); \
    testFuncWithAlloc<A>(L_, ntWrap(f), E)

        if (veryVerbose) printf("Wrap FUNC in nothrow wrapper\n");
        WTST(bslma::TestAllocator *  , nullFuncPtr      , e_INPLACE_BOTH);
        WTST(StatefulAllocator<char> , nullFuncPtr      , e_INPLACE_FUNC_ONLY);
        WTST(bsl::allocator<char>    , &simpleFunc      , e_INPLACE_BOTH);
        WTST(StatefulAllocator2<char>, &simpleFunc      , e_INPLACE_FUNC_ONLY);
        WTST(EmptySTLAllocator<char> , &IntWrapper::add1, e_INPLACE_BOTH);
        WTST(StatefulAllocator<char> , &IntWrapper::add1, e_INPLACE_FUNC_ONLY);
        WTST(bslma::TestAllocator *  , EmptyFunctor()   , e_INPLACE_BOTH);
        WTST(StatefulAllocator2<char>, EmptyFunctor()   , e_INPLACE_FUNC_ONLY);
        WTST(bsl::allocator<char>    , SmallFunctor(0)  , e_INPLACE_BOTH);
        WTST(StatefulAllocator<char> , SmallFunctor(0)  , e_INPLACE_FUNC_ONLY);
        WTST(StatefulAllocator2<char>, MediumFunctor(0) , e_INPLACE_FUNC_ONLY);
        WTST(bslma::TestAllocator *  , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        WTST(bslma::TestAllocator *  , NTSmallFunctor(0), e_INPLACE_BOTH);
        WTST(bsl::allocator<char>    , ThrowingSmallFunctor(0)
                                                        , e_INPLACE_BOTH);
        WTST(StatefulAllocator<char> , ThrowingSmallFunctor(0)
                                                        , e_INPLACE_FUNC_ONLY);
        WTST(bslma::TestAllocator *  , ThrowingEmptyFunctor(0)
                                                        , e_INPLACE_BOTH);
        WTST(StatefulAllocator2<char>, ThrowingEmptyFunctor(0)
                                                        , e_INPLACE_FUNC_ONLY);
        WTST(bslma::TestAllocator *  , SmFnAlloc(0, &xa), e_INPLACE_BOTH);
        WTST(bslma::TestAllocator *  , LgFnAlloc(0, &xa), e_OUTOFPLACE_BOTH);
        WTST(StatefulAllocator2<char>, LgFnAlloc(0, &xa), e_OUTOFPLACE_BOTH);

#undef WTST

#undef SmFnAlloc
#undef NTSmFnAlloc
#undef LgFnAlloc

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR function(allocator_arg_t, const ALLOC& alloc)
        //
        // Concerns:
        //: 1 Constructing a 'function' using this constructor yields an
        //:   empty 'function' object.
        //: 2 If 'alloc' is a pointer to a 'bslma::Allocator' object, then the
        //:   'allocator' accessor will return that pointer.
        //: 3 If 'alloc' is a 'bsl::allocator' object, then the 'allocator'
        //:   accessor will return 'alloc.mechanism()'.
        //: 4 If 'alloc' is an STL-style allocator with no state, then the
        //:   'allocator' accessor will return a pointer to an object of type
        //:   'bslma::AllocatorAdaptor<ALLOC>'.  That same pointer will be
        //:   returned by any 'function' created with that allocator type.
        //: 5 If 'alloc' is an STL-style allocator with state, then the
        //:   'allocator' accessor will return a pointer to an object of type
        //:   'bslma::AllocatorAdaptor<ALLOC>' which wraps a copy of 'alloc'.
        //: 6 If 'alloc' is other than an STL-style allocator with state, then
        //:   no memory is allocated by this constructor.
        //: 7 If 'alloc' is an STL-style allocator with state, then one block
        //:   of memory is allocated from 'alloc' itself.
        //: 8 If memory is allocated, the destructor frees it.
        //: 9 All of the above concerns also apply to the
        //:   'function(allocator_arg_t, const ALLOC&, nullptr_t)' constructor.
        //: 10 If the allocator throws an exception, no resources are leaked
        //:   (i.e., it is exception neutral).
        //
        // Plan:
        //: 1 For concern 1 test each 'function' object constructed using this
        //:   constructor to verify that converts to a Boolean false value.
        //: 2 For concern 2, construct a 'function' object with the address of
        //:   a 'bslma:TestAllocator'.  Verify that 'allocator' returns the
        //:   address of the test allocator.
        //: 3 For concern 3, construct a 'bsl::allocator' wrapping a test
        //:   allocator.  Construct a 'function' object with the
        //:   'bsl::allocator' object.  Verify that 'allocator' returns the
        //:   address of the test allocator (i.e., the 'mechanism()' of the
        //:   'bsl::allocator'.
        //: 4 For concern 4, define a stateless STL-style allocator class and
        //:   use an instance of that class to construct a 'function'
        //:   object.  Verify that 'allocator' returns a pointer that can
        //:   dynamically cast to a 'bslma::AllocatorAdaptor' wrapping the
        //:   STL-style allocator.  Verify that multiple 'function' objects
        //:   instantiated with multiple instantiations of the same STL-style
        //:   allocator return the same result from calling the 'allocator'
        //:   method.
        //: 5 For concern 5, define a stateful STL-style allocator class and
        //:   use an instance of that  class to construct a 'function'
        //:   object.  Verify that 'allocator' returns a pointer that can
        //:   dynamically cast to a 'bslma::AllocatorAdaptor' wrapping the
        //:   STL-style allocator and that the allocator wrapped by the
        //:   adaptor is equal to the original STL-style allocator.
        //: 6 For concern 6, test the results of steps 2-4 to verify that no
        //:   memory is allocated either from the global allocator or from the
        //:   allocator used to construct the 'function' object.
        //: 7 For concern 7, test the result of step 5 to verify that exactly
        //:   one block was allocated from the allocator used to construct the
        //:   'function' and that no memory was allocated from the global
        //:   allocator.
        //: 8 For concern 8, check at the end of step 7, when the 'function'
        //:   object is destroyed, that all memory was returned to the
        //:   allocator.
        //: 9 For concern 9, preform all of the previous steps using the
        //:   'function(allocator_arg_t, const ALLOC&, nullptr_t)' constructor.
        //: 10 For concern 10 perform the operations within exception-test
        //:   loop, verifying that memory use doesn't change if the
        //:   constructor call fails due to an exception.
        //
        // Testing
        //      function(allocator_arg_t, const ALLOC& alloc);
        //      function(allocator_arg_t, const ALLOC& alloc, nullptr_t);
        //      ~function();
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR function(allocator_arg_t, "
                            "const ALLOC& alloc)"
                            "\n======================================"
                            "===================\n");

        bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

        if (veryVerbose) printf("with bslma::Allocator*\n");
        bslma::TestAllocator ta;
        EXCEPTION_TEST_BEGIN(&ta, NULL) {
            EXCEPTION_TEST_TRY {
                globalAllocMonitor.reset();
                bsl::function<int(float)> f(bsl::allocator_arg, &ta);
                ASSERT(! f);
                ASSERT(&ta == f.allocator());
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());

                bsl::function<int(float)> f2(bsl::allocator_arg, &ta,
                                             bsl::nullptr_t());
                ASSERT(! f2);
                ASSERT(&ta == f2.allocator());
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;

        if (veryVerbose) printf("with bsl::allocator<T>\n");
        EXCEPTION_TEST_BEGIN(&ta, NULL) {
            EXCEPTION_TEST_TRY {
                globalAllocMonitor.reset();
                bsl::allocator<void*> alloc(&ta);

                bsl::function<int(float)> f(bsl::allocator_arg, alloc);
                ASSERT(! f);
                ASSERT(&ta == f.allocator());
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());

                bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                             bsl::nullptr_t());
                ASSERT(! f2);
                ASSERT(&ta == f2.allocator());
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;

        if (veryVerbose) printf("with stateless allocator\n");
        EXCEPTION_TEST_BEGIN(&ta, NULL) {
            typedef EmptySTLAllocator<double> Alloc;
            typedef EmptySTLAllocator<bool>   Alloc2;
            typedef
              bslma::AllocatorAdaptor<EmptySTLAllocator<char> >::Type Adaptor;
            EXCEPTION_TEST_TRY {

                globalAllocMonitor.reset();
                Alloc alloc(&ta);
                ASSERT(bsl::is_empty<Alloc>::value);

                bsl::function<int(float)> f(bsl::allocator_arg, alloc);
                ASSERT(! f);
                bslma::Allocator *erasedAlloc = f.allocator();
                ASSERT(0 != dynamic_cast<Adaptor *>(erasedAlloc));
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());

                Alloc2 alloc2(&ta);
                bsl::function<int(float)> f2(bsl::allocator_arg, alloc2,
                                             bsl::nullptr_t());
                ASSERT(! f2);
                // Every use of stateless allocator 'EmptySTLAllocator<char>'
                // yields the identical adaptor.
                ASSERT(erasedAlloc == f2.allocator());
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;

        if (veryVerbose) printf("with stateful allocator\n");
        EXCEPTION_TEST_BEGIN(&ta, NULL) {
            typedef StatefulAllocator<double> Alloc;
            EXCEPTION_TEST_TRY {

                globalAllocMonitor.reset();
                Alloc alloc(&ta);
                ASSERT(! bsl::is_empty<Alloc>::value);

                bsl::function<int(float)> f(bsl::allocator_arg, alloc);
                ASSERT(! f);
                ASSERT(areEqualAlloc(alloc, f.allocator()));
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());

                bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                             bsl::nullptr_t());
                ASSERT(! f2);
                ASSERT(areEqualAlloc(alloc, f2.allocator()));
                ASSERT(2 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;

        if (veryVerbose) printf("with large stateful allocator\n");
        EXCEPTION_TEST_BEGIN(&ta, NULL) {
            typedef StatefulAllocator2<double> Alloc;
            EXCEPTION_TEST_TRY {

                globalAllocMonitor.reset();
                Alloc alloc(&ta);
                ASSERT(! bsl::is_empty<Alloc>::value);

                {
                    bsl::function<int(float)> f(bsl::allocator_arg, alloc);
                    ASSERT(! f);
                    ASSERT(areEqualAlloc(alloc, f.allocator()));
                    ASSERT(1 == ta.numBlocksInUse());
                    ASSERT(globalAllocMonitor.isInUseSame());

                    bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                                 bsl::nullptr_t());
                    ASSERT(! f2);
                    ASSERT(areEqualAlloc(alloc, f2.allocator()));
                    ASSERT(2 == ta.numBlocksInUse());
                    ASSERT(globalAllocMonitor.isInUseSame());
                }
                ASSERT(0 == ta.numBlocksInUse());
            } EXCEPTION_TEST_ENDTRY;
        } EXCEPTION_TEST_END;

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // FUNCTOR INVOCATION
        //
        // Concerns:
        //: 1 A 'bsl::function' object that is constructed with a functor (aka
        //:   function object) can be invoked as if it were a copy of that
        //:   functor.
        //: 2 Invocation works for zero to ten arguments and yields the
        //:   expected return value and side-effects on the functor.
        //: 3 Functions can return 'void'.
        //: 4 If the return value of the 'bsl::function' is 'void', then the
        //:   return value of function-pointer invocation is discarded,
        //:   even if the return type of the pointer-to-function is non-void.
        //: 5 The prototype for a function pointer need not be an exact match
        //:   for the parameter type of a 'bsl::function' type -- So long as
        //:   each formal argument to the 'bsl::function' is implicitly
        //:   convertible to the corresponding argument to the function pointer
        //:   and the return type of invocation through the function pointer
        //:   is implicitly convertible to the return type of the
        //:   'bsl::function'.
        //: 6 Invocation works correctly whether the functor is empty, fits
        //:   within the small-object optimization, or is allocated on the
        //:   heap.
        //: 7 Side effects are observed even if the 'bsl::function' is
        //:   const. This surprising fact comes from the idea that a
        //:   'function' object is an abstraction of a pointer to a
        //:   function. Moreover, type erasure means that, at compile time, it
        //:   is not possible to determine whether the invocable object
        //:   even cares whether or not it is const.
        //: 8 When the constructor's functor argument is wrapped using
        //:   'Function_NothrowWrapper', invocation procedes as though the
        //:   wrapper were not present.
        //
        // Plan:
        //: 1 Create a set of functor class with ten overloads of
        //:   'operator()', taking 0 to 10 arguments.  The first argument (for
        //:   all but the zero-argument case) is of type 'IntWrapper' and the
        //:   remaining arguments are of type 'int'.  These invocation
        //:   operators add all of the arguments to the integer state member
        //:   in the functor and returns the result.
        //: 2 For concerns 1 and 2, implement a test function template,
        //:   'testFunctor' that constructs constructs one instance of the
        //:   specified functor type and creates 10 instances of
        //:   'bsl::function' instantiated for the specified object type,
        //:   specified return type, and 0 to 9 arguments of the specified
        //:   argument type.  The test function constructs each instance of
        //:   'bsl::function' with a copy of the functor and then invokes
        //:   it, verifying that the return value and side-effects are as
        //:   expected.
        //: 3 For concern 3, add to the functor class another 'operator()'
        //:   Taking a 'const char*' argument and returning void.  Verify that
        //:   a 'bsl::function' object this invoker can be invoked and has the
        //:   expected size-effect.
        //: 4 For concern 4, create a 'bsl::function' with parameter
        //:   'void(int)' and verify that it can be used to wrap the functor
        //:   invoked with a single argument (discarding the return value).
        //: 5 For concern 5, instantiate 'testFunctor' with using a class
        //:   'ConvertibleToInt' instead of 'int' for the argument types
        //:   and using 'IntWrapper' instead of 'int' for the return type.
        //: 6 For concern 6, repeat each of the above steps with stateless,
        //:   small, and large functor classes by instantiating 'testFunctor'
        //:   with 'EmptyFunctor', 'SmallFunctor', and 'LargeFunctor'.  It is
        //:   not necessary to test with 'MediumFunctor' as that does not test
        //:   anything not already tested by 'SmallFunctor'.
        //: 7 For concern 7, augment step 2 with a const 'bsl::function'
        //:   object. It is necessary to test only one set of arguments in
        //:   order to have confidence in the result.
        //: 8 For concern 8, augment step 2, wrapping the constructor argument
        //:   in a 'Function_NothrowWrapper'.  It is necessary to test only
        //:   one set of arguments in order to have confidence in the result.
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For functors
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTOR INVOCATION"
                            "\n==================\n");

        if (veryVerbose) printf("Plan step 2\n");
        testFunctor<SmallFunctor, int, int>("SmallFunctor int(int...)");

        if (veryVerbose) printf("Plan step 5\n");
        testFunctor<SmallFunctor, IntWrapper, ConvertibleToInt>(
            "SmallFunctor IntWrapper(ConvertibleToInt...)");

        if (veryVerbose) printf("Plan step 6\n");
        testFunctor<EmptyFunctor, int, int>("EmptyFunctor int(int...)");
        testFunctor<EmptyFunctor, IntWrapper, ConvertibleToInt>(
            "EmptyFunctor IntWrapper(ConvertibleToInt...)");
        testFunctor<LargeFunctor, int, int>("LargeFunctor int(int...)");
        testFunctor<LargeFunctor, IntWrapper, ConvertibleToInt>(
            "LargeFunctor IntWrapper(ConvertibleToInt...)");

      } break;

      case 6: {
        // --------------------------------------------------------------------
        // POINTER TO MEMBER FUNCTION INVOCATION
        //
        // Concerns:
        //
        //  All of the following concerns refer to an object 'f' of type
        //  'bsl::function<RET(T, ARGS...)>' for a specified return type
        //  'RET', class type 'T', and 0 or more additional argument types
        //  'ARGS...'.  f's constructor argument is a pointer 'fp' to member
        //  function of type 'FRET (FT::*)(FARGS...)' for a specified return
        //  type 'FRET', class type 'FT', and 0 or more argument types
        //  'FARGS...'.  The invocation arguments are 'obj' of type 'T' and
        //  'args...' of types matching 'ARGS...'.
        //
        //: 1 Invocation works for zero to nine arguments, 'args...' in
        //:   addition to the 'obj' argument and yields the expected return
        //:   value.
        //: 2 If 'T' is the same as 'FT&', invoking 'f(obj, args...)'
        //:   yields the same return value and side-effect as invoking
        //:   '(obj.*fp)(args...)'.
        //: 3 If 'T' is the same as 'FT', invoking 'f(obj, args...)'
        //:   yields the same return value as invoking '(obj.*fp)(args...)'
        //:   and will have no side effect on the (pass-by-value) 'obj'.
        //: 4 If 'T' is the same as 'FT*' or "smart pointer" to 'FT', invoking
        //:   'f(obj, args...)'  yields the same results as invoking
        //:   '((*obj).*fp)(args...)'.
        //: 5 The template argument types 'ARGS...' need not match the
        //:   member-function arguments 'FARGS...' exactly, so long as the
        //:   argument lists are the same length and each type in 'ARGS' is
        //:   implicitly convertible to the corresponding argument in
        //:   'FARGS'.
        //: 6 The return type 'RET' need not match the member-function return
        //:   type 'FRET' so long as 'RET' is implicitly convertible to
        //:   'FRET'.
        //: 7 If 'fp' is a pointer to const member function, then 'T' can be
        //:   rvalue of, reference to, pointer to, or smart-pointer to either
        //:   a const or a non-const type.  All of the above concerns apply to
        //:   both const and non-const member functions.
        //: 8 Concerns 1 and 2 also apply if 'T' is an rvalue of, reference to,
        //:   pointer to, or smart-pointer to type derived from 'FT'.
        //: 9 If 'RET' is 'void', then the return value of 'pf' is discarded,
        //:   even if 'FRET' is non-void.
        //: 10 When the 'fp' is wrapped using 'Function_NothrowWrapper',
        //:   invocation procedes as though the wrapper were not present.
        //
        // Plan:
        //: 1 Create a class 'IntWrapper' that holds an 'int' value and has
        //:   const member functions 'add0' to 'add9' and non-const member
        //:   functions 'increment0' to 'increment9' and 'voidIncrement0' to
        //:   'voidIncrement9' each taking 0 to 9 'int' arguments.  The
        //:   'sum[0-9]' functions return the 'int' sum of the arguments + the
        //:   wrapper's value.  The 'increment[0-9]' functions increment the
        //:   wrapper's value by the sum of the arguments and returns the
        //:   'int' result.  The 'voidIncrement[0-9]' functions increment the
        //:   wrapper's value by the sum of the arguments and return nothing.
        //: 2 For concern 1, implement a test function template
        //:   'testPtrToMemFunc' that creates 10 instances of 'bsl::function'
        //:   instantiated for the specified object type, specified return
        //:   type, and 0 to 9 arguments of the specified argument type.  The
        //:   test function constructs each instance with a pointer to the
        //:   corresponding 'increment[0-9]' member function of 'IntWrapper'
        //:   and then invokes it, verifying that the return value and
        //:   side-effects are as expected.
        //: 3 For concern 2, invoke 'testPtrToMemFunc' with object type
        //:   'IntWrapper&'.
        //: 4 For concern 3, ensure that 'testPtrToMemFunc' checks for no
        //:   change to 'obj' if 'T' is an rvalue type.  Invoke
        //:   'testPtrToMemFunc' with object type 'IntWrapper'.
        //: 5 For concern 4, invoke 'testPtrToMemFunc' with object types
        //:   'IntWrapper*', and 'SmartPtr<IntWrapper>'.
        //: 6 For concerns 5 & 6, repeat steps 3, 4, and 5 except using a class
        //:   'ConvertibleToInt' instead of 'int' for the arguments in 'ARGS'
        //:   and using 'IntWrapper' instead of 'RET'.
        //: 7 For concern 7, implement a test function template,
        //:   'testPtrToConstMemFunc' that works similarly to
        //:   'testPtrToMemFunc' except that it wraps the const member
        //:   functions 'sum[0-9]' instead of the non-const member functions
        //:   'increment[0-9]'.  To save compile time, since concern 1 has
        //:   already been tested, we need to test only a small number of
        //:   possible argument-list lengths (e.g. 0, 1, and 9 arguments).
        //:   Invoke 'testPtrToConstMemFunc' with object types 'IntWrapper',
        //:   'IntWrapper&', 'IntWrapper*', and 'SmartPtr<IntWrapper>', as
        //:   well as 'const' versions of the preceding.
        //: 8 For concern 8, create a class, 'IntWrapperDerived' derived from
        //:   'IntWrapper'.  Invoke 'testPtrToConstMemFunc' with object types
        //:   'IntWrapperDerived', 'IntWrapperDerived&', 'IntWrapperDerived*',
        //:   and 'SmartPtr<IntWrapperDerived>', as well as 'const' and
        //:   versions of the preceding.
        //: 9 For concern 9, create a 'bsl::function' with prototype
        //:   'void(IntWrapper, int)' and use it to invoke
        //:   'IntWrapper::increment1', thus discarding the return value.
        //:   Repeat this test but wrapping 'IntWrapper::voidIncrement1',
        //:   showing that a 'voide' function can be invoked.
        //: 10 For concern 10, add tests to 'testPtrToMemFunc' and
        //:   'testPtrToConstMemFunc' as well as to the variants in step 9
        //:   whereby the 'function' object is constructed using a
        //:   pointer-to-member-function wrapped by a
        //:   'Function_NothrowWrapper' and verify that the behavior does not
        //:   change.
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For pointer to member func
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO MEMBER FUNCTION INVOCATION"
                            "\n=====================================\n");

        if (veryVerbose) printf("Plan step 3\n");
        testPtrToMemFunc<IntWrapper&, int, int>("int(IntWrapper&, int...)");

        if (veryVerbose) printf("Plan step 4\n");
        testPtrToMemFunc<IntWrapper, int, int>("int(IntWrapper, int...)");

        if (veryVerbose) printf("Plan step 5\n");
        testPtrToMemFunc<IntWrapper*, int, int>("int(IntWrapper*, int...)");
        testPtrToMemFunc<SmartPtr<IntWrapper>, int, int>(
            "int(SmartPtr<IntWrapper>, int...)");

        if (veryVerbose) printf("Plan step 6\n");
        testPtrToMemFunc<IntWrapper, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper, ConvertibleToInt...");
        testPtrToMemFunc<IntWrapper&, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper&, ConvertibleToInt...");
        testPtrToMemFunc<IntWrapper*, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper*, ConvertibleToInt...");
        testPtrToMemFunc<SmartPtr<IntWrapper>, IntWrapper, ConvertibleToInt>(
            "IntWrapper(SmartPtr<IntWrapper>, ConvertibleToInt...");

        if (veryVerbose) printf("Plan step 7\n");
        testPtrToConstMemFunc<IntWrapper, int, int>(
            "int(IntWrapper, int...)");
        testPtrToConstMemFunc<IntWrapper&, int, int>(
            "int(IntWrapper&, int...)");
        testPtrToConstMemFunc<IntWrapper*, int, int>(
            "int(IntWrapper*, int...)");
        testPtrToConstMemFunc<SmartPtr<IntWrapper>, int, int>(
            "int(SmartPtr<IntWrapper>, int...)");
        testPtrToConstMemFunc<const IntWrapper, int, int>(
            "int(const IntWrapper, int...)");
        testPtrToConstMemFunc<const IntWrapper&, int, int>(
            "int(const IntWrapper&, int...)");
        testPtrToConstMemFunc<const IntWrapper*, int, int>(
            "int(const IntWrapper*, int...)");
        testPtrToConstMemFunc<SmartPtr<const IntWrapper>, int, int>(
            "int(SmartPtr<const IntWrapper>, int...)");

        if (veryVerbose) printf("Plan step 8\n");
        testPtrToConstMemFunc<IntWrapperDerived, int, int>(
            "int(IntWrapperDerived, int...)");
        testPtrToConstMemFunc<IntWrapperDerived&, int, int>(
            "int(IntWrapperDerived&, int...)");
        testPtrToConstMemFunc<IntWrapperDerived*, int, int>(
            "int(IntWrapperDerived*, int...)");
        testPtrToConstMemFunc<SmartPtr<IntWrapperDerived>, int, int>(
            "int(SmartPtr<IntWrapperDerived>, int...)");
        testPtrToConstMemFunc<const IntWrapperDerived, int, int>(
            "int(const IntWrapperDerived, int...)");
        testPtrToConstMemFunc<const IntWrapperDerived&, int, int>(
            "int(const IntWrapperDerived&, int...)");
        testPtrToConstMemFunc<const IntWrapperDerived*, int, int>(
            "int(const IntWrapperDerived*, int...)");
        testPtrToConstMemFunc<SmartPtr<const IntWrapperDerived>, int, int>(
            "int(SmartPtr<const IntWrapperDerived>, int...)");

        if (veryVerbose) printf("Plan step 9\n");
        IntWrapper iw(0x3001);

        bsl::function<void(IntWrapper, int)> ft(&IntWrapper::increment1);
        ft(iw, 1);                     // No return type to test
        ASSERT(0x3001 == iw.value());  // Passed by value. Original unchanged.

        bsl::function<void(IntWrapper&, int)> ftr(&IntWrapper::increment1);
        ftr(iw, 2);                    // No return type to test
        ASSERT(0x3003 == iw.value());

        bsl::function<void(IntWrapper*, int)> ftp(&IntWrapper::increment1);
        ftp(&iw, 4);                   // No return type to test
        ASSERT(0x3007 == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>,
                           int)> ftsp(&IntWrapper::increment1);
        ftsp(&iw, 8);                  // No return type to test
        ASSERT(0x300f == iw.value());

        bsl::function<void(IntWrapper*, int)>
            ntftp(ntWrap(&IntWrapper::increment1));
        ntftp(&iw, 0x10);                   // No return type to test
        ASSERT(0x301f == iw.value());


        bsl::function<void(IntWrapper, int)> vt(&IntWrapper::voidIncrement1);
        vt(iw, 0x10);                  // No return type to test
        ASSERT(0x301f == iw.value());  // Passed by value. Original unchanged.

        bsl::function<void(IntWrapper&, int)> vtr(&IntWrapper::voidIncrement1);
        vtr(iw, 0x20);                 // No return type to test
        ASSERT(0x303f == iw.value());

        bsl::function<void(IntWrapper*, int)> vtp(&IntWrapper::voidIncrement1);
        vtp(&iw, 0x40);                // No return type to test
        ASSERT(0x307f == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>,
                           int)> vtsp(&IntWrapper::voidIncrement1);
        vtsp(&iw, 0x80);               // No return type to test
        ASSERT(0x30ff == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>, int)>
            ntvtsp(ntWrap(&IntWrapper::voidIncrement1));
        vtsp(&iw, 0x100);               // No return type to test
        ASSERT(0x31ff == iw.value());

      } break;

      case 5: {
        // --------------------------------------------------------------------
        // POINTER TO FUNCTION INVOCATION
        //
        // Concerns:
        //: 1 A 'bsl::function' object that is constructed with a pointer to a
        //:   non-member function can be invoked as if it were that function.
        //: 2 Invocation works for zero to ten arguments and yields the
        //:   expected return value.
        //: 3 The prototype for a function pointer need not be an exact match
        //:   for the parameter type of a 'bsl::function' type -- So long as
        //:   each formal argument to the 'bsl::function' is implicitly
        //:   convertible to the corresponding argument to the function pointer
        //:   and the return type of invocation through the function pointer
        //:   is implicitly convertible to the return type of the
        //:   'bsl::function'.
        //: 4 Functions can return 'void'.
        //: 5 If the return value of the 'bsl::function' is 'void', then the
        //:   return value of function-pointer invocation is discarded,
        //:   even if the return type of the pointer-to-function is non-void.
        //: 6 Arguments that are supposed to be passed by reference *are*
        //:   passed by reference all the way through the invocation
        //:   interface.
        //: 7 Arguments that are supposed to be passed by value are copied
        //:   exactly once when passed through the invocation interface.
        //: 8 When the function pointer argument is wrapped using
        //:   'Function_NothrowWrapper', invocation procedes as though the
        //:   wrapper were not present.
        //
        // Plan:
        //: 1 Create a set of functions, 'sum0' to 'sum10' taking 0 to 10
        //:   arguments.  The first argument (for all but 'sum0') is of type
        //:   'IntWrapper' and the remaining arguments are of type 'int'.  The
        //:   return value is an 'int' comprising the sum of the arguments +
        //:   '0x4000'.
        //: 2 For concerns 1 and 2, create and invoke 'bsl::function's
        //:   wrapping pointers to each of the functions 'sum0' to 'sum10'.
        //:   Verify that the return from the invocations matches the expected
        //:   results.
        //: 3 For concern 3, repeat step 2 except instantiate the
        //:   'bsl::function' objects with prototypes with arguments of type
        //:   'ConvertibleToInt' and return type 'IntWrapper'.
        //: 4 For concern 4, create a global function, 'increment' that
        //:   increments its argument (passed by address) and returns void.
        //:   Verify that a 'bsl::function' object wrapping a pointer to
        //:   'increment' can be invoked and has the expected size-effect.
        //: 5 For concern 5, create a 'bsl::function' with parameter
        //:   'void(int)' and verify that it can be used to wrap 'sum1'
        //:   (discarding the return value).
        //: 6 For concern 6, implement a set of functions, 'getAddress' and
        //:   'getConstAddress' that return the address of their argument,
        //:   which is passed by reference and passed by const reference,
        //:   respectively. Wrap pointers to these functions in
        //:   'bsl::function' objects with the same signature and verify that
        //:   they return the address of their arguments.
        //: 7 For concern 7, implement a class 'CountCopies' whose copy
        //:   constructor increments a counter, so that you can keep track of
        //:   how many copies-of-copies get made. Implement a function
        //:   'numCopies' that takes a 'CountCopies' object by value and
        //:   returns the number of times it was copied.  Verify that, when
        //:   invoked through a 'bsl::function' wrapper, the argument is
        //:   copied only once.
        //: 8 For concern 8, repeat a few test cases from the steps 2 through
        //:   7, wrapping the functoin-pointer argument in a nothrow wrapper.
        //:   Note that it is not necessary to test every combination of 0 to
        //:   10 arguments in order to have confidence that the constructor
        //:   argument is being correctly unwrapped.
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For pointer to function
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO FUNCTION INVOCATION"
                            "\n==============================\n");

        if (veryVerbose) printf("Plan step 2\n");
        {
            bsl::function<int()> f0(sum0);
            ASSERT(0x4000 == f0());

            bsl::function<int(int)> f1(sum1);
            ASSERT(0x4001 == f1(1));

            bsl::function<int(int, int)> f2(sum2);
            ASSERT(0x4003 == f2(1, 2));

            bsl::function<int(int, int, int)> f3(sum3);
            ASSERT(0x4007 == f3(1, 2, 4));

            bsl::function<int(int, int, int, int)> f4(sum4);
            ASSERT(0x400f == f4(1, 2, 4, 8));

            bsl::function<int(int, int, int, int, int)> f5(sum5);
            ASSERT(0x401f == f5(1, 2, 4, 8, 0x10));

            bsl::function<int(int, int, int, int, int, int)> f6(sum6);
            ASSERT(0x403f == f6(1, 2, 4, 8, 0x10, 0x20));

            bsl::function<int(int, int, int, int, int, int, int)> f7(sum7);
            ASSERT(0x407f == f7(1, 2, 4, 8, 0x10, 0x20, 0x40));

            bsl::function<int(int,int,int,int,int,int,int,int)> f8(sum8);
            ASSERT(0x40ff == f8(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));

            bsl::function<int(int,int,int,int,int,int,int,int,int)> f9(sum9);
            ASSERT(0x41ff == f9(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            bsl::function<int(int, int, int, int, int, int, int, int,
                              int, int)> f10(sum10);
            ASSERT(0x43ff == f10(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVerbose) printf("Plan step 3\n");
        {
            typedef IntWrapper       Ret;
            typedef ConvertibleToInt Arg;

            const Arg a1(0x0001);
            const Arg a2(0x0002);
            const Arg a3(0x0004);
            const Arg a4(0x0008);
            const Arg a5(0x0010);
            const Arg a6(0x0020);
            const Arg a7(0x0040);
            const Arg a8(0x0080);
            const Arg a9(0x0100);
            const Arg a10(0x0200);

            bsl::function<Ret()> f0(sum0);
            ASSERT(0x4000 == f0());

            bsl::function<Ret(Arg)> f1(sum1);
            ASSERT(0x4001 == f1(a1));

            bsl::function<Ret(Arg, Arg)> f2(sum2);
            ASSERT(0x4003 == f2(a1, a2));

            bsl::function<Ret(Arg, Arg, Arg)> f3(sum3);
            ASSERT(0x4007 == f3(a1, a2, a3));

            bsl::function<Ret(Arg, Arg, Arg, Arg)> f4(sum4);
            ASSERT(0x400f == f4(a1, a2, a3, a4));

            bsl::function<Ret(Arg, Arg, Arg, Arg, Arg)> f5(sum5);
            ASSERT(0x401f == f5(a1, a2, a3, a4, a5));

            bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg)> f6(sum6);
            ASSERT(0x403f == f6(a1, a2, a3, a4, a5, a6));

            bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg)> f7(sum7);
            ASSERT(0x407f == f7(a1, a2, a3, a4, a5, a6, a7));

            bsl::function<Ret(Arg,Arg,Arg,Arg,Arg,Arg,Arg,Arg)> f8(sum8);
            ASSERT(0x40ff == f8(a1, a2, a3, a4, a5, a6, a7, a8));

            bsl::function<Ret(Arg,Arg,Arg,Arg,Arg,Arg,Arg,Arg,Arg)> f9(sum9);
            ASSERT(0x41ff == f9(a1, a2, a3, a4, a5, a6, a7, a8, a9));

            bsl::function<Ret(Arg, Arg, Arg, Arg, Arg, Arg, Arg, Arg,
                              Arg, Arg)> f10(sum10);
            ASSERT(0x43ff == f10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10));
        }

        // Test void return type
        if (veryVerbose) printf("Plan step 4\n");
        bsl::function<void(int*)> fvoid(&increment);
        int v = 1;
        fvoid(&v);
        ASSERT(2 == v);

        // Test discarding of return value
        if (veryVerbose) printf("Plan step 5\n");
        bsl::function<void(int)> fdiscard(&sum1);
        fdiscard(3);

        // Test pass-by-reference
        if (veryVerbose) printf("Plan step 6\n");
        bsl::function<int*(int&)> ga(getAddress);
        ASSERT(&v == ga(v));
        bsl::function<const int*(const int&)> gca(getConstAddress);
        ASSERT(&v == gca(v));
        gca(v);

        // Test pass-by-value
        if (veryVerbose) printf("Plan step 7\n");
        bsl::function<int(CountCopies)> nc(numCopies);
        CountCopies cc;
        ASSERT(1 == numCopies(cc));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERT(1 == nc(cc));
        ASSERT(0 == numCopies(CountCopies()));
        ASSERT(0 == nc(CountCopies()));
#else
        ASSERT(2 == nc(cc));
        ASSERT(0 == numCopies(CountCopies()));
        ASSERT(1 == nc(CountCopies()));
#endif
        ASSERT(0 == cc.numCopies());

        // Test 'Function_NothrowWrapper'
        if (veryVerbose) printf("Plan step 8\n");
        {
            // Test normal function
            bsl::function<int(int)> f1(ntWrap(&sum1));
            ASSERT(0x4001 == f1(1));

            // Test function with argument conversion
            typedef IntWrapper       Ret;
            typedef ConvertibleToInt Arg;

            const Arg a1(0x0001);
            const Arg a2(0x0002);

            bsl::function<Ret(Arg, Arg)> f2(ntWrap(&sum2));
            ASSERT(0x4003 == f2(a1, a2));

            // Test void return type
            bsl::function<void(int*)> fvoid(ntWrap(&increment));
            int v = 1;
            fvoid(&v);
            ASSERT(2 == v);

            // Test discarding of return value
            bsl::function<void(int)> fdiscard(ntWrap(&sum1));
            fdiscard(3);

            // Test pass-by-reference
            bsl::function<int*(int&)> ga(ntWrap(&getAddress));
            ASSERT(&v == ga(v));
            bsl::function<const int*(const int&)>
                gca(ntWrap(&getConstAddress));
            ASSERT(&v == gca(v));
            gca(v);

            // Test pass-by-value
            bsl::function<int(CountCopies)> nc(ntWrap(&numCopies));
            CountCopies cc;
            ASSERT(1 == numCopies(cc));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ASSERT(1 == nc(cc));
            ASSERT(0 == numCopies(CountCopies()));
            ASSERT(0 == nc(CountCopies()));
#else
            ASSERT(2 == nc(cc));
            ASSERT(0 == numCopies(CountCopies()));
            ASSERT(1 == nc(CountCopies()));
#endif
            ASSERT(0 == cc.numCopies());
        }

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // EMPTY FUNCTION INVOCATION
        //
        // Concerns:
        //: 1 Invoking an empty function causes 'bsl::bad_function_call' to be
        //:   thrown.
        //: 2 The above is true for functions with zero to ten arguments.
        //: 3 The 'what' member of the thrown exception returns
        //:   "bad_function_call" as a null-terminated string.
        //
        // Plan:
        //: 1 For concerns 1, default-construct 'bsl::function' objects.
        //:   Invoke each object with suitable arguments and catch any
        //:   exceptions.  Verify that 'bad_function_call' is thrown in each
        //:   case.
        //: 2 For concern 2, repeat step 1 with different template parameters
        //:   for the constructed 'function' object, having zero to ten
        //:   function arguments.
        //: 3 For concern 3, test the result of calling 'what()' on the caught
        //:   exception and verify using 'strcmp' that it is a null-terminated
        //:   string matching "bad_function_call".
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For empty function object
        //      const char* bad_function_call::what() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nEMPTY FUNCTION INVOCATION"
                            "\n=========================\n");

#ifdef BDE_BUILD_TARGET_EXC

        for (int numArgs = 0; numArgs <= 10; ++numArgs) {

            if (veryVerbose) printf("with %d arguments\n", numArgs);

            int constructed = -1;

            try {
                switch (numArgs) {
                  case 0: {
                    bsl::function<int()> f0;
                    constructed = 0;
                    f0();
                  } break;

                  case 1: {
                    bsl::function<int(int)> f1;
                    constructed = 1;
                    f1(1);
                  } break;

                  case 2: {
                    bsl::function<int(int, int)> f2;
                    constructed = 2;
                    f2(1, 2);
                  } break;

                  case 3: {
                    bsl::function<int(int, int, int)> f3;
                    constructed = 3;
                    f3(1, 2, 4);
                  } break;

                  case 4: {
                    bsl::function<int(int, int, int, int)> f4;
                    constructed = 4;
                    f4(1, 2, 4, 8);
                  } break;

                  case 5: {
                    bsl::function<int(int, int, int, int, int)> f5;
                    constructed = 5;
                    f5(1, 2, 4, 8, 0x10);
                  } break;

                  case 6: {
                    bsl::function<int(int, int, int, int, int, int)> f6;
                    constructed = 6;
                    f6(1, 2, 4, 8, 0x10, 0x20);
                  } break;

                  case 7: {
                    bsl::function<int(int, int, int, int, int, int, int)> f7;
                    constructed = 7;
                    f7(1, 2, 4, 8, 0x10, 0x20, 0x40);
                  } break;

                  case 8: {
                    bsl::function<int(int,int,int,int,int,int,int,int)> f8;
                    constructed = 8;
                    f8(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80);
                  } break;

                  case 9: {
                    bsl::function<int(int,int,int,int,int,int,int,int,int)> f9;
                    constructed = 9;
                    f9(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100);
                  } break;

                  case 10: {
                    bsl::function<int(int, int, int, int, int, int, int, int,
                                      int, int)> f10;
                    constructed = 10;
                    f10(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200);
                  } break;
                } // end switch

                LOOP_ASSERT(numArgs,0 && "Exception should have been thrown");
            }
            catch (bsl::bad_function_call ex) {
                LOOP_ASSERT(numArgs,
                            0 == strcmp("bad_function_call", ex.what()));
            }
            catch (...) {
                LOOP_ASSERT(numArgs, 0 && "Incorrect exception caught");
            }
            LOOP_ASSERT(numArgs, numArgs == constructed);
        } // end for
#endif //  BDE_BUILD_TARGET_EXC

      } break;

      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR function(FUNC)
        //
        // Concerns:
        //:  1 Construction with a null pointer to function or null
        //:    pointer to member function creates an empty 'function' object.
        //:  2 Construction with a non-null pointer to
        //:    function, non-null pointer to member function, or functor
        //:    object creates a non-empty 'function' object.
        //:  3 'operator bool' returns true for non-empty function objects.
        //:  4 'bslma::Default::defaultAllocator()' is stored as the
        //:    allocator.
        //:  5 No memory is allocated unless the invocable is ineligible for
        //:    the small-object optimization.  An invocable is eligible for
        //:    the small-object optimization if it fits within the small
        //:    object buffer and is either bitwise moveable or has a nothrow
        //:    move constructor.
        //:  6 The destructor releases allocated memory, if any.
        //:  7 For a non-empty 'function', the 'target_type' accessor returns
        //:    the 'type_info' of the invocable specified at construction.
        //:  8 For a non-empty 'function', the 'target' accessor returns a
        //:    cv-qualified pointer to a copy of the invocable specified at
        //:    construction.
        //:  9 If constructed with an invocable that wrapped in in a
        //:    'Function_NothrowWrapper', the invocable is always treated as
        //:    though it had a nothrow move constructor in concern 5.  Concern
        //:    7 and 8 apply as if the invocable were not wrapped.
        //
        // Plan:
        //:  1 For concern 1, construct 'function' objects using a null
        //:    pointer to function and a null pointer to member function.
        //:    Verify that the resulting 'function' objects evaluate to false
        //:    in a boolean context.
        //:  2 For concerns 2 and 3, construct 'function' objects with a
        //:    non-null pointer to function, a non-null pointer to member
        //:    function, and a functor object.  Verify that the resulting
        //:    objects evaluate to true in a boolean context.
        //:  3 For concern 4, verify that 'allocator' returns
        //:    'bslma::Default::defaultAllocator()' for each object
        //:    constructed in the previous two steps.
        //:  4 For concerns 5 and 6, install a test allocator as the default
        //:    allocator and override 'operator new' and 'operator delete' to
        //:    use the test allocator as well.  Verify that none of the
        //:    constructors result in memory being allocated from the test
        //:    allocator except in the case of the functor object.  Using
        //:    various functors, verify that only functors that are too large
        //:    for the small-object optimization or that have throwing move
        //:    constructors (and are not bitwise moveable) result in any
        //:    allocations, and that those allocations amount to exactly one
        //:    block.  Verify that the destructor releases any allocated
        //:    memory.
        //:  5 For concern 7, verify that the return value of 'target_type'
        //:    matches the expected 'type_info'.
        //:  6 For concern 8, verify that, for the non-empty 'function'
        //:    objects, the return value of 'target' is a non-null pointer
        //:    pointing to a copy of the invocable used to construct the
        //:    'function' object.
        //:  7 For concern 9, wrap each invocable in a
        //:    'Function_NothrowWrapper' and repeat step 4, verifying that the
        //:    small object optimization applies for any invocable that is
        //:    small enough to fit into the small object buffer.  Repeat steps
        //:    5 and 6, verifying that the behavior is the same as if the
        //:    wrapper were not used.
        //:  8 Note that the semantics and implementation of the operations
        //:    being tested here are independent of the function prototype.
        //:    It is therefore not necessary to repeat these tests with
        //:    different prototypes. (Different prototypes are tested in the
        //:    invocation tests.)
        //
        // Testing
        //      function(FUNC f);
        //      ~function();
        //      operator bool() const;               // For non-empty objects
        //      const typeinfo& target_type() const; // For non-empty objects
        //      T      * target<T>();                // For non-empty objects
        //      T const* target<T>() const;          // For non-empty objects
        //      class Function_NothrowWrapper<FUNC>
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR function(FUNC)"
                            "\n==========================\n");

        typedef int (*SimpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*SimpleMemFuncPtr_t)(int) const;

        bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

        if (veryVerbose) printf("Construct with null pointer to function\n");
        globalAllocMonitor.reset();
        {
            const SimpleFuncPtr_t nullFuncPtr = NULL;
            Obj f(nullFuncPtr); const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(NULL == F.target<SimpleFuncPtr_t>());
            ASSERT(NULL == f.target<SimpleFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());

            typedef bsl::Function_NothrowWrapper<SimpleFuncPtr_t> NtWrapper;
            Obj fw(static_cast<NtWrapper>(nullFuncPtr)); const Obj& FW = fw;
            ASSERT(! FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == FW.target_type());
            ASSERT(NULL == FW.target<SimpleFuncPtr_t>());
            ASSERT(NULL == fw.target<SimpleFuncPtr_t>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) {
            printf("Construct with null pointer to member function\n");
        }
        globalAllocMonitor.reset();
        {
            const SimpleMemFuncPtr_t nullMemFuncPtr = NULL;
            Obj f(nullMemFuncPtr); const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(NULL == F.target<SimpleMemFuncPtr_t>());
            ASSERT(NULL == f.target<SimpleMemFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());

            typedef bsl::Function_NothrowWrapper<SimpleMemFuncPtr_t> NtWrapper;
            Obj fw(static_cast<NtWrapper>(nullMemFuncPtr)); const Obj& FW = fw;
            ASSERT(! FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == FW.target_type());
            ASSERT(NULL == FW.target<SimpleMemFuncPtr_t>());
            ASSERT(NULL == fw.target<SimpleMemFuncPtr_t>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with pointer to function\n");
        globalAllocMonitor.reset();
        {
            Obj f(simpleFunc); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(SimpleFuncPtr_t) == F.target_type());
            ASSERT(F.target<SimpleFuncPtr_t>() &&
                   &simpleFunc == *F.target<SimpleFuncPtr_t>());
            ASSERT(f.target<SimpleFuncPtr_t>() &&
                   &simpleFunc == *f.target<SimpleFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(&simpleFunc));
            const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(SimpleFuncPtr_t) == FW.target_type());
            ASSERT(FW.target<SimpleFuncPtr_t>() &&
                   &simpleFunc == *FW.target<SimpleFuncPtr_t>());
            ASSERT(fw.target<SimpleFuncPtr_t>() &&
                   &simpleFunc == *fw.target<SimpleFuncPtr_t>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with pointer to member function\n");
        globalAllocMonitor.reset();
        {
            Obj f = Obj(bsl::Function_NothrowWrapper<SimpleMemFuncPtr_t>(
                      &IntWrapper::add1)); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(SimpleMemFuncPtr_t) == F.target_type());
            ASSERT(F.target<SimpleMemFuncPtr_t>() &&
                   &IntWrapper::add1 == *F.target<SimpleMemFuncPtr_t>());
            ASSERT(f.target<SimpleMemFuncPtr_t>() &&
                   &IntWrapper::add1 == *f.target<SimpleMemFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(&IntWrapper::add1));
            const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(SimpleMemFuncPtr_t) == FW.target_type());
            ASSERT(FW.target<SimpleMemFuncPtr_t>() &&
                   &IntWrapper::add1 == *FW.target<SimpleMemFuncPtr_t>());
            ASSERT(fw.target<SimpleMemFuncPtr_t>() &&
                   &IntWrapper::add1 == *fw.target<SimpleMemFuncPtr_t>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with empty functor\n");
        globalAllocMonitor.reset();
        {
            EmptyFunctor ftor;
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(EmptyFunctor) == F.target_type());
            ASSERT(F.target<EmptyFunctor>() &&
                   ftor == *F.target<EmptyFunctor>());
            ASSERT(f.target<EmptyFunctor>() &&
                   ftor == *f.target<EmptyFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(ftor)); const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(EmptyFunctor) == FW.target_type());
            ASSERT(FW.target<EmptyFunctor>() &&
                   ftor == *FW.target<EmptyFunctor>());
            ASSERT(fw.target<EmptyFunctor>() &&
                   ftor == *fw.target<EmptyFunctor>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with small functor\n");
        globalAllocMonitor.reset();
        {
            SmallFunctor ftor(42);
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(SmallFunctor) == F.target_type());
            ASSERT(F.target<SmallFunctor>() &&
                   ftor == *F.target<SmallFunctor>());
            ASSERT(f.target<SmallFunctor>() &&
                   ftor == *f.target<SmallFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(ftor)); const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(SmallFunctor) == FW.target_type());
            ASSERT(FW.target<SmallFunctor>() &&
                   ftor == *FW.target<SmallFunctor>());
            ASSERT(fw.target<SmallFunctor>() &&
                   ftor == *fw.target<SmallFunctor>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with medium functor\n");
        globalAllocMonitor.reset();
        {
            MediumFunctor ftor(84);
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(MediumFunctor) == F.target_type());
            ASSERT(F.target<MediumFunctor>() &&
                   ftor == *F.target<MediumFunctor>());
            ASSERT(f.target<MediumFunctor>() &&
                   ftor == *f.target<MediumFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with large functor\n");
        globalAllocMonitor.reset();
        {
            long long preBlocks = globalTestAllocator.numBlocksInUse();
            LargeFunctor ftor(21);
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(preBlocks + 1 == globalTestAllocator.numBlocksInUse());
            ASSERT(typeid(LargeFunctor) == F.target_type());
            ASSERT(F.target<LargeFunctor>() &&
                   ftor == *F.target<LargeFunctor>());
            ASSERT(f.target<LargeFunctor>() &&
                   ftor == *f.target<LargeFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(ftor)); const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(preBlocks + 2 == globalTestAllocator.numBlocksInUse());
            ASSERT(typeid(LargeFunctor) == FW.target_type());
            ASSERT(FW.target<LargeFunctor>() &&
                   ftor == *FW.target<LargeFunctor>());
            ASSERT(fw.target<LargeFunctor>() &&
                   ftor == *fw.target<LargeFunctor>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with nothrow-move functor\n");
        globalAllocMonitor.reset();
        {
            // This functor is eligible for the small-object optimization.
            NTSmallFunctor ftor(42);
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(NTSmallFunctor) == F.target_type());
            ASSERT(F.target<NTSmallFunctor>() &&
                   ftor == *F.target<NTSmallFunctor>());
            ASSERT(f.target<NTSmallFunctor>() &&
                   ftor == *f.target<NTSmallFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(ftor)); const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(NTSmallFunctor) == FW.target_type());
            ASSERT(FW.target<NTSmallFunctor>() &&
                   ftor == *FW.target<NTSmallFunctor>());
            ASSERT(fw.target<NTSmallFunctor>() &&
                   ftor == *fw.target<NTSmallFunctor>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with throwing-move functor\n");
        globalAllocMonitor.reset();
        {
            // This functor is NOT eligible for the small-object optimization.
            long long preBlocks = globalTestAllocator.numBlocksInUse();
            ThrowingSmallFunctor ftor(21);
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(preBlocks + 1 == globalTestAllocator.numBlocksInUse());
            ASSERT(typeid(ThrowingSmallFunctor) == F.target_type());
            ASSERT(F.target<ThrowingSmallFunctor>() &&
                   ftor == *F.target<ThrowingSmallFunctor>());
            ASSERT(f.target<ThrowingSmallFunctor>() &&
                   ftor == *f.target<ThrowingSmallFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose)
            printf("Construct with wrapped throwing-move functor\n");
        globalAllocMonitor.reset();
        {
            // This functor is NOT eligible for the small-object optimization
            // but, when wrapped, it IS eligible.
            ThrowingSmallFunctor ftor(21);
            Obj fw(ntWrap(ftor)); const Obj& FW = fw;
            ASSERT(FW);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(ThrowingSmallFunctor) == FW.target_type());
            ASSERT(FW.target<ThrowingSmallFunctor>() &&
                   ftor == *FW.target<ThrowingSmallFunctor>());
            ASSERT(fw.target<ThrowingSmallFunctor>() &&
                   ftor == *fw.target<ThrowingSmallFunctor>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with throwing empty functor\n");
        globalAllocMonitor.reset();
        {
            // This functor is NOT eligible for the small-object optimization.
            long long preBlocks = globalTestAllocator.numBlocksInUse();
            ThrowingEmptyFunctor ftor(21);
            Obj f(ftor); const Obj& F = f;
            ASSERT(F);
            ASSERT(preBlocks + 1 == globalTestAllocator.numBlocksInUse());
            ASSERT(typeid(ThrowingEmptyFunctor) == F.target_type());
            ASSERT(F.target<ThrowingEmptyFunctor>() &&
                   ftor == *F.target<ThrowingEmptyFunctor>());
            ASSERT(f.target<ThrowingEmptyFunctor>() &&
                   ftor == *f.target<ThrowingEmptyFunctor>());
            ASSERT(&globalTestAllocator == f.allocator());

            Obj fw(ntWrap(ftor)); const Obj& FW = fw;
            ASSERT(FW);
            // No ADDITIONAL memory is allocated
            ASSERT(preBlocks + 1 == globalTestAllocator.numBlocksInUse());
            ASSERT(typeid(ThrowingEmptyFunctor) == FW.target_type());
            ASSERT(FW.target<ThrowingEmptyFunctor>() &&
                   ftor == *FW.target<ThrowingEmptyFunctor>());
            ASSERT(fw.target<ThrowingEmptyFunctor>() &&
                   ftor == *fw.target<ThrowingEmptyFunctor>());
            ASSERT(&globalTestAllocator == fw.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //:  1 Default construction and construction using a nullptr_t()
        //:    each create an empty 'function' object.
        //:  2 'operator bool' returns false for empty function objects.
        //:  3 'bslma::Default::defaultAllocator()' is stored as the
        //:    allocator.
        //:  4 No memory is allocated by the constructors.
        //:  5 'target_type' returns 'typeid(void)' for empty function objects.
        //:  6 'target' returns a null pointer for empty function objects.
        //
        // Plan:
        //:  1 For concerns 1 and 2, construct 'function' objects using each
        //:    of the argument lists described in concern 1.  Verify that the
        //:    resulting 'function' objects evaluate to false in a boolean
        //:    context.
        //:  2 For concern 3, test that the value returned by the 'allocator'
        //:    accessor is 'bslma::Default::defaultAllocator()'.
        //:  3 For concern 4, install a test allocator as the default
        //:    allocator and override 'operator new' and 'operator delete' to
        //:    use the test allocator as well.  Verify that none of the
        //:    constructors result in memory being allocated from the test
        //:    allocator.
        //:  4 For concern 5, verify for each constructed empty 'function'
        //:    that the 'target_type' accessor returns 'typeid(void)'.
        //:  5 For concern 6, verify for each constructed empty 'function'
        //:    that the 'target' accessor returns a null pointer (a
        //:    null-pointer to a const object, in the case of a const
        //:    'function').
        //:  6 Note that the semantics and implementation of the operations
        //:    being tested here are independent of the function prototype.
        //:    It is therefore not necessary to repeat these tests with
        //:    different prototypes. (Different prototypes are tested in the
        //:    invocation tests.)
        //
        // Testing
        //      function();
        //      function(nullptr_t);
        //      function(FUNC f); // For a null pointer to function
        //      function(FUNC f); // For a null pointer to member function
        //      bslma::Allocator* allocator() const;
        //      operator bool() const;               // For empty objects
        //      const typeinfo& target_type() const; // For empty objects
        //      T      * target<T>();                // For empty objects
        //      T const* target<T>() const;          // For empty objects
        // --------------------------------------------------------------------

        if (verbose) printf("\nPRIMARY MANIPULATORS"
                            "\n====================\n");

        bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

        if (veryVerbose) printf("Construct with no arguments\n");
        globalAllocMonitor.reset();
        {
            Obj f; const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(  isConstPtr(F.target<bsl::nullptr_t>()));
            ASSERT(! isConstPtr(f.target<bsl::nullptr_t>()));
            ASSERT(NULL == F.target<bsl::nullptr_t>());
            ASSERT(NULL == f.target<bsl::nullptr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with nullptr_t argument\n");
        globalAllocMonitor.reset();
        {
            const bsl::nullptr_t np = bsl::nullptr_t();
            Obj f(np); const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(NULL == F.target<bsl::nullptr_t>());
            ASSERT(NULL == f.target<bsl::nullptr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: The basic functionality of this component works
        //
        // Plan:
        //   Construct 'function' objects wrapping a small variety of
        //   functors.  Verify expected emptiness.  Invoke non-empty functors
        //   and verify expected results.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (veryVerbose) printf("Testing 'result_type', 'argument_type',\n"
                                "'first_argument_type', and "
                                "'second_argument_type'\n");

        {
            typedef bsl::function<void()> Obj;
            ASSERT(  (bsl::is_same<void, Obj::result_type>::value));
        }

        {
            typedef bsl::function<float(const double&)> Obj;
            ASSERT(  (bsl::is_same<float,         Obj::result_type>::value));
            ASSERT(  (bsl::is_same<const double&, Obj::argument_type>::value));
        }

        {
            typedef bsl::function<int(double, char&)> Obj;
            ASSERT(  (bsl::is_same<int,    Obj::result_type>::value));
            ASSERT(  (bsl::is_same<double, Obj::first_argument_type>::value));
            ASSERT(  (bsl::is_same<char&,  Obj::second_argument_type>::value));
        }

        if (veryVerbose) printf("Wrap int(*)()\n");
        {
            typedef bsl::function<int()> Obj;
            Obj n;
            ASSERT(! n);
            ASSERT(typeid(void) == n.target_type());
            ASSERT(NULL == n.target<int(*)()>());

            Obj f(sum0); const Obj& F = f;
            ASSERT(0x4000 == F());  // invoke
            ASSERT(typeid(&sum0) == F.target_type());
            ASSERT(&sum0 == *f.target<int(*)()>());
            ASSERT(&sum0 == *F.target<int(*)()>());
            ASSERT(NULL == F.target<int(*)(int)>());
        }

        if (veryVerbose) printf("Wrap int(*)(int, int)\n");
        {
            typedef bsl::function<int(int, int)> Obj;
            Obj f(sum2);
            ASSERT(0x4003 == f(1, 2));
            ASSERT(typeid(&sum2) == f.target_type());
            ASSERT(&sum2 == *f.target<int(*)(int, int)>());
        }

        {
            IntWrapper iw(0x4000), *iw_p = &iw; const IntWrapper& IW = iw;

            if (veryVerbose) printf("Wrap NULL void (IntWrapper::*)(int)\n");
            void (IntWrapper::*nullMember_p)(int) = NULL;
            bsl::function<void(IntWrapper&, int)> nullf(nullMember_p);
            ASSERT(! nullf);

            if (veryVerbose) printf("Wrap void (IntWrapper::*)()\n");
            bsl::function<void(IntWrapper&)> f0(&IntWrapper::incrementBy1);
            ASSERT(f0);
            f0(iw);
            ASSERT(0x4001 == iw.value());

            if (veryVerbose) printf("Wrap void (IntWrapper::*)(int, int)\n");
            bsl::function<void(IntWrapper*, int, int)>
                f3(&IntWrapper::increment2);
            ASSERT(f3);
            f3(iw_p, 2, 4);
            ASSERT(0x4007 == iw_p->value());

            if (veryVerbose) printf("Wrap int (IntWrapper::*)(int) const\n");
            bsl::function<int(const IntWrapper&, int)> fv(&IntWrapper::add1);
            ASSERT(fv);
            ASSERT(0x400f == fv(IW, 8));
        }

      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    ASSERT(topFuncMonitor.isSameCount());
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014-2015 Bloomberg Finance L.P.
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
