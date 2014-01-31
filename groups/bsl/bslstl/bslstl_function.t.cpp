// bslstl_function.t.cpp                  -*-C++-*-

#include "bslstl_function.h"

#include <bslmf_issame.h>
#include <bslmf_removeconst.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_defaultallocatorguard.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

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
// allocator is not. Thus, whether or not the small-object optimization is
// applied to the allocator can be different in the original object vs. the
// copy.
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

    // Mutable function with 0 to 9 arguments.  Increment the value by
    // the sum of all arguments.  'increment0()' is a no-op.
    int increment0() { return d_value; }
    BSLS_MACROREPEAT(9, INCREMENT_FUNC)

    void voidIncrement0() { }
    BSLS_MACROREPEAT(9, VOID_INCREMENT_FUNC)
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
// that, since 'cc' is passed by value, there is a copy or move on every
// call. Therefore, the return value should never be less than 1.
int numCopies(CountCopies cc) { return cc.numCopies(); }

struct SmallObjectBuffer {
    // For white-box testing: size of small-object buffer in 'function'.
    void *d_padding[4];  // Size of 4 pointers
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

    enum { IS_STATELESS = true };

    explicit EmptyFunctor(int /* ignored */ = 0)
        // Constructor writes '0xee' over its memory.  Even an empty class has
        // a footprint and any code that optimizes the object away must be
        // careful not to overwrite arbitrary memory by calling the "empty"
        // constructor.
        { std::memset(this, 0xee, sizeof(*this)); }

    ~EmptyFunctor()
        // Destructor writes '0xbb' over its memory.  Even an empty class has
        // a footprint and any code that optimizes the object away must be
        // careful not to overwrite arbitrary memory by calling the "empty"
        // destructor.
        { std::memset(this, 0xbb, sizeof(*this)); }

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

    // Invocation operator that returns void.
    void operator()(const char*) { }

    int value() const { return 0; }

    friend bool operator==(const EmptyFunctor&, const EmptyFunctor&)
        { return true; }

    friend bool operator!=(const EmptyFunctor&, const EmptyFunctor&)
        { return false; }
};

class SmallFunctor
{
    // Small stateful functor.

    int d_value;  // Arbitrary state to distinguish one instance from another

public:
    enum { IS_STATELESS = false };

    explicit SmallFunctor(int v) : d_value(v) { }
    ~SmallFunctor() { std::memset(this, 0xbb, sizeof(*this)); }

#define OP_PAREN(n)                                                           \
    int operator()(const IntWrapper& iw, BSLS_MACROREPEAT_COMMA(n, INT_ARGN)) \
    {                                                                         \
        return d_value += iw.value() + BSLS_MACROREPEAT_SEP(n, ARGN, +);      \
    }

    // Invocation operator with 0 to 10 arguments.  The first argument (if
    // any) is a const reference to 'IntWrapper', although it can be passed an
    // 'int'.  The remaining arguments are of type 'int'.
    int operator()() { return d_value; }
    int operator()(const IntWrapper& iw) { return d_value += iw.value(); }
    BSLS_MACROREPEAT(9, OP_PAREN)

#undef OP_PAREN

    // Invocation operator that sets a the functor's value and returns void.
    // To ensure unambiguous overloading resolution, the argument is passed as
    // a null-terminated string, not as an integer.
    void operator()(const char* s) { d_value = std::atoi(s); }

    int value() const { return d_value; }

    friend bool operator==(const SmallFunctor& a, const SmallFunctor& b)
        { return a.value() == b.value(); }

    friend bool operator!=(const SmallFunctor& a, const SmallFunctor& b)
        { return a.value() != b.value(); }
};

class MediumFunctor : public SmallFunctor
{
    // Functor that barely fits into the small object buffer.

    char d_padding[sizeof(SmallObjectBuffer) - sizeof(SmallFunctor)];
    
public:
    explicit MediumFunctor(int v) : SmallFunctor(v)
        { std::memset(d_padding, 0xee, sizeof(d_padding)); }

    ~MediumFunctor() { std::memset(this, 0xbb, sizeof(*this)); }

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
    explicit LargeFunctor(int v) : SmallFunctor(v)
        { std::memset(d_padding, 0xee, sizeof(d_padding)); }

    ~LargeFunctor() { std::memset(this, 0xbb, sizeof(*this)); }

    friend bool operator==(const LargeFunctor& a, const LargeFunctor& b)
        { return a.value() == b.value(); }

    friend bool operator!=(const LargeFunctor& a, const LargeFunctor& b)
        { return a.value() != b.value(); }
};

// Using the curiously-recurring template pattern with a template template
// parameter, we can implement the boiler-plate part of an STL-style
// allocator.
template <class TYPE, template <class T> class ALLOC>
class STLAllocatorBase
{
    // Define the boilerplate for the specified 'ALLOC' allocator.  The
    // minimum requirement for 'ALLOC' is that it have a 'bslmaAllocator'
    // method that returns a pointer to a 'bslma::Allocator' used to allocate
    // the underlying memory.

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

// Test allocator used by all instances of 'EmptySTLAllocator'
bslma::TestAllocator *emptySTLAllocSource = NULL;

template <class TYPE>
class EmptySTLAllocator : public STLAllocatorBase<TYPE, EmptySTLAllocator>
{

public:
    explicit EmptySTLAllocator(bslma::TestAllocator *ta)
        { emptySTLAllocSource = ta; }

    template <class U>
    EmptySTLAllocator(const EmptySTLAllocator<U>&) { }

    TYPE *allocate(std::size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) emptySTLAllocSource->allocate(n * sizeof(TYPE));
    }
  
    void deallocate(TYPE *p, std::size_t) {
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

// Test allocator used by all instances of 'EmptySTLAllocator2'
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

    TYPE *allocate(std::size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) emptySTLAlloc2Source->allocate(n * sizeof(TYPE));
    }
  
    void deallocate(TYPE *p, std::size_t) {
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

    TYPE *allocate(std::size_t n, void* = 0 /* nullptr */) {
        return (TYPE*) d_mechanism->allocate(n * sizeof(TYPE));
    }
  
    void deallocate(TYPE *p, std::size_t) {
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
class TinySTLAllocator :
    public StatefulAllocatorBase<TYPE>,
    public STLAllocatorBase<TYPE, TinySTLAllocator>
{
    // Smallest stateful allocator.

public:
    explicit TinySTLAllocator(bslma::TestAllocator *mechanism)
        : StatefulAllocatorBase<TYPE>(mechanism) { }

    template <class U>
    TinySTLAllocator(const TinySTLAllocator<U>& other)
        : StatefulAllocatorBase<TYPE>(other.mechanism()) { }
};

template <class TYPE>
class SmallSTLAllocator :
    public StatefulAllocatorBase<TYPE>,
    public STLAllocatorBase<TYPE, SmallSTLAllocator>
{
    // Allocator that is small enough to fit in the SmallObjectBuffer
    // alongside SmallFunctor.

    typedef bslma::AllocatorAdaptor<TinySTLAllocator<TYPE> > Adaptor;
    char d_padding[sizeof(SmallObjectBuffer) - sizeof(Adaptor) -
                   sizeof(SmallFunctor)];
public:
    explicit SmallSTLAllocator(bslma::TestAllocator *mechanism)
        : StatefulAllocatorBase<TYPE>(mechanism) { }

    template <class U>
    SmallSTLAllocator(const SmallSTLAllocator<U>& other)
        : StatefulAllocatorBase<TYPE>(other.mechanism()) { }
};

template <class TYPE>
class MediumSTLAllocator :
    public StatefulAllocatorBase<TYPE>,
    public STLAllocatorBase<TYPE, MediumSTLAllocator>
{
    // Allocator that is small enough to fit in the SmallObjectBuffer
    // by itself or with a stateless functor.

    typedef bslma::AllocatorAdaptor<TinySTLAllocator<TYPE> > Adaptor;
    char d_padding[sizeof(SmallObjectBuffer) - sizeof(Adaptor)];
public:
    explicit MediumSTLAllocator(bslma::TestAllocator *mechanism)
        : StatefulAllocatorBase<TYPE>(mechanism) { }

    template <class U>
    MediumSTLAllocator(const MediumSTLAllocator<U>& other)
        : StatefulAllocatorBase<TYPE>(other.mechanism()) { }
};

template <class TYPE>
class LargeSTLAllocator :
    public StatefulAllocatorBase<TYPE>,
    public STLAllocatorBase<TYPE, LargeSTLAllocator>
{
    // Allocator that is too large to fit in the SmallObjectBuffer.

    typedef bslma::AllocatorAdaptor<TinySTLAllocator<TYPE> > Adaptor;
    char d_padding[sizeof(SmallObjectBuffer) - sizeof(Adaptor) + 1];
public:
    explicit LargeSTLAllocator(bslma::TestAllocator *mechanism)
        : StatefulAllocatorBase<TYPE>(mechanism) { }

    template <class U>
    LargeSTLAllocator(const LargeSTLAllocator<U>& other)
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

template <class T>
class ValueGeneratorBase {
    // Generates and values for test driver
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
    // Specialization for lvalues of type 'T'
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

template <class T, class RET, class ARG>
void testPtrToMemFunc(const char *prototypeStr)
    // Test invocation of pointer to member function wrapper.
    // Tests using non-const member functions 'IntWrapper::increment[0-9]'
{
    if (veryVeryVerbose) std::printf("\t%s\n", prototypeStr);

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
}

template <class T, class RET, class ARG>
void testPtrToConstMemFunc(const char *prototypeStr)
    // Test invocation of pointer to const member function wrapper.  Tests
    // using const member functions 'IntWrapper::add[0-9]'.  To save compile
    // time, since 'testPtrToMemFunc' already tests every possible
    // argument-list length, we test only a small number of possible
    // argument-list lengths (specifically 0, 1, and 9 arguments) here.
{
    if (veryVeryVerbose) std::printf("\t%s\n", prototypeStr);

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
}

template <class FUNCTOR, class OBJ>
bool checkValue(const OBJ& obj, int exp)
    // Return true if the functor wrapped within the specified 'obj' has a
    // value that matches the specified 'exp' value and false otherwise.
{
    if (FUNCTOR::IS_STATELESS)
        return true;  // No state to check

    const FUNCTOR& target = *obj.template target<FUNCTOR>();
    return exp == target.value();
}

template <class FUNCTOR, class RET, class ARG>
void testFunctor(const char *prototypeStr)
    // Test invocation of a 'bsl::function' wrapping a functor object.
{
    if (veryVeryVerbose) std::printf("\t%s\n", prototypeStr);

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
    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;
    static const std::size_t k_SIZE = (bsl::is_empty<ALLOC>::value ?
                                       0 : sizeof(Adaptor));
    static const std::size_t k_MAX_OVERHEAD =
        2 * sizeof(bsls::AlignmentUtil::MaxAlignedType);
    static bool sameAlloc(const ALLOC& a, bslma::Allocator *b) {
        Adaptor *adaptor = dynamic_cast<Adaptor*>(b);
        return adaptor != NULL && a == adaptor->adaptedAllocator();
    }
};

template <class T>
struct CheckAlloc<bsl::allocator<T> >
{
    static const std::size_t k_SIZE = 0;
    static const std::size_t k_MAX_OVERHEAD = 0;
    static bool sameAlloc(const bsl::allocator<T>& a, bslma::Allocator *b) {
        return a.mechanism() == b;
    }
};

template <class BA>
struct CheckAlloc<BA*>
{
    static const std::size_t k_SIZE = 0;
    static const std::size_t k_MAX_OVERHEAD = 0;
    static bool sameAlloc(bslma::Allocator *a, bslma::Allocator *b) {
        return a == b;
    }
};
    
enum WhatIsInplace {
    e_INPLACE_BOTH,      // Both function and allocator are in place
    e_INPLACE_FUNC_ONLY, // Function is in place, allocator is out of place
    e_OUTOFPLACE_BOTH    // Both function and allocator are out of place
};

template <class ALLOC, class FUNC>
void testFuncWithAlloc(FUNC func, WhatIsInplace inplace, const char *allocName)
{
    if (veryVeryVerbose) printf("\tALLOC is %s\n", allocName);

    typedef bsl::function<int(IntWrapper,int)> Obj;

    bool isEmpty = isNullPtr(func);

    const std::size_t funcSize = (bsl::is_empty<FUNC>::value ? 0 :
                                         isEmpty                    ? 0 :
                                         sizeof(FUNC));
    const std::size_t allocSize = CheckAlloc<ALLOC>::k_SIZE;
    const std::size_t maxOverhead = CheckAlloc<ALLOC>::k_MAX_OVERHEAD;

    bslma::TestAllocatorMonitor globalAllocMonitor(&globalTestAllocator);

    bsls::Types::Int64 numBlocksUsed, minBytesUsed, maxBytesUsed;
    
    switch (inplace) {
      case e_INPLACE_BOTH: {
        ASSERT(funcSize + allocSize <= sizeof(SmallObjectBuffer));
        numBlocksUsed = 0;
        minBytesUsed = 0;
        maxBytesUsed = 0;
      } break;

      case e_INPLACE_FUNC_ONLY: {
        ASSERT(funcSize <= sizeof(SmallObjectBuffer));
        ASSERT(funcSize + allocSize > sizeof(SmallObjectBuffer));
        numBlocksUsed = 1;
        minBytesUsed = allocSize;
        maxBytesUsed = minBytesUsed + maxOverhead;
      } break;

      case e_OUTOFPLACE_BOTH: {
        ASSERT(funcSize > sizeof(SmallObjectBuffer));
        numBlocksUsed = 1;
        minBytesUsed = funcSize + allocSize;
        maxBytesUsed = minBytesUsed + maxOverhead;
      } break;
    } // end switch

    bslma::TestAllocator ta;
    globalAllocMonitor.reset();
    {
        ALLOC alloc(&ta);
        Obj f(bsl::allocator_arg, alloc, func);
        ASSERT(isEmpty == !f);
        ASSERT(CheckAlloc<ALLOC>::sameAlloc(alloc, f.allocator()));
        if (f) {
            ASSERT(typeid(func) == f.target_type());
            ASSERT(f.target<FUNC>());
            ASSERT(f.target<FUNC>() && func == *f.target<FUNC>());
            ASSERT(0x4005 == f(IntWrapper(0x4000), 5));
        }
        ASSERT(numBlocksUsed == ta.numBlocksInUse());
        ASSERT(minBytesUsed <= ta.numBytesInUse() &&
               ta.numBytesInUse() <= maxBytesUsed);
        ASSERT(globalAllocMonitor.isInUseSame());
    }
    ASSERT(0 == ta.numBlocksInUse());
    ASSERT(globalAllocMonitor.isInUseSame());
}

template <class ALLOC, class FUNC>
void testCopyCtorWithAlloc(FUNC        func,
                           const bsl::function<int(IntWrapper,int)>& original,
                           const char *originalAllocName,
                           const char *copyAllocName)
{
    if (veryVeryVerbose)
        printf("\tAlloc: orig = %s, copy = %s\n", originalAllocName,
               copyAllocName);

    typedef bsl::function<int(IntWrapper,int)> Obj;

    bool copyAllocNone = (0 == std::strcmp(copyAllocName, "none"));

    bsls::Types::Int64 numBlocksUsed, numBytesUsed;

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

    // Copy-construct 'original' into 'copy' using extended copy contructor.
    {
        // We want to select one of two constructors at run time, so instead
        // of declaring a 'function' object directly, we create a buffer that
        // can hold a 'function' object and construct the 'function' later
        // using the desired constructor.
        union {
            char                                d_bytes[sizeof(Obj)];
            bsls::AlignmentUtil::MaxAlignedType d_align;
        } copyBuf;

        if (copyAllocNone) {
            // copyAllocName is "none".  Choose normal copy constructor with
            // no allocator, but install 'copyTa' as the allocator indirectly
            // by setting the default allocator.
            bslma::DefaultAllocatorGuard guard(&copyTa);
            ::new(copyBuf.d_bytes) Obj(original);
        }
        else {
            // Use extended copy constructor.
            ::new(copyBuf.d_bytes) Obj(bsl::allocator_arg, copyAlloc,
                                       original);
        }

        // 'copyBuf' now hold the copy-constructed 'function'.
        Obj& copy = *reinterpret_cast<Obj*>(copyBuf.d_bytes);
        
        ASSERT(copy.target_type() == original.target_type());
        ASSERT(CheckAlloc<ALLOC>::sameAlloc(copyAlloc, copy.allocator()));
        ASSERT(! copy == ! original)

        if (copy) {

            // Check for faithful copy of functor
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
    ASSERT(copyTa.numBlocksInUse() == 0);
    ASSERT(copyTa.numBytesInUse()  == 0);
    ASSERT(globalAllocMonitor.isInUseSame());
}

template <class ORIGINAL_ALLOC, class FUNC>
void testCopyCtor(FUNC func, const char *originalAllocName)
{
    typedef bsl::function<int(IntWrapper,int)> Obj;

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
    TEST(TinySTLAllocator<char>  );
    TEST(SmallSTLAllocator<char> );
    TEST(MediumSTLAllocator<char>);
    TEST(LargeSTLAllocator<char> );

    ASSERT(originalAllocMonitor.isInUseSame());

#undef TEST

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

      case 9: {
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
        //: 4 If the original 'function' can be invoked, the its can also be
        //:   invoked and will yield the same results.
        //: 5 If the copy constructor is invoked (without an allocator), then
        //:   the copy will use the value of
        //:   'bslma::Default::defaultAllocator()' at the time of the copy.
        //: 6 If the allocator-extended copy constructor is invoked
        //:   argument, then the copy will use the specified allocator.
        //: 7 The memory allocated by this constructor is the same as if the
        //:   copy were created like the original, except using the specified
        //:   allocator (or default allocator if none specified).
        //: 8 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes.
        //: 9 The above concerns apply to allocators arguments which are
        //:   pointers to type derived from 'bslma::Allocator',
        //:   'bsl::allocator' instantiations, stateless STL-style allocators,
        //:   and stateful STL-style allocators of various sizes.  The
        //:   original and copy can use different allocators.
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
        //: 7 For concern 7, construct a function object 'f1' using a specific
        //:   'func' argument and allocator 'a1'.  Construct a seconf function
        //:   object 'f1' using the same 'func' argument and an allocator
        //:   'a2'.  Using the extended copy constructor, create a copy of
        //:   'f1' using allocator 'a2'.  Verify that the memory allocations
        //:   during this construction match those in constructing 'f2'.
        //: 8 For concerns 8 and 9, package all of the previous plan steps
        //:   into a function template 'testCopyCtor', instantiated with a
        //:   functor and allocator.  This test template will create an
        //:   original 'function' object using the passed-in functor and
        //:   allocator and copy it with the copy constructor and with several
        //:   invocations of the extended copy constructor, using allocators
        //:   of all of the types described in concern 9.  Invoke
        //:   'testCopyCtor' with many combinations of functor and allocator
        //:   types so that every category combination is represented.
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

        if (veryVerbose) std::printf("FUNC is nullFuncPtr\n");
        TEST(bslma::TestAllocator *,   nullFuncPtr          );
        TEST(bsl::allocator<char>,     nullFuncPtr          );
        TEST(EmptySTLAllocator<char>,  nullFuncPtr          );
        TEST(TinySTLAllocator<char>,   nullFuncPtr          );
        TEST(SmallSTLAllocator<char>,  nullFuncPtr          );
        TEST(MediumSTLAllocator<char>, nullFuncPtr          );
        TEST(LargeSTLAllocator<char>,  nullFuncPtr          );

        if (veryVerbose) std::printf("FUNC is nullMemFuncPtr\n");
        TEST(bslma::TestAllocator *,   nullMemFuncPtr       );
        TEST(bsl::allocator<char>,     nullMemFuncPtr       );
        TEST(EmptySTLAllocator<char>,  nullMemFuncPtr       );
        TEST(TinySTLAllocator<char>,   nullMemFuncPtr       );
        TEST(SmallSTLAllocator<char>,  nullMemFuncPtr       );
        TEST(MediumSTLAllocator<char>, nullMemFuncPtr       );
        TEST(LargeSTLAllocator<char>,  nullMemFuncPtr       );

        if (veryVerbose) std::printf("FUNC is simpleFunc\n");
        TEST(bslma::TestAllocator *,   simpleFunc           );
        TEST(bsl::allocator<char>,     simpleFunc           );
        TEST(EmptySTLAllocator<char>,  simpleFunc           );
        TEST(TinySTLAllocator<char>,   simpleFunc           );
        TEST(SmallSTLAllocator<char>,  simpleFunc           );
        TEST(MediumSTLAllocator<char>, simpleFunc           );
        TEST(LargeSTLAllocator<char>,  simpleFunc           );

        if (veryVerbose) std::printf("FUNC is &IntWrapper::add1\n");
        TEST(bslma::TestAllocator *,   &IntWrapper::add1    );
        TEST(bsl::allocator<char>,     &IntWrapper::add1    );
        TEST(EmptySTLAllocator<char>,  &IntWrapper::add1    );
        TEST(TinySTLAllocator<char>,   &IntWrapper::add1    );
        TEST(SmallSTLAllocator<char>,  &IntWrapper::add1    );
        TEST(MediumSTLAllocator<char>, &IntWrapper::add1    );
        TEST(LargeSTLAllocator<char>,  &IntWrapper::add1    );

        if (veryVerbose) std::printf("FUNC is EmptyFunctor()\n");
        TEST(bslma::TestAllocator *,   EmptyFunctor()       );
        TEST(bsl::allocator<char>,     EmptyFunctor()       );
        TEST(EmptySTLAllocator<char>,  EmptyFunctor()       );
        TEST(TinySTLAllocator<char>,   EmptyFunctor()       );
        TEST(SmallSTLAllocator<char>,  EmptyFunctor()       );
        TEST(MediumSTLAllocator<char>, EmptyFunctor()       );
        TEST(LargeSTLAllocator<char>,  EmptyFunctor()       );

        if (veryVerbose) std::printf("FUNC is SmallFunctor(0x2000)\n");
        TEST(bslma::TestAllocator *,   SmallFunctor(0x2000) );
        TEST(bsl::allocator<char>,     SmallFunctor(0x2000) );
        TEST(EmptySTLAllocator<char>,  SmallFunctor(0x2000) );
        TEST(TinySTLAllocator<char>,   SmallFunctor(0x2000) );
        TEST(SmallSTLAllocator<char>,  SmallFunctor(0x2000) );
        TEST(MediumSTLAllocator<char>, SmallFunctor(0x2000) );
        TEST(LargeSTLAllocator<char>,  SmallFunctor(0x2000) );

        if (veryVerbose) std::printf("FUNC is MediumFunctor(0x4000)\n");
        TEST(bslma::TestAllocator *,   MediumFunctor(0x4000));
        TEST(bsl::allocator<char>,     MediumFunctor(0x4000));
        TEST(EmptySTLAllocator<char>,  MediumFunctor(0x4000));
        TEST(TinySTLAllocator<char>,   MediumFunctor(0x4000));
        TEST(SmallSTLAllocator<char>,  MediumFunctor(0x4000));
        TEST(MediumSTLAllocator<char>, MediumFunctor(0x4000));
        TEST(LargeSTLAllocator<char>,  MediumFunctor(0x4000));

        if (veryVerbose) std::printf("FUNC is LargeFunctor(0x6000)\n");
        TEST(bslma::TestAllocator *,   LargeFunctor(0x6000) );
        TEST(bsl::allocator<char>,     LargeFunctor(0x6000) );
        TEST(EmptySTLAllocator<char>,  LargeFunctor(0x6000) );
        TEST(TinySTLAllocator<char>,   LargeFunctor(0x6000) );
        TEST(SmallSTLAllocator<char>,  LargeFunctor(0x6000) );
        TEST(MediumSTLAllocator<char>, LargeFunctor(0x6000) );
        TEST(LargeSTLAllocator<char>,  LargeFunctor(0x6000) );

#undef TEST

      } break;

      case 8: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR function(allocator_arg_t, const ALLOC& alloc, FUNC func)
        //
        // Concerns:
        //: 1 Constructing a 'function' using this constructor yields an empty
        //:   'function' object if 'func' is a null pointer to function or
        //:   null pointer to member function.
        //: 2 Constructing a 'function' using this constructor yields an
        //:   invocable 'function' if 'func' is not a null pointer.  Note that
        //:   invocation is only to ensure that that the function was
        //:   constructed succesfully.  It is not necessary to thoroughly test
        //:   all argument-list combinations.
        //: 3 The 'target_type()' accessor will return 'type_id(FUNC)' and the
        //:   'target<FUNC>()' accessor will return a pointer to a function
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
        //:   'FUNC' fits in the small object buffer, no memory is allocated
        //:   by this constructor.
        //: 9 If 'alloc' is other than an STL-style allocator with state and
        //:   'FUNC' does not fit in the small object buffer, one block of
        //:   memory of sufficient size to hold 'FUNC' is allocated from the
        //:   allocator by this constructor.
        //: 10 If 'alloc' is an STL-style allocator such that the allocator
        //:   adaptor and 'FUNC' both fit within the small object buffer, then
        //:   no memory is allocated by this constructor.
        //: 11 If 'alloc' is an STL-style allocator such that the allocator
        //:   adaptor and 'FUNC' do not both fit within the small object
        //:   buffer, then one block of memory is allocated from 'alloc'
        //:   itself.
        //: 12 In step 8, if 'FUNC' by itself fits within the small object
        //:   buffer, then the allocated memory is only large enough to hold
        //:   the allocator adaptor.
        //: 13 In step 8, if 'FUNC' by itself does not fit within the small
        //:   object buffer, then the allocated memory is large enough to hold
        //:   both 'func' and the allocator adaptor.
        //: 14 If memory is allocated, the destructor frees it.
        //: 15 The above concerns apply to 'func' arguments of type pointer to
        //:   function, pointer to member function, or functor types of
        //:   various sizes.
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
        //:   'func' fits into the small object buffer, no memory is allocated
        //:   either from the global allocator or from the allocator used to
        //:   construct the 'function' object.
        //: 9 For concern 9, test the results of steps 2-4 to verify that when
        //:   'func' does not fit into the small object buffer, one block of
        //:   memory of sufficient size to hold 'FUNC' is allocated from the
        //:   allocator.
        //: 10 For concern 10, perform step 7 using alloctors of various sizes
        //:   from very small to one where, combined with 'FUNC', barely fits
        //:   within the small object buffer and verify, in each case, that no
        //:   memory is allocated either from the global allocator or from the
        //:   allocator used to construct the 'function' object.
        //: 11 For concern 11, perform step 7 using allocators of various
        //:   sizes which, combined with 'FUNC', do not fit in the small
        //:   object buffer and verify that exactly one block was allocated
        //:   from the allocator used to construct the 'function' and that no
        //:   memory was allocated from the global allocator.
        //: 12 For concern 12, look at the memory allocation from step 11 and
        //:   verify that, when 'FUNC' fits within the small object buffer,
        //:   that the allocated memory is only large enough to hold the
        //:   allocator adaptor.
        //: 13 For concern 13, look at the memory allocation from step 11 and
        //:   verify that, when 'FUNC' does not fit within the small object
        //:   buffer, that the allocated memory is large enough to hold both
        //:   'FUNC' and the allocator adaptor.
        //: 14 For concern 14, check at the end of each step, when the
        //:   'function' object is destroyed, that all memory is returned to
        //:   the allocator.
        //: 15 For concern 15, wrap the common parts of the above steps into a
        //:   function template, 'testFuncWithAlloc', which takes 'ALLOC' and
        //:   'FUNC' template parameters.  Instantiate this template with each
        //:   of the allocator types described in the previous step in
        //:   combination with each of the following invokable types: pointer
        //:   to function, pointer to member function, and functor types of
        //:   various sizes.
        //
        // Testing
        //      function(allocator_arg_t, const ALLOC& alloc, FUNC func);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCONSTRUCTOR function(allocator_arg_t, "
                            "const ALLOC& alloc, FUNC func)"
                            "\n======================================"
                            "==============================\n");

        // Null functors
        int (*nullFuncPtr)(IntWrapper, int) = 0;
        int (IntWrapper::*nullMemFuncPtr)(int) const = 0;

#define TEST(A, f, E) testFuncWithAlloc<A>(f, E, #A)

        if (veryVerbose) std::printf("FUNC is nullFuncPtr\n");
        TEST(bslma::TestAllocator *  , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , nullFuncPtr      , e_INPLACE_BOTH);
        TEST(MediumSTLAllocator<char>, nullFuncPtr      , e_INPLACE_BOTH);
        TEST(LargeSTLAllocator<char> , nullFuncPtr      , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is nullMemFuncPtr\n");
        TEST(bslma::TestAllocator *  , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(MediumSTLAllocator<char>, nullMemFuncPtr   , e_INPLACE_BOTH);
        TEST(LargeSTLAllocator<char> , nullMemFuncPtr   , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is simpleFunc\n");
        TEST(bslma::TestAllocator *  , simpleFunc       , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , simpleFunc       , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , simpleFunc       , e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , simpleFunc       , e_INPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , simpleFunc       , e_INPLACE_BOTH);
        TEST(MediumSTLAllocator<char>, simpleFunc       , e_INPLACE_FUNC_ONLY);
        TEST(LargeSTLAllocator<char> , simpleFunc       , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is &IntWrapper::add1\n");
        TEST(bslma::TestAllocator *  , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , &IntWrapper::add1, e_INPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , &IntWrapper::add1, e_INPLACE_FUNC_ONLY);
        TEST(MediumSTLAllocator<char>, &IntWrapper::add1, e_INPLACE_FUNC_ONLY);
        TEST(LargeSTLAllocator<char> , &IntWrapper::add1, e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is EmptyFunctor()\n");
        TEST(bslma::TestAllocator *  , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(MediumSTLAllocator<char>, EmptyFunctor()   , e_INPLACE_BOTH);
        TEST(LargeSTLAllocator<char> , EmptyFunctor()   , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is SmallFunctor(0)\n");
        TEST(bslma::TestAllocator *  , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , SmallFunctor(0)  , e_INPLACE_BOTH);
        TEST(MediumSTLAllocator<char>, SmallFunctor(0)  , e_INPLACE_FUNC_ONLY);
        TEST(LargeSTLAllocator<char> , SmallFunctor(0)  , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is MediumFunctor(0)\n");
        TEST(bslma::TestAllocator *  , MediumFunctor(0) , e_INPLACE_BOTH);
        TEST(bsl::allocator<char>    , MediumFunctor(0) , e_INPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , MediumFunctor(0) , e_INPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , MediumFunctor(0) , e_INPLACE_FUNC_ONLY);
        TEST(SmallSTLAllocator<char> , MediumFunctor(0) , e_INPLACE_FUNC_ONLY);
        TEST(MediumSTLAllocator<char>, MediumFunctor(0) , e_INPLACE_FUNC_ONLY);
        TEST(LargeSTLAllocator<char> , MediumFunctor(0) , e_INPLACE_FUNC_ONLY);

        if (veryVerbose) std::printf("FUNC is LargeFunctor(0)\n");
        TEST(bslma::TestAllocator *  , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(bsl::allocator<char>    , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(EmptySTLAllocator<char> , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(TinySTLAllocator<char>  , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(SmallSTLAllocator<char> , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(MediumSTLAllocator<char>, LargeFunctor(0)  , e_OUTOFPLACE_BOTH);
        TEST(LargeSTLAllocator<char> , LargeFunctor(0)  , e_OUTOFPLACE_BOTH);

#undef TEST

      } break;

      case 7: {
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
        //: 7 If 'alloc' is an STL-style allocator that fits within the small
        //:   object buffer, then no memory is allocated by this constructor.
        //: 8 If 'alloc' is an STL-style allocator that does not fit within
        //:   the small object buffer, then one block of memory is allocated
        //:   from 'alloc' itself.
        //: 9 If memory is allocated, the destructor frees it.
        //: 10 All of the above concerns also apply to the
        //:   'function(allocator_arg_t, const ALLOC&, nullptr_t)' constructor.
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
        //: 7 For concern 7, perform step 5 using alloctors of various sizes
        //:   from very small to one that barely fits within the small object
        //:   buffer and verify, in each case, that no memory is allocated
        //:   either from the global allocator or from the allocator used to
        //:   construct the 'function' object.
        //: 8 For concern 8, perform step 5 using an allocator that does not
        //:   fit in the small object buffer and verify that exactly one block
        //:   was allocated from the allocator used to construct the
        //:   'function' and that no memory was allocated from the global
        //:   allocator.
        //: 9 For concern 9, check at the end of step 8, when the 'function'
        //:   object is destroyed, that all memory was returned to the
        //:   allocator.
        //: 10 For concern 10, preform all of the previous steps using the 
        //:   'function(allocator_arg_t, const ALLOC&, nullptr_t)' constructor.
        //
        // TESTING
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
        {
            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
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
        }

        if (veryVerbose) printf("with bsl::allocator<T>\n");
        {
            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
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
        }

        if (veryVerbose) printf("with stateless allocator\n");
        {
            typedef EmptySTLAllocator<double>                         Alloc;
            typedef EmptySTLAllocator<bool>                           Alloc2;
            typedef bslma::AllocatorAdaptor<EmptySTLAllocator<char> > Adaptor;

            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
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
            ASSERT(erasedAlloc == f2.allocator());
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());
        }

        if (veryVerbose) printf("with tiny to medium allocator\n");
        {
            typedef TinySTLAllocator<double>                         Alloc;
            typedef bslma::AllocatorAdaptor<TinySTLAllocator<char> > Adaptor;

            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
            Alloc alloc(&ta);
            ASSERT(! bsl::is_empty<Alloc>::value);
            
            bsl::function<int(float)> f(bsl::allocator_arg, alloc);
            ASSERT(! f);
            bslma::Allocator *erasedAlloc = f.allocator();
            Adaptor *adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
            ASSERT(adaptor);
            ASSERT(adaptor->adaptedAllocator() == alloc);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());

            bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                         bsl::nullptr_t());
            ASSERT(! f2);
            erasedAlloc = f.allocator();
            adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
            ASSERT(adaptor);
            ASSERT(adaptor->adaptedAllocator() == alloc);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());
        }
        
        {
            typedef SmallSTLAllocator<double>                         Alloc;
            typedef bslma::AllocatorAdaptor<SmallSTLAllocator<char> > Adaptor;

            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
            Alloc alloc(&ta);
            ASSERT(! bsl::is_empty<Alloc>::value);
            
            bsl::function<int(float)> f(bsl::allocator_arg, alloc);
            ASSERT(! f);
            bslma::Allocator *erasedAlloc = f.allocator();
            Adaptor *adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
            ASSERT(adaptor);
            ASSERT(adaptor->adaptedAllocator() == alloc);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());

            bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                         bsl::nullptr_t());
            ASSERT(! f2);
            erasedAlloc = f.allocator();
            adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
            ASSERT(adaptor);
            ASSERT(adaptor->adaptedAllocator() == alloc);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());
        }
        
        {
            typedef MediumSTLAllocator<double>                         Alloc;
            typedef bslma::AllocatorAdaptor<MediumSTLAllocator<char> > Adaptor;

            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
            Alloc alloc(&ta);
            ASSERT(! bsl::is_empty<Alloc>::value);
            
            bsl::function<int(float)> f(bsl::allocator_arg, alloc);
            ASSERT(! f);
            bslma::Allocator *erasedAlloc = f.allocator();
            Adaptor *adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
            ASSERT(adaptor);
            ASSERT(adaptor->adaptedAllocator() == alloc);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());

            bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                         bsl::nullptr_t());
            ASSERT(! f2);
            erasedAlloc = f.allocator();
            adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
            ASSERT(adaptor);
            ASSERT(adaptor->adaptedAllocator() == alloc);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(globalAllocMonitor.isInUseSame());
        }
        
        if (veryVerbose) printf("with large allocator\n");
        {
            typedef LargeSTLAllocator<double>                         Alloc;
            typedef bslma::AllocatorAdaptor<LargeSTLAllocator<char> > Adaptor;

            globalAllocMonitor.reset();
            bslma::TestAllocator ta;
            Alloc alloc(&ta);
            ASSERT(! bsl::is_empty<Alloc>::value);

            {
                bsl::function<int(float)> f(bsl::allocator_arg, alloc);
                ASSERT(! f);
                bslma::Allocator *erasedAlloc = f.allocator();
                Adaptor *adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
                ASSERT(adaptor);
                ASSERT(adaptor->adaptedAllocator() == alloc);
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());

                bsl::function<int(float)> f2(bsl::allocator_arg, alloc,
                                             bsl::nullptr_t());
                ASSERT(! f2);
                erasedAlloc = f.allocator();
                adaptor = dynamic_cast<Adaptor *>(erasedAlloc);
                ASSERT(adaptor);
                ASSERT(adaptor->adaptedAllocator() == alloc);
                ASSERT(2 == ta.numBlocksInUse());
                ASSERT(globalAllocMonitor.isInUseSame());
            }
            ASSERT(0 == ta.numBlocksInUse());
        }
        
      } break;

      case 6: {
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
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For functors
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTOR INVOCATION"
                            "\n==================\n");

        if (veryVerbose) std::printf("Plan step 2\n");
        testFunctor<SmallFunctor, int, int>("SmallFunctor int(int...)");
        
        if (veryVerbose) std::printf("Plan step 5\n");
        testFunctor<SmallFunctor, IntWrapper, ConvertibleToInt>(
            "SmallFunctor IntWrapper(ConvertibleToInt...)");

        if (veryVerbose) std::printf("Plan step 6\n");
        testFunctor<EmptyFunctor, int, int>("EmptyFunctor int(int...)");
        testFunctor<EmptyFunctor, IntWrapper, ConvertibleToInt>(
            "EmptyFunctor IntWrapper(ConvertibleToInt...)");
        testFunctor<LargeFunctor, int, int>("LargeFunctor int(int...)");
        testFunctor<LargeFunctor, IntWrapper, ConvertibleToInt>(
            "LargeFunctor IntWrapper(ConvertibleToInt...)");

      } break;

      case 5: {
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
        //:   well as 'const' and versions of the preceding.
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
        //
        // Testing:
        //      RET operator()(ARGS...) const; // For pointer to member func
        // --------------------------------------------------------------------

        if (verbose) printf("\nPOINTER TO MEMBER FUNCTION INVOCATION"
                            "\n=====================================\n");

        if (veryVerbose) std::printf("Plan step 3\n");
        testPtrToMemFunc<IntWrapper&, int, int>("int(IntWrapper&, int...)");

        if (veryVerbose) std::printf("Plan step 4\n");
        testPtrToMemFunc<IntWrapper, int, int>("int(IntWrapper, int...)");

        if (veryVerbose) std::printf("Plan step 5\n");
        testPtrToMemFunc<IntWrapper*, int, int>("int(IntWrapper*, int...)");
        testPtrToMemFunc<SmartPtr<IntWrapper>, int, int>(
            "int(SmartPtr<IntWrapper>, int...)");

        if (veryVerbose) std::printf("Plan step 6\n");
        testPtrToMemFunc<IntWrapper, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper, ConvertibleToInt...");
        testPtrToMemFunc<IntWrapper&, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper&, ConvertibleToInt...");
        testPtrToMemFunc<IntWrapper*, IntWrapper, ConvertibleToInt>(
            "IntWrapper(IntWrapper*, ConvertibleToInt...");
        testPtrToMemFunc<SmartPtr<IntWrapper>, IntWrapper, ConvertibleToInt>(
            "IntWrapper(SmartPtr<IntWrapper>, ConvertibleToInt...");

        if (veryVerbose) std::printf("Plan step 7\n");
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

        if (veryVerbose) std::printf("Plan step 8\n");
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

        if (veryVerbose) std::printf("Plan step 9\n");
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


        bsl::function<void(IntWrapper, int)> vt(&IntWrapper::voidIncrement1);
        vt(iw, 0x10);                  // No return type to test
        ASSERT(0x300f == iw.value());  // Passed by value. Original unchanged. 

        bsl::function<void(IntWrapper&, int)> vtr(&IntWrapper::voidIncrement1);
        vtr(iw, 0x20);                 // No return type to test
        ASSERT(0x302f == iw.value());

        bsl::function<void(IntWrapper*, int)> vtp(&IntWrapper::voidIncrement1);
        vtp(&iw, 0x40);                // No return type to test
        ASSERT(0x306f == iw.value());

        bsl::function<void(SmartPtr<IntWrapper>,
                           int)> vtsp(&IntWrapper::voidIncrement1);
        vtsp(&iw, 0x80);               // No return type to test
        ASSERT(0x30ef == iw.value());

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
        //: 5 If the return value of the 'bsl::function' is 'void', then the
        //:   return value of function-pointer invocation is discarded,
        //:   even if the return type of the pointer-to-function is non-void.
        //: 6 Arguments that are supposed to be passed by reference *are*
        //:   passed by reference all the way through the invocation
        //:   interface.
        //: 7 Arguments that are supposed to be passed by value are copied
        //:   exactly once when passed through the invocation interface.
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
        //:   'bsl::function' objects with arguments of type
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
// TBD: Turn on when bslmf::ForwardingType is fixed
//         ASSERT(&v == gca(v));
        gca(v);

        // Test pass-by-value
        if (veryVerbose) printf("Plan step 7\n");
        bsl::function<int(CountCopies)> nc(numCopies);
        CountCopies cc;
        ASSERT(1 == numCopies(cc));
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
// TBD: Turn on when bslmf::ForwardingType is fixed
//         ASSERT(1 == nc(cc));
//         ASSERT(0 == numCopies(CountCopies()));
//         ASSERT(0 == nc(CountCopies()));
// #else
        ASSERT(2 == nc(cc));
        ASSERT(0 == numCopies(CountCopies()));
        ASSERT(1 == nc(CountCopies()));
// #endif
        ASSERT(0 == cc.numCopies());

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
