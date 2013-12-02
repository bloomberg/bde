// bslstl_function.t.cpp                  -*-C++-*-

#include "bslstl_function.h"

#include <bslmf_issame.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The 'bsl::function' class template is a value-semantic class that
// generalizes the concept of an invocable object.  An 'function' object wraps
// a pointer to function, pointer to member function, function object, or
// reference to function object.  It can also be "empty", i.e., wrap no
// object.  The saliant attribute of a 'function' object is the invocable
// object that it wraps (if any).  Because the invocable object is type-erased
// on construction, the run-time type of the invocable object is effectively a
// salient attribute as well.  A 'function' object also has a type-erased
// allocator, which is not salient.
//
// A 'function' object is effectively immutable, except that it can be
// assigned to.  A wrapped functor can also be modified in the process of
// being invoked.  As an immutable type, the only primitive manipulators are
// the constructors and assignment operators.  The primitive accessors are
// 'operator bool', the invocation operators, and the 'allocator' function.
// Black-box testing consists of constructing 'function' objects with a
// variety of invocables and testing the invocation works correctly and that
// 'operator bool' and 'allocator' functions return the expected values.  The
// biggest complication in this testing is verifying the large number of
// combinations of invocables and invocatoin prototypes (0 to 20 arguments,
// with and without return types).
//
// White-box testing looks at the memory allocation pattern. Both the
// invocable and the allocator are type erased and stored in allocated
// memory.  The implementation uses the small-object optimization to store one
// or both of these type-erased objects in the footprint of the 'function'
// object itself.  The allocation pattern is complicated by the fact that on
// copy construction and assignment, the invocable is copied but the allocator
// is not. Thus, whether or not the small-object optimization is applied to
// the allocator can be different in the original object vs. the copy.
//
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

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

//=============================================================================
//                  GLOBAL DEFINITIONS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

// Use this test allocator when another allocator is not specified.
bslma::TestAllocator globalTestAllocator("globalTestAllocator");

// Replace global new and delete to use the test allocator.
#ifdef BDE_BUILD_TARGET_EXC
void *operator new(size_t size) throw(std::bad_alloc)
#else
void *operator new(size_t size)
#endif
{
    return globalTestAllocator.allocate(size);
}

#ifdef BDE_BUILD_TARGET_EXC
void operator delete(void *address) throw()
#else
void operator delete(void *address)
#endif
{
    globalTestAllocator.deallocate(address);
}

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

// Create 11 functions with 0 to 10 integer arguments, returning
// the sum of the arguments + 0x4000.
SUMMING_FUNC(0)
BSLS_MACROREPEAT(10, SUMMING_FUNC)

class IntWrapper
{
    // Simple wrapper around an 'int' that supplies member functions (whose
    // address can be taken) for testing 'bsl::function'.

    int d_value;

public:
    IntWrapper(int i = 0) : d_value(i) { } // Convertible from 'int'

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

    // Mutable function with 0 to 9 arguments.  Increment the value by
    // the sum of all arguments.  'increment0()' is a no-op.
    int increment0() { return d_value; }
    BSLS_MACROREPEAT(9, INCREMENT_FUNC)

    void voidIncrement0() { }
    BSLS_MACROREPEAT(9, VOID_INCREMENT_FUNC)
};

class IntWrapperDerived : public IntWrapper
{
    // Derived class of 'IntWrapper'

public:
    IntWrapperDerived(int v) : IntWrapper(v) { }
};

class ConvertibleToInt
{
    // Class of objects implicitly convertible to 'int'

    int d_value;

public:
    explicit ConvertibleToInt(int v) : d_value(v) { }

    operator int() const { return d_value; }
};

// Simple function
int simpleFunc(const IntWrapper& iw, int v)
{
    return iw.value() + v;
}

// Simple functor with no state
struct EmptyFunctor
{
    // Stateless functor

#define OP_PAREN(n)                                                           \
    int operator()(const IntWrapper& iw, BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) \
    {                                                                         \
        return iw.value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);                 \
    }

    // Invocation operator with 0 to 10 arguments.  The first argument (if
    // any) is a const reference to 'IntWrapper', although it can be passed an
    // 'int'.  The remaining arguments are of type 'int'.
    int operator()() { return 0; }
    int operator()(const IntWrapper& iw) { return iw.value(); }
    BSLS_MACROREPEAT(9, OP_PAREN)

#undef OP_PAREN

    friend bool operator==(const EmptyFunctor&, const EmptyFunctor&)
        { return true; }

    friend bool operator!=(const EmptyFunctor&, const EmptyFunctor&)
        { return false; }
};

class SmallFunctor
{
    // Small stateful functor.

    int d_state;  // Arbitrary state to distinguish one instance from another

public:
    explicit SmallFunctor(int v) : d_state(v) { }

#define OP_PAREN(n)                                                           \
    int operator()(const IntWrapper& iw, BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) \
    {                                                                         \
        return d_state += iw.value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);      \
    }

    // Invocation operator with 0 to 10 arguments.  The first argument (if
    // any) is a const reference to 'IntWrapper', although it can be passed an
    // 'int'.  The remaining arguments are of type 'int'.
    int operator()() { return d_state; }
    int operator()(const IntWrapper& iw) { return d_state += iw.value(); }
    BSLS_MACROREPEAT(9, OP_PAREN)

#undef OP_PAREN

    int value() const { return d_state; }

    friend bool operator==(const SmallFunctor& a, const SmallFunctor& b)
        { return a.value() == b.value(); }

    friend bool operator!=(const SmallFunctor& a, const SmallFunctor& b)
        { return a.value() != b.value(); }
};

struct SmallObjectBuffer {
    // For white-box testing: size of small-object buffer in 'function'.
    void *d_padding[4];  // Size of 4 pointers
};

class MediumFunctor : public SmallFunctor
{
    // Functor that barely fits into the small object buffer.

    char d_padding[sizeof(SmallObjectBuffer) - sizeof(SmallFunctor)];
    
public:
    explicit MediumFunctor(int v) : SmallFunctor(v) { }

    friend bool operator==(const MediumFunctor& a, const MediumFunctor& b)
        { return a.value() == b.value(); }

    friend bool operator!=(const MediumFunctor& a, const MediumFunctor& b)
        { return a.value() != b.value(); }
};

class LargeFunctor : public SmallFunctor
{
    // Functor that barely does not fit into the small object buffer.

    char d_padding[sizeof(SmallObjectBuffer) - sizeof(SmallFunctor) + 1];
    
public:
    explicit LargeFunctor(int v) : SmallFunctor(v) { }

    friend bool operator==(const LargeFunctor& a, const LargeFunctor& b)
        { return a.value() == b.value(); }

    friend bool operator!=(const LargeFunctor& a, const LargeFunctor& b)
        { return a.value() != b.value(); }
};

inline bool isConstPtr(void *) { return false; }
inline bool isConstPtr(const void *) { return true; }

template <class T>
inline bool isNullPtrImp(const T& p, bsl::true_type /* is pointer */) {
    return 0 == p;
}

template <class T>
inline bool isNullPtrImp(const T& p, bsl::false_type /* is pointer */) {
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

//=============================================================================
//                  USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bslma::Default::setDefaultAllocator(&globalTestAllocator);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.

      case 5: {
        // --------------------------------------------------------------------
        // POINTER TO MEMBER FUNCTION INVOCATION
        //
        // Concerns:
        //
        //  All of the following concerns refer to an object 'f' of type
        //  'bsl::function<RET(T, ARGS...)>' for a specified type 'T', return
        //  type 'RET' 0 or more additional arguments 'ARGS'.  The object's
        //  constructor argument is a pointer 'fp' to member function of type
        //  'FRET (FT::*)(FARGS...)' for a specified return type 'FRET', class
        //  type 'FT', and 0 or more arguments 'FARGS'.  The invocation
        //  arguments are 'obj' of type 'T' and 'args...' of types matching
        //  'ARGS...'.
        //
        //: 1 If 'T' is the same as 'FT' or 'FT&', invoking 'f(obj, args...)'
        //:   yields the same results as invoking '(obj.*fp)(args...)'.
        //: 2 If 'T' is the same as 'T*' or "smart pointer" to 'T', invoking
        //:   'f(obj, args...)'  yields the same results as invoking
        //:   '((*obj).*fp)(args...)'.
        //: 3 Concerns 1 and 2 also apply if 'T' is an rvalue of, reference to,
        //:   pointer to, or smart-pointer to type derived from 'FT'.
        //: 4 If 'fp' is a pointer to const member function, then 'T' can be
        //:   rvalue of, reference to, pointer to, or smart-pointer to either
        //:   a const or a non-const type.  All of the above concerns apply to
        //:   both const and non-const member functions.
        //: 5 Invocation works for zero to nine arguments, 'args...' in
        //:   addition to the 'obj' argument and yields the expected return
        //:   value.
        //: 6 The template argument types 'ARGS...' need not match the
        //:   member-function arguments 'FARGS...' exactly, so long as the
        //:   argument lists are the same length and each type in 'ARGS' is
        //:   implicitly convertible to the corresponding argument in
        //:   'FARGS'.
        //: 7 The return type 'RET' need not match the member-function return
        //:   type 'FRET' so long as 'RET' is implicitly convertible to
        //:   'FRET'.
        //: 8 If 'RET' is 'void', then the return value of 'pf' is discarded,
        //:   even if 'FRET' is non-void.
        //
        // Plan:
        //: 1 Create a class 'IntWrapper' that holds an 'int' value and has
        //:   const member functions 'add0' to 'add9' and non-const member
        //:   functions 'increment0' to 'increment9' taking 0 to 9 'int'
        //:   arguments.  The 'sum[0-9]' functions return the 'int' sum of the
        //:   arguments + the wrapper's value.  The 'increment[0-9]' functions
        //:   increment the wrapper's value by the sum of the arguments and
        //:   returns the 'int' result.
        //: 2 For concerns 1 and 5, create and invoke 'bsl::function's
        //:   wrapping pointers to each of the 'IntWrapper' member functions
        //:   'increment0' to 'increment9', using 'T' template parameters of
        //:   both types 'IntWrapper', and 'IntWrapper&'.  Verify that the
        //:   return and side-effects from the invocations matches the
        //:   expected results.
        //: 3 For concerns 2 and 5, create and invoke 'bsl::function's
        //:   wrapping pointers to each of the 'IntWrapper' member functions
        //:   'increment0' to 'increment9', using 'T' template parameters of
        //:   both types 'IntWrapper*', and 'SmartPtr<IntWrapper>'.  Verify
        //:   that the return and side-effects from the invocations matches
        //:   the expected results.  
        //: 4 For concern 3, create a class, 'IntWrapperDerived' derived from
        //:   'IntWrapper' and repeat steps 2 and 3 for at least a few member
        //:   functions using pointer to member function of
        //:   'IntWrapperDerived' instead of pointer to member function of
        //:   ''IntWrapper'.  It is not necessary to try every member function
        //:   to verify that concern 3 is satisfied.
        //: 5 For concern 4, create and invoke 'bsl::function's wrapping
        //:   pointers to a few of the 'IntWrapper' member functions 'sum0' to
        //:   'sum9', using 'T' template parameters of types 'IntWrapper',
        //:   'IntWrapper&', 'IntWrapper*', and 'SmartPtr<IntWrapper>', as
        //:   well as 'const' and 'const IntWrapperDerived' versions of the
        //:   preceding.  Verify that the return value from the invocations
        //:   matches the expected results.  (There should be no
        //:   side-effects.)  It is not necessary to try every member function
        //:   to verify that concern 4 is satisfied.
        //: 6 For concern 6, repeat steps 2 and 3 except using a class
        //:   'ConvertibleToInt' instead of 'int' for the arguments in 'ARGS'.
        //: 7 For concern 7, repeat steps 2 and 3 for just a few argument
        //:   combinations except using 'IntWrapper' for 'RET'.  It is not
        //:   necessary to try every member function to verify that concern 7
        //:   is satisfied.
        //: 8 For concern 8, repeat steps 2 and 3 except using 'void' for
        //:   'RET', thus discarding the return value.  Repeat steps 2 and 3
        //:   again, except using 'void' for 'RET' and using
        //:   'voidIncrement[0-9]' instead of 'increment[0-9]', showing that a
        //:   'void' function can be invoked.
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For pointer to member func
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO MEMBER FUNCTION INVOCATION"
                            "\n=====================================\n");

        IntWrapper iw(0x3001);  const IntWrapper &IW = iw;
        IntWrapperDerived iwd(0x3001);  const IntWrapperDerived &IWD = iwd;

        if (veryVerbose) std::printf("non-const member functions\n");

        // Plan step 2
        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper val\n");
        {
            // Note: because the 'IntWrapper' argument is pass-by-value, a
            // temp copy is passed to the increment member function, leaving
            // the original value of 'iw' unchanged.

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper)> f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(IW));
            ASSERT(0x2001 == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(IW, 2));   // Modify a temp copy of 'iw'
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int,
                              int)> f3(&IntWrapper::increment2);
            ASSERT(0x2007 == f3(IW, 2, 4)); // Modify a temp copy of 'iw'
            ASSERT(0x2001 == IW.value());   // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int,
                              int)> f4(&IntWrapper::increment3);
            ASSERT(0x200f == f4(IW, 2, 4, 8)); // Modify a temp copy of 'iw'
            ASSERT(0x2001 == IW.value());      // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int, int,
                              int)> f5(&IntWrapper::increment4);
            ASSERT(0x201f == f5(IW, 2, 4, 8, 0x10)); 
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int, int, int,
                              int)> f6(&IntWrapper::increment5);
            ASSERT(0x203f == f6(IW, 2, 4, 8, 0x10, 0x20));
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int, int, int, int,
                              int)> f7(&IntWrapper::increment6);
            ASSERT(0x207f == f7(IW, 2, 4, 8, 0x10, 0x20, 0x40));
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int, int, int, int, int,
                              int)> f8(&IntWrapper::increment7);
            ASSERT(0x20ff == f8(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int, int, int, int, int,
                              int, int)> f9(&IntWrapper::increment8);
            ASSERT(0x21ff == f9(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x2001 == IW.value());  // Original 'iw' unmodified
        }

        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper ref\n");
        {
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&)> f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(iw));
            ASSERT(0x2001 == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(iw, 2));
            ASSERT(0x2003 == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int,
                              int)> f3(&IntWrapper::increment2);
            ASSERT(0x2007 == f3(iw, 2, 4));
            ASSERT(0x2007 == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int,
                              int)> f4(&IntWrapper::increment3);
            ASSERT(0x200f == f4(iw, 2, 4, 8));
            ASSERT(0x200f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int, int,
                              int)> f5(&IntWrapper::increment4);
            ASSERT(0x201f == f5(iw, 2, 4, 8, 0x10));
            ASSERT(0x201f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int, int, int,
                              int)> f6(&IntWrapper::increment5);
            ASSERT(0x203f == f6(iw, 2, 4, 8, 0x10, 0x20));
            ASSERT(0x203f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int, int, int, int,
                              int)> f7(&IntWrapper::increment6);
            ASSERT(0x207f == f7(iw, 2, 4, 8, 0x10, 0x20, 0x40));
            ASSERT(0x207f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int, int, int, int, int,
                              int)> f8(&IntWrapper::increment7);
            ASSERT(0x20ff == f8(iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
            ASSERT(0x20ff == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int, int, int, int, int,
                              int, int)> f9(&IntWrapper::increment8);
            ASSERT(0x21ff == f9(iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));
            ASSERT(0x21ff == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper&, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x23ff == IW.value());
        }

        // Plan step 3
        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper ptr\n");
        {
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*)> f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(&iw));
            ASSERT(0x2001 == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(&iw, 2));
            ASSERT(0x2003 == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int,
                              int)> f3(&IntWrapper::increment2);
            ASSERT(0x2007 == f3(&iw, 2, 4));
            ASSERT(0x2007 == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int,
                              int)> f4(&IntWrapper::increment3);
            ASSERT(0x200f == f4(&iw, 2, 4, 8));
            ASSERT(0x200f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int, int,
                              int)> f5(&IntWrapper::increment4);
            ASSERT(0x201f == f5(&iw, 2, 4, 8, 0x10));
            ASSERT(0x201f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int, int, int,
                              int)> f6(&IntWrapper::increment5);
            ASSERT(0x203f == f6(&iw, 2, 4, 8, 0x10, 0x20));
            ASSERT(0x203f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int, int, int, int,
                              int)> f7(&IntWrapper::increment6);
            ASSERT(0x207f == f7(&iw, 2, 4, 8, 0x10, 0x20, 0x40));
            ASSERT(0x207f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int, int, int, int, int,
                              int)> f8(&IntWrapper::increment7);
            ASSERT(0x20ff == f8(&iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
            ASSERT(0x20ff == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int, int, int, int, int,
                              int, int)> f9(&IntWrapper::increment8);
            ASSERT(0x21ff == f9(&iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));
            ASSERT(0x21ff == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(IntWrapper*, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(&iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x23ff == IW.value());
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via SmartPtr<IntWrapper>\n");
        {
            SmartPtr<IntWrapper> iw_p(&iw);

            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>)>
                f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(iw_p));
            ASSERT(0x2001 == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>,
                              int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(iw_p, 2));
            ASSERT(0x2003 == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int,
                              int)> f3(&IntWrapper::increment2);
            ASSERT(0x2007 == f3(iw_p, 2, 4));
            ASSERT(0x2007 == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int,
                              int)> f4(&IntWrapper::increment3);
            ASSERT(0x200f == f4(iw_p, 2, 4, 8));
            ASSERT(0x200f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int,
                              int)> f5(&IntWrapper::increment4);
            ASSERT(0x201f == f5(iw_p, 2, 4, 8, 0x10));
            ASSERT(0x201f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int)> f6(&IntWrapper::increment5);
            ASSERT(0x203f == f6(iw_p, 2, 4, 8, 0x10, 0x20));
            ASSERT(0x203f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int)> f7(&IntWrapper::increment6);
            ASSERT(0x207f == f7(iw_p, 2, 4, 8, 0x10, 0x20, 0x40));
            ASSERT(0x207f == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int, int)> f8(&IntWrapper::increment7);
            ASSERT(0x20ff == f8(iw_p, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
            ASSERT(0x20ff == IW.value());
          
            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int, int, int)> f9(&IntWrapper::increment8);
            ASSERT(0x21ff == f9(iw_p, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));
            ASSERT(0x21ff == IW.value());

            iw = IntWrapper(0x2001);
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int, int, int,
                              int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(iw_p, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x23ff == IW.value());
        }

        // Plan step 4
        if (veryVeryVerbose) std::printf(
            "\tInvoked via IntWrapperDerived val\n");
        {
            // Note: because the 'IntWrapperDerived' argument is
            // pass-by-value, a temp copy is passed to the increment member
            // function, leaving the original value of 'iwd' unchanged.

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived)> f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(IWD));
            ASSERT(0x2001 == IWD.value());

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived,
                              int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(IWD, 2));   // Modify a temp copy of 'iwd'
            ASSERT(0x2001 == IWD.value());  // Original 'iwd' unmodified
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(IWD, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x2001 == IWD.value());  // Original 'iwd' unmodified
        }
        
        if (veryVeryVerbose) std::printf(
            "\tInvoked via IntWrapperDerived ref\n");
        {
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived&)> f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(iwd));
            ASSERT(0x2001 == IWD.value());

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived&,
                              int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(iwd, 2));
            ASSERT(0x2003 == IWD.value());
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived&, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(iwd, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x23ff == IWD.value());
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via IntWrapperDerived ptr\n");
        {
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived*)> f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(&iwd));
            ASSERT(0x2001 == IWD.value());

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived*,
                              int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(&iwd, 2));
            ASSERT(0x2003 == IWD.value());
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived*, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(&iwd, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x23ff == IWD.value());
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via SmartPtr<IntWrapperDerived>\n");
        {
            SmartPtr<IntWrapperDerived> iwd_p(&iwd);

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(SmartPtr<IntWrapperDerived>)>
                f1(&IntWrapper::increment0);
            ASSERT(0x2001 == f1(iwd_p));
            ASSERT(0x2001 == IWD.value());

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(SmartPtr<IntWrapperDerived>,
                              int)> f2(&IntWrapper::increment1);
            ASSERT(0x2003 == f2(iwd_p, 2));
            ASSERT(0x2003 == IWD.value());
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(SmartPtr<IntWrapperDerived>, int, int, int, int,
                              int, int, int, int,
                              int)> f10(&IntWrapper::increment9);
            ASSERT(0x23ff == f10(iwd_p, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
            ASSERT(0x23ff == IWD.value());
        }

        // Plan step 5
        if (veryVerbose) std::printf("const member functions\n");

        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper val\n");
        {
            iw = IntWrapper(0x3001);

            bsl::function<int(IntWrapper)> f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(IW));

            bsl::function<int(IntWrapper, int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(IW, 2));
          
            bsl::function<int(IntWrapper, int,
                              int)> f3(&IntWrapper::add2);
            ASSERT(0x3007 == f3(IW, 2, 4));
          
            bsl::function<int(IntWrapper, int, int,
                              int)> f4(&IntWrapper::add3);
            ASSERT(0x300f == f4(IW, 2, 4, 8));
          
            bsl::function<int(IntWrapper, int, int, int,
                              int)> f5(&IntWrapper::add4);
            ASSERT(0x301f == f5(IW, 2, 4, 8, 0x10));
          
            bsl::function<int(IntWrapper, int, int, int, int,
                              int)> f6(&IntWrapper::add5);
            ASSERT(0x303f == f6(IW, 2, 4, 8, 0x10, 0x20));
          
            bsl::function<int(IntWrapper, int, int, int, int, int,
                              int)> f7(&IntWrapper::add6);
            ASSERT(0x307f == f7(IW, 2, 4, 8, 0x10, 0x20, 0x40));
          
            bsl::function<int(IntWrapper, int, int, int, int, int, int,
                              int)> f8(&IntWrapper::add7);
            ASSERT(0x30ff == f8(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
          
            bsl::function<int(IntWrapper, int, int, int, int, int, int,
                              int, int)> f9(&IntWrapper::add8);
            ASSERT(0x31ff == f9(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            bsl::function<int(IntWrapper, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper ref\n");
        {
            iw = IntWrapper(0x3001);

            bsl::function<int(IntWrapper&)> f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(iw));

            bsl::function<int(IntWrapper&, int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(iw, 2));
          
            bsl::function<int(IntWrapper&, int,
                              int)> f3(&IntWrapper::add2);
            ASSERT(0x3007 == f3(iw, 2, 4));
          
            bsl::function<int(IntWrapper&, int, int,
                              int)> f4(&IntWrapper::add3);
            ASSERT(0x300f == f4(iw, 2, 4, 8));
          
            bsl::function<int(IntWrapper&, int, int, int,
                              int)> f5(&IntWrapper::add4);
            ASSERT(0x301f == f5(iw, 2, 4, 8, 0x10));
          
            bsl::function<int(IntWrapper&, int, int, int, int,
                              int)> f6(&IntWrapper::add5);
            ASSERT(0x303f == f6(iw, 2, 4, 8, 0x10, 0x20));
          
            bsl::function<int(IntWrapper&, int, int, int, int, int,
                              int)> f7(&IntWrapper::add6);
            ASSERT(0x307f == f7(iw, 2, 4, 8, 0x10, 0x20, 0x40));
          
            bsl::function<int(IntWrapper&, int, int, int, int, int, int,
                              int)> f8(&IntWrapper::add7);
            ASSERT(0x30ff == f8(iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
          
            bsl::function<int(IntWrapper&, int, int, int, int, int, int,
                              int, int)> f9(&IntWrapper::add8);
            ASSERT(0x31ff == f9(iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            bsl::function<int(IntWrapper&, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper ptr\n");
        {
            iw = IntWrapper(0x3001);

            bsl::function<int(IntWrapper*)> f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(&iw));

            bsl::function<int(IntWrapper*, int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(&iw, 2));
          
            bsl::function<int(IntWrapper*, int,
                              int)> f3(&IntWrapper::add2);
            ASSERT(0x3007 == f3(&iw, 2, 4));
          
            bsl::function<int(IntWrapper*, int, int,
                              int)> f4(&IntWrapper::add3);
            ASSERT(0x300f == f4(&iw, 2, 4, 8));
          
            bsl::function<int(IntWrapper*, int, int, int,
                              int)> f5(&IntWrapper::add4);
            ASSERT(0x301f == f5(&iw, 2, 4, 8, 0x10));
          
            bsl::function<int(IntWrapper*, int, int, int, int,
                              int)> f6(&IntWrapper::add5);
            ASSERT(0x303f == f6(&iw, 2, 4, 8, 0x10, 0x20));
          
            bsl::function<int(IntWrapper*, int, int, int, int, int,
                              int)> f7(&IntWrapper::add6);
            ASSERT(0x307f == f7(&iw, 2, 4, 8, 0x10, 0x20, 0x40));
          
            bsl::function<int(IntWrapper*, int, int, int, int, int, int,
                              int)> f8(&IntWrapper::add7);
            ASSERT(0x30ff == f8(&iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
          
            bsl::function<int(IntWrapper*, int, int, int, int, int, int,
                              int, int)> f9(&IntWrapper::add8);
            ASSERT(0x31ff == f9(&iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            bsl::function<int(IntWrapper*, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(&iw, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via SmartPtr<IntWrapper>\n");
        {
            iw = IntWrapper(0x3001);
            const SmartPtr<IntWrapper> IW_P(&iw);

            bsl::function<int(SmartPtr<IntWrapper>)>
                f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(IW_P));

            bsl::function<int(SmartPtr<IntWrapper>,
                              int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(IW_P, 2));
          
            bsl::function<int(SmartPtr<IntWrapper>, int,
                              int)> f3(&IntWrapper::add2);
            ASSERT(0x3007 == f3(IW_P, 2, 4));
          
            bsl::function<int(SmartPtr<IntWrapper>, int, int,
                              int)> f4(&IntWrapper::add3);
            ASSERT(0x300f == f4(IW_P, 2, 4, 8));
          
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int,
                              int)> f5(&IntWrapper::add4);
            ASSERT(0x301f == f5(IW_P, 2, 4, 8, 0x10));
          
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int)> f6(&IntWrapper::add5);
            ASSERT(0x303f == f6(IW_P, 2, 4, 8, 0x10, 0x20));
          
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int)> f7(&IntWrapper::add6);
            ASSERT(0x307f == f7(IW_P, 2, 4, 8, 0x10, 0x20, 0x40));
          
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int, int)> f8(&IntWrapper::add7);
            ASSERT(0x30ff == f8(IW_P, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
          
            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int, int, int)> f9(&IntWrapper::add8);
            ASSERT(0x31ff == f9(IW_P, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));

            bsl::function<int(SmartPtr<IntWrapper>, int, int, int, int,
                              int, int, int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(IW_P, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf("\tInvoked via IntWrapper val\n");
        {
            iw = IntWrapper(0x3001);

            bsl::function<int(const IntWrapper)> f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(IW));

            bsl::function<int(const IntWrapper, int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(IW, 2));
          
            bsl::function<int(const IntWrapper, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via const IntWrapper ref\n");
        {
            bsl::function<int(const IntWrapper&)> f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(IW));

            bsl::function<int(const IntWrapper&, int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(IW, 2));
          
            bsl::function<int(const IntWrapper&, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via const IntWrapper ptr\n");
        {
            bsl::function<int(const IntWrapper*)> f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(&IW));

            bsl::function<int(const IntWrapper*, int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(&IW, 2));
          
            bsl::function<int(const IntWrapper*, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(&IW, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via SmartPtr<const IntWrapper>\n");
        {
            iw = IntWrapper(0x3001);
            const SmartPtr<const IntWrapper> IW_P(&IW);

            bsl::function<int(SmartPtr<const IntWrapper>)>
                f1(&IntWrapper::add0);
            ASSERT(0x3001 == f1(IW_P));

            bsl::function<int(SmartPtr<const IntWrapper>,
                              int)> f2(&IntWrapper::add1);
            ASSERT(0x3003 == f2(IW_P, 2));
          
            bsl::function<int(SmartPtr<const IntWrapper>, int, int, int, int,
                              int, int, int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x33ff == f10(IW_P, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via IntWrapperDerived val\n");
        {
            // Note: because the 'IntWrapperDerived' argument is
            // pass-by-value, a temp copy is passed to the increment member
            // function, leaving the original value of 'iwd' unchanged.

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(const IntWrapperDerived)> f1(&IntWrapper::add0);
            ASSERT(0x2001 == f1(IWD));

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived,
                              int)> f2(&IntWrapper::add1);
            ASSERT(0x2003 == f2(IWD, 2));   // Modify a temp copy of 'iwd'
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x23ff == f10(IWD, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }
        
        if (veryVeryVerbose) std::printf(
            "\tInvoked via IntWrapperDerived ref\n");
        {
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(const IntWrapperDerived&)> f1(&IntWrapper::add0);
            ASSERT(0x2001 == f1(IWD));

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived&,
                              int)> f2(&IntWrapper::add1);
            ASSERT(0x2003 == f2(iwd, 2));
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived&, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x23ff == f10(iwd, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via IntWrapperDerived ptr\n");
        {
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(const IntWrapperDerived*)> f1(&IntWrapper::add0);
            ASSERT(0x2001 == f1(&IWD));

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived*,
                              int)> f2(&IntWrapper::add1);
            ASSERT(0x2003 == f2(&iwd, 2));
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(IntWrapperDerived*, int, int, int, int, int, int,
                              int, int, int)> f10(&IntWrapper::add9);
            ASSERT(0x23ff == f10(&iwd, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

        if (veryVeryVerbose) std::printf(
            "\tInvoked via SmartPtr<IntWrapperDerived>\n");
        {
            SmartPtr<IntWrapperDerived> iwd_p(&iwd);
            SmartPtr<const IntWrapperDerived> iwd_P(&IWD);

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(SmartPtr<const IntWrapperDerived>)>
                f1(&IntWrapper::add0);
            ASSERT(0x2001 == f1(iwd_P));

            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(SmartPtr<IntWrapperDerived>,
                              int)> f2(&IntWrapper::add1);
            ASSERT(0x2003 == f2(iwd_p, 2));
          
            iwd = IntWrapperDerived(0x2001);
            bsl::function<int(SmartPtr<IntWrapperDerived>, int, int, int, int,
                              int, int, int, int,
                              int)> f10(&IntWrapper::add9);
            ASSERT(0x23ff == f10(iwd_p, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,
                                 0x200));
        }

      } break;

      case 4: {
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
        //: 5 A 'bsl::function' that returns void can wrap a pointer to a
        //:   function that returns non-void.
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
        //: 3 For concern 3, pass an 'int' as the first argument of the
        //:   invocations in step 2, forcing conversion to 'IntWrapper'.
        //:   Create a 'bsl::function' object returning an 'IntWrapper' and
        //:   verify that it can successfully wrap a pointer to 'sum1'.
        //: 4 For concern 4, create a global function, 'increment' that
        //:   increments its argument (passed by address) and returns void.
        //:   Verify that a 'bsl::function' object wrappoing a pointer to
        //:   'increment' can be invoked and has the expected size-effect.
        //: 5 For concern 5, create a 'bsl::function' with parameter
        //:   'void(int)' and verify that it can be used to wrap 'sum1'
        //:   (discarding the return value).
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For pointer to function
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO FUNCTION INVOCATION"
                            "\n==============================\n");

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
          
        bsl::function<int(int, int, int, int, int, int, int, int)> f8(sum8);
        ASSERT(0x40ff == f8(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80));
          
        bsl::function<int(int,int,int,int,int,int,int,int,int)> f9(sum9);
        ASSERT(0x41ff == f9(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100));
          
        bsl::function<int(int,int,int,int,int,int,int,int,int,int)> f10(sum10);
        ASSERT(0x43ff == f10(1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, 0x100,0x200));
          
        // Test void return type
        bsl::function<void(int*)> fvoid(&increment);
        int v = 1;
        fvoid(&v);
        ASSERT(2 == v);

        // Test discarding of return value
        bsl::function<void(int)> fdiscard(&sum1);
        fdiscard(3);

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
        //:  5 No memory is allocated unless the invocable is too large for
        //:    the small-object optimization.
        //:  6 The destructor releases allocated memory, if any.
        //:  7 For a non-empty 'function', the 'target_type' accessor returns
        //:    the 'type_info' of the invocable specified at construction.
        //:  8 For a non-empty 'function', the 'target' accessor returns a
        //:    cv-qualified pointer to a copy of the invocable specified at
        //:    construction.
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
        //:    various sized functors, verify that only functors that are too
        //:    large for the small-object optimization result in any
        //:    allocations, and that those large functors result result in
        //:    exactly one block being allocated.  Verify that the destructor
        //:    releases any allocated memory.
        //:  5 For concern 7, verify that the return value of 'target_type'
        //:    matches the expected 'type_info'.
        //:  6 For concern 8, verify that, for the non-empty 'function'
        //:    objects, the return value of 'target' is a non-null pointer
        //:    pointing to a copy of the invocable used to construct the
        //:    'function' object.
        //:  7 Note that the semantics and implementation of the operations
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
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR function(FUNC)"
                            "\n==========================\n");

        typedef bsl::function<int(const IntWrapper&, int)> Obj;
        typedef int (*simpleFuncPtr_t)(const IntWrapper&, int);
        typedef int (IntWrapper::*simpleMemFuncPtr_t)(int) const;

        bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

        if (veryVerbose) printf("Construct with null pointer to function\n");
        globalAllocMonitor.reset();
        {
            const simpleFuncPtr_t nullFuncPtr = NULL;
            Obj f(nullFuncPtr); const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(NULL == F.target<simpleFuncPtr_t>());
            ASSERT(NULL == f.target<simpleFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) {
            printf("Construct with null pointer to member function\n");
        }
        globalAllocMonitor.reset();
        {
            const simpleMemFuncPtr_t nullMemFuncPtr = NULL;
            Obj f(nullMemFuncPtr); const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(NULL == F.target<simpleMemFuncPtr_t>());
            ASSERT(NULL == f.target<simpleMemFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with pointer to function\n");
        globalAllocMonitor.reset();
        {
            Obj f(simpleFunc); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(simpleFuncPtr_t) == F.target_type());
            ASSERT(F.target<simpleFuncPtr_t>() &&
                   &simpleFunc == *F.target<simpleFuncPtr_t>());
            ASSERT(f.target<simpleFuncPtr_t>() &&
                   &simpleFunc == *f.target<simpleFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with pointer to member function\n");
        globalAllocMonitor.reset();
        {
            Obj f(&IntWrapper::add1); const Obj& F = f;
            ASSERT(F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(simpleMemFuncPtr_t) == F.target_type());
            ASSERT(F.target<simpleMemFuncPtr_t>() &&
                   &IntWrapper::add1 == *F.target<simpleMemFuncPtr_t>());
            ASSERT(f.target<simpleMemFuncPtr_t>() &&
                   &IntWrapper::add1 == *f.target<simpleMemFuncPtr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
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
        }
        ASSERT(globalAllocMonitor.isInUseSame());

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //
        // Concerns:
        //:  1 Default construction, construction using a nullptr_t(), and
        //:    construction using a null pointer each create an empty
        //:    'function' object.
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

        typedef bsl::function<int(const IntWrapper&, int)> Obj;

        bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

        if (veryVerbose) printf("Construct with no arguments\n");
        globalAllocMonitor.reset();
        {
            Obj f; const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(  isConstPtr(F.target<nullptr_t>()));
            ASSERT(! isConstPtr(f.target<nullptr_t>()));
            ASSERT(NULL == F.target<nullptr_t>());
            ASSERT(NULL == f.target<nullptr_t>());
            ASSERT(&globalTestAllocator == f.allocator());
        }
        ASSERT(globalAllocMonitor.isInUseSame());

        if (veryVerbose) printf("Construct with nullptr_t argument\n");
        globalAllocMonitor.reset();
        {
            const nullptr_t np = nullptr_t();
            Obj f(np); const Obj& F = f;
            ASSERT(! F);
            ASSERT(globalAllocMonitor.isTotalSame());
            ASSERT(typeid(void) == F.target_type());
            ASSERT(NULL == F.target<nullptr_t>());
            ASSERT(NULL == f.target<nullptr_t>());
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
        // Plan: Exercise the basic functionality of this component.
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

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
