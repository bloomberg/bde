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
#include <bslmf_removecvq.h>

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
// [ 3] allocator_type    
// [ 3] value_type        
// [ 3] pointer           
// [ 3] const_pointer     
// [ 3] void_pointer      
// [ 3] const_void_pointer
// [ 3] difference_type   
// [ 3] size_type
// [ 4] rebind_alloc<T1>
// [ 4] rebind_traits<T1>
// [ 3] propagate_on_container_copy_assignment
// [ 3] propagate_on_container_move_assignment
// [ 3] propagate_on_container_swap
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
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ 2] TEST HARNESS

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int verbose, veryVerbose, veryVeryVerbose;
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
    LOOP_ASSERT(I, (bslmf_IsSame<t1,t2>::VALUE))

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
char        const DFLT_A = ' ';
int         const DFLT_B = 99;
double      const DFLT_C = 1.0;
const char *const DFLT_D = "hello";
void       *const DFLT_E = &g_x;

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

// Most recent hint given to any allocator's 'allocate' member function.
const void* g_lastHint;

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

    pointer allocate(size_type n, const void *hint = 0) {
        g_lastHint = hint;
        return pointer(d_mechanism->allocate(n * sizeof(T)));
    }

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

    pointer allocate(size_type n, const void *hint = 0) {
        g_lastHint = hint;
        return pointer(d_mechanism->allocate(n * sizeof(T)));
    }

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
    FunkyPointer(const FunkyPointer<typename bslmf_RemoveCvq<T>::Type>& other)
        : d_imp(other.operator->()) { }

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

    pointer allocate(size_type n, const void *hint = 0) {
        g_lastHint = hint;
        return pointer((T*) d_mechanism->allocate(n * sizeof(T)), 0);
    }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(&*p); }

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

struct AttribStruct5
{
    // This test struct has up to 5 attributes of different types.  It is a
    // simple aggregate type so it can be statically constructed.
    char        d_a;
    int         d_b;
    double      d_c;
    const char *d_d;
    void       *d_e;
};

class AttribClass5
{
    // This test class has up to 5 constructor arguments and does not use the
    // 'bslma' allocator protocol.

    AttribStruct5 d_attrib;

    static int d_ctorCount;  // Count of constructor calls
    static int d_dtorCount;  // Count of destructor calls

public:
    static int ctorCount() { return d_ctorCount; }
    static int dtorCount() { return d_dtorCount; }

    AttribClass5(char        a = DFLT_A,
                 int         b = DFLT_B,
                 double      c = DFLT_C,
                 const char *d = DFLT_D,
                 void       *e = DFLT_E)
    {
        AttribStruct5 values = { a, b, c, d, e };
        d_attrib = values;
        ++d_ctorCount;
    }

    AttribClass5(const AttribClass5& other)
        : d_attrib(other.d_attrib) { ++d_ctorCount; }

    ~AttribClass5() { d_attrib.d_b = 0xdeadbeaf; ++d_dtorCount; }

    char        a() const { return d_attrib.d_a; }
    int         b() const { return d_attrib.d_b; }
    double      c() const { return d_attrib.d_c; }
    const char *d() const { return d_attrib.d_d; }
    void       *e() const { return d_attrib.d_e; }
};

int AttribClass5::d_ctorCount = 0;
int AttribClass5::d_dtorCount = 0;

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
    typedef ALLOC AllocatorType;
    
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

    AllocatorType allocator() const { return d_allocator; }
};

class AttribClass5bslma
{
    // This test class has up to 5 constructor arguments plus an (optional)
    // address of a 'bslma_Allocator'.  This class conforms to the 'bslma'
    // allocator model.

    AttribClass5     d_attrib;
    bslma_Allocator* d_allocator;

public:
    typedef bslma_Allocator* AllocatorType;

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
bool matchAttrib(const T& v, char a, int b, double c, const char *d, void *e)
{
    return v.a() == a && v.b() == b && v.c() == c && v.d() == d && v.e() == e;
}

template <class T, class ALLOC>
bool matchAttrib(const T& v, char a, int b, double c, const char *d, void *e,
                 const ALLOC& alloc)
{
    return matchAttrib(v, a, b, c, d, e) && v.allocator() == alloc;
}

template <typename T>
inline bool isMutable(T& /* x */) { return true; }
    // Return 'true'.  Preferred match if 'x' is a modifiable lvalue.

template <typename T>
inline bool isMutable(const T& /* x */) { return false; }
    // Return 'false'.  Preferred match if 'x' is an rvalue or const lvalue.

//=============================================================================
//               CLASSES AND FUNCTIONS FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// This example demonstrates the intended use of 'allocator_traits' to
// implement a standard-conforming container class. We create a container
// class that holds a single object and which meets the requirements both of a
// standard container and of a Bloomberg container.  I.e., when instantiated
// with an allocator argument it uses the standard allocator model;
// otheriwe it uses the 'bslma' model.
//..
    #include <bslstl_allocatortraits.h>
    #include <bslstl_allocator.h>
    #include <bslalg_typetraitsgroupstlsequence.h>

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
        my_Container(const my_Container& other, const ALLOC& a);
        ~my_Container();
  
        ALLOC get_allocator() const { return d_allocator; }
  
        TYPE&       front()       { return *d_valueptr; }
        const TYPE& front() const { return *d_valueptr; }
        // etc.
    };
//..
// The implementation of the constructors needs to allocate memory and
// construct an object of type 'TYPE' in the allocated memory.  Because the
// allocation and construction are done in two separate steps, we need to
// create a proctor that will deallocate the allocated memory in case the
// constructor throws an exception.  The proctor uses the uniform interface
// provided by 'allocator_traits' to access the 'pointer' and
// 'deallocate' members of 'ALLOC':
//..
    template <class ALLOC>
    class my_Proctor {
        typedef typename bsl::allocator_traits<ALLOC>::pointer pointer;
        ALLOC   d_alloc;
        pointer d_ptr;
      public:
        my_Proctor(const ALLOC& a, pointer p) : d_alloc(a), d_ptr(p) { }
        ~my_Proctor() {
            if (d_ptr) {
                bsl::allocator_traits<ALLOC>::deallocate(d_alloc, d_ptr, 1);
            }
        }
        void release() { d_ptr = pointer(); }
    };
//..
// The actual allocation and construction are done using the 'allocate' and
// 'construct' members of 'allocator_traits', which provide the correct
// semantic for passing the allocator to the constructed object when
// appropriate:
//..
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const ALLOC& a)
        : d_allocator(a)
    {
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        my_Proctor<ALLOC> proctor(a, d_valueptr);
        // Call 'construct' with no constructor arguments
        AllocTraits::construct(d_allocator, d_valueptr);
        proctor.release();
    }
  
    template <class TYPE, class ALLOC>
    my_Container<TYPE, ALLOC>::my_Container(const TYPE& v, const ALLOC& a)
        : d_allocator(a)
    {
        typedef bsl::allocator_traits<ALLOC> AllocTraits;
        d_valueptr = AllocTraits::allocate(d_allocator, 1);
        my_Proctor<ALLOC> proctor(a, d_valueptr);
        // Call 'construct' with one constructor argument of type 'TYPE'
        AllocTraits::construct(d_allocator, d_valueptr, v);
        proctor.release();
    }
//..
// The copy constructor for 'my_Container' needs to conditinally copy the
// allocator from the 'other' container.  It uses
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
        my_Proctor<ALLOC> proctor(d_allocator, d_valueptr);
        AllocTraits::construct(d_allocator, d_valueptr, *other.d_valueptr);
        proctor.release();
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
// We create a representative class, 'my_Type', which allocates memory using
// the 'bslma_Allocator' protocol:
//..
    #include <bslma_default.h>

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
// Finally, instantiationg 'my_Container' using 'my_Type', we verify that when
// we provide a the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// element.  When the container is copy-constructed, the copy uses the default
// allocator, not the allocator from the original.  Moreover, the element
// stored in the copy also uses the default allocator.
//..
    #include <bslmf_issame.h>

    int usageExample1()
    {
        bslma_TestAllocator testAlloc;
        my_Container<my_Type> C1(&testAlloc);
        ASSERT((bslmf_IsSame<my_Container<my_Type>::allocator_type,
                bsl::allocator<my_Type> >::VALUE));
        ASSERT(C1.get_allocator() == bsl::allocator<my_Type>(&testAlloc));
        ASSERT(C1.front().allocator() == &testAlloc);
        my_Container<my_Type> C2(C1);
        ASSERT(C2.get_allocator() != C1.get_allocator());
        ASSERT(C2.get_allocator() == bsl::allocator<my_Type>());
        ASSERT(C2.front().allocator() != &testAlloc);
        ASSERT(C2.front().allocator() == bslma_Default::defaultAllocator());
        return 0;
    }
//..

//=============================================================================
//                  IMPLEMENTATION OF TEST CASES
//-----------------------------------------------------------------------------

template <template <class T> class ALLOC_TMPLT>
void testAllocatorConformance(const char* allocName)
    // Test the specified 'ALLOC_TMPLT' allocator class template as part of
    // testing the test harness.  Instantiating 'ALLOC_TMPLT<AttribClass5>',
    // check that each nested typedef required for C++03 allocators exists and
    // has the correct qualities (e.g., integral, pointer-like, etc..  Check
    // that 'rebind<float>::other' is the same as 'ALLOC_TMPLT<float>'.
    // Invoke each member function and verify the expected behavior.
{
    if (verbose) printf("Testing allocator class temlate %s\n", allocName);

    typedef ALLOC_TMPLT<AttribClass5> Alloc;

    bslma_TestAllocator default_ta, ta;
    bslma_DefaultAllocatorGuard guard(&default_ta);
    
    if (veryVerbose) printf("  Testing nested typedefs\n");
    
    typedef typename Alloc::value_type      value_type;
    typedef typename Alloc::pointer         pointer;
    typedef typename Alloc::const_pointer   const_pointer;
    typedef typename Alloc::reference       reference;
    typedef typename Alloc::const_reference const_reference;
    typedef typename Alloc::size_type       size_type;
    typedef typename Alloc::difference_type difference_type;

    LOOP_ASSERT_ISSAME(allocName, value_type, AttribClass5);

    // Verify that 'pointer' and 'const_pointer' are pointer-like
    pointer p;
    LOOP_ASSERT(allocName, sizeof(*p) == sizeof(value_type));
    const_pointer cp;
    LOOP_ASSERT(allocName, sizeof(*cp) == sizeof(value_type));

    LOOP_ASSERT_ISSAME(allocName, reference, value_type&);
    LOOP_ASSERT_ISSAME(allocName, const_reference, const value_type&);

    // Verify that 'size_type' is an unsigned integral type and
    // 'difference_type' is a signed integral type.
    {
        size_type s = -1;
        LOOP_ASSERT(allocName, s > 0);
        difference_type d = -1;
        LOOP_ASSERT(allocName, d < 0);
    }

    if (veryVerbose) printf("  Testing rebind\n");
    LOOP_ASSERT_ISSAME(allocName, ALLOC_TMPLT<float>,
                       typename Alloc::template rebind<float>::other);
    
    {
        if (veryVerbose) printf("  Testing constructon\n");
        Alloc a1;
        LOOP_ASSERT(allocName, &default_ta == a1.mechanism());

        Alloc a2(&ta);
        LOOP_ASSERT(allocName, &ta == a2.mechanism());

        Alloc a3(a2);
        LOOP_ASSERT(allocName, &ta == a3.mechanism());

        Alloc a4(&ta);
        LOOP_ASSERT(allocName, &ta == a4.mechanism());

        if (veryVerbose) printf("  Testing operator== and operator!=\n");
        LOOP_ASSERT(allocName, ! (a1 == a2));
        LOOP_ASSERT(allocName,    a1 != a2 );
        LOOP_ASSERT(allocName,    a2 == a3 );
        LOOP_ASSERT(allocName, ! (a3 != a3));
        LOOP_ASSERT(allocName,   (a2 == a4));
        LOOP_ASSERT(allocName, ! (a2 != a4));
    }

    if (veryVerbose) printf("  Testing member functions\n");
    AttribClass5 val('x',  6, -1.5, "pizza", 0);
    Alloc a(&ta);

    int ctorCountBefore = AttribClass5::ctorCount();
    int dtorCountBefore = AttribClass5::dtorCount();
    const char *const hint1 = "A", *const hint2 = "B";

    g_lastHint = hint1;
    p = a.allocate(1);
    LOOP_ASSERT(allocName, ta.numBytesInUse() == sizeof(value_type));
    LOOP_ASSERT(allocName, ta.numBlocksInUse() == 1);
    LOOP_ASSERT(allocName, AttribClass5::ctorCount() == ctorCountBefore);
    LOOP_ASSERT(allocName, 0 == g_lastHint);

    a.construct(p, val);
    LOOP_ASSERT(allocName, AttribClass5::ctorCount() == ctorCountBefore + 1);
    LOOP_ASSERT(allocName, matchAttrib(*p, 'x',  6, -1.5, "pizza", 0));

    // Test that 'pointer' can convert to 'const_pointer'
    cp = p;
    LOOP_ASSERT(allocName, isMutable(*p));
    LOOP_ASSERT(allocName, ! isMutable(*cp));

    // Test that dereferencing a pointer yields a reference
    value_type& v        = *p;
    const value_type& cv = *cp;
    LOOP_ASSERT(allocName, matchAttrib(cv, 'x',  6, -1.5, "pizza", 0));

    LOOP_ASSERT(allocName, p  == a.address(v));
    LOOP_ASSERT(allocName, cp == a.address(cv));

    a.destroy(p);
    LOOP_ASSERT(allocName, AttribClass5::dtorCount() == dtorCountBefore + 1);
    LOOP_ASSERT(allocName, 0xdeadbeaf == p->b());
    LOOP_ASSERT(allocName, 0xdeadbeaf == cp->b());

    a.deallocate(p, 1);
    LOOP_ASSERT(allocName, ta.numBytesInUse() == 0)
    LOOP_ASSERT(allocName, ta.numBlocksInUse() == 0);
    LOOP_ASSERT(allocName, AttribClass5::dtorCount() == dtorCountBefore + 1);

    // Test that allocation hint is saved in g_lastHint
    g_lastHint = hint1;
    p = a.allocate(2, hint2);
    LOOP_ASSERT(allocName, ta.numBytesInUse() == 2 * sizeof(value_type));
    LOOP_ASSERT(allocName, ta.numBlocksInUse() == 1);
    LOOP_ASSERT(allocName, hint2 == g_lastHint);
    a.deallocate(p, 1);
    LOOP_ASSERT(allocName, ta.numBytesInUse() == 0)
    LOOP_ASSERT(allocName, ta.numBlocksInUse() == 0);

    LOOP_ASSERT(allocName, INT_MAX / sizeof(value_type) == a.max_size());
}

template <class T>
void testAttribClass(const char* className)
    // Test the specified 'T' attribute class as part of testing the test
    // harness.  Invoke each constructor, then test that the object has the
    // expected attributes and allocator.
{
    if (verbose) printf("Testing attribute class %s\n", className);

    typedef typename T::AllocatorType Alloc;

    // Contents of this array are unimportant -- only their addresses are used.
    int addresses[5];

    // Small set of random values for testing attributes.  None of these
    // values match the default for the corresponding attribute.
    // Floating-point values are chosen to be exactly representable in IEEE
    // format so that they can be compared for exact equality.
    static const AttribStruct5 DATA[] = {
        //  a    b    c          d             e
        // ==== === ===== ============== ==============
        {    0,  0,  0.0,            "",             0 },
        {  'x',  0, -1.5,       "pizza", &addresses[0] },
        {  'y', 42,  0.0,         "BDE", &addresses[1] },
        {  'a', 98, 3.25,            "", &addresses[2] },
        {  'm', -7, 0.25, "test driver",             0 },
        {  'z', 98, 3.25, "test driver", &addresses[3] }
    };
    static const std::size_t NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

    bslma_TestAllocator ta;
    const Alloc DFLT_ALLOC(bslma_Default::allocator(0));
    const Alloc TST_ALLOC(&ta);

    for (int i = 0; i < NUM_DATA; ++i) {
        const char        A = DATA[i].d_a;
        const int         B = DATA[i].d_b;
        const double      C = DATA[i].d_c;
        const char *const D = DATA[i].d_d;
        void       *const E = DATA[i].d_e;

        T v0;
        LOOP2_ASSERT(className, i, matchAttrib(v0, DFLT_A, DFLT_B, DFLT_C,
                                               DFLT_D, DFLT_E, DFLT_ALLOC));
        T v1(A);
        LOOP2_ASSERT(className, i, matchAttrib(v1, A, DFLT_B, DFLT_C, DFLT_D,
                                               DFLT_E, DFLT_ALLOC));
        T v2(A, B);
        LOOP2_ASSERT(className, i, matchAttrib(v2, A, B, DFLT_C, DFLT_D,
                                               DFLT_E, DFLT_ALLOC));

        T v3(A, B, C);
        LOOP2_ASSERT(className, i, matchAttrib(v3, A, B, C, DFLT_D, DFLT_E,
                                               DFLT_ALLOC));

        T v4(A, B, C, D);
        LOOP2_ASSERT(className, i, matchAttrib(v4, A, B, C, D, DFLT_E,
                                               DFLT_ALLOC));

        T v5(A, B, C, D, E);
        LOOP2_ASSERT(className, i, matchAttrib(v5, A, B, C, D, E, DFLT_ALLOC));


        T va0(TST_ALLOC);
        LOOP2_ASSERT(className, i, matchAttrib(va0, DFLT_A, DFLT_B, DFLT_C,
                                               DFLT_D, DFLT_E, TST_ALLOC));
        T va1(A, TST_ALLOC);
        LOOP2_ASSERT(className, i, matchAttrib(va1, A, DFLT_B, DFLT_C, DFLT_D,
                                               DFLT_E, TST_ALLOC));
        T va2(A, B, TST_ALLOC);
        LOOP2_ASSERT(className, i, matchAttrib(va2, A, B, DFLT_C, DFLT_D,
                                               DFLT_E, TST_ALLOC));

        T va3(A, B, C, TST_ALLOC);
        LOOP2_ASSERT(className, i, matchAttrib(va3, A, B, C, DFLT_D, DFLT_E,
                                               TST_ALLOC));

        T va4(A, B, C, D, TST_ALLOC);
        LOOP2_ASSERT(className, i, matchAttrib(va4, A, B, C, D, DFLT_E,
                                               TST_ALLOC));

        T va5(A, B, C, D, E, TST_ALLOC);
        LOOP2_ASSERT(className, i, matchAttrib(va5, A, B, C, D, E, TST_ALLOC));
    }
};

template <>
void testAttribClass<AttribClass5>(const char* className)
    // Test the 'AttribClass5' class as part of testing the test harness.
    // Invoke each constructor, then test that the object has the expected
    // attributes and allocator.  This function is a specialization of
    // 'testAttribClass' that tests 'AttribClass5' and doesn't use an
    // allocator.
{
    if (verbose) printf("Testing attribute class %s\n", className);

    // Contents of this array are unimportant -- only their addresses are used.
    int addresses[5];

    // Small set of random values for testing attributes.  None of these
    // values match the default for the corresponding attribute.
    // Floating-point values are chosen to be exactly representable in IEEE
    // format so that they can be compared for exact equality.
    static const AttribStruct5 DATA[] = {
        //  a    b    c          d             e
        // ==== === ===== ============== ==============
        {    0,  0,  0.0,            "",             0 },
        {  'x',  0, -1.5,       "pizza", &addresses[0] },
        {  'y', 42,  0.0,         "BDE", &addresses[1] },
        {  'a', 98, 3.25,            "", &addresses[2] },
        {  'm', -7, 0.25, "test driver",             0 },
        {  'z', 98, 3.25, "test driver", &addresses[3] }
    };
    static const std::size_t NUM_DATA = sizeof(DATA) / sizeof(DATA[0]);

    for (int i = 0; i < NUM_DATA; ++i) {
        const char        A = DATA[i].d_a;
        const int         B = DATA[i].d_b;
        const double      C = DATA[i].d_c;
        const char *const D = DATA[i].d_d;
        void       *const E = DATA[i].d_e;

        AttribClass5 v0;
        LOOP2_ASSERT(className, i, matchAttrib(v0, DFLT_A, DFLT_B, DFLT_C,
                                               DFLT_D, DFLT_E));
        AttribClass5 v1(A);
        LOOP2_ASSERT(className, i, matchAttrib(v1, A, DFLT_B, DFLT_C, DFLT_D,
                                               DFLT_E));
        AttribClass5 v2(A, B);
        LOOP2_ASSERT(className, i, matchAttrib(v2, A, B, DFLT_C, DFLT_D,
                                               DFLT_E));

        AttribClass5 v3(A, B, C);
        LOOP2_ASSERT(className, i, matchAttrib(v3, A, B, C, DFLT_D, DFLT_E));

        AttribClass5 v4(A, B, C, D);
        LOOP2_ASSERT(className, i, matchAttrib(v4, A, B, C, D, DFLT_E));

        AttribClass5 v5(A, B, C, D, E);
        LOOP2_ASSERT(className, i, matchAttrib(v5, A, B, C, D, E));
    }
};

template <class ALLOC>
void testNestedTypedefs(const char* allocName)
{
    if (verbose)
        printf("Testing nested typedefs for allocator %s\n", allocName);

    typedef allocator_traits<ALLOC> Traits;

    LOOP_ASSERT_ISSAME(allocName, ALLOC, typename Traits::allocator_type);
    LOOP_ASSERT_ISSAME(allocName,
                       typename ALLOC::value_type,
                       typename Traits::value_type);
    LOOP_ASSERT_ISSAME(allocName,
                       typename ALLOC::pointer,
                       typename Traits::pointer);
    LOOP_ASSERT_ISSAME(allocName,
                       typename ALLOC::const_pointer,
                       typename Traits::const_pointer);
    LOOP_ASSERT_ISSAME(allocName,
                       typename ALLOC::difference_type,
                       typename Traits::difference_type);
    LOOP_ASSERT_ISSAME(allocName,
                       typename ALLOC::size_type,
                       typename Traits::size_type);

    LOOP_ASSERT_ISSAME(allocName, void*, typename Traits::void_pointer);
    LOOP_ASSERT_ISSAME(allocName,
                       const void*, typename Traits::const_void_pointer);

    // We check whether the propagate traits are derived from false_type by
    // checking if a pointer to the trait class is convertible to a pointer to
    // false_type.  We use pointer convertibility because it avoids
    // user-defined conversions.  Convertibility from A* to B* implies either
    // that cv-A is the same as B or cv-A is derived from B, which what we are
    // looking for.
    LOOP_ASSERT(allocName,
                (bslmf_IsConvertible<
                 typename Traits::propagate_on_container_copy_assignment*,
                 bslmf_MetaInt<0>* >::VALUE));
    LOOP_ASSERT(allocName,
                (bslmf_IsConvertible<
                 typename Traits::propagate_on_container_move_assignment*,
                 bslmf_MetaInt<0>* >::VALUE));
    LOOP_ASSERT(allocName, 
                (bslmf_IsConvertible<
                 typename Traits::propagate_on_container_swap*,
                 bslmf_MetaInt<0>* >::VALUE));
}

template <template <class X> class ALLOC_TMPL, class T, class U>
void testRebind(const char* testName)
{
    typedef ALLOC_TMPL<T>            AllocT;
    typedef allocator_traits<AllocT> TraitsT;
    typedef typename TraitsT::template rebind_alloc<U>  AllocTReboundU;
    typedef typename TraitsT::template rebind_traits<U> TraitsTReboundU;

    typedef ALLOC_TMPL<U>            AllocU;
    typedef allocator_traits<AllocU> TraitsU;
    typedef typename TraitsU::template rebind_alloc<T>  AllocUReboundT;
    typedef typename TraitsU::template rebind_traits<T> TraitsUReboundT;

    // Rebind to self
    typedef typename TraitsT::template rebind_alloc<T>  AllocTReboundT;
    typedef typename TraitsT::template rebind_traits<T> TraitsTReboundT;

    // rebind to float
    typedef typename TraitsU::template rebind_alloc<float>  AllocUReboundF;
    typedef typename TraitsU::template rebind_traits<float> TraitsUReboundF;
    typedef typename TraitsT::template rebind_alloc<float>  AllocTReboundF;
    typedef typename TraitsT::template rebind_traits<float> TraitsTReboundF;

    // Rebind from 'T' to 'U'
    LOOP_ASSERT(testName, (bslmf_IsConvertible<
                           AllocTReboundU*, AllocU*>::VALUE));
    LOOP_ASSERT(testName, (bslmf_IsConvertible<
                           TraitsTReboundU*, TraitsU*>::VALUE));
    LOOP_ASSERT_ISSAME(testName,
                       typename TraitsTReboundU::allocator_type, AllocU);

    // Rebind from 'U' to 'T'
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<AllocUReboundT*, AllocT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<TraitsUReboundT*, TraitsT*>::VALUE));
    LOOP_ASSERT_ISSAME(testName,
                       typename TraitsUReboundT::allocator_type, AllocT);

    // Rebind from 'T' to 'T'
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<AllocTReboundT*, AllocT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<TraitsTReboundT*, TraitsT*>::VALUE));
    LOOP_ASSERT_ISSAME(testName,
                       typename TraitsTReboundT::allocator_type, AllocT);

    // Multiple rebind
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_alloc<T>*, 
                 AllocT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_traits<T>*, 
                 TraitsT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_alloc<U>*, 
                 AllocU*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf_IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_traits<U>*,
                 TraitsU*>::VALUE));
}

template <class ALLOC>
void testAllocateDeallocate(const char* name)
{
    typedef allocator_traits<ALLOC>          TraitsT;
    typedef typename TraitsT::value_type     value_type;
    typedef typename TraitsT::pointer        pointer;
    typedef typename TraitsT::size_type      size_type;

    const size_type SIZES[] = { 1, 2, 10 };
    const int       NUM_SIZES = sizeof(SIZES) / sizeof(SIZES[0]);

    pointer pointers[NUM_SIZES * 2];

    bslma_TestAllocator ta;

    ALLOC a(&ta);

    // Source of arbitrary addresses for hints
    const char hint[1] =  { 'H' };
    const char nonHint[1] = { '-' };

    // All of the test classes use 'AttribClass5' to keep track of their
    // constructor and destructor calls.
    const int ctorCountB = AttribClass5::ctorCount();
    const int dtorCountB = AttribClass5::dtorCount();
    
    if (verbose) printf("testing allocate(a, n) for %s\n", name);
    for (int i = 0; i < 2 * NUM_SIZES; ++i) {
        const int N = SIZES[i % NUM_SIZES];
        const bool useHint = i >= NUM_SIZES;  // Use hint for 2nd half

        const int allocationsB = ta.numAllocations();
        const int blocksInUseB = ta.numBlocksInUse();
        const int bytesInUseB  = ta.numBytesInUse();

        const size_type blockSize = N * sizeof(value_type);

        g_lastHint = nonHint;
        if (useHint) {
            if (veryVerbose) printf("  Call allocate(a, %d, hint)\n", int(N));
            pointers[i] = TraitsT::allocate(a, N, hint);
        }
        else {
            if (veryVerbose) printf("  Call allocate(a, %d)\n", int(N));
            pointers[i] = TraitsT::allocate(a, N);
        }

        // Check that hint was passed through if useHint == true, and was not
        // passed through if useHint == false.
        LOOP2_ASSERT(name, N, (useHint ? hint : 0) == g_lastHint);

        // Check memory allocation
        LOOP2_ASSERT(name, N, ta.lastAllocatedAddress() == &*pointers[i]);
        LOOP2_ASSERT(name, N, ta.lastAllocatedNumBytes() == blockSize);
        LOOP2_ASSERT(name, N, ta.numAllocations() == allocationsB + 1);
        LOOP2_ASSERT(name, N, ta.numBlocksInUse() == blocksInUseB + 1);
        LOOP2_ASSERT(name, N, ta.numBytesInUse()  == bytesInUseB + blockSize);

        // No constructors or destructors were called
        LOOP2_ASSERT(name, N, AttribClass5::ctorCount() == ctorCountB);
        LOOP2_ASSERT(name, N, AttribClass5::dtorCount() == dtorCountB);
    }

    if (verbose) printf("testing deallocate(a, p, n) for %s\n", name);
    for (int i = 0; i < 2 * NUM_SIZES; ++i) {
        const int N = SIZES[i % NUM_SIZES];

        const int allocationsB = ta.numAllocations();
        const int blocksInUseB = ta.numBlocksInUse();
        const int bytesInUseB  = ta.numBytesInUse();

        const size_type blockSize = N * sizeof(value_type);

        TraitsT::deallocate(a, pointers[i], N);

        // Check memory deallocation
        LOOP2_ASSERT(name, N, ta.lastDeallocatedNumBytes() == blockSize);
        LOOP2_ASSERT(name, N, ta.numAllocations() == allocationsB);
        LOOP2_ASSERT(name, N, ta.numBlocksInUse() == blocksInUseB - 1);
        LOOP2_ASSERT(name, N, ta.numBytesInUse()  == bytesInUseB - blockSize);

        // No constructors or destructors were called
        LOOP2_ASSERT(name, N, AttribClass5::ctorCount() == ctorCountB);
        LOOP2_ASSERT(name, N, AttribClass5::dtorCount() == dtorCountB);
    }
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

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example in the header file compiles.
        //: 2 The usage example in the header produces correct results
        //
        // Test plan:
        //   Copy the usage examples from the header into this test driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample1();

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATE AND DEALLOCATE
        //
        // Concerns:
        //: 1 For any allocator, 'a' of type 'ALLOC', with value type 'T',
        //:   calling 'allocator_traits<ALLOC>::allocate(a, n)' results in a
        //:   pass-through call to 'a.allocate(n), returning enough space for
        //:   'n' objects of type 'T'.
        //: 2 If a hint pointer, 'h', is passed to
        //:   'allocator_traits<ALLOC>::allocate(a, n, h)', then 'h' is passed
        //:   through to the underlying call to 'a.allocate(n, h)'.
        //: 3 A call to 'allocator_traits<ALLOC>::deallocate(a, p, n)' results
        //:   in a pass-through call to 'a.deallocate(p, n)', deallocating the
        //:   previously-allocated memory at 'p'.
        //: 4 Neither 'allocate' nor 'deallocate' results in any constructors
        //:   being called for 'T'.
        //
        // Plan:
        //: o Create a function template 'testAllocateDeallocate' which may
        //:   be instantiated with an arbitrary standard-conforming allocator
        //:   type, ALLOC.  This function makes multiple calls to
        //:   'allocator_traits<ALLOC>::allocate(a, n)', where 'a' is an
        //:   instance of 'ALLOC' and n is 1, 2, and 10.  Verify that the
        //:   allocator allocates the expected amount of memory. (C1)
        //: o Within 'testAllocateDeallocate', also call
        //:   'allocator_traits<ALLOC>::allocate(a, n, h)', where 'h' is an
        //:   aribrary address, and verify that the correct amount of memory
        //:   is allocated and that 'h' was passed through to the allocator.
        //:   (All of the allocators used for testing are instrumented so that
        //:   'h' is stored in a known location.) (C2)
        //: o For each pointer, 'p', returned by calling
        //:   'allocator_traits<ALLOC>::allocate(a, n [, h])' within
        //:   'testAllocateDeallocate', call
        //:   'allocator_traits<ALLOC>::deallocate(p, n)' and verify that the
        //:   storage was deallocated. (C3)
        //: o Finally, test that none of the operations within
        //:   'testAllocateDeallocate' change the count of constructors or
        //:   destructors invoked on our test types. (C4)
        //: o Instantiate and invoke 'testAllocateDealloate' on each meaningful
        //:   combination of attribute classes ('AttribClass5',
        //:   'AttribClass5Alloc', and 'AttribClass5bslma') and allocator type
        //:   ('NonBslmaAllocator', 'BslmaAllocator',
        //:   'FunkyAllocator'). (C1-C4)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATE AND DEALLOCATE"
                            "\n===============================\n");

#define TEST_ALLOC_DEALLOC(ALLOC) \
        testAllocateDeallocate<ALLOC >(#ALLOC);

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocFunky;

        TEST_ALLOC_DEALLOC(NonBslmaAllocator<AttribClass5>);
        TEST_ALLOC_DEALLOC(BslmaAllocator<AttribClass5>);
        TEST_ALLOC_DEALLOC(FunkyAllocator<AttribClass5>);

        TEST_ALLOC_DEALLOC(NonBslmaAllocator<AC5AllocNonBslma>);
        TEST_ALLOC_DEALLOC(BslmaAllocator<AC5AllocNonBslma>);
        TEST_ALLOC_DEALLOC(FunkyAllocator<AC5AllocNonBslma>);

        TEST_ALLOC_DEALLOC(NonBslmaAllocator<AC5AllocBslma>);
        TEST_ALLOC_DEALLOC(BslmaAllocator<AC5AllocBslma>);
        TEST_ALLOC_DEALLOC(FunkyAllocator<AC5AllocBslma>);

        TEST_ALLOC_DEALLOC(NonBslmaAllocator<AC5AllocFunky>);
        TEST_ALLOC_DEALLOC(BslmaAllocator<AC5AllocFunky>);
        TEST_ALLOC_DEALLOC(FunkyAllocator<AC5AllocFunky>);

        TEST_ALLOC_DEALLOC(NonBslmaAllocator<AttribClass5bslma>);
        TEST_ALLOC_DEALLOC(BslmaAllocator<AttribClass5bslma>);
        TEST_ALLOC_DEALLOC(FunkyAllocator<AttribClass5bslma>);

#undef TEST_ALLOC_DEALLOC        
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING REBIND
        //
        // Concerns:
        //: 1 For allocator template 'ALLOC_TMPL' and types 'T' and 'U',
        //:   'allocator_traits<ALLOC_TMPL<T> >::rebind_alloc<U> is the same
        //:   as, or derived from 'ALLOC_TMPL<U>'.
        //: 2 For allocator template 'ALLOC_TMPL' and types 'T' and 'U',
        //:   'allocator_traits<ALLOC_TMPL<T> >::rebind_traits<U> is the same
        //:   as, or derived from 'allocator_traits<ALLOC_TMPL<U> >'.
        //: 3 A rebind operation can be applied twice to a rebound type.
        //: 4 Concerns 1 through 3 apply if 'T' is the same as 'U'.
        //
        // Plan
        //: o Create a function template, 'testRebind' that applies
        //:   'rebind_alloc' and 'rebind_traits' and tests the resulting
        //:   types. (C1-C2)
        //: o 'testRebind' also applies 'rebind_alloc' and 'rebind_traits' to
        //:   the results of the previous rebinds. (C3)
        //: o 'testRebind' also applies 'rebind_alloc<T>' and
        //:   'rebind_traits<T>' to 'ALLOC_TMPL<T>'. (C4)
        //: o Call 'testRebind' with each combination of test allocators and
        //:   test value types, as well as with value type 'int'. (C1-C4)
        //
        // Testing
        //   rebind_alloc<U>
        //   rebind_traits<U>
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING REBIND"
                            "\n==============\n");

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<FunkyAllocator<int> >    AC5AllocFunky;

#define TEST_REBIND(ALLOC_TMP, T, U) \
        testRebind<ALLOC_TMP, T, U>(#ALLOC_TMP ", " #T ", " #U)

        // 'testRebind' applies rebind operations in both directions.  Hence,
        // after invoking 'testRebind<A, A, B>()' it is not necessary to
        // invoke 'testRebind<A, B, A>()'.
        TEST_REBIND(NonBslmaAllocator, int               , AttribClass5     );
        TEST_REBIND(NonBslmaAllocator, int               , AC5AllocNonBslma );
        TEST_REBIND(NonBslmaAllocator, int               , AC5AllocBslma    );
        TEST_REBIND(NonBslmaAllocator, int               , AC5AllocFunky    );
        TEST_REBIND(NonBslmaAllocator, int               , AttribClass5bslma);
        TEST_REBIND(NonBslmaAllocator, AttribClass5      , AC5AllocNonBslma );
        TEST_REBIND(NonBslmaAllocator, AC5AllocNonBslma  , AC5AllocBslma    );
        TEST_REBIND(NonBslmaAllocator, AC5AllocBslma     , AC5AllocFunky    );
        TEST_REBIND(NonBslmaAllocator, AC5AllocFunky     , AttribClass5bslma);

        TEST_REBIND(BslmaAllocator, int               , AttribClass5     );
        TEST_REBIND(BslmaAllocator, int               , AC5AllocNonBslma );
        TEST_REBIND(BslmaAllocator, int               , AC5AllocBslma    );
        TEST_REBIND(BslmaAllocator, int               , AC5AllocFunky    );
        TEST_REBIND(BslmaAllocator, int               , AttribClass5bslma);
        TEST_REBIND(BslmaAllocator, AttribClass5      , AC5AllocNonBslma );
        TEST_REBIND(BslmaAllocator, AC5AllocNonBslma  , AC5AllocBslma    );
        TEST_REBIND(BslmaAllocator, AC5AllocBslma     , AC5AllocFunky    );
        TEST_REBIND(BslmaAllocator, AC5AllocFunky     , AttribClass5bslma);

        TEST_REBIND(FunkyAllocator, int               , AttribClass5     );
        TEST_REBIND(FunkyAllocator, int               , AC5AllocNonBslma );
        TEST_REBIND(FunkyAllocator, int               , AC5AllocBslma    );
        TEST_REBIND(FunkyAllocator, int               , AC5AllocFunky    );
        TEST_REBIND(FunkyAllocator, int               , AttribClass5bslma);
        TEST_REBIND(FunkyAllocator, AttribClass5      , AC5AllocNonBslma );
        TEST_REBIND(FunkyAllocator, AC5AllocNonBslma  , AC5AllocBslma    );
        TEST_REBIND(FunkyAllocator, AC5AllocBslma     , AC5AllocFunky    );
        TEST_REBIND(FunkyAllocator, AC5AllocFunky     , AttribClass5bslma);

#undef TEST_REBIND
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING NESTED TYPEDEFS
        //
        // Concerns:
        //: 1 'allocator_type' is the same as 'ALLOC'.
        //: 2 'value_type', 'pointer', 'const_pointer', 'difference_type',
        //:   and 'size_type' are the same as the corresponding types within
        //:   'ALLOC'.
        //: 3 'void_pointer' is the same as 'void*' and 'const_void_pointer'
        //:   is the same as 'const void*'.
        //: 4 The types 'propagate_on_container_copy_assignment'
        //:   'propagate_on_container_move_assignment'
        //:   'propagate_on_container_swap' are each derived from
        //:   'bslmf_MetaInt<0>'.
        //: 5 Concerns 1-4 apply to any allocator type.
        //
        // Plan:
        //: o Create a template function, 'testNestedTypedefs' that
        //:   directly tests each of the types. (C1-4).
        //: o Instantiate and invoke 'testNestedTypedefs' on each meaningful
        //:   combination of attribute classes ('AttribClass5',
        //:   'AttribClass5Alloc', and 'AttribClass5bslma') and allocator type
        //:   ('NonBslmaAllocator', 'BslmaAllocator', 'FunkyAllocator').
        //
        // Testing:
        //   allocator_type    
        //   value_type        
        //   pointer           
        //   const_pointer     
        //   void_pointer      
        //   const_void_pointer
        //   difference_type   
        //   size_type
        //   propagate_on_container_copy_assignment
        //   propagate_on_container_move_assignment
        //   propagate_on_container_swap
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING NESTED TYPEDEFS"
                            "\n=======================\n");

#define TEST_NESTED_TYPEDEFS(ALLOC) \
        testNestedTypedefs<ALLOC >(#ALLOC);

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocFunky;

        TEST_NESTED_TYPEDEFS(NonBslmaAllocator<AttribClass5>);
        TEST_NESTED_TYPEDEFS(BslmaAllocator<AttribClass5>);
        TEST_NESTED_TYPEDEFS(FunkyAllocator<AttribClass5>);

        TEST_NESTED_TYPEDEFS(NonBslmaAllocator<AC5AllocNonBslma>);
        TEST_NESTED_TYPEDEFS(BslmaAllocator<AC5AllocNonBslma>);
        TEST_NESTED_TYPEDEFS(FunkyAllocator<AC5AllocNonBslma>);

        TEST_NESTED_TYPEDEFS(NonBslmaAllocator<AC5AllocBslma>);
        TEST_NESTED_TYPEDEFS(BslmaAllocator<AC5AllocBslma>);
        TEST_NESTED_TYPEDEFS(FunkyAllocator<AC5AllocBslma>);

        TEST_NESTED_TYPEDEFS(NonBslmaAllocator<AC5AllocFunky>);
        TEST_NESTED_TYPEDEFS(BslmaAllocator<AC5AllocFunky>);
        TEST_NESTED_TYPEDEFS(FunkyAllocator<AC5AllocFunky>);

        TEST_NESTED_TYPEDEFS(NonBslmaAllocator<AttribClass5bslma>);
        TEST_NESTED_TYPEDEFS(BslmaAllocator<AttribClass5bslma>);
        TEST_NESTED_TYPEDEFS(FunkyAllocator<AttribClass5bslma>);

#undef TEST_NESTED_TYPEDEFS        
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST HARNESS
        //
        // Concerns:
        //: 1  The 'NonBslmaAllocator' class defines all of the
        //:    nested types required by C++03.
        //: 2  The 'NonBslmaAllocator' class can be rebound using the
        //:    'rebind' member template.
        //: 3  The objects of 'NonBslmaAllocator' can be (explicitly)
        //:    constructed from a pointer to a 'bslma_TestAllocator'.
        //: 4  The default constructor for 'NonBslmaAllocator'
        //:    uses the currently-installed test allocator.
        //: 5  A 'NonBslmaAllocator' object can be copy-constructed.
        //: 6  Calling 'allocate' and 'deallocate' on 'NonBslmaAllocator'
        //:    objects allocates and deallocates the expected number of bytes
        //:    from the underlying 'bslma_TestAllocator'.
        //: 7  Calling 'construct' and 'destroy' for an object allocated from
        //:    a 'NonBslmaAllocator' results in the object being initialized
        //:    and destroyed as expected.
        //: 8  Calling 'address' on reference to an object allocated from a
        //:    'NonBslmaAllocator' yields its address.
        //: 9  Calling 'max_size' on a 'NonBslmaAllocator' yields 'INT_MAX'.
        //: 10 A 'NonBslmaAllocator' can be compared for equality.  The result
        //:    is true if the object was constructed from the same
        //:    'bslma_TestAllocator' address.
        //: 11 Concerns 1 through 10 also apply to 'BslmaAllocator'.
        //: 12 Concerns 1 through 10 also apply to 'FunkyAllocator'.
        //: 13 A 'bslma_Allocator*' can be implicitly converted to a
        //:    'BslmAllocator' object.
        //: 14 An 'AttribClass5' object can be constructed with 0 to 5
        //:    arguments, storing each argument in a separate attribute and
        //:    using known defaults for any remaining attributes.
        //: 15 An 'AttribClass5Alloc' object can be constructed with 0 to 5
        //:    arguments, storing each argument in a separate attribute and
        //:    using known defaults for any remaining attributes.
        //: 16 An 'AttribClass5Alloc' object can be constructed with 0 to 5
        //:    arguments plus an allocator argument, storing each argument in
        //:    a separate attribute and using known defaults for any remaining
        //:    attributes.
        //: 17 If a 'AttribClass5Alloc' object is constructed without an
        //:    allocator argument, then the default allocator is used.
        //: 18 Concerns 15 to 17 also apply to 'AttribClass5bslma' except that
        //:    the allocator attribute is of type 'bslma_Allocator*' instead
        //:    of a standard allocator type.
        //: 19 Within this test driver, all of the allocators will store the
        //:    most recent hint passed to 'allocate' in the 'g_lastHint'
        //:    global variable.
        //
        // Plan:
        //: o Create a function template, 'testAllocatorConformance', to check
        //:   C++03 conformance of 'NonBslmaAllocator', 'BslmaAllocator', and
        //:   'FunkyAllocator'.  The test function simply and directly tests
        //:   each allocator requirement from C++03.  (C1-C12).
        //: o Use 'bslmf_IsConvertible' to test that 'bslma_Allocator*' is
        //:   convertible to 'BslmaAllocator' and that it is NOT convertible
        //:   to 'NonBslmaAllocator' (C13).
        //: o Create a function template, 'testAttribClass' that tests an
        //:   attribute class by constructing it with 0 to 5 arguments and
        //:   testing its attributes. (C15)
        //: o Also within 'testAttribClass', test that the attribute class can
        //:   be constructed with an additional allocator argument (and that
        //:   it has the correct default).  (C16-C17)
        //: o Specialize 'testAttribClass' for 'AttribClass5', which has no
        //:   allocator. (C18)
        //: o Instantiate 'testAttribClass' with each meaningful combination
        //:   of attribute classes ('AttribClass5', 'AttribClass5Alloc', and
        //:   'AttribClass5bslma') and allocator type ('NonBslmaAllocator',
        //:   'BslmaAllocator', 'FunkyAllocator', 'bslma_Allocator*', and no
        //:   allocator). (C16-C18)
        //: o Within 'testAllocatorConformance', set 'g_lastHint' to a known
        //:   address before calling 'allocate' with no hint.  Verify that
        //:   'g_lastHint' gets set to null.  Repeat this test, but pass a
        //:   second known address as a hint, verifying that 'g_lastHint' gets
        //:   set to the second known address.
        //
        // Testing
        //   template <class T> class NonBsmlaAllocator;
        //   template <class T> class BsmlaAllocator;
        //   template <class T> class FunkyAllocator;
        //   class AttribClass5;
        //   class AttribClass5Alloc;
        //   class AttribClass5bslma;
        //   template <class T>
        //     bool matchAttrib(const T& v, char a, int b, double c,
        //                      const char *d, void *e);
        //   template <class T, class ALLOC>
        //     bool matchAttrib(const T& v, char a, int b, double c,
        //                      const char *d, void *e, const ALLOC& alloc);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST HARNESS"
                            "\n====================\n");

        testAllocatorConformance<NonBslmaAllocator>("NonBslmaAllocator");
        testAllocatorConformance<BslmaAllocator>("BslmaAllocator");
        testAllocatorConformance<FunkyAllocator>("FunkyAllocator");

        if (verbose)
            printf("Testing convertibility from 'bslma_Allocator*'\n");

        ASSERT(  (bslmf_IsConvertible<bslma_Allocator*,
                                      BslmaAllocator<int> >::VALUE));
        ASSERT(  (bslmf_IsConvertible<bslma_Allocator*,
                                      BslmaAllocator<AttribClass5> >::VALUE));
        ASSERT(! (bslmf_IsConvertible<bslma_Allocator*,
                                    NonBslmaAllocator<AttribClass5> >::VALUE));
        ASSERT(! (bslmf_IsConvertible<bslma_Allocator*,
                                      FunkyAllocator<int> >::VALUE));
        ASSERT(! (bslmf_IsConvertible<bslma_Allocator*,
                                      FunkyAllocator<AttribClass5> >::VALUE));

        testAttribClass<AttribClass5>("AttribClass5");
        testAttribClass<AttribClass5Alloc<NonBslmaAllocator<int> > >(
            "AttribClass5Alloc<NonBslmaAllocator<int> >");
        testAttribClass<AttribClass5Alloc<BslmaAllocator<char> > >(
            "AttribClass5Alloc<BslmaAllocator<char> >");
        testAttribClass<AttribClass5Alloc<FunkyAllocator<char> > >(
            "AttribClass5Alloc<FunkyAllocator<char> >");
        testAttribClass<AttribClass5bslma>("AttribClass5bslma");
        
      } break;
      case 1: {
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
            ASSERT(matchAttrib(*p, 'x', 88, 0.25, DFLT_D, DFLT_E));

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
            ASSERT(matchAttrib(*p, 'x', 88, 0.25, bye, DFLT_E, &ta));

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
