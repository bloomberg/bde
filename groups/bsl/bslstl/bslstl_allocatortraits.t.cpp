// bslstl_allocatortraits.t.cpp                  -*-C++-*-

#include "bslstl_allocatortraits.h"

#include <cstdio>
#include <cstdlib>
#include <climits>
#include <bslma_testallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslstl_allocator.h>
#include <bslalg_typetraitsgroupstlsequence.h>
#include <bslmf_issame.h>

using namespace BloombergLP;
using namespace bsl;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The class under test is a traits class having no state and only static
// member functions.  The class is templated on an allocator class and most of
// the public types and (static member) functions are pass-through's to the
// underlying allocator class's public types and methods.  Therefore, most of
// the testing is simply to ensure that the pass-through is correct.  The
// 'construct' function is more complex than the rest -- it doesn't call the
// underlying allocator's 'construct' method but instead uses placement new to
// construct an object in one of two ways: If the allocator type follows the
// 'bslma' allocator model, and if the object being constructed accepts a
// 'bslma_Allocator*' argument, then the allocator itself is passed as an
// extra argument to the object's constructor; otherwise, the arguments to
// 'construct' are passed to object's constructors unchanged.  The test for
// 'construct' must therefore test different combinations of both 'bslma' and
// non-'bslma' allocator classes and both 'bslma'-compliant and
// non-'bslma'-compliant object types.
//
// This test plan tests each public member of 'allocator_traits' in turn.  The
// type members are needed by the function members, so they are tested first.
// Other than that, there is no hierarchical relationship among the members,
// so the order of test is arbitrary
//
// Abbreviations:
// --------------
// Throughout this test driver, we use
//     A            ALLOC (template argument, default: bsl::allocator<T>)
//     T            A type argument to a template function
//     traits       bsl::allocator_traits<A>
//     Args...      shorthand for a family of templates <A1>, <A1,A2>, etc.
//
// ----------------------------------------------------------------------------
// 
// NESTED TYPES:
// [  ] allocator_type    
// [  ] value_type        
// [  ] pointer           
// [  ] const_pointer     
// [  ] void_pointer      
// [  ] const_void_pointer
// [  ] difference_type   
// [  ] size_type
// [  ] rebind_alloc<T1>
// [  ] rebind_traits<T1>
// [  ] propagate_on_container_copy_assignment
// [  ] propagate_on_container_move_assignment;
// [  ] propagate_on_container_swap
//
// STATIC MEMBER FUNCTIONS:
// [  ] pointer allocate(ALLOC& a, size_type n);
// [  ] pointer allocate(ALLOC& a, size_type n, const_void_pointer hint);
// [  ] void deallocate(ALLOC& a, pointer p, size_type n);
// [  ] void construct(ALLOC& a, T *p, Args&&... args);
// [  ] void destroy(ALLOC& a, T* p);
// [  ] size_type max_size(const ALLOC& a);
// [  ] ALLOC select_on_container_copy_construction(const ALLOC& rhs);
//
//-----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

//-----------------------------------------------------------------------------

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

// Short-cut assert macros
#define ASSERT_ISSAME(t1,t2) ASSERT((bslmf_IsSame<t1,t2>::VALUE))

#define LOOP_ASSERT_ISSAME(I,t1,t2) \
    LOOP_ASSERT(I, bslmf_IsSame<t1,t2>::VALUE)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
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

// Generic debug print function (3-arguments).
template <class T>
void dbg_print(const char* s, const T& val, const char* nl)
{
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

// typedef bsls_Types::Int64             Int64;
// typedef bsls_Types::Uint64            Uint64;

// 'g_x' is a dummy object with a unique address
int g_x;

// The following are default values for the 5 attributes of our test classes.
char        const default_a = ' ';
int         const default_b = 99;
double      const default_c = 1.0;
const char *const default_d = "hello";
void       *const default_e = &g_x;

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class AttribClass5
{
    // This test class has up to 5 constructor arguments and does not use the
    // 'bslma' allocator protocol.

    char        d_a;
    int         d_b;
    double      d_c;
    const char *d_d;
    void       *d_e;

public:
    AttribClass5(char        a = default_a,
                 int         b = default_b,
                 double      c = default_c,
                 const char *d = default_d,
                 void       *e = default_e)
        : d_a(a), d_b(b), d_c(c), d_d(d), d_e(e) { }

    ~AttribClass5() { d_b = 0xdeadbeaf; }

    char        a() const { return d_a; }
    int         b() const { return d_b; }
    double      c() const { return d_c; }
    const char *d() const { return d_d; }
    void       *e() const { return d_e; }
};

template <class ALLOC>
class AttribClass5Alloc
{
    // This test class has up to 5 constructor arguments plus an (optional)
    // allocator.  If the 'ALLOC' template argument is 'bsl::allocator', then
    // this class conforms to the 'bslma' allocator model, otherwise it does
    // not.

    AttribClass5 d_attrib;
    ALLOC        d_allocator;

public:
    AttribClass5Alloc(const ALLOC& alloc = ALLOC())
        : d_attrib(), d_allocator(alloc) { }
    AttribClass5Alloc(char a, const ALLOC& alloc = ALLOC())
        : d_attrib(a), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, const ALLOC& alloc = ALLOC())
        : d_attrib(a, b), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, double c, const ALLOC& alloc = ALLOC())
        : d_attrib(a, b, c), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, double c, const char *d,
                      const ALLOC& alloc = ALLOC())
        : d_attrib(a, b, c, d), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, double c, const char *d, void *e,
                      const ALLOC& alloc = ALLOC())
        : d_attrib(a, b, c, d, e), d_allocator(alloc) { }

    char        a() const { return d_attrib.a(); }
    int         b() const { return d_attrib.b(); }
    double      c() const { return d_attrib.c(); }
    const char *d() const { return d_attrib.d(); }
    void       *e() const { return d_attrib.e(); }

    ALLOC allocator() const { return d_allocator; }
};

class AttribClass5bslma
{
    // This test class has up to 5 constructor arguments plus an (optional)
    // address of a 'bslma_Allocator'.  This class conforms to the 'bslma'
    // allocator model.

    AttribClass5     d_attrib;
    bslma_Allocator* d_allocator;

public:
    AttribClass5bslma(bslma_Allocator *alloc = 0)
        : d_attrib(), d_allocator(bslma_Default::allocator(alloc)) { }
    AttribClass5bslma(char a, bslma_Allocator *alloc = 0)
        : d_attrib(a), d_allocator(bslma_Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, bslma_Allocator *alloc = 0)
        : d_attrib(a, b), d_allocator(bslma_Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, double c, bslma_Allocator *alloc = 0)
        : d_attrib(a, b, c), d_allocator(bslma_Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, double c, const char *d,
                      bslma_Allocator *alloc = 0)
        : d_attrib(a, b, c, d)
        , d_allocator(bslma_Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, double c, const char *d, void *e,
                      bslma_Allocator *alloc = 0)
        : d_attrib(a, b, c, d, e)
        , d_allocator(bslma_Default::allocator(alloc)) { }

    char        a() const { return d_attrib.a(); }
    int         b() const { return d_attrib.b(); }
    double      c() const { return d_attrib.c(); }
    const char *d() const { return d_attrib.d(); }
    void       *e() const { return d_attrib.e(); }

    bslma_Allocator *allocator() const { return d_allocator; }
};

template <class T>
class NonBslmaAllocator
{
    // This class is a C++03-compliant allocator that does not conform to the
    // 'bslma' allocator model.
    bslma_Allocator *d_mechanism;

public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T              *pointer;
    typedef const T        *const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class U> struct rebind
    {
        typedef NonBslmaAllocator<U> other;
    };

    explicit NonBslmaAllocator(bslma_Allocator *basicAlloc = 0)
        : d_mechanism(bslma_Default::allocator(basicAlloc)) { }

    pointer allocate(size_type n, const void *hint = 0)
        { return pointer(d_mechanism->allocate(n * sizeof(T))); }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(p); }

    void construct(pointer p, const T& val) { ::new ((void*) &*p) T(val); }
    void destroy(pointer p) { p->~T(); }

    // ACCESSORS
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    size_type max_size() const { return INT_MAX / sizeof(T); }

    bslma_Allocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const NonBslmaAllocator<T>& a, const NonBslmaAllocator<U>& b)
{
    return a.mechanism() == b.mechanism();
}

template <class T, class U>
inline
bool operator!=(const NonBslmaAllocator<T>& a, const NonBslmaAllocator<U>& b)
{
    return a.mechanism() != b.mechanism();
}

template <class T>
class BslmaAllocator
{
    // This class is a C++03-compliant allocator that conforms to the
    // 'bslma' allocator model (i.e., it is convertible from
    // 'bslma_Allocator*'.
    bslma_Allocator *d_mechanism;

public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T              *pointer;
    typedef const T        *const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class U> struct rebind
    {
        typedef BslmaAllocator<U> other;
    };

    BslmaAllocator(bslma_Allocator *basicAlloc = 0)
        : d_mechanism(bslma_Default::allocator(basicAlloc)) { }

    pointer allocate(size_type n, const void *hint = 0)
        { return pointer(d_mechanism->allocate(n * sizeof(T))); }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(p); }

    void construct(pointer p, const T& val) { ::new ((void*) &*p) T(val); }
    void destroy(pointer p) { p->~T(); }

    // ACCESSORS
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    size_type max_size() const { return INT_MAX / sizeof(T); }

    bslma_Allocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const BslmaAllocator<T>& a, const BslmaAllocator<U>& b)
{
    return a.mechanism() == b.mechanism();
}

template <class T, class U>
inline
bool operator!=(const BslmaAllocator<T>& a, const BslmaAllocator<U>& b)
{
    return a.mechanism() != b.mechanism();
}

template <class T>
class FunkyPointer
{
    // Pointer-like class for testing use of non-raw pointers in allocators.
    T* d_imp;

public:
    FunkyPointer() : d_imp(0) { }
    FunkyPointer(T* p, int) : d_imp(p) { }

    // Construct from null pointer
    FunkyPointer(int FunkyPointer::*) : d_imp(0) { }

    T& operator*() const { return *d_imp; }
    T* operator->() const { return d_imp; }
};

template <class T>
inline
bool operator==(FunkyPointer<T> a, FunkyPointer<T> b)
{
    return a.operator->() == b.operator->();
}

template <class T>
inline
bool operator!=(FunkyPointer<T> a, FunkyPointer<T> b)
{
    return a.operator->() != b.operator->();
}

template <class T>
class FunkyAllocator
{
    // Allocator that uses 'FunkyPointer' as its pointer type.  Not all
    // allocator-aware classes can work with such an allocator, but
    // 'allocator_traits' should work fine.
    // This class is a C++03-compliant allocator that conforms to the
    // 'bslma' allocator model (i.e., it is convertible from
    // 'bslma_Allocator*'.
    bslma_Allocator *d_mechanism;

public:
    // PUBLIC TYPES
    typedef std::size_t           size_type;
    typedef std::ptrdiff_t        difference_type;
    typedef FunkyPointer<T>       pointer;
    typedef FunkyPointer<const T> const_pointer;
    typedef T&                    reference;
    typedef const T&              const_reference;
    typedef T                     value_type;

    template <class U> struct rebind
    {
        typedef FunkyAllocator<U> other;
    };

    explicit FunkyAllocator(bslma_Allocator *basicAlloc = 0)
        : d_mechanism(bslma_Default::allocator(basicAlloc)) { }

    pointer allocate(size_type n, const void *hint = 0)
        { return pointer(d_mechanism->allocate(n * sizeof(T)), 0); }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(p); }

    void construct(pointer p, const T& val) { ::new ((void*) &*p) T(val); }
    void destroy(pointer p) { p->~T(); }

    // ACCESSORS
    pointer address(reference x) const { return pointer(&x, 0); }
    const_pointer address(const_reference x) const
        { return const_pointer(&x, 0); }
    size_type max_size() const { return INT_MAX / sizeof(T); }

    bslma_Allocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const FunkyAllocator<T>& a, const FunkyAllocator<U>& b)
{
    return a.mechanism() == b.mechanism();
}

template <class T, class U>
inline
bool operator!=(const FunkyAllocator<T>& a, const FunkyAllocator<U>& b)
{
    return a.mechanism() != b.mechanism();
}

template <class T>
bool matchAttrib(const T& v, char a, int b, double c, const char *d, void *e)
{
    return v.a() == a && v.b() == b && v.c() == c && v.d() == d && v.e() == e;
}

template <class T, class Alloc>
bool matchAttrib(const T& v, char a, int b, double c, const char *d, void *e,
                 const Alloc& alloc)
{
    return matchAttrib(v, a, b, c, d, e) && v.allocator() == alloc;
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Let's create a container class that holds a single object and which meets
// the requirements of an STL container and of a Bloomberg container:
//..
    template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
    class my_Container {
        ALLOC  d_allocator;
        TYPE  *d_valueptr;

      public:
        typedef BloombergLP::bslalg_TypeTraitsGroupStlSequence<TYPE,ALLOC>
            MyTypeTraits;
        BSLALG_DECLARE_NESTED_TRAITS(my_Container, MyTypeTraits);
            // Declare nested type traits for this class.

        typedef TYPE  value_type;
        typedef ALLOC allocator_type;
        // etc.

        explicit my_Container(const ALLOC& a = ALLOC());
        explicit my_Container(const TYPE& v, const ALLOC& a = ALLOC());
        my_Container(const my_Container& other);
        ~my_Container();

        TYPE&       front()       { return *d_valueptr; }
        const TYPE& front() const { return *d_valueptr; }
        // etc.
    };
//..
// The implementation of the constructors needs to allocate memory and
// construct an object of type 'TYPE' in the allocated memory.  Rather than
// allocating the memory directly, we use the 'allocate' member of
// 'allocator_traits'.  More importantly, we construct the object using the
// 'construct' member of 'allocator_traits', which provides the correct
// semantic for passing the allocator to the constructed object when
// appropriate:
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const ALLOC& a)
        : d_allocator(a)
    {
        // NOTE: This implementation is not exception-safe
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        AllocTraits::construct(d_allocator, d_valueptr);
    }

    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const TYPE& v, const ALLOC& a)
        : d_allocator(a)
    {
        // NOTE: This implementation is not exception-safe
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        AllocTraits::construct(d_allocator, d_valueptr, v);
    }
//..
// The copy constructor needs to conditinally copy the allocator from the
// 'other' container.  It uses
// 'allocator_traits::select_on_container_copy_construction' to decide whether
// to copy the 'other' allocator (for non-bslma allocators) or to
// default-construct the allocator (for bslma allocators).
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const my_Container& other)
        : d_allocator(bsl::allocator_traits<ALLOC>::
                      select_on_container_copy_construction(other.d_allocator))
    {
        // NOTE: This implementation is not exception-safe
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        AllocTraits::construct(d_allocator, d_valueptr, *other.d_valueptr);
    }
//..
// Finally, the destructor uses 'allocator_traits' functions to destroy and
// deallocate the value object:
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::~my_Container()
    {
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        AllocTraits::destroy(d_allocator, d_valueptr);
        AllocTraits::deallocate(d_allocator, d_valueptr, 1);
    }
//..
// Now, given a value type that uses a 'bslma_Allocator' to allocate memory:
//..
    class my_Type {

        bslma_Allocator *d_allocator;
        // etc.
      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(my_Type,
                                     bslalg_TypeTraitUsesBslmaAllocator);

        // CREATORS
        explicit my_Type(bslma_Allocator* basicAlloc = 0)
            : d_allocator(bslma_Default::allocator(basicAlloc)) { /* ... */ }
        my_Type(const my_Type& other)
            : d_allocator(bslma_Default::allocator(0)) { /* ... */ }
        my_Type(const my_Type& other, bslma_Allocator* basicAlloc)
            : d_allocator(bslma_Default::allocator(basicAlloc)) { /* ... */ }
        // etc.

        // ACCESSORS
        bslma_Allocator *allocator() const { return d_allocator; }
        // etc.
    };
//..
// We can see that the allocator is propagated to the container's element and
// that it is not copied on copy construction of the container:
//..
    int usageExample()
    {
        bslma_TestAllocator testAlloc;
        my_Container<my_Type> C1(&testAlloc);
        ASSERT(C1.front().allocator() == &testAlloc);
        my_Container<my_Type> C2(C1);
        ASSERT(C2.front().allocator() != &testAlloc);
        ASSERT(C2.front().allocator() == bslma_Default::defaultAllocator());
        return 0;
    }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 30: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   1. The usage example in the header file compiles.
        //   2. The usage example in the header produces correct results
        //
        // Test plan:
        //   Copy the usage examples from the header into this test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample();

      } break;
      case 1:
      {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   1 The allocator_traits template behaves as expected when
        //     its members are exercised simply.
        //
        // Plan:
        //   We instantiate the template with a variety allocator types.  For
        //   each instantiation, we allocate memory, deallocate memory,
        //   construct an element, and destroy the element.
	//
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma_TestAllocator default_ta;
        bslma_TestAllocator ta;

        bslma_DefaultAllocatorGuard guard(&default_ta);

        if (verbose) printf("Testing NonBslmaAllocator<AttribClass5>\n");
        {
            typedef AttribClass5                 Obj;
            typedef BslmaAllocator<Obj>          Alloc;
            typedef bsl::allocator_traits<Alloc> Traits;

            ASSERT_ISSAME(Alloc, Traits::allocator_type);
            ASSERT_ISSAME(Obj,  Traits::value_type);
            ASSERT_ISSAME(Obj*, Traits::pointer);
            ASSERT_ISSAME(const Obj*, Traits::const_pointer);

            // Test 'allocate'
            Alloc a1(&ta);
            Obj *p = Traits::allocate(a1, 1);
            ASSERT(1 == ta.numBlocksInUse());
            ASSERT(sizeof(Obj) == ta.numBytesInUse());

            // Test 'construct'
            Traits::construct(a1, p, 'x', 88, 0.25);
            ASSERT(matchAttrib(*p, 'x', 88, 0.25, default_d, default_e));

            // Test 'destroy'
            Traits::destroy(a1, p);
            ASSERT(0xdeadbeaf == p->b());

            // Test 'deallocate'
            Traits::deallocate(a1, p, 1);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(0 == ta.numBytesInUse());

            ASSERT(0 == default_ta.numBlocksTotal());
        }

        if (verbose) printf("Testing BslmaAllocator<AttribClass5bslma>\n");
        {
            typedef AttribClass5bslma            Obj;
            typedef BslmaAllocator<Obj>          Alloc;
            typedef bsl::allocator_traits<Alloc> Traits;

            ASSERT_ISSAME(Alloc, Traits::allocator_type);
            ASSERT_ISSAME(Obj,  Traits::value_type);
            ASSERT_ISSAME(Obj*, Traits::pointer);
            ASSERT_ISSAME(const Obj*, Traits::const_pointer);

            // Test 'allocate'
            Alloc a1(&ta);
            Obj *p = Traits::allocate(a1, 1);
            ASSERT(1 == ta.numBlocksInUse());
            ASSERT(sizeof(Obj) == ta.numBytesInUse());

            // Test 'construct'
            const char bye[] = "bye";
            Traits::construct(a1, p, 'x', 88, 0.25, bye);
            ASSERT(matchAttrib(*p, 'x', 88, 0.25, bye, default_e, &ta));

            // Test 'destroy'
            Traits::destroy(a1, p);
            ASSERT(0xdeadbeaf == p->b());

            // Test 'deallocate'
            Traits::deallocate(a1, p, 1);
            ASSERT(0 == ta.numBlocksInUse());
            ASSERT(0 == ta.numBytesInUse());

            ASSERT(0 == default_ta.numBlocksTotal());
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
