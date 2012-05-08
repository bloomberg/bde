// bslstl_allocatortraits.t.cpp                                       -*-C++-*-
#include <bslstl_allocatortraits.h>

#include <bslstl_allocator.h>

#include <bslalg_typetraitsgroupstlsequence.h>
#include <bslalg_typetraits.h>
#include <bslma_testallocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslmf_issame.h>
#include <bslmf_removecvq.h>
#include <bsls_bsltestutil.h>
#include <bsls_objectbuffer.h>
#include <bsls_util.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

using namespace BloombergLP;
using namespace bsl;

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
// 'bslma::Allocator*' argument, then the allocator itself is passed as an
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
// [ 5] pointer allocate(ALLOC& a, size_type n);
// [ 5] pointer allocate(ALLOC& a, size_type n, const_void_pointer hint);
// [ 5] void deallocate(ALLOC& a, pointer p, size_type n);
// [ 6] void construct(ALLOC& a, T *p, Args&&... args);
// [ 6] void destroy(ALLOC& a, T* p);
// [ 7] size_type max_size(const ALLOC& a);
// [ 8] ALLOC select_on_container_copy_construction(const ALLOC& rhs);
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 9] USAGE EXAMPLE
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
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//-----------------------------------------------------------------------------

// Short-cut assert macros
#define ASSERT_ISSAME(t1,t2) ASSERT((bslmf::IsSame< t1,t2>::VALUE))

#define LOOP_ASSERT_ISSAME(I,t1,t2) \
        LOOP_ASSERT(I, (bslmf::IsSame< t1,t2>::VALUE))

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// 'g_x' is a dummy object with a unique address
struct Uniq { } g_x;

// The following are default values for the 5 attributes of our test classes.
char        const DFLT_A = ' ';
int         const DFLT_B = 99;
double      const DFLT_C = 1.0;
const char *const DFLT_D = "hello";
Uniq       *const DFLT_E = &g_x;

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
    bslma::Allocator *d_mechanism;

  public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T              *pointer;
    typedef const T        *const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    // A client that uses this allocator should not have the
    // bslalg::TypeTraitUsesBslmaAllocator trait.  This is a stand-in trait.
    typedef struct UsesNonBslma { } ClientTrait;

    template <class U>
    struct rebind
    {
        typedef NonBslmaAllocator<U> other;
    };

    explicit NonBslmaAllocator(bslma::Allocator *basicAlloc = 0)
        : d_mechanism(bslma::Default::allocator(basicAlloc)) { }

    pointer allocate(size_type n, const void *hint = 0) {
        g_lastHint = hint;
        return pointer(d_mechanism->allocate(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(p); }

    void construct(T *p, const T& val) { ::new ((void*)p) T(val); }
    void destroy(T *p) { p->~T(); }

    // ACCESSORS
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    size_type max_size() const { return INT_MAX / sizeof(T); }

    bslma::Allocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const NonBslmaAllocator<T>& lhs,
                const NonBslmaAllocator<U>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T, class U>
inline
bool operator!=(const NonBslmaAllocator<T>& lhs,
                const NonBslmaAllocator<U>& rhs)
{
    return lhs.mechanism() != rhs.mechanism();
}

template <class T>
class BslmaAllocator
{
    // This class is a C++03-compliant allocator that conforms to the
    // 'bslma' allocator model (i.e., it is convertible from
    // 'bslma::Allocator*'.
    bslma::Allocator *d_mechanism;

    void doConstruct(T *p, const T& val, bslmf::MetaInt<0>) {
        ::new ((void*)p) T(val); 
    }

    void doConstruct(T *p, const T& val, bslmf::MetaInt<1>) {
        ::new ((void*)p) T(val, this->d_mechanism); 
    }

  public:
    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T              *pointer;
    typedef const T        *const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T               value_type;

    template <class U>
    struct rebind
    {
        typedef BslmaAllocator<U> other;
    };

    // A client that uses this allocator should have this trait
    typedef bslalg::TypeTraitUsesBslmaAllocator ClientTrait;

    BslmaAllocator(bslma::Allocator *basicAlloc = 0)
        : d_mechanism(bslma::Default::allocator(basicAlloc)) { }

    pointer allocate(size_type n, const void *hint = 0) {
        g_lastHint = hint;
        return pointer(d_mechanism->allocate(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(p); }

    void construct(T *p, const T& val) 
    {
        typedef
        typename bslalg::HasTrait<T, bslalg::TypeTraitUsesBslmaAllocator>::Type
                                                            UsesBslmaAllocator;
        doConstruct(p, val, UsesBslmaAllocator());
    }
    void destroy(T *p) { p->~T(); }

    // ACCESSORS
    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    size_type max_size() const { return INT_MAX / sizeof(T); }

    bslma::Allocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const BslmaAllocator<T>& lhs, const BslmaAllocator<U>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T, class U>
inline
bool operator!=(const BslmaAllocator<T>& lhs, const BslmaAllocator<U>& rhs)
{
    return lhs.mechanism() != rhs.mechanism();
}

template <class T>
class FunkyPointer
{
    // Pointer-like class for testing use of non-raw pointers in allocators.
    T* d_imp;

  public:
    FunkyPointer() : d_imp(0) { }
    FunkyPointer(T* p, int) : d_imp(p) { }
    FunkyPointer(const FunkyPointer<typename bslmf::RemoveCvq<T>::Type>& other)
        : d_imp(other.operator->()) { }

    // Construct from null pointer
    FunkyPointer(int FunkyPointer::*) : d_imp(0) { }

    T& operator*() const { return *d_imp; }
    T* operator->() const { return d_imp; }
};

template <class T>
inline
bool operator==(FunkyPointer<T> lhs, FunkyPointer<T> rhs)
{
    return lhs.operator->() == rhs.operator->();
}

template <class T>
inline
bool operator!=(FunkyPointer<T> lhs, FunkyPointer<T> rhs)
{
    return lhs.operator->() != rhs.operator->();
}

template <class T>
class FunkyAllocator
{
    // Allocator that uses 'FunkyPointer' as its pointer type.  Not all
    // allocator-aware classes can work with such an allocator, but
    // 'allocator_traits' should work fine.
    // This class is a C++03-compliant allocator that conforms to the
    // 'bslma' allocator model (i.e., it is convertible from
    // 'bslma::Allocator*'.
    bslma::Allocator *d_mechanism;

    void doConstruct(T *p, const T& val, bslmf::MetaInt<0>) {
        ::new ((void*)p) T(val); 
    }

    void doConstruct(T *p, const T& val, bslmf::MetaInt<1>) {
        ::new ((void*)p) T(val, this->d_mechanism); 
    }

  public:
    // PUBLIC TYPES
    typedef std::size_t           size_type;
    typedef std::ptrdiff_t        difference_type;
    typedef FunkyPointer<T>       pointer;
    typedef FunkyPointer<const T> const_pointer;
    typedef T&                    reference;
    typedef const T&              const_reference;
    typedef T                     value_type;

    template <class U>
    struct rebind
    {
        typedef FunkyAllocator<U> other;
    };

    // A client that uses this allocator should have this trait
    typedef bslalg::TypeTraitUsesBslmaAllocator ClientTrait;

    FunkyAllocator(bslma::Allocator *basicAlloc = 0)
        : d_mechanism(bslma::Default::allocator(basicAlloc)) { }

    pointer allocate(size_type n, const void *hint = 0) {
        g_lastHint = hint;
        return pointer((T*) d_mechanism->allocate(n * sizeof(T)), 0);
    }

    void deallocate(pointer p, size_type n = 1)
        { d_mechanism->deallocate(bsls::Util::addressOf(*p)); }

    void construct(T *p, const T& val) 
    {
        typedef
        typename bslalg::HasTrait<T, bslalg::TypeTraitUsesBslmaAllocator>::Type
                                                            UsesBslmaAllocator;
        doConstruct(p, val, UsesBslmaAllocator());
    }
    void destroy(T *p) { p->~T(); }

    // ACCESSORS
    pointer address(reference x) const
                               { return pointer(bsls::Util::addressOf(x), 0); }
    const_pointer address(const_reference x) const
                         { return const_pointer(bsls::Util::addressOf(x), 0); }
    size_type max_size() const { return INT_MAX / sizeof(T); }

    bslma::Allocator *mechanism() const { return d_mechanism; }
};

template <class T, class U>
inline
bool operator==(const FunkyAllocator<T>& lhs, const FunkyAllocator<U>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T, class U>
inline
bool operator!=(const FunkyAllocator<T>& lhs, const FunkyAllocator<U>& rhs)
{
    return lhs.mechanism() != rhs.mechanism();
}

struct AttribStruct5
{
    // This test struct has up to 5 attributes of different types.  It is a
    // simple aggregate type so it can be statically constructed.
    char        d_a;
    int         d_b;
    double      d_c;
    const char *d_d;
    Uniq       *d_e;
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

    typedef bslma::Allocator* AllocatorType;

    explicit AttribClass5(char        a = DFLT_A,
                          int         b = DFLT_B,
                          double      c = DFLT_C,
                          const char *d = DFLT_D,
                          Uniq       *e = DFLT_E)
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
    Uniq       *e() const { return d_attrib.d_e; }

    AllocatorType allocator() const { return bslma::Default::allocator(0); }
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
    // Use the client trait exported by the allocator.  If the allocator uses
    // the bslma model, then this type will be
    // 'bslalg::TypeTraitUsesBslmaAllocator'
    typedef typename ALLOC::ClientTrait UsesAllocTrait;
    BSLALG_DECLARE_NESTED_TRAITS(AttribClass5Alloc, UsesAllocTrait);

    typedef ALLOC AllocatorType;

    explicit AttribClass5Alloc(const ALLOC& alloc = ALLOC())
        : d_attrib(), d_allocator(alloc) { }
    explicit AttribClass5Alloc(char a, const ALLOC& alloc = ALLOC())
        : d_attrib(a), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, const ALLOC& alloc = ALLOC())
        : d_attrib(a, b), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, double c, const ALLOC& alloc = ALLOC())
        : d_attrib(a, b, c), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, double c, const char *d,
                      const ALLOC& alloc = ALLOC())
        : d_attrib(a, b, c, d), d_allocator(alloc) { }
    AttribClass5Alloc(char a, int b, double c, const char *d, Uniq *e,
                      const ALLOC& alloc = ALLOC())
        : d_attrib(a, b, c, d, e), d_allocator(alloc) { }
    AttribClass5Alloc(const AttribClass5Alloc& other)
        : d_attrib(other.d_attrib)
        , d_allocator(bslmf::IsConvertible<bslma::Allocator*,ALLOC>::VALUE ?
                      ALLOC() : other.d_allocator)
        { }
    AttribClass5Alloc(const AttribClass5Alloc& other, const ALLOC& alloc)
        : d_attrib(other.d_attrib), d_allocator(alloc) { }

    char        a() const { return d_attrib.a(); }
    int         b() const { return d_attrib.b(); }
    double      c() const { return d_attrib.c(); }
    const char *d() const { return d_attrib.d(); }
    Uniq       *e() const { return d_attrib.e(); }

    AllocatorType allocator() const { return d_allocator; }

    friend void operator&(AttribClass5Alloc&) { }
};

class AttribClass5bslma
{
    // This test class has up to 5 constructor arguments plus an (optional)
    // address of a 'bslma::Allocator'.  This class conforms to the 'bslma'
    // allocator model.

    AttribClass5      d_attrib;
    bslma::Allocator* d_allocator;

public:
    BSLALG_DECLARE_NESTED_TRAITS(AttribClass5bslma,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    typedef bslma::Allocator* AllocatorType;

    explicit AttribClass5bslma(bslma::Allocator *alloc = 0)
        : d_attrib(), d_allocator(bslma::Default::allocator(alloc)) { }

    explicit AttribClass5bslma(char a, bslma::Allocator *alloc = 0)
        : d_attrib(a), d_allocator(bslma::Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, bslma::Allocator *alloc = 0)
        : d_attrib(a, b), d_allocator(bslma::Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, double c, bslma::Allocator *alloc = 0)
        : d_attrib(a, b, c), d_allocator(bslma::Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, double c, const char *d,
                      bslma::Allocator *alloc = 0)
        : d_attrib(a, b, c, d)
        , d_allocator(bslma::Default::allocator(alloc)) { }
    AttribClass5bslma(char a, int b, double c, const char *d, Uniq *e,
                      bslma::Allocator *alloc = 0)
        : d_attrib(a, b, c, d, e)
        , d_allocator(bslma::Default::allocator(alloc)) { }
    AttribClass5bslma(const AttribClass5bslma& other,
                      bslma::Allocator *alloc = 0)
        : d_attrib(other.d_attrib)
        , d_allocator(bslma::Default::allocator(alloc)) { }

    char        a() const { return d_attrib.a(); }
    int         b() const { return d_attrib.b(); }
    double      c() const { return d_attrib.c(); }
    const char *d() const { return d_attrib.d(); }
    Uniq       *e() const { return d_attrib.e(); }

    bslma::Allocator *allocator() const { return d_allocator; }
};

template <class T>
bool matchAttrib(const T& v, char a, int b, double c, const char *d, Uniq *e)
{
    return v.a() == a && v.b() == b && v.c() == c && v.d() == d && v.e() == e;
}

template <class T, class ALLOC>
bool matchAttrib(const T& v, char a, int b, double c, const char *d, Uniq *e,
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

///Example 1: A Container Class
///- - - - - - - - - - - - - -
// This example demonstrates the intended use of 'allocator_traits' to
// implement a standard-conforming container class.  First, we create a
// container class that holds a single object and which meets the requirements
// both of a standard container and of a Bloomberg container.  I.e., when
// instantiated with an allocator argument it uses the standard allocator
// model; otherwise it uses the 'bslma' model.  We provide an alias,
// 'AllocTraits', to the specific 'allocator_traits' instantiation to simplify
// the implementation of each method that must allocate memory, or create or
// destroy elements.
//..
    #include <bslstl_allocatortraits.h>
    #include <bslstl_allocator.h>
    #include <bslalg_typetraitsgroupstlsequence.h>

    using namespace BloombergLP;

    template <class TYPE, class ALLOC = bsl::allocator<TYPE> >
    class MyContainer {
        // This class provides a container that always holds exactly one
        // element, dynamically allocated using the specified allocator.

        typedef bsl::allocator_traits<ALLOC> AllocTraits;
            // Alias for the 'allocator_traits' instantiation to use for
            // all memory management requests.

        // DATA
        ALLOC  d_allocator;
        TYPE  *d_valuep;

      public:
        // TRAITS
        typedef bslalg::TypeTraitsGroupStlSequence<TYPE,ALLOC> TypeTraits;
        BSLALG_DECLARE_NESTED_TRAITS(MyContainer, TypeTraits);
            // Declare nested type traits for this class.

        typedef TYPE  value_type;
        typedef ALLOC allocator_type;
        // etc.

        // CREATORS
        explicit MyContainer(const ALLOC& a = ALLOC());
        explicit MyContainer(const TYPE& v, const ALLOC& a = ALLOC());
        MyContainer(const MyContainer& other);
        MyContainer(const MyContainer& other, const ALLOC& a);
        ~MyContainer();

        // MANIPULATORS
        ALLOC get_allocator() const { return d_allocator; }

        // ACCESSORS
        TYPE&       front()       { return *d_valuep; }
        const TYPE& front() const { return *d_valuep; }
        // etc.
    };
//..
// Then we implement the constructors, which allocate memory and construct a
// 'TYPE' object in the allocated memory.  Because the allocation and
// construction are done in two separate steps, we need to create a proctor
// that will deallocate the allocated memory in case the constructor throws an
// exception.  The proctor uses the uniform interface provided by
// 'allocator_traits' to access the 'pointer' and 'deallocate' members of
// 'ALLOC':
//..
    template <class ALLOC>
    class MyContainerProctor {
        // This class implements a proctor to release memory allocated during
        // the construction of a 'MyContainer' object if the constructor for
        // the container's data element throws an exception.  Such a proctor
        // should be 'release'd once the element is safely constructed.

        typedef typename bsl::allocator_traits<ALLOC>::pointer pointer;
        ALLOC   d_alloc;
        pointer d_datap;

      public:
        MyContainerProctor(const ALLOC& a, pointer p)
            : d_alloc(a), d_datap(p) { }

        ~MyContainerProctor() {
            if (d_datap) {
                bsl::allocator_traits<ALLOC>::deallocate(d_alloc, d_datap, 1);
            }
        }

        void release() { d_datap = pointer(); }
    };
//..
// Next, we perform the actual allocation and construction using the
// 'allocate' and 'construct' members of 'allocator_traits', which provide the
// correct semantic for passing the allocator to the constructed object when
// appropriate:
//..
    template <class TYPE, class ALLOC>
    MyContainer<TYPE, ALLOC>::MyContainer(const ALLOC& a)
        : d_allocator(a)
    {
        d_valuep = AllocTraits::allocate(d_allocator, 1);
        MyContainerProctor<ALLOC> proctor(a, d_valuep);
        // Call 'construct' with no constructor arguments
        AllocTraits::construct(d_allocator, d_valuep);
        proctor.release();
    }

    template <class TYPE, class ALLOC>
    MyContainer<TYPE, ALLOC>::MyContainer(const TYPE& v, const ALLOC& a)
        : d_allocator(a)
    {
        d_valuep = AllocTraits::allocate(d_allocator, 1);
        MyContainerProctor<ALLOC> proctor(a, d_valuep);
        // Call 'construct' with one constructor argument of type 'TYPE'
        AllocTraits::construct(d_allocator, d_valuep, v);
        proctor.release();
    }
//..
// Next, the copy constructor for 'MyContainer' needs to conditionally copy the
// allocator from the 'other' container.  The copy constructor uses
// 'allocator_traits::select_on_container_copy_construction' to decide whether
// to copy the 'other' allocator (for non-bslma allocators) or to
// default-construct the allocator (for bslma allocators).
//..
    template <class TYPE, class ALLOC>
    MyContainer<TYPE, ALLOC>::MyContainer(const MyContainer& other)
        : d_allocator(bsl::allocator_traits<ALLOC>::
                      select_on_container_copy_construction(other.d_allocator))
    {
        d_valuep = AllocTraits::allocate(d_allocator, 1);
        MyContainerProctor<ALLOC> proctor(d_allocator, d_valuep);
        AllocTraits::construct(d_allocator, d_valuep, *other.d_valuep);
        proctor.release();
    }
//..
// Now, the destructor uses 'allocator_traits' functions to destroy and
// deallocate the value object:
//..
    template <class TYPE, class ALLOC>
    MyContainer<TYPE, ALLOC>::~MyContainer()
    {
        AllocTraits::destroy(d_allocator, d_valuep);
        AllocTraits::deallocate(d_allocator, d_valuep, 1);
    }
//..
// Finally, we perform a simple test of 'MyContainer', instantiating it with
// element type 'int':
//..
    int usageExample1()
    {
        bslma::TestAllocator testAlloc;
        MyContainer<int> C1(123, &testAlloc);
        ASSERT(C1.get_allocator() == bsl::allocator<int>(&testAlloc));
        ASSERT(C1.front() == 123);

        MyContainer<int> C2(C1);
        ASSERT(C2.get_allocator() == bsl::allocator<int>());
        ASSERT(C2.front() == 123);

        return 0;
    }
//..
///Example 2: bslma Allocator Propagation
///- - - - - - - - - - - - - - - - - - -
// To exercise the propagation of the allocator of 'MyContainer' to its
// elements, we first create a representative element class, 'MyType', that
// allocates memory using the bslma allocator protocol:
//..
    #include <bslma_default.h>

    class MyType {

        bslma::Allocator *d_allocator_p;
        // etc.
      public:
        // TRAITS
        BSLALG_DECLARE_NESTED_TRAITS(MyType,
                                     bslalg::TypeTraitUsesBslmaAllocator);

        // CREATORS
        explicit MyType(bslma::Allocator* basicAlloc = 0)
            : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
        MyType(const MyType& other)
            : d_allocator_p(bslma::Default::allocator(0)) { /* ... */ }
        MyType(const MyType& other, bslma::Allocator* basicAlloc)
            : d_allocator_p(bslma::Default::allocator(basicAlloc)) { /* ... */ }
        // etc.

        // ACCESSORS
        bslma::Allocator *allocator() const { return d_allocator_p; }
        // etc.
    };
//..
// Finally, we instantiate 'MyContainer' using 'MyType' and verify that, when
// we provide a the address of an allocator to the constructor of the
// container, the same address is passed to the constructor of the container's
// element.  We also verify that, when the container is copy-constructed, the
// copy uses the default allocator, not the allocator from the original;
// moreover, we verify that the element stored in the copy also uses the
// default allocator.
//..
    #include <bslmf_issame.h>

    int usageExample2()
    {
        bslma::TestAllocator testAlloc;
        MyContainer<MyType> C1(&testAlloc);
        ASSERT((bslmf::IsSame<MyContainer<MyType>::allocator_type,
                bsl::allocator<MyType> >::VALUE));
        ASSERT(C1.get_allocator() == bsl::allocator<MyType>(&testAlloc));
        ASSERT(C1.front().allocator() == &testAlloc);

        MyContainer<MyType> C2(C1);
        ASSERT(C2.get_allocator() != C1.get_allocator());
        ASSERT(C2.get_allocator() == bsl::allocator<MyType>());
        ASSERT(C2.front().allocator() != &testAlloc);
        ASSERT(C2.front().allocator() == bslma::Default::defaultAllocator());

        return 0;
    }
//..
///Example 3: C++03 Allocators
///- - - - - - - - - - - - - -
// This example shows that when 'MyContainer' is instantiated with a C++03
// allocator, that the allocator is a) copied on copy construction and b) is
// not propagated from the container to its elements.  First, we create a
// C++03-style allocator class template:
//..
    template <class TYPE>
    class MyCpp03Allocator {
        int d_state;

    public:
        typedef TYPE        value_type;
        typedef TYPE       *pointer;
        typedef const TYPE *const_pointer;
        typedef unsigned    size_type;
        typedef int         difference_type;

        template <class U>
        struct rebind {
            typedef MyCpp03Allocator<U> other;
        };

        explicit MyCpp03Allocator(int state = 0) : d_state(state) { }

        TYPE* allocate(size_type n, const void* = 0)
            { return (TYPE*) ::operator new(sizeof(TYPE) * n); }

        void deallocate(TYPE* p, size_type) { ::operator delete(p); }

        static size_type max_size() { return UINT_MAX / sizeof(TYPE); }

        void construct(pointer p, const TYPE& value)
            { new((void *)p) TYPE(value); }

        void destroy(pointer p) { p->~TYPE(); }

        int state() const { return d_state; }
    };

    template <class TYPE1, class TYPE2>
    inline
    bool operator==(const MyCpp03Allocator<TYPE1>& lhs,
                    const MyCpp03Allocator<TYPE2>& rhs)
    {
        return lhs.state() == rhs.state();
    }

    template <class TYPE1, class TYPE2>
    inline
    bool operator!=(const MyCpp03Allocator<TYPE1>& lhs,
                    const MyCpp03Allocator<TYPE2>& rhs)
    {
        return ! (lhs == rhs);
    }
//..
// Finally we instantiate 'MyContainer' using this allocator type and verify
// that elements are constructed using the default allocator (because the
// allocator is not propagated from the container).  We also verify that the
// allocator is copied on copy-construction:
//..
    int usageExample3()
    {
        typedef MyCpp03Allocator<MyType> MyTypeAlloc;
        MyContainer<MyType, MyTypeAlloc> C1(MyTypeAlloc(1));
        ASSERT((bslmf::IsSame<MyContainer<MyType, MyTypeAlloc>::allocator_type,
                              MyTypeAlloc>::VALUE));
        ASSERT(C1.get_allocator() == MyTypeAlloc(1));
        ASSERT(C1.front().allocator() == bslma::Default::defaultAllocator());

        MyContainer<MyType, MyTypeAlloc> C2(C1);
        ASSERT(C2.get_allocator() == C1.get_allocator());
        ASSERT(C2.get_allocator() != MyTypeAlloc())
        ASSERT(C2.front().allocator() == bslma::Default::defaultAllocator());

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

    bslma::TestAllocator default_ta, ta;
    bslma::DefaultAllocatorGuard guard(&default_ta);

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

    // The following are default values for the 5 attributes of our test classes.
    char        const VAL_A = 'x';
    int         const VAL_B = 6;
    double      const VAL_C = -1.5;
    const char *const VAL_D = "pizza";
    Uniq       *const VAL_E = 0;

    if (veryVerbose) printf("  Testing member functions\n");
    AttribClass5 val(VAL_A, VAL_B, VAL_C, VAL_D, VAL_E);
    LOOP6_ASSERT(allocName, val.a(), val.b(), val.c(), val.d(), val.e(),
                 matchAttrib(val, VAL_A, VAL_B, VAL_C, VAL_D, VAL_E));

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

    a.construct(bsls::Util::addressOf(*p), val);
    LOOP_ASSERT(allocName, AttribClass5::ctorCount() == ctorCountBefore + 1);
    LOOP6_ASSERT(allocName, p->a(), p->b(), p->c(), p->d(), p->e(),
                 matchAttrib(*p, VAL_A, VAL_B, VAL_C, VAL_D, VAL_E));

    // Test that 'pointer' can convert to 'const_pointer'
    cp = p;
    LOOP_ASSERT(allocName, isMutable(*p));
    LOOP_ASSERT(allocName, ! isMutable(*cp));

    // Test that dereferencing a pointer yields a reference
    value_type& v        = *p;
    const value_type& cv = *cp;
    LOOP6_ASSERT(allocName, cv.a(), cv.b(), cv.c(), cv.d(), cv.e(),
                 matchAttrib(cv, VAL_A, VAL_B, VAL_C, VAL_D, VAL_E));

    LOOP_ASSERT(allocName, p  == a.address(v));
    LOOP_ASSERT(allocName, cp == a.address(cv));

    a.destroy(bsls::Util::addressOf(*p));
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
    Uniq addresses[5];

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

    bslma::TestAllocator ta;
    const Alloc DFLT_ALLOC(bslma::Default::allocator(0));
    const Alloc TST_ALLOC(&ta);

    for (int i = 0; i < NUM_DATA; ++i) {
        const char        A = DATA[i].d_a;
        const int         B = DATA[i].d_b;
        const double      C = DATA[i].d_c;
        const char *const D = DATA[i].d_d;
        Uniq       *const E = DATA[i].d_e;

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
    Uniq addresses[5];

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
        Uniq       *const E = DATA[i].d_e;

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
                (bslmf::IsConvertible<
                 typename Traits::propagate_on_container_copy_assignment*,
                 bslmf::MetaInt<0>* >::VALUE));
    LOOP_ASSERT(allocName,
                (bslmf::IsConvertible<
                 typename Traits::propagate_on_container_move_assignment*,
                 bslmf::MetaInt<0>* >::VALUE));
    LOOP_ASSERT(allocName,
                (bslmf::IsConvertible<
                 typename Traits::propagate_on_container_swap*,
                 bslmf::MetaInt<0>* >::VALUE));
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
    LOOP_ASSERT(testName, (bslmf::IsConvertible<
                           AllocTReboundU*, AllocU*>::VALUE));
    LOOP_ASSERT(testName, (bslmf::IsConvertible<
                           TraitsTReboundU*, TraitsU*>::VALUE));
    LOOP_ASSERT_ISSAME(testName,
                       typename TraitsTReboundU::allocator_type, AllocU);

    // Rebind from 'U' to 'T'
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<AllocUReboundT*, AllocT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<TraitsUReboundT*, TraitsT*>::VALUE));
    LOOP_ASSERT_ISSAME(testName,
                       typename TraitsUReboundT::allocator_type, AllocT);

    // Rebind from 'T' to 'T'
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<AllocTReboundT*, AllocT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<TraitsTReboundT*, TraitsT*>::VALUE));
    LOOP_ASSERT_ISSAME(testName,
                       typename TraitsTReboundT::allocator_type, AllocT);

    // Multiple rebind
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_alloc<T>*,
                 AllocT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_traits<T>*,
                 TraitsT*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_alloc<U>*,
                 AllocU*>::VALUE));
    LOOP_ASSERT(testName,
                (bslmf::IsConvertible<
                   typename allocator_traits<AllocUReboundT>::
                                             template rebind_traits<U>*,
                 TraitsU*>::VALUE));
}

template <class ALLOC>
void testAllocateDeallocate(const char *name)
{
    typedef allocator_traits<ALLOC>          TraitsT;
    typedef typename TraitsT::value_type     value_type;
    typedef typename TraitsT::pointer        pointer;
    typedef typename TraitsT::size_type      size_type;

    const size_type SIZES[] = { 1, 2, 10 };
    const int       NUM_SIZES = sizeof(SIZES) / sizeof(SIZES[0]);

    pointer pointers[NUM_SIZES * 2];

    bslma::TestAllocator ta;

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
        LOOP2_ASSERT(name, N, ta.lastAllocatedAddress() ==
                              bsls::Util::addressOf(*pointers[i]));
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

template <class ALLOC, class T>
void testConstructDestroy(const char *allocname,
                          const char *tname,
                          bool        scoped)
{
    typedef allocator_traits<ALLOC> TraitsT;
    typedef AttribClass5            C;

    if (verbose) {
        printf("Testing construct & destroy for alloc %s & obj type %s\n",
               allocname, tname);
    }

    bslma::TestAllocator default_ta, ta;
    bslma::DefaultAllocatorGuard guard(&default_ta);

    ALLOC a(&ta);

    // Expected allocator for object after construction will be a copy of 'a'
    // if the scoped model is used or the default allocator otherwise.  Note
    // that 'T::AllocatorType might be different than 'ALLOC', in which case
    // 'scoped' must be false.
    typename T::AllocatorType exp_a(scoped ? &ta : &default_ta);

    // Contents of this array are unimportant -- only their addresses are used.
    Uniq addresses[5];

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
        Uniq       *const E = DATA[i].d_e;

        bsls::ObjectBuffer<T> objects[7];
        int expCtorCount = C::ctorCount();
        int expDtorCount = C::dtorCount();

        TraitsT::construct(a, bsls::Util::addressOf(objects[0].object()));
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[0].object(), DFLT_A, DFLT_B, DFLT_C,
                                 DFLT_D, DFLT_E, exp_a));

        TraitsT::construct(a, bsls::Util::addressOf(objects[1].object()), A);
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[1].object(), A, DFLT_B, DFLT_C,
                                 DFLT_D, DFLT_E, exp_a));

        TraitsT::construct(a, bsls::Util::addressOf(objects[2].object()),
                              A, B);
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[2].object(), A, B, DFLT_C, DFLT_D,
                                 DFLT_E, exp_a));

        TraitsT::construct(a, bsls::Util::addressOf(objects[3].object()),
                              A, B, C);
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[3].object(), A, B, C, DFLT_D, DFLT_E,
                                 exp_a));

        TraitsT::construct(a, bsls::Util::addressOf(objects[4].object()),
                              A, B, C, D);
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[4].object(), A, B, C, D, DFLT_E,
                                 exp_a));

        TraitsT::construct(a, bsls::Util::addressOf(objects[5].object()),
                              A, B, C, D, E);
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[5].object(), A, B, C, D, E, exp_a));

        TraitsT::construct(a, bsls::Util::addressOf(objects[6].object()),
                              objects[5].object());
        LOOP3_ASSERT(allocname, tname, i, C::ctorCount() == ++expCtorCount);
        LOOP3_ASSERT(allocname, tname, i, C::dtorCount() == expDtorCount);
        LOOP3_ASSERT(allocname, tname, i,
                     matchAttrib(objects[6].object(), A, B, C, D, E, exp_a));

        for (int j = 0; j < 7; ++j) {
            TraitsT::destroy(a, bsls::Util::addressOf(objects[i].object()));
            LOOP3_ASSERT(allocname,tname,i, C::ctorCount() == expCtorCount);
            LOOP3_ASSERT(allocname,tname,i, C::dtorCount() == ++expDtorCount);
            LOOP3_ASSERT(allocname,tname,i,
                         0xdeadbeaf == objects[i].object().b());
        }
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
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage examples in the header file compile.
        //: 2 The usage examples in the header produces correct results.
        //
        // Test plan:
        //: o Copy the usage examples from the header into this test driver,
        //:   replacing 'assert' with 'ASSERT' and 'main' with
        //:   'usageExample1', 'usageExample2', and 'usageExample3'. (C1)
        //: o Call 'usageExample1', 'usageExample2', and 'usageExample3' (C2)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting USAGE EXAMPLE"
                            "\n=====================\n");

        usageExample1();
        usageExample2();
        usageExample3();

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SELECT_ON_CONTAINER_COPY_CONSTRUCTION
        //
        // Concerns:
        //: 1 For an allocator 'a' of type 'ALLOC' that is convertible from
        //:   'bslma::Allocator*', 'allocator_traits<ALLOC>::
        //:   select_on_container_copy_construction(a)' returns 'ALLOC()'
        //: 2 For an allocator 'a' of type 'ALLOC' that is NOT convertible
        //:   from 'bslma::Allocator*', 'allocator_traits<ALLOC>::
        //:   select_on_container_copy_construction(a)' returns 'a'
        //
        // Plan
        //: o For a variety of allocators, test that the function under test
        //:   returns the appropriate result.
        //
        // Testing:
        //   ALLOC select_on_container_copy_construction(const ALLOC& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting select_on_container_copy_construction"
                            "\n============================================="
                            "\n");

#define TEST_SOCCC_COPY(ALLOC) {                                             \
            ALLOC a;                                                         \
            typedef allocator_traits<ALLOC> AT;                              \
            ASSERT(AT::select_on_container_copy_construction(a) == a);       \
        } break;

#define TEST_SOCCC_DFLT(ALLOC) {                                             \
            ALLOC a;                                                         \
            typedef allocator_traits<ALLOC> AT;                              \
            ASSERT(AT::select_on_container_copy_construction(a) == ALLOC()); \
        } break;

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<FunkyAllocator<int> >    AC5AllocFunky;

        TEST_SOCCC_COPY(NonBslmaAllocator<int>);
        TEST_SOCCC_DFLT(BslmaAllocator<int>);
        TEST_SOCCC_DFLT(FunkyAllocator<int>);

        TEST_SOCCC_COPY(NonBslmaAllocator<AttribClass5>);
        TEST_SOCCC_DFLT(BslmaAllocator<AttribClass5>);
        TEST_SOCCC_DFLT(FunkyAllocator<AttribClass5>);

        TEST_SOCCC_COPY(NonBslmaAllocator<AC5AllocNonBslma>);
        TEST_SOCCC_DFLT(BslmaAllocator<AC5AllocNonBslma>);
        TEST_SOCCC_DFLT(FunkyAllocator<AC5AllocNonBslma>);

        TEST_SOCCC_COPY(NonBslmaAllocator<AC5AllocBslma>);
        TEST_SOCCC_DFLT(BslmaAllocator<AC5AllocBslma>);
        TEST_SOCCC_DFLT(FunkyAllocator<AC5AllocBslma>);

        TEST_SOCCC_COPY(NonBslmaAllocator<AC5AllocFunky>);
        TEST_SOCCC_DFLT(BslmaAllocator<AC5AllocFunky>);
        TEST_SOCCC_DFLT(FunkyAllocator<AC5AllocFunky>);

        TEST_SOCCC_COPY(NonBslmaAllocator<AttribClass5bslma>);
        TEST_SOCCC_DFLT(BslmaAllocator<AttribClass5bslma>);
        TEST_SOCCC_DFLT(FunkyAllocator<AttribClass5bslma>);

#undef TEST_SOCCC_COPY
#undef TEST_SOCCC_DFLT

      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING MAX_SIZE
        //
        // Concerns:
        //: 1 For any allocator 'a' of type 'ALLOC',
        //:   'allocator_traits<ALLOC>::max_size(a)' will return the same
        //:   value as 'a.max_size()'.
        //
        // Plan:
        //: o For a variety of allocators, test the result of calling
        //:   'max_size' through the 'allocator_traits' is the same as calling
        //:   'max_size' directly.
        //
        // Testing:
        //   size_type max_size(const ALLOC& a);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting max_size"
                            "\n================\n");

#define TEST_MAX_SIZE(ALLOC) {                                              \
            ALLOC a;                                                        \
            ASSERT(allocator_traits<ALLOC >::max_size(a) == a.max_size());  \
        }

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<FunkyAllocator<int> >    AC5AllocFunky;

        TEST_MAX_SIZE(NonBslmaAllocator<int>);
        TEST_MAX_SIZE(BslmaAllocator<int>);
        TEST_MAX_SIZE(FunkyAllocator<int>);

        TEST_MAX_SIZE(NonBslmaAllocator<AttribClass5>);
        TEST_MAX_SIZE(BslmaAllocator<AttribClass5>);
        TEST_MAX_SIZE(FunkyAllocator<AttribClass5>);

        TEST_MAX_SIZE(NonBslmaAllocator<AC5AllocNonBslma>);
        TEST_MAX_SIZE(BslmaAllocator<AC5AllocNonBslma>);
        TEST_MAX_SIZE(FunkyAllocator<AC5AllocNonBslma>);

        TEST_MAX_SIZE(NonBslmaAllocator<AC5AllocBslma>);
        TEST_MAX_SIZE(BslmaAllocator<AC5AllocBslma>);
        TEST_MAX_SIZE(FunkyAllocator<AC5AllocBslma>);

        TEST_MAX_SIZE(NonBslmaAllocator<AC5AllocFunky>);
        TEST_MAX_SIZE(BslmaAllocator<AC5AllocFunky>);
        TEST_MAX_SIZE(FunkyAllocator<AC5AllocFunky>);

        TEST_MAX_SIZE(NonBslmaAllocator<AttribClass5bslma>);
        TEST_MAX_SIZE(BslmaAllocator<AttribClass5bslma>);
        TEST_MAX_SIZE(FunkyAllocator<AttribClass5bslma>);

#undef TEST_MAX_SIZE

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCT AND DESTROY
        //
        // Concerns:
        //: 1 For an allocator 'a' of type 'ALLOC', calling
        //:   'allocator_traits<ALLOC>::construct(a, p, args)', where 'p' is a
        //:   pointer to an object of type 'T' and *args* represents 0 to 5
        //:   constructor arguments for 'T', the constructor for 'T' will be
        //:   invoked for the object at 'p' and will be passed *args*.
        //: 2 (Special case of the previous concern) calling
        //:   'allocator_traits<ALLOC>::construct(a, p, v)', where 'v' is an
        //:   expression of type 'T' will result in the copy constructor for
        //:   'T' being invoked to make a copy of 'v'.
        //: 3 If 'T' is a type which has the trait
        //:   'bslalg::TypeTraitUsesBslmaAllocator', and 'ALLOC' is convertible
        //:   from 'bslma::Allocator*', then 'a' is passed as an additional
        //:   constructor argument to 'allocator_traits<ALLOC>::construct'.
        //: 4 Calling 'allocator_traits<ALLOC>::destroy(a, p)' invokes the
        //:   destructor for 'T' for the object at 'p'.
        //: 5 The behavior of 'construct' and 'destroy' is unaffected by the
        //:   type of 'ALLOC::value_type' nor by the relationship between
        //:   'ALLOC::value_type' and 'T'.
        //
        // Plan:
        //: o Create a function template, 'testConstructDestruct' that can be
        //:   instantiated with an arbitrary allocator type 'ALLOC' and type
        //:   'T' and that addresses of the above concerns as follows. Give
        //:   'testConstructDestruct' a boolean parameter, 'scoped', that the
        //:   caller sets to 'true' if 'ALLOC' is convertible from
        //:   'bslma::Allocator*' and 'T' has the trait
        //:   'bslalg::TypeTraitUsesBslmaAllocator' (C1-C5)
        //: o Within 'testConstructDestruct', create an aligned buffer 'b' for
        //:   an object of type 'T'.  Create an allocator 'a' and call
        //:   'allocator_traits<ALLOC>::construct(a, &b, args)', where args is
        //:   0 to 5 arguments, including a single argument of type 'T'.
        //:   Verify that a constructor for 'T' was called and that the
        //:   resulting object has the expected attributes. (C1,C2)
        //: o If 'scoped' is true, verify that the call to 'construct'
        //:   resulted in the constructed object using allocator 'a'.
        //:   Otherwise, the constructed object uses the default allocator.
        //:   (C3)
        //: o Call 'allocator_traits<ALLOC>::construct(a, &b)' and verify that
        //:   the 'T' destructor for the object in 'b' has been invoked. (C4)
        //: o Instantiate and invoke 'testConstructDestroy' on each meaningful
        //:   combination of attribute classes ('AttribClass5',
        //:   'AttribClass5Alloc', and 'AttribClass5bslma') and allocator type
        //:   ('NonBslmaAllocator', 'BslmaAllocator', 'FunkyAllocator',
        //:   including combinations were the allocator's 'value_type' does
        //:   and does not match the attribute class.  When instantiating
        //:   'testConstructDestroy', ensure that 'scoped' is set
        //:   appropriately for the combination of the allocator and attribute
        //:   class. (C1-C5)
        //
        // Testing:
        //   void construct(ALLOC& a, T *p, Args&&... args);
        //   void destroy(ALLOC& a, T* p);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSTRUCT AND DESTROY"
                            "\n=============================\n");

#define TEST_CONSTRUCT_DESTROY(ALLOC, T, SCOPED)       \
        testConstructDestroy<ALLOC, T >(#ALLOC, #T, (SCOPED));

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<FunkyAllocator<int> >    AC5AllocFunky;

        TEST_CONSTRUCT_DESTROY(NonBslmaAllocator<AttribClass5>,
                               AttribClass5, false);
        TEST_CONSTRUCT_DESTROY(BslmaAllocator<AttribClass5>,
                               AttribClass5, false);
        TEST_CONSTRUCT_DESTROY(FunkyAllocator<AttribClass5>,
                               AttribClass5, false);

        TEST_CONSTRUCT_DESTROY(NonBslmaAllocator<AC5AllocNonBslma>,
                               AC5AllocNonBslma, false);
        TEST_CONSTRUCT_DESTROY(BslmaAllocator<AC5AllocNonBslma>,
                               AC5AllocNonBslma, false);
        TEST_CONSTRUCT_DESTROY(FunkyAllocator<AC5AllocNonBslma>,
                               AC5AllocNonBslma, false);

        TEST_CONSTRUCT_DESTROY(NonBslmaAllocator<AC5AllocBslma>,
                               AC5AllocBslma, false);
        TEST_CONSTRUCT_DESTROY(NonBslmaAllocator<int>,
                               AC5AllocBslma, false);
        TEST_CONSTRUCT_DESTROY(BslmaAllocator<AC5AllocBslma>,
                               AC5AllocBslma, true);
        TEST_CONSTRUCT_DESTROY(FunkyAllocator<AC5AllocBslma>,
                               AC5AllocBslma, true);

        TEST_CONSTRUCT_DESTROY(NonBslmaAllocator<AC5AllocFunky>,
                               AC5AllocFunky, false);
        TEST_CONSTRUCT_DESTROY(BslmaAllocator<AC5AllocFunky>,
                               AC5AllocFunky, true);
        TEST_CONSTRUCT_DESTROY(BslmaAllocator<int>,
                               AC5AllocFunky, true);
        TEST_CONSTRUCT_DESTROY(FunkyAllocator<AC5AllocFunky>,
                               AC5AllocFunky, true);

        TEST_CONSTRUCT_DESTROY(NonBslmaAllocator<AttribClass5bslma>,
                               AttribClass5bslma, false);
        TEST_CONSTRUCT_DESTROY(BslmaAllocator<AttribClass5bslma>,
                               AttribClass5bslma, true);
        TEST_CONSTRUCT_DESTROY(FunkyAllocator<AttribClass5bslma>,
                               AttribClass5bslma, true);
        TEST_CONSTRUCT_DESTROY(FunkyAllocator<int>,
                               AttribClass5bslma, true);

#undef TEST_CONSTRUCT_DESTROY

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATE AND DEALLOCATE
        //
        // Concerns:
        //: 1 For any allocator 'a' of type 'ALLOC', with value type 'T',
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
        //:   arbitrary address, and verify that the correct amount of memory
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
        //: o Instantiate and invoke 'testAllocateDeallocate' on each
        //:   meaningful combination of attribute classes ('AttribClass5',
        //:   'AttribClass5Alloc', and 'AttribClass5bslma') and allocator type
        //:   ('NonBslmaAllocator', 'BslmaAllocator',
        //:   'FunkyAllocator'). (C1-C4)
        //
        // Testing:
        //   pointer allocate(ALLOC& a, size_type n);
        //   pointer allocate(ALLOC& a, size_type n, const_void_pointer hint);
        //   void deallocate(ALLOC& a, pointer p, size_type n);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING ALLOCATE AND DEALLOCATE"
                            "\n===============================\n");

#define TEST_ALLOC_DEALLOC(ALLOC) \
        testAllocateDeallocate<ALLOC >(#ALLOC);

        typedef AttribClass5Alloc<NonBslmaAllocator<int> > AC5AllocNonBslma;
        typedef AttribClass5Alloc<BslmaAllocator<int> >    AC5AllocBslma;
        typedef AttribClass5Alloc<FunkyAllocator<int> >    AC5AllocFunky;

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
        //:   'bslmf::MetaInt<0>'.
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
        typedef AttribClass5Alloc<FunkyAllocator<int> >    AC5AllocFunky;

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
        //:    constructed from a pointer to a 'bslma::TestAllocator'.
        //: 4  The default constructor for 'NonBslmaAllocator'
        //:    uses the currently-installed test allocator.
        //: 5  A 'NonBslmaAllocator' object can be copy-constructed.
        //: 6  Calling 'allocate' and 'deallocate' on 'NonBslmaAllocator'
        //:    objects allocates and deallocates the expected number of bytes
        //:    from the underlying 'bslma::TestAllocator'.
        //: 7  Calling 'construct' and 'destroy' for an object allocated from
        //:    a 'NonBslmaAllocator' results in the object being initialized
        //:    and destroyed as expected.
        //: 8  Calling 'address' on reference to an object allocated from a
        //:    'NonBslmaAllocator' yields its address.
        //: 9  Calling 'max_size' on a 'NonBslmaAllocator' yields 'INT_MAX'.
        //: 10 A 'NonBslmaAllocator' can be compared for equality.  The result
        //:    is true if the object was constructed from the same
        //:    'bslma::TestAllocator' address.
        //: 11 Concerns 1 through 10 also apply to 'BslmaAllocator'.
        //: 12 Concerns 1 through 10 also apply to 'FunkyAllocator'.
        //: 13 A 'bslma::Allocator*' can be implicitly converted to a
        //:    'BslmaAllocator' object.
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
        //:    the allocator attribute is of type 'bslma::Allocator*' instead
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
        //: o Use 'bslmf::IsConvertible' to test that 'bslma::Allocator*' is
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
        //:   'BslmaAllocator', 'FunkyAllocator', 'bslma::Allocator*', and no
        //:   allocator). (C16-C18)
        //: o Within 'testAllocatorConformance', set 'g_lastHint' to a known
        //:   address before calling 'allocate' with no hint.  Verify that
        //:   'g_lastHint' gets set to null.  Repeat this test, but pass a
        //:   second known address as a hint, verifying that 'g_lastHint' gets
        //:   set to the second known address.
        //
        // Testing
        //   template <class T> class NonBslmaAllocator;
        //   template <class T> class BslmaAllocator;
        //   template <class T> class FunkyAllocator;
        //   class AttribClass5;
        //   class AttribClass5Alloc;
        //   class AttribClass5bslma;
        //   template <class T>
        //     bool matchAttrib(const T& v, char a, int b, double c,
        //                      const char *d, Uniq *e);
        //   template <class T, class ALLOC>
        //     bool matchAttrib(const T& v, char a, int b, double c,
        //                      const char *d, Uniq *e, const ALLOC& alloc);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST HARNESS"
                            "\n====================\n");

        testAllocatorConformance<NonBslmaAllocator>("NonBslmaAllocator");
        testAllocatorConformance<BslmaAllocator>("BslmaAllocator");
        testAllocatorConformance<FunkyAllocator>("FunkyAllocator");

        if (verbose)
            printf("Testing convertibility from 'bslma::Allocator*'\n");

        ASSERT(  (bslmf::IsConvertible<bslma::Allocator*,
                                       BslmaAllocator<int> >::VALUE));
        ASSERT(  (bslmf::IsConvertible<bslma::Allocator*,
                                       BslmaAllocator<AttribClass5> >::VALUE));
        ASSERT(! (bslmf::IsConvertible<bslma::Allocator*,
                                    NonBslmaAllocator<AttribClass5> >::VALUE));
        ASSERT(  (bslmf::IsConvertible<bslma::Allocator*,
                                       FunkyAllocator<int> >::VALUE));
        ASSERT(  (bslmf::IsConvertible<bslma::Allocator*,
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

        bslma::TestAllocator default_ta;
        bslma::TestAllocator ta;

        bslma::DefaultAllocatorGuard guard(&default_ta);

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
